/**
 * @file properties.cpp
 * @brief Instance properties implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/runtime/instance.h"

namespace poko {
namespace core {
namespace runtime {

void Instance::setProperty(const std::string& key, const std::string& value) {
    m_properties[key] = value;
}

std::string Instance::getProperty(const std::string& key) const {
    auto it = m_properties.find(key);
    if (it != m_properties.end()) {
        return it->second;
    }
    return "";
}

bool Instance::hasProperty(const std::string& key) const {
    return m_properties.find(key) != m_properties.end();
}

void Instance::removeProperty(const std::string& key) {
    m_properties.erase(key);
}

} // namespace runtime
} // namespace core
} // namespace poko
