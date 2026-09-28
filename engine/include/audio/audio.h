/**
 * @file audio.h
 * @brief Audio system for Poko Engine
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#ifndef POKO_AUDIO_H
#define POKO_AUDIO_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum AudioFormat
 * @brief Audio sample formats
 */
typedef enum {
    AUDIO_FORMAT_U8,
    AUDIO_FORMAT_S16,
    AUDIO_FORMAT_S32,
    AUDIO_FORMAT_FLOAT,
    AUDIO_FORMAT_COUNT
} AudioFormat;

/**
 * @enum AudioChannelLayout
 * @brief Audio channel layouts
 */
typedef enum {
    AUDIO_CHANNEL_MONO,
    AUDIO_CHANNEL_STEREO,
    AUDIO_CHANNEL_2_1,
    AUDIO_CHANNEL_QUAD,
    AUDIO_CHANNEL_5_1,
    AUDIO_CHANNEL_7_1,
    AUDIO_CHANNEL_COUNT
} AudioChannelLayout;

/**
 * @enum AudioPlaybackState
 * @brief Audio playback states
 */
typedef enum {
    AUDIO_STATE_STOPPED,
    AUDIO_STATE_PLAYING,
    AUDIO_STATE_PAUSED,
    AUDIO_STATE_COUNT
} AudioPlaybackState;

/**
 * @struct AudioBuffer
 * @brief Audio buffer handle
 */
typedef struct AudioBuffer AudioBuffer;

/**
 * @struct AudioSource
 * @brief Audio source handle
 */
typedef struct AudioSource AudioSource;

/**
 * @struct AudioListener
 * @brief Audio listener handle
 */
typedef struct AudioListener AudioListener;

/**
 * @struct AudioWorld
 * @brief Audio world/system handle
 */
typedef struct AudioWorld AudioWorld;

/**
 * @brief Create audio world
 * @return Audio world handle
 */
AudioWorld* audio_world_create(void);

/**
 * @brief Destroy audio world
 * @param world Audio world to destroy
 */
void audio_world_destroy(AudioWorld* world);

/**
 * @brief Set master volume
 * @param world Audio world
 * @param volume Volume (0.0 to 1.0)
 */
void audio_world_set_master_volume(AudioWorld* world, float volume);

/**
 * @brief Get master volume
 * @param world Audio world
 * @return Volume
 */
float audio_world_get_master_volume(const AudioWorld* world);

/**
 * @brief Update audio world
 * @param world Audio world
 * @param delta_time Time step in seconds
 */
void audio_world_update(AudioWorld* world, float delta_time);

/**
 * @brief Create audio buffer from data
 * @param world Audio world
 * @param data Sample data
 * @param size Data size in bytes
 * @param format Sample format
 * @param channels Channel layout
 * @param sample_rate Sample rate in Hz
 * @return Audio buffer handle
 */
AudioBuffer* audio_buffer_create(AudioWorld* world, const void* data, size_t size, AudioFormat format, AudioChannelLayout channels, int sample_rate);

/**
 * @brief Destroy audio buffer
 * @param buffer Audio buffer to destroy
 */
void audio_buffer_destroy(AudioBuffer* buffer);

/**
 * @brief Get buffer duration
 * @param buffer Audio buffer
 * @return Duration in seconds
 */
float audio_buffer_get_duration(const AudioBuffer* buffer);

/**
 * @brief Create audio source
 * @param world Audio world
 * @return Audio source handle
 */
AudioSource* audio_source_create(AudioWorld* world);

/**
 * @brief Destroy audio source
 * @param source Audio source to destroy
 */
void audio_source_destroy(AudioSource* source);

/**
 * @brief Play audio
 * @param source Audio source
 * @param buffer Audio buffer to play
 * @param loop Loop playback
 */
void audio_source_play(AudioSource* source, AudioBuffer* buffer, bool loop);

/**
 * @brief Stop audio
 * @param source Audio source
 */
void audio_source_stop(AudioSource* source);

/**
 * @brief Pause audio
 * @param source Audio source
 */
void audio_source_pause(AudioSource* source);

/**
 * @brief Resume audio
 * @param source Audio source
 */
void audio_source_resume(AudioSource* source);

/**
 * @brief Get playback state
 * @param source Audio source
 * @return Playback state
 */
AudioPlaybackState audio_source_get_state(const AudioSource* source);

/**
 * @brief Set source volume
 * @param source Audio source
 * @param volume Volume (0.0 to 1.0)
 */
void audio_source_set_volume(AudioSource* source, float volume);

/**
 * @brief Get source volume
 * @param source Audio source
 * @return Volume
 */
float audio_source_get_volume(const AudioSource* source);

/**
 * @brief Set source pitch
 * @param source Audio source
 * @param pitch Pitch multiplier (1.0 = normal)
 */
void audio_source_set_pitch(AudioSource* source, float pitch);

/**
 * @brief Get source pitch
 * @param source Audio source
 * @return Pitch
 */
float audio_source_get_pitch(const AudioSource* source);

/**
 * @brief Set source pan
 * @param source Audio source
 * @param pan Pan (-1.0 left to 1.0 right)
 */
