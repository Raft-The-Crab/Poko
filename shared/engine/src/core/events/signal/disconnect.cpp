/**
 * @file disconnect.cpp
 * @brief Signal disconnect implementation
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

bool Signal::disconnect(size_t connectionId) noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (auto it = m_connections.begin(); it != m_connections.end(); ++it) {
        if (it->id == connectionId) {
            m_connections.erase(it);
            return true;
        }
    }
    
    return false;
}

void Signal::disconnectAll() noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_connections.clear();
}

} // namespace events
} // namespace core
} // namespace poko
