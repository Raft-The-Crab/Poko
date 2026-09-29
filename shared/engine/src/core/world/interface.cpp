/**
 * @file interface.cpp
 * @brief Global world interface implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/world/scene.h"
#include <mutex>

namespace poko {
namespace core {
namespace world {

namespace {
    World* g_globalWorld = nullptr;
    std::mutex g_globalWorldMutex;
}

World& getGlobalWorld() {
    std::lock_guard<std::mutex> lock(g_globalWorldMutex);
    
    if (!g_globalWorld) {
        g_globalWorld = new World();
    }
    
    return *g_globalWorld;
}

void destroyGlobalWorld() {
    std::lock_guard<std::mutex> lock(g_globalWorldMutex);
    
    if (g_globalWorld) {
        delete g_globalWorld;
        g_globalWorld = nullptr;
    }
}

} // namespace world
} // namespace core
} // namespace poko
