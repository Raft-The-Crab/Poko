/**
 * @file console.cpp
 * @brief Console sink implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/logging/logger.h"
#include <iostream>

namespace poko {
namespace core {
namespace logging {

/**
 * @brief Write to console
 */
void ConsoleSink::write(const LogMessage& message) {
    std::cout << logLevelToString(message.level) << " ["
              << message.subsystem << "] " << message.message << std::endl;
}

/**
 * @brief Flush console
 */
void ConsoleSink::flush() {
    std::cout.flush();
}

} // namespace logging
} // namespace core
} // namespace poko
