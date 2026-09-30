/**
 * @file broadphase.h
 * @brief Broadphase collision detection interface
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_BROADPHASE_BROADPHASE_H
#define POKO_CORE_COMPONENTS_PHYSICS_BROADPHASE_BROADPHASE_H

#include "../math/bounds.h"
#include <cstdint>
#include <vector>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace broadphase {

using math::AABB;

/**
 * @brief Broadphase collision detection
 * 
 * Broadphase is the first stage of collision detection.
 * It quickly rejects pairs that cannot possibly collide using AABBs.
 * 
 * Stages:
 * 1. Broadphase: AABB overlap test (fast, O(n) to O(n log n))
 * 2. Narrowphase: Exact collision test (slow, O(m) where m << n)
 */
class Broadphase {
public:
    /**
     * @brief Broadphase proxy handle
     */
    using ProxyId = uint32_t;
    static constexpr ProxyId INVALID_PROXY = 0xFFFFFFFF;
    
    /**
     * @brief Collision pair
     */
    struct Pair {
        ProxyId proxyA;
        ProxyId proxyB;
        
        bool operator==(const Pair& other) const noexcept {
            return (proxyA == other.proxyA && proxyB == other.proxyB) ||
                   (proxyA == other.proxyB && proxyB == other.proxyA);
        }
    };
    
    /**
     * @brief Add a proxy to broadphase
     * @param aabb Axis-aligned bounding box
     * @param userData User data (e.g., body handle)
     * @return Proxy ID
     */
    [[nodiscard]] virtual ProxyId addProxy(const AABB& aabb, uint64_t userData) = 0;
    
    /**
     * @brief Remove a proxy from broadphase
     * @param proxyId Proxy ID to remove
     */
    virtual void removeProxy(ProxyId proxyId) = 0;
    
    /**
     * @brief Update proxy AABB
     * @param proxyId Proxy ID to update
     * @param aabb New AABB
     */
    virtual void updateProxy(ProxyId proxyId, const AABB& aabb) = 0;
    
    /**
     * @brief Get all potentially colliding pairs
     * @param pairs Output vector of collision pairs
     */
    virtual void getPairs(std::vector<Pair>& pairs) = 0;
    
    /**
     * @brief Get user data for proxy
     */
    [[nodiscard]] virtual uint64_t getUserData(ProxyId proxyId) const = 0;
    
    /**
     * @brief Get AABB for proxy
     */
    [[nodiscard]] virtual AABB getAABB(ProxyId proxyId) const = 0;
    
    /**
     * @brief Clear all proxies
     */
    virtual void clear() = 0;
    
    /**
     * @brief Get proxy count
     */
    [[nodiscard]] virtual uint32_t getProxyCount() const = 0;
    
    virtual ~Broadphase() = default;
};

} // namespace broadphase
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_BROADPHASE_BROADPHASE_H
