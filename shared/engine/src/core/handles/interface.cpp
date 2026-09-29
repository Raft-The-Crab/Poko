/**
 * @file interface.cpp
 * @brief Global handle manager instance
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/handles/handle_manager.h"
#include <mutex>

namespace poko {
namespace core {
namespace handles {

// Global handle manager instance
static HandleManager g_globalHandleManager;
static std::mutex g_globalHandleManagerMutex;

HandleManager* g_handleManager = &g_globalHandleManager;

HandleManager& getHandleManager() {
    std::lock_guard<std::mutex> lock(g_globalHandleManagerMutex);
    return *g_handleManager;
}

} // namespace handles
} // namespace core
} // namespace poko
