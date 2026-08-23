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
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#elif __linux__
#include <fstream>
#elif __APPLE__
#include <mach/mach.h>
#include <mach/task_info.h>
#endif

namespace Poko {

Engine::Engine(const EngineConfig& config)
    : config_(config)
    , paused_(false)
    , time_scale_(1.0)
    , frame_count_(0)
    , average_fps_(0.0)
    , average_frame_time_ms_(0.0)
    , initial_memory_usage_(0)
{
    fps_samples_.reserve(config.max_fps_samples);
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

    // Initialize renderer
    renderer_ = std::make_unique<Rendering::BgfxRenderer>();
    Rendering::BgfxRenderer::RendererConfig renderer_config;
    renderer_config.width = config_.window_width;
    renderer_config.height = config_.window_height;
    renderer_config.vsync = config_.vsync;
    renderer_config.max_fps = static_cast<uint32_t>(config_.target_fps);
    
    if (!renderer_->Initialize(window_->get_native_handle(), renderer_config)) {
        last_error_ = "Renderer initialization failed";
        LOG_ERROR(last_error_);
        input_->reset();
        window_->shutdown();
        window_.reset();
        return false;
    }

    // Initialize render pipeline
    render_pipeline_ = std::make_unique<Rendering::RenderPipeline>();
    if (!render_pipeline_->Initialize(renderer_.get())) {
        last_error_ = "Render pipeline initialization failed";
        LOG_ERROR(last_error_);
        renderer_->Shutdown();
        renderer_.reset();
        input_->reset();
        window_->shutdown();
        window_.reset();
        return false;
    }

    // Initialize FPS samples
    fps_samples_.reserve(config_.max_fps_samples);
    for (int i = 0; i < config_.max_fps_samples; ++i) {
        fps_samples_.push_back(60.0); // Initialize with 60 FPS
    }

    // Track initial memory usage
    initial_memory_usage_ = get_current_memory_usage();

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

size_t Engine::get_current_memory_usage() const {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / (1024 * 1024); // Convert to MB
    }
#elif __linux__
    std::ifstream file("/proc/self/status");
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("VmRSS:") == 0) {
            size_t value = 0;
            sscanf(line.c_str(), "VmRSS: %zu kB", &value);
            return value / 1024; // Convert to MB
        }
    }
#elif __APPLE__
    struct task_basic_info info;
    mach_msg_type_number_t count = TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS) {
        return info.resident_size / (1024 * 1024); // Convert to MB
    }
#endif
    return 0;
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
        
        // Apply time scale
        double scaled_delta_time = delta_time_ * time_scale_;
        
        // Cap delta time to prevent spiral of death
        if (scaled_delta_time > config_.max_delta_time) {
            scaled_delta_time = config_.max_delta_time;
        }
        
        // Enforce minimum delta time
        if (scaled_delta_time < config_.min_delta_time) {
            scaled_delta_time = config_.min_delta_time;
        }
        
        // Skip update if paused
        if (!paused_) {
            update(scaled_delta_time);
        }
        
        render();
        
        // Call frame callback if registered
        if (frame_callback_) {
            frame_callback_(scaled_delta_time);
        }
        
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
        
        frame_count_++;
    }

    LOG_INFO("Game loop ended. Total frames: " + std::to_string(frame_count_));
}

void Engine::shutdown() {
    if (!initialized_) return;

    LOG_INFO("Shutting down engine...");

    // Shutdown subsystems in reverse order
    if (render_pipeline_) {
        render_pipeline_->Shutdown();
        render_pipeline_.reset();
    }

    if (renderer_) {
        renderer_->Shutdown();
        renderer_.reset();
    }

    if (input_) {
        input_->reset();
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
        
        // Handle window resize
        int new_width = window_->get_width();
        int new_height = window_->get_height();
        if (new_width != static_cast<int>(config_.window_width) || 
            new_height != static_cast<int>(config_.window_height)) {
            config_.window_width = new_width;
            config_.window_height = new_height;
            
            LOG_INFO("Window resized to: " + std::to_string(new_width) + "x" + std::to_string(new_height));
            
            if (renderer_) {
                renderer_->Resize(new_width, new_height);
            }
        }
    }

    // Update subsystems (to be implemented)
    // - Physics simulation
    // - Script execution
    // - Audio mixing
    // - Network updates
    
    // Logging frame info (every 60 frames to avoid spam)
    if (frame_count_ % 60 == 0 && config_.enable_logging) {
        LOG_DEBUG("Frame " + std::to_string(frame_count_) + 
                  " | FPS: " + std::to_string(fps_) + 
                  " | Delta: " + std::to_string(delta_time * 1000.0) + "ms");
    }
}

void Engine::render() {
    PROFILE_SCOPE("Engine::render");

    // Begin frame
    if (renderer_) {
        renderer_->BeginFrame();
    }

    // Execute render pipeline
    if (render_pipeline_) {
        render_pipeline_->Execute();
    }

    // End frame and present
    if (renderer_) {
        renderer_->EndFrame();
        
        // Update FPS for debug display
        renderer_->UpdateFPS(fps_);
    }

    // Present window (swap buffers)
    if (window_) {
        window_->present();
    }

    // Performance monitoring
    if (config_.enable_profiling && frame_count_ % 120 == 0) {
        // Log performance stats every 2 seconds at 60 FPS
        LOG_INFO("Performance Stats | FPS: " + std::to_string(average_fps_) + 
                " | Frame Time: " + std::to_string(average_frame_time_ms_) + "ms" +
                " | Memory: " + std::to_string(get_current_memory_usage()) + "MB");
    }
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
    
    // Update FPS samples for averaging
    fps_samples_.push_back(fps_);
    if (fps_samples_.size() > static_cast<size_t>(config_.max_fps_samples)) {
        fps_samples_.erase(fps_samples_.begin());
    }
    
    // Calculate average FPS
    double fps_sum = 0.0;
    for (double sample : fps_samples_) {
        fps_sum += sample;
    }
    average_fps_ = fps_sum / fps_samples_.size();
    average_frame_time_ms_ = (1.0 / average_fps_) * 1000.0;
}

double Engine::get_total_time() const {
    return total_time_;
}

std::string Engine::get_memory_stats() const {
    size_t current_memory = get_current_memory_usage();
    int64_t memory_delta = static_cast<int64_t>(current_memory) - static_cast<int64_t>(initial_memory_usage_);
    
    std::ostringstream stats;
    stats << "Memory Usage: " << current_memory << " MB\n";
    stats << "Memory Delta: " << (memory_delta >= 0 ? "+" : "") << memory_delta << " MB\n";
    stats << "Frame Count: " << frame_count_ << "\n";
    stats << "Average FPS: " << average_fps_ << "\n";
    stats << "Average Frame Time: " << average_frame_time_ms_ << " ms";
    
    return stats.str();
}

Engine::EngineStats Engine::get_stats() const {
    EngineStats stats;
    stats.fps = fps_;
    stats.average_fps = average_fps_;
    stats.delta_time = delta_time_;
    stats.total_time = total_time_;
    stats.frame_count = frame_count_;
    stats.cpu_usage_percent = average_frame_time_ms_ / (1000.0 / config_.target_fps) * 100.0;
    stats.memory_usage_mb = get_current_memory_usage();
    return stats;
}

} // namespace Poko