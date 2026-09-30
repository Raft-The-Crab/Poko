/**
 * @file apply_force.cpp
 * @brief Apply force at center of mass
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

void Physics::applyForce(float fx, float fy, float fz) noexcept {
    m_accumulatedForceX += fx;
    m_accumulatedForceY += fy;
    m_accumulatedForceZ += fz;
}

} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
