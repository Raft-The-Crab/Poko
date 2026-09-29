/**
 * @file queue.cpp
 * @brief EventDispatcher queue implementation
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

void EventDispatcher::queue(std::unique_ptr<Event> event) {
    m_queue.push(std::move(event));
}

size_t EventDispatcher::processQueue() {
    size_t processedCount = 0;
    
    while (auto event = m_queue.pop()) {
        dispatch(event.get());
        processedCount++;
    }
    
    return processedCount;
}

EventQueue& EventDispatcher::getQueue() {
    return m_queue;
}

void EventDispatcher::clearQueue() {
    m_queue.clear();
}

} // namespace events
} // namespace core
} // namespace poko
