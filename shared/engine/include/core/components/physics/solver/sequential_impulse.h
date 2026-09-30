/**
 * @file sequential_impulse.h
 * @brief Sequential impulse constraint solver
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_SOLVER_SEQUENTIAL_IMPULSE_H
#define POKO_CORE_COMPONENTS_PHYSICS_SOLVER_SEQUENTIAL_IMPULSE_H

#include "core/components/physics/contacts/contact_manifold.h"
#include "core/components/physics/constraints/constraint_row.h"
#include "core/components/physics/bodies/body_definition.h"
#include <vector>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace solver {

using contacts::ContactManifold;
using constraints::ConstraintRow;
using bodies::BodyDefinition;

/**
 * @brief Sequential impulse solver settings
 */
struct SequentialImpulseSettings {
    uint32_t velocityIterations;
    uint32_t positionIterations;
    float warmStartFactor;
    float slop;
    float baumgarte;
    float restitutionThreshold;
    
    SequentialImpulseSettings() noexcept
        : velocityIterations(8)
        , positionIterations(3)
        , warmStartFactor(0.8f)
        , slop(0.01f)
        , baumgarte(0.2f)
        , restitutionThreshold(1.0f) {}
};

/**
 * @brief Sequential impulse solver
 */
class SequentialImpulseSolver {
public:
    /**
     * @brief Constructor
     */
    SequentialImpulseSolver() noexcept;
    
    /**
     * @brief Set settings
     */
    void setSettings(const SequentialImpulseSettings& settings) noexcept;
    
    /**
     * @brief Solve velocity constraints
     */
    void solveVelocityConstraints(
        std::vector<ContactManifold>& manifolds,
        std::vector<ConstraintRow>& constraintRows,
        std::vector<BodyDefinition>& bodies
    );
    
    /**
     * @brief Solve position constraints
     */
    void solvePositionConstraints(
        std::vector<ContactManifold>& manifolds,
        std::vector<BodyDefinition>& bodies
    );
    
    /**
     * @brief Warm start from previous impulses
     */
    void warmStart(
        std::vector<ContactManifold>& manifolds,
        std::vector<ConstraintRow>& constraintRows
    );
    
private:
    SequentialImpulseSettings settings;
    
    /**
     * @brief Solve contact constraint
     */
    void solveContactConstraint(
        ContactManifold& manifold,
        size_t contactIndex,
        BodyDefinition& bodyA,
        BodyDefinition& bodyB
    );
    
    /**
     * @brief Solve constraint row
     */
    void solveConstraintRow(
        ConstraintRow& row,
        BodyDefinition& bodyA,
        BodyDefinition& bodyB
    );
};

} // namespace solver
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_SOLVER_SEQUENTIAL_IMPULSE_H
