/**
 * @file quaternion.h
 * @brief Quaternion for 3D rotations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTORS_QUATERNION_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTORS_QUATERNION_H

#include "vector3.h"
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

/**
 * @brief Quaternion for 3D rotations
 */
class Quaternion {
public:
    float w;
    float x;
    float y;
    float z;
    
    constexpr Quaternion() noexcept : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
    constexpr Quaternion(float w_, float x_, float y_, float z_) noexcept : w(w_), x(x_), y(y_), z(z_) {}
    
    static constexpr Quaternion identity() noexcept { return Quaternion{1.0f, 0.0f, 0.0f, 0.0f}; }
    static constexpr Quaternion zero() noexcept { return Quaternion{0.0f, 0.0f, 0.0f, 0.0f}; }
    
    static Quaternion fromAxisAngle(const Vector3& axis, float angle) noexcept {
        float halfAngle = angle * 0.5f;
        float s = std::sin(halfAngle);
        return Quaternion{
            std::cos(halfAngle),
            axis.x * s,
            axis.y * s,
            axis.z * s
        };
    }
    
    static Quaternion fromEuler(float yaw, float pitch, float roll) noexcept {
        float cy = std::cos(yaw * 0.5f);
        float sy = std::sin(yaw * 0.5f);
        float cp = std::cos(pitch * 0.5f);
        float sp = std::sin(pitch * 0.5f);
        float cr = std::cos(roll * 0.5f);
        float sr = std::sin(roll * 0.5f);
        
        return Quaternion{
            cr * cp * cy + sr * sp * sy,
            sr * cp * cy - cr * sp * sy,
            cr * sp * cy + sr * cp * sy,
            cr * cp * sy - sr * sp * cy
        };
    }
    
    [[nodiscard]] Quaternion operator*(const Quaternion& other) const noexcept {
        return Quaternion{
            w * other.w - x * other.x - y * other.y - z * other.z,
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w
        };
    }
    
    [[nodiscard]] Quaternion operator*(float scalar) const noexcept {
        return Quaternion{w * scalar, x * scalar, y * scalar, z * scalar};
    }
    
    [[nodiscard]] Quaternion operator/(float scalar) const noexcept {
        return Quaternion{w / scalar, x / scalar, y / scalar, z / scalar};
    }
    
    [[nodiscard]] constexpr float dot(const Quaternion& other) const noexcept {
        return w * other.w + x * other.x + y * other.y + z * other.z;
    }
    
    [[nodiscard]] float length() const noexcept {
        return std::sqrt(lengthSquared());
    }
    
    [[nodiscard]] constexpr float lengthSquared() const noexcept {
        return w * w + x * x + y * y + z * z;
    }
    
    [[nodiscard]] Quaternion normalized() const noexcept {
        float len = length();
        if (len > 0.0001f) {
            return *this / len;
        }
        return identity();
    }
    
    [[nodiscard]] Quaternion conjugate() const noexcept {
        return Quaternion{w, -x, -y, -z};
    }
    
    [[nodiscard]] Quaternion inverse() const noexcept {
        float lenSq = lengthSquared();
        if (lenSq > 0.0001f) {
            return conjugate() / lenSq;
        }
        return identity();
    }
    
    [[nodiscard]] Vector3 rotateVector(const Vector3& v) const noexcept {
        Quaternion qv = Quaternion{0.0f, v.x, v.y, v.z};
        Quaternion qvPrime = *this * qv * conjugate();
        return Vector3{qvPrime.x, qvPrime.y, qvPrime.z};
    }
    
    [[nodiscard]] static Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float t) noexcept {
        float dot = q1.dot(q2);
        Quaternion q2Temp = q2;
        if (dot < 0.0f) {
            q2Temp = -q2;
            dot = -dot;
        }
        if (dot > 0.9995f) {
            return (q1 + (q2Temp - q1) * t).normalized();
        }
        float theta0 = std::acos(std::clamp(dot, -1.0f, 1.0f));
        float theta = theta0 * t;
        float sinTheta = std::sin(theta);
        float sinTheta0 = std::sin(theta0);
        float s0 = std::cos(theta) - dot * sinTheta / sinTheta0;
        float s1 = sinTheta / sinTheta0;
        return q1 * s0 + q2Temp * s1;
    }
    
    [[nodiscard]] static Quaternion lerp(const Quaternion& q1, const Quaternion& q2, float t) noexcept {
        return (q1 + (q2 - q1) * t).normalized();
    }
};

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTORS_QUATERNION_H
