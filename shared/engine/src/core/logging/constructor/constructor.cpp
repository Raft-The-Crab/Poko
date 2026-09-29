/**
 * @file constructor.cpp
 * @brief Logger constructor and destructor implementation
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
 * @brief Constructor
 * 
 * Initializes a logger with default settings:
 * - Info minimum level
 * - No subsystem filter
 * - Console sink added by default
 */
Logger::Logger()
    : m_sinks()
    , m_minLevel(LogLevel::Info)
    , m_subsystemFilter()
{
    // Add console sink by default
    addSink(std::make_shared<ConsoleSink>());
}

/**
 * @brief Destructor
 * 
 * Cleans up the logger and all sinks.
 */
Logger::~Logger() = default;

} // namespace logging
} // namespace core
} // namespace poko
