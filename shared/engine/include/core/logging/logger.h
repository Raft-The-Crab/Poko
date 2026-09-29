/**
 * @file logger.h
 * @brief Core logging system header
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_LOGGING_LOGGER_H
#define POKO_CORE_LOGGING_LOGGER_H

#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include <functional>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <cstdio>

namespace poko {
namespace core {
namespace logging {

/**
 * @brief Log severity levels
 */
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Fatal = 4
};

/**
 * @brief Convert log level to string
 * 
 * @param level Log level
 * @return String representation
 */
[[nodiscard]] const char* logLevelToString(LogLevel level);

/**
 * @brief Convert string to log level
 * 
 * @param str String representation
 * @return Log level
 */
[[nodiscard]] LogLevel stringToLogLevel(const std::string& str);

/**
 * @brief Log message structure
 */
struct LogMessage {
    LogLevel level;
    std::string subsystem;
    std::string message;
    std::string timestamp;
    std::string file;
    int line;
    std::string function;
};

/**
 * @brief Log sink interface
 * 
 * Sinks are destinations for log messages (console, file, etc.)
 */
class LogSink {
public:
    virtual ~LogSink() = default;
    
    /**
     * @brief Write a log message
     * 
     * @param message Log message to write
     */
    virtual void write(const LogMessage& message) = 0;
    
    /**
     * @brief Flush any buffered output
     */
    virtual void flush() = 0;
};

/**
 * @brief Console log sink
 */
class ConsoleSink : public LogSink {
public:
    void write(const LogMessage& message) override;
    void flush() override;
};

/**
 * @brief File log sink
 */
class FileSink : public LogSink {
public:
    explicit FileSink(const std::string& filepath);
    ~FileSink() override;
    
    void write(const LogMessage& message) override;
    void flush() override;
    
private:
    std::string m_filepath;
    FILE* m_file;
    std::mutex m_mutex;
};

/**
 * @brief Logger class
 * 
 * Thread-safe logging system with multiple sinks and severity filtering
 */
class Logger {
public:
    /**
     * @brief Constructor
     */
    Logger();
    
    /**
     * @brief Destructor
     */
    ~Logger();
    
    /**
     * @brief Log a message
     * 
     * @param level Log severity level
     * @param subsystem Subsystem/category
     * @param message Log message
     * @param file Source file
     * @param line Source line
     * @param function Source function
     */
    void log(LogLevel level, const std::string& subsystem, const std::string& message,
             const std::string& file = "", int line = 0, const std::string& function = "");
    
    /**
     * @brief Add a log sink
     * 
     * @param sink Log sink to add
     */
    void addSink(std::shared_ptr<LogSink> sink);
    
    /**
     * @brief Remove a log sink
     * 
     * @param sink Log sink to remove
     */
    void removeSink(LogSink* sink);
    
    /**
     * @brief Set minimum log level
     * 
     * Messages below this level will be ignored
     * 
     * @param level Minimum log level
     */
    void setMinLevel(LogLevel level);
    
    /**
     * @brief Get minimum log level
     * 
     * @return Minimum log level
     */
    [[nodiscard]] LogLevel getMinLevel() const;
    
    /**
     * @brief Flush all sinks
     */
    void flush();
    
    /**
     * @brief Set subsystem filter
     * 
     * Only log messages from the specified subsystem will be processed
     * Empty string means no filtering
     * 
     * @param subsystem Subsystem to filter
     */
    void setSubsystemFilter(const std::string& subsystem);
    
    /**
     * @brief Get subsystem filter
     * 
     * @return Subsystem filter (empty if no filter)
     */
    [[nodiscard]] std::string getSubsystemFilter() const;
    
private:
    mutable std::mutex m_mutex;
    std::vector<std::shared_ptr<LogSink>> m_sinks;
    LogLevel m_minLevel;
    std::string m_subsystemFilter;
    
    /**
     * @brief Format log message
     * 
     * @param message Log message
     * @return Formatted string
     */
    [[nodiscard]] std::string formatMessage(const LogMessage& message) const;
    
    /**
     * @brief Get current timestamp
     * 
     * @return Timestamp string
     */
    [[nodiscard]] std::string getTimestamp() const;
};

/**
 * @brief Global logger instance
 * 
 * Provides access to the globally shared logger
 * 
 * @return Reference to global logger
 * 
 * @note Thread-safe
 * @note Do not destroy this instance - it's globally managed
 */
Logger& getGlobalLogger();

/**
 * @brief Convenience logging macros
 */
#define LOG_DEBUG(subsystem, message) \
    poko::core::logging::getGlobalLogger().log( \
        poko::core::logging::LogLevel::Debug, \
        subsystem, message, __FILE__, __LINE__, __FUNCTION__)

#define LOG_INFO(subsystem, message) \
    poko::core::logging::getGlobalLogger().log( \
        poko::core::logging::LogLevel::Info, \
        subsystem, message, __FILE__, __LINE__, __FUNCTION__)

#define LOG_WARNING(subsystem, message) \
    poko::core::logging::getGlobalLogger().log( \
        poko::core::logging::LogLevel::Warning, \
        subsystem, message, __FILE__, __LINE__, __FUNCTION__)

#define LOG_ERROR(subsystem, message) \
    poko::core::logging::getGlobalLogger().log( \
        poko::core::logging::LogLevel::Error, \
        subsystem, message, __FILE__, __LINE__, __FUNCTION__)

#define LOG_FATAL(subsystem, message) \
    poko::core::logging::getGlobalLogger().log( \
        poko::core::logging::LogLevel::Fatal, \
        subsystem, message, __FILE__, __LINE__, __FUNCTION__)

} // namespace logging
} // namespace core
} // namespace poko

#endif // POKO_CORE_LOGGING_LOGGER_H
