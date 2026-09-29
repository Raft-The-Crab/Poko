/**
 * @file registry.cpp
 * @brief Property registry implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/properties/property.h"

namespace poko {
namespace core {
namespace properties {

bool PropertyRegistry::registerProperty(const std::string& name, const PropertyMetadata& metadata) {
    // Validate name length
    if (name.empty() || name.length() > MAX_PROPERTY_NAME_LENGTH) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check registry size limit
    if (m_properties.size() >= MAX_PROPERTIES_PER_REGISTRY) {
        return false;
    }
    
    if (m_properties.find(name) != m_properties.end()) {
        return false; // Already exists
    }
    
    auto property = std::make_unique<Property>(metadata);
    m_properties[name] = std::move(property);
    return true;
}

bool PropertyRegistry::registerProperty(const std::string& name, const PropertyMetadata& metadata, const PropertyValue& value) {
    // Validate name length
    if (name.empty() || name.length() > MAX_PROPERTY_NAME_LENGTH) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check registry size limit
    if (m_properties.size() >= MAX_PROPERTIES_PER_REGISTRY) {
        return false;
    }
    
    if (m_properties.find(name) != m_properties.end()) {
        return false; // Already exists
    }
    
    auto property = std::make_unique<Property>(metadata, value);
    m_properties[name] = std::move(property);
    return true;
}

bool PropertyRegistry::unregisterProperty(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_properties.find(name);
    if (it == m_properties.end()) {
        return false;
    }
    
    m_properties.erase(it);
    return true;
}

Property* PropertyRegistry::getProperty(const std::string& name) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_properties.find(name);
    if (it == m_properties.end()) {
        return nullptr;
    }
    
    return it->second.get();
}

const Property* PropertyRegistry::getProperty(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_properties.find(name);
    if (it == m_properties.end()) {
        return nullptr;
    }
    
    return it->second.get();
}

bool PropertyRegistry::getValue(const std::string& name, PropertyValue& outValue) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_properties.find(name);
    if (it == m_properties.end()) {
        return false;
    }
    
    outValue = it->second->getValue();
    return true;
}

bool PropertyRegistry::setValue(const std::string& name, const PropertyValue& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_properties.find(name);
    if (it == m_properties.end()) {
        return false;
    }
    
    return it->second->setValue(value);
}

std::vector<std::string> PropertyRegistry::getPropertyNames() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<std::string> names;
    names.reserve(m_properties.size());
    
    for (const auto& pair : m_properties) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::vector<std::string> PropertyRegistry::getPropertiesByCategory(const std::string& category) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<std::string> names;
    
    for (const auto& pair : m_properties) {
        if (pair.second->getMetadata().category == category) {
            names.push_back(pair.first);
        }
    }
    
    return names;
}

size_t PropertyRegistry::getPropertyCount() const noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_properties.size();
}

bool PropertyRegistry::hasProperty(const std::string& name) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_properties.find(name) != m_properties.end();
}

void PropertyRegistry::resetAllToDefault() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (auto& pair : m_properties) {
        pair.second->resetToDefault();
    }
}

void PropertyRegistry::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_properties.clear();
}

} // namespace properties
} // namespace core
} // namespace poko
