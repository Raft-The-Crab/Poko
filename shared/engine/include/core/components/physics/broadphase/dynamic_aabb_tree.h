/**
 * @file dynamic_aabb_tree.h
 * @brief Dynamic AABB tree broadphase implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_BROADPHASE_DYNAMIC_AABB_TREE_H
#define POKO_CORE_COMPONENTS_PHYSICS_BROADPHASE_DYNAMIC_AABB_TREE_H

#include "core/components/physics/broadphase/ibroadphase.h"
#include "core/components/physics/bounds/aabb.h"
#include "core/components/physics/math/vectors/vector3.h"
#include <vector>
#include <memory>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace broadphase {

using math::Vector3;

/**
 * @brief Dynamic AABB tree node
 */
struct DynamicAABBTreeNode {
    AABB aabb;              // Fat AABB for broadphase
    AABB originalAABB;      // Original AABB for exact collision
    ColliderHandle collider;
    int32_t parent;
    int32_t child1;
    int32_t child2;
    int32_t height;
    bool isLeaf;

    DynamicAABBTreeNode() noexcept
        : aabb()
        , originalAABB()
        , collider()
        , parent(-1)
        , child1(-1)
        , child2(-1)
        , height(0)
        , isLeaf(false) {}
};

/**
 * @brief Dynamic AABB tree broadphase
 */
class DynamicAABBTree : public IBroadphase {
public:
    /**
     * @brief Constructor
     */
    DynamicAABBTree() noexcept;
    
    /**
     * @brief Destructor
     */
    ~DynamicAABBTree() override = default;
    
    /**
     * @brief Insert proxy
     */
    BroadphaseProxyHandle insert(const AABB& aabb, ColliderHandle collider) override;
    
    /**
     * @brief Remove proxy
     */
    void remove(BroadphaseProxyHandle proxy) override;
    
    /**
     * @brief Update proxy
     */
    void update(BroadphaseProxyHandle proxy, const AABB& newAABB) override;
    
    /**
     * @brief Generate collision pairs
     */
    void generatePairs(std::vector<CollisionPair>& outPairs) override;
    
    /**
     * @brief Query AABB
     */
    void queryAABB(const AABB& aabb, std::function<void(ColliderHandle)> callback) override;
    
    /**
     * @brief Get all colliders
     */
    void getAllColliders(std::vector<ColliderHandle>& outColliders) override;
    
    /**
     * @brief Clear
     */
    void clear() override;
    
    /**
     * @brief Get proxy count
     */
    [[nodiscard]] size_t getProxyCount() const override;
    
private:
    std::vector<DynamicAABBTreeNode> nodes;
    int32_t root;
    int32_t freeList;
    uint32_t proxyCount;
    float fatAABBMargin;
    
    /**
     * @brief Allocate node
     */
    int32_t allocateNode();
    
    /**
     * @brief Free node
     */
    void freeNode(int32_t node);
    
    /**
     * @brief Insert leaf
     */
    void insertLeaf(int32_t leaf);
    
    /**
     * @brief Remove leaf
     */
    void removeLeaf(int32_t leaf);
    
    /**
     * @brief Balance node
     */
    int32_t balance(int32_t node);
    
    /**
     * @brief Rotate node with specific child
     */
    int32_t rotate(int32_t node, int32_t child);
    
    /**
     * @brief Get height
     */
    [[nodiscard]] int32_t getHeight(int32_t node) const;
    
    /**
     * @brief Calculate combined AABB
     */
    [[nodiscard]] AABB calculateCombinedAABB(int32_t node) const;
    
    /**
     * @brief Query node
     */
    void queryNode(int32_t node, const AABB& aabb, std::function<void(ColliderHandle)> callback);
    
    /**
     * @brief Collect pairs from node
     */
    void collectPairs(int32_t node, std::vector<CollisionPair>& outPairs);

    /**
     * @brief Collect pairs between two nodes
     */
    void collectPairs(int32_t nodeA, int32_t nodeB, std::vector<CollisionPair>& outPairs);
};

} // namespace broadphase
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_BROADPHASE_DYNAMIC_AABB_TREE_H
