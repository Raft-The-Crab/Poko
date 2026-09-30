/**
 * @file should_collide.cpp
 * @brief Check if two bodies should collide based on layers/masks
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

bool Physics::shouldCollideWith(uint32_t otherLayer, uint32_t otherMask) const noexcept {
    // Collision occurs if:
    // 1. This body's layer is in the other body's mask
    // 2. The other body's layer is in this body's mask
    // Using bitwise AND to check if bit is set
    bool thisCollidesWithOther = (otherMask & m_collisionLayer) != 0;
    bool otherCollidesWithThis = (m_collisionMask & otherLayer) != 0;
    
    return thisCollidesWithOther && otherCollidesWithThis;
}

} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
