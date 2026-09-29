/**
 * @file instances.cpp
 * @brief Scene instance management implementation
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

bool Scene::addInstance(runtime::Instance* instance) {
    if (!instance) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check if already in scene
    auto it = std::find(m_instances.begin(), m_instances.end(), instance);
    if (it != m_instances.end()) {
        return false;
    }
    
    // Check instance limit
    if (m_instances.size() >= MAX_INSTANCES_PER_SCENE) {
        return false;
    }
    
    m_instances.push_back(instance);
    return true;
}

bool Scene::removeInstance(runtime::Instance* instance) {
    if (!instance) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = std::find(m_instances.begin(), m_instances.end(), instance);
    if (it == m_instances.end()) {
        return false;
    }
    
    m_instances.erase(it);
    return true;
}

bool Scene::hasInstance(runtime::Instance* instance) const {
    if (!instance) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = std::find(m_instances.begin(), m_instances.end(), instance);
    return it != m_instances.end();
}

runtime::Instance* Scene::findInstance(const std::string& name) const {
    if (name.empty()) {
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (auto* instance : m_instances) {
        if (instance && instance->getName() == name) {
            return instance;
        }
    }
    
    return nullptr;
}

std::vector<runtime::Instance*> Scene::findInstancesByTag(const std::string& tag) const {
    if (tag.empty()) {
        return {};
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<runtime::Instance*> result;
    for (auto* instance : m_instances) {
        if (instance && instance->hasTag(tag)) {
            result.push_back(instance);
        }
    }
    
    return result;
}

void Scene::clear() noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_instances.clear();
}

} // namespace world
} // namespace core
} // namespace poko
