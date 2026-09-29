/**
 * @file flush.cpp
 * @brief Logger flush operation implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/logging/logger.h"

namespace poko {
namespace core {
namespace logging {

/**
 * @brief Flush all sinks
 */
void Logger::flush() {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& sink : m_sinks) {
        sink->flush();
    }
}

} // namespace logging
} // namespace core
} // namespace poko
