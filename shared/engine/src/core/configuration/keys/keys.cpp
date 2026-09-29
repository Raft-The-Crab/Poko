/**
 * @file keys.cpp
 * @brief Configuration get all keys operation implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/configuration/configuration.h"
#include <vector>

namespace poko {
namespace core {
namespace configuration {

/**
 * @brief Get all keys
 * 
 * @return Vector of all configuration keys
 */
std::vector<std::string> Configuration::getAllKeys() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    std::vector<std::string> keys;
    keys.reserve(m_values.size());
    
    for (const auto& [key, value] : m_values) {
        keys.push_back(key);
    }
    
    return keys;
}

} // namespace configuration
} // namespace core
} // namespace poko
