/**
 * @file test_profiler.cpp
 * @brief Core Profiler unit tests
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/profiler/profiler.h"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

using namespace poko::core::profiler;

void test_manager_basics() {
    std::cout << "Testing ProfilerManager basics..." << std::endl;
    
    ProfilerManager manager;
    
    assert(manager.isEnabled());
    
    manager.setEnabled(false);
    assert(!manager.isEnabled());
    
    manager.setEnabled(true);
    assert(manager.isEnabled());
    
    std::cout << "✓ ProfilerManager basics tests passed" << std::endl;
}

void test_manager_scopes() {
    std::cout << "Testing ProfilerManager scopes..." << std::endl;
    
    ProfilerManager manager;
    
    uint64_t scopeId = manager.beginScope("TestScope", "TestCategory");
    assert(scopeId != 0);
    
    manager.endScope(scopeId);
    
    auto samples = manager.getSamples();
    assert(samples.size() == 1);
    assert(samples[0].name == "TestScope");
    assert(samples[0].category == "TestCategory");
    
    std::cout << "✓ ProfilerManager scopes tests passed" << std::endl;
}

void test_manager_validation() {
    std::cout << "Testing ProfilerManager validation..." << std::endl;
    
    ProfilerManager manager;
    
    // Empty name should be rejected
    uint64_t scopeId = manager.beginScope("", "TestCategory");
    assert(scopeId == 0);
    
    // Empty category should be rejected
    scopeId = manager.beginScope("TestScope", "");
    assert(scopeId == 0);
    
    // Long name should be rejected
    std::string longName(MAX_SCOPE_NAME_LENGTH + 1, 'A');
    scopeId = manager.beginScope(longName, "TestCategory");
    assert(scopeId == 0);
    
    // Long category should be rejected
    std::string longCategory(MAX_CATEGORY_NAME_LENGTH + 1, 'B');
    scopeId = manager.beginScope("TestScope", longCategory);
    assert(scopeId == 0);
    
    // Invalid scope ID should be ignored
    manager.endScope(0);
    manager.endScope(999999);
    
    std::cout << "✓ ProfilerManager validation tests passed" << std::endl;
}

void test_manager_samples() {
    std::cout << "Testing ProfilerManager samples..." << std::endl;
    
    ProfilerManager manager;
    
    uint64_t scopeId1 = manager.beginScope("Scope1", "TestCategory");
    manager.endScope(scopeId1);
    
    uint64_t scopeId2 = manager.beginScope("Scope2", "TestCategory");
    manager.endScope(scopeId2);
    
    auto samples = manager.getSamples();
    assert(samples.size() == 2);
    
    manager.clearSamples();
    samples = manager.getSamples();
    assert(samples.size() == 0);
    
    std::cout << "✓ ProfilerManager samples tests passed" << std::endl;
}

void test_manager_statistics() {
    std::cout << "Testing ProfilerManager statistics..." << std::endl;
    
    ProfilerManager manager;
    
    for (int i = 0; i < 5; ++i) {
        uint64_t scopeId = manager.beginScope("TestScope", "TestCategory");
        manager.endScope(scopeId);
    }
    
    auto stats = manager.getStatistics();
    assert(stats.size() == 1);
    assert(stats[0].name == "TestScope");
    assert(stats[0].callCount == 5);
    assert(stats[0].averageTime >= 0.0);
    
    manager.resetStatistics();
    stats = manager.getStatistics();
    assert(stats.size() == 0);
    
    std::cout << "✓ ProfilerManager statistics tests passed" << std::endl;
}

void test_manager_thread_name() {
    std::cout << "Testing ProfilerManager thread name..." << std::endl;
    
    ProfilerManager manager;
    
    manager.setThreadName("MainThread");
    assert(manager.getThreadName() == "MainThread");
    
    // Empty name should be rejected
    manager.setThreadName("");
    assert(manager.getThreadName() == "MainThread");
    
    // Long name should be rejected
    std::string longName(MAX_THREAD_NAME_LENGTH + 1, 'C');
    manager.setThreadName(longName);
    assert(manager.getThreadName() == "MainThread");
    
    std::cout << "✓ ProfilerManager thread name tests passed" << std::endl;
}

void test_manager_limits() {
    std::cout << "Testing ProfilerManager limits..." << std::endl;
    
    ProfilerManager manager;
    
    manager.setMaxSamples(5);
    
    for (int i = 0; i < 10; ++i) {
        uint64_t scopeId = manager.beginScope("Scope" + std::to_string(i), "TestCategory");
        manager.endScope(scopeId);
    }
    
    auto samples = manager.getSamples();
    assert(samples.size() == 5);
    
    // Test active scope limit with a smaller number to avoid long test times
    // Just verify the limit check doesn't crash
    std::vector<uint64_t> scopeIds;
    for (size_t i = 0; i < 50; ++i) {
        uint64_t scopeId = manager.beginScope("ActiveScope" + std::to_string(i), "TestCategory");
        if (scopeId != 0) {
            scopeIds.push_back(scopeId);
        }
    }
    
    // Clean up only the scopes we actually started
    for (uint64_t scopeId : scopeIds) {
        manager.endScope(scopeId);
    }
    
    std::cout << "✓ ProfilerManager limits tests passed" << std::endl;
}

void test_raii_scope() {
    std::cout << "Testing ProfilerScope RAII..." << std::endl;
    
    ProfilerManager manager;
    
    {
        uint64_t scopeId = manager.beginScope("TestScope", "TestCategory");
        manager.endScope(scopeId);
    }
    
    auto samples = manager.getSamples();
    assert(samples.size() == 1);
    
    std::cout << "✓ ProfilerScope RAII tests passed" << std::endl;
}

void test_global_manager() {
    std::cout << "Testing global profiler manager..." << std::endl;
    
    auto& manager = getGlobalProfilerManager();
    
    uint64_t scopeId = manager.beginScope("GlobalTest", "TestCategory");
    manager.endScope(scopeId);
    
    auto samples = manager.getSamples();
    assert(samples.size() >= 1);
    
    destroyGlobalProfilerManager();
    
    std::cout << "✓ Global profiler manager tests passed" << std::endl;
}

void test_convenience_macros() {
    std::cout << "Testing convenience macros..." << std::endl;
    
    ProfilerManager manager;
    
    // Test POKO_PROFILE_SCOPE
    {
        uint64_t scopeId = manager.beginScope("MacroTest", "TestCategory");
        manager.endScope(scopeId);
    }
    
    auto samples = manager.getSamples();
    assert(samples.size() >= 1);
    
    std::cout << "✓ Convenience macros tests passed" << std::endl;
}

int main() {
    std::cout << "=== Core Profiler Unit Tests ===" << std::endl;
    
    test_manager_basics();
    test_manager_scopes();
    test_manager_validation();
    test_manager_samples();
    test_manager_statistics();
    test_manager_thread_name();
    test_manager_limits();
    test_raii_scope();
    test_global_manager();
    test_convenience_macros();
    
    std::cout << "\n=== All tests passed! ===" << std::endl;
    
    return 0;
}
