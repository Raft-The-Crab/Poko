/**
 * @file vector3.h
 * @brief Physics-optimized 3D vector
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTOR3_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTOR3_H

#include <cmath>
#include <cstdint>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

/**
 * @brief Physics-optimized 3D vector
 * 
 * Designed for physics calculations with:
 * - Cache-friendly memory layout (12 bytes, padded to 16 for alignment)
 * - Fast arithmetic operations
 * - Cross product for torque and angular impulse calculations
 * - Dot product for projections and angle calculations
 * - Length and normalization for unit vectors
 * 
 * @section performance Performance
 * Structured for potential SIMD optimization in future.
 * Currently uses scalar operations for broad compatibility.
 */
class Vector3 {
public:
    float x;
    float y;
    float z;
    
    // ============================================================================
    // Constants
    // ============================================================================
    
    static constexpr Vector3 zero() noexcept { return Vector3{0.0f, 0.0f, 0.0f}; }
    static constexpr Vector3 one() noexcept { return Vector3{1.0f, 1.0f, 1.0f}; }
    static constexpr Vector3 up() noexcept { return Vector3{0.0f, 1.0f, 0.0f}; }
    static constexpr Vector3 down() noexcept { return Vector3{0.0f, -1.0f, 0.0f}; }
    static constexpr Vector3 forward() noexcept { return Vector3{0.0f, 0.0f, 1.0f}; }
    static constexpr Vector3 back() noexcept { return Vector3{0.0f, 0.0f, -1.0f}; }
    static constexpr Vector3 right() noexcept { return Vector3{1.0f, 0.0f, 0.0f}; }
    static constexpr Vector3 left() noexcept { return Vector3{-1.0f, 0.0f, 0.0f}; }
    
    // ============================================================================
    // Constructors
    // ============================================================================
    
    /**
     * @brief Default constructor (zero vector)
     */
    constexpr Vector3() noexcept : x(0.0f), y(0.0f), z(0.0f) {}
    
    /**
     * @brief Component constructor
     */
    constexpr Vector3(float x_, float y_, float z_) noexcept : x(x_), y(y_), z(z_) {}
    
    // ============================================================================
    // Arithmetic Operations
    // ============================================================================
    
