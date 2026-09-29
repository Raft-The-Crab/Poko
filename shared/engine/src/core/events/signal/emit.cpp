/**
 * @file emit.cpp
 * @brief Signal emit implementation
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

size_t Signal::emit(Event* event) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    size_t handledCount = 0;
    for (const auto& connection : m_connections) {
        if (connection.callback(event)) {
            handledCount++;
        }
    }
    
    return handledCount;
}

size_t Signal::getConnectionCount() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_connections.size();
}

} // namespace events
} // namespace core
} // namespace poko
