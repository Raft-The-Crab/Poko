/**
 * @file diagnostics.h
 * @brief Diagnostics system for debugging and error reporting
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_DIAGNOSTICS_DIAGNOSTICS_H
#define POKO_CORE_DIAGNOSTICS_DIAGNOSTICS_H

#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <mutex>

namespace poko {
namespace core {
namespace diagnostics {

// ============================================================================
// Severity Levels
// ============================================================================

/// Diagnostic message severity
enum class Severity : uint32_t {
    Debug = 0,       ///< Debug information
    Info = 1,        ///< General information
    Warning = 2,     ///< Warning message
    Error = 3,       ///< Error message
    Fatal = 4,       ///< Fatal error
    Trace = 5        ///< Trace information
};

// ============================================================================
// Diagnostic Message
// ============================================================================

/**
 * @brief Diagnostic message structure
 */
struct DiagnosticMessage {
    Severity severity;           ///< Message severity
    std::string category;        ///< Message category (e.g., "Memory", "Rendering")
    std::string message;        ///< Message text
    std::string file;           ///< Source file where diagnostic was generated
    uint32_t line;              ///< Source line number
    std::string function;       ///< Function name
    uint64_t timestamp;         ///< Timestamp in milliseconds
    uint32_t threadId;          ///< Thread ID
};

// ============================================================================
// Diagnostic Handler
// ============================================================================

/// Diagnostic handler function signature
using DiagnosticHandler = std::function<void(const DiagnosticMessage&)>;

// ============================================================================
// Diagnostics Manager
// ============================================================================

/**
 * @brief Diagnostics manager for error reporting and debugging
 */
class DiagnosticsManager {
public:
    DiagnosticsManager();
    ~DiagnosticsManager();
    
    /**
     * @brief Register a diagnostic handler
     * @param handler Handler function to register
     * @returns true if successful, false if handler is null or maximum handler limit reached
     * @note Thread-safe: Acquires mutex lock
     */
    bool registerHandler(DiagnosticHandler handler);
    
    /**
     * @brief Unregister all handlers
     * @note Thread-safe: Acquires mutex lock
     */
    void unregisterAllHandlers();
    
    /**
     * @brief Report a diagnostic message
     * @param severity Message severity
     * @param category Message category (must be non-empty and <= MAX_CATEGORY_NAME_LENGTH)
     * @param message Message text (must be non-empty and <= MAX_DIAGNOSTIC_MESSAGE_LENGTH)
     * @param file Source file (optional, must be <= MAX_FILEPATH_LENGTH)
     * @param line Source line (optional)
     * @param function Function name (optional, must be <= MAX_FUNCTION_NAME_LENGTH)
     * @note Thread-safe: Acquires mutex lock
     * @note Invalid inputs (empty strings, length limits exceeded) are silently rejected
     */
    void report(Severity severity, const std::string& category, const std::string& message,
             const std::string& file = "", uint32_t line = 0, const std::string& function = "");
    
    /**
     * @brief Report a debug message
     * @param category Message category (must be non-empty and <= MAX_CATEGORY_NAME_LENGTH)
     * @param message Message text (must be non-empty and <= MAX_DIAGNOSTIC_MESSAGE_LENGTH)
     * @param file Source file (optional, must be <= MAX_FILEPATH_LENGTH)
     * @param line Source line (optional)
     * @param function Function name (optional, must be <= MAX_FUNCTION_NAME_LENGTH)
     * @note Thread-safe: Acquires mutex lock
     * @note Invalid inputs are silently rejected
     */
    void debug(const std::string& category, const std::string& message,
             const std::string& file = "", uint32_t line = 0, const std::string& function = "");
    
    /**
     * @brief Report an info message
     * @param category Message category (must be non-empty and <= MAX_CATEGORY_NAME_LENGTH)
     * @param message Message text (must be non-empty and <= MAX_DIAGNOSTIC_MESSAGE_LENGTH)
     * @param file Source file (optional, must be <= MAX_FILEPATH_LENGTH)
     * @param line Source line (optional)
     * @param function Function name (optional, must be <= MAX_FUNCTION_NAME_LENGTH)
     * @note Thread-safe: Acquires mutex lock
     * @note Invalid inputs are silently rejected
     */
    void info(const std::string& category, const std::string& message,
             const std::string& file = "", uint32_t line = 0, const std::string& function = "");
    
