/**
 * @file allocate.cpp
 * @brief System allocator allocation implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 * 
 * Implements memory allocation with alignment support, guard bytes,
 * zero initialization, and statistics tracking.
 */

#include "core/memory/allocator.h"
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <mutex>
#include <string>

namespace poko {
namespace core {
namespace memory {

[[nodiscard]] void* SystemAllocator::allocate(size_t size, size_t alignment, AllocationFlags flags) {
    // ============================================================================
    // Early Exit for Zero Size
    // ============================================================================
    if (size == 0) {
        return nullptr;
    }
    
    // ============================================================================
    // Validate Size
    // ============================================================================
    if (size > MAX_ALLOCATION_SIZE) {
        if ((flags & AllocationFlags::NoThrow) != AllocationFlags::None) {
            return nullptr;
        }
        throw std::invalid_argument("SystemAllocator: Requested allocation size (" + 
            std::to_string(size) + " bytes) exceeds maximum allowed (" + 
            std::to_string(MAX_ALLOCATION_SIZE) + " bytes)");
    }
    
    // ============================================================================
    // Enforce Minimum Alignment
    // ============================================================================
    // Ensure alignment is at least MIN_ALIGNMENT for pointer alignment
    if (alignment < MIN_ALIGNMENT) {
        alignment = MIN_ALIGNMENT;
    }
    
    // ============================================================================
    // Validate Alignment is Power of 2
    // ============================================================================
    // Power of 2 check: (x & (x - 1)) == 0 for power of 2
    if (alignment == 0 || (alignment & (alignment - 1)) != 0) {
        if ((flags & AllocationFlags::NoThrow) != AllocationFlags::None) {
            return nullptr;
        }
        throw std::invalid_argument("SystemAllocator: Alignment (" + 
            std::to_string(alignment) + " bytes) must be a power of 2");
    }
    
    // ============================================================================
    // Calculate Total Size with Guard Bytes
    // ============================================================================
    size_t totalSize = size;
    if ((flags & AllocationFlags::Guard) != AllocationFlags::None) {
        totalSize += GUARD_SIZE * 2; // Guard before and after allocation
    }
    
    // ============================================================================
    // Calculate Aligned Size with Overflow Check
    // ============================================================================
    // Round up to alignment boundary: (size + alignment - 1) & ~(alignment - 1)
    size_t alignedSize = (totalSize + alignment - 1) & ~(alignment - 1);
    
    // Check for overflow (alignedSize should be >= totalSize)
    if (alignedSize < totalSize) {
        if ((flags & AllocationFlags::NoThrow) != AllocationFlags::None) {
            return nullptr;
        }
        throw std::invalid_argument("SystemAllocator: Alignment calculation overflow for size " + 
            std::to_string(size) + " with alignment " + std::to_string(alignment));
    }
    
    // ============================================================================
    // Platform-Specific Aligned Allocation
    // ============================================================================
    void* ptr = nullptr;
#ifdef _WIN32
    // Windows: Use _aligned_malloc for aligned allocation
    ptr = _aligned_malloc(alignedSize, alignment);
#else
    // POSIX: Use posix_memalign for aligned allocation
    // Note: posix_memalign requires alignment to be >= sizeof(void*)
    if (posix_memalign(&ptr, alignment, alignedSize) != 0) {
        ptr = nullptr;
    }
#endif
    
    // ============================================================================
    // Handle Allocation Failure
    // ============================================================================
    if (!ptr) {
        if ((flags & AllocationFlags::NoThrow) != AllocationFlags::None) {
            return nullptr;
        }
        throw std::bad_alloc();
    }
    
    // ============================================================================
    // Write Guard Patterns (if requested)
    // ============================================================================
    if ((flags & AllocationFlags::Guard) != AllocationFlags::None) {
        uint8_t* bytes = static_cast<uint8_t*>(ptr);
        // Guard before allocation
        std::memset(bytes, GUARD_PATTERN, GUARD_SIZE);
        // Guard after allocation
        std::memset(bytes + GUARD_SIZE + size, GUARD_PATTERN, GUARD_SIZE);
    }
    
    // ============================================================================
    // Zero Memory (if requested)
    // ============================================================================
    if ((flags & AllocationFlags::ZeroMemory) != AllocationFlags::None) {
        uint8_t* bytes = static_cast<uint8_t*>(ptr);
        // Skip guard bytes if present
        size_t zeroStart = (flags & AllocationFlags::Guard) != AllocationFlags::None ? GUARD_SIZE : 0;
        std::memset(bytes + zeroStart, 0, size);
    }
    
    // ============================================================================
    // Update Statistics (if tracking enabled)
    // ============================================================================
    // Use lock_guard for thread-safe statistics update
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_trackingEnabled.load(std::memory_order_relaxed)) {
        // Update allocation counters
        m_stats.totalAllocated.fetch_add(alignedSize, std::memory_order_relaxed);
        m_stats.currentUsage.fetch_add(alignedSize, std::memory_order_relaxed);
        m_stats.allocationCount.fetch_add(1, std::memory_order_relaxed);
        
        // Update peak usage with atomic compare-exchange loop
        // This ensures peak tracking is thread-safe and accurate
        size_t current = m_stats.currentUsage.load(std::memory_order_relaxed);
        size_t peak = m_stats.peakUsage.load(std::memory_order_relaxed);
        while (current > peak) {
            // Try to update peak to current
            // If another thread updated it in between, retry with new peak value
            if (m_stats.peakUsage.compare_exchange_weak(peak, current, 
                std::memory_order_relaxed, std::memory_order_relaxed)) {
                break; // Successfully updated peak
            }
            // Retry with updated peak value
        }
    }
    
    // ============================================================================
    // Return Pointer (adjust for guard bytes if present)
    // ============================================================================
    // Note: For production use with guards, we would need to store allocation metadata
    // (including guard flag) in a header to properly deallocate. Current implementation
    // requires users to remember if guards were enabled for proper deallocation.
    // 
    // Future improvement: Add allocation header with:
    // - Guard flag
    // - Original size
    // - Original alignment
    // This would enable automatic guard validation on deallocation.
    if ((flags & AllocationFlags::Guard) != AllocationFlags::None) {
        return static_cast<uint8_t*>(ptr) + GUARD_SIZE;
    }
    
    return ptr;
}

} // namespace memory
} // namespace core
} // namespace poko