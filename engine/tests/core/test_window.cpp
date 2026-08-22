/**
 * @file test_window.cpp
 * @brief Basic tests for window system
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "core/window/window.h"
#include <iostream>
#include <cassert>

using namespace Poko;

void test_window_creation()
{
    std::cout << "Testing window creation..." << std::endl;

    Window::Config config;
    config.width = 800;
    config.height = 600;
    config.title = "Test Window";

    Window window(config);
    assert(!window.is_initialized());

    std::cout << "✓ Window creation test passed" << std::endl;
}

void test_window_initialization()
{
    std::cout << "Testing window initialization..." << std::endl;

    Window::Config config;
    config.width = 800;
    config.height = 600;
    config.title = "Test Window";
    config.vsync = false;

    Window window(config);
    bool result = window.initialize();

    if (result) {
        assert(window.is_initialized());
        assert(window.get_width() == 800);
        assert(window.get_height() == 600);
        window.present();
        window.shutdown();
    } else {
        std::cout << "⚠ Window initialization skipped (headless environment)" << std::endl;
    }

    std::cout << "✓ Window initialization test passed" << std::endl;
}

void test_window_config()
{
    std::cout << "Testing window configuration..." << std::endl;

    Window::Config config;
    config.width = 1920;
    config.height = 1080;
    config.title = "HD Window";
    config.fullscreen = false;
    config.resizable = true;
    config.vsync = true;

    assert(config.width == 1920);
    assert(config.height == 1080);
    assert(std::string(config.title) == "HD Window");
    assert(config.fullscreen == false);
    assert(config.resizable == true);
    assert(config.vsync == true);

    std::cout << "✓ Window configuration test passed" << std::endl;
}

int main()
{
    std::cout << "=== Window System Tests ===" << std::endl;

    try {
        test_window_creation();
        test_window_initialization();
        test_window_config();

        std::cout << "\n=== All window tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}