/**
 * @file matrix4x4.h
 * @brief 4x4 matrix for transform calculations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_MATRIX4X4_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_MATRIX4X4_H

#include "vector3.h"
#include "vector4.h"
#include "quaternion.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

/**
 * @brief 4x4 matrix (column-major)
 * 
 * Used for transforms and projective calculations.
 * Column-major layout for graphics API compatibility.
 */
class Matrix4x4 {
public:
    // Column-major storage
    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;
    
    /**
     * @brief Default constructor (identity)
     */
    constexpr Matrix4x4() noexcept
        : m00(1.0f), m01(0.0f), m02(0.0f), m03(0.0f)
        , m10(0.0f), m11(1.0f), m12(0.0f), m13(0.0f)
        , m20(0.0f), m21(0.0f), m22(1.0f), m23(0.0f)
        , m30(0.0f), m31(0.0f), m32(0.0f), m33(1.0f) {}
    
    /**
     * @brief Component constructor
     */
    constexpr Matrix4x4(
        float m00_, float m01_, float m02_, float m03_,
        float m10_, float m11_, float m12_, float m13_,
        float m20_, float m21_, float m22_, float m23_,
        float m30_, float m31_, float m32_, float m33_
    ) noexcept
        : m00(m00_), m01(m01_), m02(m02_), m03(m03_)
        , m10(m10_), m11(m11_), m12(m12_), m13(m13_)
        , m20(m20_), m21(m21_), m22(m22_), m23(m23_)
        , m30(m30_), m31(m31_), m32(m32_), m33(m33_) {}
    
    /**
     * @brief Constants
     */
    static constexpr Matrix4x4 zero() noexcept {
        return Matrix4x4{
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f
        };
    }
    
    static constexpr Matrix4x4 identity() noexcept {
        return Matrix4x4{
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }
    
    /**
     * @brief From translation
     */
    static Matrix4x4 translation(const Vector3& t) noexcept {
        return Matrix4x4{
            1.0f, 0.0f, 0.0f, t.x,
            0.0f, 1.0f, 0.0f, t.y,
            0.0f, 0.0f, 1.0f, t.z,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }
    
    /**
     * @brief From scale
     */
    static Matrix4x4 scale(const Vector3& s) noexcept {
        return Matrix4x4{
            s.x, 0.0f, 0.0f, 0.0f,
            0.0f, s.y, 0.0f, 0.0f,
            0.0f, 0.0f, s.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }
    
    /**
     * @brief From rotation quaternion
     */
    static Matrix4x4 rotation(const Quaternion& q) noexcept {
        // Convert quaternion to 4x4 rotation matrix
        // (implementation placeholder)
        (void)q;
        return identity();
    }
    
    /**
     * @brief Matrix multiplication
     */
    [[nodiscard]] Matrix4x4 operator*(const Matrix4x4& other) const noexcept {
        return Matrix4x4{
            m00 * other.m00 + m01 * other.m10 + m02 * other.m20 + m03 * other.m30,
            m00 * other.m01 + m01 * other.m11 + m02 * other.m21 + m03 * other.m31,
            m00 * other.m02 + m01 * other.m12 + m02 * other.m22 + m03 * other.m32,
            m00 * other.m03 + m01 * other.m13 + m02 * other.m23 + m03 * other.m33,
            
            m10 * other.m00 + m11 * other.m10 + m12 * other.m20 + m13 * other.m30,
            m10 * other.m01 + m11 * other.m11 + m12 * other.m21 + m13 * other.m31,
            m10 * other.m02 + m11 * other.m12 + m12 * other.m22 + m13 * other.m32,
            m10 * other.m03 + m11 * other.m13 + m12 * other.m23 + m13 * other.m33,
            
            m20 * other.m00 + m21 * other.m10 + m22 * other.m20 + m23 * other.m30,
            m20 * other.m01 + m21 * other.m11 + m22 * other.m21 + m23 * other.m31,
            m20 * other.m02 + m21 * other.m12 + m22 * other.m22 + m23 * other.m32,
            m20 * other.m03 + m21 * other.m13 + m22 * other.m23 + m23 * other.m33,
            
            m30 * other.m00 + m31 * other.m10 + m32 * other.m20 + m33 * other.m30,
            m30 * other.m01 + m31 * other.m11 + m32 * other.m21 + m33 * other.m31,
            m30 * other.m02 + m31 * other.m12 + m32 * other.m22 + m33 * other.m32,
            m30 * other.m03 + m31 * other.m13 + m32 * other.m23 + m33 * other.m33
        };
    }
    
    /**
     * @brief Transform point
     */
    [[nodiscard]] Vector3 transformPoint(const Vector3& p) const noexcept {
        return Vector3{
            m00 * p.x + m01 * p.y + m02 * p.z + m03,
            m10 * p.x + m11 * p.y + m12 * p.z + m13,
            m20 * p.x + m21 * p.y + m22 * p.z + m23
        };
    }
    
    /**
     * @brief Transform vector (no translation)
     */
    [[nodiscard]] Vector3 transformVector(const Vector3& v) const noexcept {
        return Vector3{
            m00 * v.x + m01 * v.y + m02 * v.z,
            m10 * v.x + m11 * v.y + m12 * v.z,
            m20 * v.x + m21 * v.y + m22 * v.z
        };
    }
    
    /**
     * @brief Transpose
     */
    [[nodiscard]] Matrix4x4 transpose() const noexcept {
        return Matrix4x4{
            m00, m10, m20, m30,
            m01, m11, m21, m31,
            m02, m12, m22, m32,
            m03, m13, m23, m33
        };
    }
    
    /**
     * @brief Inverse (for transforms)
     */
    [[nodiscard]] Matrix4x4 inverse() const noexcept {
        // Simplified inverse for orthonormal transforms
        // Full inverse for general 4x4 is more complex
        // (implementation placeholder)
        return transpose();
    }
};

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_MATRIX4X4_H
