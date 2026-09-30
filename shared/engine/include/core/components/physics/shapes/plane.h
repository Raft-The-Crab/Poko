/**
 * @file plane.h
 * @brief Plane collision shape
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_SHAPES_PLANE_H
#define POKO_CORE_COMPONENTS_PHYSICS_SHAPES_PLANE_H

#include "../math/vector3.h"
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace shapes {

/**
 * @brief Plane collision shape
 * 
 * Defined by normal and distance from origin.
 * Used for:
 * - Ground planes
 * - Static geometry faces
 * - Collision detection
 */
class Plane {
public:
    Vector3 normal;
    float distance;
    
    /**
     * @brief Default constructor (XZ plane at origin)
     */
    constexpr Plane() noexcept : normal(0.0f, 1.0f, 0.0f), distance(0.0f) {}
    
    /**
     * @brief Component constructor
     */
    constexpr Plane(const Vector3& normal_, float distance_) noexcept
        : normal(normal_), distance(distance_) {}
    
    /**
     * @brief Point-normal constructor
     */
    static Plane fromPointNormal(const Vector3& point, const Vector3& normal_) noexcept {
        Vector3 n = normal_.normalized();
        float d = -n.dot(point);
        return Plane(n, d);
    }
    
    /**
     * @brief Three-point constructor
     */
    static Plane fromThreePoints(
        const Vector3& p1,
        const Vector3& p2,
        const Vector3& p3
    ) noexcept {
        Vector3 v1 = p2 - p1;
        Vector3 v2 = p3 - p1;
        Vector3 n = v1.cross(v2).normalized();
        return fromPointNormal(p1, n);
    }
    
    /**
     * @brief Distance from point to plane
     */
    [[nodiscard]] float distanceToPoint(const Vector3& point) const noexcept {
        return normal.dot(point) + distance;
    }
    
    /**
     * @brief Project point onto plane
     */
    [[nodiscard]] Vector3 projectPoint(const Vector3& point) const noexcept {
        float d = distanceToPoint(point);
        return point - normal * d;
    }
    
    /**
     * @brief Check if point is on plane (within epsilon)
     */
    [[nodiscard]] bool contains(const Vector3& point, float epsilon = 0.0001f) const noexcept {
        return std::abs(distanceToPoint(point)) < epsilon;
    }
    
    /**
     * @brief Get which side of the plane a point is on
     * @return Positive if in front, negative if behind, zero if on plane
     */
    [[nodiscard]] float side(const Vector3& point) const noexcept {
        return distanceToPoint(point);
    }
};

} // namespace shapes
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_SHAPES_PLANE_H
