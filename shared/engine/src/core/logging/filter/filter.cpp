/**
 * @file filter.cpp
 * @brief Logger subsystem filter implementation
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
 * @brief Set subsystem filter
 */
void Logger::setSubsystemFilter(const std::string& subsystem) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_subsystemFilter = subsystem;
}

/**
 * @brief Get subsystem filter
 */
std::string Logger::getSubsystemFilter() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_subsystemFilter;
}

} // namespace logging
} // namespace core
} // namespace poko
