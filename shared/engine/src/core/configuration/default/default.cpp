/**
 * @file default.cpp
 * @brief Configuration default value operations implementation
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
 * @brief Set default value
 * 
 * Default values are used when a key is not found in the main configuration.
 * 
 * @param key Configuration key
 * @param value Default value
 */
void Configuration::setDefault(const std::string& key, const ConfigValue& value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_defaults[key] = value;
}

} // namespace configuration
} // namespace core
} // namespace poko
