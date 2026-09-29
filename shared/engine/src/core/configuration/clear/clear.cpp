/**
 * @file clear.cpp
 * @brief Configuration clear and size operations implementation
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
 * @brief Clear all configuration values
 * 
 * Removes all configuration values but keeps defaults.
 */
void Configuration::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_values.clear();
}

/**
 * @brief Get number of configuration entries
 * 
 * @return Number of entries
 */
size_t Configuration::size() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_values.size();
}

} // namespace configuration
} // namespace core
} // namespace poko
