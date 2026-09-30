/**
 * @file constraint_definition.h
 * @brief Constraint definition
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_CONSTRAINTS_CONSTRAINT_DEFINITION_H
#define POKO_CORE_COMPONENTS_PHYSICS_CONSTRAINTS_CONSTRAINT_DEFINITION_H

#include "../core/handle.h"
#include "../math/vectors/vector3.h"
#include "../math/vectors/quaternion.h"
#include "../transforms/transform.h"
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace constraints {

using core::ConstraintHandle;
using core::BodyHandle;
using math::Vector3;
using math::Quaternion;
using transforms::Transform;

/**
 * @brief Constraint type
 */
enum class ConstraintType : uint32_t {
    Distance,
    Fixed,
    BallSocket,
    Hinge,
    Prismatic,
    Spring,
    Unknown
};

/**
 * @brief Constraint definition
 */
class ConstraintDefinition {
public:
    ConstraintHandle handle;
    ConstraintType type;
    BodyHandle bodyA;
    BodyHandle bodyB;
    Vector3 anchorA;
    Vector3 anchorB;
    Quaternion rotationA;
    Quaternion rotationB;
    float distance;
    bool collideConnected;
    bool enabled;

    /**
     * @brief Constructor
     */
    ConstraintDefinition() noexcept
        : handle()
        , type(ConstraintType::Unknown)
        , bodyA()
        , bodyB()
        , anchorA(0.0f, 0.0f, 0.0f)
        , anchorB(0.0f, 0.0f, 0.0f)
        , rotationA(Quaternion::identity())
        , rotationB(Quaternion::identity())
        , distance(1.0f)
        , collideConnected(false)
        , enabled(true) {}

    /**
     * @brief Check if constraint is valid
     */
    [[nodiscard]] bool isValid() const noexcept {
        return bodyA.isValid() && bodyB.isValid() && type != ConstraintType::Unknown;
    }
};

/**
 * @brief Distance constraint definition
 */
class DistanceConstraintDefinition : public ConstraintDefinition {
public:
    float distance;
    float damping;

    DistanceConstraintDefinition() noexcept
        : ConstraintDefinition()
        , distance(1.0f)
        , damping(0.5f) {
        type = ConstraintType::Distance;
    }

    [[nodiscard]] bool isValid() const noexcept {
        return ConstraintDefinition::isValid() && distance > 0.0f && damping >= 0.0f && damping <= 1.0f;
    }
};

/**
 * @brief Fixed (weld) constraint definition
 */
class FixedConstraintDefinition : public ConstraintDefinition {
public:
    Quaternion referenceRotationA;
    Quaternion referenceRotationB;

    FixedConstraintDefinition() noexcept
        : ConstraintDefinition()
        , referenceRotationA(Quaternion::identity())
        , referenceRotationB(Quaternion::identity()) {
        type = ConstraintType::Fixed;
    }

    [[nodiscard]] bool isValid() const noexcept {
        return ConstraintDefinition::isValid() &&
               std::abs(referenceRotationA.lengthSquared() - 1.0f) < 0.01f &&
               std::abs(referenceRotationB.lengthSquared() - 1.0f) < 0.01f;
    }
};

/**
 * @brief Ball socket constraint definition
 */
class BallSocketConstraintDefinition : public ConstraintDefinition {
public:
    BallSocketConstraintDefinition() noexcept : ConstraintDefinition() {
        type = ConstraintType::BallSocket;
    }

    [[nodiscard]] bool isValid() const noexcept {
        return ConstraintDefinition::isValid();
    }
};

/**
 * @brief Hinge constraint definition
 */
class HingeConstraintDefinition : public ConstraintDefinition {
public:
    Vector3 axis;
    float lowerLimit;
    float upperLimit;

    HingeConstraintDefinition() noexcept
        : ConstraintDefinition()
        , axis(0.0f, 1.0f, 0.0f)
        , lowerLimit(-3.14159f)
        , upperLimit(3.14159f) {
        type = ConstraintType::Hinge;
    }

    [[nodiscard]] bool isValid() const noexcept {
        return ConstraintDefinition::isValid() &&
               axis.lengthSquared() > 0.0001f &&
               lowerLimit <= upperLimit;
    }
};

/**
 * @brief Prismatic (slider) constraint definition
 */
class PrismaticConstraintDefinition : public ConstraintDefinition {
public:
    Vector3 axis;
    float lowerLimit;
    float upperLimit;

    PrismaticConstraintDefinition() noexcept
        : ConstraintDefinition()
        , axis(0.0f, 1.0f, 0.0f)
        , lowerLimit(-100.0f)
        , upperLimit(100.0f) {
        type = ConstraintType::Prismatic;
    }

    [[nodiscard]] bool isValid() const noexcept {
        return ConstraintDefinition::isValid() &&
               axis.lengthSquared() > 0.0001f &&
               lowerLimit <= upperLimit;
    }
};

} // namespace constraints
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_CONSTRAINTS_CONSTRAINT_DEFINITION_H
