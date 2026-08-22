/**
 * @file test_window_events.cpp
 * @brief Window event system tests
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "core/window/window.h"
#include <iostream>
#include <cassert>

using namespace Poko;

void test_window_event_callback()
{
    std::cout << "Testing window event callback..." << std::endl;

    Window::Config config;
    config.width = 800;
    config.height = 600;
    config.title = "Event Test Window";

    Window window(config);
    window.initialize();

    int event_count = 0;
    window.set_event_callback([&event_count](const WindowEvent& event) {
        event_count++;
        std::cout << "Window event received: " << static_cast<int>(event.type) << std::endl;
    });

    // Simulate some window events
    window.update();

    window.shutdown();

    std::cout << "✓ Window event callback test passed" << std::endl;
}

void test_window_event_types()
{
    std::cout << "Testing window event types..." << std::endl;

    Window::Config config;
    config.width = 1024;
    config.height = 768;
    config.title = "Event Types Test";

    Window window(config);
    window.initialize();

    bool resize_received = false;
    bool close_received = false;

    window.set_event_callback([&resize_received, &close_received](const WindowEvent& event) {
        if (event.type == WindowEventType::Resize) {
            resize_received = true;
        }
        if (event.type == WindowEventType::Close) {
            close_received = true;
        }
    });

    window.update();

    window.shutdown();

    std::cout << "✓ Window event types test passed" << std::endl;
}

int main()
{
    std::cout << "=== Window Event System Tests ===" << std::endl;

    try {
        test_window_event_callback();
        test_window_event_types();

        std::cout << "\n=== All window event tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}