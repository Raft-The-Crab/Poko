/**
 * @file logger.h
 * @brief Core logging system for Poko Engine
 * @details Provides structured logging with multiple severity levels, sinks, and thread-safe operations
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace poko {
namespace core {

/**
 * @enum LogLevel
 * @brief Severity levels for log messages
 */
enum class LogLevel {
    TRACE,   ///< Detailed trace information for debugging
    DEBUG,   ///< Debug-level information
    INFO,    ///< General informational messages
    WARNING, ///< Warning messages for potentially harmful situations
    ERROR,   ///< Error messages for error events
    FATAL   ///< Fatal error messages that may lead to application termination
};

/**
 * @class LogSink
 * @brief Abstract base class for log output destinations
 * @details Concrete implementations can write to console, files, network, etc.
 */
class LogSink {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~LogSink() = default;

    /**
     * @brief Write a log message to the sink
     * @param level The severity level of the message
     * @param message The formatted log message
     * @param context Optional context information (e.g., subsystem name)
     */
    virtual void log(LogLevel level, const std::string& message, const std::string& context) = 0;
};

/**
 * @class ConsoleSink
 * @brief Log sink that outputs to the console with color coding
 * @details Uses ANSI color codes for different log levels
 */
class ConsoleSink : public LogSink {
public:
    /**
     * @brief Write a log message to the console
     * @param level The severity level of the message
     * @param message The formatted log message
     * @param context Optional context information
     */
    void log(LogLevel level, const std::string& message, const std::string& context) override;
};

/**
 * @class FileSink
 * @brief Log sink that writes to a file
 * @details Thread-safe file writing with automatic flushing
 */
class FileSink : public LogSink {
public:
    /**
     * @brief Construct a file sink that writes to the specified file
     * @param filename Path to the log file (appends if exists, creates if not)
     */
    explicit FileSink(const std::string& filename);

    /**
     * @brief Destructor that closes the file
     */
    ~FileSink() override;

    /**
     * @brief Write a log message to the file
     * @param level The severity level of the message
     * @param message The formatted log message
     * @param context Optional context information
     */
    void log(LogLevel level, const std::string& message, const std::string& context) override;

private:
    std::ofstream file_;   ///< Output file stream
    std::mutex mutex_;     ///< Mutex for thread-safe file operations
};

/**
 * @class Logger
 * @brief Main logging system with singleton pattern
 * @details Thread-safe logger that supports multiple sinks and configurable log levels
 */
class Logger {
public:
    /**
     * @brief Get the singleton instance of the logger
     * @return Reference to the logger instance
     */
    static Logger& instance();

    /**
     * @brief Add a log sink to receive log messages
     * @param sink Shared pointer to the log sink
     */
    void add_sink(std::shared_ptr<LogSink> sink);

    /**
     * @brief Remove a log sink
     * @param sink Pointer to the sink to remove
     */
    void remove_sink(LogSink* sink);

    /**
     * @brief Set the minimum log level to output
     * @param level The minimum severity level (messages below this level are ignored)
     */
    void set_level(LogLevel level);

    /**
     * @brief Get the current minimum log level
     * @return The current minimum log level
     */
    LogLevel get_level();

    /**
     * @brief Log a message at the specified level
     * @param level The severity level
     * @param message The log message
     * @param context Optional context information
     */
    void log(LogLevel level, const std::string& message, const std::string& context = "");

    /**
     * @brief Set the default context for log messages
     * @param context The context string (e.g., subsystem name)
     */
    void set_context(const std::string& context);

    /**
     * @brief Log a trace message
     * @param message The log message
     */
    void trace(const std::string& message);

    /**
     * @brief Log a debug message
     * @param message The log message
     */
    void debug(const std::string& message);

    /**
     * @brief Log an info message
     * @param message The log message
     */
    void info(const std::string& message);

    /**
     * @brief Log a warning message
     * @param message The log message
     */
    void warning(const std::string& message);

    /**
     * @brief Log an error message
     * @param message The log message
     */
    void error(const std::string& message);

    /**
     * @brief Log a fatal message
     * @param message The log message
     */
    void fatal(const std::string& message);

private:
    /**
     * @brief Private constructor for singleton pattern
     */
    Logger();

    /**
     * @brief Private destructor
     */
    ~Logger() = default;

    /**
     * @brief Delete copy constructor
     */
    Logger(const Logger&) = delete;

    /**
     * @brief Delete assignment operator
     */
    Logger& operator=(const Logger&) = delete;

    /**
     * @brief Format a log message with timestamp
     * @param level The severity level
     * @param message The raw message
     * @return Formatted message string
     */
    std::string format_message(LogLevel level, const std::string& message);

    /**
     * @brief Get current timestamp as string
     * @return Timestamp in format "YYYY-MM-DD HH:MM:SS.mmm"
     */
    std::string timestamp();

    std::vector<std::shared_ptr<LogSink>> sinks_; ///< Registered log sinks
    LogLevel min_level_;                          ///< Minimum log level to output
    std::string context_;                         ///< Default context string
    std::mutex mutex_;                            ///< Mutex for thread-safe operations
};

} // namespace core
} // namespace poko

// Convenience macros for logging
#define POKO_LOG_TRACE(...) ::poko::core::Logger::instance().trace(__VA_ARGS__)
#define POKO_LOG_DEBUG(...) ::poko::core::Logger::instance().debug(__VA_ARGS__)
#define POKO_LOG_INFO(...)  ::poko::core::Logger::instance().info(__VA_ARGS__)
#define POKO_LOG_WARN(...)  ::poko::core::Logger::instance().warning(__VA_ARGS__)
#define POKO_LOG_ERROR(...) ::poko::core::Logger::instance().error(__VA_ARGS__)
#define POKO_LOG_FATAL(...) ::poko::core::Logger::instance().fatal(__VA_ARGS__)

// Context setting macro
#define POKO_LOG_CONTEXT(ctx) ::poko::core::Logger::instance().set_context(ctx)
