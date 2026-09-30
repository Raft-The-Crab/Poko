/**
 * @file matrix3x3.h
 * @brief 3x3 matrix for physics calculations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATRICES_MATRIX3X3_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATRICES_MATRIX3X3_H

#include "../math/vectors/vector3.h"
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace matrices {

using math::Vector3;

/**
 * @brief 3x3 matrix (column-major)
 */
class Matrix3x3 {
public:
    float m00, m01, m02;
    float m10, m11, m12;
    float m20, m21, m22;
    
    constexpr Matrix3x3() noexcept
        : m00(1.0f), m01(0.0f), m02(0.0f)
        , m10(0.0f), m11(1.0f), m12(0.0f)
        , m20(0.0f), m21(0.0f), m22(1.0f) {}
    
    constexpr Matrix3x3(
        float m00_, float m01_, float m02_,
        float m10_, float m11_, float m12_,
        float m20_, float m21_, float m22_
    ) noexcept
        : m00(m00_), m01(m01_), m02(m02_)
        , m10(m10_), m11(m11_), m12(m12_)
        , m20(m20_), m21(m21_), m22(m22_) {}
    
    static constexpr Matrix3x3 zero() noexcept {
        return Matrix3x3{
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f
        };
    }
    
    static constexpr Matrix3x3 identity() noexcept {
        return Matrix3x3{
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f
        };
    }
    
    static Matrix3x3 diagonal(float xx, float yy, float zz) noexcept {
        return Matrix3x3{
            xx, 0.0f, 0.0f,
            0.0f, yy, 0.0f,
            0.0f, 0.0f, zz
        };
    }
    
    static Matrix3x3 inertiaTensor(float Ixx, float Iyy, float Izz) noexcept {
        return diagonal(Ixx, Iyy, Izz);
    }
    
    static Matrix3x3 crossProductMatrix(const Vector3& v) noexcept {
        // skew(v) * x = v × x
        return Matrix3x3{
            0.0f, v.z, -v.y,
            -v.z, 0.0f, v.x,
            v.y, -v.x, 0.0f
        };
    }
    
    [[nodiscard]] Matrix3x3 operator*(const Matrix3x3& other) const noexcept {
        return Matrix3x3{
            m00 * other.m00 + m01 * other.m10 + m02 * other.m20,
            m00 * other.m01 + m01 * other.m11 + m02 * other.m21,
            m00 * other.m02 + m01 * other.m12 + m02 * other.m22,
            
            m10 * other.m00 + m11 * other.m10 + m12 * other.m20,
            m10 * other.m01 + m11 * other.m11 + m12 * other.m21,
            m10 * other.m02 + m11 * other.m12 + m12 * other.m22,
            
            m20 * other.m00 + m21 * other.m10 + m22 * other.m20,
            m20 * other.m01 + m21 * other.m11 + m22 * other.m21,
            m20 * other.m02 + m21 * other.m12 + m22 * other.m22
        };
    }
    
    [[nodiscard]] Vector3 operator*(const Vector3& v) const noexcept {
        return Vector3{
            m00 * v.x + m01 * v.y + m02 * v.z,
            m10 * v.x + m11 * v.y + m12 * v.z,
            m20 * v.x + m21 * v.y + m22 * v.z
        };
    }
    
    [[nodiscard]] constexpr float determinant() const noexcept {
        return m00 * (m11 * m22 - m12 * m21)
             - m01 * (m10 * m22 - m12 * m20)
             + m02 * (m10 * m21 - m11 * m20);
    }
    
    [[nodiscard]] bool tryInverse(Matrix3x3& result, float epsilon = 0.0001f) const noexcept {
        float det = determinant();
        if (std::abs(det) < epsilon) {
            return false;
        }
        
        float invDet = 1.0f / det;
        result.m00 = (m11 * m22 - m12 * m21) * invDet;
        result.m01 = (m02 * m21 - m01 * m22) * invDet;
        result.m02 = (m01 * m12 - m02 * m11) * invDet;
        result.m10 = (m12 * m20 - m10 * m22) * invDet;
        result.m11 = (m00 * m22 - m02 * m20) * invDet;
        result.m12 = (m02 * m10 - m00 * m12) * invDet;
        result.m20 = (m10 * m21 - m11 * m20) * invDet;
        result.m21 = (m01 * m20 - m00 * m21) * invDet;
        result.m22 = (m00 * m11 - m01 * m10) * invDet;
        
        return true;
    }
    
    [[nodiscard]] Matrix3x3 transpose() const noexcept {
        return Matrix3x3{
            m00, m10, m20,
            m01, m11, m21,
            m02, m12, m22
        };
    }
};

} // namespace matrices
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATRICES_MATRIX3X3_H
