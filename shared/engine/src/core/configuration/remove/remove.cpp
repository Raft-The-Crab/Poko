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
 * @param key Configuration key
 * @return True if key exists
 */
bool Configuration::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_values.find(key) != m_values.end() || m_defaults.find(key) != m_defaults.end();
}

/**
 * @brief Remove a configuration value
 * 
 * @param key Configuration key
 * @return True if key was removed
 */
bool Configuration::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_values.erase(key) > 0;
}

} // namespace configuration
} // namespace core
} // namespace poko
