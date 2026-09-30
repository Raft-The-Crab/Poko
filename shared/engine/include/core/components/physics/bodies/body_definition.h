/**
 * @file body_definition.h
 * @brief Rigid body definition
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_BODIES_BODY_DEFINITION_H
#define POKO_CORE_COMPONENTS_PHYSICS_BODIES_BODY_DEFINITION_H

#include "core/components/physics/transforms/transform.h"
#include "core/components/physics/math/vectors/vector3.h"
#include "core/components/physics/math/vectors/quaternion.h"
#include "core/components/physics/core/handle.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace bodies {

using transforms::Transform;
using math::Vector3;
using math::Quaternion;
using core::BodyHandle;

/**
 * @brief Motion type
 */
enum class MotionType : uint32_t {
    Static,
    Kinematic,
    Dynamic
};

/**
 * @brief Rigid body definition
 */
class BodyDefinition {
public:
    BodyHandle handle;
    Transform transform;
    Vector3 linearVelocity;
    Vector3 angularVelocity;
    float mass;
    MotionType motionType;
    bool sleepingEnabled;
    float sleepThreshold;
    uint32_t collisionLayer;
    uint32_t collisionMask;
    
    /**
     * @brief Constructor
     */
    BodyDefinition() noexcept
        : handle()
        , transform()
        , linearVelocity(0.0f, 0.0f, 0.0f)
        , angularVelocity(0.0f, 0.0f, 0.0f)
        , mass(1.0f)
        , motionType(MotionType::Dynamic)
        , sleepingEnabled(true)
        , sleepThreshold(0.01f)
        , collisionLayer(1)
        , collisionMask(0xFFFFFFFF) {}
    
    /**
     * @brief Check if body is static
     */
    [[nodiscard]] bool isStatic() const noexcept {
        return motionType == MotionType::Static;
    }
    
    /**
     * @brief Check if body is kinematic
     */
    [[nodiscard]] bool isKinematic() const noexcept {
        return motionType == MotionType::Kinematic;
    }
    
    /**
     * @brief Check if body is dynamic
     */
    [[nodiscard]] bool isDynamic() const noexcept {
        return motionType == MotionType::Dynamic;
    }
    
    /**
     * @brief Check if body should integrate
     */
    [[nodiscard]] bool shouldIntegrate() const noexcept {
        return isDynamic() || isKinematic();
    }
};

} // namespace bodies
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_BODIES_BODY_DEFINITION_H
