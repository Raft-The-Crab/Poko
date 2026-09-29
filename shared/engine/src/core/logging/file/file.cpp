/**
 * @file file.cpp
 * @brief File sink implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/logging/logger.h"
#include <iostream>
#include <cstdio>

namespace poko {
namespace core {
namespace logging {

/**
 * @brief Constructor
 */
FileSink::FileSink(const std::string& filepath)
    : m_filepath(filepath)
    , m_file(nullptr)
{
    // Validate filepath length
    if (filepath.length() > MAX_LOG_FILEPATH_LENGTH) {
        std::cerr << "Log filepath exceeds maximum length: " << filepath << std::endl;
        return;
    }
    
    m_file = fopen(filepath.c_str(), "a");
    if (!m_file) {
        std::cerr << "Failed to open log file: " << filepath << std::endl;
    }
}

/**
 * @brief Destructor
 */
FileSink::~FileSink() {
    if (m_file) {
        fclose(m_file);
    }
}

/**
 * @brief Write to file
 */
void FileSink::write(const LogMessage& message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_file) {
        fprintf(m_file, "[%s] [%s] [%s] %s\n",
                message.timestamp.c_str(),
                logLevelToString(message.level),
                message.subsystem.c_str(),
                message.message.c_str());
    }
}

/**
 * @brief Flush file
 */
void FileSink::flush() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (m_file) {
        fflush(m_file);
    }
}

} // namespace logging
} // namespace core
} // namespace poko
