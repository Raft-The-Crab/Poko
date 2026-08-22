/**
 * @file config.cpp
 * @brief Configuration system implementation for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "core/config/config.h"
#include "core/logging/logger.h"
#include <fstream>
#include <sstream>

namespace Poko {

Config::Config()
    : m_windowWidth(1280)
    , m_windowHeight(720)
    , m_windowTitle("PokoEngine")
    , m_fullscreen(false)
    , m_vsync(true)
    , m_targetFPS(60)
    , m_masterVolume(1.0f)
    , m_musicVolume(0.8f)
    , m_sfxVolume(1.0f)
    , m_physicsEnabled(true)
    , m_maxPhysicsSubsteps(4)
{
}

Config::~Config()
{
}

bool Config::LoadFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open config file: " + filename);
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }

        // Parse key=value pairs
        size_t delimiterPos = line.find('=');
        if (delimiterPos != std::string::npos) {
            std::string key = line.substr(0, delimiterPos);
            std::string value = line.substr(delimiterPos + 1);

            // Trim whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            // Set configuration values
            SetValue(key, value);
        }
    }

    file.close();
    LOG_INFO("Config loaded from: " + filename);
    return true;
}

bool Config::SaveToFile(const std::string& filename) const
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        LOG_ERROR("Failed to create config file: " + filename);
        return false;
    }

    file << "# PokoEngine Configuration File\n";
    file << "# Auto-generated - do not edit manually\n\n";

    file << "[Window]\n";
    file << "width=" << m_windowWidth << "\n";
    file << "height=" << m_windowHeight << "\n";
    file << "title=" << m_windowTitle << "\n";
    file << "fullscreen=" << (m_fullscreen ? "true" : "false") << "\n\n";

    file << "[Graphics]\n";
    file << "vsync=" << (m_vsync ? "true" : "false") << "\n";
    file << "target_fps=" << m_targetFPS << "\n\n";

    file << "[Audio]\n";
    file << "master_volume=" << m_masterVolume << "\n";
    file << "music_volume=" << m_musicVolume << "\n";
    file << "sfx_volume=" << m_sfxVolume << "\n\n";

    file << "[Physics]\n";
    file << "enabled=" << (m_physicsEnabled ? "true" : "false") << "\n";
    file << "max_substeps=" << m_maxPhysicsSubsteps << "\n";

    file.close();
    LOG_INFO("Config saved to: " + filename);
    return true;
}

void Config::SetValue(const std::string& key, const std::string& value)
{
    if (key == "width") {
        m_windowWidth = std::stoi(value);
    } else if (key == "height") {
        m_windowHeight = std::stoi(value);
    } else if (key == "title") {
        m_windowTitle = value;
    } else if (key == "fullscreen") {
        m_fullscreen = (value == "true");
    } else if (key == "vsync") {
        m_vsync = (value == "true");
    } else if (key == "target_fps") {
        m_targetFPS = std::stoi(value);
    } else if (key == "master_volume") {
        m_masterVolume = std::stof(value);
    } else if (key == "music_volume") {
        m_musicVolume = std::stof(value);
    } else if (key == "sfx_volume") {
        m_sfxVolume = std::stof(value);
    } else if (key == "enabled") {
        m_physicsEnabled = (value == "true");
    } else if (key == "max_substeps") {
        m_maxPhysicsSubsteps = std::stoi(value);
    }
}

// Window settings
int Config::GetWindowWidth() const { return m_windowWidth; }
int Config::GetWindowHeight() const { return m_windowHeight; }
std::string Config::GetWindowTitle() const { return m_windowTitle; }
bool Config::IsFullscreen() const { return m_fullscreen; }

void Config::SetWindowWidth(int width) { m_windowWidth = width; }
void Config::SetWindowHeight(int height) { m_windowHeight = height; }
void Config::SetWindowTitle(const std::string& title) { m_windowTitle = title; }
void Config::SetFullscreen(bool fullscreen) { m_fullscreen = fullscreen; }

// Graphics settings
bool Config::IsVSyncEnabled() const { return m_vsync; }
int Config::GetTargetFPS() const { return m_targetFPS; }

void Config::SetVSync(bool vsync) { m_vsync = vsync; }
void Config::SetTargetFPS(int fps) { m_targetFPS = fps; }

// Audio settings
float Config::GetMasterVolume() const { return m_masterVolume; }
float Config::GetMusicVolume() const { return m_musicVolume; }
float Config::GetSFXVolume() const { return m_sfxVolume; }

void Config::SetMasterVolume(float volume) { m_masterVolume = volume; }
void Config::SetMusicVolume(float volume) { m_musicVolume = volume; }
void Config::SetSFXVolume(float volume) { m_sfxVolume = volume; }

// Physics settings
bool Config::IsPhysicsEnabled() const { return m_physicsEnabled; }
int Config::GetMaxPhysicsSubsteps() const { return m_maxPhysicsSubsteps; }

void Config::SetPhysicsEnabled(bool enabled) { m_physicsEnabled = enabled; }
void Config::SetMaxPhysicsSubsteps(int substeps) { m_maxPhysicsSubsteps = substeps; }

} // namespace Poko