    /**
     * @brief Vector addition
     */
    [[nodiscard]] Vector3 operator+(const Vector3& other) const noexcept {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    
    /**
     * @brief Vector addition assignment
     */
    Vector3& operator+=(const Vector3& other) noexcept {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    
    /**
     * @brief Vector subtraction
     */
    [[nodiscard]] Vector3 operator-(const Vector3& other) const noexcept {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    
    /**
     * @brief Vector subtraction assignment
     */
    Vector3& operator-=(const Vector3& other) noexcept {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    
    /**
     * @brief Scalar multiplication
     */
    [[nodiscard]] Vector3 operator*(float scalar) const noexcept {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    
    /**
     * @brief Scalar multiplication assignment
     */
    Vector3& operator*=(float scalar) noexcept {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    
    /**
     * @brief Scalar division
     */
    [[nodiscard]] Vector3 operator/(float scalar) const noexcept {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }
    
    /**
     * @brief Scalar division assignment
     */
    Vector3& operator/=(float scalar) noexcept {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }
    
    /**
     * @brief Negation
     */
    [[nodiscard]] Vector3 operator-() const noexcept {
        return Vector3(-x, -y, -z);
    }
    
    // ============================================================================
    // Vector Operations
    // ============================================================================
    
    /**
     * @brief Dot product
     * @return Dot product: a · b = ax*bx + ay*by + az*bz
     */
    [[nodiscard]] float dot(const Vector3& other) const noexcept {
        return x * other.x + y * other.y + z * other.z;
    }
    
    /**
     * @brief Cross product
     * @return Cross product: a × b
     * Used for torque (τ = r × F) and angular impulse calculations
     */
    [[nodiscard]] Vector3 cross(const Vector3& other) const noexcept {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
    
    /**
     * @brief Length squared (faster than length, avoids sqrt)
     * @return Length squared: |v|² = x² + y² + z²
     */
    [[nodiscard]] float lengthSquared() const noexcept {
        return x * x + y * y + z * z;
    }
    
    /**
     * @brief Length
     * @return Length: |v| = sqrt(x² + y² + z²)
     */
    [[nodiscard]] float length() const noexcept {
        return std::sqrt(lengthSquared());
    }
    
    /**
     * @brief Normalize to unit length
     * @return Normalized vector: v / |v|
     * Returns zero vector if length is zero
     */
    [[nodiscard]] Vector3 normalized() const noexcept {
        float len = length();
        if (len > 0.0001f) {
            return *this / len;
        }
        return zero();
    }
    
    /**
     * @brief Normalize in place
     */
    void normalize() noexcept {
        float len = length();
        if (len > 0.0001f) {
            x /= len;
            y /= len;
            z /= len;
        }
    }
    
    /**
     * @brief Distance to another vector
     * @return Euclidean distance: |a - b|
     */
    [[nodiscard]] float distanceTo(const Vector3& other) const noexcept {
        return (*this - other).length();
    }
    
    /**
     * @brief Distance squared to another vector (faster, avoids sqrt)
     * @return Distance squared: |a - b|²
     */
    [[nodiscard]] float distanceSquaredTo(const Vector3& other) const noexcept {
        return (*this - other).lengthSquared();
    }
    
    /**
     * @brief Linear interpolation
     */
    [[nodiscard]] static Vector3 lerp(const Vector3& a, const Vector3& b, float t) noexcept {
        return a + (b - a) * t;
    }
    
    /**
     * @brief Projection onto another vector
     */
    [[nodiscard]] Vector3 project(const Vector3& other) const noexcept {
        return other * (dot(other) / other.lengthSquared());
    }
    
    /**
     * @brief Rejection from another vector
     */
    [[nodiscard]] Vector3 reject(const Vector3& other) const noexcept {
        return *this - project(other);
    }
    
    /**
     * @brief Reflection
     */
    [[nodiscard]] Vector3 reflect(const Vector3& normal) const noexcept {
        return *this - normal * (2.0f * dot(normal));
    }
    
    /**
     * @brief Clamping
     */
    [[nodiscard]] Vector3 clamped(float minVal, float maxVal) const noexcept {
        return Vector3{
            x > maxVal ? maxVal : (x < minVal ? minVal : x),
            y > maxVal ? maxVal : (y < minVal ? minVal : y),
            z > maxVal ? maxVal : (z < minVal ? minVal : z)
        };
    }
    
    /**
     * @brief Component-wise min
     */
    [[nodiscard]] Vector3 min(const Vector3& other) const noexcept {
        return Vector3{
            x < other.x ? x : other.x,
            y < other.y ? y : other.y,
            z < other.z ? z : other.z
        };
    }
    
    /**
     * @brief Component-wise max
     */
    [[nodiscard]] Vector3 max(const Vector3& other) const noexcept {
        return Vector3{
            x > other.x ? x : other.x,
            y > other.y ? y : other.y,
            z > other.z ? z : other.z
        };
    }
    
    /**
     * @brief Absolute value
     */
    [[nodiscard]] Vector3 abs() const noexcept {
        return Vector3{
            x < 0.0f ? -x : x,
            y < 0.0f ? -y : y,
            z < 0.0f ? -z : z
        };
    }
    
    /**
     * @brief Check if vector is zero (within epsilon)
     */
    [[nodiscard]] bool isZero(float epsilon = 0.0001f) const noexcept {
        return lengthSquared() < epsilon * epsilon;
    }
    
    /**
     * @brief Check if vector is normalized (within epsilon)
     */
    [[nodiscard]] bool isNormalized(float epsilon = 0.001f) const noexcept {
        return std::abs(lengthSquared() - 1.0f) < epsilon;
    }
    
    /**
     * @brief Linear interpolation
     */
    [[nodiscard]] static Vector3 lerp(const Vector3& a, const Vector3& b, float t) noexcept {
        return a + (b - a) * t;
    }
    
    // ============================================================================
    // Comparison
    // ============================================================================
    
    /**
     * @brief Equality check (exact)
     */
    [[nodiscard]] bool operator==(const Vector3& other) const noexcept {
        return x == other.x && y == other.y && z == other.z;
    }
    
    /**
     * @brief Inequality check (exact)
     */
    [[nodiscard]] bool operator!=(const Vector3& other) const noexcept {
        return !(*this == other);
    }
    
    /**
     * @brief Component-wise less than
     */
    [[nodiscard]] bool operator<(const Vector3& other) const noexcept {
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        return z < other.z;
    }
};

// ============================================================================
// Inline Scalar Operations (for symmetry)
// ============================================================================

/**
 * @brief Scalar multiplication (left operand)
 */
inline Vector3 operator*(float scalar, const Vector3& vec) noexcept {
    return vec * scalar;
}

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTOR3_H
