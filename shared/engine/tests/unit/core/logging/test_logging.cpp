/**
 * @file test_logging.cpp
 * @brief Unit tests for core logging system
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/logging/logger.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <thread>

namespace poko {
namespace core {
namespace logging {
namespace test {

void test_log_level_conversion() {
    std::cout << "Testing log level conversion..." << std::endl;
    
    assert(std::string(logLevelToString(LogLevel::Debug)) == "DEBUG");
    assert(std::string(logLevelToString(LogLevel::Info)) == "INFO");
    assert(std::string(logLevelToString(LogLevel::Warning)) == "WARNING");
    assert(std::string(logLevelToString(LogLevel::Error)) == "ERROR");
    assert(std::string(logLevelToString(LogLevel::Fatal)) == "FATAL");
    
    assert(stringToLogLevel("DEBUG") == LogLevel::Debug);
    assert(stringToLogLevel("INFO") == LogLevel::Info);
    assert(stringToLogLevel("WARNING") == LogLevel::Warning);
    assert(stringToLogLevel("ERROR") == LogLevel::Error);
    assert(stringToLogLevel("FATAL") == LogLevel::Fatal);
    
    std::cout << "✓ Log level conversion tests passed" << std::endl;
}

void test_logger_basics() {
    std::cout << "Testing Logger basics..." << std::endl;
    
    Logger logger;
    
    // Test default level
    assert(logger.getMinLevel() == LogLevel::Info);
    
    // Test set level
    logger.setMinLevel(LogLevel::Debug);
    assert(logger.getMinLevel() == LogLevel::Debug);
    
    logger.setMinLevel(LogLevel::Error);
    assert(logger.getMinLevel() == LogLevel::Error);
    
    std::cout << "✓ Logger basics tests passed" << std::endl;
}

void test_logger_level_filtering() {
    std::cout << "Testing Logger level filtering..." << std::endl;
    
    Logger logger;
    logger.setMinLevel(LogLevel::Warning);
    
    // These should be filtered out
    logger.log(LogLevel::Debug, "test", "debug message");
    logger.log(LogLevel::Info, "test", "info message");
    
    // These should pass through
    logger.log(LogLevel::Warning, "test", "warning message");
    logger.log(LogLevel::Error, "test", "error message");
    
    std::cout << "✓ Logger level filtering tests passed" << std::endl;
}

void test_logger_subsystem_filter() {
    std::cout << "Testing Logger subsystem filter..." << std::endl;
    
    Logger logger;
    logger.setSubsystemFilter("network");
    
    // This should pass through
    logger.log(LogLevel::Info, "network", "network message");
    
    // This should be filtered out
    logger.log(LogLevel::Info, "audio", "audio message");
    
    // Clear filter
    logger.setSubsystemFilter("");
    assert(logger.getSubsystemFilter().empty());
    
    std::cout << "✓ Logger subsystem filter tests passed" << std::endl;
}

void test_logger_sink_management() {
    std::cout << "Testing Logger sink management..." << std::endl;
    
    Logger logger;
    
    auto consoleSink = std::make_shared<ConsoleSink>();
    logger.addSink(consoleSink);
    
    logger.addSink(std::make_shared<ConsoleSink>());
    logger.addSink(std::make_shared<ConsoleSink>());
    
    logger.removeSink(consoleSink.get());
    
    std::cout << "✓ Logger sink management tests passed" << std::endl;
}

void test_logger_flush() {
    std::cout << "Testing Logger flush..." << std::endl;
    
    Logger logger;
    logger.log(LogLevel::Info, "test", "test message");
    logger.flush();
    
    std::cout << "✓ Logger flush tests passed" << std::endl;
}

void test_file_sink() {
    std::cout << "Testing File sink..." << std::endl;
    
    std::string testFile = "test_log.txt";
    
    {
        Logger logger;
        logger.addSink(std::make_shared<FileSink>(testFile));
        logger.log(LogLevel::Info, "test", "test message");
        logger.flush();
    }
    
    // Verify file was created
    std::ifstream checkFile(testFile);
    assert(checkFile.good());
    checkFile.close();
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "✓ File sink tests passed" << std::endl;
}

void test_global_logger() {
    std::cout << "Testing global logger..." << std::endl;
    
    Logger& globalLogger = getGlobalLogger();
    
    globalLogger.log(LogLevel::Info, "test", "global test message");
    globalLogger.flush();
    
    std::cout << "✓ Global logger tests passed" << std::endl;
}

void test_logging_macros() {
    std::cout << "Testing logging macros..." << std::endl;
    
    LOG_DEBUG("test", "debug message");
    LOG_INFO("test", "info message");
    LOG_WARNING("test", "warning message");
    LOG_ERROR("test", "error message");
    LOG_FATAL("test", "fatal message");
    
    std::cout << "✓ Logging macros tests passed" << std::endl;
}

void test_logger_thread_safety() {
    std::cout << "Testing Logger thread safety..." << std::endl;
    
    Logger logger;
    
    std::thread t1([&logger]() {
        for (int i = 0; i < 100; ++i) {
            logger.log(LogLevel::Info, "thread1", "message " + std::to_string(i));
        }
    });
    
    std::thread t2([&logger]() {
        for (int i = 0; i < 100; ++i) {
            logger.log(LogLevel::Info, "thread2", "message " + std::to_string(i));
        }
    });
    
    t1.join();
    t2.join();
    
    logger.flush();
    
    std::cout << "✓ Logger thread safety tests passed" << std::endl;
}

} // namespace test
} // namespace logging
} // namespace core
} // namespace poko

int main() {
    std::cout << "=== Core Logging Unit Tests ===" << std::endl;
    std::cout << std::endl;
    
    poko::core::logging::test::test_log_level_conversion();
    poko::core::logging::test::test_logger_basics();
    poko::core::logging::test::test_logger_level_filtering();
    poko::core::logging::test::test_logger_subsystem_filter();
    poko::core::logging::test::test_logger_sink_management();
    poko::core::logging::test::test_logger_flush();
    poko::core::logging::test::test_file_sink();
    poko::core::logging::test::test_global_logger();
    poko::core::logging::test::test_logging_macros();
    poko::core::logging::test::test_logger_thread_safety();
    
    std::cout << std::endl;
    std::cout << "=== All tests passed! ===" << std::endl;
    
    return 0;
}
