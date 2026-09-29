/**
 * @file tracking.cpp
 * @brief System allocator tracking implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/memory/allocator.h"
#include <mutex>

namespace poko {
namespace core {
namespace memory {

void SystemAllocator::setTrackingEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_trackingEnabled = enabled;
}

bool SystemAllocator::isTrackingEnabled() const {
    // Fast path: read atomic flag without lock for performance
    // In a more sophisticated implementation, this could be std::atomic<bool>
    return m_trackingEnabled;
}

} // namespace memory
} // namespace core
} // namespace poko