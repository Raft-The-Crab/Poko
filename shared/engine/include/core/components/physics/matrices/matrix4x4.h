/**
 * @file matrix4x4.h
 * @brief 4x4 matrix for transform calculations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATRICES_MATRIX4X4_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATRICES_MATRIX4X4_H

#include "../math/vectors/vector3.h"
#include "../math/vectors/vector4.h"
#include "../math/vectors/quaternion.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace matrices {

using math::Vector3;
using math::Vector4;
using math::Quaternion;

/**
 * @brief 4x4 matrix (column-major)
 */
class Matrix4x4 {
public:
    float m00, m01, m02, m03;
    float m10, m11, m12, m13;
    float m20, m21, m22, m23;
    float m30, m31, m32, m33;
    
    constexpr Matrix4x4() noexcept
        : m00(1.0f), m01(0.0f), m02(0.0f), m03(0.0f)
        , m10(0.0f), m11(1.0f), m12(0.0f), m13(0.0f)
        , m20(0.0f), m21(0.0f), m22(1.0f), m23(0.0f)
        , m30(0.0f), m31(0.0f), m32(0.0f), m33(1.0f) {}
    
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
    
    static Matrix4x4 translation(const Vector3& t) noexcept {
        return Matrix4x4{
            1.0f, 0.0f, 0.0f, t.x,
            0.0f, 1.0f, 0.0f, t.y,
            0.0f, 0.0f, 1.0f, t.z,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }
    
    static Matrix4x4 scale(const Vector3& s) noexcept {
        return Matrix4x4{
            s.x, 0.0f, 0.0f, 0.0f,
            0.0f, s.y, 0.0f, 0.0f,
            0.0f, 0.0f, s.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
    }
    
    [[nodiscard]] Vector3 transformPoint(const Vector3& p) const noexcept {
        return Vector3{
            m00 * p.x + m01 * p.y + m02 * p.z + m03,
            m10 * p.x + m11 * p.y + m12 * p.z + m13,
            m20 * p.x + m21 * p.y + m22 * p.z + m23
        };
    }
    
    [[nodiscard]] Vector3 transformVector(const Vector3& v) const noexcept {
        return Vector3{
            m00 * v.x + m01 * v.y + m02 * v.z,
            m10 * v.x + m11 * v.y + m12 * v.z,
            m20 * v.x + m21 * v.y + m22 * v.z
        };
    }
    
    [[nodiscard]] Matrix4x4 transpose() const noexcept {
        return Matrix4x4{
            m00, m10, m20, m30,
            m01, m11, m21, m31,
            m02, m12, m22, m32,
            m03, m13, m23, m33
        };
    }
};

} // namespace matrices
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATRICES_MATRIX4X4_H
