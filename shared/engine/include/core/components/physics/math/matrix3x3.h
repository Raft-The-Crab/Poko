/**
 * @file matrix3x3.h
 * @brief Physics-specific 3x3 matrix for inertia tensors
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_MATRIX3X3_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_MATRIX3X3_H

#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

/**
 * @brief Physics-specific 3x3 matrix for inertia tensors
 */
struct Matrix3x3 {
    float m00, m01, m02;
    float m10, m11, m12;
    float m20, m21, m22;
    
    /**
     * @brief Default constructor (identity matrix)
     */
    constexpr Matrix3x3() noexcept
        : m00(1.0f), m01(0.0f), m02(0.0f)
        , m10(0.0f), m11(1.0f), m12(0.0f)
        , m20(0.0f), m21(0.0f), m22(1.0f) {}
    
    /**
     * @brief Diagonal matrix constructor
     */
    constexpr Matrix3x3(float d0, float d1, float d2) noexcept
        : m00(d0), m01(0.0f), m02(0.0f)
        , m10(0.0f), m11(d1), m12(0.0f)
        , m20(0.0f), m21(0.0f), m22(d2) {}
    
    /**
     * @brief Identity matrix constant
     */
    static constexpr Matrix3x3 identity() noexcept { return Matrix3x3(); }
    
    /**
     * @brief Zero matrix constant
     */
    static constexpr Matrix3x3 zero() noexcept {
        return Matrix3x3(0.0f, 0.0f, 0.0f);
    }
    
    /**
     * @brief Diagonal matrix from scalar
     */
    static constexpr Matrix3x3 diagonal(float scalar) noexcept {
        return Matrix3x3(scalar, scalar, scalar);
    }
};

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_MATRIX3X3_H
