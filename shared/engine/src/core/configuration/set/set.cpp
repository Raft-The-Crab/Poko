/**
 * @file set.cpp
 * @brief Configuration set operation implementation
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
 * @brief Set a configuration value
 * 
 * Thread-safe operation that sets or updates a configuration value.
 * If the key already exists, its value is overwritten.
 * Uses exclusive lock for write operations.
 * 
 * @param key Configuration key (non-empty string)
 * @param value Configuration value to store
 * 
 * @note Empty keys are rejected (no-op)
 * @note Thread-safe via shared mutex (exclusive lock for writes)
 */
void Configuration::set(const std::string& key, const ConfigValue& value) {
    // Reject empty keys as they are not meaningful
    if (key.empty()) {
        return;
    }
    
    // Use unique_lock for write operations - exclusive access
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    m_values[key] = value;
}

} // namespace configuration
} // namespace core
} // namespace poko
