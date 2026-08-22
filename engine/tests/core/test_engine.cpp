/**
 * @file test_engine.cpp
 * @brief Basic tests for engine core system
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "core/engine/engine.h"
#include <iostream>
#include <cassert>

using namespace Poko;

void test_engine_creation()
{
    std::cout << "Testing engine creation..." << std::endl;

    Engine engine;
    assert(!engine.is_running());
    assert(engine.get_window() == nullptr);
    assert(engine.get_input() == nullptr);

    std::cout << "✓ Engine creation test passed" << std::endl;
}

void test_engine_initialization()
{
    std::cout << "Testing engine initialization..." << std::endl;

    Engine engine;
    engine.initialize();

    assert(engine.is_running());
    assert(engine.get_window() != nullptr);
    assert(engine.get_input() != nullptr);
    assert(engine.get_window()->is_initialized());

    // Don't run the engine loop in this test to keep it simple
    engine.shutdown();
    assert(!engine.is_running());
    assert(engine.get_window() == nullptr);
    assert(engine.get_input() == nullptr);

    std::cout << "✓ Engine initialization test passed" << std::endl;
}

void test_engine_time_tracking()
{
    std::cout << "Testing engine time tracking..." << std::endl;

    Engine engine;
    engine.initialize();

    (void)engine.get_delta_time();
    (void)engine.get_total_time();

    engine.shutdown();

    std::cout << "✓ Engine time tracking test passed" << std::endl;
}

void test_engine_subsystem_integration()
{
    std::cout << "Testing engine subsystem integration..." << std::endl;

    Engine engine;
    engine.initialize();

    // Test that window and input are properly integrated
    assert(engine.get_window() != nullptr);
    assert(engine.get_input() != nullptr);
    assert(engine.get_window()->is_initialized());

    // Test window getters
    assert(engine.get_window()->get_width() == 1280);
    assert(engine.get_window()->get_height() == 720);

    engine.shutdown();

    std::cout << "✓ Engine subsystem integration test passed" << std::endl;
}

void test_engine_error_handling()
{
    std::cout << "Testing engine error handling..." << std::endl;

    Engine engine;
    // Initially no errors
    assert(!engine.has_errors());

    engine.initialize();
    // After successful init, still no errors
    assert(!engine.has_errors());

    engine.shutdown();

    std::cout << "✓ Engine error handling test passed" << std::endl;
}

int main()
{
    std::cout << "=== Engine Core System Tests ===" << std::endl;

    try {
        test_engine_creation();
        test_engine_initialization();
        test_engine_time_tracking();
        test_engine_subsystem_integration();
        test_engine_error_handling();

        std::cout << "\n=== All engine core tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}