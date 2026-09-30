/**
 * @file apply_torque.cpp
 * @brief Apply torque
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

void Physics::applyTorque(float tx, float ty, float tz) noexcept {
    m_accumulatedTorqueX += tx;
    m_accumulatedTorqueY += ty;
    m_accumulatedTorqueZ += tz;
}

} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
