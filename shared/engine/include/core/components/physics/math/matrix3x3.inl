/**
 * @file matrix3x3.inl
 * @brief Matrix3x3 inline implementations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_MATRIX3X3_INL
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_MATRIX3X3_INL

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

// ============================================================================
// Constants
// ============================================================================

inline const Matrix3x3 Matrix3x3::ZERO(
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f
);

inline const Matrix3x3 Matrix3x3::IDENTITY(
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
);

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_MATRIX3X3_INL
