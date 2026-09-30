/**
 * @file profiler.h
 * @brief Profiler hooks for performance monitoring and instrumentation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_PROFILER_PROFILER_H
#define POKO_CORE_PROFILER_PROFILER_H

#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <mutex>
#include <chrono>

namespace poko {
namespace core {
namespace profiler {

// ============================================================================
// Profiler Sample
// ============================================================================

/**
 * @brief Profiler sample representing a single measurement
 */
struct ProfilerSample {
    std::string name;              ///< Sample name (e.g., function name, operation)
    std::string category;          ///< Sample category (e.g., "Rendering", "Physics")
    uint64_t startTime;            ///< Start time in microseconds
    uint64_t duration;             ///< Duration in microseconds
    uint32_t threadId;             ///< Thread ID
    uint32_t depth;                ///< Call stack depth
};

// ============================================================================
// Profiler Statistics
// ============================================================================

/**
 * @brief Profiler statistics for a named scope
 */
struct ProfilerStatistics {
    std::string name;              ///< Scope name
    uint64_t callCount;            ///< Total call count
    uint64_t totalTime;            ///< Total time in microseconds
    uint64_t minTime;              ///< Minimum time in microseconds
    uint64_t maxTime;              ///< Maximum time in microseconds
    double averageTime;            ///< Average time in microseconds
};

// ============================================================================
// Profiler Manager
// ============================================================================

/**
 * @brief Profiler manager for performance monitoring
 */
class ProfilerManager {
public:
    ProfilerManager();
    ~ProfilerManager();
    
    /**
     * @brief Enable/disable profiling
     * @param enabled true to enable, false to disable
     * @note Thread-safe: Acquires mutex lock
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief Check if profiling is enabled
     * @returns true if enabled, false otherwise
     * @note Thread-safe: Acquires mutex lock
     */
    bool isEnabled() const;
    
    /**
     * @brief Begin a profiling scope
     * @param name Scope name (must be non-empty and <= MAX_SCOPE_NAME_LENGTH)
     * @param category Scope category (must be non-empty and <= MAX_CATEGORY_NAME_LENGTH)
     * @returns Scope ID for endScope call, or 0 if profiling disabled or invalid input
     * @note Thread-safe: Acquires mutex lock
     * @note Invalid inputs are silently rejected
     */
    uint64_t beginScope(const std::string& name, const std::string& category);
    
    /**
     * @brief End a profiling scope
     * @param scopeId Scope ID returned by beginScope
     * @note Thread-safe: Acquires mutex lock
     * @note Invalid scope IDs are silently ignored
     */
    void endScope(uint64_t scopeId);
    
    /**
     * @brief Get profiler samples
     * @returns Vector of profiler samples
     * @note Thread-safe: Acquires mutex lock
     * @note Returns a copy of the samples for thread-safe access
     */
    std::vector<ProfilerSample> getSamples() const;
    
    /**
     * @brief Clear profiler samples
     * @note Thread-safe: Acquires mutex lock
     */
    void clearSamples() noexcept;
    
    /**
     * @brief Set maximum sample count
     * @param maxSamples Maximum number of samples to keep
     * @note Thread-safe: Acquires mutex lock
     * @note If current samples exceed new limit, oldest samples are removed
     */
    void setMaxSamples(size_t maxSamples);
    
    /**
     * @brief Get profiler statistics
     * @returns Vector of profiler statistics for each scope
     * @note Thread-safe: Acquires mutex lock
     * @note Returns a copy of the statistics for thread-safe access
     */
    std::vector<ProfilerStatistics> getStatistics() const;
    
    /**
     * @brief Reset statistics
     * @note Thread-safe: Acquires mutex lock
     */
    void resetStatistics();
    
    /**
     * @brief Set current thread name
     * @param name Thread name (must be non-empty and <= MAX_THREAD_NAME_LENGTH)
     * @note Thread-safe: Acquires mutex lock
     * @note Invalid inputs are silently ignored
     */
    void setThreadName(const std::string& name);
    
    /**
     * @brief Get current thread name
     * @returns Thread name, or empty string if not set
     * @note Thread-safe: Acquires mutex lock
     */
    std::string getThreadName() const;
    
private:
    mutable std::mutex m_mutex;
    bool m_enabled;
    std::vector<ProfilerSample> m_samples;
    size_t m_maxSamples;
    std::vector<ProfilerStatistics> m_statistics;
    std::string m_threadName;
    
    // Scope tracking
    struct ActiveScope {
        uint64_t id;
        std::string name;
        std::string category;
        uint64_t startTime;
        uint32_t threadId;
        uint32_t depth;
    };
    std::vector<ActiveScope> m_activeScopes;
    uint64_t m_nextScopeId;
    uint32_t m_threadDepth;
    
    uint64_t getCurrentTime() const;
    uint32_t getCurrentThreadId() const;
};

// ============================================================================
// Global Profiler Manager
// ============================================================================

/**
 * @brief Get global profiler manager
 * @returns Reference to global profiler manager
 */
ProfilerManager& getGlobalProfilerManager();

/**
 * @brief Destroy global profiler manager
 */
void destroyGlobalProfilerManager();

// ============================================================================
// Profiler Scope RAII Helper
// ============================================================================

/**
 * @brief RAII helper for automatic scope profiling
 */
class ProfilerScope {
public:
    /**
     * @brief Constructor - begins profiling scope
     * @param name Scope name
     * @param category Scope category
     */
    ProfilerScope(const std::string& name, const std::string& category);
    
    /**
     * @brief Destructor - ends profiling scope
     */
    ~ProfilerScope();
    
    // Non-copyable
    ProfilerScope(const ProfilerScope&) = delete;
    ProfilerScope& operator=(const ProfilerScope&) = delete;
    
    // Movable
    ProfilerScope(ProfilerScope&& other) noexcept;
    ProfilerScope& operator=(ProfilerScope&& other) noexcept;
    
private:
    uint64_t m_scopeId;
    bool m_valid;
};

// ============================================================================
// Convenience Macros
// ============================================================================

#define POKO_PROFILE_SCOPE(name, category) \
    poko::core::profiler::ProfilerScope POKO_CONCAT(profilerScope_, __LINE__)(name, category)

#define POKO_PROFILE_FUNCTION() \
    POKO_PROFILE_SCOPE(__FUNCTION__, "Function")

#define POKO_PROFILE_FUNCTION_CATEGORY(category) \
    POKO_PROFILE_SCOPE(__FUNCTION__, category)

// Helper macro for unique variable names
#define POKO_CONCAT_IMPL(x, y) x##y
#define POKO_CONCAT(x, y) POKO_CONCAT_IMPL(x, y)

// ============================================================================
// Constants
// ============================================================================

/// Maximum scope name length
constexpr size_t MAX_SCOPE_NAME_LENGTH = 256;

/// Maximum category name length
constexpr size_t MAX_CATEGORY_NAME_LENGTH = 128;

/// Maximum thread name length
constexpr size_t MAX_THREAD_NAME_LENGTH = 64;

/// Maximum profiler samples
constexpr size_t MAX_PROFILER_SAMPLES = 10000;

} // namespace profiler
} // namespace core
} // namespace poko

#endif // POKO_CORE_PROFILER_PROFILER_H
