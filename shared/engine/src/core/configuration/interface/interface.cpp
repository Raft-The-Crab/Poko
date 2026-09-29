/**
 * @file interface.cpp
 * @brief Configuration interface implementation - global configuration instance
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/configuration/configuration.h"
#include <mutex>

namespace poko {
namespace core {
namespace configuration {

namespace {
    std::mutex g_configMutex;
    Configuration* g_globalConfiguration = nullptr;
}

/**
 * @brief Get global configuration
 * 
 * Returns a reference to the globally shared Configuration instance.
 * This configuration is intended for general-purpose configuration management throughout the engine.
 * 
 * @return Reference to global configuration
 * 
 * @note Thread-safe access is guaranteed
 * @note Do not destroy this configuration - it's globally managed
 */
Configuration& getGlobalConfiguration() {
    std::lock_guard<std::mutex> lock(g_configMutex);
    
    if (!g_globalConfiguration) {
        static Configuration instance;
        g_globalConfiguration = &instance;
    }
    
    return *g_globalConfiguration;
}

} // namespace configuration
} // namespace core
} // namespace poko
