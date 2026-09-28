/**
 * @file input.h
 * @brief Input system interface for Poko Engine
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#ifndef POKO_INPUT_H
#define POKO_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

// Input system handle (opaque)
typedef struct InputSystem InputSystem;

// Input action types
#define INPUT_ACTION_BUTTON 0
#define INPUT_ACTION_AXIS 1
#define INPUT_ACTION_VECTOR2 2
#define INPUT_ACTION_VECTOR3 3

// Windows virtual key codes (subset)
#define VK_LBUTTON 0x01
#define VK_RBUTTON 0x02
#define VK_MBUTTON 0x04
#define VK_XBUTTON1 0x05
#define VK_XBUTTON2 0x06
#define VK_BACK 0x08
#define VK_TAB 0x09
#define VK_RETURN 0x0D
#define VK_SHIFT 0x10
#define VK_CONTROL 0x11
#define VK_MENU 0x12
#define VK_ESCAPE 0x1B
#define VK_SPACE 0x20
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
#define VK_INSERT 0x2D
#define VK_DELETE 0x2E
#define VK_F1 0x70
#define VK_F2 0x71
#define VK_F3 0x72
#define VK_F4 0x73
#define VK_F5 0x74
#define VK_F6 0x75
#define VK_F7 0x76
#define VK_F8 0x77
#define VK_F9 0x78
#define VK_F10 0x79
#define VK_F11 0x7A
#define VK_F12 0x7B

/**
 * Create the input system
 * @return Input system handle, or NULL on failure
 */
InputSystem* input_system_create(void);

/**
 * Destroy the input system
 * @param system Input system handle
 */
void input_system_destroy(InputSystem* system);

/**
 * Update the input system (call once per frame)
 * @param system Input system handle
 */
void input_system_update(InputSystem* system);

/**
 * Set the window handle for input processing
 * @param system Input system handle
 * @param handle Window handle (HWND on Windows)
 */
void input_system_set_window_handle(InputSystem* system, void* handle);

/**
 * Register an input action
 * @param system Input system handle
 * @param name Action name
 * @param type Action type (button, axis, vector2, vector3)
 * @return true on success, false on failure
 */
bool input_register_action(InputSystem* system, const char* name, int type);

/**
 * Bind a key to an action
 * @param system Input system handle
 * @param action_name Action name
 * @param key Virtual key code
 * @return true on success, false on failure
 */
bool input_bind_key(InputSystem* system, const char* action_name, int key);

/**
 * Bind a mouse button to an action
 * @param system Input system handle
 * @param action_name Action name
 * @param button Mouse button (VK_LBUTTON, VK_RBUTTON, etc.)
 * @return true on success, false on failure
 */
bool input_bind_mouse_button(InputSystem* system, const char* action_name, int button);

/**
 * Check if an action is currently pressed
 * @param system Input system handle
 * @param action_name Action name
 * @return true if pressed, false otherwise
 */
bool input_is_action_pressed(InputSystem* system, const char* action_name);

/**
 * Check if an action was just pressed this frame
 * @param system Input system handle
 * @param action_name Action name
 * @return true if just pressed, false otherwise
 */
bool input_was_action_pressed(InputSystem* system, const char* action_name);

/**
 * Get the value of an action (for axis actions)
 * @param system Input system handle
 * @param action_name Action name
 * @return Action value (0.0 to 1.0 for buttons, -1.0 to 1.0 for axes)
 */
float input_get_action_value(InputSystem* system, const char* action_name);

/**
 * Get the current mouse position
 * @param system Input system handle
 * @param x Output X position
 * @param y Output Y position
 */
void input_get_mouse_position(InputSystem* system, int* x, int* y);

/**
 * Get the mouse movement delta since last frame
 * @param system Input system handle
 * @param dx Output X delta
 * @param dy Output Y delta
 */
void input_get_mouse_delta(InputSystem* system, int* dx, int* dy);

/**
 * Get the mouse wheel delta
 * @param system Input system handle
 * @return Mouse wheel delta
 */
int input_get_mouse_wheel_delta(InputSystem* system);

/**
 * Check if a key is currently down
 * @param system Input system handle
 * @param key Virtual key code
 * @return true if down, false otherwise
 */
bool input_is_key_down(InputSystem* system, int key);

/**
 * Check if a mouse button is currently down
 * @param system Input system handle
 * @param button Mouse button
 * @return true if down, false otherwise
 */
bool input_is_mouse_button_down(InputSystem* system, int button);

/**
 * Capture or release the mouse
 * @param system Input system handle
 * @param capture true to capture, false to release
 */
void input_capture_mouse(InputSystem* system, bool capture);

/**
 * Capture or release the keyboard
 * @param system Input system handle
 * @param capture true to capture, false to release
 */
void input_capture_keyboard(InputSystem* system, bool capture);

/**
 * Process a Windows message (call from window procedure)
 * @param system Input system handle
 * @param message Windows message ID
 * @param wparam Message WPARAM
 * @param lparam Message LPARAM
 */
void input_process_windows_message(InputSystem* system, unsigned int message, unsigned long long wparam, long long lparam);

#ifdef __cplusplus
}
#endif

#endif // POKO_INPUT_H