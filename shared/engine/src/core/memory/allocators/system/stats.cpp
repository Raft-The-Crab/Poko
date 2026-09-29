/**
 * @file stats.cpp
 * @brief System allocator statistics implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/memory/allocator.h"
#include "core/memory/allocators/interface.h"

namespace poko {
namespace core {
namespace memory {

AllocationStats SystemAllocator::getStats() const {
    // Return a snapshot copy of current statistics
    AllocationStats stats;
    stats.totalAllocated = m_stats.totalAllocated.load(std::memory_order_relaxed);
    stats.totalFreed = m_stats.totalFreed.load(std::memory_order_relaxed);
    stats.currentUsage = m_stats.currentUsage.load(std::memory_order_relaxed);
    stats.peakUsage = m_stats.peakUsage.load(std::memory_order_relaxed);
    stats.allocationCount = m_stats.allocationCount.load(std::memory_order_relaxed);
    stats.deallocationCount = m_stats.deallocationCount.load(std::memory_order_relaxed);
    return stats;
}

} // namespace memory
} // namespace core
} // namespace poko