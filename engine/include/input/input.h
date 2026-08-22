/**
 * @file input.h
 * @brief Input system interface
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-21
 */

#ifndef POKO_INPUT_INPUT_H
#define POKO_INPUT_INPUT_H

#include <unordered_map>
#include <utility>
#include <string>
#include <vector>
#include <array>

namespace Poko {

/**
 * @brief Key codes
 */
enum class KeyCode {
    UNKNOWN = 0,
    // Alphanumeric
    A = 4, B = 5, C = 6, D = 7, E = 8, F = 9, G = 10, H = 11, I = 12, J = 13,
    K = 14, L = 15, M = 16, N = 17, O = 18, P = 19, Q = 20, R = 21, S = 22, T = 23,
    U = 24, V = 25, W = 26, X = 27, Y = 28, Z = 29,
    _0 = 30, _1 = 31, _2 = 32, _3 = 33, _4 = 34, _5 = 35, _6 = 36, _7 = 37, _8 = 38, _9 = 39,
    // Special keys
    ESCAPE = 41,
    SPACE = 44,
    ENTER = 40,
    TAB = 43,
    BACKSPACE = 42,
    // Arrow keys
    LEFT = 80, RIGHT = 79, UP = 82, DOWN = 81,
    // Modifier keys
    LSHIFT = 225, RSHIFT = 229, LCTRL = 224, RCTRL = 228, LALT = 226, RALT = 230,
    // Function keys
    F1 = 58, F2 = 59, F3 = 60, F4 = 61, F5 = 62, F6 = 63, F7 = 64, F8 = 65, F9 = 66, F10 = 67,
    F11 = 68, F12 = 69
};

/**
 * @brief Mouse buttons
 */
enum class MouseButton {
    LEFT = 1,
    MIDDLE = 2,
    RIGHT = 3,
    X1 = 4,
    X2 = 5
};

/**
 * @brief Gamepad buttons
 */
enum class GamepadButton {
    A = 0,
    B = 1,
    X = 2,
    Y = 3,
    LEFT_BUMPER = 4,
    RIGHT_BUMPER = 5,
    BACK = 6,
    START = 7,
    LEFT_STICK = 8,
    RIGHT_STICK = 9,
    DPAD_UP = 10,
    DPAD_DOWN = 11,
    DPAD_LEFT = 12,
    DPAD_RIGHT = 13
};

/**
 * @brief Gamepad axes
 */
enum class GamepadAxis {
    LEFT_X = 0,
    LEFT_Y = 1,
    RIGHT_X = 2,
    RIGHT_Y = 3,
    LEFT_TRIGGER = 4,
    RIGHT_TRIGGER = 5
};

/**
 * @brief Input system
 * 
 * Manages keyboard, mouse, and gamepad input.
 */
class Input {
public:
    Input();
    ~Input();

    /**
     * @brief Check if a key is currently down
     * @param key Key code to check
     * @return true if key is down, false otherwise
     */
    bool is_key_down(KeyCode key) const;

    /**
     * @brief Check if a key was pressed this frame
     * @param key Key code to check
     * @return true if key was pressed this frame, false otherwise
     */
    bool is_key_pressed(KeyCode key) const;

    /**
     * @brief Check if a key was released this frame
     * @param key Key code to check
     * @return true if key was released this frame, false otherwise
     */
    bool is_key_released(KeyCode key) const;

    /**
     * @brief Check if a mouse button is currently down
     * @param button Mouse button to check
     * @return true if button is down, false otherwise
     */
    bool is_mouse_down(MouseButton button) const;

    /**
     * @brief Check if a mouse button was pressed this frame
     * @param button Mouse button to check
     * @return true if button was pressed this frame, false otherwise
     */
    bool is_mouse_pressed(MouseButton button) const;

    /**
     * @brief Check if a mouse button was released this frame
     * @param button Mouse button to check
     * @return true if button was released this frame, false otherwise
     */
    bool is_mouse_released(MouseButton button) const;

    /**
     * @brief Get current mouse position
     * @return Pair of (x, y) coordinates
     */
    std::pair<int, int> get_mouse_position() const;

    /**
     * @brief Get mouse movement delta
     * @return Pair of (dx, dy) movement since last frame
     */
    std::pair<int, int> get_mouse_delta() const;

    /**
     * @brief Get mouse scroll delta
     * @return Pair of (scroll_x, scroll_y) scroll values
     */
    std::pair<int, int> get_mouse_scroll() const;

