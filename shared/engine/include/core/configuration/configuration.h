/**
 * @file configuration.h
 * @brief Core configuration system header
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_CONFIGURATION_CONFIGURATION_H
#define POKO_CORE_CONFIGURATION_CONFIGURATION_H

#include <string>
#include <map>
#include <variant>
#include <optional>
#include <mutex>
#include <shared_mutex>
#include <vector>

namespace poko {
namespace core {
namespace configuration {

/**
 * @brief Configuration value type
 * 
 * Supports various value types: bool, int, double, string
 */
using ConfigValue = std::variant<bool, int, double, std::string>;

/**
 * @brief Configuration system class
 * 
 * Provides runtime configuration management with:
 * - Key-value storage with type safety
 * - Thread-safe access
 * - Configuration file loading
 * - Default values
 * - Change notifications
 */
class Configuration {
public:
    /**
     * @brief Constructor
     */
    Configuration() noexcept;
    
    /**
     * @brief Destructor
     */
    ~Configuration();
    
    /**
     * @brief Copy constructor (deleted)
     */
    Configuration(const Configuration&) = delete;
    
    /**
     * @brief Copy assignment (deleted)
     */
    Configuration& operator=(const Configuration&) = delete;
    
    /**
     * @brief Move constructor
     */
    Configuration(Configuration&&) noexcept = default;
    
    /**
     * @brief Move assignment
     */
    Configuration& operator=(Configuration&&) noexcept = default;

    /**
     * @brief Set a configuration value
     * 
     * @param key Configuration key
     * @param value Configuration value
     * 
     * @note Thread-safe
     */
    void set(const std::string& key, const ConfigValue& value);
    
    /**
     * @brief Set a configuration value (move overload)
     * 
     * @param key Configuration key (moved)
     * @param value Configuration value (moved)
     * 
     * @note Thread-safe
     */
    void set(std::string&& key, ConfigValue&& value) noexcept;

    /**
     * @brief Get a configuration value
     * 
     * @param key Configuration key
     * @return Optional containing the value if key exists
     * 
     * @note Thread-safe
     */
    [[nodiscard]] std::optional<ConfigValue> get(const std::string& key) const;
    
    /**
     * @brief Get a configuration value with default
     * 
     * @param key Configuration key
     * @param defaultValue Default value if key doesn't exist
     * @return The value or default
     * 
     * @note Thread-safe
     */
    template<typename T>
    [[nodiscard]] T getOrDefault(const std::string& key, const T& defaultValue) const {
        auto value = get(key);
        if (value && std::holds_alternative<T>(*value)) {
            return std::get<T>(*value);
        }
        return defaultValue;
    }
    
    /**
     * @brief Get boolean value
     * 
     * @param key Configuration key
     * @return Optional containing the value if key exists and is bool
     * 
     * @note Thread-safe
     */
    [[nodiscard]] std::optional<bool> getBool(const std::string& key) const;
    
    /**
     * @brief Get integer value
     * 
     * @param key Configuration key
     * @return Optional containing the value if key exists and is int
     * 
     * @note Thread-safe
     */
    [[nodiscard]] std::optional<int> getInt(const std::string& key) const;
    
    /**
     * @brief Get double value
     * 
     * @param key Configuration key
     * @return Optional containing the value if key exists and is double
     * 
     * @note Thread-safe
     */
    [[nodiscard]] std::optional<double> getDouble(const std::string& key) const;
    
    /**
     * @brief Get string value
     * 
     * @param key Configuration key
     * @return Optional containing the value if key exists and is string
     * 
     * @note Thread-safe
     */
    [[nodiscard]] std::optional<std::string> getString(const std::string& key) const;
    
    /**
     * @brief Check if key exists
     * 
     * @param key Configuration key
     * @return True if key exists
     * 
     * @note Thread-safe
     */
    [[nodiscard]] bool has(const std::string& key) const noexcept;

    /**
     * @brief Remove a configuration value
     * 
     * @param key Configuration key
     * @return True if key was removed
     * 
     * @note Thread-safe
     */
    bool remove(const std::string& key) noexcept;
    
    /**
     * @brief Clear all configuration values
     * 
     * @note Thread-safe
     */
    void clear();
    
    /**
     * @brief Get number of configuration entries
     * 
     * @return Number of entries
     * 
     * @note Thread-safe
     */
    [[nodiscard]] size_t size() const;

    /**
     * @brief Load configuration from file
     * 
     * @param filepath Path to configuration file
     * @return True if loaded successfully
     * 
     * @note Thread-safe
     */
    bool loadFromFile(const std::string& filepath);

    /**
     * @brief Save configuration to file
     * 
     * @param filepath Path to configuration file
     * @return True if saved successfully
     * 
     * @note Thread-safe
     */
    bool saveToFile(const std::string& filepath) const;

    /**
     * @brief Set default value
     * 
     * Default values are used when a key is not found
     * 
     * @param key Configuration key
     * @param value Default value
     * 
     * @note Thread-safe
     */
    void setDefault(const std::string& key, const ConfigValue& value);

    /**
     * @brief Get all keys
     * 
     * @return Vector of all configuration keys
     * 
     * @note Thread-safe
     */
    [[nodiscard]] std::vector<std::string> getAllKeys() const;

private:
    mutable std::shared_mutex m_mutex;  ///< Shared mutex for read-write optimization
    std::map<std::string, ConfigValue> m_values;
    std::map<std::string, ConfigValue> m_defaults;
};

/**
 * @brief Global configuration instance
 * 
 * Provides access to the globally shared configuration system
 * 
 * @return Reference to global configuration
 * 
 * @note Thread-safe
 * @note Do not destroy this instance - it's globally managed
 */
Configuration& getGlobalConfiguration();

} // namespace configuration
} // namespace core
} // namespace poko

#endif // POKO_CORE_CONFIGURATION_CONFIGURATION_H
