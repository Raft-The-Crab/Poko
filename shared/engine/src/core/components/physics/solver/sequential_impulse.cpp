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

            // Find body indices
            size_t bodyAIndex = manifold.bodyA.index;
            size_t bodyBIndex = manifold.bodyB.index;

            if (bodyAIndex >= bodies.size() || bodyBIndex >= bodies.size()) {
                continue;
            }

            BodyDefinition& bodyA = bodies[bodyAIndex];
            BodyDefinition& bodyB = bodies[bodyBIndex];

            for (uint32_t i = 0; i < manifold.contactCount; ++i) {
                solveContactConstraint(manifold, i, bodyA, bodyB);
            }
        }

        // Solve constraints
        for (auto& row : constraintRows) {
            // Find body indices from constraint row (would need constraint-to-body mapping)
            // For now, skip since we don't have body references in constraint rows
            (void)row;
        }
    }
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

    float invMassA = bodyA.mass > 0.0f ? 1.0f / bodyA.mass : 0.0f;
    float invMassB = bodyB.mass > 0.0f ? 1.0f / bodyB.mass : 0.0f;

    // Angular contribution to effective mass
    // Simplified: treat inertia as scalar (I = 2/5 * m * r^2 for sphere)
    // In production, this would use the full 3x3 inverse inertia tensor
    float inertiaA = bodyA.mass > 0.0f ? (0.4f * bodyA.mass * 0.1f * 0.1f) : 0.0f; // Assume 0.1m radius
    float inertiaB = bodyB.mass > 0.0f ? (0.4f * bodyB.mass * 0.1f * 0.1f) : 0.0f;
    float invInertiaA = inertiaA > 0.0f ? 1.0f / inertiaA : 0.0f;
    float invInertiaB = inertiaB > 0.0f ? 1.0f / inertiaB : 0.0f;

    Vector3 rA_cross_n = rA.cross(contact.normal);
    Vector3 rB_cross_n = rB.cross(contact.normal);

    float angularTermA = rA_cross_n.lengthSquared() * invInertiaA;
    float angularTermB = rB_cross_n.lengthSquared() * invInertiaB;

    float effectiveMass = 1.0f / (invMassA + invMassB + angularTermA + angularTermB);

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

    // Apply normal impulse (linear and angular)
    Vector3 impulse = contact.normal * j;
    bodyA.linearVelocity = bodyA.linearVelocity - impulse * invMassA;
    bodyB.linearVelocity = bodyB.linearVelocity + impulse * invMassB;

    // Apply angular impulse: τ = r × F
    Vector3 angularImpulseA = rA.cross(impulse * -1.0f);
    Vector3 angularImpulseB = rB.cross(impulse);
    bodyA.angularVelocity = bodyA.angularVelocity + angularImpulseA * invInertiaA;
    bodyB.angularVelocity = bodyB.angularVelocity + angularImpulseB * invInertiaB;

    // Friction (Coulomb friction with two tangent directions)
    float tangentSpeed = vTangent.length();
    if (tangentSpeed > 0.0001f) {
        // Build tangent basis
        Vector3 tangent1 = vTangent.normalized();
        Vector3 tangent2 = contact.normal.cross(tangent1);
        if (tangent2.lengthSquared() < 0.0001f) {
            tangent2 = Vector3(0.0f, 1.0f, 0.0f).cross(tangent1);
        }
        tangent2 = tangent2.normalized();

        // Calculate effective mass for tangent directions
        Vector3 rA_cross_t1 = rA.cross(tangent1);
        Vector3 rB_cross_t1 = rB.cross(tangent1);
        float angularTermA_t1 = rA_cross_t1.lengthSquared() * invInertiaA;
        float angularTermB_t1 = rB_cross_t1.lengthSquared() * invInertiaB;
        float effectiveMassT1 = 1.0f / (invMassA + invMassB + angularTermA_t1 + angularTermB_t1);

        Vector3 rA_cross_t2 = rA.cross(tangent2);
        Vector3 rB_cross_t2 = rB.cross(tangent2);
        float angularTermA_t2 = rA_cross_t2.lengthSquared() * invInertiaA;
        float angularTermB_t2 = rB_cross_t2.lengthSquared() * invInertiaB;
        float effectiveMassT2 = 1.0f / (invMassA + invMassB + angularTermA_t2 + angularTermB_t2);

        // Calculate tangent impulses
        float jt1 = -effectiveMassT1 * vRel.dot(tangent1);
        float jt2 = -effectiveMassT2 * vRel.dot(tangent2);

        // Clamp friction impulses
        float maxFriction = manifold.friction * contact.normalImpulse;
        float frictionMagnitude = std::sqrt(jt1 * jt1 + jt2 * jt2);
        if (frictionMagnitude > maxFriction) {
            float scale = maxFriction / frictionMagnitude;
            jt1 *= scale;
            jt2 *= scale;
        }

        // Apply friction impulses
        Vector3 frictionImpulse = tangent1 * jt1 + tangent2 * jt2;
        bodyA.linearVelocity = bodyA.linearVelocity - frictionImpulse * invMassA;
        bodyB.linearVelocity = bodyB.linearVelocity + frictionImpulse * invMassB;

        // Apply angular friction impulses
        Vector3 angularFrictionA = rA.cross(frictionImpulse * -1.0f);
        Vector3 angularFrictionB = rB.cross(frictionImpulse);
        bodyA.angularVelocity = bodyA.angularVelocity + angularFrictionA * invInertiaA;
        bodyB.angularVelocity = bodyB.angularVelocity + angularFrictionB * invInertiaB;

        // Store tangent impulses for warm starting
        contact.tangent1Impulse = jt1;
        contact.tangent2Impulse = jt2;
    }
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

