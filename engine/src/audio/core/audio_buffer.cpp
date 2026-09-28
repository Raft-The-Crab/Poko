/**
 * @file audio_buffer.cpp
 * @brief Audio buffer management
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

// Audio buffer implementation
struct AudioBufferImpl {
    uint64_t id;
    void* data;
    size_t size;
    AudioFormat format;
    AudioChannelLayout channels;
    int sample_rate;
    float duration;
};

// Convert to internal implementation
static AudioBufferImpl* to_buffer_impl(AudioBuffer* buffer) {
    return reinterpret_cast<AudioBufferImpl*>(buffer);
}

static const AudioBufferImpl* to_buffer_impl(const AudioBuffer* buffer) {
    return reinterpret_cast<const AudioBufferImpl*>(buffer);
}

static AudioBuffer* from_buffer_impl(AudioBufferImpl* impl) {
    return reinterpret_cast<AudioBuffer*>(impl);
}

// Helper function to get bytes per sample
static int bytes_per_sample(AudioFormat format) {
    switch (format) {
        case AUDIO_FORMAT_U8: return 1;
        case AUDIO_FORMAT_S16: return 2;
        case AUDIO_FORMAT_S32: return 4;
        case AUDIO_FORMAT_FLOAT: return 4;
        default: return 2;
    }
}

// Helper function to get number of channels
static int num_channels(AudioChannelLayout channels) {
    switch (channels) {
        case AUDIO_CHANNEL_MONO: return 1;
        case AUDIO_CHANNEL_STEREO: return 2;
        case AUDIO_CHANNEL_2_1: return 3;
        case AUDIO_CHANNEL_QUAD: return 4;
        case AUDIO_CHANNEL_5_1: return 6;
        case AUDIO_CHANNEL_7_1: return 8;
        default: return 2;
    }
}

} // namespace audio
} // namespace poko

// C API implementation
extern "C" {

using namespace poko::audio;

AudioBuffer* audio_buffer_create(AudioWorld* world, const void* data, size_t size, AudioFormat format, AudioChannelLayout channels, int sample_rate) {
    if (!world || !data || size == 0) return NULL;
    
    AudioBufferImpl* buffer = new AudioBufferImpl();
    buffer->id = 0; // Will be set by world
    buffer->data = malloc(size);
    if (buffer->data) {
        memcpy(buffer->data, data, size);
    }
    buffer->size = size;
    buffer->format = format;
    buffer->channels = channels;
    buffer->sample_rate = sample_rate;
    
    // Calculate duration
    int sample_count = size / (bytes_per_sample(format) * num_channels(channels));
    buffer->duration = (float)sample_count / (float)sample_rate;
    
    POKO_LOG_INFO("Audio: Buffer created - Duration: " + std::to_string(buffer->duration) + "s, Format: " + 
                  std::to_string(format) + ", Channels: " + std::to_string(num_channels(channels)));
    
    return from_buffer_impl(buffer);
}

void audio_buffer_destroy(AudioBuffer* buffer) {
    if (!buffer) return;
    
    AudioBufferImpl* impl = to_buffer_impl(buffer);
    
    if (impl->data) {
        free(impl->data);
    }
    delete impl;
    
    POKO_LOG_DEBUG("Audio: Buffer destroyed");
}

float audio_buffer_get_duration(const AudioBuffer* buffer) {
    if (!buffer) return 0.0f;
    
    const AudioBufferImpl* impl = to_buffer_impl(buffer);
    return impl->duration;
}

} // extern "C"