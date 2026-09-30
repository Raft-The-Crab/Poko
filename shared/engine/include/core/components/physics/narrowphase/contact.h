/**
 * @file contact.h
 * @brief Contact manifold and contact point
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_NARROWPHASE_CONTACT_H
#define POKO_CORE_COMPONENTS_PHYSICS_NARROWPHASE_CONTACT_H

#include "../math/vector3.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace narrowphase {

/**
 * @brief Contact point
 */
struct ContactPoint {
    math::Vector3 position;    // Contact position in world space
    math::Vector3 normal;      // Contact normal (from A to B)
    float penetration;         // Penetration depth
    float normalImpulse;       // Accumulated normal impulse
    float tangentImpulse[2];   // Accumulated tangent impulses (friction)
    
    ContactPoint() noexcept
        : penetration(0.0f)
        , normalImpulse(0.0f)
        , tangentImpulse{0.0f, 0.0f} {}
};

/**
 * @brief Contact manifold
 * 
 * Stores up to 4 contact points for a pair of bodies.
 */
struct ContactManifold {
    static constexpr uint32_t MAX_CONTACT_POINTS = 4;
    
    ContactPoint contacts[MAX_CONTACT_POINTS];
    uint32_t contactCount;
    uint64_t bodyA;  // User data for body A
    uint64_t bodyB;  // User data for body B
    
    ContactManifold() noexcept : contactCount(0), bodyA(0), bodyB(0) {}
    
    /**
     * @brief Add a contact point
     */
    void addContact(const ContactPoint& contact) noexcept {
        if (contactCount < MAX_CONTACT_POINTS) {
            contacts[contactCount++] = contact;
        }
    }
    
    /**
     * @brief Clear all contacts
     */
    void clear() noexcept {
        contactCount = 0;
    }
};

} // namespace narrowphase
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_NARROWPHASE_CONTACT_H