    /**
     * @brief Check if a gamepad button is currently down
     * @param gamepad_index Gamepad index (0-3)
     * @param button Gamepad button to check
     * @return true if button is down, false otherwise
     */
    bool is_gamepad_button_down(int gamepad_index, GamepadButton button) const;

    /**
     * @brief Check if a gamepad button was pressed this frame
     * @param gamepad_index Gamepad index (0-3)
     * @param button Gamepad button to check
     * @return true if button was pressed this frame, false otherwise
     */
    bool is_gamepad_button_pressed(int gamepad_index, GamepadButton button) const;

    /**
     * @brief Check if a gamepad button was released this frame
     * @param gamepad_index Gamepad index (0-3)
     * @param button Gamepad button to check
     * @return true if button was released this frame, false otherwise
     */
    bool is_gamepad_button_released(int gamepad_index, GamepadButton button) const;

    /**
     * @brief Get gamepad axis value
     * @param gamepad_index Gamepad index (0-3)
     * @param axis Gamepad axis to get
     * @return Axis value (-1.0 to 1.0 for sticks, 0.0 to 1.0 for triggers)
     */
    float get_gamepad_axis(int gamepad_index, GamepadAxis axis) const;

    /**
     * @brief Check if a gamepad is connected
     * @param gamepad_index Gamepad index (0-3)
     * @return true if gamepad is connected, false otherwise
     */
    bool is_gamepad_connected(int gamepad_index) const;

    /**
     * @brief Get text input from keyboard (for text fields)
     * @return Text input since last frame
     */
    std::string get_text_input() const;

    /**
     * @brief Check if any key is currently down
     * @return true if any key is down, false otherwise
     */
    bool is_any_key_down() const;

    /**
     * @brief Check if any mouse button is currently down
     * @return true if any mouse button is down, false otherwise
     */
    bool is_any_mouse_down() const;

    /**
     * @brief Set mouse position (for cursor warping)
     * @param x X coordinate
     * @param y Y coordinate
     */
    void set_mouse_position(int x, int y);

    /**
     * @brief Lock mouse cursor to window
     * @param locked Whether to lock cursor
     */
    void set_mouse_locked(bool locked);

    /**
     * @brief Check if mouse cursor is locked
     * @return true if cursor is locked, false otherwise
     */
    bool is_mouse_locked() const;

    /**
     * @brief Get touch points (for mobile/touch devices)
     * @return Vector of touch point data
     */
    struct TouchPoint {
        int id;
        int x;
        int y;
        bool pressed;
        bool released;
    };
    std::vector<TouchPoint> get_touch_points() const;

    /**
     * @brief Check if touch input is available
     * @return true if touch input is available, false otherwise
     */
    bool is_touch_available() const;

    /**
     * @brief Update input state (call once per frame)
     */
    void update();

    /**
     * @brief Reset all input state
     */
    void reset();

private:
    // Key states
    std::unordered_map<KeyCode, bool> keys_down_;
    std::unordered_map<KeyCode, bool> keys_pressed_;
    std::unordered_map<KeyCode, bool> keys_released_;

    // Mouse button states
    std::unordered_map<MouseButton, bool> mouse_down_;
    std::unordered_map<MouseButton, bool> mouse_pressed_;
    std::unordered_map<MouseButton, bool> mouse_released_;

    // Mouse position
    int mouse_x_ = 0;
    int mouse_y_ = 0;
    int mouse_dx_ = 0;
    int mouse_dy_ = 0;
    int mouse_scroll_x_ = 0;
    int mouse_scroll_y_ = 0;
    bool mouse_locked_ = false;

    // Gamepad states (4 gamepads max)
    static constexpr int MAX_GAMEPADS = 4;
    struct GamepadState {
        bool connected = false;
        std::unordered_map<GamepadButton, bool> buttons_down;
        std::unordered_map<GamepadButton, bool> buttons_pressed;
        std::unordered_map<GamepadButton, bool> buttons_released;
        std::unordered_map<GamepadAxis, float> axes;
    };
    std::array<GamepadState, MAX_GAMEPADS> gamepads_;

    // Text input
    std::string text_input_;

    // Touch input
    std::vector<TouchPoint> touch_points_;
    bool touch_available_ = false;
};

} // namespace Poko

#endif // POKO_INPUT_INPUT_H