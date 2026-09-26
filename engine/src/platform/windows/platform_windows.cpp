/**
 * @file platform_windows.cpp
 * @brief Windows-specific platform implementation
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "platform/platform.h"
#include <windows.h>
#include <shlobj.h>
#include <sstream>
#include <fstream>
#include <cstdint>

namespace poko {
namespace platform {

class WindowsPlatform : public Platform {
public:
    bool initialize() override {
        // Windows-specific initialization
        return true;
    }

    void shutdown() override {
        // Windows-specific cleanup
    }

    PlatformType get_platform_type() const override {
        return PlatformType::WINDOWS;
    }

    std::string get_platform_info() const override {
        OSVERSIONINFOEX osvi;
        ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

        // Note: GetVersionEx is deprecated but works for basic info
        // In production, use VersionHelpers.h functions
        std::stringstream ss;
        ss << "Windows ";
        // Add version info when proper Windows API integration is added
        return ss.str();
    }

    std::vector<DisplayInfo> get_displays() const override {
        std::vector<DisplayInfo> displays;
        // TODO: Implement Windows display enumeration using EnumDisplayMonitors
        return displays;
    }

    bool set_display_mode(int display_index, const DisplayMode& mode) override {
        (void)display_index;
        (void)mode;
        // TODO: Implement Windows display mode change using ChangeDisplaySettings
        return false;
    }

    DisplayMode get_display_mode(int display_index) const override {
        (void)display_index;
        // TODO: Implement Windows display mode query using EnumDisplaySettings
        return {1920, 1080, 60, false};
    }

    bool supports_window_management() const override {
        return true;
    }

    bool supports_touch_input() const override {
        // Windows can support touch on touch-enabled devices
        // TODO: Implement proper touch detection
        return false;
    }

    bool supports_gamepad_input() const override {
        return true;
    }

    std::string get_cache_directory() const override {
        wchar_t path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path))) {
            std::wstring wpath(path);
            std::string cache_dir(wpath.begin(), wpath.end());
            cache_dir += "\\Poko\\Cache";
            return cache_dir;
        }
        return "C:\\ProgramData\\Poko\\Cache";
    }

    std::string get_data_directory() const override {
        wchar_t path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, path))) {
            std::wstring wpath(path);
            std::string data_dir(wpath.begin(), wpath.end());
            data_dir += "\\Poko\\Data";
            return data_dir;
        }
        return "C:\\ProgramData\\Poko\\Data";
    }

    std::string get_save_directory() const override {
        wchar_t path[MAX_PATH];
        if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_MYDOCUMENTS, nullptr, 0, path))) {
            std::wstring wpath(path);
            std::string save_dir(wpath.begin(), wpath.end());
            save_dir += "\\Poko\\Saves";
            return save_dir;
        }
        return "C:\\Users\\Public\\Documents\\Poko\\Saves";
    }

    std::string get_temp_directory() const override {
        wchar_t path[MAX_PATH];
        if (GetTempPathW(MAX_PATH, path)) {
            std::wstring wpath(path);
            return std::string(wpath.begin(), wpath.end());
        }
        return "C:\\Temp";
    }

    bool directory_exists(const std::string& path) const override {
        DWORD attrib = GetFileAttributesA(path.c_str());
        return (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY));
    }

    bool create_directory(const std::string& path) const override {
        return CreateDirectoryA(path.c_str(), nullptr) || GetLastError() == ERROR_ALREADY_EXISTS;
    }

    uint64_t get_time_ms() const override {
        LARGE_INTEGER frequency, counter;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&counter);
        return (counter.QuadPart * 1000) / frequency.QuadPart;
    }

    uint64_t get_time_us() const override {
        LARGE_INTEGER frequency, counter;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&counter);
        return (counter.QuadPart * 1000000) / frequency.QuadPart;
    }

    void sleep_ms(uint32_t milliseconds) const override {
        Sleep(milliseconds);
    }

    void register_memory_pressure_callback(std::function<void()> callback) override {
        (void)callback;
        // TODO: Implement Windows memory pressure callback using memory notification APIs
    }

    void register_thermal_callback(std::function<void()> callback) override {
        (void)callback;
        // TODO: Implement Windows thermal callback using thermal notification APIs
    }
};

// Windows-specific factory function
namespace poko {
namespace platform {
std::unique_ptr<Platform> create_windows_platform() {
    return std::make_unique<WindowsPlatform>();
}
} // namespace platform
} // namespace poko

} // namespace platform
} // namespace poko
