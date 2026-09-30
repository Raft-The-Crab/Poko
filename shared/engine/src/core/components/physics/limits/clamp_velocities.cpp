/**
 * @file clamp_velocities.cpp
 * @brief Clamp velocities to maximum limits
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

void Physics::clampVelocities() noexcept {
    // Clamp linear velocity magnitude
    float linearSpeedSquared = m_linearVelocityX * m_linearVelocityX +
                              m_linearVelocityY * m_linearVelocityY +
                              m_linearVelocityZ * m_linearVelocityZ;
    
    if (linearSpeedSquared > m_maxLinearVelocity * m_maxLinearVelocity) {
        float linearSpeed = std::sqrt(linearSpeedSquared);
        if (linearSpeed > 0.0001f) {
            float scale = m_maxLinearVelocity / linearSpeed;
            m_linearVelocityX *= scale;
            m_linearVelocityY *= scale;
            m_linearVelocityZ *= scale;
        }
    }
    
    // Clamp angular velocity magnitude
    float angularSpeedSquared = m_angularVelocityX * m_angularVelocityX +
                               m_angularVelocityY * m_angularVelocityY +
                               m_angularVelocityZ * m_angularVelocityZ;
    
    if (angularSpeedSquared > m_maxAngularVelocity * m_maxAngularVelocity) {
        float angularSpeed = std::sqrt(angularSpeedSquared);
        if (angularSpeed > 0.0001f) {
            float scale = m_maxAngularVelocity / angularSpeed;
            m_angularVelocityX *= scale;
            m_angularVelocityY *= scale;
            m_angularVelocityZ *= scale;
        }
    }
}

} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
