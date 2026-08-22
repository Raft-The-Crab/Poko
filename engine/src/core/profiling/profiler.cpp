/**
 * @file profiler.cpp
 * @brief Engine profiling implementation
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "core/profiling/profiler.h"
#include "core/logging/logger.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace Poko {
namespace Profiling {

Profiler& Profiler::get_instance()
{
    static Profiler instance;
    return instance;
}

void Profiler::begin_scope(const std::string& name)
{
    ScopeTimer timer;
    timer.start_time = std::chrono::high_resolution_clock::now();
    m_active_scopes[name] = timer;
}

void Profiler::end_scope(const std::string& name)
{
    auto it = m_active_scopes.find(name);
    if (it == m_active_scopes.end()) {
        LOG_WARNING("Profiler: Scope '" + name + "' not found in active scopes");
        return;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end_time - it->second.start_time).count();

    // Update timing data
    if (m_timing_data.find(name) == m_timing_data.end()) {
        TimingData data;
        data.name = name;
        data.total_time_ms = duration;
        data.average_time_ms = duration;
        data.min_time_ms = duration;
        data.max_time_ms = duration;
        data.call_count = 1;
        m_timing_data[name] = data;
    } else {
        auto& data = m_timing_data[name];
        data.total_time_ms += duration;
        data.call_count++;
        data.average_time_ms = data.total_time_ms / data.call_count;
        data.min_time_ms = std::min(data.min_time_ms, duration);
        data.max_time_ms = std::max(data.max_time_ms, duration);
    }

    m_active_scopes.erase(it);
}

void Profiler::track_allocation(const std::string& category, size_t bytes)
{
    if (m_memory_data.find(category) == m_memory_data.end()) {
        MemoryData data;
        data.category = category;
        data.allocated_bytes = bytes;
        data.freed_bytes = 0;
        data.current_bytes = bytes;
        data.allocation_count = 1;
        m_memory_data[category] = data;
    } else {
        auto& data = m_memory_data[category];
        data.allocated_bytes += bytes;
        data.current_bytes += bytes;
        data.allocation_count++;
    }
}

void Profiler::track_deallocation(const std::string& category, size_t bytes)
{
    if (m_memory_data.find(category) == m_memory_data.end()) {
        LOG_WARNING("Profiler: Category '" + category + "' not found in memory tracking");
        return;
    }

    auto& data = m_memory_data[category];
    data.freed_bytes += bytes;
    data.current_bytes = (data.current_bytes >= bytes) ? data.current_bytes - bytes : 0;
}

TimingData Profiler::get_timing_data(const std::string& name) const
{
    auto it = m_timing_data.find(name);
    if (it != m_timing_data.end()) {
        return it->second;
    }
    return TimingData{};
}

MemoryData Profiler::get_memory_data(const std::string& category) const
{
    auto it = m_memory_data.find(category);
    if (it != m_memory_data.end()) {
        return it->second;
    }
    return MemoryData{};
}

std::vector<TimingData> Profiler::get_all_timing_data() const
{
    std::vector<TimingData> data;
    for (const auto& pair : m_timing_data) {
        data.push_back(pair.second);
    }
    return data;
}

std::vector<MemoryData> Profiler::get_all_memory_data() const
{
    std::vector<MemoryData> data;
    for (const auto& pair : m_memory_data) {
        data.push_back(pair.second);
    }
    return data;
}

void Profiler::reset()
{
    m_active_scopes.clear();
    m_timing_data.clear();
    m_memory_data.clear();
}

void Profiler::print_report() const
{
    LOG_INFO("=== Performance Report ===");

    // Print timing data
    LOG_INFO("--- Timing Data ---");
    for (const auto& pair : m_timing_data) {
        const auto& data = pair.second;
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3);
        oss << data.name << ": avg=" << data.average_time_ms << "ms, "
            << "min=" << data.min_time_ms << "ms, "
            << "max=" << data.max_time_ms << "ms, "
            << "calls=" << data.call_count;
        LOG_INFO(oss.str());
    }

    // Print memory data
    LOG_INFO("--- Memory Data ---");
    for (const auto& pair : m_memory_data) {
        const auto& data = pair.second;
        std::ostringstream oss;
        oss << data.category << ": allocated=" << data.allocated_bytes << " bytes, "
            << "freed=" << data.freed_bytes << " bytes, "
            << "current=" << data.current_bytes << " bytes, "
            << "allocations=" << data.allocation_count;
        LOG_INFO(oss.str());
    }

    LOG_INFO("=== End Report ===");
}

ScopeTimer::ScopeTimer(const std::string& name)
    : m_name(name)
{
    Profiler::get_instance().begin_scope(name);
}

ScopeTimer::~ScopeTimer()
{
    Profiler::get_instance().end_scope(m_name);
}

} // namespace Profiling
} // namespace Poko