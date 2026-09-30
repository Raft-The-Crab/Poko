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
#include "core/components/physics/matrices/matrix3x3.h"
#include "core/components/physics/core/handle.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace bodies {

using transforms::Transform;
using math::Vector3;
using math::Quaternion;
using matrices::Matrix3x3;
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
    Vector3 position;
    Quaternion rotation;
    Vector3 linearVelocity;
    Vector3 angularVelocity;
    Vector3 force;
    Vector3 torque;
    float mass;
    Matrix3x3 inertiaTensor;
    Matrix3x3 inverseInertiaTensor;
    float linearDamping;
    float angularDamping;
    float gravityScale;
    MotionType motionType;
    bool isAwake;
    bool sleepingEnabled;
    float sleepTime;
    float sleepThreshold;
    uint32_t collisionLayer;
    uint32_t collisionMask;

    /**
     * @brief Constructor
     */
    BodyDefinition() noexcept
        : handle()
        , transform()
        , position(0.0f, 0.0f, 0.0f)
        , rotation(1.0f, 0.0f, 0.0f, 0.0f)
        , linearVelocity(0.0f, 0.0f, 0.0f)
        , angularVelocity(0.0f, 0.0f, 0.0f)
        , force(0.0f, 0.0f, 0.0f)
        , torque(0.0f, 0.0f, 0.0f)
        , mass(1.0f)
        , inertiaTensor(Matrix3x3::identity())
        , inverseInertiaTensor(Matrix3x3::identity())
        , linearDamping(0.01f)
        , angularDamping(0.01f)
        , gravityScale(1.0f)
        , motionType(MotionType::Dynamic)
        , isAwake(true)
        , sleepingEnabled(true)
        , sleepTime(0.0f)
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

    /**
     * @brief Check if body is valid
     */
    [[nodiscard]] bool isValid() const noexcept {
        return handle.isValid() && mass > 0.0f && linearDamping >= 0.0f && angularDamping >= 0.0f;
    }

    /**
     * @brief Check if body is sleeping
     */
    [[nodiscard]] bool isSleeping() const noexcept {
        return !isAwake && sleepingEnabled;
    }

    /**
     * @brief Wake body
     */
    void wake() noexcept {
        isAwake = true;
        sleepTime = 0.0f;
    }

    /**
     * @brief Put body to sleep
     */
    void sleep() noexcept {
        isAwake = false;
        linearVelocity = Vector3::zero();
        angularVelocity = Vector3::zero();
    }

    /**
     * @brief Get kinetic energy
     */
    [[nodiscard]] float getKineticEnergy() const noexcept {
        float linearKE = 0.5f * mass * linearVelocity.lengthSquared();
        float angularKE = 0.5f * angularVelocity.lengthSquared(); // Simplified
        return linearKE + angularKE;
    }

    /**
     * @brief Check if body is moving above sleep threshold
     */
    [[nodiscard]] bool isMoving() const noexcept {
        return linearVelocity.lengthSquared() > sleepThreshold * sleepThreshold ||
               angularVelocity.lengthSquared() > sleepThreshold * sleepThreshold;
    }

    /**
     * @brief Apply force at center of mass
     */
    void applyForce(const Vector3& force_) noexcept {
        force += force_;
    }

    /**
     * @brief Apply torque
     */
    void applyTorque(const Vector3& torque_) noexcept {
        torque += torque_;
    }

    /**
     * @brief Clear accumulated forces
     */
    void clearForces() noexcept {
        force = Vector3::zero();
        torque = Vector3::zero();
    }

    /**
     * @brief Get world position (from transform)
     */
    [[nodiscard]] Vector3 getWorldPosition() const noexcept {
        return transform.position;
    }

    /**
     * @brief Get world rotation (from transform)
     */
    [[nodiscard]] Quaternion getWorldRotation() const noexcept {
        return transform.rotation;
    }

    /**
     * @brief Update transform from position/rotation
     */
    void updateTransform() noexcept {
        transform.position = position;
        transform.rotation = rotation;
    }

    /**
     * @brief Clamp velocities to reasonable limits
     */
    void clampVelocities(float maxLinear, float maxAngular) noexcept {
        float linearSpeed = linearVelocity.length();
        if (linearSpeed > maxLinear) {
            linearVelocity = linearVelocity * (maxLinear / linearSpeed);
        }

        float angularSpeed = angularVelocity.length();
        if (angularSpeed > maxAngular) {
            angularVelocity = angularVelocity * (maxAngular / angularSpeed);
        }
    }

    /**
     * @brief Set inertia tensor for sphere
     */
    void setSphereInertia(float radius) noexcept {
        float i = (2.0f / 5.0f) * mass * radius * radius;
        inertiaTensor = Matrix3x3(
            i, 0.0f, 0.0f,
            0.0f, i, 0.0f,
            0.0f, 0.0f, i
        );
        (void)inertiaTensor.tryInverse(inverseInertiaTensor);
    }

    /**
     * @brief Set inertia tensor for box
     */
    void setBoxInertia(const Vector3& halfExtents) noexcept {
        float ix = (1.0f / 12.0f) * mass * (halfExtents.y * halfExtents.y + halfExtents.z * halfExtents.z);
        float iy = (1.0f / 12.0f) * mass * (halfExtents.x * halfExtents.x + halfExtents.z * halfExtents.z);
        float iz = (1.0f / 12.0f) * mass * (halfExtents.x * halfExtents.x + halfExtents.y * halfExtents.y);
        inertiaTensor = Matrix3x3(
            ix, 0.0f, 0.0f,
            0.0f, iy, 0.0f,
            0.0f, 0.0f, iz
        );
        (void)inertiaTensor.tryInverse(inverseInertiaTensor);
    }

    /**
     * @brief Set inertia tensor for capsule
     */
    void setCapsuleInertia(float radius, float height) noexcept {
        // Capsule inertia: treat as cylinder with hemispherical ends
        float h = height - 2.0f * radius; // Cylinder height
        if (h < 0.0f) h = 0.0f;

        // Cylinder inertia about x and y axes
        float iCylinder = (1.0f / 12.0f) * mass * (3.0f * radius * radius + h * h);
        // Hemisphere inertia
        float iHemisphere = (2.0f / 5.0f) * mass * radius * radius;

        // Approximate combined inertia
        float i = iCylinder + iHemisphere;
        inertiaTensor = Matrix3x3(
            i, 0.0f, 0.0f,
            0.0f, i, 0.0f,
            0.0f, 0.0f, i
        );
        (void)inertiaTensor.tryInverse(inverseInertiaTensor);
    }

    /**
     * @brief Update world-space inverse inertia tensor based on rotation
     */
    void updateWorldInertia() noexcept {
        if (isStatic()) {
            inverseInertiaTensor = Matrix3x3::zero();
            return;
        }

        Matrix3x3 rotationMatrix = Matrix3x3::fromQuaternion(rotation);
        Matrix3x3 worldInertia = rotationMatrix * inertiaTensor * rotationMatrix.transpose();
        (void)worldInertia.tryInverse(inverseInertiaTensor);
    }
};

} // namespace bodies
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_BODIES_BODY_DEFINITION_H
