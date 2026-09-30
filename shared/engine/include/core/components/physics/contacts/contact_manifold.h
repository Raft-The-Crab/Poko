/**
 * @file contact_manifold.h
 * @brief Contact manifold for solver
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_CONTACTS_CONTACT_MANIFOLD_H
#define POKO_CORE_COMPONENTS_PHYSICS_CONTACTS_CONTACT_MANIFOLD_H

#include "core/components/physics/narrowphase/collision_result.h"
#include "core/components/physics/core/handle.h"
#include "core/components/physics/math/vectors/vector3.h"
#include <vector>
#include <cstdint>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace contacts {

using narrowphase::ContactPoint;
using core::ColliderHandle;
using core::BodyHandle;
using math::Vector3;

constexpr uint32_t MAX_CONTACTS = 4;

/**
 * @brief Contact point with solver data
 */
struct SolverContact {
    Vector3 position;
    Vector3 normal;
    Vector3 tangent1;
    Vector3 tangent2;
    float penetration;
    float normalImpulse;
    float tangent1Impulse;
    float tangent2Impulse;
    float normalMass;
    float tangentMass;
    uint32_t featureIdA;
    uint32_t featureIdB;
    
    SolverContact() noexcept
        : position(0.0f, 0.0f, 0.0f)
        , normal(0.0f, 1.0f, 0.0f)
        , tangent1(0.0f, 0.0f, 0.0f)
        , tangent2(0.0f, 0.0f, 0.0f)
        , penetration(0.0f)
        , normalImpulse(0.0f)
        , tangent1Impulse(0.0f)
        , tangent2Impulse(0.0f)
        , normalMass(0.0f)
        , tangentMass(0.0f)
        , featureIdA(0)
        , featureIdB(0) {}
};

/**
 * @brief Contact manifold
 */
class ContactManifold {
public:
    BodyHandle bodyA;
    BodyHandle bodyB;
    Vector3 normal;
    std::vector<SolverContact> contacts;
    uint32_t contactCount;
    float friction;
    float restitution;

    /**
     * @brief Constructor
     */
    ContactManifold() noexcept
        : bodyA()
        , bodyB()
        , normal(0.0f, 1.0f, 0.0f)
        , contactCount(0)
        , friction(0.3f)
        , restitution(0.0f) {
        contacts.reserve(MAX_CONTACTS);
    }

    /**
     * @brief Clear manifold
     */
    void clear() noexcept;

    /**
     * @brief Add contact
     */
    void addContact(const SolverContact& contact) noexcept {
        if (contactCount < contacts.size()) {
            contacts[contactCount] = contact;
        } else {
            contacts.push_back(contact);
        }
        contactCount++;
    }

    /**
     * @brief Reduce contacts to maximum
     */
    void reduceContacts(uint32_t maxContacts) noexcept;

    /**
     * @brief Match contacts with previous manifold for warm starting
     */
    void matchContacts(const ContactManifold& previous) noexcept;

    /**
     * @brief Calculate tangent basis
     */
    void calculateTangentBasis() noexcept;

    /**
     * @brief Check if manifold is valid
     */
    [[nodiscard]] bool isValid() const noexcept;

    /**
     * @brief Get total penetration
     */
    [[nodiscard]] float getTotalPenetration() const noexcept;

    /**
     * @brief Get average contact position
     */
    [[nodiscard]] Vector3 getAverageContactPosition() const noexcept;
};

} // namespace contacts
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_CONTACTS_CONTACT_MANIFOLD_H
