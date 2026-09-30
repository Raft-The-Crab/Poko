/**
 * @file vector3.h
 * @brief 3D vector for physics calculations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTORS_VECTOR3_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTORS_VECTOR3_H

#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

/**
 * @brief 3D vector
 */
class Vector3 {
public:
    float x;
    float y;
    float z;
    
    constexpr Vector3() noexcept : x(0.0f), y(0.0f), z(0.0f) {}
    constexpr Vector3(float x_, float y_, float z_) noexcept : x(x_), y(y_), z(z_) {}
    
    static constexpr Vector3 zero() noexcept { return Vector3{0.0f, 0.0f, 0.0f}; }
    static constexpr Vector3 one() noexcept { return Vector3{1.0f, 1.0f, 1.0f}; }
    static constexpr Vector3 up() noexcept { return Vector3{0.0f, 1.0f, 0.0f}; }
    static constexpr Vector3 down() noexcept { return Vector3{0.0f, -1.0f, 0.0f}; }
    static constexpr Vector3 forward() noexcept { return Vector3{0.0f, 0.0f, 1.0f}; }
    static constexpr Vector3 back() noexcept { return Vector3{0.0f, 0.0f, -1.0f}; }
    static constexpr Vector3 right() noexcept { return Vector3{1.0f, 0.0f, 0.0f}; }
    static constexpr Vector3 left() noexcept { return Vector3{-1.0f, 0.0f, 0.0f}; }
    
    [[nodiscard]] constexpr Vector3 operator+(const Vector3& other) const noexcept {
        return Vector3{x + other.x, y + other.y, z + other.z};
    }

    [[nodiscard]] constexpr Vector3 operator-(const Vector3& other) const noexcept {
        return Vector3{x - other.x, y - other.y, z - other.z};
    }

    [[nodiscard]] constexpr Vector3 operator*(float scalar) const noexcept {
        return Vector3{x * scalar, y * scalar, z * scalar};
    }

    [[nodiscard]] constexpr Vector3 operator/(float scalar) const noexcept {
        return Vector3{x / scalar, y / scalar, z / scalar};
    }

    constexpr Vector3& operator+=(const Vector3& other) noexcept {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    constexpr Vector3& operator-=(const Vector3& other) noexcept {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    constexpr Vector3& operator*=(float scalar) noexcept {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    constexpr Vector3& operator/=(float scalar) noexcept {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    
    [[nodiscard]] constexpr Vector3 operator-() const noexcept {
        return Vector3{-x, -y, -z};
    }
    
    [[nodiscard]] constexpr float dot(const Vector3& other) const noexcept {
        return x * other.x + y * other.y + z * other.z;
    }
    
    [[nodiscard]] constexpr Vector3 cross(const Vector3& other) const noexcept {
        return Vector3{
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }
    
    [[nodiscard]] constexpr float lengthSquared() const noexcept {
        return x * x + y * y + z * z;
    }
    
    [[nodiscard]] float length() const noexcept {
        return std::sqrt(lengthSquared());
    }
    
    [[nodiscard]] Vector3 normalized() const noexcept {
        float len = length();
        if (len > 0.0001f) {
            return *this / len;
        }
        return zero();
    }
    
    [[nodiscard]] float distanceTo(const Vector3& other) const noexcept {
        return (*this - other).length();
    }
    
    [[nodiscard]] constexpr float distanceSquaredTo(const Vector3& other) const noexcept {
        return (*this - other).lengthSquared();
    }
    
    [[nodiscard]] static Vector3 lerp(const Vector3& a, const Vector3& b, float t) noexcept {
        return a + (b - a) * t;
    }
    
    [[nodiscard]] Vector3 min(const Vector3& other) const noexcept {
        return Vector3{
            x < other.x ? x : other.x,
            y < other.y ? y : other.y,
            z < other.z ? z : other.z
        };
    }
    
    [[nodiscard]] Vector3 max(const Vector3& other) const noexcept {
        return Vector3{
            x > other.x ? x : other.x,
            y > other.y ? y : other.y,
            z > other.z ? z : other.z
        };
    }
    
    [[nodiscard]] Vector3 abs() const noexcept {
        return Vector3{
            x < 0.0f ? -x : x,
            y < 0.0f ? -y : y,
            z < 0.0f ? -z : z
        };
    }
    
    [[nodiscard]] constexpr bool isZero() const noexcept {
        return x == 0.0f && y == 0.0f && z == 0.0f;
    }
    
    [[nodiscard]] bool isNormalized(float epsilon = 0.001f) const noexcept {
        return std::abs(lengthSquared() - 1.0f) < epsilon;
    }

    [[nodiscard]] constexpr float getComponent(int index) const noexcept {
        if (index == 0) return x;
        if (index == 1) return y;
        return z;
    }
};

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTORS_VECTOR3_H
