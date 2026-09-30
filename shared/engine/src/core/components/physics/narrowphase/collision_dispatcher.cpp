/**
 * @file collision_dispatcher.cpp
 * @brief Collision dispatcher implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "../../include/core/components/physics/narrowphase/collision_dispatcher.h"

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace narrowphase {

CollisionDispatcher::CollisionDispatcher() noexcept {
    // Register default algorithms will be added here
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
