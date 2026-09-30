/**
 * @file island.h
 * @brief Island management for sleeping and parallel solving
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_SIMULATION_ISLAND_H
#define POKO_CORE_COMPONENTS_PHYSICS_SIMULATION_ISLAND_H

#include "core/components/physics/core/handle.h"
#include "core/components/physics/contacts/contact_manifold.h"
#include <vector>
#include <unordered_map>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace simulation {

using core::BodyHandle;
using core::ConstraintHandle;
using contacts::ContactManifold;

/**
 * @brief Island for sleeping and parallel solving
 */
class Island {
public:
    std::vector<BodyHandle> bodies;
    std::vector<ConstraintHandle> constraints;
    std::vector<ContactManifold> manifolds;
    bool isSleeping;
    float sleepTime;

    /**
     * @brief Constructor
     */
    Island() noexcept : isSleeping(false), sleepTime(0.0f) {}

    /**
     * @brief Clear island
     */
    void clear() noexcept {
        bodies.clear();
        constraints.clear();
        manifolds.clear();
        isSleeping = false;
        sleepTime = 0.0f;
    }

    /**
     * @brief Check if island should sleep
     */
    [[nodiscard]] bool shouldSleep(float sleepThreshold) const noexcept;

    /**
     * @brief Update sleep time
     */
    void updateSleepTime(float deltaTime) noexcept;

    /**
     * @brief Wake island
     */
    void wake() noexcept {
        isSleeping = false;
        sleepTime = 0.0f;
    }

    /**
     * @brief Get body count
     */
    [[nodiscard]] size_t getBodyCount() const noexcept {
        return bodies.size();
    }

    /**
     * @brief Get constraint count
     */
    [[nodiscard]] size_t getConstraintCount() const noexcept {
        return constraints.size();
    }

    /**
     * @brief Get manifold count
     */
    [[nodiscard]] size_t getManifoldCount() const noexcept {
        return manifolds.size();
    }

    /**
     * @brief Check if island is valid
     */
    [[nodiscard]] bool isValid() const noexcept {
        return !bodies.empty() || !constraints.empty();
    }

    /**
     * @brief Reserve capacity for performance
     */
    void reserve(size_t bodyCount, size_t constraintCount, size_t manifoldCount) noexcept {
        bodies.reserve(bodyCount);
        constraints.reserve(constraintCount);
        manifolds.reserve(manifoldCount);
    }
};

/**
 * @brief Island builder
 */
class IslandBuilder {
public:
    /**
     * @brief Constructor
     */
    IslandBuilder() noexcept;

    /**
     * @brief Build islands from contacts and constraints
     */
    void buildIslands(
        const std::vector<ContactManifold>& manifolds,
        const std::vector<ConstraintHandle>& constraints,
        std::vector<Island>& outIslands
    );

    /**
     * @brief Clear
     */
    void clear() noexcept;

    /**
     * @brief Get visited count (debug)
     */
    [[nodiscard]] size_t getVisitedCount() const noexcept {
        return visited.size();
    }

private:
    std::vector<uint32_t> visited;
    std::vector<int32_t> bodyToIsland;

    /**
     * @brief DFS visit to collect connected bodies (using uint32_t indices)
     */
    void dfsVisitUint32(
        uint32_t bodyIndex,
        const std::unordered_map<uint32_t, std::vector<uint32_t>>& adjacency,
        Island& island
    );

    /**
     * @brief Visit body for graph traversal (deprecated, kept for compatibility)
     */
    void visitBody(
        BodyHandle body,
        int32_t islandIndex,
        const std::vector<ContactManifold>& manifolds,
        const std::vector<ConstraintHandle>& constraints
    );
};

} // namespace simulation
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_SIMULATION_ISLAND_H
