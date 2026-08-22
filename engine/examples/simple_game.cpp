/**
 * @file simple_game.cpp
 * @brief Simple example game demonstrating the Poko Engine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-21
 */

#include "core/engine/engine.h"
#include "input/input.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::cout << "=== Poko Engine Simple Game Example ===" << std::endl;
    
    // Configure engine
    EngineConfig config;
    config.window_width = 1280;
    config.window_height = 720;
    config.window_title = "Poko Engine - Simple Game";
    config.vsync = true;
    config.target_fps = 60.0;
    config.frame_budget_ms = 1.5;
    
    // Create engine
    Engine engine(config);
    
    // Initialize engine
    if (!engine.initialize()) {
        std::cerr << "Failed to initialize engine!" << std::endl;
        return 1;
    }
    
    std::cout << "Engine initialized successfully!" << std::endl;
    std::cout << "Running simple game..." << std::endl;
    std::cout << "Press ESC to exit" << std::endl;
    
    // Run the engine
    engine.run();
    
    std::cout << "=== Simple Game Complete ===" << std::endl;
    return 0;
}