/**
 * @file ibroadphase.h
 * @brief Broadphase interface
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_BROADPHASE_IBROADPHASE_H
#define POKO_CORE_COMPONENTS_PHYSICS_BROADPHASE_IBROADPHASE_H

#include "core/components/physics/bounds/aabb.h"
#include "core/components/physics/core/handle.h"
#include <vector>
#include <functional>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace broadphase {

using bounds::AABB;
using core::BroadphaseProxyHandle;
using core::ColliderHandle;

/**
 * @brief Collision pair
 */
struct CollisionPair {
    ColliderHandle colliderA;
    ColliderHandle colliderB;

    constexpr CollisionPair() noexcept : colliderA(), colliderB() {}
    constexpr CollisionPair(ColliderHandle a, ColliderHandle b) noexcept : colliderA(a), colliderB(b) {}

    [[nodiscard]] bool operator==(const CollisionPair& other) const noexcept {
        return (colliderA == other.colliderA && colliderB == other.colliderB) ||
               (colliderA == other.colliderB && colliderB == other.colliderA);
    }

    [[nodiscard]] bool operator!=(const CollisionPair& other) const noexcept {
        return !(*this == other);
    }

    [[nodiscard]] bool isValid() const noexcept {
        return colliderA.isValid() && colliderB.isValid();
    }
};

/**
 * @brief Broadphase interface
 */
class IBroadphase {
public:
    virtual ~IBroadphase() = default;
    
    /**
     * @brief Insert proxy
     */
    virtual BroadphaseProxyHandle insert(const AABB& aabb, ColliderHandle collider) = 0;
    
    /**
     * @brief Remove proxy
     */
    virtual void remove(BroadphaseProxyHandle proxy) = 0;
    
    /**
     * @brief Update proxy
     */
    virtual void update(BroadphaseProxyHandle proxy, const AABB& newAABB) = 0;
    
    /**
     * @brief Generate collision pairs
     */
    virtual void generatePairs(std::vector<CollisionPair>& outPairs) = 0;
    
    /**
     * @brief Query AABB
     */
    virtual void queryAABB(const AABB& aabb, std::function<void(ColliderHandle)> callback) = 0;
    
    /**
     * @brief Get all colliders
     */
    virtual void getAllColliders(std::vector<ColliderHandle>& outColliders) = 0;
    
    /**
     * @brief Clear
     */
    virtual void clear() = 0;
    
    /**
     * @brief Get proxy count
     */
    [[nodiscard]] virtual size_t getProxyCount() const = 0;
};

} // namespace broadphase
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_BROADPHASE_IBROADPHASE_H