    /**
     * @brief Report a warning message
     * @param category Message category (must be non-empty and <= MAX_CATEGORY_NAME_LENGTH)
     * @param message Message text (must be non-empty and <= MAX_DIAGNOSTIC_MESSAGE_LENGTH)
     * @param file Source file (optional, must be <= MAX_FILEPATH_LENGTH)
     * @param line Source line (optional)
     * @param function Function name (optional, must be <= MAX_FUNCTION_NAME_LENGTH)
     * @note Thread-safe: Acquires mutex lock
     * @note Invalid inputs are silently rejected
     */
    void warning(const std::string& category, const std::string& message,
                const std::string& file = "", uint32_t line = 0, const std::string& function = "");
    
    /**
     * @brief Report an error message
     * @param category Message category (must be non-empty and <= MAX_CATEGORY_NAME_LENGTH)
     * @param message Message text (must be non-empty and <= MAX_DIAGNOSTIC_MESSAGE_LENGTH)
     * @param file Source file (optional, must be <= MAX_FILEPATH_LENGTH)
     * @param line Source line (optional)
     * @param function Function name (optional, must be <= MAX_FUNCTION_NAME_LENGTH)
     * @note Thread-safe: Acquires mutex lock
     * @note Invalid inputs are silently rejected
     */
    void error(const std::string& category, const std::string& message,
               const std::string& file = "", uint32_t line = 0, const std::string& function = "");
    
    /**
     * @brief Report a fatal error message
     * @param category Message category (must be non-empty and <= MAX_CATEGORY_NAME_LENGTH)
     * @param message Message text (must be non-empty and <= MAX_DIAGNOSTIC_MESSAGE_LENGTH)
     * @param file Source file (optional, must be <= MAX_FILEPATH_LENGTH)
     * @param line Source line (optional)
     * @param function Function name (optional, must be <= MAX_FUNCTION_NAME_LENGTH)
     * @note Thread-safe: Acquires mutex lock
     * @note Invalid inputs are silently rejected
     */
    void fatal(const std::string& category, const std::string& message,
               const std::string& file = "", uint32_t line = 0, const std::string& function = "");
    
    /**
     * @brief Report a trace message
     * @param category Message category (must be non-empty and <= MAX_CATEGORY_NAME_LENGTH)
     * @param message Message text (must be non-empty and <= MAX_DIAGNOSTIC_MESSAGE_LENGTH)
     * @param file Source file (optional, must be <= MAX_FILEPATH_LENGTH)
     * @param line Source line (optional)
     * @param function Function name (optional, must be <= MAX_FUNCTION_NAME_LENGTH)
     * @note Thread-safe: Acquires mutex lock
     * @note Invalid inputs are silently rejected
     */
    void trace(const std::string& category, const std::string& message,
               const std::string& file = "", uint32_t line = 0, const std::string& function = "");
    
    /**
     * @brief Enable/disable a specific severity level
     * @param severity Severity level to enable/disable
     * @param enabled true to enable, false to disable
     * @note Thread-safe: Acquires mutex lock
     * @note Invalid severity values are silently ignored
     */
    void setSeverityEnabled(Severity severity, bool enabled);
    
    /**
     * @brief Check if a severity level is enabled
     * @param severity Severity level to check
     * @returns true if enabled, false otherwise
     * @note Thread-safe: Acquires mutex lock
     * @note Invalid severity values return false
     */
    bool isSeverityEnabled(Severity severity) const;
    
    /**
     * @brief Enable/disable a specific category
     * @param category Category to enable/disable (must be non-empty and <= MAX_CATEGORY_NAME_LENGTH)
     * @param enabled true to enable, false to disable
     * @note Thread-safe: Acquires mutex lock
     * @note Invalid category values are silently ignored
     * @note When enabledCategories is empty, all categories are enabled
     */
    void setCategoryEnabled(const std::string& category, bool enabled);
    
    /**
     * @brief Check if a category is enabled
     * @param category Category to check
     * @returns true if enabled, false otherwise
     * @note Thread-safe: Acquires mutex lock
     * @note Empty category returns false
     * @note When enabledCategories is empty, all categories are enabled
     */
    bool isCategoryEnabled(const std::string& category) const;
    
