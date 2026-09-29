/**
 * @file info.cpp
 * @brief EventQueue info implementation
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

bool EventQueue::empty() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_events.empty();
}

size_t EventQueue::size() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_events.size();
}

void EventQueue::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_events.clear();
}

} // namespace events
} // namespace core
} // namespace poko
