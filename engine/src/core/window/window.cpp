/**
 * @file window.cpp
 * @brief Window management implementation using SDL2
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-21
 */

#include "core/window/window.h"
#include "core/logging/logger.h"
#include <SDL2/SDL.h>
#include <iostream>

namespace Poko {

Window::Window(const Config& config)
    : config_(config)
{
}

Window::~Window() {
    shutdown();
}

bool Window::initialize() {
    LOG_INFO("Initializing SDL2 window...");

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        LOG_ERROR("SDL2 initialization failed: " + std::string(SDL_GetError()));
        return false;
    }

    // Create window flags (bgfx doesn't need OpenGL context)
    Uint32 window_flags = SDL_WINDOW_SHOWN;
    if (config_.fullscreen) {
        window_flags |= SDL_WINDOW_FULLSCREEN;
    }
    if (config_.resizable) {
        window_flags |= SDL_WINDOW_RESIZABLE;
    }

    // Create window
    sdl_window_ = SDL_CreateWindow(
        config_.title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config_.width,
        config_.height,
        window_flags
    );

    if (!sdl_window_) {
        LOG_ERROR("Window creation failed: " + std::string(SDL_GetError()));
        SDL_Quit();
        return false;
    }

    // Don't create OpenGL context - bgfx will handle rendering
    sdl_context_ = nullptr;

    initialized_ = true;
    should_close_ = false;

    LOG_INFO("Window initialized successfully: " + std::to_string(config_.width) + "x" + std::to_string(config_.height));
    return true;
}

void Window::update() {
    if (!initialized_) return;

    process_events();
}

void Window::process_events()
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                should_close_ = true;
                if (event_callback_) {
                    WindowEvent window_event;
                    window_event.type = WindowEventType::Close;
                    event_callback_(window_event);
                }
                break;

            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        config_.width = event.window.data1;
                        config_.height = event.window.data2;
                        LOG_DEBUG("Window resize event: " + std::to_string(config_.width) + "x" + std::to_string(config_.height));
                        if (event_callback_) {
                            WindowEvent window_event;
                            window_event.type = WindowEventType::Resize;
                            window_event.width = event.window.data1;
                            window_event.height = event.window.data2;
                            event_callback_(window_event);
                        }
                        break;

                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        if (event_callback_) {
                            WindowEvent window_event;
                            window_event.type = WindowEventType::FocusGained;
                            window_event.focused = true;
                            event_callback_(window_event);
                        }
                        break;

                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        if (event_callback_) {
                            WindowEvent window_event;
                            window_event.type = WindowEventType::FocusLost;
                            window_event.focused = false;
                            event_callback_(window_event);
                        }
                        break;

                    case SDL_WINDOWEVENT_MINIMIZED:
                        if (event_callback_) {
                            WindowEvent window_event;
                            window_event.type = WindowEventType::Minimize;
                            event_callback_(window_event);
                        }
                        break;

                    case SDL_WINDOWEVENT_MAXIMIZED:
                        if (event_callback_) {
                            WindowEvent window_event;
                            window_event.type = WindowEventType::Maximize;
                            event_callback_(window_event);
                        }
                        break;

                    case SDL_WINDOWEVENT_RESTORED:
                        if (event_callback_) {
                            WindowEvent window_event;
                            window_event.type = WindowEventType::Restore;
                            event_callback_(window_event);
                        }
                        break;
                }
                break;

            default:
                break;
        }
    }
}

void Window::set_event_callback(WindowEventCallback callback)
{
    event_callback_ = callback;
}

void Window::present() {
    if (!initialized_) return;

    // bgfx handles presentation, so this is just a placeholder
    // The actual buffer swap happens in bgfx_renderer::EndFrame()
}

void Window::shutdown() {
    if (!initialized_) return;

    if (sdl_window_) {
        SDL_DestroyWindow(static_cast<SDL_Window*>(sdl_window_));
        sdl_window_ = nullptr;
    }
    SDL_Quit();

    initialized_ = false;
}

void Window::set_title(const std::string& title) {
    if (!initialized_) return;

    config_.title = title.c_str();
    SDL_SetWindowTitle(static_cast<SDL_Window*>(sdl_window_), title.c_str());
}

} // namespace Poko