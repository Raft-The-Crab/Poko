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

[[nodiscard]] HandleManagerStats HandleManager::getStats() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Create stats snapshot from atomic counters
    HandleManagerStats stats;
    stats.totalAllocated = m_totalAllocated.load(std::memory_order_relaxed);
    stats.totalFreed = m_totalFreed.load(std::memory_order_relaxed);
    stats.activeHandles = m_activeCount.load(std::memory_order_relaxed);
    stats.freeListSize = m_freeList.size();
    
    return stats;
}

} // namespace handles
} // namespace core
} // namespace poko
