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
#include <unordered_set>
#include <vector>

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
    const std::vector<ConstraintDefinition>& constraints,
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

    // Add edges from constraints
    for (size_t i = 0; i < constraints.size(); ++i) {
        const auto& constraint = constraints[i];
        if (constraint.bodyA.isValid() && constraint.bodyB.isValid() && constraint.enabled) {
            adjacency[constraint.bodyA.index].push_back(constraint.bodyB.index);
            adjacency[constraint.bodyB.index].push_back(constraint.bodyA.index);
        }
    }

    // Build a map from body index to constraint indices
    std::unordered_map<uint32_t, std::vector<size_t>> bodyToConstraintIndices;
    for (size_t i = 0; i < constraints.size(); ++i) {
        const auto& constraint = constraints[i];
        if (constraint.bodyA.isValid() && constraint.enabled) {
            bodyToConstraintIndices[constraint.bodyA.index].push_back(i);
        }
        if (constraint.bodyB.isValid() && constraint.enabled) {
            bodyToConstraintIndices[constraint.bodyB.index].push_back(i);
        }
    }

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

            // Add constraint handles for this island
            std::unordered_set<size_t> constraintSet;
            for (const auto& bodyHandle : island.bodies) {
                auto it = bodyToConstraintIndices.find(bodyHandle.index);
                if (it != bodyToConstraintIndices.end()) {
                    for (size_t constraintIndex : it->second) {
                        constraintSet.insert(constraintIndex);
                    }
                }
            }

            // Convert constraint indices to handles
            for (size_t constraintIndex : constraintSet) {
                if (constraintIndex < constraints.size()) {
                    island.constraints.push_back(constraints[constraintIndex].handle);
                }
            }

            outIslands.push_back(std::move(island));
        }
    }
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
