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
    // Validate Guard Bytes (if present)
    // ============================================================================
    // Check for allocation header to determine if guards were used
    uint8_t* rawPtr = static_cast<uint8_t*>(ptr);
    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(rawPtr - sizeof(AllocationHeader) - GUARD_SIZE);
    
    // Check if this allocation has a valid header
    bool hasValidHeader = (header->guard == HEADER_GUARD && header->hasGuard);
    
    if (hasValidHeader) {
        // Validate guard bytes before deallocation
        uint8_t* allocationStart = rawPtr - GUARD_SIZE;
        
        // Validate guard before allocation
        uint8_t* guardBefore = allocationStart - sizeof(AllocationHeader);
        for (size_t i = 0; i < GUARD_SIZE; ++i) {
            if (guardBefore[i] != GUARD_PATTERN) {
                // Guard corruption detected - continue deallocation but could log error
                // In production builds, this might be logged to an error tracking system
            }
        }
        
        // Validate guard after allocation
        uint8_t* guardAfter = allocationStart + header->size;
        for (size_t i = 0; i < GUARD_SIZE; ++i) {
            if (guardAfter[i] != GUARD_PATTERN) {
                // Guard corruption detected - continue deallocation but could log error
            }
        }
        
        // Use stored size from header if not provided
        if (size == 0) {
            size = header->size;
        }
        
        // Adjust pointer to original allocation
        rawPtr = reinterpret_cast<uint8_t*>(header);
    }
    
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
    // Pointer is now adjusted to the original allocation if guards were used
#ifdef _WIN32
    _aligned_free(rawPtr);
#else
    free(rawPtr);
#endif
}

} // namespace memory
} // namespace core
} // namespace poko