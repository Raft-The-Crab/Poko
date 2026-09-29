/**
 * @file manager.cpp
 * @brief Component manager implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/component.h"

namespace poko {
namespace core {
namespace components {

bool ComponentManager::attachComponent(uint64_t instanceId, std::unique_ptr<Component> component) {
    // Validate inputs
    if (!component) {
        return false;
    }
    
    ComponentID typeId = component->getTypeID();
    if (typeId == INVALID_COMPONENT_ID) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Find or create component map for this instance
    auto& components = m_instanceComponents[instanceId];
    
    // Check if component of this type already exists
    if (components.find(typeId) != components.end()) {
        return false; // Already attached
    }
    
    components[typeId] = std::move(component);
    return true;
}

bool ComponentManager::detachComponent(uint64_t instanceId, ComponentID typeId) {
    // Validate inputs
    if (typeId == INVALID_COMPONENT_ID) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto instanceIt = m_instanceComponents.find(instanceId);
    if (instanceIt == m_instanceComponents.end()) {
        return false; // Instance not found
    }
    
    auto& components = instanceIt->second;
    auto componentIt = components.find(typeId);
    
    if (componentIt == components.end()) {
        return false; // Component not found
    }
    
    components.erase(componentIt);
    
    // Remove instance entry if no components left
    if (components.empty()) {
        m_instanceComponents.erase(instanceIt);
    }
    
    return true;
}

Component* ComponentManager::getComponent(uint64_t instanceId, ComponentID typeId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto instanceIt = m_instanceComponents.find(instanceId);
    if (instanceIt == m_instanceComponents.end()) {
        return nullptr;
    }
    
    auto& components = instanceIt->second;
    auto componentIt = components.find(typeId);
    
    if (componentIt == components.end()) {
        return nullptr;
    }
    
    return componentIt->second.get();
}

const Component* ComponentManager::getComponent(uint64_t instanceId, ComponentID typeId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto instanceIt = m_instanceComponents.find(instanceId);
    if (instanceIt == m_instanceComponents.end()) {
        return nullptr;
    }
    
    const auto& components = instanceIt->second;
    auto componentIt = components.find(typeId);
    
    if (componentIt == components.end()) {
        return nullptr;
    }
    
    return componentIt->second.get();
}

bool ComponentManager::hasComponent(uint64_t instanceId, ComponentID typeId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto instanceIt = m_instanceComponents.find(instanceId);
    if (instanceIt == m_instanceComponents.end()) {
        return false;
    }
    
    const auto& components = instanceIt->second;
    return components.find(typeId) != components.end();
}

std::vector<Component*> ComponentManager::getComponents(uint64_t instanceId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<Component*> result;
    
    auto instanceIt = m_instanceComponents.find(instanceId);
    if (instanceIt == m_instanceComponents.end()) {
        return result;
    }
    
    auto& components = instanceIt->second;
    result.reserve(components.size());
    
    for (auto& pair : components) {
        result.push_back(pair.second.get());
    }
    
    return result;
}

std::vector<const Component*> ComponentManager::getComponents(uint64_t instanceId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<const Component*> result;
    
    auto instanceIt = m_instanceComponents.find(instanceId);
    if (instanceIt == m_instanceComponents.end()) {
        return result;
    }
    
    const auto& components = instanceIt->second;
    result.reserve(components.size());
    
    for (const auto& pair : components) {
        result.push_back(pair.second.get());
    }
    
    return result;
}

std::vector<ComponentID> ComponentManager::getComponentTypes(uint64_t instanceId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<ComponentID> result;
    
    auto instanceIt = m_instanceComponents.find(instanceId);
    if (instanceIt == m_instanceComponents.end()) {
        return result;
    }
    
    const auto& components = instanceIt->second;
    result.reserve(components.size());
    
    for (const auto& pair : components) {
        result.push_back(pair.first);
    }
    
    return result;
}

size_t ComponentManager::getComponentCount(uint64_t instanceId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto instanceIt = m_instanceComponents.find(instanceId);
    if (instanceIt == m_instanceComponents.end()) {
        return 0;
    }
    
    return instanceIt->second.size();
}

void ComponentManager::removeComponents(uint64_t instanceId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_instanceComponents.erase(instanceId);
}

void ComponentManager::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_instanceComponents.clear();
}

} // namespace components
} // namespace core
} // namespace poko
