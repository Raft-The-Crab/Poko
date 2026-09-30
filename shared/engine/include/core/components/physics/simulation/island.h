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

#include "../core/handle.h"
#include "../contacts/contact_manifold.h"
#include <vector>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace simulation {

using core::BodyHandle;
using core::ConstraintHandle;
using bodies::BodyDefinition;
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
    void updateSleepTime(float deltaTime, const std::vector<BodyDefinition>& bodyStorage) noexcept;
    
    /**
     * @brief Wake island
     */
    void wake() noexcept {
        isSleeping = false;
        sleepTime = 0.0f;
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
    
private:
    std::vector<bool> visited;
    std::vector<int32_t> bodyToIsland;
    
    /**
     * @brief Visit body for graph traversal
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
