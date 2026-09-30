/**
 * @file dual_quaternion.h
 * @brief Dual quaternion for rigid body transforms
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_DUAL_QUATERNION_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_DUAL_QUATERNION_H

#include "quaternion.h"
#include "vector3.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

/**
 * @brief Dual quaternion for rigid body transforms
 * 
 * Represents rotation and translation in a single quaternion pair.
 * Useful for skinning and character animation.
 */
class DualQuaternion {
public:
    Quaternion real;  // Rotation
    Quaternion dual;  // Translation (encoded)
    
    /**
     * @brief Default constructor (identity)
     */
    constexpr DualQuaternion() noexcept
        : real(Quaternion::identity())
        , dual(Quaternion::zero()) {}
    
    /**
     * @brief Component constructor
     */
    constexpr DualQuaternion(const Quaternion& real_, const Quaternion& dual_) noexcept
        : real(real_)
        , dual(dual_) {}
    
    /**
     * @brief From transform
     */
    static DualQuaternion fromTransform(const Vector3& position, const Quaternion& rotation) noexcept {
        Quaternion q = rotation.normalized();
        Quaternion d = Quaternion::zero();
        d.x = 0.5f * (position.x * q.w + position.y * q.z - position.z * q.y);
        d.y = 0.5f * (position.y * q.w + position.z * q.x - position.x * q.z);
        d.z = 0.5f * (position.z * q.w + position.x * q.y - position.y * q.x);
        d.w = -0.5f * (position.x * q.x + position.y * q.y + position.z * q.z);
        return DualQuaternion{q, d};
    }
    
    /**
     * @brief Identity
     */
    static constexpr DualQuaternion identity() noexcept {
        return DualQuaternion{};
    }
    
    /**
     * @brief Dual quaternion multiplication
     */
    [[nodiscard]] DualQuaternion operator*(const DualQuaternion& other) const noexcept {
        return DualQuaternion{
            real * other.real,
            real * other.dual + dual * other.real
        };
    }
    
    /**
     * @brief Conjugate
     */
    [[nodiscard]] DualQuaternion conjugate() const noexcept {
        return DualQuaternion{real.conjugate(), dual.conjugate()};
    }
    
    /**
     * @brief Normalize
     */
    [[nodiscard]] DualQuaternion normalized() const noexcept {
        float len = real.length();
        if (len > 0.0001f) {
            return DualQuaternion{real / len, dual / len};
        }
        return identity();
    }
    
    /**
     * @brief Extract position
     */
    [[nodiscard]] Vector3 getPosition() const noexcept {
        Quaternion t = dual * real.conjugate();
        return Vector3{2.0f * t.x, 2.0f * t.y, 2.0f * t.z};
    }
    
    /**
     * @brief Extract rotation
     */
    [[nodiscard]] Quaternion getRotation() const noexcept {
        return real;
    }
};

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_DUAL_QUATERNION_H
