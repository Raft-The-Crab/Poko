/**
 * @file set_linear_damping.cpp
 * @brief Set linear damping with clamping
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

void Physics::setLinearDamping(float damping) noexcept {
    m_linearDamping = damping;
    // Clamp to valid range
    if (m_linearDamping < MIN_DAMPING) m_linearDamping = MIN_DAMPING;
    if (m_linearDamping > MAX_DAMPING) m_linearDamping = MAX_DAMPING;
}

} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
