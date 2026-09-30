/**
 * @file dynamic_aabb_tree.cpp
 * @brief Dynamic AABB tree broadphase implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "../../include/core/components/physics/broadphase/dynamic_aabb_tree.h"
#include "../../include/core/components/physics/bounds/aabb.h"
#include <algorithm>
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace broadphase {

DynamicAABBTree::DynamicAABBTree() noexcept
    : root(-1)
    , freeList(-1)
    , proxyCount(0) {
    nodes.reserve(1024);
}

BroadphaseProxyHandle DynamicAABBTree::insert(const AABB& aabb, ColliderHandle collider) {
    int32_t leaf = allocateNode();
    nodes[leaf].aabb = aabb;
    nodes[leaf].collider = collider;
    nodes[leaf].parent = -1;
    nodes[leaf].child1 = -1;
    nodes[leaf].child2 = -1;
    nodes[leaf].height = 0;
    nodes[leaf].isLeaf = true;
    
    insertLeaf(leaf);
    proxyCount++;
    
    return BroadphaseProxyHandle(static_cast<uint32_t>(leaf), 0);
}

void DynamicAABBTree::remove(BroadphaseProxyHandle proxy) {
    int32_t leaf = static_cast<int32_t>(proxy.index);
    removeLeaf(leaf);
    freeNode(leaf);
    proxyCount--;
}

void DynamicAABBTree::update(BroadphaseProxyHandle proxy, const AABB& newAABB) {
    int32_t leaf = static_cast<int32_t>(proxy.index);
    
    // Check if the new AABB is still within the fat AABB
    if (nodes[leaf].aabb.containsMin(newAABB.min) && nodes[leaf].aabb.containsMax(newAABB.max)) {
        return;
    }
    
    removeLeaf(leaf);
    nodes[leaf].aabb = newAABB;
    insertLeaf(leaf);
}

void DynamicAABBTree::generatePairs(std::vector<CollisionPair>& outPairs) {
    outPairs.clear();
    if (root == -1) return;
    
    collectPairs(root, outPairs);
}

void DynamicAABBTree::queryAABB(const AABB& aabb, std::function<void(ColliderHandle)> callback) {
    if (root == -1) return;
    queryNode(root, aabb, callback);
}

void DynamicAABBTree::getAllColliders(std::vector<ColliderHandle>& outColliders) {
    outColliders.clear();
    outColliders.reserve(proxyCount);
    
    for (const auto& node : nodes) {
        if (node.isLeaf) {
            outColliders.push_back(node.collider);
        }
    }
}

void DynamicAABBTree::clear() {
    nodes.clear();
    root = -1;
    freeList = -1;
    proxyCount = 0;
}

size_t DynamicAABBTree::getProxyCount() const {
    return proxyCount;
}

int32_t DynamicAABBTree::allocateNode() {
    if (freeList != -1) {
        int32_t node = freeList;
        freeList = nodes[node].parent;
        nodes[node].parent = -1;
        nodes[node].child1 = -1;
        nodes[node].child2 = -1;
        nodes[node].height = 0;
        nodes[node].isLeaf = false;
        return node;
    }
    
    nodes.emplace_back();
    return static_cast<int32_t>(nodes.size()) - 1;
}

void DynamicAABBTree::freeNode(int32_t node) {
    nodes[node].parent = freeList;
    freeList = node;
}

void DynamicAABBTree::insertLeaf(int32_t leaf) {
    if (root == -1) {
        root = leaf;
        return;
    }
    
    // Find the best sibling for the new leaf
    AABB leafAABB = nodes[leaf].aabb;
    int32_t sibling = root;
    
    while (!nodes[sibling].isLeaf) {
        int32_t child1 = nodes[sibling].child1;
        int32_t child2 = nodes[sibling].child2;
        
        AABB combined = nodes[child1].aabb;
        combined.expandToInclude(nodes[child2].aabb);
        
        float cost1 = combined.area() - nodes[child1].aabb.area() - nodes[child2].aabb.area();
        
        AABB parentAABB = nodes[sibling].aabb;
        parentAABB.expandToInclude(leafAABB);
        float cost2 = parentAABB.area() - nodes[sibling].aabb.area();
        
        if (cost2 < cost1) {
            break;
        }
        
        sibling = nodes[child1].aabb.area() < nodes[child2].aabb.area() ? child1 : child2;
    }
    
    // Create a new parent
    int32_t oldParent = nodes[sibling].parent;
    int32_t newParent = allocateNode();
    nodes[newParent].parent = oldParent;
    nodes[newParent].aabb = nodes[sibling].aabb;
    nodes[newParent].aabb.expandToInclude(leafAABB);
    nodes[newParent].height = nodes[sibling].height + 1;
    nodes[newParent].isLeaf = false;
    nodes[newParent].child1 = sibling;
    nodes[newParent].child2 = leaf;
    
    if (oldParent != -1) {
        if (nodes[oldParent].child1 == sibling) {
            nodes[oldParent].child1 = newParent;
        } else {
            nodes[oldParent].child2 = newParent;
        }
    } else {
        root = newParent;
    }
    
    nodes[sibling].parent = newParent;
    nodes[leaf].parent = newParent;
    
    // Walk back up the tree refitting AABBs
    int32_t index = nodes[leaf].parent;
    while (index != -1) {
        index = balance(index);
        
        int32_t child1 = nodes[index].child1;
        int32_t child2 = nodes[index].child2;
        
        nodes[index].aabb = nodes[child1].aabb;
        nodes[index].aabb.expandToInclude(nodes[child2].aabb);
        nodes[index].height = 1 + std::max(getHeight(child1), getHeight(child2));
        
        index = nodes[index].parent;
    }
}

void DynamicAABBTree::removeLeaf(int32_t leaf) {
    if (leaf == root) {
        root = -1;
        return;
    }
    
    int32_t parent = nodes[leaf].parent;
    int32_t grandParent = nodes[parent].parent;
    int32_t sibling;
    
    if (nodes[parent].child1 == leaf) {
        sibling = nodes[parent].child2;
    } else {
        sibling = nodes[parent].child1;
    }
    
    if (grandParent != -1) {
        if (nodes[grandParent].child1 == parent) {
            nodes[grandParent].child1 = sibling;
        } else {
            nodes[grandParent].child2 = sibling;
        }
        nodes[sibling].parent = grandParent;
        
        int32_t index = grandParent;
        while (index != -1) {
            index = balance(index);
            
            int32_t child1 = nodes[index].child1;
            int32_t child2 = nodes[index].child2;
            
            nodes[index].aabb = nodes[child1].aabb;
            nodes[index].aabb.expandToInclude(nodes[child2].aabb);
            nodes[index].height = 1 + std::max(getHeight(child1), getHeight(child2));
            
            index = nodes[index].parent;
        }
    } else {
        root = sibling;
        nodes[sibling].parent = -1;
    }
    
    freeNode(parent);
}

int32_t DynamicAABBTree::balance(int32_t node) {
    if (nodes[node].isLeaf || nodes[node].height < 2) {
        return node;
    }
    
    int32_t child1 = nodes[node].child1;
    int32_t child2 = nodes[node].child2;
    
    int32_t balance = getHeight(child2) - getHeight(child1);
    
    if (balance > 1) {
        return rotate(node, child2);
    }
    
    if (balance < -1) {
        return rotate(node, child1);
    }
    
    return node;
}

int32_t DynamicAABBTree::rotate(int32_t node, int32_t child) {
    int32_t grandParent = nodes[node].parent;
    int32_t sibling1 = nodes[child].child1;
    int32_t sibling2 = nodes[child].child2;
    
    if (nodes[node].child1 == child) {
        nodes[child].child1 = node;
        nodes[node].child2 = sibling1;
    } else {
        nodes[child].child2 = node;
        nodes[node].child1 = sibling2;
    }
    
    nodes[child].parent = grandParent;
    nodes[node].parent = child;
    
    if (sibling1 != -1) nodes[sibling1].parent = node;
    if (sibling2 != -1) nodes[sibling2].parent = node;
    
    if (grandParent != -1) {
        if (nodes[grandParent].child1 == node) {
            nodes[grandParent].child1 = child;
        } else {
            nodes[grandParent].child2 = child;
        }
    } else {
        root = child;
    }
    
    nodes[node].aabb = calculateCombinedAABB(node);
    nodes[child].aabb = calculateCombinedAABB(child);
    
    nodes[node].height = 1 + std::max(getHeight(nodes[node].child1), getHeight(nodes[node].child2));
    nodes[child].height = 1 + std::max(getHeight(nodes[child].child1), getHeight(nodes[child].child2));
    
    return child;
}

int32_t DynamicAABBTree::getHeight(int32_t node) const {
    if (node == -1) return 0;
    return nodes[node].height;
}

AABB DynamicAABBTree::calculateCombinedAABB(int32_t node) const {
    AABB combined = nodes[nodes[node].child1].aabb;
    combined.expandToInclude(nodes[nodes[node].child2].aabb);
    return combined;
}

void DynamicAABBTree::queryNode(int32_t node, const AABB& aabb, std::function<void(ColliderHandle)> callback) {
    if (!nodes[node].aabb.intersects(aabb)) {
        return;
    }
    
    if (nodes[node].isLeaf) {
        callback(nodes[node].collider);
    } else {
        queryNode(nodes[node].child1, aabb, callback);
        queryNode(nodes[node].child2, aabb, callback);
    }
}

void DynamicAABBTree::collectPairs(int32_t node, std::vector<CollisionPair>& outPairs) {
    if (nodes[node].isLeaf) {
        return;
    }
    
    int32_t child1 = nodes[node].child1;
    int32_t child2 = nodes[node].child2;
    
    if (nodes[child1].aabb.intersects(nodes[child2].aabb)) {
        collectPairs(child1, child2, outPairs);
    }
    
    collectPairs(child1, outPairs);
    collectPairs(child2, outPairs);
}

void DynamicAABBTree::collectPairs(int32_t nodeA, int32_t nodeB, std::vector<CollisionPair>& outPairs) {
    if (nodes[nodeA].isLeaf && nodes[nodeB].isLeaf) {
        outPairs.emplace_back(nodes[nodeA].collider, nodes[nodeB].collider);
        return;
    }
    
    if (nodes[nodeA].isLeaf || (!nodes[nodeB].isLeaf && nodes[nodeB].height > nodes[nodeA].height)) {
        int32_t child1 = nodes[nodeB].child1;
        int32_t child2 = nodes[nodeB].child2;
        
        if (nodes[nodeA].aabb.intersects(nodes[child1].aabb)) {
            collectPairs(nodeA, child1, outPairs);
        }
        if (nodes[nodeA].aabb.intersects(nodes[child2].aabb)) {
            collectPairs(nodeA, child2, outPairs);
        }
    } else {
        int32_t child1 = nodes[nodeA].child1;
        int32_t child2 = nodes[nodeA].child2;
        
        if (nodes[nodeB].aabb.intersects(nodes[child1].aabb)) {
            collectPairs(child1, nodeB, outPairs);
        }
        if (nodes[nodeB].aabb.intersects(nodes[child2].aabb)) {
            collectPairs(child2, nodeB, outPairs);
        }
    }
}

} // namespace broadphase
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
