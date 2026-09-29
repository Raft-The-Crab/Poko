/**
 * @file interface.cpp
 * @brief Global property system interface
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/properties/property.h"
#include <mutex>

namespace poko {
namespace core {
namespace properties {

namespace {
    std::unique_ptr<PropertyRegistry> g_globalRegistry;
    std::mutex g_registryMutex;
} // anonymous namespace

/**
 * @brief Get the global property registry
 * Creates the registry on first call
 */
PropertyRegistry& getGlobalPropertyRegistry() {
    std::lock_guard<std::mutex> lock(g_registryMutex);
    
    if (!g_globalRegistry) {
        g_globalRegistry = std::make_unique<PropertyRegistry>();
    }
    
    return *g_globalRegistry;
}

/**
 * @brief Destroy the global property registry
 * Called during engine shutdown
 */
void destroyGlobalPropertyRegistry() {
    std::lock_guard<std::mutex> lock(g_registryMutex);
    g_globalRegistry.reset();
}

} // namespace properties
} // namespace core
} // namespace poko
