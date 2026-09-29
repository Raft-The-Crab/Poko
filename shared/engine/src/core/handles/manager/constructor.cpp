/**
 * @file constructor.cpp
 * @brief HandleManager constructor implementation
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

HandleManager::HandleManager()
    : m_activeCount(0)
    , m_totalAllocated(0)
    , m_totalFreed(0)
{
    // Pre-allocate capacity to minimize reallocations
    m_entries.reserve(MAX_HANDLES);
}

HandleManager::~HandleManager() {
    // Cleanup is automatic - vector destructor handles it
    // Note: Does NOT free any outstanding handles - users must free them
}

} // namespace handles
} // namespace core
} // namespace poko
