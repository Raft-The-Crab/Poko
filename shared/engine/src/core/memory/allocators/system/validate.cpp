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
    
    // Calculate pointer to actual allocation (account for guard bytes)
    // The allocate function returns pointer past the guard region
    uint8_t* allocationStart = static_cast<uint8_t*>(ptr) - GUARD_SIZE;
    
    // ============================================================================
    // Validate Guard Before Allocation
    // ============================================================================
    uint8_t* guardBefore = allocationStart;
    for (size_t i = 0; i < GUARD_SIZE; ++i) {
        if (guardBefore[i] != GUARD_PATTERN) {
            return false; // Guard before allocation corrupted
        }
    }
    
    // ============================================================================
    // Validate Guard After Allocation
    // ============================================================================
    uint8_t* guardAfter = allocationStart + GUARD_SIZE + size;
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
