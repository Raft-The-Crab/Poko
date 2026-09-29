/**
 * @file log.cpp
 * @brief Logger log operation implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/logging/logger.h"
#include <iostream>

namespace poko {
namespace core {
namespace logging {

/**
 * @brief Log a message
 * 
 * Thread-safe operation that filters and writes log messages to all sinks.
 */
void Logger::log(LogLevel level, const std::string& subsystem, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Filter by level
    if (level < m_minLevel) {
        return;
    }
    
    // Filter by subsystem
    if (!m_subsystemFilter.empty() && subsystem != m_subsystemFilter) {
        return;
    }
    
    // Create log message
    LogMessage logMsg;
    logMsg.level = level;
    logMsg.subsystem = subsystem;
    logMsg.message = message;
    logMsg.timestamp = getTimestamp();
    logMsg.file = file;
    logMsg.line = line;
    logMsg.function = function;
    
    // Write to all sinks
    for (auto& sink : m_sinks) {
        sink->write(logMsg);
    }
}

} // namespace logging
} // namespace core
} // namespace poko
