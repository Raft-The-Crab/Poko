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

namespace poko {
namespace core {
namespace memory {

void SystemAllocator::reset() noexcept {
    // System allocator doesn't support reset - individual deallocation required
    // Stats can be reset if tracking is enabled
    // Use try_lock to avoid potential exceptions from lock acquisition
    // If lock fails, skip the reset - acceptable for a reset operation
    if (m_mutex.try_lock()) {
        if (m_trackingEnabled.load(std::memory_order_relaxed)) {
            m_stats.totalAllocated.store(0, std::memory_order_relaxed);
            m_stats.totalFreed.store(0, std::memory_order_relaxed);
            m_stats.currentUsage.store(0, std::memory_order_relaxed);
            m_stats.peakUsage.store(0, std::memory_order_relaxed);
            m_stats.allocationCount.store(0, std::memory_order_relaxed);
            m_stats.deallocationCount.store(0, std::memory_order_relaxed);
        }
        m_mutex.unlock();
    }
}

} // namespace memory
} // namespace core
} // namespace poko