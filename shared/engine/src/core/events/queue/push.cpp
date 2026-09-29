/**
 * @file push.cpp
 * @brief EventQueue push implementation
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

void EventQueue::push(std::unique_ptr<Event> event) {
    // Validate event
    if (!event) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Insert in priority order (higher priority comes first)
    // Find the first position where priority is lower than our event
    auto it = m_events.begin();
    while (it != m_events.end() && (*it)->getPriority() < event->getPriority()) {
        ++it;
    }
    m_events.insert(it, std::move(event));
}

} // namespace events
} // namespace core
} // namespace poko
