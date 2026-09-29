/**
 * @file interface.cpp
 * @brief Core events system interface implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/events/event.h"
#include <mutex>

namespace poko {
namespace core {
namespace events {

// ============================================================================
// Global Event Dispatcher
// ============================================================================

namespace {
    std::mutex g_dispatcherMutex;
    EventDispatcher g_dispatcherInstance;
}

/**
 * @brief Global event dispatcher instance
 * 
 * Points to the globally shared EventDispatcher instance.
 * Used throughout the engine for event routing.
 */
EventDispatcher* g_eventDispatcher = &g_dispatcherInstance;

/**
 * @brief Get global event dispatcher
 * 
 * Returns a reference to the globally shared EventDispatcher instance.
 * This dispatcher is intended for general-purpose event handling throughout the engine.
 * 
 * @return Reference to global event dispatcher
 * 
 * @note Thread-safe access is guaranteed
 * @note Do not destroy this dispatcher - it's globally managed
 */
EventDispatcher& getEventDispatcher() {
    std::lock_guard<std::mutex> lock(g_dispatcherMutex);
    return *g_eventDispatcher;
}

} // namespace events
} // namespace core
} // namespace poko
