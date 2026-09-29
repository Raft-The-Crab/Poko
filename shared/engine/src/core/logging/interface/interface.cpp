/**
 * @file interface.cpp
 * @brief Logging interface implementation - global logger instance
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/logging/logger.h"
#include <mutex>

namespace poko {
namespace core {
namespace logging {

namespace {
    std::mutex g_loggerMutex;
    Logger* g_globalLogger = nullptr;
}

/**
 * @brief Get global logger
 * 
 * Returns a reference to the globally shared Logger instance.
 * This logger is intended for general-purpose logging throughout the engine.
 * 
 * @return Reference to global logger
 * 
 * @note Thread-safe access is guaranteed
 * @note Do not destroy this logger - it's globally managed
 */
Logger& getGlobalLogger() {
    std::lock_guard<std::mutex> lock(g_loggerMutex);
    
    if (!g_globalLogger) {
        static Logger instance;
        g_globalLogger = &instance;
    }
    
    return *g_globalLogger;
}

} // namespace logging
} // namespace core
} // namespace poko
