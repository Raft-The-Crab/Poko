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

#include "constraint.h"
#include "../narrowphase/contact.h"
#include <vector>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace solver {

/**
 * @brief Sequential impulse solver configuration
 */
struct SolverConfig {
    uint32_t velocityIterations = 8;
    uint32_t positionIterations = 3;
    float baumgarte = 0.2f;
    float slop = 0.01f;
    float warmStartFactor = 0.8f;
};

/**
 * @brief Sequential impulse solver
 * 
 * Implements projected Gauss-Seidel iterative solver.
 * Good for stacking stability and performance.
 */
class SequentialImpulse {
public:
    /**
     * @brief Constructor
     */
    SequentialImpulse(const SolverConfig& config = SolverConfig()) noexcept
        : m_config(config) {}
    
    /**
     * @brief Solve constraints
     */
    void solve(
        std::vector<narrowphase::ContactManifold>& manifolds,
        float deltaTime
    );
    
    /**
     * @brief Warm start contacts
     */
    void warmStart(std::vector<narrowphase::ContactManifold>& manifolds);
    
    /**
     * @brief Pre-solve (prepare for solving)
     */
    void preSolve(std::vector<narrowphase::ContactManifold>& manifolds, float deltaTime);
    
    /**
     * @brief Apply velocity iterations
     */
    void solveVelocityConstraints(std::vector<narrowphase::ContactManifold>& manifolds);
    
    /**
     * @brief Apply position iterations
     */
    void solvePositionConstraints(std::vector<narrowphase::ContactManifold>& manifolds);
    
private:
    SolverConfig m_config;
};

} // namespace solver
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_SOLVER_SEQUENTIAL_IMPULSE_H
