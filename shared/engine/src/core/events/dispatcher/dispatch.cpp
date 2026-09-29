/**
 * @file dispatch.cpp
 * @brief EventDispatcher dispatch implementation
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

size_t EventDispatcher::dispatch(Event* event) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    size_t handledCount = 0;
    for (const auto& handler : m_handlers) {
        if (handler.eventId == event->getId()) {
            if (handler.callback(event)) {
                handledCount++;
            }
        }
    }
    
    return handledCount;
}

} // namespace events
} // namespace core
} // namespace poko
