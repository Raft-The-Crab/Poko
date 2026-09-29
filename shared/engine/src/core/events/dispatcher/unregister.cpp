/**
 * @file unregister.cpp
 * @brief EventDispatcher unregister implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/events/event.h"

namespace poko {
namespace core {
namespace events {

bool EventDispatcher::unregisterHandler(size_t connectionId) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it) {
        if (it->connectionId == connectionId) {
            m_handlers.erase(it);
            return true;
        }
    }
    
    return false;
}

} // namespace events
} // namespace core
} // namespace poko
