/**
 * @file physics.cpp
 * @brief Physics component implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/physics.h"
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {

// ============================================================================
// Physics Implementation
// ============================================================================

Physics::Physics() noexcept
    : m_bodyType(BodyType::Dynamic)
    , m_shapeType(ShapeType::Box)
    , m_sphereRadius(DEFAULT_SPHERE_RADIUS)
    , m_boxHalfExtX(DEFAULT_BOX_HALF_EXTENT)
    , m_boxHalfExtY(DEFAULT_BOX_HALF_EXTENT)
    , m_boxHalfExtZ(DEFAULT_BOX_HALF_EXTENT)
    , m_capsuleRadius(DEFAULT_CAPSULE_RADIUS)
    , m_capsuleHeight(DEFAULT_CAPSULE_HEIGHT)
    , m_material()
    , m_mass(1.0f)
    , m_inertiaXX(1.0f)
    , m_inertiaXY(0.0f)
    , m_inertiaXZ(0.0f)
    , m_inertiaYX(0.0f)
    , m_inertiaYY(1.0f)
    , m_inertiaYZ(0.0f)
    , m_inertiaZX(0.0f)
    , m_inertiaZY(0.0f)
    , m_inertiaZZ(1.0f)
    , m_linearVelocityX(0.0f)
    , m_linearVelocityY(0.0f)
    , m_linearVelocityZ(0.0f)
    , m_angularVelocityX(0.0f)
    , m_angularVelocityY(0.0f)
    , m_angularVelocityZ(0.0f)
    , m_gravityScale(DEFAULT_GRAVITY_SCALE)
    , m_collisionLayer(1)
    , m_collisionMask(0xFFFFFFFF)
    , m_sleepThreshold(DEFAULT_SLEEP_THRESHOLD)
    , m_sleepEnabled(true)
    , m_isSleeping(false)
    , m_linearDamping(DEFAULT_LINEAR_DAMPING)
    , m_angularDamping(DEFAULT_ANGULAR_DAMPING)
    , m_ccdEnabled(false)
    , m_ccdMotionThreshold(DEFAULT_CCD_MOTION_THRESHOLD)
    , m_ccdSweptSphereRadius(0.0f)
    , m_maxLinearVelocity(DEFAULT_MAX_LINEAR_VELOCITY)
    , m_maxAngularVelocity(DEFAULT_MAX_ANGULAR_VELOCITY)
    , m_accumulatedForceX(0.0f)
    , m_accumulatedForceY(0.0f)
    , m_accumulatedForceZ(0.0f)
    , m_accumulatedTorqueX(0.0f)
    , m_accumulatedTorqueY(0.0f)
    , m_accumulatedTorqueZ(0.0f)
{
}

void Physics::onCreate() {
    // Initialize physics resources
    m_state = ComponentState::Created;
}

void Physics::onActivate() {
    // Activate physics simulation
    m_state = ComponentState::Active;
}

void Physics::onDeactivate() {
    // Deactivate physics simulation
    m_state = ComponentState::Deactivating;
}

void Physics::onDestroy() {
    // Cleanup physics resources
    clearAccumulatedForces();
    m_state = ComponentState::Destroyed;
}



} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
