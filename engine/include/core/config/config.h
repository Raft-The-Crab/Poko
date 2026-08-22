/**
 * @file config.h
 * @brief Configuration system header for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#pragma once

#include <string>

namespace Poko {

/**
 * @brief Configuration system for engine settings
 */
class Config {
public:
    Config();
    ~Config();

    /**
     * @brief Load configuration from file
     * @param filename Path to configuration file
     * @return true if loaded successfully, false otherwise
     */
    bool LoadFromFile(const std::string& filename);

    /**
     * @brief Save configuration to file
     * @param filename Path to configuration file
     * @return true if saved successfully, false otherwise
     */
    bool SaveToFile(const std::string& filename) const;

    // Window settings
    int GetWindowWidth() const;
    int GetWindowHeight() const;
    std::string GetWindowTitle() const;
    bool IsFullscreen() const;

    void SetWindowWidth(int width);
    void SetWindowHeight(int height);
    void SetWindowTitle(const std::string& title);
    void SetFullscreen(bool fullscreen);

    // Graphics settings
    bool IsVSyncEnabled() const;
    int GetTargetFPS() const;

    void SetVSync(bool vsync);
    void SetTargetFPS(int fps);

    // Audio settings
    float GetMasterVolume() const;
    float GetMusicVolume() const;
    float GetSFXVolume() const;

    void SetMasterVolume(float volume);
    void SetMusicVolume(float volume);
    void SetSFXVolume(float volume);

    // Physics settings
    bool IsPhysicsEnabled() const;
    int GetMaxPhysicsSubsteps() const;

    void SetPhysicsEnabled(bool enabled);
    void SetMaxPhysicsSubsteps(int substeps);

private:
    void SetValue(const std::string& key, const std::string& value);

    // Window settings
    int m_windowWidth;
    int m_windowHeight;
    std::string m_windowTitle;
    bool m_fullscreen;

    // Graphics settings
    bool m_vsync;
    int m_targetFPS;

    // Audio settings
    float m_masterVolume;
    float m_musicVolume;
    float m_sfxVolume;

    // Physics settings
    bool m_physicsEnabled;
    int m_maxPhysicsSubsteps;
};

} // namespace Poko