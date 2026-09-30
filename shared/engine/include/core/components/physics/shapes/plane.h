/**
 * @file plane.h
 * @brief Plane collision shape
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_SHAPES_PLANE_H
#define POKO_CORE_COMPONENTS_PHYSICS_SHAPES_PLANE_H

#include "../primitives/shape_type.h"
#include "../math/vectors/vector3.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace shapes {

using math::Vector3;
using primitives::ShapeType;

/**
 * @brief Plane collision shape (infinite)
 */
class Plane {
public:
    Vector3 normal;
    float constant;
    
    constexpr Plane() noexcept : normal(0.0f, 1.0f, 0.0f), constant(0.0f) {}
    constexpr Plane(const Vector3& normal_, float constant_) noexcept : normal(normal_), constant(constant_) {}
    
    /**
     * @brief Get shape type
     */
    [[nodiscard]] static constexpr ShapeType type() noexcept {
        return ShapeType::Plane;
    }
    
    /**
     * @brief Distance from point to plane
     */
    [[nodiscard]] float distanceToPoint(const Vector3& point) const noexcept {
        return normal.dot(point) + constant;
    }
    
    /**
     * @brief Validate shape
     */
    [[nodiscard]] bool isValid() const noexcept {
        return normal.isNormalized(0.001f);
    }
};

} // namespace shapes
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_SHAPES_PLANE_H
