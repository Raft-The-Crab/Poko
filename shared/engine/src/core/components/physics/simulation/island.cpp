/**
 * @file island.cpp
 * @brief Island management implementation
 *
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 *
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/simulation/island.h"
#include "core/components/physics/core/handle.h"
#include <algorithm>
#include <unordered_map>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace simulation {

bool Island::shouldSleep(float sleepThreshold) const noexcept {
    (void)sleepThreshold;
    return sleepTime > 1.0f;
}

void Island::updateSleepTime(float deltaTime) noexcept {
    if (!isSleeping) {
        sleepTime += deltaTime;
    }
}

IslandBuilder::IslandBuilder() noexcept {
    // Initialize empty
}

void IslandBuilder::buildIslands(
    const std::vector<ContactManifold>& manifolds,
    const std::vector<ConstraintHandle>& constraints,
    std::vector<Island>& outIslands
) {
    outIslands.clear();
    visited.clear();
    bodyToIsland.clear();

    // Build adjacency graph from contacts and constraints
    std::unordered_map<uint32_t, std::vector<uint32_t>> adjacency;

    // Add edges from contact manifolds
    for (const auto& manifold : manifolds) {
        if (manifold.bodyA.isValid() && manifold.bodyB.isValid()) {
            adjacency[manifold.bodyA.index].push_back(manifold.bodyB.index);
            adjacency[manifold.bodyB.index].push_back(manifold.bodyA.index);
        }
    }

    // Note: Constraint edges would be added here if we had constraint runtime data
    // For now, constraints are handled separately in the solver

    // Find connected components using DFS
    for (const auto& [bodyIndex, neighbors] : adjacency) {
        bool found = false;
        for (uint32_t v : visited) {
            if (v == bodyIndex) {
                found = true;
                break;
            }
        }

        if (!found) {
            Island island;
            island.isSleeping = false;
            island.sleepTime = 0.0f;

            // DFS to collect all bodies in this island
            dfsVisitUint32(bodyIndex, adjacency, island);

            outIslands.push_back(std::move(island));
        }
    }

    (void)constraints;
}

void IslandBuilder::clear() noexcept {
    visited.clear();
    bodyToIsland.clear();
}

void IslandBuilder::dfsVisitUint32(
    uint32_t bodyIndex,
    const std::unordered_map<uint32_t, std::vector<uint32_t>>& adjacency,
    Island& island
) {
    // Check if already visited
    for (uint32_t v : visited) {
        if (v == bodyIndex) {
            return;
        }
    }

    visited.push_back(bodyIndex);
    island.bodies.push_back(BodyHandle(bodyIndex, 0));

    auto it = adjacency.find(bodyIndex);
    if (it != adjacency.end()) {
        for (uint32_t neighborIndex : it->second) {
            dfsVisitUint32(neighborIndex, adjacency, island);
        }
    }
}

void IslandBuilder::visitBody(
    BodyHandle body,
    int32_t islandIndex,
    const std::vector<ContactManifold>& manifolds,
    const std::vector<ConstraintHandle>& constraints
) {
    // This method is no longer used; replaced by dfsVisitUint32
    (void)body;
    (void)islandIndex;
    (void)manifolds;
    (void)constraints;
}

} // namespace simulation
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
