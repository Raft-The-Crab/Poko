/**
 * @file validate.cpp
 * @brief HandleManager validation implementation
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

[[nodiscard]] bool HandleManager::isValid(Handle handle) const noexcept {
    // ============================================================================
    // Validate Handle
    // ============================================================================
    if (!handle.isValid()) {
        return false;
    }
    
    // Use try_lock to avoid potential exceptions from lock acquisition
    // If lock fails, assume invalid (conservative but safe)
    if (!m_mutex.try_lock()) {
        return false;
    }
    
    // Check if index is within bounds
    if (handle.index >= m_entries.size()) {
        m_mutex.unlock();
        return false;
    }
    
    // Check if active and generation matches
    const HandleEntry& entry = m_entries[handle.index];
    const bool valid = entry.active && entry.generation == handle.generation;
    
    m_mutex.unlock();
    return valid;
}

[[nodiscard]] HandleGeneration HandleManager::getGeneration(HandleIndex index) const noexcept {
    // Use try_lock to avoid potential exceptions from lock acquisition
    // If lock fails, return 0 (safe default)
    if (!m_mutex.try_lock()) {
        return 0;
    }
    
    // Return 0 for out-of-bounds indices
    if (index >= m_entries.size()) {
        m_mutex.unlock();
        return 0;
    }
    
    const HandleGeneration gen = m_entries[index].generation;
    m_mutex.unlock();
    return gen;
}

} // namespace handles
} // namespace core
} // namespace poko
