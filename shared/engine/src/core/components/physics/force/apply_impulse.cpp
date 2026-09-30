/**
 * @file apply_impulse.cpp
 * @brief Apply impulse at center of mass
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

void Physics::applyImpulse(float ix, float iy, float iz) noexcept {
    // Impulse is instantaneous change in velocity
    // Apply as if it were force for accumulation
    m_accumulatedForceX += ix;
    m_accumulatedForceY += iy;
    m_accumulatedForceZ += iz;
}

} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
