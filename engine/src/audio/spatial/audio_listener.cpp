/**
 * @file audio_listener.cpp
 * @brief Audio listener management for spatial audio
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "audio/audio.h"
#include "core/logging/logger.h"
#include <cstring>

namespace poko {
namespace audio {

// Audio listener implementation
struct AudioListenerImpl {
    uint64_t id;
    float position[3];
    float orientation_at[3];
    float orientation_up[3];
    float velocity[3];
    float gain;
};

// Convert to internal implementation
static AudioListenerImpl* to_listener_impl(AudioListener* listener) {
    return reinterpret_cast<AudioListenerImpl*>(listener);
}

static const AudioListenerImpl* to_listener_impl(const AudioListener* listener) {
    return reinterpret_cast<const AudioListenerImpl*>(listener);
}

static AudioListener* from_listener_impl(AudioListenerImpl* impl) {
    return reinterpret_cast<AudioListener*>(impl);
}

} // namespace audio
} // namespace poko

// C API implementation
extern "C" {

using namespace poko::audio;

AudioListener* audio_listener_create(AudioWorld* world) {
    if (!world) return NULL;
    
    AudioListenerImpl* listener = new AudioListenerImpl();
    listener->id = 0; // Will be set by world
    listener->position[0] = 0.0f;
    listener->position[1] = 0.0f;
    listener->position[2] = 0.0f;
    listener->orientation_at[0] = 0.0f;
    listener->orientation_at[1] = 0.0f;
    listener->orientation_at[2] = -1.0f;
    listener->orientation_up[0] = 0.0f;
    listener->orientation_up[1] = 1.0f;
    listener->orientation_up[2] = 0.0f;
    listener->velocity[0] = 0.0f;
    listener->velocity[1] = 0.0f;
    listener->velocity[2] = 0.0f;
    listener->gain = 1.0f;
    
    POKO_LOG_DEBUG("Audio: Listener created with ID " + std::to_string(listener->id));
    return from_listener_impl(listener);
}

void audio_listener_destroy(AudioListener* listener) {
    if (!listener) return;
    
    AudioListenerImpl* impl = to_listener_impl(listener);
    delete impl;
    
    POKO_LOG_DEBUG("Audio: Listener destroyed");
}

void audio_listener_set_position(AudioListener* listener, float x, float y, float z) {
    if (!listener) return;
    
    AudioListenerImpl* impl = to_listener_impl(listener);
    impl->position[0] = x;
    impl->position[1] = y;
    impl->position[2] = z;
}

void audio_listener_get_position(const AudioListener* listener, float* x, float* y, float* z) {
    if (!listener) return;
    
    const AudioListenerImpl* impl = to_listener_impl(listener);
    if (x) *x = impl->position[0];
    if (y) *y = impl->position[1];
    if (z) *z = impl->position[2];
}

void audio_listener_set_orientation(AudioListener* listener, float at_x, float at_y, float at_z, float up_x, float up_y, float up_z) {
    if (!listener) return;
    
    AudioListenerImpl* impl = to_listener_impl(listener);
    impl->orientation_at[0] = at_x;
    impl->orientation_at[1] = at_y;
    impl->orientation_at[2] = at_z;
    impl->orientation_up[0] = up_x;
    impl->orientation_up[1] = up_y;
    impl->orientation_up[2] = up_z;
}

void audio_listener_get_orientation(const AudioListener* listener, float* at_x, float* at_y, float* at_z, float* up_x, float* up_y, float* up_z) {
    if (!listener) return;
    
    const AudioListenerImpl* impl = to_listener_impl(listener);
    if (at_x) *at_x = impl->orientation_at[0];
    if (at_y) *at_y = impl->orientation_at[1];
    if (at_z) *at_z = impl->orientation_at[2];
    if (up_x) *up_x = impl->orientation_up[0];
    if (up_y) *up_y = impl->orientation_up[1];
    if (up_z) *up_z = impl->orientation_up[2];
}

void audio_listener_set_velocity(AudioListener* listener, float x, float y, float z) {
    if (!listener) return;
    
    AudioListenerImpl* impl = to_listener_impl(listener);
    impl->velocity[0] = x;
    impl->velocity[1] = y;
    impl->velocity[2] = z;
}

void audio_listener_get_velocity(const AudioListener* listener, float* x, float* y, float* z) {
    if (!listener) return;
    
    const AudioListenerImpl* impl = to_listener_impl(listener);
    if (x) *x = impl->velocity[0];
    if (y) *y = impl->velocity[1];
    if (z) *z = impl->velocity[2];
}

void audio_listener_set_gain(AudioListener* listener, float gain) {
    if (!listener) return;
    
    AudioListenerImpl* impl = to_listener_impl(listener);
    impl->gain = std::max(0.0f, std::min(1.0f, gain));
}

float audio_listener_get_gain(const AudioListener* listener) {
    if (!listener) return 1.0f;
    
    const AudioListenerImpl* impl = to_listener_impl(listener);
    return impl->gain;
}

} // extern "C"