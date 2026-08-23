/**
 * @file window.h
 * @brief Window management interface using SDL2
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-21
 */

#ifndef POKO_CORE_WINDOW_WINDOW_H
#define POKO_CORE_WINDOW_WINDOW_H

#include <string>
#include <functional>

namespace Poko {

/**
 * @brief Window event types
 */
enum class WindowEventType {
    Resize,
    Close,
    FocusGained,
    FocusLost,
    Minimize,
    Maximize,
    Restore
};

/**
 * @brief Window event data
 */
struct WindowEvent {
    WindowEventType type;
    int width;
    int height;
    bool focused;
};

/**
 * @brief Window event callback type
 */
using WindowEventCallback = std::function<void(const WindowEvent&)>;

/**
 * @brief Window configuration
 */
struct WindowConfig {
    int width = 1280;
    int height = 720;
    const char* title = "Poko Engine";
    bool vsync = true;
    bool fullscreen = false;
    bool resizable = true;
};

/**
 * @brief Window management class
 *
 * Manages SDL2 window creation, event handling, and presentation.
 */
class Window {
public:
    /**
     * @brief Window configuration
     */
    using Config = WindowConfig;

    /**
     * @brief Construct window with configuration
     * @param config Window configuration
     */
    explicit Window(const Config& config);

    /**
     * @brief Destructor
     */
    ~Window();

    /**
     * @brief Initialize the window
     * @return true if initialization succeeded
     */
    bool initialize();

    /**
     * @brief Update window (handle events)
     */
    void update();

    /**
     * @brief Present window (swap buffers)
     */
    void present();

    /**
     * @brief Shutdown the window
     */
    void shutdown();

    /**
     * @brief Set window title
     * @param title New window title
     */
    void set_title(const std::string& title);

    /**
     * @brief Check if window should close
     * @return true if window should close
     */
    bool should_close() const { return should_close_; }

    /**
     * @brief Check if window is initialized
     * @return true if initialized
     */
    bool is_initialized() const { return initialized_; }

    /**
     * @brief Get window width
     * @return Window width
     */
    int get_width() const { return config_.width; }

    /**
     * @brief Get window height
     * @return Window height
     */
    int get_height() const { return config_.height; }

    /**
     * @brief Get native window handle
     * @return Native window handle
     */
    void* get_native_handle() const { return sdl_window_; }

    /**
     * @brief Get window configuration
     * @return Current window configuration
     */
    const Config& get_config() const { return config_; }

    /**
     * @brief Set window event callback
     * @param callback Function to call when window events occur
     */
    void set_event_callback(WindowEventCallback callback);

private:
    void process_events();

    Config config_;
    bool initialized_ = false;
    bool should_close_ = false;
    void* sdl_window_ = nullptr;
    void* sdl_context_ = nullptr;
    WindowEventCallback event_callback_;
};

} // namespace Poko

#endif // POKO_CORE_WINDOW_WINDOW_H