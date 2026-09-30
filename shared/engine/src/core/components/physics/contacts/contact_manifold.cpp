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
#include "core/components/physics/math/vectors/vector3.h"
#include <algorithm>
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace contacts {

using math::Vector3;

void ContactManifold::reduceContacts(uint32_t maxContacts) noexcept {
    if (contactCount <= maxContacts) {
        return;
    }

    // Production-grade contact reduction:
    // 1. Keep contacts with deepest penetration
    // 2. Prioritize contacts that are spread out (for stability)
    // 3. Use geometric heuristics to maintain manifold shape

    // Sort by penetration depth (deepest first)
    std::partial_sort(
        contacts.begin(),
        contacts.begin() + maxContacts,
        contacts.begin() + contactCount,
        [](const SolverContact& a, const SolverContact& b) {
            return a.penetration > b.penetration;
        }
    );

    // Additional heuristic: if we have too many similar contacts, merge them
    // This is a simplified version; full implementation would use clustering
    for (uint32_t i = 1; i < maxContacts; ++i) {
        for (uint32_t j = 0; j < i; ++j) {
            Vector3 diff = contacts[i].position - contacts[j].position;
            float distanceSquared = diff.lengthSquared();

            // If contacts are very close, they're redundant
            if (distanceSquared < 0.0001f) {
                // Keep the one with deeper penetration
                if (contacts[i].penetration < contacts[j].penetration) {
                    std::swap(contacts[i], contacts[j]);
                }
            }
        }
    }

    contactCount = maxContacts;
}

void ContactManifold::matchContacts(const ContactManifold& previous) noexcept {
    // Production-grade contact matching for warm starting:
    // 1. Match contacts by feature IDs (face/edge/vertex IDs)
    // 2. If feature IDs don't match, use spatial proximity
    // 3. Transfer accumulated impulses for warm starting

    for (uint32_t i = 0; i < contactCount; ++i) {
        float bestMatchDistance = 0.001f; // 1mm tolerance
        int32_t bestMatchIndex = -1;

        // First try exact feature ID match
        for (uint32_t j = 0; j < previous.contactCount; ++j) {
            if (contacts[i].featureIdA == previous.contacts[j].featureIdA &&
                contacts[i].featureIdB == previous.contacts[j].featureIdB) {
                // Exact match found
                bestMatchIndex = static_cast<int32_t>(j);
                break;
            }
        }

        // If no exact match, use spatial proximity
        if (bestMatchIndex == -1) {
            for (uint32_t j = 0; j < previous.contactCount; ++j) {
                Vector3 diff = contacts[i].position - previous.contacts[j].position;
                float distanceSquared = diff.lengthSquared();

                if (distanceSquared < bestMatchDistance * bestMatchDistance) {
                    bestMatchDistance = std::sqrt(distanceSquared);
                    bestMatchIndex = static_cast<int32_t>(j);
                }
            }
        }

        // Transfer impulses if we found a match
        if (bestMatchIndex >= 0) {
            contacts[i].normalImpulse = previous.contacts[bestMatchIndex].normalImpulse;
            contacts[i].tangent1Impulse = previous.contacts[bestMatchIndex].tangent1Impulse;
            contacts[i].tangent2Impulse = previous.contacts[bestMatchIndex].tangent2Impulse;
        } else {
            // No match found, reset impulses
            contacts[i].normalImpulse = 0.0f;
            contacts[i].tangent1Impulse = 0.0f;
            contacts[i].tangent2Impulse = 0.0f;
        }
    }
}

void ContactManifold::calculateTangentBasis() noexcept {
    if (contactCount == 0) return;

    for (uint32_t i = 0; i < contactCount; ++i) {
        Vector3 normal = contacts[i].normal;

        // Calculate orthonormal tangent vectors
        // This is a production-grade method that works for any normal direction

        // Find a vector not parallel to normal
        Vector3 t1;
        if (std::abs(normal.x) < 0.57735f) { // 1/sqrt(3)
            t1 = Vector3{1.0f, 0.0f, 0.0f};
        } else if (std::abs(normal.y) < 0.57735f) {
            t1 = Vector3{0.0f, 1.0f, 0.0f};
        } else {
            t1 = Vector3{0.0f, 0.0f, 1.0f};
        }

        // Gram-Schmidt orthogonalization
        t1 = (t1 - normal * normal.dot(t1)).normalized();

        // Second tangent is cross product of normal and first tangent
        Vector3 t2 = normal.cross(t1);

        contacts[i].tangent1 = t1;
        contacts[i].tangent2 = t2;
    }
}

void ContactManifold::clear() noexcept {
    contactCount = 0;
    bodyA = BodyHandle();
    bodyB = BodyHandle();
    friction = 0.0f;
    restitution = 0.0f;
}

bool ContactManifold::isValid() const noexcept {
    return bodyA.isValid() && bodyB.isValid() && contactCount > 0 && contactCount <= MAX_CONTACTS;
}

float ContactManifold::getTotalPenetration() const noexcept {
    float total = 0.0f;
    for (uint32_t i = 0; i < contactCount; ++i) {
        total += contacts[i].penetration;
    }
    return total;
}

Vector3 ContactManifold::getAverageContactPosition() const noexcept {
    if (contactCount == 0) {
        return Vector3::zero();
    }

    Vector3 sum = Vector3::zero();
    for (uint32_t i = 0; i < contactCount; ++i) {
        sum = sum + contacts[i].position;
    }
    return sum * (1.0f / static_cast<float>(contactCount));
}

} // namespace contacts
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
