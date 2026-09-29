/**
 * @file get.cpp
 * @brief Configuration get operations implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/configuration/configuration.h"
#include <optional>

namespace poko {
namespace core {
namespace configuration {

/**
 * @brief Get a configuration value
 * 
 * Thread-safe operation that retrieves a configuration value.
 * 
 * @param key Configuration key
 * @return Optional containing the value if key exists
 */
std::optional<ConfigValue> Configuration::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto it = m_values.find(key);
    if (it != m_values.end()) {
        return it->second;
    }
    
    // Check defaults
    auto defaultIt = m_defaults.find(key);
    if (defaultIt != m_defaults.end()) {
        return defaultIt->second;
    }
    
    return std::nullopt;
}

/**
 * @brief Get boolean value
 * 
 * @param key Configuration key
 * @return Optional containing the value if key exists and is bool
 */
std::optional<bool> Configuration::getBool(const std::string& key) const {
    auto value = get(key);
    if (value && std::holds_alternative<bool>(*value)) {
        return std::get<bool>(*value);
    }
    return std::nullopt;
}

/**
 * @brief Get integer value
 * 
 * @param key Configuration key
 * @return Optional containing the value if key exists and is int
 */
std::optional<int> Configuration::getInt(const std::string& key) const {
    auto value = get(key);
    if (value && std::holds_alternative<int>(*value)) {
        return std::get<int>(*value);
    }
    return std::nullopt;
}

/**
 * @brief Get double value
 * 
 * @param key Configuration key
 * @return Optional containing the value if key exists and is double
 */
std::optional<double> Configuration::getDouble(const std::string& key) const {
    auto value = get(key);
    if (value && std::holds_alternative<double>(*value)) {
        return std::get<double>(*value);
    }
    return std::nullopt;
}

/**
 * @brief Get string value
 * 
 * @param key Configuration key
 * @return Optional containing the value if key exists and is string
 */
std::optional<std::string> Configuration::getString(const std::string& key) const {
    auto value = get(key);
    if (value && std::holds_alternative<std::string>(*value)) {
        return std::get<std::string>(*value);
    }
    return std::nullopt;
}

} // namespace configuration
} // namespace core
} // namespace poko
