/**
 * @file capsule.h
 * @brief Capsule collision shape
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_SHAPES_CAPSULE_H
#define POKO_CORE_COMPONENTS_PHYSICS_SHAPES_CAPSULE_H

#include "core/components/physics/shapes/primitives/shape_type.h"
#include "core/components/physics/math/vectors/vector3.h"
#include "core/components/physics/bounds/aabb.h"
#include "core/components/physics/matrices/matrix3x3.h"
#include <cmath>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace shapes {

using math::Vector3;
using bounds::AABB;
using primitives::ShapeType;
using matrices::Matrix3x3;

/**
 * @brief Capsule collision shape
 */
class Capsule {
public:
    float radius;
    float height;
    
    constexpr Capsule() noexcept : radius(0.5f), height(1.0f) {}
    constexpr Capsule(float radius_, float height_) noexcept : radius(radius_), height(height_) {}
    
    /**
     * @brief Get shape type
     */
    [[nodiscard]] static constexpr ShapeType type() noexcept {
        return ShapeType::Capsule;
    }
    
    /**
     * @brief Calculate volume
     */
    [[nodiscard]] float volume() const noexcept {
        float cylinderVolume = M_PI * radius * radius * height;
        float sphereVolume = (4.0f / 3.0f) * M_PI * radius * radius * radius;
        return cylinderVolume + sphereVolume;
    }
    
    /**
     * @brief Calculate mass from density
     */
    [[nodiscard]] float calculateMass(float density) const noexcept {
        return volume() * density;
    }
    
    /**
     * @brief Calculate inertia tensor for capsule
     */
    [[nodiscard]] Matrix3x3 calculateInertiaTensor(float mass) const noexcept {
        // Simplified approximation for capsule (cylinder + hemispheres)
        float Iy = (1.0f / 12.0f) * mass * (3.0f * radius * radius + height * height);
        float Ix = Iy + (1.0f / 4.0f) * mass * radius * radius;
        float Iz = Ix;
        return Matrix3x3::diagonal(Ix, Iy, Iz);
    }
    
    /**
     * @brief Get local AABB
     */
    [[nodiscard]] AABB getLocalAABB() const noexcept {
        float halfHeight = height * 0.5f + radius;
        return AABB::fromCenterExtent(
            Vector3::zero(),
            Vector3{radius, halfHeight, radius}
        );
    }
    
    /**
     * @brief Validate shape
     */
    [[nodiscard]] bool isValid() const noexcept {
        return radius > 0.0f && height > 0.0f;
    }

    /**
     * @brief Support mapping for GJK
     */
    [[nodiscard]] Vector3 support(const Vector3& direction) const noexcept {
        Vector3 normalizedDir = direction.normalized();
        // Project direction onto capsule axis
        Vector3 axis(0.0f, 1.0f, 0.0f);
        float axisProjection = normalizedDir.dot(axis);
        float halfHeight = height * 0.5f;

        if (std::abs(axisProjection) > 0.0001f) {
            // Direction has component along axis
            Vector3 axisPoint = axis * (halfHeight * (axisProjection > 0 ? 1.0f : -1.0f));
            Vector3 radialDir = normalizedDir - axis * axisProjection;
            float radialLengthSquared = radialDir.lengthSquared();

            if (radialLengthSquared > 0.0001f) {
                float radialLength = std::sqrt(radialLengthSquared);
                return axisPoint + radialDir * (radius / radialLength);
            } else {
                return axisPoint;
            }
        } else {
            // Direction is purely radial
            return normalizedDir * radius;
        }
    }
};

} // namespace shapes
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_SHAPES_CAPSULE_H
