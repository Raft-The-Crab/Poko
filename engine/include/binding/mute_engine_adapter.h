/**
 * @file mute_engine_adapter.h
 * @brief Mute Engine Adapter - Bridges Mute Engine API to Poko Engine
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#ifndef MUTE_ENGINE_ADAPTER_H
#define MUTE_ENGINE_ADAPTER_H

#include "mute/engine/engine.h"
#include "mute/sandbox/sandbox.h"
#include "mute/runtime/value.h"
#include "runtime/instance.h"
#include "runtime/world.h"
#include <stdint.h>
#include <cstring>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct MuteEngineAdapter
 * @brief Adapter state for Mute Engine API
 */
typedef struct {
    void* engine_handle;  // Handle to actual Poko Engine instance
    MuteContext current_context;
    SandboxState* sandbox_state;
    EngineState* api_state;
    bool is_initialized;
} MuteEngineAdapter;

/**
 * @brief Initialize engine adapter
 * @param adapter Adapter to initialize
 * @param world_handle Poko world handle
 * @return true on success
 */
bool mute_engine_adapter_init(MuteEngineAdapter* adapter, void* world_handle);

/**
 * @brief Free engine adapter
 * @param adapter Adapter to free
 */
void mute_engine_adapter_free(MuteEngineAdapter* adapter);

/**
 * @brief Call engine API function
 * @param adapter Engine adapter
 * @param api API to call
 * @param arguments Arguments array
 * @param arg_count Argument count
 * @return Result value
 */
Value mute_engine_adapter_call(MuteEngineAdapter* adapter, EngineAPI api, Value* arguments, size_t arg_count);

/**
 * @brief Set Mute execution context
 * @param adapter Engine adapter
 * @param context Context to set
 */
void mute_engine_adapter_set_context(MuteEngineAdapter* adapter, MuteContext context);

/**
 * @brief Get current context
 * @param adapter Engine adapter
 * @return Current context
 */
MuteContext mute_engine_adapter_get_context(const MuteEngineAdapter* adapter);

/**
 * @brief Check if API is available in current context
 * @param adapter Engine adapter
 * @param api API to check
 * @return true if available
 */
bool mute_engine_adapter_is_available(const MuteEngineAdapter* adapter, EngineAPI api);

/**
 * @brief Check if operation is permitted by sandbox
 * @param adapter Engine adapter
 * @param capability Capability to check
 * @return true if permitted
 */
bool mute_engine_adapter_check_permission(const MuteEngineAdapter* adapter, SandboxCapability capability);

/**
 * @brief Get engine handle for API
 * @param adapter Engine adapter
 * @param api API to get handle for
 * @return Engine handle or NULL
 */
void* mute_engine_adapter_get_handle(const MuteEngineAdapter* adapter, EngineAPI api);

/**
 * @brief Register engine function (called by Poko Engine during initialization)
 * @param name Function name
 * @param function_ptr Function pointer
 */
void mute_engine_adapter_set_function(const char* name, void* function_ptr);

/**
 * @brief Register all engine functions (called by Poko Engine during initialization)
 */
void mute_engine_adapter_register_functions(void);

#ifdef __cplusplus
}
#endif

#endif // MUTE_ENGINE_ADAPTER_H
