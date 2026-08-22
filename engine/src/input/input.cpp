/**
 * @file input.cpp
 * @brief Input system implementation
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-21
 */

#include "input/input.h"
#include <SDL2/SDL.h>

namespace Poko {

bool Input::is_key_down(KeyCode key) const {
    auto it = keys_down_.find(key);
    return it != keys_down_.end() && it->second;
}

bool Input::is_key_pressed(KeyCode key) const {
    auto it = keys_pressed_.find(key);
    return it != keys_pressed_.end() && it->second;
}

bool Input::is_key_released(KeyCode key) const {
    auto it = keys_released_.find(key);
    return it != keys_released_.end() && it->second;
}

bool Input::is_mouse_down(MouseButton button) const {
    auto it = mouse_down_.find(button);
    return it != mouse_down_.end() && it->second;
}

bool Input::is_mouse_pressed(MouseButton button) const {
    auto it = mouse_pressed_.find(button);
    return it != mouse_pressed_.end() && it->second;
}

bool Input::is_mouse_released(MouseButton button) const {
    auto it = mouse_released_.find(button);
    return it != mouse_released_.end() && it->second;
}

std::pair<int, int> Input::get_mouse_position() const {
    return {mouse_x_, mouse_y_};
}

std::pair<int, int> Input::get_mouse_delta() const {
    return {mouse_dx_, mouse_dy_};
}

std::pair<int, int> Input::get_mouse_scroll() const {
    return {mouse_scroll_x_, mouse_scroll_y_};
}

void Input::update() {
    // Reset frame-specific states
    keys_pressed_.clear();
    keys_released_.clear();
    mouse_pressed_.clear();
    mouse_released_.clear();
    
    // Reset mouse delta
    mouse_dx_ = 0;
    mouse_dy_ = 0;
    mouse_scroll_x_ = 0;
    mouse_scroll_y_ = 0;
    
    // Process SDL events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                if (!event.key.repeat) {
                    keys_pressed_[static_cast<KeyCode>(event.key.keysym.sym)] = true;
                    keys_down_[static_cast<KeyCode>(event.key.keysym.sym)] = true;
                }
                break;
            case SDL_KEYUP:
                keys_released_[static_cast<KeyCode>(event.key.keysym.sym)] = true;
                keys_down_[static_cast<KeyCode>(event.key.keysym.sym)] = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                mouse_pressed_[static_cast<MouseButton>(event.button.button)] = true;
                mouse_down_[static_cast<MouseButton>(event.button.button)] = true;
                break;
            case SDL_MOUSEBUTTONUP:
                mouse_released_[static_cast<MouseButton>(event.button.button)] = true;
                mouse_down_[static_cast<MouseButton>(event.button.button)] = false;
                break;
            case SDL_MOUSEMOTION:
                mouse_dx_ = event.motion.xrel;
                mouse_dy_ = event.motion.yrel;
                mouse_x_ = event.motion.x;
                mouse_y_ = event.motion.y;
                break;
            case SDL_MOUSEWHEEL:
                mouse_scroll_x_ = event.wheel.x;
                mouse_scroll_y_ = event.wheel.y;
                break;
            default:
                break;
        }
    }
}

void Input::reset() {
    // Clear all states
    keys_down_.clear();
    keys_pressed_.clear();
    keys_released_.clear();
    mouse_down_.clear();
    mouse_pressed_.clear();
    mouse_released_.clear();
    
    // Reset mouse position
    mouse_x_ = 0;
    mouse_y_ = 0;
    mouse_dx_ = 0;
    mouse_dy_ = 0;
    mouse_scroll_x_ = 0;
    mouse_scroll_y_ = 0;
}

} // namespace Poko