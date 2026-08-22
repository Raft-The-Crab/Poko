/**
 * @file logger.h
 * @brief Simple logging system for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#pragma once

#include <string>
#include <sstream>
#include <mutex>

namespace Poko {
namespace Logging {

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error
};

class Logger {
public:
    static void Log(LogLevel level, const char* file, int line, const std::string& message);
    static void SetLogLevel(LogLevel level);

private:
    static LogLevel s_log_level;
    static std::mutex s_mutex;
};

#define LOG_DEBUG(msg) Poko::Logging::Logger::Log(Poko::Logging::LogLevel::Debug, __FILE__, __LINE__, msg)
#define LOG_INFO(msg) Poko::Logging::Logger::Log(Poko::Logging::LogLevel::Info, __FILE__, __LINE__, msg)
#define LOG_WARNING(msg) Poko::Logging::Logger::Log(Poko::Logging::LogLevel::Warning, __FILE__, __LINE__, msg)
#define LOG_ERROR(msg) Poko::Logging::Logger::Log(Poko::Logging::LogLevel::Error, __FILE__, __LINE__, msg)

} // namespace Logging
} // namespace Poko