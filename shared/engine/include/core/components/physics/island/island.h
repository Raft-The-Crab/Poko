/**
 * @file island.h
 * @brief Physics island for sleeping and optimization
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_ISLAND_ISLAND_H
#define POKO_CORE_COMPONENTS_PHYSICS_ISLAND_ISLAND_H

#include "../world/world.h"
#include "../narrowphase/contact.h"
#include <vector>
#include <cstdint>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace island {

using world::BodyHandle;
using world::RigidBody;

/**
 * @brief Physics island
 * 
 * Groups connected bodies for independent solving.
 * Enables sleeping optimization.
 */
class Island {
public:
    /**
     * @brief Constructor
     */
    Island() noexcept
        : bodies()
        , isSleeping(false)
        , sleepTimer(0.0f) {}
    
    /**
     * @brief Get bodies in island
     */
    [[nodiscard]] const std::vector<BodyHandle>& getBodies() const noexcept {
        return bodies;
    }
    
    /**
     * @brief Add body to island
     */
    void addBody(BodyHandle body) noexcept {
        bodies.push_back(body);
    }
    
    /**
     * @brief Clear bodies
     */
    void clear() noexcept {
        bodies.clear();
    }
    
    /**
     * @brief Check if island is sleeping
     */
    [[nodiscard]] bool getIsSleeping() const noexcept {
        return isSleeping;
    }
    
    /**
     * @brief Set sleeping state
     */
    void setIsSleeping(bool sleeping) noexcept {
        isSleeping = sleeping;
    }
    
    /**
     * @brief Get sleep timer
     */
    [[nodiscard]] float getSleepTimer() const noexcept {
        return sleepTimer;
    }
    
    /**
     * @brief Set sleep timer
     */
    void setSleepTimer(float timer) noexcept {
        sleepTimer = timer;
    }
    
    /**
     * @brief Update sleep timer
     */
    void updateSleepTimer(float deltaTime, float sleepThreshold) noexcept {
        if (isSleeping) return;
        
        sleepTimer += deltaTime;
        
        // Check if all bodies are below threshold
        bool allBelowThreshold = true;
        for (BodyHandle handle : bodies) {
            // In a full implementation, check body velocity
            // For now, placeholder
            (void)handle;
        }
        
        if (allBelowThreshold && sleepTimer > 1.0f) {
            isSleeping = true;
        } else {
            sleepTimer = 0.0f;
        }
    }
    
    /**
     * @brief Wake up island
     */
    void wakeUp() noexcept {
        isSleeping = false;
        sleepTimer = 0.0f;
    }
    
private:
    std::vector<BodyHandle> bodies;
    bool isSleeping;
    float sleepTimer;
};

/**
 * @brief Island manager
 * 
 * Manages island construction and sleep/wake logic.
 */
class IslandManager {
public:
    /**
     * @brief Constructor
     */
    IslandManager() noexcept
        : islands()
        , sleepThreshold(0.01f)
        , sleepTime(1.0f) {}
    
    /**
     * @brief Build islands from contacts
     */
    void buildIslands(const std::vector<narrowphase::ContactManifold>& manifolds,
                      const std::vector<RigidBody>& bodies) {
        islands.clear();
        
        // In a full implementation, this would:
        // 1. Build graph from contacts
        // 2. Find connected components
        // 3. Create islands for each component
        // 4. Handle static bodies as boundary nodes
        
        // For now, placeholder
        (void)manifolds;
        (void)bodies;
    }
    
    /**
     * @brief Update island sleep states
     */
    void updateSleep(float deltaTime) {
        for (Island& island : islands) {
            island.updateSleepTimer(deltaTime, sleepThreshold);
        }
    }
    
    /**
     * @brief Wake up island containing body
     */
    void wakeUpBody(BodyHandle body) {
        // Find island containing body and wake it
        for (Island& island : islands) {
            for (BodyHandle handle : island.getBodies()) {
                if (handle == body) {
                    island.wakeUp();
                    return;
                }
            }
        }
    }
    
    /**
     * @brief Get islands
     */
    [[nodiscard]] const std::vector<Island>& getIslands() const noexcept {
        return islands;
    }
    
    /**
     * @brief Set sleep threshold
     */
    void setSleepThreshold(float threshold) noexcept {
        sleepThreshold = threshold;
    }
    
    /**
     * @brief Set sleep time
     */
    void setSleepTime(float time) noexcept {
        sleepTime = time;
    }
    
private:
    std::vector<Island> islands;
    float sleepThreshold;
    float sleepTime;
};

} // namespace island
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_ISLAND_ISLAND_H
