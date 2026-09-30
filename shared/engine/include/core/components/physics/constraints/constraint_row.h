/**
 * @file constraint_row.h
 * @brief Generic constraint solver row
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_CONSTRAINTS_CONSTRAINT_ROW_H
#define POKO_CORE_COMPONENTS_PHYSICS_CONSTRAINTS_CONSTRAINT_ROW_H

#include "core/components/physics/math/vectors/vector3.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace constraints {

using math::Vector3;

/**
 * @brief Generic constraint solver row
 *
 * The solver operates on rows rather than having separate solving code for every joint.
 */
struct ConstraintRow {
    // Jacobians
    Vector3 linearJacobianA;
    Vector3 angularJacobianA;
    Vector3 linearJacobianB;
    Vector3 angularJacobianB;

    // Effective mass
    float effectiveMass;

    // Bias
    float bias;

    // Limits
    float lowerLimit;
    float upperLimit;

    // Accumulated impulse
    float accumulatedImpulse;

    // Flags
    uint32_t flags;

    /**
     * @brief Constructor
     */
    ConstraintRow() noexcept
        : linearJacobianA(0.0f, 0.0f, 0.0f)
        , angularJacobianA(0.0f, 0.0f, 0.0f)
        , linearJacobianB(0.0f, 0.0f, 0.0f)
        , angularJacobianB(0.0f, 0.0f, 0.0f)
        , effectiveMass(0.0f)
        , bias(0.0f)
        , lowerLimit(-1e30f)
        , upperLimit(1e30f)
        , accumulatedImpulse(0.0f)
        , flags(0) {}

    /**
     * @brief Clear row (reset to default)
     */
    void clear() noexcept {
        linearJacobianA = Vector3::zero();
        angularJacobianA = Vector3::zero();
        linearJacobianB = Vector3::zero();
        angularJacobianB = Vector3::zero();
        effectiveMass = 0.0f;
        bias = 0.0f;
        lowerLimit = -1e30f;
        upperLimit = 1e30f;
        accumulatedImpulse = 0.0f;
        flags = 0;
    }

    /**
     * @brief Check if row is valid
     */
    [[nodiscard]] bool isValid() const noexcept {
        return effectiveMass > 0.0f && lowerLimit <= upperLimit;
    }

    /**
     * @brief Check if impulse is within limits
     */
    [[nodiscard]] bool isImpulseClamped() const noexcept {
        return accumulatedImpulse <= lowerLimit || accumulatedImpulse >= upperLimit;
    }

    /**
     * @brief Set as equality constraint (no limits)
     */
    void setEquality() noexcept {
        lowerLimit = -1e30f;
        upperLimit = 1e30f;
    }

    /**
     * @brief Set as inequality constraint (one-sided)
     */
    void setInequality(float minVal = 0.0f) noexcept {
        lowerLimit = minVal;
        upperLimit = 1e30f;
    }

    /**
     * @brief Set as fixed constraint (both limits equal)
     */
    void setFixed(float value = 0.0f) noexcept {
        lowerLimit = value;
        upperLimit = value;
    }
};

} // namespace constraints
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_CONSTRAINTS_CONSTRAINT_ROW_H
