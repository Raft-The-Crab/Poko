/**
 * @file query.h
 * @brief Physics query subsystem
 *
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 *
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_QUERY_QUERY_H
#define POKO_CORE_COMPONENTS_PHYSICS_QUERY_QUERY_H

#include "core/components/physics/core/handle.h"
#include "core/components/physics/bounds/aabb.h"
#include "core/components/physics/geometry/ray.h"
#include "core/components/physics/colliders/collider_definition.h"
#include "core/components/physics/bodies/body_definition.h"
#include "core/components/physics/math/vectors/vector3.h"
#include <vector>
#include <functional>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace query {

using core::ColliderHandle;
using core::BodyHandle;
using bounds::AABB;
using geometry::Ray;
using colliders::ColliderDefinition;
using bodies::BodyDefinition;
using math::Vector3;

/**
 * @brief Query result
 */
struct QueryResult {
    ColliderHandle collider;
    BodyHandle body;
    float distance;
    Vector3 point;
    Vector3 normal;

    QueryResult() noexcept
        : collider()
        , body()
        , distance(0.0f)
        , point(0.0f, 0.0f, 0.0f)
        , normal(0.0f, 1.0f, 0.0f) {}

    [[nodiscard]] bool isValid() const noexcept {
        return collider.isValid() && distance >= 0.0f;
    }
};

/**
 * @brief Raycast result
 */
struct RaycastResult : public QueryResult {
    bool hit;

    RaycastResult() noexcept : QueryResult(), hit(false) {}

    [[nodiscard]] bool isValid() const noexcept {
        return hit && QueryResult::isValid();
    }
};

/**
 * @brief Overlap result
 */
struct OverlapResult {
    ColliderHandle collider;
    BodyHandle body;

    OverlapResult() noexcept : collider(), body() {}

    [[nodiscard]] bool isValid() const noexcept {
        return collider.isValid();
    }
};

/**
 * @brief Query callback type
 */
using QueryCallback = std::function<bool(const QueryResult&)>;

/**
 * @brief Raycast callback type
 */
using RaycastCallback = std::function<bool(const RaycastResult&)>;

/**
 * @brief Overlap callback type
 */
using OverlapCallback = std::function<bool(const OverlapResult&)>;

/**
 * @brief Query filter
 */
struct QueryFilter {
    uint32_t collisionLayer;
    uint32_t collisionMask;
    bool ignoreTriggers;

    QueryFilter() noexcept
        : collisionLayer(0xFFFFFFFF)
        , collisionMask(0xFFFFFFFF)
        , ignoreTriggers(false) {}

    [[nodiscard]] bool shouldQuery(const ColliderDefinition& collider) const noexcept {
        if (ignoreTriggers && collider.isTrigger) {
            return false;
        }
        return (collisionLayer & collider.collisionMask) != 0 &&
               (collider.collisionLayer & collisionMask) != 0;
    }
};

/**
 * @brief Query subsystem
 */
class QuerySystem {
public:
    /**
     * @brief Constructor
     */
    QuerySystem() noexcept = default;

    /**
     * @brief Raycast in world
     */
    [[nodiscard]] bool raycast(
        const Ray& ray,
        RaycastResult& result,
        const QueryFilter& filter = QueryFilter()
    ) const noexcept;

    /**
     * @brief Raycast all (find all hits along ray)
     */
    [[nodiscard]] size_t raycastAll(
        const Ray& ray,
        std::vector<RaycastResult>& results,
        const QueryFilter& filter = QueryFilter()
    ) const noexcept;

    /**
     * @brief Raycast with callback
     */
    void raycast(
        const Ray& ray,
        RaycastCallback callback,
        const QueryFilter& filter = QueryFilter()
    ) const noexcept;

    /**
     * @brief Point query (check if point is inside any collider)
     */
    [[nodiscard]] bool pointQuery(
        const Vector3& point,
        OverlapResult& result,
        const QueryFilter& filter = QueryFilter()
    ) const noexcept;

    /**
     * @brief AABB overlap query
     */
    [[nodiscard]] size_t overlapAABB(
        const AABB& aabb,
        std::vector<OverlapResult>& results,
        const QueryFilter& filter = QueryFilter()
    ) const noexcept;

    /**
     * @brief AABB overlap with callback
     */
    void overlapAABB(
        const AABB& aabb,
        OverlapCallback callback,
        const QueryFilter& filter = QueryFilter()
    ) const noexcept;

    /**
     * @brief Sphere overlap query
     */
    [[nodiscard]] size_t overlapSphere(
        const Vector3& center,
        float radius,
        std::vector<OverlapResult>& results,
        const QueryFilter& filter = QueryFilter()
    ) const noexcept;

    /**
     * @brief Capsule overlap query
     */
    [[nodiscard]] size_t overlapCapsule(
        const Vector3& pointA,
        const Vector3& pointB,
        float radius,
        std::vector<OverlapResult>& results,
        const QueryFilter& filter = QueryFilter()
    ) const noexcept;

    /**
     * @brief Get collider at index
     */
    [[nodiscard]] const ColliderDefinition* getCollider(ColliderHandle handle) const noexcept {
        (void)handle;
        return nullptr; // Implemented by world
    }

    /**
     * @brief Get body at index
     */
    [[nodiscard]] const BodyDefinition* getBody(BodyHandle handle) const noexcept {
        (void)handle;
        return nullptr; // Implemented by world
    }

    /**
     * @brief Clear query cache
     */
    void clear() noexcept {
        // Clear any cached query results
    }
};

} // namespace query
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_QUERY_QUERY_H
