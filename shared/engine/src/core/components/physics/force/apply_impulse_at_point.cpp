/**
 * @file apply_impulse_at_point.cpp
 * @brief Apply impulse at a specific point (generates angular impulse)
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/physics.h"

namespace poko {
namespace core {
namespace components {
namespace physics {

void Physics::applyImpulseAtPoint(float ix, float iy, float iz, float px, float py, float pz) noexcept {
    // Apply impulse as force for now (will be applied to velocity in integration)
    m_accumulatedForceX += ix;
    m_accumulatedForceY += iy;
    m_accumulatedForceZ += iz;
    
    // Calculate angular impulse: τ = r × J
    // Angular impulse = cross product of (point - center) and impulse
    float angularImpulseX = py * iz - pz * iy;
    float angularImpulseY = pz * ix - px * iz;
    float angularImpulseZ = px * iy - py * ix;
    
    m_accumulatedTorqueX += angularImpulseX;
    m_accumulatedTorqueY += angularImpulseY;
    m_accumulatedTorqueZ += angularImpulseZ;
}

} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
