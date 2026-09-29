/**
 * @file free.cpp
 * @brief HandleManager free implementation
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

[[nodiscard]] bool HandleManager::free(Handle handle) {
    // ============================================================================
    // Validate Handle Basic Structure
    // ============================================================================
    // Null/invalid handles are rejected immediately
    if (!handle.isValid()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // ============================================================================
    // Validate Index Bounds
    // ============================================================================
    // Index must be within the allocated entry range
    if (handle.index >= m_entries.size()) {
        return false;
    }
    
    // ============================================================================
    // Validate Generation (Stale Handle Detection)
    // ============================================================================
    // Generation mismatch indicates the handle was freed and reused
    // This prevents use-after-free bugs and stale reference attacks
    HandleEntry& entry = m_entries[handle.index];
    if (entry.generation != handle.generation) {
        return false; // Stale handle - generation mismatch
    }
    
    // ============================================================================
    // Double-Free Protection
    // ============================================================================
    // Check if already freed to prevent double-free bugs
    if (!entry.active) {
        return false; // Already freed
    }
    
    // ============================================================================
    // Mark as Inactive and Add to Free List
    // ============================================================================
    // Mark entry as inactive so future allocations can reuse the index
    // Add index to free list for efficient reuse
    entry.active = false;
    m_freeList.push_back(handle.index);
    
    // Update atomic statistics (lock-free, no need for mutex)
    // Relaxed memory ordering is sufficient for statistics
    m_activeCount.fetch_sub(1, std::memory_order_relaxed);
    m_totalFreed.fetch_add(1, std::memory_order_relaxed);
    
    return true;
}

} // namespace handles
} // namespace core
} // namespace poko
