/**
 * @file collision_dispatcher.h
 * @brief Collision dispatcher for narrowphase
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_NARROWPHASE_COLLISION_DISPATCHER_H
#define POKO_CORE_COMPONENTS_PHYSICS_NARROWPHASE_COLLISION_DISPATCHER_H

#include "collision_result.h"
#include "../shapes/primitives/shape_type.h"
#include "../shapes/shape_definition.h"
#include "../colliders/collider_definition.h"
#include "../transforms/transform.h"
#include <functional>
#include <unordered_map>
#include <cstdint>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace narrowphase {

using shapes::ShapeType;
using shapes::ShapeDefinition;
using colliders::ColliderDefinition;
using transforms::Transform;

/**
 * @brief Pair algorithm function signature
 */
using PairAlgorithm = std::function<CollisionResult(
    const ShapeDefinition& shapeA,
    const Transform& transformA,
    const ShapeDefinition& shapeB,
    const Transform& transformB
)>;

/**
 * @brief Collision dispatcher
 */
class CollisionDispatcher {
public:
    /**
     * @brief Constructor
     */
    CollisionDispatcher() noexcept;
    
    /**
     * @brief Register pair algorithm
     */
    void registerAlgorithm(ShapeType typeA, ShapeType typeB, PairAlgorithm algorithm);
    
    /**
     * @brief Dispatch collision test
     */
    CollisionResult dispatch(
        const ShapeDefinition& shapeA,
        const Transform& transformA,
        const ShapeDefinition& shapeB,
        const Transform& transformB
    );
    
    /**
     * @brief Check if algorithm is registered
     */
    [[nodiscard]] bool hasAlgorithm(ShapeType typeA, ShapeType typeB) const;

    /**
     * @brief Clear all algorithms
     */
    void clear() noexcept {
        algorithms.clear();
    }

    /**
     * @brief Get algorithm count
     */
    [[nodiscard]] size_t getAlgorithmCount() const noexcept {
        return algorithms.size();
    }

private:
    using PairKey = uint64_t;

    static PairKey makePairKey(ShapeType typeA, ShapeType typeB) noexcept;

    std::unordered_map<PairKey, PairAlgorithm> algorithms;
};

} // namespace narrowphase
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_NARROWPHASE_COLLISION_DISPATCHER_H
