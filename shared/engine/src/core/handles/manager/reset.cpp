/**
 * @file reset.cpp
 * @brief HandleManager reset implementation
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

void HandleManager::reset() noexcept {
    // Use try_lock to avoid potential exceptions from lock acquisition
    // If lock fails, skip the reset - acceptable for a reset operation
    if (!m_mutex.try_lock()) {
        return;
    }
    
    // Clear all data structures
    m_entries.clear();
    m_freeList.clear();
    
    // Reset atomic counters
    m_activeCount.store(0, std::memory_order_relaxed);
    m_totalAllocated.store(0, std::memory_order_relaxed);
    m_totalFreed.store(0, std::memory_order_relaxed);
    
    m_mutex.unlock();
    
    // Note: This invalidates ALL handles, even active ones
    // Users should ensure no handles are in use before calling reset()
}

} // namespace handles
} // namespace core
} // namespace poko
