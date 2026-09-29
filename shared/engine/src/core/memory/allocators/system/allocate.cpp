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

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * @brief Check if a value is a power of two
 */
static constexpr bool isPowerOfTwo(size_t value) noexcept {
    return value != 0 && (value & (value - 1)) == 0;
}

/**
 * @brief Align a size up to the given alignment
 */
static constexpr size_t alignUp(size_t size, size_t alignment) noexcept {
    return (size + alignment - 1) & ~(alignment - 1);
}

[[nodiscard]] void* SystemAllocator::allocate(size_t size, size_t alignment, AllocationFlags flags) {
    // ============================================================================
    // Early Exit for Zero Size
    // ============================================================================
    // Zero-size allocations are allowed but return nullptr (C++ standard behavior)
    if (size == 0) {
        return nullptr;
    }
    
    // ============================================================================
    // Validate Size Against Maximum
    // ============================================================================
    // Prevent integer overflow in size calculations
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
    // This prevents misaligned pointers which can cause crashes on some architectures
    if (alignment < MIN_ALIGNMENT) {
        alignment = MIN_ALIGNMENT;
    }
    
    // ============================================================================
    // Validate Alignment is Power of 2
    // ============================================================================
    // Most aligned allocation APIs require power-of-2 alignment
    // This validation catches user errors early
    if (alignment == 0 || !isPowerOfTwo(alignment)) {
        if ((flags & AllocationFlags::NoThrow) != AllocationFlags::None) {
            return nullptr;
        }
        throw std::invalid_argument("SystemAllocator: Alignment (" + 
            std::to_string(alignment) + " bytes) must be a power of 2");
    }
    
    // ============================================================================
    // Calculate Total Size with Guard Bytes and Header
    // ============================================================================
    // Guard bytes add padding before and after the allocation for corruption detection
    // Header stores metadata for proper deallocation and validation
    size_t totalSize = size;
    if ((flags & AllocationFlags::Guard) != AllocationFlags::None) {
        totalSize += sizeof(AllocationHeader) + GUARD_SIZE * 2; // Header + guard before/after
    }
    
    // ============================================================================
    // Calculate Aligned Size with Overflow Check
    // ============================================================================
    // Round up to alignment boundary for proper alignment
    // Check for overflow to prevent integer wraparound
    size_t alignedSize = alignUp(totalSize, alignment);
    
    if (alignedSize < totalSize) {
        // Overflow occurred in alignment calculation
        if ((flags & AllocationFlags::NoThrow) != AllocationFlags::None) {
            return nullptr;
        }
        throw std::invalid_argument("SystemAllocator: Alignment calculation overflow for size " + 
            std::to_string(size) + " with alignment " + std::to_string(alignment));
    }
    
    // ============================================================================
    // Platform-Specific Aligned Allocation
    // ============================================================================
    // Use platform-specific aligned allocation functions
    void* ptr = nullptr;
#ifdef _WIN32
    // Windows: Use _aligned_malloc for aligned allocation
    // _aligned_malloc properly handles alignment requirements
    ptr = _aligned_malloc(alignedSize, alignment);
#else
    // POSIX: Use posix_memalign for aligned allocation
    // posix_memalign requires alignment to be >= sizeof(void*)
    // We already enforce MIN_ALIGNMENT which is sizeof(void*)
    if (posix_memalign(&ptr, alignment, alignedSize) != 0) {
        ptr = nullptr;
    }
#endif
    
    // ============================================================================
    // Handle Allocation Failure
    // ============================================================================
    // Check if allocation succeeded (may fail due to OOM or other errors)
    if (!ptr) {
        if ((flags & AllocationFlags::NoThrow) != AllocationFlags::None) {
            return nullptr;
        }
        throw std::bad_alloc();
    }
    
    // ============================================================================
    // Write Guard Patterns and Header (if requested)
    // ============================================================================
    // Guard bytes help detect buffer overflows and underflows
    // Header stores metadata for automatic deallocation and validation
    if ((flags & AllocationFlags::Guard) != AllocationFlags::None) {
        uint8_t* bytes = static_cast<uint8_t*>(ptr);
        
        // Write header with metadata
        AllocationHeader* header = reinterpret_cast<AllocationHeader*>(bytes);
        header->guard = HEADER_GUARD;
        header->size = size;
        header->alignment = alignment;
        header->hasGuard = true;
        
        // Guard before allocation (after header)
        std::memset(bytes + sizeof(AllocationHeader), GUARD_PATTERN, GUARD_SIZE);
        // Guard after allocation
        std::memset(bytes + sizeof(AllocationHeader) + GUARD_SIZE + size, GUARD_PATTERN, GUARD_SIZE);
    }
    
    // ============================================================================
    // Zero Memory (if requested)
    // ============================================================================
    // Zero-initialize the allocation for security and correctness
    if ((flags & AllocationFlags::ZeroMemory) != AllocationFlags::None) {
        uint8_t* bytes = static_cast<uint8_t*>(ptr);
        // Skip header and guard bytes if present (only zero the user-accessible region)
        size_t zeroStart = (flags & AllocationFlags::Guard) != AllocationFlags::None 
            ? sizeof(AllocationHeader) + GUARD_SIZE : 0;
        std::memset(bytes + zeroStart, 0, size);
    }
    
    // ============================================================================
    // Update Statistics (if tracking enabled)
    // ============================================================================
    // Use lock_guard for thread-safe statistics update
    // Statistics are optional and can be disabled for performance
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_trackingEnabled.load(std::memory_order_relaxed)) {
        // Update allocation counters atomically
        m_stats.totalAllocated.fetch_add(alignedSize, std::memory_order_relaxed);
        m_stats.currentUsage.fetch_add(alignedSize, std::memory_order_relaxed);
        m_stats.allocationCount.fetch_add(1, std::memory_order_relaxed);
        
        // Update peak usage with atomic compare-exchange loop
        // This ensures peak tracking is thread-safe and accurate even under contention
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
    // Return Pointer (adjust for header and guard bytes if present)
    // ============================================================================
    // When guard bytes are enabled, return pointer past the header and guard region
    // This enables automatic deallocation and validation using the stored metadata
    if ((flags & AllocationFlags::Guard) != AllocationFlags::None) {
        return static_cast<uint8_t*>(ptr) + sizeof(AllocationHeader) + GUARD_SIZE;
    }
    
    return ptr;
}

} // namespace memory
} // namespace core
} // namespace poko