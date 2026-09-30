/**
 * @file vector2.h
 * @brief 2D vector for physics calculations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTORS_VECTOR2_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTORS_VECTOR2_H

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
    
    constexpr Vector2() noexcept : x(0.0f), y(0.0f) {}
    constexpr Vector2(float x_, float y_) noexcept : x(x_), y(y_) {}
    
    static constexpr Vector2 zero() noexcept { return Vector2{0.0f, 0.0f}; }
    static constexpr Vector2 one() noexcept { return Vector2{1.0f, 1.0f}; }
    static constexpr Vector2 up() noexcept { return Vector2{0.0f, 1.0f}; }
    static constexpr Vector2 down() noexcept { return Vector2{0.0f, -1.0f}; }
    static constexpr Vector2 right() noexcept { return Vector2{1.0f, 0.0f}; }
    static constexpr Vector2 left() noexcept { return Vector2{-1.0f, 0.0f}; }
    
    [[nodiscard]] constexpr Vector2 operator+(const Vector2& other) const noexcept {
        return Vector2{x + other.x, y + other.y};
    }
    
    [[nodiscard]] constexpr Vector2 operator-(const Vector2& other) const noexcept {
        return Vector2{x - other.x, y - other.y};
    }
    
    [[nodiscard]] constexpr Vector2 operator*(float scalar) const noexcept {
        return Vector2{x * scalar, y * scalar};
    }
    
    [[nodiscard]] constexpr Vector2 operator/(float scalar) const noexcept {
        return Vector2{x / scalar, y / scalar};
    }
    
    [[nodiscard]] constexpr Vector2 operator-() const noexcept {
        return Vector2{-x, -y};
    }
    
    [[nodiscard]] constexpr float dot(const Vector2& other) const noexcept {
        return x * other.x + y * other.y;
    }
    
    [[nodiscard]] constexpr float cross(const Vector2& other) const noexcept {
        return x * other.y - y * other.x;
    }
    
    [[nodiscard]] constexpr float lengthSquared() const noexcept {
        return x * x + y * y;
    }
    
    [[nodiscard]] float length() const noexcept {
        return std::sqrt(lengthSquared());
    }
    
    [[nodiscard]] Vector2 normalized() const noexcept {
        float len = length();
        if (len > 0.0001f) {
            return *this / len;
        }
        return zero();
    }
    
    [[nodiscard]] float distanceTo(const Vector2& other) const noexcept {
        return (*this - other).length();
    }
    
    [[nodiscard]] constexpr float distanceSquaredTo(const Vector2& other) const noexcept {
        return (*this - other).lengthSquared();
    }
    
    [[nodiscard]] constexpr bool isZero() const noexcept {
        return x == 0.0f && y == 0.0f;
    }
};

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTORS_VECTOR2_H
