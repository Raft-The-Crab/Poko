/**
 * @file dynamic_aabb_tree.cpp
 * @brief Dynamic AABB tree implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/broadphase/dynamic_aabb_tree.h"
#include <algorithm>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace broadphase {

DynamicAABBTree::ProxyId DynamicAABBTree::addProxy(const AABB& aabb, uint64_t userData) {
    uint32_t node = allocateNode();
    m_nodes[node].aabb = aabb;
    m_nodes[node].userData = userData;
    m_nodes[node].isLeaf = true;
    
    insertLeaf(node);
    m_proxyCount++;
    
    return node;
}

void DynamicAABBTree::removeProxy(ProxyId proxyId) {
    if (proxyId == INVALID_PROXY || proxyId >= m_nodes.size()) {
        return;
    }
    
    if (!m_nodes[proxyId].isLeaf) {
        return;
    }
    
    removeLeaf(proxyId);
    freeNode(proxyId);
    m_proxyCount--;
}

void DynamicAABBTree::updateProxy(ProxyId proxyId, const AABB& aabb) {
    if (proxyId == INVALID_PROXY || proxyId >= m_nodes.size()) {
        return;
    }
    
    if (!m_nodes[proxyId].isLeaf) {
        return;
    }
    
    // Check if the proxy moved significantly
    AABB oldAABB = m_nodes[proxyId].aabb;
    oldAABB.expand(0.01f); // Small margin
    
    if (oldAABB.contains(aabb)) {
        // Still within the old AABB, no need to update
        m_nodes[proxyId].aabb = aabb;
        return;
    }
    
    // Remove and re-insert
    removeLeaf(proxyId);
    m_nodes[proxyId].aabb = aabb;
    insertLeaf(proxyId);
}

void DynamicAABBTree::getPairs(std::vector<Pair>& pairs) {
    pairs.clear();
    if (m_root == Node::NULL_NODE) {
        return;
    }
    
    collectPairs(m_root, pairs);
}

uint64_t DynamicAABBTree::getUserData(ProxyId proxyId) const {
    if (proxyId == INVALID_PROXY || proxyId >= m_nodes.size()) {
        return 0;
    }
    return m_nodes[proxyId].userData;
}

AABB DynamicAABBTree::getAABB(ProxyId proxyId) const {
    if (proxyId == INVALID_PROXY || proxyId >= m_nodes.size()) {
        return AABB();
    }
    return m_nodes[proxyId].aabb;
}

void DynamicAABBTree::clear() {
    m_nodes.clear();
    m_root = Node::NULL_NODE;
    m_freeList = Node::NULL_NODE;
    m_proxyCount = 0;
    m_nodeCount = 0;
    m_nodeCapacity = 16;
}

uint32_t DynamicAABBTree::allocateNode() {
    if (m_freeList != Node::NULL_NODE) {
        uint32_t node = m_freeList;
        m_freeList = m_nodes[node].parent;
        m_nodes[node].parent = Node::NULL_NODE;
        m_nodes[node].child1 = Node::NULL_NODE;
        m_nodes[node].child2 = Node::NULL_NODE;
        m_nodes[node].height = 0;
        m_nodes[node].isLeaf = false;
        return node;
    }
    
    if (m_nodeCount >= m_nodeCapacity) {
        m_nodeCapacity *= 2;
        m_nodes.resize(m_nodeCapacity);
    }
    
    uint32_t node = m_nodeCount++;
    m_nodes[node].parent = Node::NULL_NODE;
    m_nodes[node].child1 = Node::NULL_NODE;
    m_nodes[node].child2 = Node::NULL_NODE;
    m_nodes[node].height = 0;
    m_nodes[node].isLeaf = false;
    
    return node;
}

void DynamicAABBTree::freeNode(uint32_t node) {
    m_nodes[node].parent = m_freeList;
    m_freeList = node;
}

void DynamicAABBTree::insertLeaf(uint32_t leaf) {
    if (m_root == Node::NULL_NODE) {
        m_root = leaf;
        m_nodes[leaf].parent = Node::NULL_NODE;
        return;
    }
    
    // Find the best sibling for the new leaf
    AABB leafAABB = m_nodes[leaf].aabb;
    uint32_t sibling = m_root;
    
    while (!m_nodes[sibling].isLeaf) {
        uint32_t child1 = m_nodes[sibling].child1;
        uint32_t child2 = m_nodes[sibling].child2;
        
        AABB combined = AABB::unionOf(m_nodes[sibling].aabb, leafAABB);
        (void)combined.getSurfaceArea(); // Compute for future use
        
        AABB aabb1 = AABB::unionOf(m_nodes[child1].aabb, leafAABB);
        float cost1 = aabb1.getSurfaceArea() - m_nodes[child1].aabb.getSurfaceArea();
        
        AABB aabb2 = AABB::unionOf(m_nodes[child2].aabb, leafAABB);
        float cost2 = aabb2.getSurfaceArea() - m_nodes[child2].aabb.getSurfaceArea();
        
        if (cost1 < cost2) {
            sibling = child1;
        } else {
            sibling = child2;
        }
    }
    
    // Create a new parent
    uint32_t oldParent = m_nodes[sibling].parent;
    uint32_t newParent = allocateNode();
    m_nodes[newParent].parent = oldParent;
    m_nodes[newParent].aabb = AABB::unionOf(m_nodes[sibling].aabb, leafAABB);
    m_nodes[newParent].height = m_nodes[sibling].height + 1;
    m_nodes[newParent].isLeaf = false;
    
    if (oldParent != Node::NULL_NODE) {
        if (m_nodes[oldParent].child1 == sibling) {
            m_nodes[oldParent].child1 = newParent;
        } else {
            m_nodes[oldParent].child2 = newParent;
        }
        
        m_nodes[newParent].child1 = sibling;
        m_nodes[newParent].child2 = leaf;
        m_nodes[sibling].parent = newParent;
        m_nodes[leaf].parent = newParent;
    } else {
        m_nodes[newParent].child1 = sibling;
        m_nodes[newParent].child2 = leaf;
        m_nodes[sibling].parent = newParent;
        m_nodes[leaf].parent = newParent;
        m_root = newParent;
    }
    
    // Walk back up the tree refitting AABBs
    uint32_t index = m_nodes[leaf].parent;
    while (index != Node::NULL_NODE) {
        index = balance(index);
        
        uint32_t child1 = m_nodes[index].child1;
        uint32_t child2 = m_nodes[index].child2;
        
        m_nodes[index].aabb = AABB::unionOf(m_nodes[child1].aabb, m_nodes[child2].aabb);
        m_nodes[index].height = 1 + std::max(m_nodes[child1].height, m_nodes[child2].height);
        
        index = m_nodes[index].parent;
    }
}

void DynamicAABBTree::removeLeaf(uint32_t leaf) {
    if (leaf == m_root) {
        m_root = Node::NULL_NODE;
        return;
    }
    
    uint32_t parent = m_nodes[leaf].parent;
    uint32_t grandParent = m_nodes[parent].parent;
    uint32_t sibling = getSibling(leaf);
    
    if (grandParent != Node::NULL_NODE) {
        if (m_nodes[grandParent].child1 == parent) {
            m_nodes[grandParent].child1 = sibling;
        } else {
            m_nodes[grandParent].child2 = sibling;
        }
        
        m_nodes[sibling].parent = grandParent;
        freeNode(parent);
        
        uint32_t index = grandParent;
        while (index != Node::NULL_NODE) {
            index = balance(index);
            
            uint32_t child1 = m_nodes[index].child1;
            uint32_t child2 = m_nodes[index].child2;
            
            m_nodes[index].aabb = AABB::unionOf(m_nodes[child1].aabb, m_nodes[child2].aabb);
            m_nodes[index].height = 1 + std::max(m_nodes[child1].height, m_nodes[child2].height);
            
            index = m_nodes[index].parent;
        }
    } else {
        m_root = sibling;
        m_nodes[sibling].parent = Node::NULL_NODE;
        freeNode(parent);
    }
}

uint32_t DynamicAABBTree::balance(uint32_t node) {
    if (m_nodes[node].isLeaf || m_nodes[node].height < 2) {
        return node;
    }
    
    uint32_t child1 = m_nodes[node].child1;
    uint32_t child2 = m_nodes[node].child2;
    
    int32_t balance = m_nodes[child2].height - m_nodes[child1].height;
    
    if (balance > 1) {
        return rotateUp(node);
    }
    
    if (balance < -1) {
        return rotateDown(node);
    }
    
    return node;
}

uint32_t DynamicAABBTree::getSibling(uint32_t node) const noexcept {
    uint32_t parent = m_nodes[node].parent;
    if (parent == Node::NULL_NODE) {
        return Node::NULL_NODE;
    }
    
    if (m_nodes[parent].child1 == node) {
        return m_nodes[parent].child2;
    } else {
        return m_nodes[parent].child1;
    }
}

uint32_t DynamicAABBTree::rotateUp(uint32_t node) {
    uint32_t child1 = m_nodes[node].child1;
    uint32_t child2 = m_nodes[node].child2;
    uint32_t grandChild1 = m_nodes[child2].child1;
    uint32_t grandChild2 = m_nodes[child2].child2;
    
    m_nodes[child2].child1 = node;
    m_nodes[child2].parent = m_nodes[node].parent;
    m_nodes[node].parent = child2;
    
    if (m_nodes[child2].parent != Node::NULL_NODE) {
        if (m_nodes[m_nodes[child2].parent].child1 == node) {
            m_nodes[m_nodes[child2].parent].child1 = child2;
        } else {
            m_nodes[m_nodes[child2].parent].child2 = child2;
        }
    } else {
        m_root = child2;
    }
    
    m_nodes[node].child2 = grandChild1;
    m_nodes[grandChild1].parent = node;
    
    m_nodes[node].aabb = AABB::unionOf(m_nodes[child1].aabb, m_nodes[grandChild1].aabb);
    m_nodes[child2].aabb = AABB::unionOf(m_nodes[node].aabb, m_nodes[grandChild2].aabb);
    
    m_nodes[node].height = 1 + std::max(m_nodes[child1].height, m_nodes[grandChild1].height);
    m_nodes[child2].height = 1 + std::max(m_nodes[node].height, m_nodes[grandChild2].height);
    
    return child2;
}

uint32_t DynamicAABBTree::rotateDown(uint32_t node) {
    uint32_t child1 = m_nodes[node].child1;
    uint32_t child2 = m_nodes[node].child2;
    uint32_t grandChild1 = m_nodes[child1].child1;
    uint32_t grandChild2 = m_nodes[child1].child2;
    
    m_nodes[child1].child2 = node;
    m_nodes[child1].parent = m_nodes[node].parent;
    m_nodes[node].parent = child1;
    
    if (m_nodes[child1].parent != Node::NULL_NODE) {
        if (m_nodes[m_nodes[child1].parent].child1 == node) {
            m_nodes[m_nodes[child1].parent].child1 = child1;
        } else {
            m_nodes[m_nodes[child1].parent].child2 = child1;
        }
    } else {
        m_root = child1;
    }
    
    m_nodes[node].child1 = grandChild2;
    m_nodes[grandChild2].parent = node;
    
    m_nodes[node].aabb = AABB::unionOf(m_nodes[child2].aabb, m_nodes[grandChild2].aabb);
    m_nodes[child1].aabb = AABB::unionOf(m_nodes[node].aabb, m_nodes[grandChild1].aabb);
    
    m_nodes[node].height = 1 + std::max(m_nodes[child2].height, m_nodes[grandChild2].height);
    m_nodes[child1].height = 1 + std::max(m_nodes[node].height, m_nodes[grandChild1].height);
    
    return child1;
}

void DynamicAABBTree::query(uint32_t node, const AABB& aabb, std::vector<ProxyId>& results) const {
    if (node == Node::NULL_NODE) {
        return;
    }
    
    if (!m_nodes[node].aabb.intersects(aabb)) {
        return;
    }
    
    if (m_nodes[node].isLeaf) {
        results.push_back(node);
        return;
    }
    
    query(m_nodes[node].child1, aabb, results);
    query(m_nodes[node].child2, aabb, results);
}

void DynamicAABBTree::collectPairs(uint32_t node, std::vector<Pair>& pairs) {
    if (node == Node::NULL_NODE || m_nodes[node].isLeaf) {
        return;
    }
    
    // Collect pairs between children
    uint32_t child1 = m_nodes[node].child1;
    uint32_t child2 = m_nodes[node].child2;
    
    collectPairs(child1, pairs);
    collectPairs(child2, pairs);
    
    // Collect cross pairs
    std::vector<ProxyId> results;
    query(child1, m_nodes[child2].aabb, results);
    
    for (ProxyId proxy : results) {
        Pair pair;
        pair.proxyA = child2;
        pair.proxyB = proxy;
        pairs.push_back(pair);
    }
}

} // namespace broadphase
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
