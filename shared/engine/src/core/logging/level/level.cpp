/**
 * @file level.cpp
 * @brief Logger level management implementation
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
 * @brief Set minimum log level
 */
void Logger::setMinLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_minLevel = level;
}

/**
 * @brief Get minimum log level
 */
LogLevel Logger::getMinLevel() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_minLevel;
}

} // namespace logging
} // namespace core
} // namespace poko
