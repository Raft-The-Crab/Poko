/**
 * @file audio_mixer.h
 * @brief Audio mixer interface for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <atomic>
#include <mutex>
#include <AL/al.h>
#include <AL/alc.h>
#include <glm/glm.hpp>

namespace Poko {
namespace Audio {

/**
 * @brief Audio sample configuration
 */
struct AudioConfig {
    uint32_t sample_rate = 44100;
    uint16_t channels = 2;
    uint16_t bits_per_sample = 16;
};

/**
 * @brief Audio category for volume control and ducking
 */
enum class AudioCategory {
    SFX = 0,
    Music = 1,
    Voice = 2,
    Ambient = 3,
    UI = 4
};

/**
 * @brief Audio mixer configuration
 */
struct MixerConfig {
    size_t max_sources = 32;
    bool enable_spatial_audio = true;
    bool enable_ducking = true;
    float ducking_attack_time = 0.1f;
    float ducking_release_time = 0.5f;
    float ducking_volume = 0.3f;
};

/**
 * @brief Audio sample data
 */
struct AudioSample {
    uint32_t id;
    std::string name;
    AudioConfig config;
    ALuint buffer = 0;
    bool loaded = false;
    float priority = 0.5f;
    AudioCategory category = AudioCategory::SFX;
    size_t ref_count = 0;
};

/**
 * @brief Active sound state
 */
struct ActiveSound {
    uint32_t sample_id;
    uint32_t sound_id;
    ALuint source;
    float instance_volume;
    float priority;
    bool playing;
    bool paused;
    bool looping;
    glm::vec3 position;
    glm::vec3 velocity;
    AudioCategory category;
    float fade_volume = 1.0f;
    bool fading_out = false;
    bool fading_in = false;
    float fade_duration = 0.0f;
    float fade_elapsed = 0.0f;
};

/**
 * @brief Audio mixer for mixing and playing audio
 */
class AudioMixer {
public:
    AudioMixer();
    ~AudioMixer();

    /**
     * @brief Initialize the audio mixer
     * @param config Audio configuration
     * @param mixer_config Mixer configuration
     * @return true if initialization succeeded
     */
    bool Initialize(const AudioConfig& config, const MixerConfig& mixer_config = MixerConfig{});

    /**
     * @brief Shutdown the audio mixer
     */
    void Shutdown();

    /**
     * @brief Load an audio sample
     * @param name Sample name
     * @param data Sample data (will be moved/copied)
     * @param config Sample configuration
     * @param category Audio category
     * @return Sample ID
     */
    uint32_t LoadSample(const std::string& name, std::vector<int16_t>&& data, const AudioConfig& config, AudioCategory category = AudioCategory::SFX);

    /**
     * @brief Unload an audio sample
     * @param sample_id Sample ID
     * @return true if unloaded successfully
     */
    bool UnloadSample(uint32_t sample_id);

    /**
     * @brief Play a sample
     * @param sample_id Sample ID
     * @param volume Volume (0.0 to 1.0)
     * @param pan Pan (-1.0 left to 1.0 right)
     * @param priority Priority (0.0 to 1.0) for channel stealing
     * @param position 3D position for spatial audio
     * @param looping Whether to loop the sound
     * @return Sound instance ID
     */
    uint32_t PlaySample(uint32_t sample_id, float volume = 1.0f, float pan = 0.0f, float priority = 0.5f, 
                      const glm::vec3& position = glm::vec3(0.0f), bool looping = false);

    /**
     * @brief Stop a playing sound instance
     * @param sound_id Sound instance ID
     * @param fade_out_ms Fade out duration in milliseconds (0 = instant)
     */
    void StopSound(uint32_t sound_id, uint32_t fade_out_ms = 0);

    /**
     * @brief Pause a playing sound
     * @param sound_id Sound instance ID
     */
    void PauseSound(uint32_t sound_id);

    /**
     * @brief Resume a paused sound
     * @param sound_id Sound instance ID
     */
    void ResumeSound(uint32_t sound_id);

    /**
     * @brief Set sound volume
     * @param sound_id Sound instance ID
     * @param volume Volume (0.0 to 1.0)
     * @param fade_ms Fade duration in milliseconds (0 = instant)
     */
    void SetSoundVolume(uint32_t sound_id, float volume, uint32_t fade_ms = 0);

