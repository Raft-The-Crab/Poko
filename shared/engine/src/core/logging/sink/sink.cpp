/**
 * @file sink.cpp
 * @brief Logger sink management implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/logging/logger.h"
#include <algorithm>

namespace poko {
namespace core {
namespace logging {

/**
 * @brief Add a log sink
 */
void Logger::addSink(std::shared_ptr<LogSink> sink) {
    // Validate sink
    if (!sink) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sinks.push_back(sink);
}

/**
 * @brief Remove a log sink
 */
void Logger::removeSink(LogSink* sink) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sinks.erase(
        std::remove_if(m_sinks.begin(), m_sinks.end(),
            [sink](const std::shared_ptr<LogSink>& s) { return s.get() == sink; }),
        m_sinks.end()
    );
}

} // namespace logging
} // namespace core
} // namespace poko
