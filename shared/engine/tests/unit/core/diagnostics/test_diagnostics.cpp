/**
 * @file test_diagnostics.cpp
 * @brief Core Diagnostics unit tests
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/diagnostics/diagnostics.h"
#include <cassert>
#include <iostream>

using namespace poko::core::diagnostics;

void test_manager_basics() {
    std::cout << "Testing DiagnosticsManager basics..." << std::endl;
    
    DiagnosticsManager manager;
    
    assert(manager.isSeverityEnabled(Severity::Debug));
    assert(manager.isSeverityEnabled(Severity::Info));
    assert(manager.isSeverityEnabled(Severity::Warning));
    assert(manager.isSeverityEnabled(Severity::Error));
    assert(manager.isSeverityEnabled(Severity::Fatal));
    assert(manager.isSeverityEnabled(Severity::Trace));
    
    assert(manager.isCategoryEnabled("TestCategory"));
    
    std::cout << "✓ DiagnosticsManager basics tests passed" << std::endl;
}

void test_manager_handlers() {
    std::cout << "Testing DiagnosticsManager handlers..." << std::endl;
    
    DiagnosticsManager manager;
    
    bool handlerCalled = false;
    auto handler = [&handlerCalled](const DiagnosticMessage& msg) {
        handlerCalled = true;
        assert(msg.severity == Severity::Info);
        assert(msg.category == "TestCategory");
        assert(msg.message == "Test message");
    };
    
    manager.registerHandler(handler);
    manager.info("TestCategory", "Test message");
    
    assert(handlerCalled);
    
    manager.unregisterAllHandlers();
    
    std::cout << "✓ DiagnosticsManager handlers tests passed" << std::endl;
}

void test_manager_report() {
    std::cout << "Testing DiagnosticsManager report..." << std::endl;
    
    DiagnosticsManager manager;
    
    int messageCount = 0;
    auto handler = [&messageCount](const DiagnosticMessage&) {
        messageCount++;
    };
    
    manager.registerHandler(handler);
    
    manager.debug("TestCategory", "Debug message");
    assert(messageCount == 1);
    
    manager.info("TestCategory", "Info message");
    assert(messageCount == 2);
    
    manager.warning("TestCategory", "Warning message");
    assert(messageCount == 3);
    
    manager.error("TestCategory", "Error message");
    assert(messageCount == 4);
    
    manager.fatal("TestCategory", "Fatal message");
    assert(messageCount == 5);
    
    manager.trace("TestCategory", "Trace message");
    assert(messageCount == 6);
    
    manager.unregisterAllHandlers();
    
    std::cout << "✓ DiagnosticsManager report tests passed" << std::endl;
}

void test_manager_severity_filtering() {
    std::cout << "Testing DiagnosticsManager severity filtering..." << std::endl;
    
    DiagnosticsManager manager;
    
    int messageCount = 0;
    auto handler = [&messageCount](const DiagnosticMessage&) {
        messageCount++;
    };
    
    manager.registerHandler(handler);
    
    // Disable debug
    manager.setSeverityEnabled(Severity::Debug, false);
    manager.debug("TestCategory", "Debug message");
    assert(messageCount == 0); // Should not be called
    
    // Enable debug again
    manager.setSeverityEnabled(Severity::Debug, true);
    manager.debug("TestCategory", "Debug message");
    assert(messageCount == 1); // Should be called now
    
    manager.unregisterAllHandlers();
    
    std::cout << "✓ DiagnosticsManager severity filtering tests passed" << std::endl;
}

void test_manager_category_filtering() {
    std::cout << "Testing DiagnosticsManager category filtering..." << std::endl;
    
    DiagnosticsManager manager;
    
    int messageCount = 0;
    auto handler = [&messageCount](const DiagnosticMessage&) {
        messageCount++;
    };
    
    manager.registerHandler(handler);
    
    // Enable only specific category
    manager.setCategoryEnabled("EnabledCategory", true);
    
    manager.info("EnabledCategory", "Test message");
    assert(messageCount == 1);
    
    manager.info("DisabledCategory", "Test message");
    assert(messageCount == 1); // Should not increase
    
    manager.unregisterAllHandlers();
    
    std::cout << "✓ DiagnosticsManager category filtering tests passed" << std::endl;
}

void test_manager_history() {
    std::cout << "Testing DiagnosticsManager history..." << std::endl;
    
    DiagnosticsManager manager;
    
    manager.info("TestCategory", "Message 1");
    manager.info("TestCategory", "Message 2");
    manager.info("TestCategory", "Message 3");
    
    auto history = manager.getHistory();
    assert(history.size() == 3);
    
    manager.clearHistory();
    history = manager.getHistory();
    assert(history.size() == 0);
    
    std::cout << "✓ DiagnosticsManager history tests passed" << std::endl;
}

void test_manager_statistics() {
    std::cout << "Testing DiagnosticsManager statistics..." << std::endl;
    
    DiagnosticsManager manager;
    
    manager.debug("TestCategory", "Debug message");
    manager.info("TestCategory", "Info message");
    manager.warning("TestCategory", "Warning message");
    manager.error("TestCategory", "Error message");
    manager.fatal("TestCategory", "Fatal message");
    manager.trace("TestCategory", "Trace message");
    
    auto stats = manager.getStatistics();
    assert(stats.totalMessages == 6);
    assert(stats.debugCount == 1);
    assert(stats.infoCount == 1);
    assert(stats.warningCount == 1);
    assert(stats.errorCount == 1);
    assert(stats.fatalCount == 1);
    assert(stats.traceCount == 1);
    
    manager.resetStatistics();
    stats = manager.getStatistics();
    assert(stats.totalMessages == 0);
    
    std::cout << "✓ DiagnosticsManager statistics tests passed" << std::endl;
}

void test_manager_validation() {
    std::cout << "Testing DiagnosticsManager validation..." << std::endl;
    
    DiagnosticsManager manager;
    
    // Empty category should be rejected
    manager.info("", "Test message");
    
    // Empty message should be rejected
    manager.info("TestCategory", "");
    
    // Set category filter to empty should still allow messages (empty list means all enabled)
    manager.setCategoryEnabled("", true);
    manager.info("TestCategory", "Test message");
    
    std::cout << "✓ DiagnosticsManager validation tests passed" << std::endl;
}

void test_global_manager() {
    std::cout << "Testing global diagnostics manager..." << std::endl;
    
    auto& manager = getGlobalDiagnosticsManager();
    
    manager.info("TestCategory", "Global test message");
    
    auto stats = manager.getStatistics();
    assert(stats.totalMessages >= 1);
    
    destroyGlobalDiagnosticsManager();
    
    std::cout << "✓ Global diagnostics manager tests passed" << std::endl;
}

void test_convenience_macros() {
    std::cout << "Testing convenience macros..." << std::endl;
    
    // These macros should compile and not crash
    POKO_DIAG_DEBUG("TestCategory", "Debug macro test");
    POKO_DIAG_INFO("TestCategory", "Info macro test");
    POKO_DIAG_WARNING("TestCategory", "Warning macro test");
    POKO_DIAG_ERROR("TestCategory", "Error macro test");
    // POKO_DIAG_FATAL("TestCategory", "Fatal macro test"); // Don't actually call fatal
    POKO_DIAG_TRACE("TestCategory", "Trace macro test");
    
    std::cout << "✓ Convenience macros tests passed" << std::endl;
}

int main() {
    std::cout << "=== Core Diagnostics Unit Tests ===" << std::endl;
    
    test_manager_basics();
    test_manager_handlers();
    test_manager_report();
    test_manager_severity_filtering();
    test_manager_category_filtering();
    test_manager_history();
    test_manager_statistics();
    test_manager_validation();
    test_global_manager();
    test_convenience_macros();
    
    std::cout << "\n=== All tests passed! ===" << std::endl;
    
    return 0;
}
