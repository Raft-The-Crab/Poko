/**
 * @file tracking.cpp
 * @brief System allocator tracking implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/memory/allocator.h"

namespace poko {
namespace core {
namespace memory {

void SystemAllocator::setTrackingEnabled(bool enabled) noexcept {
    // Use atomic store for lock-free tracking flag update
    // This allows fast-path checks without acquiring mutex
    m_trackingEnabled.store(enabled, std::memory_order_relaxed);
}

bool SystemAllocator::isTrackingEnabled() const noexcept {
    // Fast path: read atomic flag without lock for performance
    // Relaxed memory ordering is sufficient for this boolean flag
    return m_trackingEnabled.load(std::memory_order_relaxed);
}

} // namespace memory
} // namespace core
} // namespace poko