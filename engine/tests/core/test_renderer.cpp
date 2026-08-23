/**
 * @file test_renderer.cpp
 * @brief Tests for bgfx renderer
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-23
 */

#include "rendering/bgfx/bgfx_renderer.h"
#include "core/window/window.h"
#include "core/logging/logger.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace Poko;
using namespace Poko::Rendering;

int main() {
    std::cout << "=== Bgfx Renderer Test ===" << std::endl;

    // Create window
    Window::Config window_config;
    window_config.width = 1280;
    window_config.height = 720;
    window_config.title = "Bgfx Renderer Test";
    window_config.vsync = false;
    window_config.resizable = true;

    Window window(window_config);

    if (!window.initialize()) {
        std::cerr << "Failed to initialize window" << std::endl;
        return 1;
    }

    std::cout << "Window initialized successfully" << std::endl;

    // Create renderer
    BgfxRenderer renderer;

    BgfxRenderer::RendererConfig renderer_config;
    renderer_config.width = window_config.width;
    renderer_config.height = window_config.height;
    renderer_config.vsync = window_config.vsync;
    renderer_config.max_fps = 60;
    renderer_config.renderer_type = bgfx::RendererType::Count; // Auto-detect

    if (!renderer.Initialize(window.get_native_handle(), renderer_config)) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        window.shutdown();
        return 1;
    }

    std::cout << "Renderer initialized successfully" << std::endl;
    std::cout << "Renderer Type: " << renderer.GetRendererType() << std::endl;
    std::cout << "GPU Name: " << renderer.GetGPUName() << std::endl;

    // Enable debug text
    renderer.SetDebugText(true);

    // Test clear colors
    auto resolution = renderer.GetResolution();
    std::cout << "Resolution: " << resolution.first << "x" << resolution.second << std::endl;

    // Run a simple render loop for a few seconds
    std::cout << "Running render loop for 3 seconds..." << std::endl;

    auto start_time = std::chrono::steady_clock::now();
    int frame_count = 0;

    while (true) {
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();

        if (elapsed >= 3) {
            break;
        }

        // Update window
        window.update();
        if (window.should_close()) {
            break;
        }

        // Clear with different colors based on frame count
        float r = (float)(frame_count % 255) / 255.0f;
        float g = (float)((frame_count * 2) % 255) / 255.0f;
        float b = (float)((frame_count * 3) % 255) / 255.0f;
        renderer.Clear(r, g, b, 1.0f);

        // Begin frame
        renderer.BeginFrame();

        // Render (simple clear pass)
        renderer.EndFrame();

        frame_count++;
    }

    std::cout << "Rendered " << frame_count << " frames" << std::endl;
    std::cout << "Average FPS: " << (frame_count / 3.0) << std::endl;

    // Get stats
    std::cout << "\n=== Renderer Statistics ===" << std::endl;
    std::cout << renderer.GetStats() << std::endl;

    // Test resize
    std::cout << "\nTesting resize to 800x600..." << std::endl;
    renderer.Resize(800, 600);
    auto new_resolution = renderer.GetResolution();
    std::cout << "New resolution: " << new_resolution.first << "x" << new_resolution.second << std::endl;

    // Shutdown
    std::cout << "\nShutting down renderer..." << std::endl;
    renderer.Shutdown();

    std::cout << "Shutting down window..." << std::endl;
    window.shutdown();

    std::cout << "=== Test Complete ===" << std::endl;
    return 0;
}