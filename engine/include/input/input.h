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
 * @brief Input system
 * 
 * Manages keyboard, mouse, and gamepad input.
 */
class Input {
public:
    Input() = default;
    ~Input() = default;

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
};

} // namespace Poko

#endif // POKO_INPUT_INPUT_H