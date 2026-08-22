/**
 * @file audio_mixer.cpp
 * @brief Audio mixer implementation for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "audio/mixer/audio_mixer.h"
#include "core/logging/logger.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <algorithm>
#include <cstring>

namespace Poko {
namespace Audio {

AudioMixer::AudioMixer()
    : m_initialized(false)
    , m_master_volume(1.0f)
    , m_master_volume_target(1.0f)
    , m_master_volume_fade_speed(0.0f)
    , m_sample_id_counter(0)
    , m_sound_id_counter(0)
    , m_device(nullptr)
    , m_context(nullptr)
    , m_listener_position(0.0f)
    , m_listener_velocity(0.0f)
    , m_listener_forward(0.0f, 0.0f, -1.0f)
    , m_listener_up(0.0f, 1.0f, 0.0f)
{
    // Initialize category volumes
    m_category_volumes[AudioCategory::SFX] = 1.0f;
    m_category_volumes[AudioCategory::Music] = 1.0f;
    m_category_volumes[AudioCategory::Voice] = 1.0f;
    m_category_volumes[AudioCategory::Ambient] = 1.0f;
    m_category_volumes[AudioCategory::UI] = 1.0f;
}

AudioMixer::~AudioMixer()
{
    if (m_initialized) {
        Shutdown();
    }
}

bool AudioMixer::Initialize(const AudioConfig& config, const MixerConfig& mixer_config)
{
    if (m_initialized) {
        LOG_WARNING("AudioMixer already initialized");
        return false;
    }

    LOG_INFO("Initializing AudioMixer with OpenAL...");

    m_config = config;
    m_mixer_config = mixer_config;
    m_samples.reserve(128);

    // Open OpenAL device
    m_device = alcOpenDevice(nullptr);
    if (!m_device) {
        LOG_ERROR("AudioMixer: Failed to open OpenAL device");
        return false;
    }

    // Create OpenAL context
    m_context = alcCreateContext(m_device, nullptr);
    if (!m_context) {
        LOG_ERROR("AudioMixer: Failed to create OpenAL context");
        alcCloseDevice(m_device);
        m_device = nullptr;
        return false;
    }

    // Make context current
    if (!alcMakeContextCurrent(m_context)) {
        LOG_ERROR("AudioMixer: Failed to make OpenAL context current");
        alcDestroyContext(m_context);
        alcCloseDevice(m_device);
        m_context = nullptr;
        m_device = nullptr;
        return false;
    }

    // Configure distance model for spatial audio
    if (m_mixer_config.enable_spatial_audio) {
        alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
        alDopplerFactor(1.0f);
        alDopplerVelocity(343.0f); // Speed of sound in m/s
    }

    // Pre-allocate source pool
    m_source_pool.resize(m_mixer_config.max_sources, 0);
    m_source_available.resize(m_mixer_config.max_sources, true);
    
    alGenSources(static_cast<ALsizei>(m_mixer_config.max_sources), m_source_pool.data());
    ALenum error = alGetError();
    
    if (error != AL_NO_ERROR) {
        // Handle partial allocation
        for (size_t i = 0; i < m_mixer_config.max_sources; i++) {
            if (alGetError() == AL_NO_ERROR) {
                m_source_available[i] = true;
            } else {
                m_source_available[i] = false;
                m_source_pool[i] = 0;
            }
        }
        LOG_WARNING("AudioMixer: Partial source pool allocation, got " + 
                   std::to_string(std::count(m_source_available.begin(), m_source_available.end(), true)) + 
                   " sources out of " + std::to_string(m_mixer_config.max_sources));
    }

    LOG_INFO("AudioMixer initialized successfully with " + 
            std::to_string(std::count(m_source_available.begin(), m_source_available.end(), true)) + " sources");

    // Initialize ducking state
    m_category_ducking_state[AudioCategory::SFX] = false;
    m_category_ducking_state[AudioCategory::Music] = false;
    m_category_ducking_state[AudioCategory::Voice] = false;
    m_category_ducking_state[AudioCategory::Ambient] = false;
    m_category_ducking_state[AudioCategory::UI] = false;

    m_initialized = true;
    return true;
}

void AudioMixer::Shutdown()
{
    if (!m_initialized) {
        return;
    }

    LOG_INFO("Shutting down AudioMixer...");

    std::lock_guard<std::mutex> lock(m_mutex);

    // Stop all active sounds
    for (auto& sound : m_active_sounds) {
        if (sound.source) {
            alSourceStop(sound.source);
        }
    }

    // Delete all sources
    for (auto& sound : m_active_sounds) {
        if (sound.source) {
            alSourcei(sound.source, AL_BUFFER, 0);
        }
    }

    m_active_sounds.clear();

    // Delete all audio buffers
    for (auto& [id, sample] : m_samples) {
        if (sample.buffer) {
            alDeleteBuffers(1, &sample.buffer);
            sample.buffer = 0;
        }
    }

    m_samples.clear();
    m_sample_id_counter = 0;
    m_sound_id_counter = 0;

    // Delete source pool
    alDeleteSources(static_cast<ALsizei>(m_source_pool.size()), m_source_pool.data());
    m_source_pool.clear();
    m_source_available.clear();

    // Destroy OpenAL context
    if (m_context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(m_context);
        m_context = nullptr;
    }

    // Close OpenAL device
    if (m_device) {
        alcCloseDevice(m_device);
        m_device = nullptr;
    }

    m_initialized = false;
    LOG_INFO("AudioMixer shutdown complete");
}

uint32_t AudioMixer::LoadSample(const std::string& name, std::vector<int16_t>&& data, const AudioConfig& config, AudioCategory category)
{
    if (!m_initialized) {
        LOG_ERROR("AudioMixer: Cannot load sample - not initialized");
        return 0;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    // Check if sample already exists
    for (const auto& [id, sample] : m_samples) {
        if (sample.name == name) {
            LOG_WARNING("AudioMixer: Sample already loaded: " + name);
            return id;
        }
    }

    // Create OpenAL buffer
    ALuint buffer;
    alGenBuffers(1, &buffer);
    if (alGetError() != AL_NO_ERROR) {
        LOG_ERROR("AudioMixer: Failed to generate OpenAL buffer");
        return 0;
    }

    // Determine OpenAL format
    ALenum format = AL_FORMAT_STEREO16;
    if (config.channels == 1) {
        format = (config.bits_per_sample == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
    } else {
        format = (config.bits_per_sample == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
    }

    // Upload audio data to buffer
    alBufferData(buffer, format, data.data(), static_cast<ALsizei>(data.size() * sizeof(int16_t)), 
                static_cast<ALsizei>(config.sample_rate));
    if (alGetError() != AL_NO_ERROR) {
        LOG_ERROR("AudioMixer: Failed to upload audio data to buffer");
        alDeleteBuffers(1, &buffer);
        return 0;
    }

    // Store sample
    AudioSample sample;
    sample.id = ++m_sample_id_counter;
    sample.name = name;
    sample.config = config;
    sample.buffer = buffer;
    sample.loaded = true;
    sample.category = category;
    sample.ref_count = 0;

    m_samples[sample.id] = std::move(sample);

    LOG_INFO("Loaded audio sample: " + name + " (ID: " + std::to_string(sample.id) + 
            ", size: " + std::to_string(data.size()) + " samples, category: " + 
            std::to_string(static_cast<int>(category)) + ")");
    return sample.id;
}

bool AudioMixer::UnloadSample(uint32_t sample_id)
{
    if (!m_initialized) {
        return false;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_samples.find(sample_id);
    if (it == m_samples.end()) {
        LOG_WARNING("AudioMixer: Sample not found: " + std::to_string(sample_id));
        return false;
    }

    // Stop all sounds using this sample
    for (auto sound_it = m_active_sounds.begin(); sound_it != m_active_sounds.end(); ) {
        if (sound_it->sample_id == sample_id) {
            alSourceStop(sound_it->source);
            alSourcei(sound_it->source, AL_BUFFER, 0);
            ReleaseSource(sound_it->source);
            sound_it = m_active_sounds.erase(sound_it);
        } else {
            ++sound_it;
        }
    }

    // Delete buffer
    if (it->second.buffer) {
        alDeleteBuffers(1, &it->second.buffer);
        it->second.buffer = 0;
    }

    LOG_INFO("Unloaded audio sample: " + it->second.name + " (ID: " + std::to_string(sample_id) + ")");
    m_samples.erase(it);
    return true;
}

ALuint AudioMixer::GetAvailableSource()
{
    for (size_t i = 0; i < m_source_pool.size(); i++) {
        if (m_source_available[i] && m_source_pool[i] != 0) {
            m_source_available[i] = false;
            return m_source_pool[i];
        }
    }
    return 0;
}

void AudioMixer::ReleaseSource(ALuint source)
{
    for (size_t i = 0; i < m_source_pool.size(); i++) {
        if (m_source_pool[i] == source) {
            m_source_available[i] = true;
            // Reset source state
            alSourcei(source, AL_BUFFER, 0);
            alSourcef(source, AL_GAIN, 1.0f);
            alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
            alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
            alSourcef(source, AL_PITCH, 1.0f);
            alSourcei(source, AL_LOOPING, AL_FALSE);
            return;
        }
    }
}

void AudioMixer::StealLowPriorityChannel(uint32_t sample_id, float priority)
{
    auto lowest_priority = m_active_sounds.end();
    float lowest_priority_value = 1.0f;

    for (auto it = m_active_sounds.begin(); it != m_active_sounds.end(); ++it) {
        if (it->playing && !it->paused && it->priority < lowest_priority_value) {
            lowest_priority = it;
            lowest_priority_value = it->priority;
        }
    }

    if (lowest_priority != m_active_sounds.end() && priority > lowest_priority_value) {
        alSourceStop(lowest_priority->source);
        alSourcei(lowest_priority->source, AL_BUFFER, 0);
        ReleaseSource(lowest_priority->source);
        
        if (m_sound_finished_callback) {
            m_sound_finished_callback(lowest_priority->sound_id);
        }
        
        m_active_sounds.erase(lowest_priority);
        LOG_DEBUG("Stole low priority channel for sample " + std::to_string(sample_id));
    }
}

uint32_t AudioMixer::PlaySample(uint32_t sample_id, float volume, float pan, float priority, 
                               const glm::vec3& position, bool looping)
{
    if (!m_initialized) {
        LOG_ERROR("AudioMixer: Cannot play sample - not initialized");
        return 0;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    auto it = m_samples.find(sample_id);
    if (it == m_samples.end() || !it->second.loaded) {
        LOG_ERROR("AudioMixer: Sample not found or not loaded: " + std::to_string(sample_id));
        return 0;
    }

    // Increment reference count
    it->second.ref_count++;

    // Get available source or steal channel
    ALuint source = GetAvailableSource();
    if (source == 0) {
        StealLowPriorityChannel(sample_id, priority);
        source = GetAvailableSource();
        
        if (source == 0) {
            LOG_WARNING("AudioMixer: No available sources (channel stealing failed)");
            it->second.ref_count--;
            return 0;
        }
    }

    // Bind buffer to source
    alSourcei(source, AL_BUFFER, static_cast<ALint>(it->second.buffer));
    if (alGetError() != AL_NO_ERROR) {
        LOG_ERROR("AudioMixer: Failed to bind buffer to source");
        ReleaseSource(source);
        it->second.ref_count--;
        return 0;
    }

    // Set looping
    alSourcei(source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);

    // Apply volume
    float final_volume = GetEffectiveVolume(it->second.category);
    final_volume = std::clamp(volume * final_volume * 1.0f * 1.0f, 0.0f, 1.0f); // duck_volume defaults to 1.0
    alSourcef(source, AL_GAIN, final_volume);

    // Apply spatial audio
    if (m_mixer_config.enable_spatial_audio) {
        alSource3f(source, AL_POSITION, position.x, position.y, position.z);
        alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        alSourcef(source, AL_REFERENCE_DISTANCE, 1.0f);
        alSourcef(source, AL_MAX_DISTANCE, 100.0f);
        alSourcef(source, AL_ROLLOFF_FACTOR, 1.0f);
    } else {
        // Apply pan for non-spatial audio
        pan = std::clamp(pan, -1.0f, 1.0f);
        alSource3f(source, AL_POSITION, -pan, 0.0f, 0.0f);
    }

    // Play source
    alSourcePlay(source);
    if (alGetError() != AL_NO_ERROR) {
        LOG_ERROR("AudioMixer: Failed to play source");
        alSourcei(source, AL_BUFFER, 0);
        ReleaseSource(source);
        it->second.ref_count--;
        return 0;
    }

    // Track active sound
    ActiveSound sound;
    sound.sample_id = sample_id;
    sound.sound_id = ++m_sound_id_counter;
    sound.source = source;
    sound.instance_volume = volume;
    sound.priority = priority;
    sound.playing = true;
    sound.paused = false;
    sound.looping = looping;
    sound.position = position;
    sound.velocity = glm::vec3(0.0f);
    sound.category = it->second.category;
    sound.fade_volume = 1.0f;
    sound.fading_out = false;
    sound.fading_in = false;
    sound.duck_volume = 1.0f;
    sound.duck_target = 1.0f;
    sound.duck_attack_time = m_mixer_config.ducking_attack_time;
    sound.duck_release_time = m_mixer_config.ducking_release_time;
    sound.duck_elapsed = 0.0f;
    sound.is_ducking = false;
    
    m_active_sounds.push_back(sound);

    LOG_DEBUG("Playing sample: " + it->second.name + " (sound_id: " + std::to_string(sound.sound_id) + 
            ", volume: " + std::to_string(final_volume) + ", priority: " + std::to_string(priority) + ")");
    return sound.sound_id;
}

void AudioMixer::StopSound(uint32_t sound_id, uint32_t fade_out_ms)
{
    if (!m_initialized) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& sound : m_active_sounds) {
        if (sound.sound_id == sound_id && sound.playing && !sound.paused) {
            if (fade_out_ms > 0) {
                sound.fading_out = true;
                sound.fade_duration = fade_out_ms / 1000.0f;
                sound.fade_elapsed = 0.0f;
            } else {
                alSourceStop(sound.source);
                alSourcei(sound.source, AL_BUFFER, 0);
                ReleaseSource(sound.source);
                sound.playing = false;
                
                // Decrement reference count
                auto sample_it = m_samples.find(sound.sample_id);
                if (sample_it != m_samples.end()) {
                    sample_it->second.ref_count--;
                }
                
                if (m_sound_finished_callback) {
                    m_sound_finished_callback(sound_id);
                }
            }
            return;
        }
    }
}

void AudioMixer::PauseSound(uint32_t sound_id)
{
    if (!m_initialized) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& sound : m_active_sounds) {
        if (sound.sound_id == sound_id && sound.playing && !sound.paused) {
            alSourcePause(sound.source);
            sound.paused = true;
            LOG_DEBUG("Paused sound: " + std::to_string(sound_id));
            return;
        }
    }
}

void AudioMixer::ResumeSound(uint32_t sound_id)
{
    if (!m_initialized) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& sound : m_active_sounds) {
        if (sound.sound_id == sound_id && sound.playing && sound.paused) {
            alSourcePlay(sound.source);
            sound.paused = false;
            LOG_DEBUG("Resumed sound: " + std::to_string(sound_id));
            return;
        }
    }
}

void AudioMixer::SetSoundVolume(uint32_t sound_id, float volume, uint32_t fade_ms)
{
    if (!m_initialized) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& sound : m_active_sounds) {
        if (sound.sound_id == sound_id && sound.playing) {
            if (fade_ms > 0) {
                sound.fading_in = true;
                sound.fade_duration = fade_ms / 1000.0f;
                sound.fade_elapsed = 0.0f;
                sound.instance_volume = volume;
            } else {
                sound.instance_volume = volume;
                float final_volume = GetEffectiveVolume(sound.category);
                final_volume = std::clamp(volume * final_volume * sound.fade_volume * sound.duck_volume, 0.0f, 1.0f);
                alSourcef(sound.source, AL_GAIN, final_volume);
            }
            return;
        }
    }
}

void AudioMixer::SetSoundPosition(uint32_t sound_id, const glm::vec3& position)
{
    if (!m_initialized || !m_mixer_config.enable_spatial_audio) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& sound : m_active_sounds) {
        if (sound.sound_id == sound_id && sound.playing) {
            sound.position = position;
            alSource3f(sound.source, AL_POSITION, position.x, position.y, position.z);
            return;
        }
    }
}

void AudioMixer::SetSoundVelocity(uint32_t sound_id, const glm::vec3& velocity)
{
    if (!m_initialized || !m_mixer_config.enable_spatial_audio) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& sound : m_active_sounds) {
        if (sound.sound_id == sound_id && sound.playing) {
            sound.velocity = velocity;
            alSource3f(sound.source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
            return;
        }
    }
}

void AudioMixer::StopCategory(AudioCategory category, uint32_t fade_out_ms)
{
    if (!m_initialized) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto& sound : m_active_sounds) {
        if (sound.category == category && sound.playing && !sound.paused) {
            StopSound(sound.sound_id, fade_out_ms);
        }
    }
}

void AudioMixer::SetCategoryVolume(AudioCategory category, float volume)
{
    m_category_volumes[category] = std::clamp(volume, 0.0f, 1.0f);
    
    if (!m_initialized) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    // Update all active sounds in this category
    for (auto& sound : m_active_sounds) {
        if (sound.category == category && sound.playing) {
            float final_volume = GetEffectiveVolume(category);
            final_volume = std::clamp(sound.instance_volume * final_volume * sound.fade_volume * sound.duck_volume, 0.0f, 1.0f);
            alSourcef(sound.source, AL_GAIN, final_volume);
        }
    }
}

float AudioMixer::GetCategoryVolume(AudioCategory category) const
{
    auto it = m_category_volumes.find(category);
    if (it != m_category_volumes.end()) {
        return it->second;
    }
    return 1.0f;
}

void AudioMixer::SetMasterVolume(float volume, uint32_t fade_ms)
{
    m_master_volume_target = std::clamp(volume, 0.0f, 1.0f);
    
    if (fade_ms > 0) {
        m_master_volume_fade_speed = (m_master_volume_target - m_master_volume) / (fade_ms / 1000.0f);
    } else {
        m_master_volume = m_master_volume_target;
        m_master_volume_fade_speed = 0.0f;
        
        if (!m_initialized) {
            return;
        }

        std::lock_guard<std::mutex> lock(m_mutex);

        for (auto& sound : m_active_sounds) {
            if (sound.playing) {
                float final_volume = GetEffectiveVolume(sound.category);
                final_volume = std::clamp(sound.instance_volume * final_volume * sound.fade_volume * sound.duck_volume, 0.0f, 1.0f);
                alSourcef(sound.source, AL_GAIN, final_volume);
            }
        }
    }
}

float AudioMixer::GetMasterVolume() const
{
    return m_master_volume;
}

void AudioMixer::SetListenerPosition(const glm::vec3& position)
{
    if (!m_initialized || !m_mixer_config.enable_spatial_audio) {
        return;
    }

    m_listener_position = position;
    alListener3f(AL_POSITION, position.x, position.y, position.z);
}

void AudioMixer::SetListenerVelocity(const glm::vec3& velocity)
{
    if (!m_initialized || !m_mixer_config.enable_spatial_audio) {
        return;
    }

    m_listener_velocity = velocity;
    alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}

void AudioMixer::SetListenerOrientation(const glm::vec3& forward, const glm::vec3& up)
{
    if (!m_initialized || !m_mixer_config.enable_spatial_audio) {
        return;
    }

    m_listener_forward = forward;
    m_listener_up = up;
    
    ALfloat orientation[6] = {
        forward.x, forward.y, forward.z,
        up.x, up.y, up.z
    };
    alListenerfv(AL_ORIENTATION, orientation);
}

void AudioMixer::TriggerDucking(AudioCategory category, AudioCategory trigger_category)
{
    if (!m_initialized || !m_mixer_config.enable_ducking) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    // Mark this category as being ducked by the trigger category
    m_ducking_triggers[category] = trigger_category;
    m_category_ducking_state[category] = true;

    // Start ducking all sounds in this category
    for (auto& sound : m_active_sounds) {
        if (sound.category == category && sound.playing && !sound.paused) {
            sound.duck_target = m_mixer_config.ducking_volume;
            sound.duck_attack_time = m_mixer_config.ducking_attack_time;
            sound.duck_elapsed = 0.0f;
            sound.is_ducking = true;
        }
    }

    LOG_DEBUG("Ducking triggered for category " + std::to_string(static_cast<int>(category)) + 
              " by " + std::to_string(static_cast<int>(trigger_category)));
}

void AudioMixer::ReleaseDucking(AudioCategory category)
{
    if (!m_initialized || !m_mixer_config.enable_ducking) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    // Release ducking for this category
    m_ducking_triggers.erase(category);
    m_category_ducking_state[category] = false;

    // Release ducking for all sounds in this category
    for (auto& sound : m_active_sounds) {
        if (sound.category == category && sound.playing && !sound.paused) {
            sound.duck_target = 1.0f;
            sound.duck_release_time = m_mixer_config.ducking_release_time;
            sound.duck_elapsed = 0.0f;
            sound.is_ducking = true;
        }
    }

    LOG_DEBUG("Ducking released for category " + std::to_string(static_cast<int>(category)));
}

void AudioMixer::Update(float delta_time)
{
    if (!m_initialized) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    // Update master volume fade
    if (m_master_volume_fade_speed != 0.0f) {
        m_master_volume += m_master_volume_fade_speed * delta_time;
        
        if ((m_master_volume_fade_speed > 0.0f && m_master_volume >= m_master_volume_target) ||
            (m_master_volume_fade_speed < 0.0f && m_master_volume <= m_master_volume_target)) {
            m_master_volume = m_master_volume_target;
            m_master_volume_fade_speed = 0.0f;
        }
        
        // Update all active sounds
        for (auto& sound : m_active_sounds) {
            if (sound.playing) {
                float final_volume = GetEffectiveVolume(sound.category);
                final_volume = std::clamp(sound.instance_volume * final_volume * sound.fade_volume * sound.duck_volume, 0.0f, 1.0f);
                alSourcef(sound.source, AL_GAIN, final_volume);
            }
        }
    }

    // Update fades
    UpdateFades(delta_time);

    // Update ducking
    if (m_mixer_config.enable_ducking) {
        UpdateDucking(delta_time);
    }

    // Update spatial audio
    if (m_mixer_config.enable_spatial_audio) {
        UpdateSpatialAudio();
    }

    // Cleanup finished sounds
    CleanupFinishedSounds();
}

void AudioMixer::UpdateFades(float delta_time)
{
    for (auto it = m_active_sounds.begin(); it != m_active_sounds.end(); ) {
        if (it->fading_out) {
            it->fade_elapsed += delta_time;
            it->fade_volume = 1.0f - (it->fade_elapsed / it->fade_duration);
            
            if (it->fade_volume <= 0.0f) {
                alSourceStop(it->source);
                alSourcei(it->source, AL_BUFFER, 0);
                ReleaseSource(it->source);
                it->playing = false;
                
                auto sample_it = m_samples.find(it->sample_id);
                if (sample_it != m_samples.end()) {
                    sample_it->second.ref_count--;
                }
                
                if (m_sound_finished_callback) {
                    m_sound_finished_callback(it->sound_id);
                }
                
                it = m_active_sounds.erase(it);
                continue;
            } else {
                float final_volume = GetEffectiveVolume(it->category);
                final_volume = std::clamp(it->instance_volume * final_volume * it->fade_volume * it->duck_volume, 0.0f, 1.0f);
                alSourcef(it->source, AL_GAIN, final_volume);
            }
        } else if (it->fading_in) {
            it->fade_elapsed += delta_time;
            it->fade_volume = it->fade_elapsed / it->fade_duration;
            
            if (it->fade_volume >= 1.0f) {
                it->fade_volume = 1.0f;
                it->fading_in = false;
            }
            
            float final_volume = GetEffectiveVolume(it->category);
            final_volume = std::clamp(it->instance_volume * final_volume * it->fade_volume * it->duck_volume, 0.0f, 1.0f);
            alSourcef(it->source, AL_GAIN, final_volume);
        }
        ++it;
    }
}

void AudioMixer::UpdateDucking(float delta_time)
{
    for (auto& sound : m_active_sounds) {
        if (!sound.playing || sound.paused || !sound.is_ducking) {
            continue;
        }

        // Check if this category should be ducked
        auto trigger_it = m_ducking_triggers.find(sound.category);
        bool should_duck = (trigger_it != m_ducking_triggers.end());

        // Determine target duck volume
        float target_volume = should_duck ? m_mixer_config.ducking_volume : 1.0f;
        float transition_time = should_duck ? sound.duck_attack_time : sound.duck_release_time;

        // Update duck volume
        if (transition_time > 0.0f) {
            sound.duck_elapsed += delta_time;
            float progress = std::min(sound.duck_elapsed / transition_time, 1.0f);
            
            if (should_duck) {
                // Attack phase: volume goes from 1.0 to ducking_volume
                sound.duck_volume = 1.0f - (1.0f - m_mixer_config.ducking_volume) * progress;
            } else {
                // Release phase: volume goes from ducking_volume to 1.0
                sound.duck_volume = m_mixer_config.ducking_volume + (1.0f - m_mixer_config.ducking_volume) * progress;
            }

            // Check if transition is complete
            if (progress >= 1.0f) {
                sound.duck_volume = target_volume;
                sound.is_ducking = false;
                sound.duck_elapsed = 0.0f;
            }
        } else {
            // Instant transition
            sound.duck_volume = target_volume;
            sound.is_ducking = false;
        }

        // Apply duck volume
        float final_volume = GetEffectiveVolume(sound.category);
        final_volume = std::clamp(sound.instance_volume * final_volume * sound.fade_volume * sound.duck_volume, 0.0f, 1.0f);
        alSourcef(sound.source, AL_GAIN, final_volume);
    }
}

void AudioMixer::UpdateSpatialAudio()
{
    // Update listener
    alListener3f(AL_POSITION, m_listener_position.x, m_listener_position.y, m_listener_position.z);
    alListener3f(AL_VELOCITY, m_listener_velocity.x, m_listener_velocity.y, m_listener_velocity.z);
    
    ALfloat orientation[6] = {
        m_listener_forward.x, m_listener_forward.y, m_listener_forward.z,
        m_listener_up.x, m_listener_up.y, m_listener_up.z
    };
    alListenerfv(AL_ORIENTATION, orientation);

    // Update sound positions and velocities
    for (auto& sound : m_active_sounds) {
        if (sound.playing && !sound.paused) {
            alSource3f(sound.source, AL_POSITION, sound.position.x, sound.position.y, sound.position.z);
            alSource3f(sound.source, AL_VELOCITY, sound.velocity.x, sound.velocity.y, sound.velocity.z);
        }
    }
}

float AudioMixer::GetEffectiveVolume(AudioCategory category) const
{
    float category_volume = GetCategoryVolume(category);
    return m_master_volume * category_volume;
}

void AudioMixer::CleanupFinishedSounds()
{
    for (auto it = m_active_sounds.begin(); it != m_active_sounds.end(); ) {
        if (it->playing && !it->paused && !it->fading_out) {
            ALint state;
            alGetSourcei(it->source, AL_SOURCE_STATE, &state);

            if (state == AL_STOPPED) {
                alSourcei(it->source, AL_BUFFER, 0);
                ReleaseSource(it->source);
                it->playing = false;
                
                auto sample_it = m_samples.find(it->sample_id);
                if (sample_it != m_samples.end()) {
                    sample_it->second.ref_count--;
                }
                
                if (m_sound_finished_callback) {
                    m_sound_finished_callback(it->sound_id);
                }
                
                it = m_active_sounds.erase(it);
                continue;
            }
        }
        ++it;
    }
}

bool AudioMixer::IsSoundFinished(const ActiveSound& sound) const
{
    if (!sound.playing || sound.paused) {
        return false;
    }

    ALint state;
    alGetSourcei(sound.source, AL_SOURCE_STATE, &state);
    return state == AL_STOPPED;
}

bool AudioMixer::IsInitialized() const
{
    return m_initialized;
}

size_t AudioMixer::GetActiveSoundCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_active_sounds.size();
}

size_t AudioMixer::GetLoadedSampleCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_samples.size();
}

void AudioMixer::SetSoundFinishedCallback(std::function<void(uint32_t sound_id)> callback)
{
    m_sound_finished_callback = std::move(callback);
}

} // namespace Audio
} // namespace Poko
