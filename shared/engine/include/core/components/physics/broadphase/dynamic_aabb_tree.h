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

#include "broadphase.h"
#include "../math/bounds.h"
#include <cstdint>
#include <vector>
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace broadphase {

using math::AABB;

/**
 * @brief Dynamic AABB tree (BVH) broadphase
 * 
 * Fast, self-balancing bounding volume hierarchy.
 * Good for dynamic scenes with moving objects.
 * 
 * Complexity:
 * - Insert: O(log n)
 * - Remove: O(log n)
 * - Update: O(log n)
 * - Query: O(log n + m) where m is number of results
 */
class DynamicAABBTree : public Broadphase {
public:
    static constexpr uint32_t MAX_PROXIES = 65536;
    static constexpr uint32_t STACK_CAPACITY = 256;
    
    /**
     * @brief Constructor
     */
    DynamicAABBTree() = default;
    
    /**
     * @brief Add a proxy to tree
     */
    [[nodiscard]] ProxyId addProxy(const AABB& aabb, uint64_t userData) override;
    
    /**
     * @brief Remove a proxy from tree
     */
    void removeProxy(ProxyId proxyId) override;
    
    /**
     * @brief Update proxy AABB
     */
    void updateProxy(ProxyId proxyId, const AABB& aabb) override;
    
    /**
     * @brief Get all potentially colliding pairs
     */
    void getPairs(std::vector<Pair>& pairs) override;
    
    /**
     * @brief Get user data for proxy
     */
    [[nodiscard]] uint64_t getUserData(ProxyId proxyId) const override;
    
    /**
     * @brief Get AABB for proxy
     */
    [[nodiscard]] AABB getAABB(ProxyId proxyId) const override;
    
    /**
     * @brief Clear all proxies
     */
    void clear() override;
    
    /**
     * @brief Get proxy count
     */
    [[nodiscard]] uint32_t getProxyCount() const override { return m_proxyCount; }
    
private:
    /**
     * @brief Tree node
     */
    struct Node {
        static constexpr uint32_t NULL_NODE = 0xFFFFFFFF;
        
        AABB aabb;
        uint64_t userData;
        uint32_t parent;
        uint32_t child1;
        uint32_t child2;
        int32_t height;
        bool isLeaf;
        
        Node() noexcept
            : userData(0)
            , parent(NULL_NODE)
            , child1(NULL_NODE)
            , child2(NULL_NODE)
            , height(0)
            , isLeaf(false) {}
    };
    
    /**
     * @brief Allocate a node
     */
    [[nodiscard]] uint32_t allocateNode();
    
    /**
     * @brief Free a node
     */
    void freeNode(uint32_t node);
    
    /**
     * @brief Insert leaf into tree
     */
    void insertLeaf(uint32_t leaf);
    
    /**
     * @brief Remove leaf from tree
     */
    void removeLeaf(uint32_t leaf);
    
    /**
     * @brief Balance tree
     */
    uint32_t balance(uint32_t node);
    
    /**
     * @brief Get sibling
     */
    [[nodiscard]] uint32_t getSibling(uint32_t node) const noexcept;
    
    /**
     * @brief Rotate up
     */
    uint32_t rotateUp(uint32_t node);
    
    /**
     * @brief Rotate down
     */
    uint32_t rotateDown(uint32_t node);
    
    /**
     * @brief Query tree for overlaps
     */
    void query(uint32_t node, const AABB& aabb, std::vector<ProxyId>& results) const;
    
    /**
     * @brief Collect all pairs
     */
    void collectPairs(uint32_t node, std::vector<Pair>& pairs);
    
    // Tree nodes
    std::vector<Node> m_nodes;
    uint32_t m_root = Node::NULL_NODE;
    uint32_t m_freeList = Node::NULL_NODE;
    uint32_t m_proxyCount = 0;
    uint32_t m_nodeCount = 0;
    uint32_t m_nodeCapacity = 16;
};

} // namespace broadphase
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_BROADPHASE_DYNAMIC_AABB_TREE_H
