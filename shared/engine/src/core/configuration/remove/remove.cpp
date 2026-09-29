/**
 * @file remove.cpp
 * @brief Configuration remove and has operations implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/configuration/configuration.h"

namespace poko {
namespace core {
namespace configuration {

/**
 * @brief Check if key exists
 * 
 * Checks both user-set values and default values.
 * A key exists if it's in either the values map or defaults map.
 * Uses shared lock for concurrent reads.
 * 
 * @param key Configuration key (non-empty string)
 * @return True if key exists in values or defaults
 * 
 * @note Empty keys always return false
 * @note Thread-safe via shared mutex (allows concurrent reads)
 */
bool Configuration::has(const std::string& key) const {
    // Empty keys are considered non-existent
    if (key.empty()) {
        return false;
    }
    
    // Use shared_lock for read operations - allows concurrent reads
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    return m_values.find(key) != m_values.end() || m_defaults.find(key) != m_defaults.end();
}

/**
 * @brief Remove a configuration value
 * 
 * Removes a key from the user-set values.
 * If the key has a default value, the default remains accessible.
 * Uses exclusive lock for write operations.
 * 
 * @param key Configuration key (non-empty string)
 * @return True if key was found and removed from user values
 * 
 * @note Empty keys return false (no-op)
 * @note Only removes user-set values, not defaults
 * @note Thread-safe via shared mutex (exclusive lock for writes)
 */
bool Configuration::remove(const std::string& key) {
    // Empty keys cannot be removed
    if (key.empty()) {
        return false;
    }
    
    // Use unique_lock for write operations - exclusive access
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    return m_values.erase(key) > 0;
}

} // namespace configuration
} // namespace core
} // namespace poko
