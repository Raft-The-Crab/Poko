/**
 * @file deallocate.cpp
 * @brief System allocator deallocation implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/memory/allocator.h"
#ifdef _WIN32
#include <malloc.h>
#endif

namespace poko {
namespace core {
namespace memory {

void SystemAllocator::deallocate(void* ptr, size_t size) noexcept {
    // ============================================================================
    // Null Pointer Check
    // ============================================================================
    // Null pointer deallocation is safe and idempotent
    if (!ptr) return;
    
    // ============================================================================
    // Update Statistics (if tracking enabled and size provided)
    // ============================================================================
    // Statistics update is done with relaxed memory ordering for performance
    // Lock is only needed for the update, not the read of tracking flag
    if (m_trackingEnabled.load(std::memory_order_relaxed) && size > 0) {
        size_t current = m_stats.currentUsage.load(std::memory_order_relaxed);
        
        // Clamp size to current usage to prevent underflow
        // This handles the case where user provides incorrect size
        // Production robustness: clamp rather than throw or underflow
        if (current < size) {
            size = current;
        }
        
        // Update deallocation counters atomically
        // These operations are lock-free and won't throw
        m_stats.totalFreed.fetch_add(size, std::memory_order_relaxed);
        m_stats.currentUsage.fetch_sub(size, std::memory_order_relaxed);
        m_stats.deallocationCount.fetch_add(1, std::memory_order_relaxed);
    }
    
    // ============================================================================
    // Platform-Specific Deallocation
    // ============================================================================
    // These system calls are guaranteed not to throw
    // Note: If guard bytes were used, the pointer should have been adjusted
    // back to the original allocation before deallocation. Current implementation
    // requires users to handle this manually.
#ifdef _WIN32
    _aligned_free(ptr);
#else
    free(ptr);
#endif
}

} // namespace memory
} // namespace core
} // namespace poko