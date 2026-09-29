/**
 * @file register.cpp
 * @brief EventDispatcher register implementation
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

size_t EventDispatcher::registerHandler(EventId eventId, EventCallback callback) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    size_t id = m_nextConnectionId.fetch_add(1, std::memory_order_relaxed);
    m_handlers.push_back({id, eventId, std::move(callback)});
    
    return id;
}

} // namespace events
} // namespace core
} // namespace poko
