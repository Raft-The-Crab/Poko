/**
 * @file validate.cpp
 * @brief System allocator guard validation implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/memory/allocator.h"
#include <cstring>

namespace poko {
namespace core {
namespace memory {

[[nodiscard]] bool SystemAllocator::validateGuardBytes(void* ptr, size_t size, size_t alignment) const noexcept {
    // Alignment parameter is not used in current implementation
    // but kept for future enhancements
    (void)alignment;
    
    // Null pointer check
    if (!ptr) {
        return false;
    }
    
    // Calculate pointer to allocation header
    uint8_t* rawPtr = static_cast<uint8_t*>(ptr);
    AllocationHeader* header = reinterpret_cast<AllocationHeader*>(rawPtr - sizeof(AllocationHeader) - GUARD_SIZE);
    
    // Check if this allocation has a valid header
    if (header->guard != HEADER_GUARD || !header->hasGuard) {
        return false; // No valid header or guards not enabled
    }
    
    // Use stored size from header if not provided
    if (size == 0) {
        size = header->size;
    }
    
    // Calculate pointer to guard before allocation (after header)
    uint8_t* allocationStart = rawPtr - GUARD_SIZE;
    
    // ============================================================================
    // Validate Guard Before Allocation
    // ============================================================================
    uint8_t* guardBefore = reinterpret_cast<uint8_t*>(header) + sizeof(AllocationHeader);
    for (size_t i = 0; i < GUARD_SIZE; ++i) {
        if (guardBefore[i] != GUARD_PATTERN) {
            return false; // Guard before allocation corrupted
        }
    }
    
    // ============================================================================
    // Validate Guard After Allocation
    // ============================================================================
    uint8_t* guardAfter = allocationStart + size;
    for (size_t i = 0; i < GUARD_SIZE; ++i) {
        if (guardAfter[i] != GUARD_PATTERN) {
            return false; // Guard after allocation corrupted
        }
    }
    
    return true; // All guard bytes intact
}

} // namespace memory
} // namespace core
} // namespace poko
