/**
 * @file apply_force_at_point.cpp
 * @brief Apply force at a specific point (generates torque)
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

void Physics::applyForceAtPoint(float fx, float fy, float fz, float px, float py, float pz) noexcept {
    // Apply force at center of mass
    m_accumulatedForceX += fx;
    m_accumulatedForceY += fy;
    m_accumulatedForceZ += fz;
    
    // Calculate torque: τ = r × F
    // Torque = cross product of (point - center) and force
    // Assuming center of mass is at origin (0,0,0) for body space
    float torqueX = py * fz - pz * fy;
    float torqueY = pz * fx - px * fz;
    float torqueZ = px * fy - py * fx;
    
    m_accumulatedTorqueX += torqueX;
    m_accumulatedTorqueY += torqueY;
    m_accumulatedTorqueZ += torqueZ;
}

} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
