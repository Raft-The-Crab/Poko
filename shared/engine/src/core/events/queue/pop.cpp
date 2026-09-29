/**
 * @file pop.cpp
 * @brief EventQueue pop implementation
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

std::unique_ptr<Event> EventQueue::pop() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_events.empty()) {
        return nullptr;
    }
    
    auto event = std::move(m_events.back());
    m_events.pop_back();
    return event;
}

} // namespace events
} // namespace core
} // namespace poko
