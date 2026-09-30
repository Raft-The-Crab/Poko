/**
 * @file integrate_velocity.cpp
 * @brief Semi-implicit Euler velocity integration
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

// Semi-implicit Euler integration (symplectic)
// Update velocity first, then position with new velocity
// This preserves energy better than explicit Euler
void Physics::integrateVelocity(float deltaTime, float gravityX, float gravityY, float gravityZ) noexcept {
    // Only integrate dynamic bodies
    if (m_bodyType != BodyType::Dynamic) return;
    
    // If sleeping, skip integration
    if (m_isSleeping) return;
    
    // Apply gravity (scaled by gravity scale)
    float gravityAccelX = gravityX * m_gravityScale;
    float gravityAccelY = gravityY * m_gravityScale;
    float gravityAccelZ = gravityZ * m_gravityScale;
    
    // Calculate acceleration from accumulated forces: a = F / m
    float forceAccelX = m_accumulatedForceX / m_mass;
    float forceAccelY = m_accumulatedForceY / m_mass;
    float forceAccelZ = m_accumulatedForceZ / m_mass;
    
    // Total acceleration
    float accelX = gravityAccelX + forceAccelX;
    float accelY = gravityAccelY + forceAccelY;
    float accelZ = gravityAccelZ + forceAccelZ;
    
    // Semi-implicit Euler: v_new = v_old + a * dt
    m_linearVelocityX += accelX * deltaTime;
    m_linearVelocityY += accelY * deltaTime;
    m_linearVelocityZ += accelZ * deltaTime;
    
    // Clamp to velocity limits
    clampVelocities();
    
    // Apply damping
    applyDamping(deltaTime);
}

} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
