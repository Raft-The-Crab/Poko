/**
 * @file interface.cpp
 * @brief Global component system interface
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/component.h"
#include <mutex>

namespace poko {
namespace core {
namespace components {

namespace {
    std::unique_ptr<ComponentRegistry> g_globalRegistry;
    std::mutex g_registryMutex;
} // anonymous namespace

ComponentRegistry& getGlobalComponentRegistry() {
    std::lock_guard<std::mutex> lock(g_registryMutex);
    
    if (!g_globalRegistry) {
        g_globalRegistry = std::make_unique<ComponentRegistry>();
    }
    
    return *g_globalRegistry;
}

void destroyGlobalComponentRegistry() {
    std::lock_guard<std::mutex> lock(g_registryMutex);
    g_globalRegistry.reset();
}

} // namespace components
} // namespace core
} // namespace poko
