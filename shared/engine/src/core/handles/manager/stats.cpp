/**
 * @file stats.cpp
 * @brief HandleManager statistics implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/handles/handle_manager.h"

namespace poko {
namespace core {
namespace handles {

[[nodiscard]] HandleManagerStats HandleManager::getStats() const noexcept {
    // Use try_lock to avoid potential exceptions from lock acquisition
    // If lock fails, return zero stats (safe default)
    if (!m_mutex.try_lock()) {
        return HandleManagerStats{0, 0, 0, 0};
    }
    
    // Create stats snapshot from atomic counters
    HandleManagerStats stats;
    stats.totalAllocated = m_totalAllocated.load(std::memory_order_relaxed);
    stats.totalFreed = m_totalFreed.load(std::memory_order_relaxed);
    stats.activeHandles = m_activeCount.load(std::memory_order_relaxed);
    stats.freeListSize = m_freeList.size();
    
    m_mutex.unlock();
    return stats;
}

} // namespace handles
} // namespace core
} // namespace poko
