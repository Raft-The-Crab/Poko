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
#include <vector>
#include <functional>
#include "core/window/window.h"
#include "input/input.h"
#include "rendering/bgfx/bgfx_renderer.h"
#include "rendering/pipelines/render_pipeline.h"

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
    bool enable_profiling = true;
    bool enable_logging = true;
    int max_fps_samples = 60; // For FPS averaging
    double min_delta_time = 0.001; // 1ms minimum
    double max_delta_time = 0.1; // 100ms maximum (spiral of death prevention)
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
     * @brief Get renderer
     * @return Pointer to bgfx renderer
     */
    Rendering::BgfxRenderer* get_renderer() const { return renderer_.get(); }

    /**
     * @brief Get render pipeline
     * @return Pointer to render pipeline
     */
    Rendering::RenderPipeline* get_render_pipeline() const { return render_pipeline_.get(); }

    /**
     * @brief Get scripting engine
     * @return Pointer to scripting engine
     */
    // MuteEngine* get_scripting() const { return scripting_.get(); }

    /**
     * @brief Get average FPS over recent frames
     * @return Average FPS
     */
    double get_average_fps() const { return average_fps_; }

    /**
     * @brief Get frame time average (milliseconds)
     * @return Average frame time in ms
     */
    double get_average_frame_time_ms() const { return average_frame_time_ms_; }

    /**
     * @brief Get memory usage statistics
     * @return Memory usage string
     */
    std::string get_memory_stats() const;

    /**
     * @brief Get current memory usage in MB
     * @return Memory usage in MB
     */
    size_t get_current_memory_usage() const;

    /**
     * @brief Pause/unpause engine
     * @param paused Whether to pause
     */
    void set_paused(bool paused) { paused_ = paused; }

    /**
     * @brief Check if engine is paused
     * @return true if paused
     */
    bool is_paused() const { return paused_; }

    /**
     * @brief Set time scale (slow motion, fast forward)
     * @param scale Time scale multiplier (1.0 = normal, 0.5 = half speed, 2.0 = double speed)
     */
    void set_time_scale(double scale) { time_scale_ = scale; }

    /**
     * @brief Get current time scale
     * @return Time scale multiplier
     */
    double get_time_scale() const { return time_scale_; }

    /**
     * @brief Register frame callback
     * @param callback Function to call each frame
     */
    void set_frame_callback(std::function<void(double)> callback) { frame_callback_ = callback; }

    /**
     * @brief Engine state statistics
     */
    struct EngineStats {
        double fps;
        double average_fps;
        double delta_time;
        double total_time;
        uint64_t frame_count;
        double cpu_usage_percent;
        size_t memory_usage_mb;
    };

    /**
     * @brief Get engine statistics
     * @return Current engine statistics
     */
    EngineStats get_stats() const;

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
    std::unique_ptr<Rendering::BgfxRenderer> renderer_;
    std::unique_ptr<Rendering::RenderPipeline> render_pipeline_;
    // std::unique_ptr<MuteEngine> scripting_;

    // Engine state
    bool paused_ = false;
    double time_scale_ = 1.0;
    uint64_t frame_count_ = 0;

    // FPS averaging
    double average_fps_ = 0.0;
    double average_frame_time_ms_ = 0.0;
    std::vector<double> fps_samples_;

    // Frame callback
    std::function<void(double)> frame_callback_;

    // Memory tracking
    size_t initial_memory_usage_ = 0;
};

} // namespace poko

#endif // POKO_ENGINE_ENGINE_H