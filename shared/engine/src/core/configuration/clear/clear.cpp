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
 * Removes all user-set configuration values but preserves default values.
 * After clearing, all keys will return their default values (if set).
 * 
 * @note This is a destructive operation that cannot be undone
 * @note Default values are preserved
 * @note Thread-safe via mutex protection
 */
void Configuration::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_values.clear();
}

/**
 * @brief Get number of configuration entries
 * 
 * Returns the count of user-set configuration values.
 * Does not include default values in the count.
 * 
 * @return Number of user-set configuration entries
 * 
 * @note Only counts user-set values, not defaults
 * @note Thread-safe via mutex protection
 */
size_t Configuration::size() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_values.size();
}

} // namespace configuration
} // namespace core
} // namespace poko
