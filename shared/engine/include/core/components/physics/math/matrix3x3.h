/**
 * @file matrix3x3.h
 * @brief Physics-optimized 3x3 matrix for inertia tensors and rotation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_MATRIX3X3_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_MATRIX3X3_H

#include "vector3.h"
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

/**
 * @brief Physics-optimized 3x3 matrix
 * 
 * Used for:
 * - Inertia tensors (angular mass properties)
 * - Rotation matrices (3D rotations)
 * - Cross product matrices (for angular velocity calculations)
 * 
 * Stored in column-major order for compatibility with graphics APIs:
 * | m00 m03 m06 |
 * | m01 m04 m07 |
 * | m02 m05 m08 |
 * 
 * @section inertia_tensors Inertia Tensors
 * For a diagonal inertia tensor (principal axes aligned with world axes):
 * | Ixx  0   0  |
 * | 0   Iyy  0  |
 * | 0    0  Izz |
 */
class Matrix3x3 {
public:
    // Matrix elements (column-major)
    float m00, m01, m02;  // Column 0
    float m03, m04, m05;  // Column 1
    float m06, m07, m08;  // Column 2
    
    // ============================================================================
    // Constants
    // ============================================================================
    
    static const Matrix3x3 ZERO;
    static const Matrix3x3 IDENTITY;
    
    // ============================================================================
    // Constructors
    // ============================================================================
    
    /**
     * @brief Default constructor (zero matrix)
     */
    constexpr Matrix3x3() noexcept
        : m00(0.0f), m01(0.0f), m02(0.0f)
        , m03(0.0f), m04(0.0f), m05(0.0f)
        , m06(0.0f), m07(0.0f), m08(0.0f) {}
    
    /**
     * @brief Component constructor
     */
    constexpr Matrix3x3(
        float m00_, float m01_, float m02_,
        float m03_, float m04_, float m05_,
        float m06_, float m07_, float m08_
    ) noexcept
        : m00(m00_), m01(m01_), m02(m02_)
        , m03(m03_), m04(m04_), m05(m05_)
        , m06(m06_), m07(m07_), m08(m08_) {}
    
    /**
     * @brief Diagonal matrix constructor
     */
    static Matrix3x3 diagonal(float d0, float d1, float d2) noexcept {
        return Matrix3x3(
            d0, 0.0f, 0.0f,
            0.0f, d1, 0.0f,
            0.0f, 0.0f, d2
        );
    }
    
    /**
     * @brief Create diagonal inertia tensor
     */
    static Matrix3x3 inertiaTensor(float Ixx, float Iyy, float Izz) noexcept {
        return diagonal(Ixx, Iyy, Izz);
    }
    
    // ============================================================================
    // Matrix Operations
    // ============================================================================
    
    /**
     * @brief Matrix multiplication
     */
    [[nodiscard]] Matrix3x3 operator*(const Matrix3x3& other) const noexcept {
        return Matrix3x3(
            m00 * other.m00 + m03 * other.m01 + m06 * other.m02,
            m01 * other.m00 + m04 * other.m01 + m07 * other.m02,
            m02 * other.m00 + m05 * other.m01 + m08 * other.m02,
            
            m00 * other.m03 + m03 * other.m04 + m06 * other.m05,
            m01 * other.m03 + m04 * other.m04 + m07 * other.m05,
            m02 * other.m03 + m05 * other.m04 + m08 * other.m05,
            
            m00 * other.m06 + m03 * other.m07 + m06 * other.m08,
            m01 * other.m06 + m04 * other.m07 + m07 * other.m08,
            m02 * other.m06 + m05 * other.m07 + m08 * other.m08
        );
    }
    
    /**
     * @brief Matrix-vector multiplication
     */
    [[nodiscard]] Vector3 operator*(const Vector3& vec) const noexcept {
        return Vector3(
            m00 * vec.x + m03 * vec.y + m06 * vec.z,
            m01 * vec.x + m04 * vec.y + m07 * vec.z,
            m02 * vec.x + m05 * vec.y + m08 * vec.z
        );
    }
    
