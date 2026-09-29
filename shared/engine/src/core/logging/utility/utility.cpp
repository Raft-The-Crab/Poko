/**
 * @file utility.cpp
 * @brief Logging utility functions implementation
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
 * @brief Convert log level to string
 */
const char* logLevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Fatal: return "FATAL";
        default: return "UNKNOWN";
    }
}

/**
 * @brief Convert string to log level
 */
LogLevel stringToLogLevel(const std::string& str) {
    if (str == "DEBUG") return LogLevel::Debug;
    if (str == "INFO") return LogLevel::Info;
    if (str == "WARNING") return LogLevel::Warning;
    if (str == "ERROR") return LogLevel::Error;
    if (str == "FATAL") return LogLevel::Fatal;
    return LogLevel::Info; // Default
}

} // namespace logging
} // namespace core
} // namespace poko
