/**
 * @file set_angular_damping.cpp
 * @brief Set angular damping with clamping
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

void Physics::setAngularDamping(float damping) noexcept {
    m_angularDamping = damping;
    // Clamp to valid range
    if (m_angularDamping < MIN_DAMPING) m_angularDamping = MIN_DAMPING;
    if (m_angularDamping > MAX_DAMPING) m_angularDamping = MAX_DAMPING;
}

} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
