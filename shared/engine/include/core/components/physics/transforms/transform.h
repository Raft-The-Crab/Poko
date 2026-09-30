/**
 * @file transform.h
 * @brief Transform for rigid bodies
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_TRANSFORMS_TRANSFORM_H
#define POKO_CORE_COMPONENTS_PHYSICS_TRANSFORMS_TRANSFORM_H

#include "core/components/physics/math/vectors/vector3.h"
#include "core/components/physics/math/vectors/quaternion.h"
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace transforms {

using math::Vector3;
using math::Quaternion;

/**
 * @brief Transform (position and rotation)
 */
class Transform {
public:
    Vector3 position;
    Quaternion rotation;
    
    constexpr Transform() noexcept
        : position(0.0f, 0.0f, 0.0f)
        , rotation(Quaternion::identity()) {}
    
    constexpr Transform(const Vector3& position_, const Quaternion& rotation_) noexcept
        : position(position_)
        , rotation(rotation_) {}
    
    static constexpr Transform identity() noexcept {
        return Transform{Vector3::zero(), Quaternion::identity()};
    }
    
    [[nodiscard]] Transform compose(const Transform& other) const noexcept {
        return Transform{
            position + rotation.rotateVector(other.position),
            rotation * other.rotation
        };
    }
    
    [[nodiscard]] Transform inverse() const noexcept {
        Quaternion invRotation = rotation.inverse();
        return Transform{
            invRotation.rotateVector(-position),
            invRotation
        };
    }
    
    [[nodiscard]] Vector3 transformPoint(const Vector3& p) const noexcept {
        return position + rotation.rotateVector(p);
    }
    
    [[nodiscard]] Vector3 transformVector(const Vector3& v) const noexcept {
        return rotation.rotateVector(v);
    }
    
    [[nodiscard]] static Transform lerp(const Transform& a, const Transform& b, float t) noexcept {
        return Transform{
            Vector3::lerp(a.position, b.position, t),
            Quaternion::lerp(a.rotation, b.rotation, t)
        };
    }

    [[nodiscard]] bool isValid() const noexcept {
        return std::abs(rotation.lengthSquared() - 1.0f) < 0.01f;
    }

    [[nodiscard]] Transform withPosition(const Vector3& newPosition) const noexcept {
        return Transform{newPosition, rotation};
    }

    [[nodiscard]] Transform withRotation(const Quaternion& newRotation) const noexcept {
        return Transform{position, newRotation};
    }
};

} // namespace transforms
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_TRANSFORMS_TRANSFORM_H
