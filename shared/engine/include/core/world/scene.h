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

/**
 * @brief Maximum number of instances per scene (for safety)
 */
constexpr size_t MAX_INSTANCES_PER_SCENE = 1000000;

/**
 * @brief Maximum number of scenes per world (for safety)
 */
constexpr size_t MAX_SCENES_PER_WORLD = 10000;

/**
 * @brief Maximum scene name length (for safety)
 */
constexpr size_t MAX_SCENE_NAME_LENGTH = 256;

/**
 * @brief Maximum tag length for scene queries (for safety)
 */
constexpr size_t MAX_SCENE_TAG_LENGTH = 128;

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
     * @param name New name (empty name assigns "UnnamedScene", names longer than MAX_SCENE_NAME_LENGTH are truncated)
     * @note Thread-safe with mutex protection
     */
    void setName(const std::string& name);
    
    /**
     * @brief Set scene name (move overload)
     * @param name New name (moved, empty name assigns "UnnamedScene", names longer than MAX_SCENE_NAME_LENGTH are truncated)
     * @note Thread-safe with mutex protection
     */
    void setName(std::string&& name) noexcept;
    
    /**
     * @brief Add an instance to the scene
     * @param instance Instance to add (null instance rejected)
     * @return true if added, false if already in scene or null
     * @note Thread-safe with mutex protection
     */
    bool addInstance(runtime::Instance* instance);
    
    /**
     * @brief Remove an instance from the scene
     * @param instance Instance to remove (null instance rejected)
     * @return true if removed, false if not in scene or null
     * @note Thread-safe with mutex protection
     */
    bool removeInstance(runtime::Instance* instance);
    
    /**
     * @brief Check if instance is in scene
     * @param instance Instance to check (null returns false)
     * @return true if instance is in scene, false if not or null
     * @note Thread-safe with mutex protection
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
     * @param name Instance name to find (empty name returns nullptr)
     * @return Instance pointer or nullptr if not found or name is empty
     * @note Thread-safe with mutex protection
     */
    [[nodiscard]] runtime::Instance* findInstance(const std::string& name) const;
    
    /**
     * @brief Find instances by tag
     * @param tag Tag to search for (empty tag returns empty vector)
     * @return Vector of instances with the tag
     * @note Thread-safe with mutex protection
     */
    [[nodiscard]] std::vector<runtime::Instance*> findInstancesByTag(const std::string& tag) const;
    
    /**
     * @brief Clear all instances from scene
     * @note Thread-safe with mutex protection
     */
    void clear() noexcept;
    
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
     * @param name Scene name (empty name rejected)
     * @return Scene pointer or nullptr if name is empty or duplicate
     * @note Thread-safe with mutex protection
     */
    [[nodiscard]] Scene* createScene(const std::string& name);
    
    /**
     * @brief Destroy a scene
     * @param sceneId Scene ID to destroy (INVALID_SCENE_ID rejected)
     * @return true if destroyed, false if not found or invalid
     * @note Deactivates scene if it was active
     * @note Thread-safe with mutex protection
     */
    bool destroyScene(SceneID sceneId);
    
    /**
     * @brief Get scene by ID
     * @param sceneId Scene ID (INVALID_SCENE_ID returns nullptr)
     * @return Scene pointer or nullptr if not found
     * @note Thread-safe with mutex protection
     */
    [[nodiscard]] Scene* getScene(SceneID sceneId);
    [[nodiscard]] const Scene* getScene(SceneID sceneId) const;
    
    /**
     * @brief Get scene by name
     * @param name Scene name (empty name returns nullptr)
     * @return Scene pointer or nullptr if not found
     * @note Thread-safe with mutex protection
     */
    [[nodiscard]] Scene* getScene(const std::string& name);
    [[nodiscard]] const Scene* getScene(const std::string& name) const;
    
    /**
     * @brief Check if scene exists by ID
     * @param sceneId Scene ID (INVALID_SCENE_ID returns false)
     * @return true if scene exists, false otherwise
     * @note Thread-safe with mutex protection
     */
    [[nodiscard]] bool hasScene(SceneID sceneId) const noexcept;
    
    /**
     * @brief Check if scene exists by name
     * @param name Scene name (empty name returns false)
     * @return true if scene exists, false otherwise
     * @note Thread-safe with mutex protection
     */
    [[nodiscard]] bool hasScene(const std::string& name) const noexcept;
    
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
     * @param sceneId Scene ID to activate (INVALID_SCENE_ID clears active scene)
     * @note Deactivates previous active scene and activates new scene
     * @note Thread-safe with mutex protection
     */
    void setActiveScene(SceneID sceneId);
    
    /**
     * @brief Clear all scenes
     * @note Deactivates active scene before clearing
     * @note Thread-safe with mutex protection
     */
    void clear() noexcept;
    
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
