/**
 * @file animation.h
 * @brief Animation system interface for Poko Engine
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#ifndef POKO_ANIMATION_H
#define POKO_ANIMATION_H

#ifdef __cplusplus
extern "C" {
#endif

// Animation handle (opaque)
typedef struct Animation Animation;

// Animation state
typedef enum {
    ANIMATION_STATE_STOPPED = 0,
    ANIMATION_STATE_PLAYING = 1,
    ANIMATION_STATE_PAUSED = 2
} AnimationState;

// Animation playback mode
typedef enum {
    ANIMATION_MODE_ONCE = 0,
    ANIMATION_MODE_LOOP = 1,
    ANIMATION_MODE_PING_PONG = 2
} AnimationMode;

/**
 * Create an animation
 * @param name Animation name
 * @param duration Animation duration in seconds
 * @return Animation handle, or NULL on failure
 */
Animation* animation_create(const char* name, float duration);

/**
 * Destroy an animation
 * @param animation Animation handle
 */
void animation_destroy(Animation* animation);

/**
 * Play an animation
 * @param animation Animation handle
 * @param speed Playback speed multiplier
 * @return true on success, false on failure
 */
bool animation_play(Animation* animation, float speed);

/**
 * Stop an animation
 * @param animation Animation handle
 * @return true on success, false on failure
 */
bool animation_stop(Animation* animation);

/**
 * Pause an animation
 * @param animation Animation handle
 * @return true on success, false on failure
 */
bool animation_pause(Animation* animation);

/**
 * Resume a paused animation
 * @param animation Animation handle
 * @return true on success, false on failure
 */
bool animation_resume(Animation* animation);

/**
 * Get animation state
 * @param animation Animation handle
 * @return Animation state
 */
AnimationState animation_get_state(const Animation* animation);

/**
 * Set animation playback speed
 * @param animation Animation handle
 * @param speed Playback speed multiplier
 * @return true on success, false on failure
 */
bool animation_set_speed(Animation* animation, float speed);

/**
 * Get animation playback speed
 * @param animation Animation handle
 * @return Playback speed multiplier
 */
float animation_get_speed(const Animation* animation);

/**
 * Set animation playback mode
 * @param animation Animation handle
 * @param mode Playback mode
 * @return true on success, false on failure
 */
bool animation_set_mode(Animation* animation, AnimationMode mode);

/**
 * Get animation playback mode
 * @param animation Animation handle
 * @return Playback mode
 */
AnimationMode animation_get_mode(const Animation* animation);

/**
 * Set animation time
 * @param animation Animation handle
 * @param time Time in seconds
 * @return true on success, false on failure
 */
bool animation_set_time(Animation* animation, float time);

/**
 * Get animation time
 * @param animation Animation handle
 * @return Current time in seconds
 */
float animation_get_time(const Animation* animation);

/**
 * Get animation duration
 * @param animation Animation handle
 * @return Duration in seconds
 */
float animation_get_duration(const Animation* animation);

/**
 * Update animation (call once per frame)
 * @param animation Animation handle
 * @param delta_time Time since last frame in seconds
 * @return true on success, false on failure
 */
bool animation_update(Animation* animation, float delta_time);

/**
 * Check if animation has finished
 * @param animation Animation handle
 * @return true if finished, false otherwise
 */
bool animation_is_finished(const Animation* animation);

/**
 * Set animation completion callback
 * @param animation Animation handle
 * @param callback Function to call when animation completes
 * @param user_data User data to pass to callback
 */
void animation_set_completion_callback(Animation* animation, void (*callback)(void*), void* user_data);

#ifdef __cplusplus
}
#endif

#endif // POKO_ANIMATION_H