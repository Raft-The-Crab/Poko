/**
 * @file transform.h
 * @brief Transform for rigid bodies
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_MATH_TRANSFORM_H
#define POKO_CORE_COMPONENTS_PHYSICS_MATH_TRANSFORM_H

#include "../vectors/vector3.h"
#include "../vectors/quaternion.h"
#include "../matrices/matrix4x4.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace math {

/**
 * @brief Transform (position and rotation)
 * 
 * Optional scale should not be part of rigid-body transform
 * to avoid unnecessary physics complexity.
 */
class Transform {
public:
    Vector3 position;
    Quaternion rotation;
    
    /**
     * @brief Default constructor (identity)
     */
    constexpr Transform() noexcept
        : position(0.0f, 0.0f, 0.0f)
        , rotation(Quaternion::identity()) {}
    
    /**
     * @brief Component constructor
     */
    constexpr Transform(const Vector3& position_, const Quaternion& rotation_) noexcept
        : position(position_)
        , rotation(rotation_) {}
    
    /**
     * @brief Identity transform
     */
    static constexpr Transform identity() noexcept {
        return Transform{Vector3::zero(), Quaternion::identity()};
    }
    
    /**
     * @brief Compose two transforms
     */
    [[nodiscard]] Transform compose(const Transform& other) const noexcept {
        return Transform{
            position + rotation.rotateVector(other.position),
            rotation * other.rotation
        };
    }
    
    /**
     * @brief Inverse transform
     */
    [[nodiscard]] Transform inverse() const noexcept {
        Quaternion invRotation = rotation.inverse();
        return Transform{
            invRotation.rotateVector(-position),
            invRotation
        };
    }
    
    /**
     * @brief Transform point
     */
    [[nodiscard]] Vector3 transformPoint(const Vector3& p) const noexcept {
        return position + rotation.rotateVector(p);
    }
    
    /**
     * @brief Transform vector (no translation)
     */
    [[nodiscard]] Vector3 transformVector(const Vector3& v) const noexcept {
        return rotation.rotateVector(v);
    }
    
    /**
     * @brief Transform direction (no translation, normalized)
     */
    [[nodiscard]] Vector3 transformDirection(const Vector3& d) const noexcept {
        return rotation.rotateVector(d).normalized();
    }
    
    /**
     * @brief Inverse transform point
     */
    [[nodiscard]] Vector3 inverseTransformPoint(const Vector3& p) const noexcept {
        Quaternion invRotation = rotation.inverse();
        return invRotation.rotateVector(p - position);
    }
    
    /**
     * @brief Inverse transform vector
     */
    [[nodiscard]] Vector3 inverseTransformVector(const Vector3& v) const noexcept {
        return rotation.inverse().rotateVector(v);
    }
    
    /**
     * @brief To 4x4 matrix
     */
    [[nodiscard]] Matrix4x4 toMatrix4x4() const noexcept {
        Matrix4x4 result = Matrix4x4::rotation(rotation);
        result.m03 = position.x;
        result.m13 = position.y;
        result.m23 = position.z;
        return result;
    }
    
    /**
     * @brief Interpolate between two transforms
     */
    [[nodiscard]] static Transform lerp(const Transform& a, const Transform& b, float t) noexcept {
        return Transform{
            Vector3::lerp(a.position, b.position, t),
            Quaternion::lerp(a.rotation, b.rotation, t)
        };
    }
    
    /**
     * @brief Spherical linear interpolation
     */
    [[nodiscard]] static Transform slerp(const Transform& a, const Transform& b, float t) noexcept {
        return Transform{
            Vector3::lerp(a.position, b.position, t),
            Quaternion::slerp(a.rotation, b.rotation, t)
        };
    }
};

} // namespace math
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_MATH_TRANSFORM_H