    /**
     * @brief Scalar multiplication
     */
    [[nodiscard]] Matrix3x3 operator*(float scalar) const noexcept {
        return Matrix3x3(
            m00 * scalar, m01 * scalar, m02 * scalar,
            m03 * scalar, m04 * scalar, m05 * scalar,
            m06 * scalar, m07 * scalar, m08 * scalar
        );
    }
    
    /**
     * @brief Matrix addition
     */
    [[nodiscard]] Matrix3x3 operator+(const Matrix3x3& other) const noexcept {
        return Matrix3x3(
            m00 + other.m00, m01 + other.m01, m02 + other.m02,
            m03 + other.m03, m04 + other.m04, m05 + other.m05,
            m06 + other.m06, m07 + other.m07, m08 + other.m08
        );
    }
    
    /**
     * @brief Matrix subtraction
     */
    [[nodiscard]] Matrix3x3 operator-(const Matrix3x3& other) const noexcept {
        return Matrix3x3(
            m00 - other.m00, m01 - other.m01, m02 - other.m02,
            m03 - other.m03, m04 - other.m04, m05 - other.m05,
            m06 - other.m06, m07 - other.m07, m08 - other.m08
        );
    }
    
    /**
     * @brief Transpose
     */
    [[nodiscard]] Matrix3x3 transpose() const noexcept {
        return Matrix3x3(
            m00, m03, m06,
            m01, m04, m07,
            m02, m05, m08
        );
    }
    
    /**
     * @brief Determinant
     */
    [[nodiscard]] float determinant() const noexcept {
        return m00 * (m04 * m08 - m05 * m07)
             - m03 * (m01 * m08 - m02 * m07)
             + m06 * (m01 * m05 - m02 * m04);
    }
    
    /**
     * @brief Inverse (for inertia tensor inversion)
     * Returns zero matrix if determinant is zero
     */
    [[nodiscard]] Matrix3x3 inverse() const noexcept {
        float det = determinant();
        if (std::abs(det) < 0.0001f) {
            return ZERO;
        }
        
        float invDet = 1.0f / det;
        
        return Matrix3x3(
            (m04 * m08 - m05 * m07) * invDet,
            (m02 * m07 - m01 * m08) * invDet,
            (m01 * m05 - m02 * m04) * invDet,
            
            (m05 * m06 - m03 * m08) * invDet,
            (m00 * m08 - m02 * m06) * invDet,
            (m02 * m03 - m00 * m05) * invDet,
            
            (m03 * m07 - m04 * m06) * invDet,
            (m01 * m06 - m00 * m07) * invDet,
            (m00 * m04 - m01 * m03) * invDet
        );
    }
    
    /**
     * @brief Create cross product matrix from vector
     * Used for angular velocity calculations: ω × r = [ω]× * r
     */
    static Matrix3x3 crossProductMatrix(const Vector3& vec) noexcept {
        return Matrix3x3(
            0.0f, -vec.z, vec.y,
            vec.z, 0.0f, -vec.x,
            -vec.y, vec.x, 0.0f
        );
    }
    
    /**
     * @brief Check if matrix is identity
     */
    [[nodiscard]] bool isIdentity(float epsilon = 0.001f) const noexcept {
        return std::abs(m00 - 1.0f) < epsilon &&
               std::abs(m01) < epsilon &&
               std::abs(m02) < epsilon &&
               std::abs(m03) < epsilon &&
               std::abs(m04 - 1.0f) < epsilon &&
               std::abs(m05) < epsilon &&
               std::abs(m06) < epsilon &&
               std::abs(m07) < epsilon &&
               std::abs(m08 - 1.0f) < epsilon;
    }
    
    /**
     * @brief Check if matrix is zero
     */
    [[nodiscard]] bool isZero(float epsilon = 0.001f) const noexcept {
        return std::abs(m00) < epsilon &&
               std::abs(m01) < epsilon &&
               std::abs(m02) < epsilon &&
               std::abs(m03) < epsilon &&
               std::abs(m04) < epsilon &&
               std::abs(m05) < epsilon &&
               std::abs(m06) < epsilon &&
               std::abs(m07) < epsilon &&
               std::abs(m08) < epsilon;
    }
};

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

// Include inline implementations
#include "matrix3x3.inl"

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_MATRIX3X3_H
