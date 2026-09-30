/**
 * @file apply_damping.cpp
 * @brief Apply damping to velocities
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

void Physics::applyDamping(float deltaTime) noexcept {
    // Linear damping: v *= (1 - damping * dt)
    // Clamped to prevent reversing velocity
    float linearDampingFactor = 1.0f - (m_linearDamping * deltaTime);
    if (linearDampingFactor < 0.0f) linearDampingFactor = 0.0f;
    if (linearDampingFactor > 1.0f) linearDampingFactor = 1.0f;
    
    m_linearVelocityX *= linearDampingFactor;
    m_linearVelocityY *= linearDampingFactor;
    m_linearVelocityZ *= linearDampingFactor;
    
    // Angular damping: ω *= (1 - damping * dt)
    // Clamped to prevent reversing angular velocity
    float angularDampingFactor = 1.0f - (m_angularDamping * deltaTime);
    if (angularDampingFactor < 0.0f) angularDampingFactor = 0.0f;
    if (angularDampingFactor > 1.0f) angularDampingFactor = 1.0f;
    
    m_angularVelocityX *= angularDampingFactor;
    m_angularVelocityY *= angularDampingFactor;
    m_angularVelocityZ *= angularDampingFactor;
}

} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