void audio_source_set_pan(AudioSource* source, float pan);

/**
 * @brief Get source pan
 * @param source Audio source
 * @return Pan
 */
float audio_source_get_pan(const AudioSource* source);

/**
 * @brief Set source position (3D)
 * @param source Audio source
 * @param x Position X
 * @param y Position Y
 * @param z Position Z
 */
void audio_source_set_position(AudioSource* source, float x, float y, float z);

/**
 * @brief Get source position
 * @param source Audio source
 * @param x Output position X
 * @param y Output position Y
 * @param z Output position Z
 */
void audio_source_get_position(const AudioSource* source, float* x, float* y, float* z);

/**
 * @brief Set source velocity
 * @param source Audio source
 * @param x Velocity X
 * @param y Velocity Y
 * @param z Velocity Z
 */
void audio_source_set_velocity(AudioSource* source, float x, float y, float z);

/**
 * @brief Get source velocity
 * @param source Audio source
 * @param x Output velocity X
 * @param y Output velocity Y
 * @param z Output velocity Z
 */
void audio_source_get_velocity(const AudioSource* source, float* x, float* y, float* z);

/**
 * @brief Set source direction
 * @param source Audio source
 * @param x Direction X
 * @param y Direction Y
 * @param z Direction Z
 */
void audio_source_set_direction(AudioSource* source, float x, float y, float z);

/**
 * @brief Get source direction
 * @param source Audio source
 * @param x Output direction X
 * @param y Output direction Y
 * @param z Output direction Z
 */
void audio_source_get_direction(const AudioSource* source, float* x, float* y, float* z);

/**
 * @brief Set source min distance
 * @param source Audio source
 * @param distance Minimum distance for attenuation
 */
void audio_source_set_min_distance(AudioSource* source, float distance);

/**
 * @brief Get source min distance
 * @param source Audio source
 * @return Minimum distance
 */
float audio_source_get_min_distance(const AudioSource* source);

/**
 * @brief Set source max distance
 * @param source Audio source
 * @param distance Maximum distance for attenuation
 */
void audio_source_set_max_distance(AudioSource* source, float distance);

/**
 * @brief Get source max distance
 * @param source Audio source
 * @return Maximum distance
 */
float audio_source_get_max_distance(const AudioSource* source);

/**
 * @brief Set source rolloff factor
 * @param source Audio source
 * @param factor Rolloff factor
 */
void audio_source_set_rolloff_factor(AudioSource* source, float factor);

/**
 * @brief Get source rolloff factor
 * @param source Audio source
 * @return Rolloff factor
 */
float audio_source_get_rolloff_factor(const AudioSource* source);

/**
 * @brief Create audio listener
 * @param world Audio world
 * @return Audio listener handle
 */
AudioListener* audio_listener_create(AudioWorld* world);

/**
 * @brief Destroy audio listener
 * @param listener Audio listener to destroy
 */
void audio_listener_destroy(AudioListener* listener);

/**
 * @brief Set listener position
 * @param listener Audio listener
 * @param x Position X
 * @param y Position Y
 * @param z Position Z
 */
void audio_listener_set_position(AudioListener* listener, float x, float y, float z);

/**
 * @brief Get listener position
 * @param listener Audio listener
 * @param x Output position X
 * @param y Output position Y
 * @param z Output position Z
 */
void audio_listener_get_position(const AudioListener* listener, float* x, float* y, float* z);

/**
 * @brief Set listener orientation
 * @param listener Audio listener
 * @param at_x At direction X
 * @param at_y At direction Y
 * @param at_z At direction Z
 * @param up_x Up direction X
 * @param up_y Up direction Y
 * @param up_z Up direction Z
 */
void audio_listener_set_orientation(AudioListener* listener, float at_x, float at_y, float at_z, float up_x, float up_y, float up_z);

/**
 * @brief Get listener orientation
 * @param listener Audio listener
 * @param at_x Output at direction X
 * @param at_y Output at direction Y
 * @param at_z Output at direction Z
 * @param up_x Output up direction X
 * @param up_y Output up direction Y
 * @param up_z Output up direction Z
 */
void audio_listener_get_orientation(const AudioListener* listener, float* at_x, float* at_y, float* at_z, float* up_x, float* up_y, float* up_z);

/**
 * @brief Set listener velocity
 * @param listener Audio listener
 * @param x Velocity X
 * @param y Velocity Y
 * @param z Velocity Z
 */
void audio_listener_set_velocity(AudioListener* listener, float x, float y, float z);

/**
 * @brief Get listener velocity
 * @param listener Audio listener
 * @param x Output velocity X
 * @param y Output velocity Y
 * @param z Output velocity Z
 */
void audio_listener_get_velocity(const AudioListener* listener, float* x, float* y, float* z);

/**
 * @brief Set listener gain
 * @param listener Audio listener
 * @param gain Gain multiplier
 */
void audio_listener_set_gain(AudioListener* listener, float gain);

/**
 * @brief Get listener gain
 * @param listener Audio listener
 * @return Gain
 */
float audio_listener_get_gain(const AudioListener* listener);

#ifdef __cplusplus
}
#endif

#endif // POKO_AUDIO_H
