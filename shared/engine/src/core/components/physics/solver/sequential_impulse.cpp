/**
 * @file sequential_impulse.cpp
 * @brief Sequential impulse solver implementation
 *
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 *
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/solver/sequential_impulse.h"
#include "core/components/physics/math/vectors/vector3.h"
#include "core/components/physics/math/vectors/quaternion.h"
#include "core/components/physics/transforms/transform.h"
#include <algorithm>
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace solver {

using math::Vector3;
using math::Quaternion;
using transforms::Transform;

SequentialImpulseSolver::SequentialImpulseSolver() noexcept {
    settings.velocityIterations = 8;
    settings.positionIterations = 3;
    settings.warmStartFactor = 0.8f;
    settings.slop = 0.01f;
    settings.baumgarte = 0.2f;
    settings.restitutionThreshold = 1.0f;
    settings.maxLinearCorrection = 0.2f;
    settings.maxAngularCorrection = 0.2f;
}

void SequentialImpulseSolver::setSettings(const SequentialImpulseSettings& settings_) noexcept {
    settings = settings_;
}

void SequentialImpulseSolver::solveVelocityConstraints(
    std::vector<ContactManifold>& manifolds,
    std::vector<ConstraintRow>& constraintRows,
    std::vector<BodyDefinition>& bodies
) {
    // Warm start
    warmStart(manifolds, constraintRows);

    // Velocity iterations
    for (uint32_t iter = 0; iter < settings.velocityIterations; ++iter) {
        // Solve contacts
        for (auto& manifold : manifolds) {
            if (!manifold.bodyA.isValid() || !manifold.bodyB.isValid()) {
                continue;
            }

            // Get body indices (in a real implementation, this would be more sophisticated)
            // For now, we skip actual body lookup since the BodyDefinition is not directly accessible
            // In production, we'd have a body array indexed by handle

            for (uint32_t i = 0; i < manifold.contactCount; ++i) {
                // Placeholder for contact solving
                // solveContactConstraint would be called here with actual body references
                (void)i;
            }
        }

        // Solve constraints
        for (auto& row : constraintRows) {
            (void)row;
            // Placeholder for constraint row solving
            // solveConstraintRow would be called here with actual body references
        }
    }

    (void)bodies;
}

void SequentialImpulseSolver::solvePositionConstraints(
    std::vector<ContactManifold>& manifolds,
    std::vector<BodyDefinition>& bodies
) {
    (void)bodies;

    for (uint32_t iter = 0; iter < settings.positionIterations; ++iter) {
        for (auto& manifold : manifolds) {
            for (uint32_t i = 0; i < manifold.contactCount; ++i) {
                auto& contact = manifold.contacts[i];

                // Calculate position correction using Baumgarte stabilization
                // correction = baumgarte * penetration / (invMassA + invMassB)
                // This is a simplified version; full implementation would handle angular corrections

                float penetration = contact.penetration;
                if (penetration > settings.slop) {
                    float correction = settings.baumgarte * (penetration - settings.slop);
                    // Apply correction to positions (would modify body transforms in production)
                    (void)correction;
                }
            }
        }
    }
}

void SequentialImpulseSolver::warmStart(
    std::vector<ContactManifold>& manifolds,
    std::vector<ConstraintRow>& constraintRows
) {
    for (auto& manifold : manifolds) {
        for (uint32_t i = 0; i < manifold.contactCount; ++i) {
            manifold.contacts[i].normalImpulse *= settings.warmStartFactor;
            manifold.contacts[i].tangent1Impulse *= settings.warmStartFactor;
            manifold.contacts[i].tangent2Impulse *= settings.warmStartFactor;
        }
    }

    for (auto& row : constraintRows) {
        row.accumulatedImpulse *= settings.warmStartFactor;
    }
}

void SequentialImpulseSolver::solveContactConstraint(
    ContactManifold& manifold,
    size_t contactIndex,
    BodyDefinition& bodyA,
    BodyDefinition& bodyB
) {
    auto& contact = manifold.contacts[contactIndex];

    // Calculate relative velocity at contact point
    // v_rel = vB + ωB × rB - (vA + ωA × rA)
    // where rA = contactPoint - centerOfMassA, rB = contactPoint - centerOfMassB

    Vector3 rA = contact.position - bodyA.transform.position;
    Vector3 rB = contact.position - bodyB.transform.position;

    Vector3 vA = bodyA.linearVelocity + bodyA.angularVelocity.cross(rA);
    Vector3 vB = bodyB.linearVelocity + bodyB.angularVelocity.cross(rB);
    Vector3 vRel = vB - vA;

    // Calculate normal velocity
    float vNormal = vRel.dot(contact.normal);

    // Separate velocity into normal and tangent components
    Vector3 vNormalVec = contact.normal * vNormal;
    Vector3 vTangent = vRel - vNormalVec;

    // Calculate effective mass for normal direction
    // K = 1/mA + 1/mB + (rA × n) · IA^-1 · (rA × n) + (rB × n) · IB^-1 · (rB × n)
    // effectiveMass = 1 / K

    // Simplified effective mass (assuming uniform mass distribution)
    float invMassA = bodyA.mass > 0.0f ? 1.0f / bodyA.mass : 0.0f;
    float invMassB = bodyB.mass > 0.0f ? 1.0f / bodyB.mass : 0.0f;
    float effectiveMass = 1.0f / (invMassA + invMassB);

    // Calculate normal impulse
    // j = -effectiveMass * (vNormal + restitution * vNormal_initial)
    // Clamp accumulated impulse to be >= 0

    float restitution = manifold.restitution;
    if (vNormal > -settings.restitutionThreshold) {
        restitution = 0.0f;
    }

    float j = -effectiveMass * (vNormal + restitution * vNormal);
    float oldImpulse = contact.normalImpulse;
    contact.normalImpulse = std::max(0.0f, oldImpulse + j);
    j = contact.normalImpulse - oldImpulse;

    // Apply normal impulse
    Vector3 impulse = contact.normal * j;
    bodyA.linearVelocity = bodyA.linearVelocity - impulse * invMassA;
    bodyB.linearVelocity = bodyB.linearVelocity + impulse * invMassB;

    // Friction (simplified Coulomb friction)
    // Calculate tangent impulse based on vTangent
    float tangentSpeed = vTangent.length();
    if (tangentSpeed > 0.0001f) {
        Vector3 tangentDir = vTangent.normalized();
        float jt = -effectiveMass * tangentSpeed;

        // Clamp friction impulse
        float maxFriction = manifold.friction * contact.normalImpulse;
        jt = std::max(-maxFriction, std::min(maxFriction, jt));

        Vector3 frictionImpulse = tangentDir * jt;
        bodyA.linearVelocity = bodyA.linearVelocity - frictionImpulse * invMassA;
        bodyB.linearVelocity = bodyB.linearVelocity + frictionImpulse * invMassB;
    }

    (void)manifold;
    (void)contactIndex;
}

void SequentialImpulseSolver::solveConstraintRow(
    ConstraintRow& row,
    BodyDefinition& bodyA,
    BodyDefinition& bodyB
) {
    // Calculate relative velocity
    // v_rel = J * v

    float invMassA = bodyA.mass > 0.0f ? 1.0f / bodyA.mass : 0.0f;
    float invMassB = bodyB.mass > 0.0f ? 1.0f / bodyB.mass : 0.0f;

    // Simplified Jacobian velocity
    float vRel = bodyA.linearVelocity.dot(row.linearJacobianA) +
                 bodyB.linearVelocity.dot(row.linearJacobianB);

    // Calculate effective mass
    // K = J * M^-1 * J^T
    float K = invMassA * row.linearJacobianA.lengthSquared() +
              invMassB * row.linearJacobianB.lengthSquared();
    float effectiveMass = K > 0.0001f ? 1.0f / K : 0.0f;

    // Calculate bias (position error correction)
    float bias = row.bias * settings.baumgarte;

    // Solve for impulse
    float j = -effectiveMass * (vRel + bias);

    // Clamp impulse
    float oldImpulse = row.accumulatedImpulse;
    row.accumulatedImpulse = std::max(row.lowerLimit, std::min(row.upperLimit, oldImpulse + j));
    j = row.accumulatedImpulse - oldImpulse;

    // Apply impulse
    bodyA.linearVelocity += row.linearJacobianA * (j * invMassA);
    bodyB.linearVelocity += row.linearJacobianB * (j * invMassB);
}

void SequentialImpulseSolver::resetImpulses(std::vector<ContactManifold>& manifolds) noexcept {
    for (auto& manifold : manifolds) {
        for (auto& contact : manifold.contacts) {
            contact.normalImpulse = 0.0f;
            contact.tangent1Impulse = 0.0f;
            contact.tangent2Impulse = 0.0f;
        }
    }
}

} // namespace solver
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
