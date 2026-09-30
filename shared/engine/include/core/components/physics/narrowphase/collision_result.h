/**
 * @file collision_result.h
 * @brief Collision result for narrowphase
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_NARROWPHASE_COLLISION_RESULT_H
#define POKO_CORE_COMPONENTS_PHYSICS_NARROWPHASE_COLLISION_RESULT_H

#include "core/components/physics/math/vectors/vector3.h"
#include <cstdint>
#include <vector>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace narrowphase {

using math::Vector3;

/**
 * @brief Contact point
 */
struct ContactPoint {
    Vector3 position;
    Vector3 normal;
    float penetration;
    uint32_t featureIdA;
    uint32_t featureIdB;

    ContactPoint() noexcept
        : position(0.0f, 0.0f, 0.0f)
        , normal(0.0f, 1.0f, 0.0f)
        , penetration(0.0f)
        , featureIdA(0)
        , featureIdB(0) {}

    /**
     * @brief Check if contact is valid
     */
    [[nodiscard]] bool isValid() const noexcept {
        return penetration > 0.0f && normal.lengthSquared() > 0.9f;
    }

    /**
     * @brief Get contact confidence (heuristic)
     */
    [[nodiscard]] float getConfidence() const noexcept {
        return penetration;
    }
};

/**
 * @brief Collision result
 */
struct CollisionResult {
    bool isColliding;
    Vector3 normal;
    float penetration;
    std::vector<ContactPoint> contacts;

    CollisionResult() noexcept : isColliding(false), normal(0.0f, 1.0f, 0.0f), penetration(0.0f) {}

    /**
     * @brief Clear result
     */
    void clear() noexcept {
        isColliding = false;
        normal = Vector3(0.0f, 1.0f, 0.0f);
        penetration = 0.0f;
        contacts.clear();
    }

    /**
     * @brief Get contact count
     */
    [[nodiscard]] size_t getContactCount() const noexcept {
        return contacts.size();
    }

    /**
     * @brief Check if result is valid
     */
    [[nodiscard]] bool isValid() const noexcept {
        if (!isColliding) return true; // No collision is valid
        return penetration > 0.0f && normal.lengthSquared() > 0.9f;
    }

    /**
     * @brief Reserve capacity for contacts
     */
    void reserve(size_t count) noexcept {
        contacts.reserve(count);
    }

    /**
     * @brief Add contact point
     */
    void addContact(const ContactPoint& contact) noexcept {
        contacts.push_back(contact);
    }
};

} // namespace narrowphase
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_NARROWPHASE_COLLISION_RESULT_H
