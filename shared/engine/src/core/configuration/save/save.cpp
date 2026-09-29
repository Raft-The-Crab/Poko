/**
 * @file save.cpp
 * @brief Configuration save to file implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/configuration/configuration.h"
#include <fstream>
#include <sstream>

namespace poko {
namespace core {
namespace configuration {

/**
 * @brief Save configuration to file
 * 
 * Saves configuration to a simple key=value format file.
 * Uses shared lock for concurrent reads.
 * 
 * @param filepath Path to configuration file
 * @return True if saved successfully
 * 
 * @note Thread-safe via shared mutex (allows concurrent reads)
 */
bool Configuration::saveToFile(const std::string& filepath) const {
    // Validate filepath
    if (filepath.empty()) {
        return false;
    }
    
    // Use shared_lock for read operations - allows concurrent reads
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write header
    file << "# Poko Engine Configuration File\n";
    file << "# Auto-generated - do not edit manually\n\n";
    
    // Write all values
    for (const auto& [key, value] : m_values) {
        if (std::holds_alternative<bool>(value)) {
            file << key << "=" << (std::get<bool>(value) ? "true" : "false") << "\n";
        } else if (std::holds_alternative<int>(value)) {
            file << key << "=" << std::get<int>(value) << "\n";
        } else if (std::holds_alternative<double>(value)) {
            file << key << "=" << std::get<double>(value) << "\n";
        } else if (std::holds_alternative<std::string>(value)) {
            file << key << "=" << std::get<std::string>(value) << "\n";
        }
    }
    
    return true;
}

} // namespace configuration
} // namespace core
} // namespace poko
