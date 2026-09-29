/**
 * @file registry.cpp
 * @brief Component registry implementation
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

bool ComponentRegistry::registerComponent(ComponentID typeId, const char* typeName, ComponentFactory factory) {
    // Validate inputs
    if (typeId == INVALID_COMPONENT_ID) {
        return false;
    }
    if (typeName == nullptr || typeName[0] == '\0') {
        return false;
    }
    if (!factory) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_types.find(typeId) != m_types.end()) {
        return false; // Already registered
    }
    
    ComponentTypeInfo info;
    info.typeName = typeName;
    info.factory = std::move(factory);
    
    m_types[typeId] = std::move(info);
    return true;
}

bool ComponentRegistry::unregisterComponent(ComponentID typeId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_types.find(typeId);
    if (it == m_types.end()) {
        return false;
    }
    
    m_types.erase(it);
    return true;
}

std::unique_ptr<Component> ComponentRegistry::createComponent(ComponentID typeId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_types.find(typeId);
    if (it == m_types.end()) {
        return nullptr;
    }
    
    return it->second.factory();
}

bool ComponentRegistry::isRegistered(ComponentID typeId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_types.find(typeId) != m_types.end();
}

const char* ComponentRegistry::getTypeName(ComponentID typeId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_types.find(typeId);
    if (it == m_types.end()) {
        return "";
    }
    
    return it->second.typeName;
}

std::vector<ComponentID> ComponentRegistry::getRegisteredTypes() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<ComponentID> types;
    types.reserve(m_types.size());
    
    for (const auto& pair : m_types) {
        types.push_back(pair.first);
    }
    
    return types;
}

size_t ComponentRegistry::getRegisteredCount() const noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_types.size();
}

void ComponentRegistry::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_types.clear();
}

} // namespace components
} // namespace core
} // namespace poko
