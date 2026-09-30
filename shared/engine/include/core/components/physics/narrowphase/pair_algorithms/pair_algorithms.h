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

#include "../shapes/sphere.h"
#include "../shapes/box.h"
#include "../shapes/capsule.h"
#include "../shapes/plane.h"
#include "../transforms/transform.h"
#include "../math/vectors/vector3.h"
#include "collision_result.h"
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

} // namespace narrowphase
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_NARROWPHASE_PAIR_ALGORITHMS_H
