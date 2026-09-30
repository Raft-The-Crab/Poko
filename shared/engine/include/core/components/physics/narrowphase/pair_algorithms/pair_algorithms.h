/**
 * @file pair_algorithms.h
 * @brief Pair collision algorithms
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_NARROWPHASE_PAIR_ALGORITHMS_H
#define POKO_CORE_COMPONENTS_PHYSICS_NARROWPHASE_PAIR_ALGORITHMS_H

#include "core/components/physics/shapes/sphere.h"
#include "core/components/physics/shapes/box.h"
#include "core/components/physics/shapes/capsule.h"
#include "core/components/physics/shapes/plane.h"
#include "core/components/physics/transforms/transform.h"
#include "core/components/physics/math/vectors/vector3.h"
#include "core/components/physics/narrowphase/collision_result.h"
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace narrowphase {

using shapes::Sphere;
using shapes::Box;
using shapes::Capsule;
using shapes::Plane;
using transforms::Transform;
using math::Vector3;

/**
 * @brief Sphere-sphere collision
 */
CollisionResult collideSphereSphere(
    const Sphere& sphereA,
    const Transform& transformA,
    const Sphere& sphereB,
    const Transform& transformB
);

/**
 * @brief Sphere-box collision
 */
CollisionResult collideSphereBox(
    const Sphere& sphere,
    const Transform& sphereTransform,
    const Box& box,
    const Transform& boxTransform
);

/**
 * @brief Box-box collision
 */
CollisionResult collideBoxBox(
    const Box& boxA,
    const Transform& transformA,
    const Box& boxB,
    const Transform& transformB
);

/**
 * @brief Sphere-plane collision
 */
CollisionResult collideSpherePlane(
    const Sphere& sphere,
    const Transform& sphereTransform,
    const Plane& plane,
    const Transform& planeTransform
);

/**
 * @brief Capsule-plane collision
 */
CollisionResult collideCapsulePlane(
    const Capsule& capsule,
    const Transform& capsuleTransform,
    const Plane& plane,
    const Transform& planeTransform
);

/**
 * @brief Sphere-capsule collision
 */
CollisionResult collideSphereCapsule(
    const Sphere& sphere,
    const Transform& sphereTransform,
    const Capsule& capsule,
    const Transform& capsuleTransform
);

/**
 * @brief Capsule-capsule collision
 */
CollisionResult collideCapsuleCapsule(
    const Capsule& capsuleA,
    const Transform& transformA,
    const Capsule& capsuleB,
    const Transform& transformB
);

} // namespace narrowphase
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_NARROWPHASE_PAIR_ALGORITHMS_H
