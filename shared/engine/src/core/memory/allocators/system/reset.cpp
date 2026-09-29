/**
 * @file reset.cpp
 * @brief System allocator reset implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/memory/allocator.h"
#include "core/memory/allocators/interface.h"
#include <mutex>

namespace poko {
namespace core {
namespace memory {

void SystemAllocator::reset() {
    // System allocator doesn't support reset - individual deallocation required
    // Stats can be reset if tracking is enabled
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_trackingEnabled) {
        m_stats.totalAllocated.store(0, std::memory_order_relaxed);
        m_stats.totalFreed.store(0, std::memory_order_relaxed);
        m_stats.currentUsage.store(0, std::memory_order_relaxed);
        m_stats.peakUsage.store(0, std::memory_order_relaxed);
        m_stats.allocationCount.store(0, std::memory_order_relaxed);
        m_stats.deallocationCount.store(0, std::memory_order_relaxed);
    }
}

} // namespace memory
} // namespace core
} // namespace poko