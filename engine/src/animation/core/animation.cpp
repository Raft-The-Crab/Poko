/**
 * @file animation.cpp
 * @brief Animation core implementation
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "animation/animation.h"
#include "core/logging/logger.h"
#include <cstring>
#include <map>
#include <mutex>

namespace poko {
namespace animation {

// Animation implementation
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

// Animation manager
struct AnimationManagerImpl {
    std::map<std::string, AnimationImpl*> animations;
    std::mutex mutex;
    uint64_t next_animation_id;
};

// Convert to internal implementation
static AnimationImpl* to_animation_impl(Animation* animation) {
    return reinterpret_cast<AnimationImpl*>(animation);
}

static const AnimationImpl* to_animation_impl(const Animation* animation) {
    return reinterpret_cast<const AnimationImpl*>(animation);
}

static Animation* from_animation_impl(AnimationImpl* impl) {
    return reinterpret_cast<Animation*>(impl);
}

} // namespace animation
} // namespace poko

// C API implementation
extern "C" {

using namespace poko::animation;

Animation* animation_create(const char* name, float duration) {
    if (!name || duration <= 0.0f) return nullptr;
    
    static AnimationManagerImpl* manager = nullptr;
    if (!manager) {
        manager = new AnimationManagerImpl();
        manager->next_animation_id = 1;
    }
    
    std::lock_guard<std::mutex> lock(manager->mutex);
    
    AnimationImpl* animation = new AnimationImpl();
    animation->name = name;
    animation->duration = duration;
    animation->current_time = 0.0f;
    animation->speed = 1.0f;
    animation->state = ANIMATION_STATE_STOPPED;
    animation->mode = ANIMATION_MODE_ONCE;
    animation->is_finished = false;
    animation->completion_callback = nullptr;
    animation->callback_user_data = nullptr;
    animation->ping_pong_direction = true;
    
    manager->animations[name] = animation;
    
    POKO_LOG_INFO("Animation: Created animation '" + std::string(name) + "' with duration " + std::to_string(duration) + "s");
    return from_animation_impl(animation);
}

void animation_destroy(Animation* animation) {
    if (!animation) return;
    
    static AnimationManagerImpl* manager = nullptr;
    if (!manager) return;
    
    std::lock_guard<std::mutex> lock(manager->mutex);
    
    AnimationImpl* impl = to_animation_impl(animation);
    manager->animations.erase(impl->name);
    delete impl;
    
    POKO_LOG_DEBUG("Animation: Destroyed animation '" + impl->name + "'");
}

AnimationState animation_get_state(const Animation* animation) {
    if (!animation) return ANIMATION_STATE_STOPPED;
    
    const AnimationImpl* impl = to_animation_impl(animation);
    return impl->state;
}

bool animation_is_finished(const Animation* animation) {
    if (!animation) return true;
    
    const AnimationImpl* impl = to_animation_impl(animation);
    return impl->is_finished;
}

void animation_set_completion_callback(Animation* animation, void (*callback)(void*), void* user_data) {
    if (!animation) return;
    
    AnimationImpl* impl = to_animation_impl(animation);
    impl->completion_callback = callback;
    impl->callback_user_data = user_data;
}

} // extern "C"