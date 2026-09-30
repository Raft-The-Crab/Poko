/**
 * @file set_linear_velocity.cpp
 * @brief Set linear velocity
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

void Physics::setLinearVelocity(float x, float y, float z) noexcept {
    m_linearVelocityX = x;
    m_linearVelocityY = y;
    m_linearVelocityZ = z;
}

} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
