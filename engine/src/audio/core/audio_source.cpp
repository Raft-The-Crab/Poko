/**
 * @file audio_source.cpp
 * @brief Audio source management
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "audio/audio.h"
#include "core/logging/logger.h"
#include <cstring>
#include <cmath>

namespace poko {
namespace audio {

// Audio source implementation
struct AudioSourceImpl {
    uint64_t id;
    AudioBuffer* buffer;
    AudioPlaybackState state;
    float volume;
    float pitch;
    float pan;
    float position[3];
    float velocity[3];
    float direction[3];
    float min_distance;
    float max_distance;
    float rolloff_factor;
    bool is_looping;
    float current_time;
};

// Convert to internal implementation
static AudioSourceImpl* to_source_impl(AudioSource* source) {
    return reinterpret_cast<AudioSourceImpl*>(source);
}

static const AudioSourceImpl* to_source_impl(const AudioSource* source) {
    return reinterpret_cast<const AudioSourceImpl*>(source);
}

static AudioSource* from_source_impl(AudioSourceImpl* impl) {
    return reinterpret_cast<AudioSource*>(impl);
}

} // namespace audio
} // namespace poko

// C API implementation
extern "C" {

using namespace poko::audio;

AudioSource* audio_source_create(AudioWorld* world) {
    if (!world) return NULL;
    
    AudioSourceImpl* source = new AudioSourceImpl();
    source->id = 0; // Will be set by world
    source->buffer = NULL;
    source->state = AUDIO_STATE_STOPPED;
    source->volume = 1.0f;
    source->pitch = 1.0f;
    source->pan = 0.0f;
    source->position[0] = 0.0f;
    source->position[1] = 0.0f;
    source->position[2] = 0.0f;
    source->velocity[0] = 0.0f;
    source->velocity[1] = 0.0f;
    source->velocity[2] = 0.0f;
    source->direction[0] = 0.0f;
    source->direction[1] = 0.0f;
    source->direction[2] = 0.0f;
    source->min_distance = 1.0f;
    source->max_distance = 100.0f;
    source->rolloff_factor = 1.0f;
    source->is_looping = false;
    source->current_time = 0.0f;
    
    POKO_LOG_DEBUG("Audio: Source created with ID " + std::to_string(source->id));
    return from_source_impl(source);
}

void audio_source_destroy(AudioSource* source) {
    if (!source) return;
    
    AudioSourceImpl* impl = to_source_impl(source);
    delete impl;
    
    POKO_LOG_DEBUG("Audio: Source destroyed");
}

void audio_source_play(AudioSource* source, AudioBuffer* buffer, bool loop) {
    if (!source || !buffer) return;
    
    AudioSourceImpl* impl = to_source_impl(source);
    
    impl->buffer = buffer;
    impl->is_looping = loop;
    impl->state = AUDIO_STATE_PLAYING;
    impl->current_time = 0.0f;
    
    POKO_LOG_INFO("Audio: Playing audio - Loop: " + std::to_string(loop));
}

void audio_source_stop(AudioSource* source) {
    if (!source) return;
    
    AudioSourceImpl* impl = to_source_impl(source);
    impl->state = AUDIO_STATE_STOPPED;
    impl->current_time = 0.0f;
    
    POKO_LOG_DEBUG("Audio: Source stopped");
}

void audio_source_pause(AudioSource* source) {
    if (!source) return;
    
    AudioSourceImpl* impl = to_source_impl(source);
    if (impl->state == AUDIO_STATE_PLAYING) {
        impl->state = AUDIO_STATE_PAUSED;
    }
    
    POKO_LOG_DEBUG("Audio: Source paused");
}

void audio_source_resume(AudioSource* source) {
    if (!source) return;
    
    AudioSourceImpl* impl = to_source_impl(source);
    if (impl->state == AUDIO_STATE_PAUSED) {
        impl->state = AUDIO_STATE_PLAYING;
    }
    
    POKO_LOG_DEBUG("Audio: Source resumed");
}

AudioPlaybackState audio_source_get_state(const AudioSource* source) {
    if (!source) return AUDIO_STATE_STOPPED;
    
    const AudioSourceImpl* impl = to_source_impl(source);
    return impl->state;
}

void audio_source_set_volume(AudioSource* source, float volume) {
    if (!source) return;
    
    AudioSourceImpl* impl = to_source_impl(source);
    impl->volume = std::max(0.0f, std::min(1.0f, volume));
    
    POKO_LOG_DEBUG("Audio: Source volume set to " + std::to_string(impl->volume));
}

float audio_source_get_volume(const AudioSource* source) {
    if (!source) return 1.0f;
    
    const AudioSourceImpl* impl = to_source_impl(source);
    return impl->volume;
}

void audio_source_set_pitch(AudioSource* source, float pitch) {
    if (!source) return;
    
    AudioSourceImpl* impl = to_source_impl(source);
    impl->pitch = std::max(0.1f, std::min(10.0f, pitch));
    
    POKO_LOG_DEBUG("Audio: Source pitch set to " + std::to_string(impl->pitch));
}

float audio_source_get_pitch(const AudioSource* source) {
    if (!source) return 1.0f;
    
    const AudioSourceImpl* impl = to_source_impl(source);
    return impl->pitch;
}

void audio_source_set_pan(AudioSource* source, float pan) {
    if (!source) return;
    
    AudioSourceImpl* impl = to_source_impl(source);
    impl->pan = std::max(-1.0f, std::min(1.0f, pan));
    
    POKO_LOG_DEBUG("Audio: Source pan set to " + std::to_string(impl->pan));
}

float audio_source_get_pan(const AudioSource* source) {
    if (!source) return 0.0f;
    
    const AudioSourceImpl* impl = to_source_impl(source);
    return impl->pan;
}

void audio_source_set_position(AudioSource* source, float x, float y, float z) {
    if (!source) return;
    
    AudioSourceImpl* impl = to_source_impl(source);
    impl->position[0] = x;
    impl->position[1] = y;
    impl->position[2] = z;
}

void audio_source_get_position(const AudioSource* source, float* x, float* y, float* z) {
    if (!source) return;
    
    const AudioSourceImpl* impl = to_source_impl(source);
    if (x) *x = impl->position[0];
    if (y) *y = impl->position[1];
    if (z) *z = impl->position[2];
}

void audio_source_set_velocity(AudioSource* source, float x, float y, float z) {
    if (!source) return;
    
    AudioSourceImpl* impl = to_source_impl(source);
    impl->velocity[0] = x;
    impl->velocity[1] = y;
    impl->velocity[2] = z;
}

void audio_source_get_velocity(const AudioSource* source, float* x, float* y, float* z) {
    if (!source) return;
    
    const AudioSourceImpl* impl = to_source_impl(source);
    if (x) *x = impl->velocity[0];
    if (y) *y = impl->velocity[1];
    if (z) *z = impl->velocity[2];
}

void audio_source_set_direction(AudioSource* source, float x, float y, float z) {
    if (!source) return;
    
    AudioSourceImpl* impl = to_source_impl(source);
    impl->direction[0] = x;
    impl->direction[1] = y;
    impl->direction[2] = z;
}

void audio_source_get_direction(const AudioSource* source, float* x, float* y, float* z) {
    if (!source) return;
    
    const AudioSourceImpl* impl = to_source_impl(source);
    if (x) *x = impl->direction[0];
    if (y) *y = impl->direction[1];
    if (z) *z = impl->direction[2];
}

void audio_source_set_min_distance(AudioSource* source, float distance) {
    if (!source) return;
    
    AudioSourceImpl* impl = to_source_impl(source);
    impl->min_distance = std::max(0.1f, distance);
}

float audio_source_get_min_distance(const AudioSource* source) {
    if (!source) return 1.0f;
    
    const AudioSourceImpl* impl = to_source_impl(source);
    return impl->min_distance;
}

void audio_source_set_max_distance(AudioSource* source, float distance) {
    if (!source) return;
    
    AudioSourceImpl* impl = to_source_impl(source);
    impl->max_distance = std::max(impl->min_distance, distance);
}

float audio_source_get_max_distance(const AudioSource* source) {
    if (!source) return 100.0f;
    
    const AudioSourceImpl* impl = to_source_impl(source);
    return impl->max_distance;
}

void audio_source_set_rolloff_factor(AudioSource* source, float factor) {
    if (!source) return;
    
    AudioSourceImpl* impl = to_source_impl(source);
    impl->rolloff_factor = std::max(0.1f, factor);
}

float audio_source_get_rolloff_factor(const AudioSource* source) {
    if (!source) return 1.0f;
    
    const AudioSourceImpl* impl = to_source_impl(source);
    return impl->rolloff_factor;
}

} // extern "C"