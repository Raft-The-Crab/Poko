/**
 * @file quaternion.h
 * @brief Physics-optimized quaternion for 3D rotations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_QUATERNION_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_QUATERNION_H

#include "vector3.h"
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

/**
 * @brief Physics-optimized quaternion for 3D rotations
 * 
 * Quaternions provide:
 * - Smooth interpolation (slerp)
 * - No gimbal lock
 * - Compact representation (4 floats vs 9 for rotation matrix)
 * - Efficient composition of rotations
 * 
 * Stored as (w, x, y, z) where w is the scalar part.
 * Represents rotation of angle θ around axis (x, y, z):
 * q = [cos(θ/2), sin(θ/2) * axis]
 * 
 * @section physics Physics Applications
 * - Angular velocity integration
 * - Orientation of rigid bodies
 * - Rotational damping
 * - Constraint orientation limits
 */
class Quaternion {
public:
    float w;  // Scalar part
    float x;  // Vector part X
    float y;  // Vector part Y
    float z;  // Vector part Z
    
    // ============================================================================
    // Constants
    // ============================================================================
    
    static const Quaternion IDENTITY;
    static const Quaternion ZERO;
    
    // ============================================================================
    // Constructors
    // ============================================================================
    
    /**
     * @brief Default constructor (identity quaternion)
     */
    constexpr Quaternion() noexcept : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
    
    /**
     * @brief Component constructor
     */
    constexpr Quaternion(float w_, float x_, float y_, float z_) noexcept
        : w(w_), x(x_), y(y_), z(z_) {}
    
    /**
     * @brief Axis-angle constructor
     * @param axis Rotation axis (must be normalized)
     * @param angle Rotation angle in radians
     */
    static Quaternion fromAxisAngle(const Vector3& axis, float angle) noexcept {
        float halfAngle = angle * 0.5f;
        float sinHalf = std::sin(halfAngle);
        return Quaternion(
            std::cos(halfAngle),
            axis.x * sinHalf,
            axis.y * sinHalf,
            axis.z * sinHalf
        );
    }
    
    /**
     * @brief Euler angles constructor (yaw, pitch, roll)
     * @param yaw Rotation around Y axis (radians)
     * @param pitch Rotation around X axis (radians)
     * @param roll Rotation around Z axis (radians)
     */
    static Quaternion fromEuler(float yaw, float pitch, float roll) noexcept {
        float cy = std::cos(yaw * 0.5f);
        float sy = std::sin(yaw * 0.5f);
        float cp = std::cos(pitch * 0.5f);
        float sp = std::sin(pitch * 0.5f);
        float cr = std::cos(roll * 0.5f);
        float sr = std::sin(roll * 0.5f);
        
        return Quaternion(
            cr * cp * cy + sr * sp * sy,
            sr * cp * cy - cr * sp * sy,
            cr * sp * cy + sr * cp * sy,
            cr * cp * sy - sr * sp * cy
        );
    }
    
    // ============================================================================
    // Quaternion Operations
    // ============================================================================
    
    /**
     * @brief Quaternion multiplication (composition of rotations)
     */
    [[nodiscard]] Quaternion operator*(const Quaternion& other) const noexcept {
        return Quaternion(
            w * other.w - x * other.x - y * other.y - z * other.z,
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w
        );
    }
    
    /**
     * @brief Scalar multiplication
     */
    [[nodiscard]] Quaternion operator*(float scalar) const noexcept {
        return Quaternion(w * scalar, x * scalar, y * scalar, z * scalar);
    }
    
    /**
     * @brief Scalar division
     */
    [[nodiscard]] Quaternion operator/(float scalar) const noexcept {
        return Quaternion(w / scalar, x / scalar, y / scalar, z / scalar);
    }
    
    /**
     * @brief Quaternion addition
     */
    [[nodiscard]] Quaternion operator+(const Quaternion& other) const noexcept {
        return Quaternion(w + other.w, x + other.x, y + other.y, z + other.z);
    }
    
    /**
     * @brief Quaternion subtraction
     */
    [[nodiscard]] Quaternion operator-(const Quaternion& other) const noexcept {
        return Quaternion(w - other.w, x - other.x, y - other.y, z - other.z);
    }
    
    /**
     * @brief Negation
     */
    [[nodiscard]] Quaternion operator-() const noexcept {
        return Quaternion(-w, -x, -y, -z);
    }
    
    /**
     * @brief Dot product
     */
    [[nodiscard]] float dot(const Quaternion& other) const noexcept {
        return w * other.w + x * other.x + y * other.y + z * other.z;
    }
    
