/**
 * @file active_count.cpp
 * @brief HandleManager active count implementation
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

[[nodiscard]] size_t HandleManager::getActiveCount() const noexcept {
    // Atomic read is lock-free and noexcept
    return m_activeCount.load(std::memory_order_relaxed);
}

} // namespace handles
} // namespace core
} // namespace poko
