/**
 * @file collider_definition.h
 * @brief Collider definition (collider instance)
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_COLLIDERS_COLLIDER_DEFINITION_H
#define POKO_CORE_COMPONENTS_PHYSICS_COLLIDERS_COLLIDER_DEFINITION_H

#include "core/components/physics/shapes/shape_definition.h"
#include "core/components/physics/transforms/transform.h"
#include "core/components/physics/core/handle.h"
#include "core/components/physics/math/vectors/vector3.h"
#include "core/components/physics/math/vectors/quaternion.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace colliders {

using shapes::ShapeDefinition;
using transforms::Transform;
using core::ColliderHandle;
using core::MaterialHandle;
using core::ShapeHandle;
using core::BodyHandle;
using math::Vector3;
using math::Quaternion;

/**
 * @brief Collider definition (instance of a shape)
 *
 * Separates Shape Definition from Collider Instance per architecture.
 * A shape can be reused across many colliders.
 */
class ColliderDefinition {
public:
    ColliderHandle handle;
    BodyHandle bodyHandle;
    ShapeHandle shapeHandle;
    MaterialHandle materialHandle;
    Transform localTransform;
    uint32_t collisionLayer;
    uint32_t collisionMask;
    bool isTrigger;
    bool isSensor;

    /**
     * @brief Constructor
     */
    ColliderDefinition() noexcept
        : handle()
        , bodyHandle()
        , shapeHandle()
        , materialHandle()
        , localTransform()
        , collisionLayer(1)
        , collisionMask(0xFFFFFFFF)
        , isTrigger(false)
        , isSensor(false) {}

    /**
     * @brief Get world transform from body transform
     */
    [[nodiscard]] Transform getWorldTransform(const Transform& bodyTransform) const noexcept {
        return bodyTransform.compose(localTransform);
    }

    /**
     * @brief Check collision with another collider
     */
    [[nodiscard]] bool shouldCollide(const ColliderDefinition& other) const noexcept {
        return (collisionLayer & other.collisionMask) != 0 &&
               (other.collisionLayer & collisionMask) != 0;
    }

    /**
     * @brief Check if collider is valid
     */
    [[nodiscard]] bool isValid() const noexcept {
        return handle.isValid() && bodyHandle.isValid() && shapeHandle.isValid();
    }

    /**
     * @brief Check if collider is active (non-trigger, non-sensor)
     */
    [[nodiscard]] bool isActiveCollider() const noexcept {
        return !isTrigger && !isSensor;
    }

    /**
     * @brief Check if collider participates in physics simulation
     */
    [[nodiscard]] bool isPhysicsCollider() const noexcept {
        return !isTrigger;
    }

    /**
     * @brief Check if collider is a sensor (generates events only)
     */
    [[nodiscard]] bool isSensorOnly() const noexcept {
        return isSensor;
    }

    /**
     * @brief Set collision layer
     */
    void setCollisionLayer(uint32_t layer) noexcept {
        collisionLayer = layer;
    }

    /**
     * @brief Set collision mask
     */
    void setCollisionMask(uint32_t mask) noexcept {
        collisionMask = mask;
    }

    /**
     * @brief Enable collision with a specific layer
     */
    void enableCollisionWith(uint32_t layer) noexcept {
        collisionMask |= (1u << layer);
    }

    /**
     * @brief Disable collision with a specific layer
     */
    void disableCollisionWith(uint32_t layer) noexcept {
        collisionMask &= ~(1u << layer);
    }
};

} // namespace colliders
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_COLLIDERS_COLLIDER_DEFINITION_H
