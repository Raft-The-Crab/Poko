/**
 * @file contact_manifold.cpp
 * @brief Contact manifold implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/contacts/contact_manifold.h"
#include <algorithm>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace contacts {

void ContactManifold::reduceContacts(uint32_t maxContacts) noexcept {
    if (contactCount <= maxContacts) {
        return;
    }
    
    // Simple reduction: keep deepest contacts
    std::sort(contacts.begin(), contacts.begin() + contactCount,
        [](const SolverContact& a, const SolverContact& b) {
            return a.penetration > b.penetration;
        });
    
    contactCount = maxContacts;
}

void ContactManifold::matchContacts(const ContactManifold& previous) noexcept {
    for (uint32_t i = 0; i < contactCount; ++i) {
        for (uint32_t j = 0; j < previous.contactCount; ++j) {
            if (contacts[i].featureIdA == previous.contacts[j].featureIdA &&
                contacts[i].featureIdB == previous.contacts[j].featureIdB) {
                // Transfer impulses for warm starting
                contacts[i].normalImpulse = previous.contacts[j].normalImpulse;
                contacts[i].tangent1Impulse = previous.contacts[j].tangent1Impulse;
                contacts[i].tangent2Impulse = previous.contacts[j].tangent2Impulse;
                break;
            }
        }
    }
}

void ContactManifold::calculateTangentBasis() noexcept {
    if (contactCount == 0) return;
    
    for (uint32_t i = 0; i < contactCount; ++i) {
        Vector3 normal = contacts[i].normal;
        
        // Calculate tangent vectors (orthogonal to normal)
        Vector3 t1;
        if (std::abs(normal.x) > 0.9f) {
            t1 = Vector3{0.0f, 1.0f, 0.0f};
        } else {
            t1 = Vector3{1.0f, 0.0f, 0.0f};
        }
        
        contacts[i].tangent1 = (t1 - normal * normal.dot(t1)).normalized();
        contacts[i].tangent2 = normal.cross(contacts[i].tangent1);
    }
}

} // namespace contacts
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
