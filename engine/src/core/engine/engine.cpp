/**
 * @file engine.cpp
 * @brief Core engine implementation
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-21
 */

#include "core/engine/engine.h"
#include "core/window/window.h"
#include "core/logging/logger.h"
#include "core/profiling/profiler.h"
// #include "scripting/mute_engine.h"
// #include "scripting/engine_bindings.h"
#include <SDL2/SDL.h>
#include <thread>

namespace Poko {

Engine::Engine(const EngineConfig& config)
    : config_(config)
{
}

Engine::~Engine() {
    shutdown();
}

bool Engine::initialize() {
    LOG_INFO("Initializing Poko Engine...");
    LOG_INFO("Window: " + std::to_string(config_.window_width) + "x" + std::to_string(config_.window_height));
    LOG_INFO("Title: " + std::string(config_.window_title));

    // Initialize window
    Window::Config window_config;
    window_config.width = config_.window_width;
    window_config.height = config_.window_height;
    window_config.title = config_.window_title;
    window_config.vsync = config_.vsync;

    window_ = std::make_unique<Window>(window_config);
    if (!window_->initialize()) {
        last_error_ = "Window initialization failed";
        LOG_ERROR(last_error_);
        return false;
    }

    // Set up window event callback
    window_->set_event_callback([this](const WindowEvent& event) {
        if (event.type == WindowEventType::Close) {
            running_ = false;
        }
    });

    // Initialize input
    input_ = std::make_unique<Input>();
    if (!input_) {
        last_error_ = "Input system creation failed";
        LOG_ERROR(last_error_);
        window_->shutdown();
        window_.reset();
        return false;
    }

    // Initialize scripting (commented out until Mute is properly integrated)
    // MuteEngine::Config script_config;
    // script_config.frame_budget_ms = config_.frame_budget_ms;
    // scripting_ = std::make_unique<MuteEngine>(script_config);
    // if (!scripting_->initialize()) {
    //     last_error_ = "Scripting initialization failed";
    //     LOG_ERROR(last_error_);
    //     input_->reset();
    //     input_.reset();
    //     window_->shutdown();
    //     window_.reset();
    //     return false;
    // }
    // scripting_->register_engine_bindings(this);

    // Initialize subsystems (to be implemented)
    // - Rendering (bgfx)
    // - Audio (OpenAL)
    // - Physics (Jolt)

    initialized_ = true;
    running_ = true;
    last_frame_time_ = std::chrono::high_resolution_clock::now();
    start_time_ = last_frame_time_;

    LOG_INFO("Engine initialized successfully!");
    return true;
}

void Engine::run() {
    if (!initialized_) {
        LOG_ERROR("Engine not initialized!");
        return;
    }

    LOG_INFO("Starting game loop...");
    
    const double target_frame_time = config_.target_fps > 0 ? 1.0 / config_.target_fps : 0.0;
    
    while (running_) {
        calculate_frame_timing();
        
        // Cap delta time to prevent spiral of death
        if (delta_time_ > 0.1) {
            delta_time_ = 0.1;
        }
        
        update(delta_time_);
        render();
        
        // Frame rate limiting
        if (target_frame_time > 0.0) {
            double frame_time = delta_time_;
            if (frame_time < target_frame_time) {
                double sleep_time = (target_frame_time - frame_time) * 1000.0;
                if (sleep_time > 1.0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(sleep_time)));
                }
            }
        }
    }

    LOG_INFO("Game loop ended.");
}

void Engine::shutdown() {
    if (!initialized_) return;

    LOG_INFO("Shutting down engine...");

    // Shutdown subsystems in reverse order
    // if (scripting_) {
    //     scripting_->shutdown();
    //     scripting_.reset();
    // }

    if (input_) {
        input_->reset();
        input_.reset();
    }

    if (window_) {
        window_->shutdown();
        window_.reset();
    }

    running_ = false;
    initialized_ = false;

    LOG_INFO("Engine shutdown complete.");
}

void Engine::update(double delta_time) {
    PROFILE_SCOPE("Engine::update");

    // Update input
    if (input_) {
        input_->update();
    }

    // Update window (handle events)
    if (window_) {
        window_->update();
        if (window_->should_close()) {
            running_ = false;
        }
    }

    // Update subsystems (to be implemented)
    // - Physics simulation
    // - Script execution
    // - Audio mixing
    // - Network updates
    (void)delta_time; // Suppress unused parameter warning
}

void Engine::render() {
    PROFILE_SCOPE("Engine::render");

    // Present window (swap buffers)
    if (window_) {
        window_->present();
    }

    // Render frame (to be implemented)
    // - Clear buffers
    // - Render scene
}

void Engine::calculate_frame_timing() {
    auto current_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = current_time - last_frame_time_;
    delta_time_ = elapsed.count();
    last_frame_time_ = current_time;
    
    // Calculate total time
    std::chrono::duration<double> total_elapsed = current_time - start_time_;
    total_time_ = total_elapsed.count();
    
    // Calculate FPS
    if (delta_time_ > 0.0) {
        fps_ = 1.0 / delta_time_;
    }
}

double Engine::get_total_time() const {
    return total_time_;
}

} // namespace Poko