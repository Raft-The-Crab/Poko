/**
 * @file manager.cpp
 * @brief Diagnostics manager implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/diagnostics/diagnostics.h"
#include <algorithm>

namespace poko {
namespace core {
namespace diagnostics {

namespace {
    uint64_t getCurrentTimestamp() {
        // Use platform layer to get current time in milliseconds
        // For now, use a simple counter-based timestamp since we don't have time module linked
        static uint64_t counter = 0;
        return ++counter;
    }
}

DiagnosticsManager::DiagnosticsManager()
    : m_maxHistorySize(MAX_DIAGNOSTIC_HISTORY_SIZE)
{
    // Enable all severity levels by default
    for (int i = 0; i < 6; ++i) {
        m_severityEnabled[i] = true;
    }
    
    // Initialize statistics
    m_statistics.totalMessages = 0;
    m_statistics.debugCount = 0;
    m_statistics.infoCount = 0;
    m_statistics.warningCount = 0;
    m_statistics.errorCount = 0;
    m_statistics.fatalCount = 0;
    m_statistics.traceCount = 0;
}

DiagnosticsManager::~DiagnosticsManager() {
    unregisterAllHandlers();
}

bool DiagnosticsManager::registerHandler(DiagnosticHandler handler) {
    if (!handler) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    m_handlers.push_back(handler);
    return true;
}

void DiagnosticsManager::unregisterAllHandlers() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_handlers.clear();
}

void DiagnosticsManager::report(Severity severity, const std::string& category, const std::string& message,
                                const std::string& file, uint32_t line, const std::string& function) {
    // Validate category length
    if (category.empty() || category.length() > MAX_CATEGORY_NAME_LENGTH) {
        return;
    }
    
    // Validate message length
    if (message.empty() || message.length() > MAX_DIAGNOSTIC_MESSAGE_LENGTH) {
        return;
    }
    
    // Validate filepath length
    if (file.length() > MAX_FILEPATH_LENGTH) {
        return;
    }
    
    // Validate function name length
    if (function.length() > MAX_FUNCTION_NAME_LENGTH) {
        return;
    }
    
    // Check if severity is enabled
    int severityIndex = static_cast<int>(severity);
    if (severityIndex < 0 || severityIndex >= 6) {
        return;
    }
    
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (!m_severityEnabled[severityIndex]) {
            return;
        }
        
        // Check if category is enabled
        bool categoryEnabled = false;
        if (m_enabledCategories.empty()) {
            categoryEnabled = true; // No filters means all categories enabled
        } else {
            for (const auto& cat : m_enabledCategories) {
                if (cat == category) {
                    categoryEnabled = true;
                    break;
                }
            }
        }
        
        if (!categoryEnabled) {
            return;
        }
    }
    
    // Create diagnostic message
    DiagnosticMessage diag;
    diag.severity = severity;
    diag.category = category;
    diag.message = message;
    diag.file = file;
    diag.line = line;
    diag.function = function;
    diag.timestamp = getCurrentTimestamp();
    diag.threadId = getCurrentTimestamp(); // Simplified thread ID (use timestamp as proxy)
    
    // Update statistics
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_statistics.totalMessages++;
        
        switch (severity) {
            case Severity::Debug: m_statistics.debugCount++; break;
            case Severity::Info: m_statistics.infoCount++; break;
            case Severity::Warning: m_statistics.warningCount++; break;
            case Severity::Error: m_statistics.errorCount++; break;
            case Severity::Fatal: m_statistics.fatalCount++; break;
            case Severity::Trace: m_statistics.traceCount++; break;
        }
        
        // Add to history
        m_history.push_back(diag);
        
        // Trim history if too large
        if (m_history.size() > m_maxHistorySize) {
            m_history.erase(m_history.begin());
        }
        
        // Call handlers
        for (auto& handler : m_handlers) {
            handler(diag);
        }
    }
}

void DiagnosticsManager::debug(const std::string& category, const std::string& message,
                              const std::string& file, uint32_t line, const std::string& function) {
    report(Severity::Debug, category, message, file, line, function);
}

void DiagnosticsManager::info(const std::string& category, const std::string& message,
                             const std::string& file, uint32_t line, const std::string& function) {
    report(Severity::Info, category, message, file, line, function);
}

void DiagnosticsManager::warning(const std::string& category, const std::string& message,
                                const std::string& file, uint32_t line, const std::string& function) {
    report(Severity::Warning, category, message, file, line, function);
}

void DiagnosticsManager::error(const std::string& category, const std::string& message,
                               const std::string& file, uint32_t line, const std::string& function) {
    report(Severity::Error, category, message, file, line, function);
}

void DiagnosticsManager::fatal(const std::string& category, const std::string& message,
                               const std::string& file, uint32_t line, const std::string& function) {
    report(Severity::Fatal, category, message, file, line, function);
}

void DiagnosticsManager::trace(const std::string& category, const std::string& message,
                               const std::string& file, uint32_t line, const std::string& function) {
    report(Severity::Trace, category, message, file, line, function);
}

void DiagnosticsManager::setSeverityEnabled(Severity severity, bool enabled) {
    int severityIndex = static_cast<int>(severity);
    if (severityIndex < 0 || severityIndex >= 6) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    m_severityEnabled[severityIndex] = enabled;
}

bool DiagnosticsManager::isSeverityEnabled(Severity severity) const {
    int severityIndex = static_cast<int>(severity);
    if (severityIndex < 0 || severityIndex >= 6) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_severityEnabled[severityIndex];
}

void DiagnosticsManager::setCategoryEnabled(const std::string& category, bool enabled) {
    if (category.empty() || category.length() > MAX_CATEGORY_NAME_LENGTH) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (enabled) {
        // Add to enabled categories if not already present
        bool found = false;
        for (const auto& cat : m_enabledCategories) {
            if (cat == category) {
                found = true;
                break;
            }
        }
        if (!found) {
            m_enabledCategories.push_back(category);
        }
    } else {
        // Remove from enabled categories
        m_enabledCategories.erase(
            std::remove(m_enabledCategories.begin(), m_enabledCategories.end(), category),
            m_enabledCategories.end()
        );
    }
}

bool DiagnosticsManager::isCategoryEnabled(const std::string& category) const {
    if (category.empty()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // If no categories are explicitly enabled, all are enabled
    if (m_enabledCategories.empty()) {
        return true;
    }
    
    // Check if category is in enabled list
    for (const auto& cat : m_enabledCategories) {
        if (cat == category) {
            return true;
        }
    }
    
    return false;
}

std::vector<DiagnosticMessage> DiagnosticsManager::getHistory() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_history;
}

void DiagnosticsManager::clearHistory() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_history.clear();
}

void DiagnosticsManager::setMaxHistorySize(size_t maxSize) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_maxHistorySize = maxSize;
    
    // Trim history if necessary
    while (m_history.size() > m_maxHistorySize) {
        m_history.erase(m_history.begin());
    }
}

DiagnosticsManager::Statistics DiagnosticsManager::getStatistics() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_statistics;
}

void DiagnosticsManager::resetStatistics() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_statistics.totalMessages = 0;
    m_statistics.debugCount = 0;
    m_statistics.infoCount = 0;
    m_statistics.warningCount = 0;
    m_statistics.errorCount = 0;
    m_statistics.fatalCount = 0;
    m_statistics.traceCount = 0;
}

} // namespace diagnostics
} // namespace core
} // namespace poko
