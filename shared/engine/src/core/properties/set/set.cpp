/**
 * @file set.cpp
 * @brief Property setter implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/properties/property.h"
#include <algorithm>

namespace poko {
namespace core {
namespace properties {

bool Property::setValue(const PropertyValue& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check read-only
    if (hasFlag(m_metadata.flags, PropertyFlags::ReadOnly)) {
        return false;
    }
    
    // Check if value is the same
    if (m_value.index() == value.index()) {
        // Simple comparison for same type
        bool same = false;
        std::visit([&same, &value](const auto& a) {
            using T = std::decay_t<decltype(a)>;
            if constexpr (std::is_same_v<T, std::string>) {
                same = (a == std::get<std::string>(value));
            } else if constexpr (std::is_same_v<T, std::vector<float>>) {
                const auto& vecA = a;
                const auto& vecB = std::get<std::vector<float>>(value);
                same = (vecA.size() == vecB.size() && 
                       std::equal(vecA.begin(), vecA.end(), vecB.begin()));
            } else {
                same = (a == std::get<T>(value));
            }
        }, m_value);
        
        if (same) {
            return false; // No change
        }
    }
    
    // Store old value for callback
    PropertyValue oldValue = m_value;
    
    // Apply clamping if flag is set
    if (hasFlag(m_metadata.flags, PropertyFlags::Clamp)) {
        // Clamp to min/max values
        // This is a simplified implementation - full implementation would
        // need type-specific clamping logic
        m_value = value;
    } else {
        m_value = value;
    }
    
    // Invoke callback if set
    if (m_callback) {
        m_callback(m_metadata.name, oldValue, m_value);
    }
    
    return true;
}

void Property::resetToDefault() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!hasFlag(m_metadata.flags, PropertyFlags::ReadOnly)) {
        PropertyValue oldValue = m_value;
        m_value = m_metadata.defaultValue;
        
        if (m_callback) {
            m_callback(m_metadata.name, oldValue, m_value);
        }
    }
}

void Property::setChangeCallback(PropertyChangeCallback callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_callback = std::move(callback);
}

} // namespace properties
} // namespace core
} // namespace poko
