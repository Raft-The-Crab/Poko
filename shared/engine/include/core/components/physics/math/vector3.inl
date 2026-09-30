/**
 * @file vector3.inl
 * @brief Vector3 inline implementations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTOR3_INL
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTOR3_INL

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

// ============================================================================
// Constants
// ============================================================================

inline const Vector3 Vector3::ZERO(0.0f, 0.0f, 0.0f);
inline const Vector3 Vector3::ONE(1.0f, 1.0f, 1.0f);
inline const Vector3 Vector3::UP(0.0f, 1.0f, 0.0f);
inline const Vector3 Vector3::DOWN(0.0f, -1.0f, 0.0f);
inline const Vector3 Vector3::FORWARD(0.0f, 0.0f, 1.0f);
inline const Vector3 Vector3::BACK(0.0f, 0.0f, -1.0f);
inline const Vector3 Vector3::RIGHT(1.0f, 0.0f, 0.0f);
inline const Vector3 Vector3::LEFT(-1.0f, 0.0f, 0.0f);

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_VECTOR3_INL
