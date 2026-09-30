/**
 * @file manager.cpp
 * @brief Profiler manager implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/profiler/profiler.h"
#include <algorithm>
#include <unordered_map>

namespace poko {
namespace core {
namespace profiler {

namespace {
    thread_local uint32_t t_threadDepth = 0;
}

ProfilerManager::ProfilerManager()
    : m_enabled(true)
    , m_maxSamples(MAX_PROFILER_SAMPLES)
    , m_nextScopeId(1)
    , m_threadDepth(0)
{
}

ProfilerManager::~ProfilerManager() {
    // Cleanup is automatic
}

void ProfilerManager::setEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_enabled = enabled;
}

bool ProfilerManager::isEnabled() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_enabled;
}

uint64_t ProfilerManager::beginScope(const std::string& name, const std::string& category) {
    // Validate name length
    if (name.empty() || name.length() > MAX_SCOPE_NAME_LENGTH) {
        return 0;
    }
    
    // Validate category length
    if (category.empty() || category.length() > MAX_CATEGORY_NAME_LENGTH) {
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_enabled) {
        return 0;
    }
    
    uint64_t scopeId = m_nextScopeId++;
    uint64_t startTime = getCurrentTime();
    uint32_t threadId = getCurrentThreadId();
    
    ActiveScope scope;
    scope.id = scopeId;
    scope.name = name;
    scope.category = category;
    scope.startTime = startTime;
    scope.threadId = threadId;
    scope.depth = m_threadDepth;
    
    m_activeScopes.push_back(scope);
    m_threadDepth++;
    t_threadDepth++;
    
    return scopeId;
}

void ProfilerManager::endScope(uint64_t scopeId) {
    if (scopeId == 0) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // Find the active scope
    auto it = std::find_if(m_activeScopes.begin(), m_activeScopes.end(),
        [scopeId](const ActiveScope& scope) {
            return scope.id == scopeId;
        });
    
    if (it == m_activeScopes.end()) {
        return;
    }
    
    uint64_t endTime = getCurrentTime();
    uint64_t duration = endTime - it->startTime;
    
    // Create sample
    ProfilerSample sample;
    sample.name = it->name;
    sample.category = it->category;
    sample.startTime = it->startTime;
    sample.duration = duration;
    sample.threadId = it->threadId;
    sample.depth = it->depth;
    
    // Add to samples
    m_samples.push_back(sample);
    
    // Trim samples if too large
    if (m_samples.size() > m_maxSamples) {
        m_samples.erase(m_samples.begin());
    }
    
    // Update statistics
    bool found = false;
    for (auto& stat : m_statistics) {
        if (stat.name == it->name) {
            stat.callCount++;
            stat.totalTime += duration;
            if (duration < stat.minTime) {
                stat.minTime = duration;
            }
            if (duration > stat.maxTime) {
                stat.maxTime = duration;
            }
            stat.averageTime = static_cast<double>(stat.totalTime) / stat.callCount;
            found = true;
            break;
        }
    }
    
    if (!found) {
        ProfilerStatistics stat;
        stat.name = it->name;
        stat.callCount = 1;
        stat.totalTime = duration;
        stat.minTime = duration;
        stat.maxTime = duration;
        stat.averageTime = static_cast<double>(duration);
        m_statistics.push_back(stat);
    }
    
    // Remove from active scopes
    m_activeScopes.erase(it);
    m_threadDepth--;
    t_threadDepth--;
}

std::vector<ProfilerSample> ProfilerManager::getSamples() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_samples;
}

void ProfilerManager::clearSamples() noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_samples.clear();
}

void ProfilerManager::setMaxSamples(size_t maxSamples) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_maxSamples = maxSamples;
    
    // Trim samples if necessary
    while (m_samples.size() > m_maxSamples) {
        m_samples.erase(m_samples.begin());
    }
}

std::vector<ProfilerStatistics> ProfilerManager::getStatistics() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_statistics;
}

void ProfilerManager::resetStatistics() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_statistics.clear();
}

void ProfilerManager::setThreadName(const std::string& name) {
    if (name.empty() || name.length() > MAX_THREAD_NAME_LENGTH) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    m_threadName = name;
}

std::string ProfilerManager::getThreadName() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_threadName;
}

uint64_t ProfilerManager::getCurrentTime() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

uint32_t ProfilerManager::getCurrentThreadId() const {
    // Simple thread ID hash
    return static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&t_threadDepth));
}

} // namespace profiler
} // namespace core
} // namespace poko
