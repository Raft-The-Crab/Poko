/**
 * @file profiler.h
 * @brief Engine profiling and performance tracking
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#pragma once

#include <string>
#include <chrono>
#include <unordered_map>
#include <vector>

namespace Poko {
namespace Profiling {

/**
 * @brief Performance timing data
 */
struct TimingData {
    std::string name;
    double total_time_ms;
    double average_time_ms;
    double min_time_ms;
    double max_time_ms;
    uint64_t call_count;
};

/**
 * @brief Memory tracking data
 */
struct MemoryData {
    std::string category;
    size_t allocated_bytes;
    size_t freed_bytes;
    size_t current_bytes;
    uint64_t allocation_count;
};

/**
 * @brief Engine profiler for performance monitoring
 */
class Profiler {
public:
    static Profiler& get_instance();

    /**
     * @brief Start timing a scope
     * @param name Scope name
     */
    void begin_scope(const std::string& name);

    /**
     * @brief End timing a scope
     * @param name Scope name
     */
    void end_scope(const std::string& name);

    /**
     * @brief Track memory allocation
     * @param category Memory category
     * @param bytes Number of bytes allocated
     */
    void track_allocation(const std::string& category, size_t bytes);

    /**
     * @brief Track memory deallocation
     * @param category Memory category
     * @param bytes Number of bytes freed
     */
    void track_deallocation(const std::string& category, size_t bytes);

    /**
     * @brief Get timing data for a scope
     * @param name Scope name
     * @return Timing data
     */
    TimingData get_timing_data(const std::string& name) const;

    /**
     * @brief Get memory data for a category
     * @param category Memory category
     * @return Memory data
     */
    MemoryData get_memory_data(const std::string& category) const;

    /**
     * @brief Get all timing data
     * @return All timing data
     */
    std::vector<TimingData> get_all_timing_data() const;

    /**
     * @brief Get all memory data
     * @return All memory data
     */
    std::vector<MemoryData> get_all_memory_data() const;

    /**
     * @brief Reset all profiling data
     */
    void reset();

    /**
     * @brief Print profiling report
     */
    void print_report() const;

private:
    Profiler() = default;

    struct ScopeTimer {
        std::chrono::high_resolution_clock::time_point start_time;
    };

    std::unordered_map<std::string, ScopeTimer> m_active_scopes;
    std::unordered_map<std::string, TimingData> m_timing_data;
    std::unordered_map<std::string, MemoryData> m_memory_data;
};

/**
 * @brief RAII scope timer for automatic profiling
 */
class ScopeTimer {
public:
    ScopeTimer(const std::string& name);
    ~ScopeTimer();

private:
    std::string m_name;
};

#define PROFILE_SCOPE(name) Poko::Profiling::ScopeTimer _scope_timer(name)

} // namespace Profiling
} // namespace Poko