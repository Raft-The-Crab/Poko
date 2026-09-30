/**
 * @file set_angular_velocity.cpp
 * @brief Set angular velocity
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

void Physics::setAngularVelocity(float x, float y, float z) noexcept {
    m_angularVelocityX = x;
    m_angularVelocityY = y;
    m_angularVelocityZ = z;
}

} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
