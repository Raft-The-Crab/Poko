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

// Constructor
Input::Input() {
    // Initialize gamepad states
    for (auto& gamepad : gamepads_) {
        gamepad.connected = false;
    }
    text_input_.clear();
    touch_points_.clear();
    touch_available_ = false;
    mouse_locked_ = false;
    mouse_x_ = 0;
    mouse_y_ = 0;
    mouse_dx_ = 0;
    mouse_dy_ = 0;
    mouse_scroll_x_ = 0;
    mouse_scroll_y_ = 0;
}

Input::~Input() {
    // Clean up any SDL gamepad resources
    for (int i = 0; i < MAX_GAMEPADS; ++i) {
        if (gamepads_[i].connected) {
            SDL_GameController* controller = SDL_GameControllerFromInstanceID(i);
            if (controller) {
                SDL_GameControllerClose(controller);
            }
        }
    }
}

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
    text_input_.clear();
    
    // Reset gamepad frame-specific states
    for (auto& gamepad : gamepads_) {
        gamepad.buttons_pressed.clear();
        gamepad.buttons_released.clear();
    }
    
    // Reset mouse delta
    mouse_dx_ = 0;
    mouse_dy_ = 0;
    mouse_scroll_x_ = 0;
    mouse_scroll_y_ = 0;
    
    // Reset touch input
    touch_points_.clear();
    
    // Process SDL events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
                if (!event.key.repeat) {
                    keys_pressed_[static_cast<KeyCode>(event.key.keysym.sym)] = true;
                    keys_down_[static_cast<KeyCode>(event.key.keysym.sym)] = true;
                    
                    // Add character to text input (for printable characters)
                    if (event.key.keysym.sym >= 32 && event.key.keysym.sym <= 126) {
                        text_input_ += static_cast<char>(event.key.keysym.sym);
                    }
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
            case SDL_CONTROLLERDEVICEADDED:
                if (event.cdevice.which < MAX_GAMEPADS) {
                    gamepads_[event.cdevice.which].connected = true;
                    SDL_GameControllerOpen(event.cdevice.which);
                }
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                if (event.cdevice.which < MAX_GAMEPADS) {
                    gamepads_[event.cdevice.which].connected = false;
                    gamepads_[event.cdevice.which].buttons_down.clear();
                    gamepads_[event.cdevice.which].axes.clear();
                }
                break;
            case SDL_CONTROLLERBUTTONDOWN:
                if (event.cbutton.which < MAX_GAMEPADS) {
                    auto& gamepad = gamepads_[event.cbutton.which];
                    if (gamepad.connected) {
                        GamepadButton button = static_cast<GamepadButton>(event.cbutton.button);
                        gamepad.buttons_pressed[button] = true;
                        gamepad.buttons_down[button] = true;
                    }
                }
                break;
            case SDL_CONTROLLERBUTTONUP:
                if (event.cbutton.which < MAX_GAMEPADS) {
                    auto& gamepad = gamepads_[event.cbutton.which];
                    if (gamepad.connected) {
                        GamepadButton button = static_cast<GamepadButton>(event.cbutton.button);
                        gamepad.buttons_released[button] = true;
                        gamepad.buttons_down[button] = false;
                    }
                }
                break;
            case SDL_CONTROLLERAXISMOTION:
                if (event.caxis.which < MAX_GAMEPADS) {
                    auto& gamepad = gamepads_[event.caxis.which];
                    if (gamepad.connected) {
                        GamepadAxis axis = static_cast<GamepadAxis>(event.caxis.axis);
                        // Normalize axis to -1.0 to 1.0 range
                        float value = event.caxis.value / 32767.0f;
                        gamepad.axes[axis] = value;
                    }
                }
                break;
            case SDL_FINGERDOWN:
            case SDL_FINGERUP:
            case SDL_FINGERMOTION:
                touch_available_ = true;
                // Process touch events
                TouchPoint touch;
                touch.id = event.tfinger.fingerId;
                touch.x = static_cast<int>(event.tfinger.x);
                touch.y = static_cast<int>(event.tfinger.y);
                touch.pressed = (event.type == SDL_FINGERDOWN);
                touch.released = (event.type == SDL_FINGERUP);
                touch_points_.push_back(touch);
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
    mouse_locked_ = false;

    // Reset gamepad states
    for (auto& gamepad : gamepads_) {
        gamepad.connected = false;
        gamepad.buttons_down.clear();
        gamepad.buttons_pressed.clear();
        gamepad.buttons_released.clear();
        gamepad.axes.clear();
    }

    // Reset text input
    text_input_.clear();

    // Reset touch input
    touch_points_.clear();
    touch_available_ = false;
}

// Gamepad functions
bool Input::is_gamepad_button_down(int gamepad_index, GamepadButton button) const {
    if (gamepad_index < 0 || gamepad_index >= MAX_GAMEPADS) return false;
    const auto& gamepad = gamepads_[gamepad_index];
    if (!gamepad.connected) return false;
    
    auto it = gamepad.buttons_down.find(button);
    return it != gamepad.buttons_down.end() && it->second;
}

bool Input::is_gamepad_button_pressed(int gamepad_index, GamepadButton button) const {
    if (gamepad_index < 0 || gamepad_index >= MAX_GAMEPADS) return false;
    const auto& gamepad = gamepads_[gamepad_index];
    if (!gamepad.connected) return false;
    
    auto it = gamepad.buttons_pressed.find(button);
    return it != gamepad.buttons_pressed.end() && it->second;
}

bool Input::is_gamepad_button_released(int gamepad_index, GamepadButton button) const {
    if (gamepad_index < 0 || gamepad_index >= MAX_GAMEPADS) return false;
    const auto& gamepad = gamepads_[gamepad_index];
    if (!gamepad.connected) return false;
    
    auto it = gamepad.buttons_released.find(button);
    return it != gamepad.buttons_released.end() && it->second;
}

float Input::get_gamepad_axis(int gamepad_index, GamepadAxis axis) const {
    if (gamepad_index < 0 || gamepad_index >= MAX_GAMEPADS) return 0.0f;
    const auto& gamepad = gamepads_[gamepad_index];
    if (!gamepad.connected) return 0.0f;
    
    auto it = gamepad.axes.find(axis);
    return (it != gamepad.axes.end()) ? it->second : 0.0f;
}

bool Input::is_gamepad_connected(int gamepad_index) const {
    if (gamepad_index < 0 || gamepad_index >= MAX_GAMEPADS) return false;
    return gamepads_[gamepad_index].connected;
}

// Enhanced input functions
std::string Input::get_text_input() const {
    return text_input_;
}

bool Input::is_any_key_down() const {
    return !keys_down_.empty();
}

bool Input::is_any_mouse_down() const {
    return !mouse_down_.empty();
}

void Input::set_mouse_position(int x, int y) {
    mouse_x_ = x;
    mouse_y_ = y;
    mouse_dx_ = 0;
    mouse_dy_ = 0;
}

void Input::set_mouse_locked(bool locked) {
    mouse_locked_ = locked;
    if (locked) {
        SDL_SetRelativeMouseMode(SDL_TRUE);
    } else {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }
}

bool Input::is_mouse_locked() const {
    return mouse_locked_;
}

// Touch input functions
std::vector<Input::TouchPoint> Input::get_touch_points() const {
    return touch_points_;
}

bool Input::is_touch_available() const {
    return touch_available_;
}

} // namespace Poko