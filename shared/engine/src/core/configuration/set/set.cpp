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
 * 
 * @param key Configuration key
 * @param value Configuration value
 */
void Configuration::set(const std::string& key, const ConfigValue& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_values[key] = value;
}

} // namespace configuration
} // namespace core
} // namespace poko
