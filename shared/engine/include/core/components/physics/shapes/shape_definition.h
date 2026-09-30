/**
 * @file shape_definition.h
 * @brief Shape definition (immutable, reusable)
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_SHAPES_SHAPE_DEFINITION_H
#define POKO_CORE_COMPONENTS_PHYSICS_SHAPES_SHAPE_DEFINITION_H

#include "primitives/shape_type.h"
#include "sphere.h"
#include "box.h"
#include "capsule.h"
#include "plane.h"
#include "../core/handle.h"
#include "../bounds/aabb.h"
#include "../matrices/matrix3x3.h"
#include <variant>
#include <memory>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace shapes {

using primitives::ShapeType;
using core::ShapeHandle;
using bounds::AABB;
using matrices::Matrix3x3;

/**
 * @brief Shape definition variant
 */
using ShapeVariant = std::variant<
    Sphere,
    Box,
    Capsule,
    Plane
>;

/**
 * @brief Shape definition (immutable, reusable across colliders)
 */
class ShapeDefinition {
public:
    ShapeHandle handle;
    ShapeType type;
    ShapeVariant shape;
    
    /**
     * @brief Constructor
     */
    template<typename T>
    ShapeDefinition(ShapeHandle handle_, T&& shape_) noexcept
        : handle(handle_)
        , type(T::type())
        , shape(std::forward<T>(shape_)) {}
    
    /**
     * @brief Get local AABB
     */
    [[nodiscard]] AABB getLocalAABB() const noexcept {
        return std::visit([](auto&& s) { return s.getLocalAABB(); }, shape);
    }
    
    /**
     * @brief Calculate mass from density
     */
    [[nodiscard]] float calculateMass(float density) const noexcept {
        return std::visit([density](auto&& s) { return s.calculateMass(density); }, shape);
    }
    
    /**
     * @brief Calculate inertia tensor
     */
    [[nodiscard]] Matrix3x3 calculateInertiaTensor(float mass) const noexcept {
        return std::visit([mass](auto&& s) { return s.calculateInertiaTensor(mass); }, shape);
    }
    
    /**
     * @brief Validate shape
     */
    [[nodiscard]] bool isValid() const noexcept {
        return std::visit([](auto&& s) { return s.isValid(); }, shape);
    }
};

} // namespace shapes
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_SHAPES_SHAPE_DEFINITION_H
