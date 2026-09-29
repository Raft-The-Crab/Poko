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

[[nodiscard]] bool HandleManager::isValid(Handle handle) const {
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
    
    // Check if active and generation matches
    const HandleEntry& entry = m_entries[handle.index];
    return entry.active && entry.generation == handle.generation;
}

[[nodiscard]] HandleGeneration HandleManager::getGeneration(HandleIndex index) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Return 0 for out-of-bounds indices
    if (index >= m_entries.size()) {
        return 0;
    }
    
    return m_entries[index].generation;
}

} // namespace handles
} // namespace core
} // namespace poko
