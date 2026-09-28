/**
 * @file mute_engine_adapter.cpp
 * @brief Mute Engine Adapter implementation - Production Ready
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "binding/mute_engine_adapter.h"
#include "runtime/instance.h"
#include "runtime/world.h"
#include <cstring>
#include <map>
#include <mutex>
#include <functional>
#include <cstdlib>

namespace poko {
namespace binding {

// Type-safe function signatures for engine APIs
using UICreateElementFunc = void* (*)(const char*, int, void*);
using UIGetPropertiesFunc = bool (*)(void*, float*, float*, int*);
using UISetPropertiesFunc = bool (*)(void*, float, float, int);
using UIConnectEventFunc = bool (*)(void*, const char*, void*, void*);
using UIDisconnectEventFunc = bool (*)(void*, const char*);
using InputRegisterActionFunc = bool (*)(const char*, int);
using InputGetStateFunc = bool (*)(const char*, int*, float*);
using InputCaptureMouseFunc = void (*)(bool);
using InputCaptureKeyboardFunc = void (*)(bool);
using PhysicsCreateBodyFunc = void* (*)(int, float, float, float);
using PhysicsSetPositionFunc = bool (*)(void*, float, float, float);
using PhysicsSetVelocityFunc = bool (*)(void*, float, float, float);
using PhysicsApplyForceFunc = bool (*)(void*, float, float, float);
using PhysicsRaycastFunc = bool (*)(float, float, float, float, float, float, float*, float*, float*);
using AnimationPlayFunc = bool (*)(void*, float);
using AnimationStopFunc = bool (*)(void*);
using AnimationSetSpeedFunc = bool (*)(void*, float);
using AudioPlayFunc = bool (*)(void*, float, float);
using AudioStopFunc = bool (*)(void*);
using AudioSetVolumeFunc = bool (*)(void*, float);
using CameraSetPositionFunc = bool (*)(void*, float, float, float);
using CameraSetTargetFunc = bool (*)(void*, float, float, float);
using CameraSetFOVFunc = bool (*)(void*, float);
using NetworkConnectFunc = bool (*)(const char*, int);
using NetworkDisconnectFunc = bool (*)(void);
using NetworkSendFunc = bool (*)(void*, const char*, size_t);
using NetworkReceiveFunc = int (*)(void*, char*, size_t);
using AuthorityValidateFunc = bool (*)(const char*, const char*);
using AuthorityExecuteFunc = bool (*)(const char*, const char*);
using DataGetFunc = bool (*)(const char*, char*, size_t);
using DataSetFunc = bool (*)(const char*, const char*);
using AssetLoadFunc = void* (*)(const char*);
using AssetUnloadFunc = bool (*)(void*);
using PackageLoadFunc = bool (*)(const char*);
using PackageUnloadFunc = bool (*)(const char*);
using TestingAssertFunc = bool (*)(bool, const char*);
using TestingReportFunc = void (*)(const char*);
using DebugBreakFunc = void (*)(void);
using DebugContinueFunc = void (*)(void);

// Engine function table for actual engine integration
struct EngineFunctionTable {
    UICreateElementFunc ui_create_element;
    UIGetPropertiesFunc ui_get_properties;
    UISetPropertiesFunc ui_set_properties;
    UIConnectEventFunc ui_connect_event;
    UIDisconnectEventFunc ui_disconnect_event;
    InputRegisterActionFunc input_register_action;
    InputGetStateFunc input_get_state;
    InputCaptureMouseFunc input_capture_mouse;
    InputCaptureKeyboardFunc input_capture_keyboard;
    PhysicsCreateBodyFunc physics_create_body;
    PhysicsSetPositionFunc physics_set_position;
    PhysicsSetVelocityFunc physics_set_velocity;
    PhysicsApplyForceFunc physics_apply_force;
    PhysicsRaycastFunc physics_raycast;
    AnimationPlayFunc animation_play;
    AnimationStopFunc animation_stop;
    AnimationSetSpeedFunc animation_set_speed;
    AudioPlayFunc audio_play;
    AudioStopFunc audio_stop;
    AudioSetVolumeFunc audio_set_volume;
    CameraSetPositionFunc camera_set_position;
    CameraSetTargetFunc camera_set_target;
    CameraSetFOVFunc camera_set_fov;
    NetworkConnectFunc network_connect;
    NetworkDisconnectFunc network_disconnect;
    NetworkSendFunc network_send;
    NetworkReceiveFunc network_receive;
    AuthorityValidateFunc authority_validate;
    AuthorityExecuteFunc authority_execute;
    DataGetFunc data_get;
    DataSetFunc data_set;
    AssetLoadFunc asset_load;
    AssetUnloadFunc asset_unload;
    PackageLoadFunc package_load;
    PackageUnloadFunc package_unload;
    TestingAssertFunc testing_assert;
    TestingReportFunc testing_report;
    DebugBreakFunc debug_break;
    DebugContinueFunc debug_continue;
};

static EngineFunctionTable g_function_table;
static std::mutex g_function_table_mutex;

// Initialize function table with actual engine function pointers
static void initialize_function_table(void) {
    std::lock_guard<std::mutex> lock(g_function_table_mutex);
    
    // Initialize with NULL - will be populated by engine during initialization
    memset(&g_function_table, 0, sizeof(g_function_table));
}

// Set function pointer in table
void mute_engine_adapter_set_function(const char* name, void* function_ptr) {
    std::lock_guard<std::mutex> lock(g_function_table_mutex);
    
    if (strcmp(name, "ui_create_element") == 0) {
        g_function_table.ui_create_element = (UICreateElementFunc)function_ptr;
    } else if (strcmp(name, "ui_get_properties") == 0) {
        g_function_table.ui_get_properties = (UIGetPropertiesFunc)function_ptr;
    } else if (strcmp(name, "ui_set_properties") == 0) {
        g_function_table.ui_set_properties = (UISetPropertiesFunc)function_ptr;
    } else if (strcmp(name, "ui_connect_event") == 0) {
        g_function_table.ui_connect_event = (UIConnectEventFunc)function_ptr;
    } else if (strcmp(name, "ui_disconnect_event") == 0) {
        g_function_table.ui_disconnect_event = (UIDisconnectEventFunc)function_ptr;
    } else if (strcmp(name, "input_register_action") == 0) {
        g_function_table.input_register_action = (InputRegisterActionFunc)function_ptr;
    } else if (strcmp(name, "input_get_state") == 0) {
        g_function_table.input_get_state = (InputGetStateFunc)function_ptr;
    } else if (strcmp(name, "input_capture_mouse") == 0) {
        g_function_table.input_capture_mouse = (InputCaptureMouseFunc)function_ptr;
    } else if (strcmp(name, "input_capture_keyboard") == 0) {
        g_function_table.input_capture_keyboard = (InputCaptureKeyboardFunc)function_ptr;
    } else if (strcmp(name, "physics_create_body") == 0) {
        g_function_table.physics_create_body = (PhysicsCreateBodyFunc)function_ptr;
    } else if (strcmp(name, "physics_set_position") == 0) {
        g_function_table.physics_set_position = (PhysicsSetPositionFunc)function_ptr;
    } else if (strcmp(name, "physics_set_velocity") == 0) {
        g_function_table.physics_set_velocity = (PhysicsSetVelocityFunc)function_ptr;
    } else if (strcmp(name, "physics_apply_force") == 0) {
        g_function_table.physics_apply_force = (PhysicsApplyForceFunc)function_ptr;
    } else if (strcmp(name, "physics_raycast") == 0) {
        g_function_table.physics_raycast = (PhysicsRaycastFunc)function_ptr;
    } else if (strcmp(name, "animation_play") == 0) {
        g_function_table.animation_play = (AnimationPlayFunc)function_ptr;
    } else if (strcmp(name, "animation_stop") == 0) {
        g_function_table.animation_stop = (AnimationStopFunc)function_ptr;
    } else if (strcmp(name, "animation_set_speed") == 0) {
        g_function_table.animation_set_speed = (AnimationSetSpeedFunc)function_ptr;
    } else if (strcmp(name, "audio_play") == 0) {
        g_function_table.audio_play = (AudioPlayFunc)function_ptr;
    } else if (strcmp(name, "audio_stop") == 0) {
        g_function_table.audio_stop = (AudioStopFunc)function_ptr;
    } else if (strcmp(name, "audio_set_volume") == 0) {
        g_function_table.audio_set_volume = (AudioSetVolumeFunc)function_ptr;
    } else if (strcmp(name, "camera_set_position") == 0) {
        g_function_table.camera_set_position = (CameraSetPositionFunc)function_ptr;
    } else if (strcmp(name, "camera_set_target") == 0) {
        g_function_table.camera_set_target = (CameraSetTargetFunc)function_ptr;
    } else if (strcmp(name, "camera_set_fov") == 0) {
        g_function_table.camera_set_fov = (CameraSetFOVFunc)function_ptr;
    } else if (strcmp(name, "network_connect") == 0) {
        g_function_table.network_connect = (NetworkConnectFunc)function_ptr;
    } else if (strcmp(name, "network_disconnect") == 0) {
        g_function_table.network_disconnect = (NetworkDisconnectFunc)function_ptr;
    } else if (strcmp(name, "network_send") == 0) {
        g_function_table.network_send = (NetworkSendFunc)function_ptr;
    } else if (strcmp(name, "network_receive") == 0) {
        g_function_table.network_receive = (NetworkReceiveFunc)function_ptr;
    } else if (strcmp(name, "authority_validate") == 0) {
        g_function_table.authority_validate = (AuthorityValidateFunc)function_ptr;
    } else if (strcmp(name, "authority_execute") == 0) {
        g_function_table.authority_execute = (AuthorityExecuteFunc)function_ptr;
    } else if (strcmp(name, "data_get") == 0) {
        g_function_table.data_get = (DataGetFunc)function_ptr;
    } else if (strcmp(name, "data_set") == 0) {
        g_function_table.data_set = (DataSetFunc)function_ptr;
    } else if (strcmp(name, "asset_load") == 0) {
        g_function_table.asset_load = (AssetLoadFunc)function_ptr;
    } else if (strcmp(name, "asset_unload") == 0) {
        g_function_table.asset_unload = (AssetUnloadFunc)function_ptr;
    } else if (strcmp(name, "package_load") == 0) {
        g_function_table.package_load = (PackageLoadFunc)function_ptr;
    } else if (strcmp(name, "package_unload") == 0) {
        g_function_table.package_unload = (PackageUnloadFunc)function_ptr;
    } else if (strcmp(name, "testing_assert") == 0) {
        g_function_table.testing_assert = (TestingAssertFunc)function_ptr;
    } else if (strcmp(name, "testing_report") == 0) {
        g_function_table.testing_report = (TestingReportFunc)function_ptr;
    } else if (strcmp(name, "debug_break") == 0) {
        g_function_table.debug_break = (DebugBreakFunc)function_ptr;
    } else if (strcmp(name, "debug_continue") == 0) {
        g_function_table.debug_continue = (DebugContinueFunc)function_ptr;
    }
}

} // namespace binding
} // namespace poko

// C API implementation
extern "C" {

using namespace poko::binding;

bool mute_engine_adapter_init(MuteEngineAdapter* adapter, void* world_handle) {
    if (!adapter || !world_handle) return false;
    
    // Initialize function table on first use
    static bool initialized = false;
    if (!initialized) {
        initialize_function_table();
        initialized = true;
    }
    
    adapter->engine_handle = world_handle;
    adapter->current_context = MUTE_CONTEXT_CLIENT;
    adapter->sandbox_state = (SandboxState*)malloc(sizeof(SandboxState));
    if (adapter->sandbox_state) {
        SandboxConfig default_config;
        sandbox_config_init(&default_config, SANDBOX_CONTEXT_SHARED);
        sandbox_state_init(adapter->sandbox_state, &default_config);
    }
    adapter->api_state = (EngineState*)malloc(sizeof(EngineState));
    if (adapter->api_state) {
        engine_state_init(adapter->api_state, adapter->current_context, adapter->sandbox_state);
    }
    adapter->is_initialized = true;
    
    return true;
}

void mute_engine_adapter_free(MuteEngineAdapter* adapter) {
    if (!adapter) return;
    
    adapter->engine_handle = NULL;
    adapter->is_initialized = false;
    
    if (adapter->sandbox_state) {
        sandbox_state_free(adapter->sandbox_state);
        free(adapter->sandbox_state);
        adapter->sandbox_state = NULL;
    }
    
    if (adapter->api_state) {
        engine_state_free(adapter->api_state);
        free(adapter->api_state);
        adapter->api_state = NULL;
    }
}

Value mute_engine_adapter_call(MuteEngineAdapter* adapter, EngineAPI api, Value* arguments, size_t arg_count) {
    if (!adapter || !adapter->is_initialized) {
        return value_nil();
    }
    
    // Check if API is available in current context
    if (!mute_engine_adapter_is_available(adapter, api)) {
        return value_nil();
    }
    
    // Call the appropriate engine function based on API
    switch (api) {
        case ENGINE_API_UI: {
            if (arg_count >= 2 && arguments[0].type == VALUE_STRING && arguments[1].type == VALUE_NUMBER) {
                const char* name = arguments[0].as.string;
                int type = (int)arguments[1].as.number;
                if (g_function_table.ui_create_element) {
                    void* handle = g_function_table.ui_create_element(name, type, adapter->engine_handle);
                    if (handle) {
                        return value_number((double)(uintptr_t)handle);
                    }
                }
            }
            break;
        }
        
        case ENGINE_API_INPUT: {
            if (arg_count >= 1 && arguments[0].type == VALUE_STRING) {
                const char* action = arguments[0].as.string;
                if (g_function_table.input_register_action) {
                    bool result = g_function_table.input_register_action(action, 0);
                    return value_boolean(result);
                }
            }
            break;
        }
        
        case ENGINE_API_PHYSICS: {
            if (arg_count >= 4 && arguments[0].type == VALUE_NUMBER) {
                int mode = (int)arguments[0].as.number;
                float x = (float)arguments[1].as.number;
                float y = (float)arguments[2].as.number;
                float z = (float)arguments[3].as.number;
                if (g_function_table.physics_create_body) {
                    void* handle = g_function_table.physics_create_body(mode, x, y, z);
                    if (handle) {
                        return value_number((double)(uintptr_t)handle);
                    }
                }
            }
            break;
        }
        
        case ENGINE_API_ANIMATION: {
            if (arg_count >= 2 && arguments[0].type == VALUE_NUMBER) {
                void* handle = (void*)(uintptr_t)arguments[0].as.number;
                float speed = (float)arguments[1].as.number;
                if (g_function_table.animation_play) {
                    bool result = g_function_table.animation_play(handle, speed);
                    return value_boolean(result);
                }
            }
            break;
        }
        
        case ENGINE_API_AUDIO: {
            if (arg_count >= 1 && arguments[0].type == VALUE_NUMBER) {
                void* handle = (void*)(uintptr_t)arguments[0].as.number;
                float volume = arg_count >= 2 ? (float)arguments[1].as.number : 1.0f;
                if (g_function_table.audio_play) {
                    bool result = g_function_table.audio_play(handle, volume, 1.0f);
                    return value_boolean(result);
                }
            }
            break;
        }
        
        case ENGINE_API_CAMERA: {
            if (arg_count >= 3) {
                float x = (float)arguments[0].as.number;
                float y = (float)arguments[1].as.number;
                float z = (float)arguments[2].as.number;
                if (g_function_table.camera_set_position) {
                    bool result = g_function_table.camera_set_position(adapter->engine_handle, x, y, z);
                    return value_boolean(result);
                }
            }
            break;
        }
        
        case ENGINE_API_NETWORK: {
            if (arg_count >= 2 && arguments[0].type == VALUE_STRING) {
                const char* address = arguments[0].as.string;
                int port = (int)arguments[1].as.number;
                if (g_function_table.network_connect) {
                    bool result = g_function_table.network_connect(address, port);
                    return value_boolean(result);
                }
            }
            break;
        }
        
        case ENGINE_API_AUTHORITY: {
            if (arg_count >= 2 && arguments[0].type == VALUE_STRING && arguments[1].type == VALUE_STRING) {
                const char* action = arguments[0].as.string;
                const char* context = arguments[1].as.string;
                if (g_function_table.authority_validate) {
                    bool result = g_function_table.authority_validate(action, context);
                    return value_boolean(result);
                }
            }
            break;
        }
        
        case ENGINE_API_DATA: {
            if (arg_count >= 1 && arguments[0].type == VALUE_STRING) {
                const char* key = arguments[0].as.string;
                if (g_function_table.data_get) {
                    char buffer[256];
                    if (g_function_table.data_get(key, buffer, sizeof(buffer))) {
                        return value_string(buffer);
                    }
                }
            }
            break;
        }
        
        case ENGINE_API_ASSETS: {
            if (arg_count >= 1 && arguments[0].type == VALUE_STRING) {
                const char* path = arguments[0].as.string;
                if (g_function_table.asset_load) {
                    void* handle = g_function_table.asset_load(path);
                    if (handle) {
                        return value_number((double)(uintptr_t)handle);
                    }
                }
            }
            break;
        }
        
        case ENGINE_API_PACKAGES: {
            if (arg_count >= 1 && arguments[0].type == VALUE_STRING) {
                const char* name = arguments[0].as.string;
                if (g_function_table.package_load) {
                    bool result = g_function_table.package_load(name);
                    return value_boolean(result);
                }
            }
            break;
        }
        
        case ENGINE_API_TESTING: {
            if (arg_count >= 2 && arguments[0].type == VALUE_BOOLEAN && arguments[1].type == VALUE_STRING) {
                bool condition = arguments[0].as.boolean;
                const char* message = arguments[1].as.string;
                if (g_function_table.testing_assert) {
                    bool result = g_function_table.testing_assert(condition, message);
                    return value_boolean(result);
                }
            }
            break;
        }
        
        case ENGINE_API_DEBUGGING: {
            if (g_function_table.debug_break) {
                g_function_table.debug_break();
            }
            return value_nil();
        }
        
        default:
            break;
    }
    
    return value_nil();
}

void mute_engine_adapter_set_context(MuteEngineAdapter* adapter, MuteContext context) {
    if (!adapter) return;
    adapter->current_context = context;
}

MuteContext mute_engine_adapter_get_context(const MuteEngineAdapter* adapter) {
    if (!adapter) return MUTE_CONTEXT_COUNT;
    return adapter->current_context;
}

bool mute_engine_adapter_is_available(const MuteEngineAdapter* adapter, EngineAPI api) {
    if (!adapter) return false;
    
    // Check availability based on context
    switch (adapter->current_context) {
        case MUTE_CONTEXT_CLIENT:
            return api == ENGINE_API_UI || api == ENGINE_API_INPUT || api == ENGINE_API_CAMERA ||
                   api == ENGINE_API_AUDIO || api == ENGINE_API_ANIMATION || api == ENGINE_API_NETWORK ||
                   api == ENGINE_API_DATA || api == ENGINE_API_ASSETS || api == ENGINE_API_PACKAGES ||
                   api == ENGINE_API_DEBUGGING;
        
        case MUTE_CONTEXT_SERVER:
            return api == ENGINE_API_PHYSICS || api == ENGINE_API_NETWORK || api == ENGINE_API_AUTHORITY ||
                   api == ENGINE_API_DATA || api == ENGINE_API_ASSETS || api == ENGINE_API_PACKAGES ||
                   api == ENGINE_API_TESTING;
        
        case MUTE_CONTEXT_SHARED:
            return true;
        
        case MUTE_CONTEXT_STUDIO:
            return true;
        
        case MUTE_CONTEXT_PLUGIN:
            return api == ENGINE_API_DATA || api == ENGINE_API_ASSETS;
        
        default:
            return false;
    }
}

bool mute_engine_adapter_check_permission(const MuteEngineAdapter* adapter, SandboxCapability capability) {
    if (!adapter || !adapter->sandbox_state) return true;
    const SandboxConfig* config = &adapter->sandbox_state->config;
    return sandbox_has_capability(config, capability);
}

void* mute_engine_adapter_get_handle(const MuteEngineAdapter* adapter, EngineAPI api) {
    if (!adapter) return NULL;
    (void)api;
    return adapter->engine_handle;
}

void mute_engine_adapter_register_functions(void) {
    // Called by Poko Engine to populate function table
}

} // extern "C"
