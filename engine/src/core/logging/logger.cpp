/**
 * @file logger.cpp
 * @brief Implementation of the core logging system
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "core/logging/logger.h"
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace poko {
namespace core {

// ConsoleSink implementation
void ConsoleSink::log(LogLevel level, const std::string& message, const std::string& context) {
    const char* level_str = "";
    const char* color_code = "";
    const char* reset_code = "\033[0m";

    // Set level string and color code based on severity
    switch (level) {
        case LogLevel::TRACE:
            level_str = "TRACE";
            color_code = "\033[90m"; // Gray
            break;
        case LogLevel::DEBUG:
            level_str = "DEBUG";
            color_code = "\033[36m"; // Cyan
            break;
        case LogLevel::INFO:
            level_str = "INFO";
            color_code = "\033[32m"; // Green
            break;
        case LogLevel::WARNING:
            level_str = "WARN";
            color_code = "\033[33m"; // Yellow
            break;
        case LogLevel::ERROR:
            level_str = "ERROR";
            color_code = "\033[31m"; // Red
            break;
        case LogLevel::FATAL:
            level_str = "FATAL";
            color_code = "\033[35m"; // Magenta
            break;
    }

    // Build the log line with context if provided
    std::string log_line = message;
    if (!context.empty()) {
        log_line = "[" + context + "] " + log_line;
    }

    // Output to console with color coding
    std::cout << color_code << "[" << level_str << "] " << reset_code << log_line << std::endl;
}

// FileSink implementation
FileSink::FileSink(const std::string& filename) {
    file_.open(filename, std::ios::app);
    if (!file_.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

FileSink::~FileSink() {
    if (file_.is_open()) {
        file_.close();
    }
}

void FileSink::log(LogLevel level, const std::string& message, const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!file_.is_open()) {
        return;
    }

    // Set level string based on severity
    const char* level_str = "";
    switch (level) {
        case LogLevel::TRACE: level_str = "TRACE"; break;
        case LogLevel::DEBUG: level_str = "DEBUG"; break;
        case LogLevel::INFO:  level_str = "INFO";  break;
        case LogLevel::WARNING: level_str = "WARN";  break;
        case LogLevel::ERROR: level_str = "ERROR"; break;
        case LogLevel::FATAL: level_str = "FATAL"; break;
    }

    // Build the log line with context if provided
    std::string log_line = message;
    if (!context.empty()) {
        log_line = "[" + context + "] " + log_line;
    }

    // Write to file and flush immediately
    file_ << "[" << level_str << "] " << log_line << std::endl;
    file_.flush();
}

// Logger implementation
Logger::Logger() {
    // Add console sink by default
    add_sink(std::make_shared<ConsoleSink>());
}

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

void Logger::add_sink(std::shared_ptr<LogSink> sink) {
    std::lock_guard<std::mutex> lock(mutex_);
    sinks_.push_back(sink);
}

void Logger::remove_sink(LogSink* sink) {
    std::lock_guard<std::mutex> lock(mutex_);
    sinks_.erase(
        std::remove_if(sinks_.begin(), sinks_.end(),
            [sink](const std::shared_ptr<LogSink>& s) { return s.get() == sink; }),
        sinks_.end()
    );
}

void Logger::set_level(LogLevel level) {
    std::lock_guard<std::mutex> lock(mutex_);
    min_level_ = level;
}

LogLevel Logger::get_level() {
    std::lock_guard<std::mutex> lock(mutex_);
    return min_level_;
}

void Logger::log(LogLevel level, const std::string& message, const std::string& context) {
    // Filter messages below minimum level
    if (level < min_level_) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    std::string formatted_message = format_message(level, message);

    // Send to all registered sinks
    for (auto& sink : sinks_) {
        sink->log(level, formatted_message, context);
    }
}

void Logger::set_context(const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    context_ = context;
}

std::string Logger::format_message(LogLevel level, const std::string& message) {
    (void)level; // Suppress unused parameter warning
    return timestamp() + " " + message;
}

std::string Logger::timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return ss.str();
}

// Convenience method implementations
void Logger::trace(const std::string& message) {
    log(LogLevel::TRACE, message, context_);
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message, context_);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message, context_);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message, context_);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message, context_);
}

void Logger::fatal(const std::string& message) {
    log(LogLevel::FATAL, message, context_);
}

} // namespace core
} // namespace poko
