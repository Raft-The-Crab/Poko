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
    // Validate Handle
    // ============================================================================
    if (!handle.isValid()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Check if index is within bounds
    if (handle.index >= m_entries.size()) {
        return false;
    }
    
    // Check if generation matches (stale handle detection)
    HandleEntry& entry = m_entries[handle.index];
    if (entry.generation != handle.generation) {
        return false; // Stale handle - generation mismatch
    }
    
    // Check if already freed (double-free protection)
    if (!entry.active) {
        return false; // Already freed
    }
    
    // ============================================================================
    // Mark as Inactive and Add to Free List
    // ============================================================================
    entry.active = false;
    m_freeList.push_back(handle.index);
    
    // Update atomic statistics
    m_activeCount.fetch_sub(1, std::memory_order_relaxed);
    m_totalFreed.fetch_add(1, std::memory_order_relaxed);
    
    return true;
}

} // namespace handles
} // namespace core
} // namespace poko
