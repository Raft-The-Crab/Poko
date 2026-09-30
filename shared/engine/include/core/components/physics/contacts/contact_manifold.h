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

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace contacts {

using narrowphase::ContactPoint;
using core::ColliderHandle;
using math::Vector3;

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
    ColliderHandle colliderA;
    ColliderHandle colliderB;
    Vector3 normal;
    std::vector<SolverContact> contacts;
    uint32_t contactCount;
    
    /**
     * @brief Constructor
     */
    ContactManifold() noexcept
        : colliderA()
        , colliderB()
        , normal(0.0f, 1.0f, 0.0f)
        , contactCount(0) {}
    
    /**
     * @brief Clear manifold
     */
    void clear() noexcept {
        contacts.clear();
        contactCount = 0;
    }
    
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
};

} // namespace contacts
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_CONTACTS_CONTACT_MANIFOLD_H