void SequentialImpulseSolver::setupDistanceConstraint(
    ConstraintRow& row,
    const Vector3& anchorA,
    const Vector3& anchorB,
    float distance,
    BodyDefinition& bodyA,
    BodyDefinition& bodyB
) noexcept {
    // Calculate world-space anchor points
    Vector3 worldAnchorA = bodyA.transform.position + bodyA.transform.rotation.rotateVector(anchorA);
    Vector3 worldAnchorB = bodyB.transform.position + bodyB.transform.rotation.rotateVector(anchorB);

    // Calculate direction from A to B
    Vector3 direction = worldAnchorB - worldAnchorA;
    float currentDistance = direction.length();

    if (currentDistance < 0.0001f) {
        direction = Vector3(0.0f, 1.0f, 0.0f);
        currentDistance = 0.0f;
    } else {
        direction = direction / currentDistance;
    }

    // Calculate position error
    float error = currentDistance - distance;

    // Setup Jacobian (constraint acts along direction)
    row.linearJacobianA = direction;
    row.linearJacobianB = -direction;
    row.angularJacobianA = Vector3::zero();
    row.angularJacobianB = Vector3::zero();

    // Set limits (distance constraint is one-sided: distance >= 0)
    row.lowerLimit = 0.0f;
    row.upperLimit = 1e30f;

    // Set bias for position correction
    row.bias = error * settings.baumgarte;
}

void SequentialImpulseSolver::setupFixedConstraint(
    ConstraintRow& row,
    const Vector3& anchorA,
    const Vector3& anchorB,
    const Quaternion& rotationA,
    const Quaternion& rotationB,
    BodyDefinition& bodyA,
    BodyDefinition& bodyB
) noexcept {
    // Calculate world-space anchor points
    Vector3 worldAnchorA = bodyA.transform.position + bodyA.transform.rotation.rotateVector(anchorA);
    Vector3 worldAnchorB = bodyB.transform.position + bodyB.transform.rotation.rotateVector(anchorB);

    // Calculate position error
    Vector3 positionError = worldAnchorB - worldAnchorA;

    // Calculate rotation error
    Quaternion relativeRotation = bodyB.transform.rotation * rotationB * (bodyA.transform.rotation * rotationA).inverse();
    Vector3 rotationError = Vector3(relativeRotation.x, relativeRotation.y, relativeRotation.z) * 2.0f;

    // Setup Jacobian for position constraint (simplified to 1 DOF for now)
    row.linearJacobianA = Vector3(1.0f, 0.0f, 0.0f);
    row.linearJacobianB = Vector3(-1.0f, 0.0f, 0.0f);
    row.angularJacobianA = Vector3::zero();
    row.angularJacobianB = Vector3::zero();

    // Set limits (fixed constraint is equality)
    row.lowerLimit = -1e30f;
    row.upperLimit = 1e30f;

    // Set bias for position correction
    row.bias = (positionError.length() + rotationError.length()) * settings.baumgarte;
}

void SequentialImpulseSolver::setupBallSocketConstraint(
    ConstraintRow& row,
    const Vector3& anchorA,
    const Vector3& anchorB,
    BodyDefinition& bodyA,
    BodyDefinition& bodyB
) noexcept {
    // Calculate world-space anchor points
    Vector3 worldAnchorA = bodyA.transform.position + bodyA.transform.rotation.rotateVector(anchorA);
    Vector3 worldAnchorB = bodyB.transform.position + bodyB.transform.rotation.rotateVector(anchorB);

    // Calculate direction from A to B
    Vector3 direction = worldAnchorB - worldAnchorA;
    float distance = direction.length();

    if (distance < 0.0001f) {
        direction = Vector3(0.0f, 1.0f, 0.0f);
    } else {
        direction = direction / distance;
    }

    // Setup Jacobian (ball socket allows rotation, constrains position)
    row.linearJacobianA = direction;
    row.linearJacobianB = -direction;
    row.angularJacobianA = Vector3::zero();
    row.angularJacobianB = Vector3::zero();

    // Set limits (ball socket is one-sided: distance >= 0)
    row.lowerLimit = 0.0f;
    row.upperLimit = 1e30f;

    // Set bias for position correction
    row.bias = distance * settings.baumgarte;
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
