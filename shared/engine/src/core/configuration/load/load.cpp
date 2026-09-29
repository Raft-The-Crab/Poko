/**
 * @file load.cpp
 * @brief Configuration load from file implementation
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
 * @brief Load configuration from file
 * 
 * Loads configuration from a simple key=value format file.
 * Lines starting with # are comments.
 * Uses exclusive lock for write operations.
 * 
 * @param filepath Path to configuration file
 * @return True if loaded successfully
 * 
 * @note Thread-safe via shared mutex (exclusive lock for writes)
 */
bool Configuration::loadFromFile(const std::string& filepath) {
    // Use unique_lock for write operations - exclusive access
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse key=value
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos) {
            std::string key = line.substr(0, eqPos);
            std::string valueStr = line.substr(eqPos + 1);
            
            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            valueStr.erase(0, valueStr.find_first_not_of(" \t"));
            valueStr.erase(valueStr.find_last_not_of(" \t") + 1);
            
            // Try to parse as different types
            if (valueStr == "true") {
                m_values[key] = true;
            } else if (valueStr == "false") {
                m_values[key] = false;
            } else {
                // Try integer
                try {
                    int intVal = std::stoi(valueStr);
                    m_values[key] = intVal;
                } catch (...) {
                    // Try double
                    try {
                        double doubleVal = std::stod(valueStr);
                        m_values[key] = doubleVal;
                    } catch (...) {
                        // String
                        m_values[key] = valueStr;
                    }
                }
            }
        }
    }
    
    return true;
}

} // namespace configuration
} // namespace core
} // namespace poko