    /**
     * @brief Length squared
     */
    [[nodiscard]] float lengthSquared() const noexcept {
        return w * w + x * x + y * y + z * z;
    }
    
    /**
     * @brief Length
     */
    [[nodiscard]] float length() const noexcept {
        return std::sqrt(lengthSquared());
    }
    
    /**
     * @brief Normalize to unit quaternion
     */
    [[nodiscard]] Quaternion normalized() const noexcept {
        float len = length();
        if (len > 0.0001f) {
            return *this / len;
        }
        return IDENTITY;
    }
    
    /**
     * @brief Normalize in place
     */
    void normalize() noexcept {
        float len = length();
        if (len > 0.0001f) {
            w /= len;
            x /= len;
            y /= len;
            z /= len;
        }
    }
    
    /**
     * @brief Conjugate (inverse rotation)
     */
    [[nodiscard]] Quaternion conjugate() const noexcept {
        return Quaternion(w, -x, -y, -z);
    }
    
    /**
     * @brief Inverse (conjugate / length²)
     */
    [[nodiscard]] Quaternion inverse() const noexcept {
        float lenSq = lengthSquared();
        if (lenSq > 0.0001f) {
            return conjugate() / lenSq;
        }
        return IDENTITY;
    }
    
    /**
     * @brief Rotate a vector by this quaternion
     * v' = q * v * q⁻¹
     */
    [[nodiscard]] Vector3 rotateVector(const Vector3& vec) const noexcept {
        // Optimized formula: v' = v + 2 * cross(q.xyz, cross(q.xyz, v) + q.w * v)
        Vector3 qvec(x, y, z);
        Vector3 uv = qvec.cross(vec);
        Vector3 uuv = qvec.cross(uv);
        uv = uv * (2.0f * w);
        uuv = uuv * 2.0f;
        return vec + uv + uuv;
    }
    
    /**
     * @brief Spherical linear interpolation (slerp)
     * Interpolates between two quaternions along the shortest path
     */
    static Quaternion slerp(const Quaternion& q1, const Quaternion& q2, float t) noexcept {
        float dot = q1.dot(q2);
        
        // If dot is negative, take the shorter path
        Quaternion q2Temp = q2;
        if (dot < 0.0f) {
            q2Temp = -q2;
            dot = -dot;
        }
        
        // If quaternions are very close, use linear interpolation
        if (dot > 0.9995f) {
            Quaternion result = q1 + (q2Temp - q1) * t;
            return result.normalized();
        }
        
        float theta0 = std::acos(dot);
        float theta = theta0 * t;
        float sinTheta = std::sin(theta);
        float sinTheta0 = std::sin(theta0);
        
        float s0 = std::cos(theta) - dot * sinTheta / sinTheta0;
        float s1 = sinTheta / sinTheta0;
        
        return q1 * s0 + q2Temp * s1;
    }
    
    /**
     * @brief Linear interpolation (lerp) followed by normalization
     * Faster than slerp but not constant angular velocity
     */
    static Quaternion lerp(const Quaternion& q1, const Quaternion& q2, float t) noexcept {
        Quaternion result = q1 + (q2 - q1) * t;
        return result.normalized();
    }
    
    /**
     * @brief Check if quaternion is identity
     */
    [[nodiscard]] bool isIdentity(float epsilon = 0.001f) const noexcept {
        return std::abs(w - 1.0f) < epsilon &&
               std::abs(x) < epsilon &&
               std::abs(y) < epsilon &&
               std::abs(z) < epsilon;
    }
    
    /**
     * @brief Check if quaternion is normalized
     */
    [[nodiscard]] bool isNormalized(float epsilon = 0.001f) const noexcept {
        return std::abs(lengthSquared() - 1.0f) < epsilon;
    }
    
    /**
     * @brief Get rotation axis
     */
    [[nodiscard]] Vector3 getAxis() const noexcept {
        float sinHalf = std::sqrt(1.0f - w * w);
        if (sinHalf > 0.0001f) {
            return Vector3(x, y, z) / sinHalf;
        }
        return Vector3::UP;
    }
    
    /**
     * @brief Get rotation angle in radians
     */
    [[nodiscard]] float getAngle() const noexcept {
        float clampedW = w;
        if (clampedW > 1.0f) clampedW = 1.0f;
        if (clampedW < -1.0f) clampedW = -1.0f;
        return 2.0f * std::acos(clampedW);
    }
};

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

// Include inline implementations
#include "quaternion.inl"

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_QUATERNION_H
