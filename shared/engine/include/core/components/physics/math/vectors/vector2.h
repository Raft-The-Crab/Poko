/**
 * @file vector2.h
 * @brief 2D vector for physics calculations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTOR2_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTOR2_H

#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

/**
 * @brief 2D vector
 */
class Vector2 {
public:
    float x;
    float y;
    
    /**
     * @brief Default constructor (zero vector)
     */
    constexpr Vector2() noexcept : x(0.0f), y(0.0f) {}
    
    /**
     * @brief Component constructor
     */
    constexpr Vector2(float x_, float y_) noexcept : x(x_), y(y_) {}
    
    /**
     * @brief Constants
     */
    static constexpr Vector2 zero() noexcept { return Vector2{0.0f, 0.0f}; }
    static constexpr Vector2 one() noexcept { return Vector2{1.0f, 1.0f}; }
    static constexpr Vector2 up() noexcept { return Vector2{0.0f, 1.0f}; }
    static constexpr Vector2 down() noexcept { return Vector2{0.0f, -1.0f}; }
    static constexpr Vector2 right() noexcept { return Vector2{1.0f, 0.0f}; }
    static constexpr Vector2 left() noexcept { return Vector2{-1.0f, 0.0f}; }
    
    /**
     * @brief Addition
     */
    [[nodiscard]] constexpr Vector2 operator+(const Vector2& other) const noexcept {
        return Vector2{x + other.x, y + other.y};
    }
    
    /**
     * @brief Subtraction
     */
    [[nodiscard]] constexpr Vector2 operator-(const Vector2& other) const noexcept {
        return Vector2{x - other.x, y - other.y};
    }
    
    /**
     * @brief Scalar multiplication
     */
    [[nodiscard]] constexpr Vector2 operator*(float scalar) const noexcept {
        return Vector2{x * scalar, y * scalar};
    }
    
    /**
     * @brief Scalar division
     */
    [[nodiscard]] constexpr Vector2 operator/(float scalar) const noexcept {
        return Vector2{x / scalar, y / scalar};
    }
    
    /**
     * @brief Negation
     */
    [[nodiscard]] constexpr Vector2 operator-() const noexcept {
        return Vector2{-x, -y};
    }
    
    /**
     * @brief Dot product
     */
    [[nodiscard]] constexpr float dot(const Vector2& other) const noexcept {
        return x * other.x + y * other.y;
    }
    
    /**
     * @brief Cross product (2D, returns scalar z-component)
     */
    [[nodiscard]] constexpr float cross(const Vector2& other) const noexcept {
        return x * other.y - y * other.x;
    }
    
    /**
     * @brief Length squared
     */
    [[nodiscard]] constexpr float lengthSquared() const noexcept {
        return x * x + y * y;
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
    [[nodiscard]] Vector2 normalized() const noexcept {
        float len = length();
        if (len > 0.0001f) {
            return *this / len;
        }
        return zero();
    }
    
    /**
     * @brief Distance to another vector
     */
    [[nodiscard]] float distanceTo(const Vector2& other) const noexcept {
        return (*this - other).length();
    }
    
    /**
     * @brief Distance squared to another vector
     */
    [[nodiscard]] constexpr float distanceSquaredTo(const Vector2& other) const noexcept {
        return (*this - other).lengthSquared();
    }
    
    /**
     * @brief Check if zero
     */
    [[nodiscard]] constexpr bool isZero() const noexcept {
        return x == 0.0f && y == 0.0f;
    }
    
    /**
     * @brief Check if normalized
     */
    [[nodiscard]] bool isNormalized(float epsilon = 0.001f) const noexcept {
        return std::abs(length() - 1.0f) < epsilon;
    }
};

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTOR2_H
