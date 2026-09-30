/**
 * @file vector4.h
 * @brief 4D vector for physics calculations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTORS_VECTOR4_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTORS_VECTOR4_H

#include "vector3.h"
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

/**
 * @brief 4D vector
 */
class Vector4 {
public:
    float x;
    float y;
    float z;
    float w;
    
    constexpr Vector4() noexcept : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    constexpr Vector4(float x_, float y_, float z_, float w_) noexcept : x(x_), y(y_), z(z_), w(w_) {}
    explicit constexpr Vector4(const Vector3& v, float w_ = 1.0f) noexcept : x(v.x), y(v.y), z(v.z), w(w_) {}
    
    static constexpr Vector4 zero() noexcept { return Vector4{0.0f, 0.0f, 0.0f, 0.0f}; }
    static constexpr Vector4 one() noexcept { return Vector4{1.0f, 1.0f, 1.0f, 1.0f}; }
    
    [[nodiscard]] constexpr Vector4 operator+(const Vector4& other) const noexcept {
        return Vector4{x + other.x, y + other.y, z + other.z, w + other.w};
    }
    
    [[nodiscard]] constexpr float dot(const Vector4& other) const noexcept {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }
    
    [[nodiscard]] constexpr float lengthSquared() const noexcept {
        return x * x + y * y + z * z + w * w;
    }
    
    [[nodiscard]] float length() const noexcept {
        return std::sqrt(lengthSquared());
    }
    
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

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTORS_VECTOR4_H
