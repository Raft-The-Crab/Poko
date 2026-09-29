/**
 * @file allocate.cpp
 * @brief HandleManager allocate implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/handles/handle_manager.h"
#include <stdexcept>

namespace poko {
namespace core {
namespace handles {

[[nodiscard]] Handle HandleManager::allocate() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    HandleIndex index;
    HandleGeneration generation;
    
    // ============================================================================
    // Try to Reuse a Freed Index
    // ============================================================================
    // Reusing indices improves memory efficiency and cache locality
    // Free list uses LIFO (last-in-first-out) for better cache behavior
    if (!m_freeList.empty()) {
        // Pop from free list (LIFO for cache efficiency)
        index = m_freeList.back();
        m_freeList.pop_back();
        
        // Get entry and increment generation to invalidate old handles
        // Generation increment prevents stale reference attacks
        HandleEntry& entry = m_entries[index];
        generation = ++entry.generation;
        entry.active = true;
    } else {
        // ============================================================================
        // Allocate New Index
        // ============================================================================
        // Check capacity limit to prevent unbounded memory growth
        if (m_entries.size() >= MAX_HANDLES) {
            throw std::runtime_error("HandleManager: Maximum handle capacity reached (" + 
                std::to_string(MAX_HANDLES) + " handles)");
        }
        
        index = static_cast<HandleIndex>(m_entries.size());
        generation = 1; // Start at generation 1 (0 is unused)
        
        // Create new entry with generation 1 and mark as active
        HandleEntry entry;
        entry.generation = generation;
        entry.active = true;
        m_entries.push_back(entry);
    }
    
    // Update atomic statistics (lock-free, no need for mutex)
    // Relaxed memory ordering is sufficient for statistics
    m_activeCount.fetch_add(1, std::memory_order_relaxed);
    m_totalAllocated.fetch_add(1, std::memory_order_relaxed);
    
    return Handle(index, generation);
}

} // namespace handles
} // namespace core
} // namespace poko
