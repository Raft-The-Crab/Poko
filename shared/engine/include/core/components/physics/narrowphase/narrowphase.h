/**
 * @file narrowphase.h
 * @brief Narrowphase collision detection interface
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_NARROWPHASE_NARROWPHASE_H
#define POKO_CORE_COMPONENTS_PHYSICS_NARROWPHASE_NARROWPHASE_H

#include "contact.h"
#include "../shapes/sphere.h"
#include "../shapes/box.h"
#include "../shapes/capsule.h"
#include <vector>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace narrowphase {

/**
 * @brief Narrowphase collision detection
 * 
 * Performs exact collision detection for shape pairs.
 * Uses GJK for convex shapes and specialized tests for primitives.
 */
class Narrowphase {
public:
    /**
     * @brief Sphere-sphere collision
     */
    static bool collideSphereSphere(
        const shapes::Sphere& a,
        const shapes::Sphere& b,
        ContactManifold& manifold
    );
    
    /**
     * @brief Sphere-box collision
     */
    static bool collideSphereBox(
        const shapes::Sphere& sphere,
        const shapes::Box& box,
        ContactManifold& manifold
    );
    
    /**
     * @brief Box-box collision (SAT)
     */
    static bool collideBoxBox(
        const shapes::Box& a,
        const shapes::Box& b,
        ContactManifold& manifold
    );
    
    /**
     * @brief Capsule-sphere collision
     */
    static bool collideCapsuleSphere(
        const shapes::Capsule& capsule,
        const shapes::Sphere& sphere,
        ContactManifold& manifold
    );
    
    /**
     * @brief Capsule-box collision
     */
    static bool collideCapsuleBox(
        const shapes::Capsule& capsule,
        const shapes::Box& box,
        ContactManifold& manifold
    );
    
    /**
     * @brief Capsule-capsule collision
     */
    static bool collideCapsuleCapsule(
        const shapes::Capsule& a,
        const shapes::Capsule& b,
        ContactManifold& manifold
    );
};

} // namespace narrowphase
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_NARROWPHASE_NARROWPHASE_H
