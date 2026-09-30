/**
 * @file constraint.h
 * @brief Constraint base class
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_SOLVER_CONSTRAINT_H
#define POKO_CORE_COMPONENTS_PHYSICS_SOLVER_CONSTRAINT_H

#include "../math/vector3.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace solver {

/**
 * @brief Constraint type
 */
enum class ConstraintType : uint32_t {
    Contact = 0,
    Distance = 1,
    Hinge = 2,
    Spherical = 3,
    Fixed = 4,
    Slider = 5
};

/**
 * @brief Constraint base class
 */
class Constraint {
public:
    ConstraintType type;
    uint64_t bodyA;
    uint64_t bodyB;
    
    Constraint(ConstraintType type_, uint64_t bodyA_, uint64_t bodyB_) noexcept
        : type(type_)
        , bodyA(bodyA_)
        , bodyB(bodyB_) {}
    
    virtual ~Constraint() = default;
    
    /**
     * @brief Solve constraint
     */
    virtual void solve(float deltaTime) = 0;
};

} // namespace solver
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_SOLVER_CONSTRAINT_H
