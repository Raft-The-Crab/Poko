/**
 * @file platform_android.cpp
 * @brief Android-specific platform implementation
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "platform/platform.h"
#include <android/log.h>
#include <android/asset_manager.h>
#include <sstream>
#include <unistd.h>
#include <sys/stat.h>
#include <cstdint>

namespace poko {
namespace platform {

class AndroidPlatform : public Platform {
public:
    bool initialize() override {
        // Android-specific initialization
        // TODO: Get JNIEnv and AssetManager from Java side
        return true;
    }

    void shutdown() override {
        // Android-specific cleanup
    }

    PlatformType get_platform_type() const override {
        return PlatformType::ANDROID;
    }

    std::string get_platform_info() const override {
        // TODO: Get Android version and device info
        return "Android";
    }

    std::vector<DisplayInfo> get_displays() const override {
        std::vector<DisplayInfo> displays;
        // TODO: Implement Android display enumeration using AChoreographer
        return displays;
    }

    bool set_display_mode(int display_index, const DisplayMode& mode) override {
        (void)display_index;
        (void)mode;
        // Android doesn't support arbitrary display mode changes
        return false;
    }

    DisplayMode get_display_mode(int display_index) const override {
        (void)display_index;
        // TODO: Get current Android display mode using AChoreographer
        return {1920, 1080, 60, false};
    }

    bool supports_window_management() const override {
        return false; // Android has different window management
    }

    bool supports_touch_input() const override {
        return true;
    }

    bool supports_gamepad_input() const override {
        return true; // Android supports gamepads via USB/Bluetooth
    }

    std::string get_cache_directory() const override {
        // Android cache directory
        return "/data/data/com.poko.pokox/cache";
    }

    std::string get_data_directory() const override {
        // Android data directory
        return "/data/data/com.poko.pokox/files";
    }

    std::string get_save_directory() const override {
        // Android save directory (same as data directory for now)
        return "/data/data/com.poko.pokox/files/saves";
    }

    std::string get_temp_directory() const override {
        // Android temp directory
        return "/data/data/com.poko.pokox/cache/tmp";
    }

    bool directory_exists(const std::string& path) const override {
        struct stat info;
        return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
    }

    bool create_directory(const std::string& path) const override {
        return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
    }

    uint64_t get_time_ms() const override {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
    }

    uint64_t get_time_us() const override {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (ts.tv_sec * 1000000) + (ts.tv_nsec / 1000);
    }

    void sleep_ms(uint32_t milliseconds) const override {
        usleep(milliseconds * 1000);
    }

    void register_memory_pressure_callback(std::function<void()> callback) override {
        (void)callback;
        // TODO: Implement Android memory pressure callback using ComponentCallbacks2
    }

    void register_thermal_callback(std::function<void()> callback) override {
        (void)callback;
        // TODO: Implement Android thermal callback using thermal status APIs
    }
};

// Android-specific factory function
namespace poko {
namespace platform {
std::unique_ptr<Platform> create_android_platform() {
    return std::make_unique<AndroidPlatform>();
}
} // namespace platform
} // namespace poko

} // namespace platform
} // namespace poko
