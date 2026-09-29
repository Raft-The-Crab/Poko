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

void HandleManager::reset() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Clear all data structures
    m_entries.clear();
    m_freeList.clear();
    
    // Reset atomic counters
    m_activeCount.store(0, std::memory_order_relaxed);
    m_totalAllocated.store(0, std::memory_order_relaxed);
    m_totalFreed.store(0, std::memory_order_relaxed);
    
    // Note: This invalidates ALL handles, even active ones
    // Users should ensure no handles are in use before calling reset()
}

size_t HandleManager::getCapacity() const {
    return MAX_HANDLES;
}

size_t HandleManager::getActiveCount() const {
    return m_activeCount.load(std::memory_order_relaxed);
}

} // namespace handles
} // namespace core
} // namespace poko
