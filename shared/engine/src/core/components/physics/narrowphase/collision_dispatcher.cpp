/**
 * @file collision_dispatcher.cpp
 * @brief Collision dispatcher implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/narrowphase/collision_dispatcher.h"
#include "core/components/physics/narrowphase/pair_algorithms/pair_algorithms.h"
#include "core/components/physics/shapes/primitives/shape_type.h"
#include <variant>
#include <algorithm>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace narrowphase {

CollisionDispatcher::CollisionDispatcher() noexcept {
    // Register sphere-sphere
    registerAlgorithm(ShapeType::Sphere, ShapeType::Sphere, 
        [](const ShapeDefinition& a, const Transform& ta, const ShapeDefinition& b, const Transform& tb) -> CollisionResult {
            if (a.type != ShapeType::Sphere || b.type != ShapeType::Sphere) {
                CollisionResult result;
                result.isColliding = false;
                return result;
            }
            const Sphere& sphereA = std::get<Sphere>(a.shape);
            const Sphere& sphereB = std::get<Sphere>(b.shape);
            return collideSphereSphere(sphereA, ta, sphereB, tb);
        });
    
    // Register sphere-box
    registerAlgorithm(ShapeType::Sphere, ShapeType::Box,
        [](const ShapeDefinition& a, const Transform& ta, const ShapeDefinition& b, const Transform& tb) -> CollisionResult {
            if (a.type != ShapeType::Sphere || b.type != ShapeType::Box) {
                CollisionResult result;
                result.isColliding = false;
                return result;
            }
            const Sphere& sphere = std::get<Sphere>(a.shape);
            const Box& box = std::get<Box>(b.shape);
            return collideSphereBox(sphere, ta, box, tb);
        });
    
    registerAlgorithm(ShapeType::Box, ShapeType::Sphere,
        [](const ShapeDefinition& a, const Transform& ta, const ShapeDefinition& b, const Transform& tb) -> CollisionResult {
            if (a.type != ShapeType::Box || b.type != ShapeType::Sphere) {
                CollisionResult result;
                result.isColliding = false;
                return result;
            }
            const Box& box = std::get<Box>(a.shape);
            const Sphere& sphere = std::get<Sphere>(b.shape);
            return collideSphereBox(sphere, tb, box, ta);
        });
    
    // Register box-box
    registerAlgorithm(ShapeType::Box, ShapeType::Box,
        [](const ShapeDefinition& a, const Transform& ta, const ShapeDefinition& b, const Transform& tb) -> CollisionResult {
            if (a.type != ShapeType::Box || b.type != ShapeType::Box) {
                CollisionResult result;
                result.isColliding = false;
                return result;
            }
            const Box& boxA = std::get<Box>(a.shape);
            const Box& boxB = std::get<Box>(b.shape);
            return collideBoxBox(boxA, ta, boxB, tb);
        });
    
    // Register sphere-plane
    registerAlgorithm(ShapeType::Sphere, ShapeType::Plane,
        [](const ShapeDefinition& a, const Transform& ta, const ShapeDefinition& b, const Transform& tb) -> CollisionResult {
            if (a.type != ShapeType::Sphere || b.type != ShapeType::Plane) {
                CollisionResult result;
                result.isColliding = false;
                return result;
            }
            const Sphere& sphere = std::get<Sphere>(a.shape);
            const Plane& plane = std::get<Plane>(b.shape);
            return collideSpherePlane(sphere, ta, plane, tb);
        });
    
    registerAlgorithm(ShapeType::Plane, ShapeType::Sphere,
        [](const ShapeDefinition& a, const Transform& ta, const ShapeDefinition& b, const Transform& tb) -> CollisionResult {
            if (a.type != ShapeType::Plane || b.type != ShapeType::Sphere) {
                CollisionResult result;
                result.isColliding = false;
                return result;
            }
            const Plane& plane = std::get<Plane>(a.shape);
            const Sphere& sphere = std::get<Sphere>(b.shape);
            return collideSpherePlane(sphere, tb, plane, ta);
        });
    
    // Register capsule-plane
    registerAlgorithm(ShapeType::Capsule, ShapeType::Plane,
        [](const ShapeDefinition& a, const Transform& ta, const ShapeDefinition& b, const Transform& tb) -> CollisionResult {
            if (a.type != ShapeType::Capsule || b.type != ShapeType::Plane) {
                CollisionResult result;
                result.isColliding = false;
                return result;
            }
            const Capsule& capsule = std::get<Capsule>(a.shape);
            const Plane& plane = std::get<Plane>(b.shape);
            return collideCapsulePlane(capsule, ta, plane, tb);
        });
    
    registerAlgorithm(ShapeType::Plane, ShapeType::Capsule,
        [](const ShapeDefinition& a, const Transform& ta, const ShapeDefinition& b, const Transform& tb) -> CollisionResult {
            if (a.type != ShapeType::Plane || b.type != ShapeType::Capsule) {
                CollisionResult result;
                result.isColliding = false;
                return result;
            }
            const Plane& plane = std::get<Plane>(a.shape);
            const Capsule& capsule = std::get<Capsule>(b.shape);
            return collideCapsulePlane(capsule, tb, plane, ta);
        });

    // Register sphere-capsule
    registerAlgorithm(ShapeType::Sphere, ShapeType::Capsule,
        [](const ShapeDefinition& a, const Transform& ta, const ShapeDefinition& b, const Transform& tb) -> CollisionResult {
            if (a.type != ShapeType::Sphere || b.type != ShapeType::Capsule) {
                CollisionResult result;
                result.isColliding = false;
                return result;
            }
            const Sphere& sphere = std::get<Sphere>(a.shape);
            const Capsule& capsule = std::get<Capsule>(b.shape);
            return collideSphereCapsule(sphere, ta, capsule, tb);
        });

    registerAlgorithm(ShapeType::Capsule, ShapeType::Sphere,
        [](const ShapeDefinition& a, const Transform& ta, const ShapeDefinition& b, const Transform& tb) -> CollisionResult {
            if (a.type != ShapeType::Capsule || b.type != ShapeType::Sphere) {
                CollisionResult result;
                result.isColliding = false;
                return result;
            }
            const Capsule& capsule = std::get<Capsule>(a.shape);
            const Sphere& sphere = std::get<Sphere>(b.shape);
            CollisionResult result_swapped = collideSphereCapsule(sphere, tb, capsule, ta);
            result_swapped.normal = -result_swapped.normal;
            // Swap feature IDs for correct contact persistence
            for (auto& contact : result_swapped.contacts) {
                uint32_t temp = contact.featureIdA;
                contact.featureIdA = contact.featureIdB;
                contact.featureIdB = temp;
            }
            return result_swapped;
        });

    // Register capsule-capsule
    registerAlgorithm(ShapeType::Capsule, ShapeType::Capsule,
        [](const ShapeDefinition& a, const Transform& ta, const ShapeDefinition& b, const Transform& tb) -> CollisionResult {
            if (a.type != ShapeType::Capsule || b.type != ShapeType::Capsule) {
                CollisionResult result;
                result.isColliding = false;
                return result;
            }
            const Capsule& capsuleA = std::get<Capsule>(a.shape);
            const Capsule& capsuleB = std::get<Capsule>(b.shape);
            return collideCapsuleCapsule(capsuleA, ta, capsuleB, tb);
        });
}

void CollisionDispatcher::registerAlgorithm(ShapeType typeA, ShapeType typeB, PairAlgorithm algorithm) {
    PairKey key = makePairKey(typeA, typeB);
    algorithms[key] = algorithm;
}

CollisionResult CollisionDispatcher::dispatch(
    const ShapeDefinition& shapeA,
    const Transform& transformA,
    const ShapeDefinition& shapeB,
    const Transform& transformB
) {
    PairKey key = makePairKey(shapeA.type, shapeB.type);
    
    auto it = algorithms.find(key);
    if (it != algorithms.end()) {
        return it->second(shapeA, transformA, shapeB, transformB);
    }
    
    // Try swapped order
    PairKey swappedKey = makePairKey(shapeB.type, shapeA.type);
    auto swappedIt = algorithms.find(swappedKey);
    if (swappedIt != algorithms.end()) {
        CollisionResult result = swappedIt->second(shapeB, transformB, shapeA, transformA);
        // Swap and negate normal
        result.normal = -result.normal;
        // Swap feature IDs for correct contact persistence
        for (auto& contact : result.contacts) {
            uint32_t temp = contact.featureIdA;
            contact.featureIdA = contact.featureIdB;
            contact.featureIdB = temp;
        }
        return result;
    }
    
    // No algorithm registered
    CollisionResult result;
    result.isColliding = false;
    return result;
}

bool CollisionDispatcher::hasAlgorithm(ShapeType typeA, ShapeType typeB) const {
    PairKey key = makePairKey(typeA, typeB);
    return algorithms.find(key) != algorithms.end();
}

CollisionDispatcher::PairKey CollisionDispatcher::makePairKey(ShapeType typeA, ShapeType typeB) noexcept {
    uint64_t a = static_cast<uint64_t>(typeA);
    uint64_t b = static_cast<uint64_t>(typeB);
    return (a << 32) | b;
}

} // namespace narrowphase
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
