/**
 * @file logger.cpp
 * @brief Simple logging system implementation for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "core/logging/logger.h"
#include <iostream>
#include <ctime>
#include <iomanip>

namespace Poko {
namespace Logging {

LogLevel Logger::s_log_level = LogLevel::Info;
std::mutex Logger::s_mutex;

void Logger::Log(LogLevel level, const char* file, int line, const std::string& message)
{
    // Filter based on log level
    if (level < s_log_level) {
        return;
    }

    std::lock_guard<std::mutex> lock(s_mutex);

    // Get current time
    std::time_t now = std::time(nullptr);
    std::tm tm = *std::localtime(&now);

    // Format log level
    const char* level_str = "";
    switch (level) {
        case LogLevel::Debug: level_str = "DEBUG"; break;
        case LogLevel::Info: level_str = "INFO"; break;
        case LogLevel::Warning: level_str = "WARNING"; break;
        case LogLevel::Error: level_str = "ERROR"; break;
    }

    // Format timestamp
    std::cout << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << "]";

    // Format log level
    std::cout << "[" << level_str << "]";

    // Format file and line
    std::cout << "[" << file << ":" << line << "]";

    // Output message
    std::cout << " " << message << std::endl;
}

void Logger::SetLogLevel(LogLevel level)
{
    s_log_level = level;
}

} // namespace Logging
} // namespace Poko