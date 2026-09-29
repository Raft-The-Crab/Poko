/**
 * @file format.cpp
 * @brief Logger formatting and timestamp implementation
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
 * @brief Format log message
 */
std::string Logger::formatMessage(const LogMessage& message) const {
    std::ostringstream oss;
    
    oss << "[" << message.timestamp << "] "
        << "[" << logLevelToString(message.level) << "] "
        << "[" << message.subsystem << "] ";
    
    if (!message.file.empty()) {
        oss << "[" << message.file << ":" << message.line << "] ";
    }
    
    oss << message.message;
    
    return oss.str();
}

/**
 * @brief Get current timestamp
 */
std::string Logger::getTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    oss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return oss.str();
}

} // namespace logging
} // namespace core
} // namespace poko
