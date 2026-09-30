/**
 * @file clear_accumulated_forces.cpp
 * @brief Clear accumulated forces and torques
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

void Physics::clearAccumulatedForces() noexcept {
    m_accumulatedForceX = 0.0f;
    m_accumulatedForceY = 0.0f;
    m_accumulatedForceZ = 0.0f;
    m_accumulatedTorqueX = 0.0f;
    m_accumulatedTorqueY = 0.0f;
    m_accumulatedTorqueZ = 0.0f;
}

} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
