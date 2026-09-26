/**
 * @file platform.h
 * @brief Platform abstraction layer for Poko Engine
 * @details Provides cross-platform abstraction for OS-specific functionality
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#pragma once

#include <string>
#include <memory>
#include <functional>
#include <vector>
#include <cstdint>

namespace poko {
namespace platform {

/**
 * @enum PlatformType
 * @brief Supported platform types
 */
enum class PlatformType {
    WINDOWS,
    ANDROID,
    UNKNOWN
};

/**
 * @enum WindowState
 * @brief Window state enumeration
 */
enum class WindowState {
    NORMAL,
    MINIMIZED,
    MAXIMIZED,
    FULLSCREEN,
    HIDDEN
};

/**
 * @struct DisplayMode
 * @brief Display mode configuration
 */
struct DisplayMode {
    int width;
    int height;
    int refresh_rate;
    bool vsync_enabled;
};

/**
 * @struct DisplayInfo
 * @brief Display information
 */
struct DisplayInfo {
    int width;
    int height;
    int refresh_rate;
    std::string name;
    bool primary;
};

/**
 * @class Platform
 * @brief Main platform abstraction interface
 * @details Provides platform-specific functionality through a unified interface
 */
class Platform {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the platform instance
     */
    static Platform& instance();

    /**
     * @brief Initialize the platform
     * @return true if initialization succeeded
     */
    virtual bool initialize() = 0;

    /**
     * @brief Shutdown the platform
     */
    virtual void shutdown() = 0;

    /**
     * @brief Get the current platform type
     * @return The platform type
     */
    virtual PlatformType get_platform_type() const = 0;

    /**
     * @brief Get platform-specific information
     * @return Platform information string
     */
    virtual std::string get_platform_info() const = 0;

    /**
     * @brief Get available displays
     * @return Vector of display information
     */
    virtual std::vector<DisplayInfo> get_displays() const = 0;

    /**
     * @brief Set the display mode
     * @param display_index Display index
     * @param mode Display mode to set
     * @return true if mode change succeeded
     */
    virtual bool set_display_mode(int display_index, const DisplayMode& mode) = 0;

    /**
     * @brief Get the current display mode
     * @param display_index Display index
     * @return Current display mode
     */
    virtual DisplayMode get_display_mode(int display_index) const = 0;

    /**
     * @brief Check if the platform supports window management
     * @return true if window management is supported
     */
    virtual bool supports_window_management() const = 0;

    /**
     * @brief Check if the platform supports touch input
     * @return true if touch input is supported
     */
    virtual bool supports_touch_input() const = 0;

    /**
     * @brief Check if the platform supports gamepad input
     * @return true if gamepad input is supported
     */
    virtual bool supports_gamepad_input() const = 0;

    /**
     * @brief Get the default cache directory
     * @return Path to the cache directory
     */
    virtual std::string get_cache_directory() const = 0;

    /**
     * @brief Get the default data directory
     * @return Path to the data directory
     */
    virtual std::string get_data_directory() const = 0;

    /**
     * @brief Get the default save directory
     * @return Path to the save directory
     */
    virtual std::string get_save_directory() const = 0;

    /**
     * @brief Get the temporary directory
     * @return Path to the temporary directory
     */
    virtual std::string get_temp_directory() const = 0;

    /**
     * @brief Check if a directory exists
     * @param path Directory path
     * @return true if directory exists
     */
    virtual bool directory_exists(const std::string& path) const = 0;

    /**
     * @brief Create a directory
     * @param path Directory path to create
     * @return true if directory was created
     */
    virtual bool create_directory(const std::string& path) const = 0;

    /**
     * @brief Get the current time in milliseconds
     * @return Current time in milliseconds
     */
    virtual uint64_t get_time_ms() const = 0;

    /**
     * @brief Get the current time in microseconds
     * @return Current time in microseconds
     */
    virtual uint64_t get_time_us() const = 0;

    /**
     * @brief Sleep for the specified duration
     * @param milliseconds Duration to sleep in milliseconds
     */
    virtual void sleep_ms(uint32_t milliseconds) const = 0;

    /**
     * @brief Register a callback for memory pressure signals
     * @param callback Function to call when memory pressure is detected
     */
    virtual void register_memory_pressure_callback(std::function<void()> callback) = 0;

    /**
     * @brief Register a callback for thermal signals
     * @param callback Function to call when thermal pressure is detected
     */
    virtual void register_thermal_callback(std::function<void()> callback) = 0;

public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    virtual ~Platform() = default;

protected:
    /**
     * @brief Protected constructor for singleton pattern
     */
    Platform() = default;
};

/**
 * @class PlatformFactory
 * @brief Factory for creating platform-specific implementations
 */
class PlatformFactory {
public:
    /**
     * @brief Create the appropriate platform instance
     * @return Unique pointer to the platform instance
     */
    static std::unique_ptr<Platform> create();
};

/**
 * @brief Convenience function to get the current platform type
 * @return The current platform type
 */
PlatformType get_current_platform();

/**
 * @brief Check if running on Windows
 * @return true if running on Windows
 */
bool is_windows();

/**
 * @brief Check if running on Android
 * @return true if running on Android
 */
bool is_android();


} // namespace platform
} // namespace poko
