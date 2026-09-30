/**
 * @file box.h
 * @brief Box collision shape
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_SHAPES_BOX_H
#define POKO_CORE_COMPONENTS_PHYSICS_SHAPES_BOX_H

#include "core/components/physics/shapes/primitives/shape_type.h"
#include "core/components/physics/math/vectors/vector3.h"
#include "core/components/physics/bounds/aabb.h"
#include "core/components/physics/matrices/matrix3x3.h"

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
 * @brief Box collision shape (half-extents)
 */
class Box {
public:
    Vector3 halfExtents;
    
    constexpr Box() noexcept : halfExtents(0.5f, 0.5f, 0.5f) {}
    constexpr explicit Box(const Vector3& halfExtents_) noexcept : halfExtents(halfExtents_) {}
    
    /**
     * @brief Get shape type
     */
    [[nodiscard]] static constexpr ShapeType type() noexcept {
        return ShapeType::Box;
    }
    
    /**
     * @brief Calculate volume
     */
    [[nodiscard]] float volume() const noexcept {
        return 8.0f * halfExtents.x * halfExtents.y * halfExtents.z;
    }
    
    /**
     * @brief Calculate mass from density
     */
    [[nodiscard]] float calculateMass(float density) const noexcept {
        return volume() * density;
    }
    
    /**
     * @brief Calculate inertia tensor for box
     */
    [[nodiscard]] Matrix3x3 calculateInertiaTensor(float mass) const noexcept {
        float m = mass / 12.0f;
        float y2 = halfExtents.y * halfExtents.y;
        float z2 = halfExtents.z * halfExtents.z;
        float x2 = halfExtents.x * halfExtents.x;
        
        return Matrix3x3::diagonal(
            m * (y2 + z2),
            m * (x2 + z2),
            m * (x2 + y2)
        );
    }
    
    /**
     * @brief Get local AABB
     */
    [[nodiscard]] AABB getLocalAABB() const noexcept {
        return AABB::fromCenterExtent(Vector3::zero(), halfExtents);
    }

    /**
     * @brief Validate shape
     */
    [[nodiscard]] bool isValid() const noexcept {
        return halfExtents.x > 0.0f && halfExtents.y > 0.0f && halfExtents.z > 0.0f;
    }

    /**
     * @brief Get surface area
     */
    [[nodiscard]] float surfaceArea() const noexcept {
        Vector3 s = halfExtents * 2.0f;
        return 2.0f * (s.x * s.y + s.y * s.z + s.z * s.x);
    }

    /**
     * @brief Support mapping for GJK
     */
    [[nodiscard]] Vector3 support(const Vector3& direction) const noexcept {
        return Vector3{
            direction.x > 0 ? halfExtents.x : -halfExtents.x,
            direction.y > 0 ? halfExtents.y : -halfExtents.y,
            direction.z > 0 ? halfExtents.z : -halfExtents.z
        };
    }
};

} // namespace shapes
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_SHAPES_BOX_H
