/**
 * @file vector4.h
 * @brief 4D vector for physics calculations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTOR4_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTOR4_H

#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

// Forward declaration
class Vector3;

/**
 * @brief 4D vector
 */
class Vector4 {
public:
    float x;
    float y;
    float z;
    float w;
    
    /**
     * @brief Default constructor (zero vector)
     */
    constexpr Vector4() noexcept : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    
    /**
     * @brief Component constructor
     */
    constexpr Vector4(float x_, float y_, float z_, float w_) noexcept
        : x(x_), y(y_), z(z_), w(w_) {}
    
    /**
     * @brief From Vector3 (w = 1.0f for point, w = 0.0f for vector)
     */
    explicit Vector4(const Vector3& v, float w_ = 1.0f) noexcept
        : x(v.x), y(v.y), z(v.z), w(w_) {}
    
    /**
     * @brief Constants
     */
    static constexpr Vector4 zero() noexcept { return Vector4{0.0f, 0.0f, 0.0f, 0.0f}; }
    static constexpr Vector4 one() noexcept { return Vector4{1.0f, 1.0f, 1.0f, 1.0f}; }
    
    /**
     * @brief Addition
     */
    [[nodiscard]] constexpr Vector4 operator+(const Vector4& other) const noexcept {
        return Vector4{x + other.x, y + other.y, z + other.z, w + other.w};
    }
    
    /**
     * @brief Subtraction
     */
    [[nodiscard]] constexpr Vector4 operator-(const Vector4& other) const noexcept {
        return Vector4{x - other.x, y - other.y, z - other.z, w - other.w};
    }
    
    /**
     * @brief Scalar multiplication
     */
    [[nodiscard]] constexpr Vector4 operator*(float scalar) const noexcept {
        return Vector4{x * scalar, y * scalar, z * scalar, w * scalar};
    }
    
    /**
     * @brief Dot product
     */
    [[nodiscard]] constexpr float dot(const Vector4& other) const noexcept {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }
    
    /**
     * @brief Length squared
     */
    [[nodiscard]] constexpr float lengthSquared() const noexcept {
        return x * x + y * y + z * z + w * w;
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
    [[nodiscard]] Vector4 normalized() const noexcept {
        float len = length();
        if (len > 0.0001f) {
            return *this / len;
        }
        return zero();
    }
};

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTOR4_H
