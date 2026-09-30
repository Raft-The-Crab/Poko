/**
 * @file sphere.h
 * @brief Sphere collision shape
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_SHAPES_SPHERE_H
#define POKO_CORE_COMPONENTS_PHYSICS_SHAPES_SPHERE_H

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
 * @brief Sphere collision shape
 */
class Sphere {
public:
    float radius;
    
    constexpr Sphere() noexcept : radius(1.0f) {}
    constexpr explicit Sphere(float radius_) noexcept : radius(radius_) {}
    
    /**
     * @brief Get shape type
     */
    [[nodiscard]] static constexpr ShapeType type() noexcept {
        return ShapeType::Sphere;
    }
    
    /**
     * @brief Calculate volume
     */
    [[nodiscard]] float volume() const noexcept {
        return (4.0f / 3.0f) * M_PI * radius * radius * radius;
    }
    
    /**
     * @brief Calculate mass from density
     */
    [[nodiscard]] float calculateMass(float density) const noexcept {
        return volume() * density;
    }
    
    /**
     * @brief Calculate inertia tensor for sphere
     */
    [[nodiscard]] Matrix3x3 calculateInertiaTensor(float mass) const noexcept {
        float I = (2.0f / 5.0f) * mass * radius * radius;
        return Matrix3x3::diagonal(I, I, I);
    }
    
    /**
     * @brief Get local AABB
     */
    [[nodiscard]] AABB getLocalAABB() const noexcept {
        return AABB::fromCenterExtent(Vector3::zero(), Vector3{radius, radius, radius});
    }
    
    /**
     * @brief Support mapping for GJK
     */
    [[nodiscard]] Vector3 support(const Vector3& direction) const noexcept {
        float lengthSquared = direction.lengthSquared();
        if (lengthSquared > 0.0001f) {
            float invLength = 1.0f / std::sqrt(lengthSquared);
            return direction * (radius * invLength);
        }
        return Vector3{radius, 0.0f, 0.0f};
    }
    
    /**
     * @brief Validate shape
     */
    [[nodiscard]] bool isValid() const noexcept {
        return radius > 0.0f && std::isfinite(radius);
    }
};

} // namespace shapes
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_SHAPES_SPHERE_H
