/**
 * @file interface.cpp
 * @brief Global profiler manager interface implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/profiler/profiler.h"
#include <memory>

namespace poko {
namespace core {
namespace profiler {

namespace {
    std::unique_ptr<ProfilerManager> g_globalProfilerManager;
    std::mutex g_globalMutex;
}

ProfilerManager& getGlobalProfilerManager() {
    std::lock_guard<std::mutex> lock(g_globalMutex);
    
    if (!g_globalProfilerManager) {
        g_globalProfilerManager = std::make_unique<ProfilerManager>();
    }
    
    return *g_globalProfilerManager;
}

void destroyGlobalProfilerManager() {
    std::lock_guard<std::mutex> lock(g_globalMutex);
    g_globalProfilerManager.reset();
}

// ============================================================================
// ProfilerScope RAII Helper Implementation
// ============================================================================

ProfilerScope::ProfilerScope(const std::string& name, const std::string& category)
    : m_scopeId(0)
    , m_valid(false)
{
    auto& profiler = getGlobalProfilerManager();
    m_scopeId = profiler.beginScope(name, category);
    m_valid = (m_scopeId != 0);
}

ProfilerScope::~ProfilerScope() {
    if (m_valid && m_scopeId != 0) {
        auto& profiler = getGlobalProfilerManager();
        profiler.endScope(m_scopeId);
    }
}

ProfilerScope::ProfilerScope(ProfilerScope&& other) noexcept
    : m_scopeId(other.m_scopeId)
    , m_valid(other.m_valid)
{
    other.m_scopeId = 0;
    other.m_valid = false;
}

ProfilerScope& ProfilerScope::operator=(ProfilerScope&& other) noexcept {
    if (this != &other) {
        if (m_valid && m_scopeId != 0) {
            auto& profiler = getGlobalProfilerManager();
            profiler.endScope(m_scopeId);
        }
        
        m_scopeId = other.m_scopeId;
        m_valid = other.m_valid;
        
        other.m_scopeId = 0;
        other.m_valid = false;
    }
    return *this;
}

} // namespace profiler
} // namespace core
} // namespace poko
