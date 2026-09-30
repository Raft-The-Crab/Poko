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

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace constraints {

using core::ConstraintHandle;
using core::BodyHandle;
using math::Vector3;
using math::Quaternion;

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
        , collideConnected(false)
        , enabled(true) {}
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
};

/**
 * @brief Ball socket constraint definition
 */
class BallSocketConstraintDefinition : public ConstraintDefinition {
public:
    BallSocketConstraintDefinition() noexcept : ConstraintDefinition() {
        type = ConstraintType::BallSocket;
    }
};

} // namespace constraints
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_CONSTRAINTS_CONSTRAINT_DEFINITION_H
