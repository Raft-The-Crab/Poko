/**
 * @file input.cpp
 * @brief Input system implementation for Poko Engine
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "input/input.h"
#include "core/logging/logger.h"
#include <cstring>
#include <map>
#include <mutex>
#include <vector>
#include <windows.h>
#include <windowsx.h>

// Windows message constants
#define WM_KEYDOWN 0x0100
#define WM_KEYUP 0x0101
#define WM_SYSKEYDOWN 0x0104
#define WM_SYSKEYUP 0x0105
#define WM_MOUSEMOVE 0x0200
#define WM_LBUTTONDOWN 0x0201
#define WM_LBUTTONUP 0x0202
#define WM_RBUTTONDOWN 0x0204
#define WM_RBUTTONUP 0x0205
#define WM_MBUTTONDOWN 0x0207
#define WM_MBUTTONUP 0x0208
#define WM_XBUTTONDOWN 0x020B
#define WM_XBUTTONUP 0x020C
#define WM_MOUSEWHEEL 0x020A
#define XBUTTON1 0x0001
#define XBUTTON2 0x0002

namespace poko {
namespace input {

// Input action state
struct InputAction {
    std::string name;
    int type;
    bool is_pressed;
    bool was_pressed;
    float value;
    std::vector<int> keys; // Key bindings
    std::vector<int> mouse_buttons; // Mouse button bindings
};

// Input system state
struct InputSystemImpl {
    std::map<std::string, InputAction> actions;
    std::map<int, bool> key_states;
    std::map<int, bool> mouse_button_states;
    int mouse_x;
    int mouse_y;
    int mouse_wheel_delta;
    bool mouse_captured;
    bool keyboard_captured;
    HWND window_handle;
    std::mutex mutex;
    uint64_t next_action_id;
};

// Convert engine action type to string
static const char* action_type_to_string(int type) {
    switch (type) {
        case 0: return "Button";
        case 1: return "Axis";
        case 2: return "Vector2";
        case 3: return "Vector3";
        default: return "Unknown";
    }
}

} // namespace input
} // namespace poko

// C API implementation
extern "C" {

using namespace poko::input;

InputSystem* input_system_create(void) {
    InputSystemImpl* impl = new InputSystemImpl();
    
    impl->mouse_x = 0;
    impl->mouse_y = 0;
    impl->mouse_wheel_delta = 0;
    impl->mouse_captured = false;
    impl->keyboard_captured = false;
    impl->window_handle = NULL;
    impl->next_action_id = 1;
    
    POKO_LOG_INFO("Input: Input system created");
    return reinterpret_cast<InputSystem*>(impl);
}

void input_system_destroy(InputSystem* system) {
    if (!system) return;
    
    InputSystemImpl* impl = reinterpret_cast<InputSystemImpl*>(system);
    delete impl;
    
    POKO_LOG_INFO("Input: Input system destroyed");
}

void input_system_update(InputSystem* system) {
    if (!system) return;
    
    InputSystemImpl* impl = reinterpret_cast<InputSystemImpl*>(system);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    // Update all actions
    for (auto& pair : impl->actions) {
        InputAction& action = pair.second;
        action.was_pressed = action.is_pressed;
        
        // Check key states
        for (int key : action.keys) {
            auto it = impl->key_states.find(key);
            if (it != impl->key_states.end() && it->second) {
                action.is_pressed = true;
                action.value = 1.0f;
                break;
            }
        }
        
        // Check mouse button states
        for (int button : action.mouse_buttons) {
            auto it = impl->mouse_button_states.find(button);
            if (it != impl->mouse_button_states.end() && it->second) {
                action.is_pressed = true;
                action.value = 1.0f;
                break;
            }
        }
        
        // Reset if no input detected
        if (action.keys.empty() && action.mouse_buttons.empty()) {
            action.is_pressed = false;
            action.value = 0.0f;
        }
    }
    
    // Reset mouse wheel delta
    impl->mouse_wheel_delta = 0;
}

void input_system_set_window_handle(InputSystem* system, void* handle) {
    if (!system) return;
    
    InputSystemImpl* impl = reinterpret_cast<InputSystemImpl*>(system);
    std::lock_guard<std::mutex> lock(impl->mutex);
    impl->window_handle = reinterpret_cast<HWND>(handle);
    
    POKO_LOG_DEBUG("Input: Window handle set");
}

bool input_register_action(InputSystem* system, const char* name, int type) {
    if (!system || !name) return false;
    
    InputSystemImpl* impl = reinterpret_cast<InputSystemImpl*>(system);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    InputAction action;
    action.name = name;
    action.type = type;
    action.is_pressed = false;
    action.was_pressed = false;
    action.value = 0.0f;
    
    impl->actions[name] = action;
    
    POKO_LOG_INFO("Input: Registered action '" + std::string(name) + "' with type " + action_type_to_string(type));
    return true;
}

bool input_bind_key(InputSystem* system, const char* action_name, int key) {
    if (!system || !action_name) return false;
    
    InputSystemImpl* impl = reinterpret_cast<InputSystemImpl*>(system);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    auto it = impl->actions.find(action_name);
    if (it != impl->actions.end()) {
        it->second.keys.push_back(key);
        POKO_LOG_DEBUG("Input: Bound key " + std::to_string(key) + " to action '" + std::string(action_name) + "'");
        return true;
    }
    
    return false;
}

bool input_bind_mouse_button(InputSystem* system, const char* action_name, int button) {
    if (!system || !action_name) return false;
    
    InputSystemImpl* impl = reinterpret_cast<InputSystemImpl*>(system);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    auto it = impl->actions.find(action_name);
    if (it != impl->actions.end()) {
        it->second.mouse_buttons.push_back(button);
        POKO_LOG_DEBUG("Input: Bound mouse button " + std::to_string(button) + " to action '" + std::string(action_name) + "'");
        return true;
    }
    
    return false;
}

bool input_is_action_pressed(InputSystem* system, const char* action_name) {
    if (!system || !action_name) return false;
    
    InputSystemImpl* impl = reinterpret_cast<InputSystemImpl*>(system);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    auto it = impl->actions.find(action_name);
    if (it != impl->actions.end()) {
        return it->second.is_pressed;
    }
    
    return false;
}

bool input_was_action_pressed(InputSystem* system, const char* action_name) {
    if (!system || !action_name) return false;
    
    InputSystemImpl* impl = reinterpret_cast<InputSystemImpl*>(system);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    auto it = impl->actions.find(action_name);
    if (it != impl->actions.end()) {
        return it->second.was_pressed && !it->second.is_pressed;
    }
    
    return false;
}

float input_get_action_value(InputSystem* system, const char* action_name) {
    if (!system || !action_name) return 0.0f;
    
    InputSystemImpl* impl = reinterpret_cast<InputSystemImpl*>(system);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    auto it = impl->actions.find(action_name);
    if (it != impl->actions.end()) {
        return it->second.value;
    }
    
    return 0.0f;
}

void input_get_mouse_position(InputSystem* system, int* x, int* y) {
    if (!system) return;
    
    InputSystemImpl* impl = reinterpret_cast<InputSystemImpl*>(system);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    if (x) *x = impl->mouse_x;
    if (y) *y = impl->mouse_y;
}

void input_get_mouse_delta(InputSystem* system, int* dx, int* dy) {
    if (!system) return;
    
    InputSystemImpl* impl = reinterpret_cast<InputSystemImpl*>(system);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    // Calculate delta (simplified - in real implementation would track previous position)
    if (dx) *dx = 0; // Would be calculated from movement
    if (dy) *dy = 0;
}

int input_get_mouse_wheel_delta(InputSystem* system) {
    if (!system) return 0;
    
    InputSystemImpl* impl = reinterpret_cast<InputSystemImpl*>(system);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    return impl->mouse_wheel_delta;
}

bool input_is_key_down(InputSystem* system, int key) {
    if (!system) return false;
    
    InputSystemImpl* impl = reinterpret_cast<InputSystemImpl*>(system);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    auto it = impl->key_states.find(key);
    if (it != impl->key_states.end()) {
        return it->second;
    }
    
    return false;
}

bool input_is_mouse_button_down(InputSystem* system, int button) {
    if (!system) return false;
    
    InputSystemImpl* impl = reinterpret_cast<InputSystemImpl*>(system);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    auto it = impl->mouse_button_states.find(button);
    if (it != impl->mouse_button_states.end()) {
        return it->second;
    }
    
    return false;
}

void input_capture_mouse(InputSystem* system, bool capture) {
    if (!system) return;
    
    InputSystemImpl* impl = reinterpret_cast<InputSystemImpl*>(system);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    if (capture && impl->window_handle) {
        SetCapture(impl->window_handle);
        impl->mouse_captured = true;
        POKO_LOG_DEBUG("Input: Mouse captured");
    } else {
        ReleaseCapture();
        impl->mouse_captured = false;
        POKO_LOG_DEBUG("Input: Mouse released");
    }
}

void input_capture_keyboard(InputSystem* system, bool capture) {
    if (!system) return;
    
    InputSystemImpl* impl = reinterpret_cast<InputSystemImpl*>(system);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    impl->keyboard_captured = capture;
    POKO_LOG_DEBUG("Input: Keyboard capture " + std::string(capture ? "enabled" : "disabled"));
}

// Windows message handler (should be called from window procedure)
void input_process_windows_message(InputSystem* system, unsigned int message, unsigned long long wparam, long long lparam) {
    if (!system) return;
    
    InputSystemImpl* impl = reinterpret_cast<InputSystemImpl*>(system);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    switch (message) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            impl->key_states[static_cast<int>(wparam)] = true;
            break;
            
        case WM_KEYUP:
        case WM_SYSKEYUP:
            impl->key_states[static_cast<int>(wparam)] = false;
            break;
            
        case WM_LBUTTONDOWN:
            impl->mouse_button_states[VK_LBUTTON] = true;
            break;
            
        case WM_LBUTTONUP:
            impl->mouse_button_states[VK_LBUTTON] = false;
            break;
            
        case WM_RBUTTONDOWN:
            impl->mouse_button_states[VK_RBUTTON] = true;
            break;
            
        case WM_RBUTTONUP:
            impl->mouse_button_states[VK_RBUTTON] = false;
            break;
            
        case WM_MBUTTONDOWN:
            impl->mouse_button_states[VK_MBUTTON] = true;
            break;
            
        case WM_MBUTTONUP:
            impl->mouse_button_states[VK_MBUTTON] = false;
            break;
            
        case WM_XBUTTONDOWN:
            if (GET_XBUTTON_WPARAM(wparam) == XBUTTON1) {
                impl->mouse_button_states[VK_XBUTTON1] = true;
            } else if (GET_XBUTTON_WPARAM(wparam) == XBUTTON2) {
                impl->mouse_button_states[VK_XBUTTON2] = true;
            }
            break;
            
        case WM_XBUTTONUP:
            if (GET_XBUTTON_WPARAM(wparam) == XBUTTON1) {
                impl->mouse_button_states[VK_XBUTTON1] = false;
            } else if (GET_XBUTTON_WPARAM(wparam) == XBUTTON2) {
                impl->mouse_button_states[VK_XBUTTON2] = false;
            }
            break;
            
        case WM_MOUSEMOVE:
            impl->mouse_x = GET_X_LPARAM(lparam);
            impl->mouse_y = GET_Y_LPARAM(lparam);
            break;
            
        case WM_MOUSEWHEEL:
            impl->mouse_wheel_delta = GET_WHEEL_DELTA_WPARAM(wparam);
            break;
    }
}

} // extern "C"