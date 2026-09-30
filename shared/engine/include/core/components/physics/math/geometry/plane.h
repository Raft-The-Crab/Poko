/**
 * @file plane.h
 * @brief Plane for physics calculations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_PLANE_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_PLANE_H

#include "vector3.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

/**
 * @brief Plane defined by normal and distance from origin
 */
class Plane {
public:
    Vector3 normal;
    float distance;
    
    /**
     * @brief Default constructor (XY plane at origin)
     */
    constexpr Plane() noexcept
        : normal(0.0f, 1.0f, 0.0f)
        , distance(0.0f) {}
    
    /**
     * @brief Point-normal constructor
     */
    constexpr Plane(const Vector3& normal_, float distance_) noexcept
        : normal(normal_)
        , distance(distance_) {}
    
    /**
     * @brief Three-point constructor
     */
    static Plane fromPoints(const Vector3& p0, const Vector3& p1, const Vector3& p2) noexcept {
        Vector3 edge1 = p1 - p0;
        Vector3 edge2 = p2 - p0;
        Vector3 n = edge1.cross(edge2).normalized();
        float d = -n.dot(p0);
        return Plane{n, d};
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
     * @brief Check which side point is on
     * @return >0 for front, <0 for back, 0 for on plane
     */
    [[nodiscard]] float side(const Vector3& point) const noexcept {
        return distanceToPoint(point);
    }
    
    /**
     * @brief Check if point is on plane (within tolerance)
     */
    [[nodiscard]] bool contains(const Vector3& point, float epsilon = 0.001f) const noexcept {
        return std::abs(distanceToPoint(point)) < epsilon;
    }
};

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_PLANE_H
