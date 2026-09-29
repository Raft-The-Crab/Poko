/**
 * @file registry.cpp
 * @brief EventTypeRegistry implementation
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

std::atomic<EventId> EventTypeRegistry::s_nextId{1};

EventId EventTypeRegistry::getNextId() noexcept {
    return s_nextId.fetch_add(1, std::memory_order_relaxed);
}

void EventTypeRegistry::reset() noexcept {
    s_nextId.store(1, std::memory_order_relaxed);
}

} // namespace events
} // namespace core
} // namespace poko