    /**
     * @brief Set sound position (for spatial audio)
     * @param sound_id Sound instance ID
     * @param position 3D position
     */
    void SetSoundPosition(uint32_t sound_id, const glm::vec3& position);

    /**
     * @brief Set sound velocity (for Doppler effect)
     * @param sound_id Sound instance ID
     * @param velocity 3D velocity
     */
    void SetSoundVelocity(uint32_t sound_id, const glm::vec3& velocity);

    /**
     * @brief Stop all sounds in a category
     * @param category Audio category
     * @param fade_out_ms Fade out duration
     */
    void StopCategory(AudioCategory category, uint32_t fade_out_ms = 0);

    /**
     * @brief Set category volume
     * @param category Audio category
     * @param volume Volume (0.0 to 1.0)
     */
    void SetCategoryVolume(AudioCategory category, float volume);

    /**
     * @brief Get category volume
     * @param category Audio category
     * @return Current category volume
     */
    float GetCategoryVolume(AudioCategory category) const;

    /**
     * @brief Set master volume
     * @param volume Volume (0.0 to 1.0)
     * @param fade_ms Fade duration in milliseconds (0 = instant)
     */
    void SetMasterVolume(float volume, uint32_t fade_ms = 0);

    /**
     * @brief Get master volume
     * @return Current master volume
     */
    float GetMasterVolume() const;

    /**
     * @brief Set listener position (for spatial audio)
     * @param position 3D position
     */
    void SetListenerPosition(const glm::vec3& position);

    /**
     * @brief Set listener velocity (for Doppler effect)
     * @param velocity 3D velocity
     */
    void SetListenerVelocity(const glm::vec3& velocity);

    /**
     * @brief Set listener orientation
     * @param forward Forward direction vector
     * @param up Up direction vector
     */
    void SetListenerOrientation(const glm::vec3& forward, const glm::vec3& up);

    /**
     * @brief Update audio mixer (called each frame)
     * @param delta_time Time since last update
     */
    void Update(float delta_time);

    /**
     * @brief Check if initialized
     * @return true if initialized
     */
    bool IsInitialized() const;

    /**
     * @brief Get number of active sounds
     * @return Active sound count
     */
    size_t GetActiveSoundCount() const;

    /**
     * @brief Get number of loaded samples
     * @return Loaded sample count
     */
    size_t GetLoadedSampleCount() const;

    /**
     * @brief Set sound finished callback
     * @param callback Function to call when sound finishes
     */
    void SetSoundFinishedCallback(std::function<void(uint32_t sound_id)> callback);

private:
    bool m_initialized;
    AudioConfig m_config;
    MixerConfig m_mixer_config;
    float m_master_volume;
    float m_master_volume_target;
    float m_master_volume_fade_speed;
    
    std::unordered_map<AudioCategory, float> m_category_volumes;
    
    std::unordered_map<uint32_t, AudioSample> m_samples;
    std::vector<ActiveSound> m_active_sounds;
    uint32_t m_sample_id_counter;
    uint32_t m_sound_id_counter;
    
    ALCdevice* m_device;
    ALCcontext* m_context;
    
    std::vector<ALuint> m_source_pool;
    std::vector<bool> m_source_available;
    
    mutable std::mutex m_mutex;
    
    std::function<void(uint32_t sound_id)> m_sound_finished_callback;
    
    glm::vec3 m_listener_position;
    glm::vec3 m_listener_velocity;
    glm::vec3 m_listener_forward;
    glm::vec3 m_listener_up;
    
    ALuint GetAvailableSource();
    void ReleaseSource(ALuint source);
    void StealLowPriorityChannel(uint32_t sample_id, float priority);
    void UpdateFades(float delta_time);
    void UpdateSpatialAudio();
    void ApplyVolume(float instance_volume, AudioCategory category, float fade_volume);
    float GetEffectiveVolume(AudioCategory category) const;
    void CleanupFinishedSounds();
    bool IsSoundFinished(const ActiveSound& sound) const;
};

} // namespace Audio
} // namespace Poko
