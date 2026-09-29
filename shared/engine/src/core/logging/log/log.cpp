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
 * Thread-safe operation that filters and writes log messages to all registered sinks.
 * Messages are filtered by minimum log level and optional subsystem filter before
 * being written to all sinks.
 * 
 * @param level Log severity level (Debug, Info, Warning, Error, Fatal)
 * @param subsystem Subsystem/category name (e.g., "Renderer", "Physics")
 * @param message Log message content
 * @param file Source file name (optional, for debugging)
 * @param line Source line number (optional, for debugging)
 * @param function Source function name (optional, for debugging)
 * 
 * @note Messages below minimum level are silently dropped
 * @note Empty subsystem filter means no subsystem filtering
 * @note All sinks receive the filtered message
 * @note Thread-safe via mutex protection
 */
void Logger::log(LogLevel level, const std::string& subsystem, const std::string& message,
                const std::string& file, int line, const std::string& function) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // ============================================================================
    // Filter by Minimum Log Level
    // ============================================================================
    // Messages below the minimum level are dropped early for performance
    // This reduces unnecessary work for low-priority debug messages
    if (level < m_minLevel) {
        return;
    }
    
    // ============================================================================
    // Filter by Subsystem
    // ============================================================================
    // If a subsystem filter is set, only messages from that subsystem are logged
    // Empty filter means no subsystem filtering (all subsystems allowed)
    if (!m_subsystemFilter.empty() && subsystem != m_subsystemFilter) {
        return;
    }
    
    // ============================================================================
    // Create Log Message Structure
    // ============================================================================
    // Populate the log message with all available information
    // This structure is passed to all sinks for consistent formatting
    LogMessage logMsg;
    logMsg.level = level;
    logMsg.subsystem = subsystem;
    logMsg.message = message;
    logMsg.timestamp = getTimestamp();
    logMsg.file = file;
    logMsg.line = line;
    logMsg.function = function;
    
    // ============================================================================
    // Write to All Registered Sinks
    // ============================================================================
    // Each sink receives the log message and formats it according to its type
    // (console, file, network, etc.)
    for (auto& sink : m_sinks) {
        if (sink) {
            sink->write(logMsg);
        }
    }
}

} // namespace logging
} // namespace core
} // namespace poko
