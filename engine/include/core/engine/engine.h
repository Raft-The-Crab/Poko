/**
 * @file engine.h
 * @brief Core engine interface and main loop
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-21
 */

#ifndef POKO_ENGINE_ENGINE_H
#define POKO_ENGINE_ENGINE_H

#include <memory>
#include <chrono>
#include <string>
#include "core/window/window.h"
#include "input/input.h"

namespace Poko {

// Forward declarations
// class MuteEngine;

/**
 * @brief Engine configuration
 */
struct EngineConfig {
    int window_width = 1280;
    int window_height = 720;
    const char* window_title = "Poko Engine";
    bool vsync = true;
    double target_fps = 60.0;
    double frame_budget_ms = 1.5;
};

/**
 * @brief Main engine class
 * 
 * Manages the game loop, subsystems, and core engine functionality.
 */
class Engine {
public:
    /**
     * @brief Construct engine with configuration
     * @param config Engine configuration
     */
    explicit Engine(const EngineConfig& config = EngineConfig());

    /**
     * @brief Destructor
     */
    ~Engine();

    /**
     * @brief Initialize engine and all subsystems
     * @return true if successful, false otherwise
     */
    bool initialize();

    /**
     * @brief Run the main game loop
     */
    void run();

    /**
     * @brief Shutdown engine and cleanup
     */
    void shutdown();

    /**
     * @brief Check if engine is running
     * @return true if running, false otherwise
     */
    bool is_running() const { return running_; }

    /**
     * @brief Check if engine has errors
     * @return true if there are errors
     */
    bool has_errors() const { return !last_error_.empty(); }

    /**
     * @brief Get last error message
     * @return Error message
     */
    const std::string& get_last_error() const { return last_error_; }

    /**
     * @brief Clear error state
     */
    void clear_error() { last_error_.clear(); }

    /**
     * @brief Get delta time (time since last frame)
     * @return Delta time in seconds
     */
    double get_delta_time() const { return delta_time_; }

    /**
     * @brief Get current FPS
     * @return Current frames per second
     */
    double get_fps() const { return fps_; }
    
    /**
     * @brief Get frame time (seconds per frame)
     * @return Frame time in seconds
     */
    double get_frame_time() const { return delta_time_; }
    
    /**
     * @brief Get total running time
     * @return Total time in seconds since engine started
     */
    double get_total_time() const;
    
    /**
     * @brief Set engine to exit
     */
    void set_should_exit() { running_ = false; }
    
    /**
     * @brief Get input system
     * @return Pointer to input system
     */
    Input* get_input() const { return input_.get(); }

    /**
     * @brief Get window
     * @return Pointer to window
     */
    Window* get_window() const { return window_.get(); }

    /**
     * @brief Get scripting engine
     * @return Pointer to scripting engine
     */
    // MuteEngine* get_scripting() const { return scripting_.get(); }

private:
    /**
     * @brief Update engine subsystems
     * @param delta_time Time since last update
     */
    void update(double delta_time);

    /**
     * @brief Render a frame
     */
    void render();

    /**
     * @brief Calculate frame timing
     */
    void calculate_frame_timing();

    EngineConfig config_;
    bool running_ = false;
    bool initialized_ = false;
    
    // Frame timing
    double delta_time_ = 0.0;
    double fps_ = 0.0;
    std::chrono::high_resolution_clock::time_point last_frame_time_;
    std::chrono::high_resolution_clock::time_point start_time_;
    double total_time_ = 0.0;

    // Error handling
    std::string last_error_;

    // Subsystems
    std::unique_ptr<Window> window_;
    std::unique_ptr<Input> input_;
    // std::unique_ptr<MuteEngine> scripting_;
};

} // namespace poko

#endif // POKO_ENGINE_ENGINE_H