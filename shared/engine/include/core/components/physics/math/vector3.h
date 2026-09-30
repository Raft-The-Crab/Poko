/**
 * @file vector3.h
 * @brief Physics-specific Vector3 for physics calculations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTOR3_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTOR3_H

#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

/**
 * @brief Physics-specific 3D vector
 * 
 * This is optimized for physics calculations with
 * physics-specific operations that may differ from
 * general rendering math.
 */
struct Vector3 {
    float x, y, z;
    
    /**
     * @brief Default constructor (zero vector)
     */
    constexpr Vector3() noexcept : x(0.0f), y(0.0f), z(0.0f) {}
    
    /**
     * @brief Component constructor
     */
    constexpr Vector3(float x_, float y_, float z_) noexcept : x(x_), y(y_), z(z_) {}
    
    /**
     * @brief Add vector
     */
    [[nodiscard]] constexpr Vector3 operator+(const Vector3& other) const noexcept {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    
    /**
     * @brief Subtract vector
     */
    [[nodiscard]] constexpr Vector3 operator-(const Vector3& other) const noexcept {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    
    /**
     * @brief Scalar multiplication
     */
    [[nodiscard]] constexpr Vector3 operator*(float scalar) const noexcept {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    
    /**
     * @brief Scalar division
     */
    [[nodiscard]] constexpr Vector3 operator/(float scalar) const noexcept {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }
    
    /**
     * @brief Dot product
     */
    [[nodiscard]] constexpr float dot(const Vector3& other) const noexcept {
        return x * other.x + y * other.y + z * other.z;
    }
    
    /**
     * @brief Cross product
     */
    [[nodiscard]] constexpr Vector3 cross(const Vector3& other) const noexcept {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    
    /**
     * @brief Length squared
     */
    [[nodiscard]] constexpr float lengthSquared() const noexcept {
        return x * x + y * y + z * z;
    }
    
    /**
     * @brief Length
     */
    [[nodiscard]] float length() const noexcept {
        return std::sqrt(lengthSquared());
    }
    
    /**
     * @brief Normalize
     */
    [[nodiscard]] Vector3 normalized() const noexcept {
        float len = length();
        if (len > 0.0001f) {
            return *this / len;
        }
        return Vector3();
    }
    
    /**
     * @brief Distance to another vector
     */
    [[nodiscard]] float distanceTo(const Vector3& other) const noexcept {
        return (*this - other).length();
    }
    
    /**
     * @brief Zero vector constant
     */
    static constexpr Vector3 zero() noexcept { return Vector3(0.0f, 0.0f, 0.0f); }
    
    /**
     * @brief Up vector constant
     */
    static constexpr Vector3 up() noexcept { return Vector3(0.0f, 1.0f, 0.0f); }
    
    /**
     * @brief Down vector constant
     */
    static constexpr Vector3 down() noexcept { return Vector3(0.0f, -1.0f, 0.0f); }
    
    /**
     * @brief Forward vector constant
     */
    static constexpr Vector3 forward() noexcept { return Vector3(0.0f, 0.0f, 1.0f); }
    
    /**
     * @brief Back vector constant
     */
    static constexpr Vector3 back() noexcept { return Vector3(0.0f, 0.0f, -1.0f); }
    
    /**
     * @brief Right vector constant
     */
    static constexpr Vector3 right() noexcept { return Vector3(1.0f, 0.0f, 0.0f); }
    
    /**
     * @brief Left vector constant
     */
    static constexpr Vector3 left() noexcept { return Vector3(-1.0f, 0.0f, 0.0f); }
};

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTOR3_H
