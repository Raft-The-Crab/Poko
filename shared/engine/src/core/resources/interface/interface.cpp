/**
 * @file interface.cpp
 * @brief Global resource manager interface implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/resources/resource.h"
#include <memory>

namespace poko {
namespace core {
namespace resources {

namespace {
    std::unique_ptr<ResourceManager> g_globalResourceManager;
    std::mutex g_globalMutex;
}

ResourceManager& getGlobalResourceManager() {
    std::lock_guard<std::mutex> lock(g_globalMutex);
    
    if (!g_globalResourceManager) {
        g_globalResourceManager = std::make_unique<ResourceManager>();
    }
    
    return *g_globalResourceManager;
}

void destroyGlobalResourceManager() {
    std::lock_guard<std::mutex> lock(g_globalMutex);
    g_globalResourceManager.reset();
}

} // namespace resources
} // namespace core
} // namespace poko
