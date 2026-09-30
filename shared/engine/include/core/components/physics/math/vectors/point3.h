/**
 * @file point3.h
 * @brief 3D point for physics calculations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_POINT3_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_POINT3_H

#include "vector3.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

/**
 * @brief 3D point (semantic distinction from vector)
 * 
 * Points represent positions in space.
 * Vectors represent directions and displacements.
 * This distinction is important for transforms and physics calculations.
 */
class Point3 {
public:
    float x;
    float y;
    float z;
    
    /**
     * @brief Default constructor (origin)
     */
    constexpr Point3() noexcept : x(0.0f), y(0.0f), z(0.0f) {}
    
    /**
     * @brief Component constructor
     */
    constexpr Point3(float x_, float y_, float z_) noexcept : x(x_), y(y_), z(z_) {}
    
    /**
     * @brief From Vector3
     */
    explicit constexpr Point3(const Vector3& v) noexcept : x(v.x), y(v.y), z(v.z) {}
    
    /**
     * @brief To Vector3
     */
    [[nodiscard]] constexpr Vector3 toVector3() const noexcept {
        return Vector3{x, y, z};
    }
    
    /**
     * @brief Point addition (affine combination)
     */
    [[nodiscard]] constexpr Point3 operator+(const Vector3& v) const noexcept {
        return Point3{x + v.x, y + v.y, z + v.z};
    }
    
    /**
     * @brief Point subtraction (returns vector)
     */
    [[nodiscard]] constexpr Vector3 operator-(const Point3& other) const noexcept {
        return Vector3{x - other.x, y - other.y, z - other.z};
    }
    
    /**
     * @brief Point subtraction by vector
     */
    [[nodiscard]] constexpr Point3 operator-(const Vector3& v) const noexcept {
        return Point3{x - v.x, y - v.y, z - v.z};
    }
    
    /**
     * @brief Distance to another point
     */
    [[nodiscard]] float distanceTo(const Point3& other) const noexcept {
        return (*this - other).length();
    }
    
    /**
     * @brief Distance squared to another point
     */
    [[nodiscard]] constexpr float distanceSquaredTo(const Point3& other) const noexcept {
        return (*this - other).lengthSquared();
    }
};

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_POINT3_H
