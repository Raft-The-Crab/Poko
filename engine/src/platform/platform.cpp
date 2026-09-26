/**
 * @file platform.cpp
 * @brief Platform abstraction implementation with conditional compilation
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "platform/platform.h"
#include <chrono>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#elif defined(__ANDROID__)
#include <android/log.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

namespace poko {
namespace platform {

// Common platform detection
PlatformType get_current_platform() {
#ifdef _WIN32
    return PlatformType::WINDOWS;
#elif defined(__ANDROID__)
    return PlatformType::ANDROID;
#else
    return PlatformType::UNKNOWN;
#endif
}

bool is_windows() {
    return get_current_platform() == PlatformType::WINDOWS;
}

bool is_android() {
    return get_current_platform() == PlatformType::ANDROID;
}

// Platform implementation
class PlatformImpl : public Platform {
public:
    bool initialize() override {
        return true;
    }

    void shutdown() override {
        // Cleanup
    }

    PlatformType get_platform_type() const override {
        return get_current_platform();
    }

    std::string get_platform_info() const override {
        switch (get_platform_type()) {
            case PlatformType::WINDOWS: return "Windows";
            case PlatformType::ANDROID: return "Android";
            default: return "Unknown";
        }
    }

    std::vector<DisplayInfo> get_displays() const override {
        // TODO: Implement display enumeration
        return {};
    }

    bool set_display_mode(int display_index, const DisplayMode& mode) override {
        (void)display_index;
        (void)mode;
        return false;
    }

    DisplayMode get_display_mode(int display_index) const override {
        (void)display_index;
        return {1920, 1080, 60, false};
    }

    bool supports_window_management() const override {
        return !is_android();
    }

    bool supports_touch_input() const override {
        return is_android();
    }

    bool supports_gamepad_input() const override {
        return is_windows();
    }

    std::string get_cache_directory() const override {
#ifdef _WIN32
        wchar_t path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path))) {
            std::wstring wpath(path);
            std::string cache_dir(wpath.begin(), wpath.end());
            cache_dir += "\\Poko\\Cache";
            return cache_dir;
        }
        return "C:\\ProgramData\\Poko\\Cache";
#elif defined(__ANDROID__)
        return "/data/data/com.poko.pokox/cache";
#else
        return "/tmp/poko_cache";
#endif
    }

    std::string get_data_directory() const override {
#ifdef _WIN32
        wchar_t path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path))) {
            std::wstring wpath(path);
            std::string data_dir(wpath.begin(), wpath.end());
            data_dir += "\\Poko\\Data";
            return data_dir;
        }
        return "C:\\ProgramData\\Poko\\Data";
#elif defined(__ANDROID__)
        return "/data/data/com.poko.pokox/files";
#else
        return "/tmp/poko_data";
#endif
    }

    std::string get_save_directory() const override {
#ifdef _WIN32
        wchar_t path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_MYDOCUMENTS, nullptr, 0, path))) {
            std::wstring wpath(path);
            std::string save_dir(wpath.begin(), wpath.end());
            save_dir += "\\Poko\\Saves";
            return save_dir;
        }
        return "C:\\Users\\Public\\Documents\\Poko\\Saves";
#elif defined(__ANDROID__)
        return "/data/data/com.poko.pokox/files/saves";
#else
        return "/tmp/poko_save";
#endif
    }

    std::string get_temp_directory() const override {
#ifdef _WIN32
        wchar_t path[MAX_PATH];
        if (GetTempPathW(MAX_PATH, path)) {
            std::wstring wpath(path);
            return std::string(wpath.begin(), wpath.end());
        }
        return "C:\\Temp";
#elif defined(__ANDROID__)
        return "/data/data/com.poko.pokox/cache/tmp";
#else
        return "/tmp";
#endif
    }

    bool directory_exists(const std::string& path) const override {
#ifdef _WIN32
        DWORD attrib = GetFileAttributesA(path.c_str());
        return (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY));
#elif defined(__ANDROID__)
        struct stat info;
        return stat(path.c_str(), &info) == 0 && S_ISDIR(info.st_mode);
#else
        (void)path;
        return false;
#endif
    }

    bool create_directory(const std::string& path) const override {
#ifdef _WIN32
        return CreateDirectoryA(path.c_str(), nullptr) || GetLastError() == ERROR_ALREADY_EXISTS;
#elif defined(__ANDROID__)
        return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
#else
        (void)path;
        return false;
#endif
    }

    uint64_t get_time_ms() const override {
#ifdef _WIN32
        LARGE_INTEGER frequency, counter;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&counter);
        return (counter.QuadPart * 1000) / frequency.QuadPart;
#else
        auto now = std::chrono::steady_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
#endif
    }

    uint64_t get_time_us() const override {
#ifdef _WIN32
        LARGE_INTEGER frequency, counter;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&counter);
        return (counter.QuadPart * 1000000) / frequency.QuadPart;
#else
        auto now = std::chrono::steady_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
#endif
    }

    void sleep_ms(uint32_t milliseconds) const override {
#ifdef _WIN32
        Sleep(milliseconds);
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
#endif
    }

    void register_memory_pressure_callback(std::function<void()> callback) override {
        (void)callback;
        // TODO: Implement platform-specific callbacks
    }

    void register_thermal_callback(std::function<void()> callback) override {
        (void)callback;
        // TODO: Implement platform-specific callbacks
    }
};

// Platform factory implementation
std::unique_ptr<Platform> PlatformFactory::create() {
    return std::make_unique<PlatformImpl>();
}

// Platform singleton instance
Platform& Platform::instance() {
    static std::unique_ptr<Platform> instance = PlatformFactory::create();
    return *instance;
}

} // namespace platform
} // namespace poko
