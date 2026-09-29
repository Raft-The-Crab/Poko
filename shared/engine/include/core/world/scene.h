/**
 * @file scene.h
 * @brief World/Scene system header
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 * 
 * This module provides scene management for organizing instances
 * into hierarchical worlds with loading, querying, and lifecycle support.
 */

#ifndef POKO_CORE_WORLD_SCENE_H
#define POKO_CORE_WORLD_SCENE_H

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <mutex>

#include "core/handles/handle.h"
#include "core/runtime/instance.h"

namespace poko {
namespace core {
namespace world {

// ============================================================================
// Scene ID
// ============================================================================

/**
 * @brief Unique identifier for scenes
 */
using SceneID = uint64_t;

/**
 * @brief Invalid scene ID constant
 */
constexpr SceneID INVALID_SCENE_ID = 0;

// ============================================================================
// Scene Class
// ============================================================================

/**
 * @brief Scene class for organizing instances
 * 
 * Scenes provide hierarchical organization of instances, supporting
 * scene loading/unloading, instance queries, and lifecycle management.
 */
class Scene {
public:
    Scene();
    explicit Scene(const std::string& name);
    ~Scene();
    
    // Delete copy/move (scenes should be unique)
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&) = delete;
    Scene& operator=(Scene&&) = delete;
    
    /**
     * @brief Get scene ID
     */
    [[nodiscard]] SceneID getId() const noexcept { return m_id; }
    
    /**
     * @brief Get scene name
     */
    [[nodiscard]] const std::string& getName() const noexcept { return m_name; }
    
    /**
     * @brief Set scene name
     */
    void setName(const std::string& name);
    void setName(std::string&& name);
    
    /**
     * @brief Add an instance to the scene
     * @param instance Instance to add
     * @return true if added, false if already in scene
     */
    bool addInstance(runtime::Instance* instance);
    
    /**
     * @brief Remove an instance from the scene
     * @param instance Instance to remove
     * @return true if removed, false if not in scene
     */
    bool removeInstance(runtime::Instance* instance);
    
    /**
     * @brief Check if instance is in scene
     */
    [[nodiscard]] bool hasInstance(runtime::Instance* instance) const;
    
    /**
     * @brief Get all instances in scene
     */
    [[nodiscard]] const std::vector<runtime::Instance*>& getInstances() const noexcept { return m_instances; }
    
    /**
     * @brief Get instance count
     */
    [[nodiscard]] size_t getInstanceCount() const noexcept { return m_instances.size(); }
    
    /**
     * @brief Find instance by name
     */
    [[nodiscard]] runtime::Instance* findInstance(const std::string& name) const;
    
    /**
     * @brief Find instances by tag
     */
    [[nodiscard]] std::vector<runtime::Instance*> findInstancesByTag(const std::string& tag) const;
    
    /**
     * @brief Clear all instances from scene
     */
    void clear();
    
    /**
     * @brief Check if scene is active
     */
    [[nodiscard]] bool isActive() const noexcept { return m_active; }
    
    /**
     * @brief Set scene active state
     */
    void setActive(bool active) noexcept { m_active = active; }
    
private:
    SceneID m_id;
    std::string m_name;
    std::vector<runtime::Instance*> m_instances;
    bool m_active;
    mutable std::mutex m_mutex;
};

// ============================================================================
// World Class
// ============================================================================

/**
 * @brief World class for managing multiple scenes
 * 
 * World provides a container for scenes with loading, unloading,
 * and scene management operations.
 */
class World {
public:
    World();
    ~World();
    
    // Delete copy/move (world should be unique)
    World(const World&) = delete;
    World& operator=(const World&) = delete;
    World(World&&) = delete;
    World& operator=(World&&) = delete;
    
    /**
     * @brief Create a new scene
     * @param name Scene name
     * @return Scene pointer or nullptr if name is empty
     */
    [[nodiscard]] Scene* createScene(const std::string& name);
    
    /**
     * @brief Destroy a scene
     * @param sceneId Scene ID to destroy
     * @return true if destroyed, false if not found
     */
    bool destroyScene(SceneID sceneId);
    
    /**
     * @brief Get scene by ID
     */
    [[nodiscard]] Scene* getScene(SceneID sceneId);
    [[nodiscard]] const Scene* getScene(SceneID sceneId) const;
    
    /**
     * @brief Get scene by name
     */
    [[nodiscard]] Scene* getScene(const std::string& name);
    [[nodiscard]] const Scene* getScene(const std::string& name) const;
    
    /**
     * @brief Check if scene exists
     */
    [[nodiscard]] bool hasScene(SceneID sceneId) const;
    [[nodiscard]] bool hasScene(const std::string& name) const;
    
    /**
     * @brief Get all scenes
     */
    [[nodiscard]] const std::vector<Scene*>& getScenes() const noexcept { return m_scenes; }
    
    /**
     * @brief Get scene count
     */
    [[nodiscard]] size_t getSceneCount() const noexcept { return m_scenes.size(); }
    
    /**
     * @brief Get active scene
     */
    [[nodiscard]] Scene* getActiveScene() noexcept;
    [[nodiscard]] const Scene* getActiveScene() const noexcept;
    
    /**
     * @brief Set active scene
     */
    void setActiveScene(SceneID sceneId);
    
    /**
     * @brief Clear all scenes
     */
    void clear();
    
private:
    std::vector<Scene*> m_scenes;
    std::unordered_map<SceneID, Scene*> m_sceneMap;
    std::unordered_map<std::string, Scene*> m_nameMap;
    SceneID m_nextSceneId;
    Scene* m_activeScene;
    mutable std::mutex m_mutex;
};

// ============================================================================
// Global World Interface
// ============================================================================

/**
 * @brief Get the global world instance
 * Creates the world on first call
 */
World& getGlobalWorld();

/**
 * @brief Destroy the global world instance
 * Called during engine shutdown
 */
void destroyGlobalWorld();

} // namespace world
} // namespace core
} // namespace poko

#endif // POKO_CORE_WORLD_SCENE_H
