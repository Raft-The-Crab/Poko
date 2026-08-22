/**
 * @file engine_bindings.h
 * @brief Engine API bindings header for Mute scripts
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-21
 */

#ifndef POKO_SCRIPTING_ENGINE_BINDINGS_H
#define POKO_SCRIPTING_ENGINE_BINDINGS_H

namespace poko {

// Forward declarations
struct MuteVM;
struct MuteVMContext;
struct MuteValue;
struct MuteTable;

/**
 * @brief Set the global engine instance
 * @param engine Pointer to engine instance
 */
void set_engine_instance(void* engine);

/**
 * @brief Get the global engine instance
 * @return Pointer to engine instance
 */
void* get_engine_instance();

/**
 * @brief Register engine API bindings with Mute VM
 * @param vm Mute VM instance
 * @param engine Pointer to engine instance
 */
void register_engine_bindings(MuteVM* vm, void* engine);

// C binding functions for Mute

/**
 * @brief Mute binding: exit the engine
 */
int engine_exit(void* vm);

/**
 * @brief Mute binding: get delta time
 */
int engine_get_delta_time(void* vm);

/**
 * @brief Mute binding: get FPS
 */
int engine_get_fps(void* vm);

/**
 * @brief Mute binding: get total time
 */
int engine_get_total_time(void* vm);

/**
 * @brief Mute binding: check if key is down
 */
int engine_is_key_down(void* vm);

/**
 * @brief Mute binding: check if key was pressed
 */
int engine_is_key_pressed(void* vm);

/**
 * @brief Mute binding: get mouse position
 */
int engine_get_mouse_position(void* vm);

/**
 * @brief Mute binding: set window title
 */
int engine_set_window_title(void* vm);

/**
 * @brief Mute binding: get window width
 */
int engine_get_window_width(void* vm);

/**
 * @brief Mute binding: get window height
 */
int engine_get_window_height(void* vm);

} // namespace poko

#endif // POKO_SCRIPTING_ENGINE_BINDINGS_H