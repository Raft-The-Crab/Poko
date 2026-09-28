/**
 * @file animation_playback.cpp
 * @brief Animation playback control
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "animation/animation.h"
#include "core/logging/logger.h"
#include <cmath>

namespace poko {
namespace animation {

// Forward declaration
struct AnimationImpl {
    std::string name;
    float duration;
    float current_time;
    float speed;
    AnimationState state;
    AnimationMode mode;
    bool is_finished;
    void (*completion_callback)(void*);
    void* callback_user_data;
    bool ping_pong_direction;
};

// Convert to internal implementation
static AnimationImpl* to_animation_impl(Animation* animation) {
    return reinterpret_cast<AnimationImpl*>(animation);
}

static const AnimationImpl* to_animation_impl(const Animation* animation) {
    return reinterpret_cast<const AnimationImpl*>(animation);
}

} // namespace animation
} // namespace poko

// C API implementation
extern "C" {

using namespace poko::animation;

bool animation_play(Animation* animation, float speed) {
    if (!animation) return false;
    
    AnimationImpl* impl = to_animation_impl(animation);
    impl->speed = std::max(0.1f, speed);
    impl->state = ANIMATION_STATE_PLAYING;
    impl->is_finished = false;
    impl->ping_pong_direction = true;
    
    POKO_LOG_DEBUG("Animation: Playing '" + impl->name + "' with speed " + std::to_string(impl->speed));
    return true;
}

bool animation_stop(Animation* animation) {
    if (!animation) return false;
    
    AnimationImpl* impl = to_animation_impl(animation);
    impl->state = ANIMATION_STATE_STOPPED;
    impl->current_time = 0.0f;
    impl->is_finished = false;
    
    POKO_LOG_DEBUG("Animation: Stopped '" + impl->name + "'");
    return true;
}

bool animation_pause(Animation* animation) {
    if (!animation) return false;
    
    AnimationImpl* impl = to_animation_impl(animation);
    if (impl->state == ANIMATION_STATE_PLAYING) {
        impl->state = ANIMATION_STATE_PAUSED;
        POKO_LOG_DEBUG("Animation: Paused '" + impl->name + "'");
        return true;
    }
    return false;
}

bool animation_resume(Animation* animation) {
    if (!animation) return false;
    
    AnimationImpl* impl = to_animation_impl(animation);
    if (impl->state == ANIMATION_STATE_PAUSED) {
        impl->state = ANIMATION_STATE_PLAYING;
        POKO_LOG_DEBUG("Animation: Resumed '" + impl->name + "'");
        return true;
    }
    return false;
}

bool animation_set_speed(Animation* animation, float speed) {
    if (!animation) return false;
    
    AnimationImpl* impl = to_animation_impl(animation);
    impl->speed = std::max(0.1f, speed);
    
    POKO_LOG_DEBUG("Animation: Set speed to " + std::to_string(impl->speed) + " for '" + impl->name + "'");
    return true;
}

float animation_get_speed(const Animation* animation) {
    if (!animation) return 1.0f;
    
    const AnimationImpl* impl = to_animation_impl(animation);
    return impl->speed;
}

bool animation_set_mode(Animation* animation, AnimationMode mode) {
    if (!animation) return false;
    
    AnimationImpl* impl = to_animation_impl(animation);
    impl->mode = mode;
    
    POKO_LOG_DEBUG("Animation: Set mode to " + std::to_string(mode) + " for '" + impl->name + "'");
    return true;
}

AnimationMode animation_get_mode(const Animation* animation) {
    if (!animation) return ANIMATION_MODE_ONCE;
    
    const AnimationImpl* impl = to_animation_impl(animation);
    return impl->mode;
}

bool animation_set_time(Animation* animation, float time) {
    if (!animation) return false;
    
    AnimationImpl* impl = to_animation_impl(animation);
    impl->current_time = std::max(0.0f, std::min(impl->duration, time));
    
    return true;
}

float animation_get_time(const Animation* animation) {
    if (!animation) return 0.0f;
    
    const AnimationImpl* impl = to_animation_impl(animation);
    return impl->current_time;
}

float animation_get_duration(const Animation* animation) {
    if (!animation) return 0.0f;
    
    const AnimationImpl* impl = to_animation_impl(animation);
    return impl->duration;
}

bool animation_update(Animation* animation, float delta_time) {
    if (!animation || delta_time <= 0.0f) return false;
    
    AnimationImpl* impl = to_animation_impl(animation);
    
    if (impl->state != ANIMATION_STATE_PLAYING) return true;
    
    // Update time based on speed and direction
    float time_delta = delta_time * impl->speed;
    
    if (impl->mode == ANIMATION_MODE_PING_PONG && !impl->ping_pong_direction) {
        time_delta = -time_delta;
    }
    
    impl->current_time += time_delta;
    
    // Handle playback modes
    if (impl->mode == ANIMATION_MODE_ONCE) {
        if (impl->current_time >= impl->duration) {
            impl->current_time = impl->duration;
            impl->state = ANIMATION_STATE_STOPPED;
            impl->is_finished = true;
            
            if (impl->completion_callback) {
                impl->completion_callback(impl->callback_user_data);
            }
        }
    } else if (impl->mode == ANIMATION_MODE_LOOP) {
        if (impl->current_time >= impl->duration) {
            impl->current_time = 0.0f;
        }
    } else if (impl->mode == ANIMATION_MODE_PING_PONG) {
        if (impl->ping_pong_direction) {
            if (impl->current_time >= impl->duration) {
                impl->current_time = impl->duration;
                impl->ping_pong_direction = false;
            }
        } else {
            if (impl->current_time <= 0.0f) {
                impl->current_time = 0.0f;
                impl->ping_pong_direction = true;
            }
        }
    }
    
    return true;
}

} // extern "C"