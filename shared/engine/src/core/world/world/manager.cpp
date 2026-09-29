/**
 * @file manager.cpp
 * @brief World manager implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/world/scene.h"
#include <algorithm>

namespace poko {
namespace core {
namespace world {

World::World()
    : m_scenes()
    , m_sceneMap()
    , m_nameMap()
    , m_nextSceneId(1)
    , m_activeScene(nullptr)
    , m_mutex()
{
}

World::~World() {
    clear();
}

Scene* World::createScene(const std::string& name) {
    if (name.empty()) {
        return nullptr;
    }
    
    if (name.length() > MAX_SCENE_NAME_LENGTH) {
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check if scene with this name already exists
    if (m_nameMap.find(name) != m_nameMap.end()) {
        return nullptr;
    }
    
    // Check scene limit
    if (m_scenes.size() >= MAX_SCENES_PER_WORLD) {
        return nullptr;
    }
    
    auto* scene = new Scene(name);
    m_scenes.push_back(scene);
    m_sceneMap[scene->getId()] = scene;
    m_nameMap[name] = scene;
    
    return scene;
}

bool World::destroyScene(SceneID sceneId) {
    if (sceneId == INVALID_SCENE_ID) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_sceneMap.find(sceneId);
    if (it == m_sceneMap.end()) {
        return false;
    }
    
    Scene* scene = it->second;
    
    // Remove from name map
    m_nameMap.erase(scene->getName());
    
    // Remove from scene map
    m_sceneMap.erase(it);
    
    // Remove from vector
    auto vecIt = std::find(m_scenes.begin(), m_scenes.end(), scene);
    if (vecIt != m_scenes.end()) {
        m_scenes.erase(vecIt);
    }
    
    // Clear active scene if this was it
    if (m_activeScene == scene) {
        m_activeScene->setActive(false);
        m_activeScene = nullptr;
    }
    
    delete scene;
    return true;
}

Scene* World::getScene(SceneID sceneId) {
    if (sceneId == INVALID_SCENE_ID) {
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_sceneMap.find(sceneId);
    if (it == m_sceneMap.end()) {
        return nullptr;
    }
    
    return it->second;
}

const Scene* World::getScene(SceneID sceneId) const {
    if (sceneId == INVALID_SCENE_ID) {
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_sceneMap.find(sceneId);
    if (it == m_sceneMap.end()) {
        return nullptr;
    }
    
    return it->second;
}

Scene* World::getScene(const std::string& name) {
    if (name.empty()) {
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_nameMap.find(name);
    if (it == m_nameMap.end()) {
        return nullptr;
    }
    
    return it->second;
}

const Scene* World::getScene(const std::string& name) const {
    if (name.empty()) {
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_nameMap.find(name);
    if (it == m_nameMap.end()) {
        return nullptr;
    }
    
    return it->second;
}

bool World::hasScene(SceneID sceneId) const noexcept {
    if (sceneId == INVALID_SCENE_ID) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_sceneMap.find(sceneId) != m_sceneMap.end();
}

bool World::hasScene(const std::string& name) const noexcept {
    if (name.empty()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_nameMap.find(name) != m_nameMap.end();
}

Scene* World::getActiveScene() noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_activeScene;
}

const Scene* World::getActiveScene() const noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_activeScene;
}

void World::setActiveScene(SceneID sceneId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Deactivate old scene
    if (m_activeScene != nullptr) {
        m_activeScene->setActive(false);
    }
    
    if (sceneId == INVALID_SCENE_ID) {
        m_activeScene = nullptr;
        return;
    }
    
    auto it = m_sceneMap.find(sceneId);
    if (it != m_sceneMap.end()) {
        m_activeScene = it->second;
        m_activeScene->setActive(true);
    } else {
        m_activeScene = nullptr;
    }
}

void World::clear() noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Deactivate active scene
    if (m_activeScene != nullptr) {
        m_activeScene->setActive(false);
    }
    
    for (auto* scene : m_scenes) {
        delete scene;
    }
    
    m_scenes.clear();
    m_sceneMap.clear();
    m_nameMap.clear();
    m_activeScene = nullptr;
}

} // namespace world
} // namespace core
} // namespace poko