    /**
     * @brief Get diagnostic message history
     * @returns Vector of diagnostic messages
     * @note Thread-safe: Acquires mutex lock
     * @note Returns a copy of the history for thread-safe access
     */
    std::vector<DiagnosticMessage> getHistory() const;
    
    /**
     * @brief Clear diagnostic history
     * @note Thread-safe: Acquires mutex lock
     */
    void clearHistory() noexcept;
    
    /**
     * @brief Set maximum history size
     * @param maxSize Maximum number of messages to keep
     * @note Thread-safe: Acquires mutex lock
     * @note If current history exceeds new limit, oldest messages are removed
     */
    void setMaxHistorySize(size_t maxSize);
    
    /**
     * @brief Get diagnostic statistics
     * @returns Statistics structure with message counts
     * @note Thread-safe: Acquires mutex lock
     * @note Returns a copy of the statistics for thread-safe access
     */
    struct Statistics {
        uint64_t totalMessages;       ///< Total messages reported
        uint64_t debugCount;         ///< Debug message count
        uint64_t infoCount;          ///< Info message count
        uint64_t warningCount;       ///< Warning message count
        uint64_t errorCount;         ///< Error message count
        uint64_t fatalCount;         ///< Fatal message count
        uint64_t traceCount;         ///< Trace message count
    };
    
    Statistics getStatistics() const;
    
    /**
     * @brief Reset statistics
     */
    void resetStatistics();
    
private:
    mutable std::mutex m_mutex;
    std::vector<DiagnosticHandler> m_handlers;
    std::vector<DiagnosticMessage> m_history;
    size_t m_maxHistorySize;
    bool m_severityEnabled[6];  // One for each severity level
    std::vector<std::string> m_enabledCategories;
    Statistics m_statistics;
};

// ============================================================================
// Global Diagnostics Manager
// ============================================================================

/**
 * @brief Get global diagnostics manager
 * @returns Reference to global diagnostics manager
 */
DiagnosticsManager& getGlobalDiagnosticsManager();

/**
 * @brief Destroy global diagnostics manager
 */
void destroyGlobalDiagnosticsManager();

// ============================================================================
// Convenience Macros
// ============================================================================

#define POKO_DIAG_DEBUG(category, message) \
    poko::core::diagnostics::getGlobalDiagnosticsManager().debug(category, message, __FILE__, __LINE__, __FUNCTION__)

#define POKO_DIAG_INFO(category, message) \
    poko::core::diagnostics::getGlobalDiagnosticsManager().info(category, message, __FILE__, __LINE__, __FUNCTION__)

#define POKO_DIAG_WARNING(category, message) \
    poko::core::diagnostics::getGlobalDiagnosticsManager().warning(category, message, __FILE__, __LINE__, __FUNCTION__)

#define POKO_DIAG_ERROR(category, message) \
    poko::core::diagnostics::getGlobalDiagnosticsManager().error(category, message, __FILE__, __LINE__, __FUNCTION__)

#define POKO_DIAG_FATAL(category, message) \
    poko::core::diagnostics::getGlobalDiagnosticsManager().fatal(category, message, __FILE__, __LINE__, __FUNCTION__)

#define POKO_DIAG_TRACE(category, message) \
    poko::core::diagnostics::getGlobalDiagnosticsManager().trace(category, message, __FILE__, __LINE__, __FUNCTION__)

// ============================================================================
// Constants
// ============================================================================

/// Maximum diagnostic history size
constexpr size_t MAX_DIAGNOSTIC_HISTORY_SIZE = 10000;

/// Maximum category name length
constexpr size_t MAX_CATEGORY_NAME_LENGTH = 128;

/// Maximum diagnostic message length
constexpr size_t MAX_DIAGNOSTIC_MESSAGE_LENGTH = 4096;

/// Maximum file path length
constexpr size_t MAX_FILEPATH_LENGTH = 1024;

/// Maximum function name length
constexpr size_t MAX_FUNCTION_NAME_LENGTH = 256;

/// Maximum number of handlers that can be registered
constexpr size_t MAX_DIAGNOSTIC_HANDLERS = 100;

} // namespace diagnostics
} // namespace core
} // namespace poko

#endif // POKO_CORE_DIAGNOSTICS_DIAGNOSTICS_H
