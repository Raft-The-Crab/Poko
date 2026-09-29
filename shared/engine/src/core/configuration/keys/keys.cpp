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
 * Returns all user-set configuration keys.
 * Uses shared lock for concurrent reads.
 * 
 * @return Vector of all configuration keys
 * 
 * @note Only returns user-set keys, not default keys
 * @note Thread-safe via shared mutex (allows concurrent reads)
 */
std::vector<std::string> Configuration::getAllKeys() const {
    // Use shared_lock for read operations - allows concurrent reads
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
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
