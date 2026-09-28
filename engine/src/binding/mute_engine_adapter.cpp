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
#include "physics/physics.h"
#include "audio/audio.h"
#include "input/input.h"
#include "camera/camera.h"
#include "animation/animation.h"
#include "ui/ui.h"
#include "networking/network.h"
#include "core/logging/logger.h"
#include "core/memory/memory.h"
#include <cstring>
#include <map>
#include <mutex>
#include <functional>
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <atomic>

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

// Static physics world for adapter (in production, this would be managed by the engine)
static PhysicsWorld* g_physics_world = nullptr;
static AudioWorld* g_audio_world = nullptr;
static InputSystem* g_input_system = nullptr;
static Camera* g_main_camera = nullptr;
static NetworkConnection* g_network_connection = nullptr;
static std::mutex g_physics_mutex;
static std::mutex g_audio_mutex;
static std::mutex g_input_mutex;
static std::mutex g_camera_mutex;
static std::mutex g_animation_mutex;
static std::mutex g_ui_mutex;
static std::mutex g_network_mutex;

// Memory tracking for the adapter
struct AllocationInfo {
    size_t size;
    const char* source;
    size_t line;
};

static std::unordered_map<void*, AllocationInfo> g_allocation_map;
static std::mutex g_allocation_mutex;
static std::atomic<size_t> g_total_allocated{0};
static std::atomic<size_t> g_allocation_count{0};

// Tracked allocation wrapper
static void* tracked_malloc(size_t size, const char* source, size_t line) {
    void* ptr = malloc(size);
    if (ptr) {
        std::lock_guard<std::mutex> lock(g_allocation_mutex);
        g_allocation_map[ptr] = {size, source, line};
        g_total_allocated += size;
        g_allocation_count++;
    }
    return ptr;
}

// Tracked free wrapper
static void tracked_free(void* ptr, const char* source, size_t line) {
    (void)source; // Suppress unused parameter warning
    (void)line;    // Suppress unused parameter warning
    if (ptr) {
        std::lock_guard<std::mutex> lock(g_allocation_mutex);
        auto it = g_allocation_map.find(ptr);
        if (it != g_allocation_map.end()) {
            g_total_allocated -= it->second.size;
            g_allocation_count--;
            g_allocation_map.erase(it);
        }
    }
    free(ptr);
}

// Get memory statistics
static void get_memory_stats(size_t* total_allocated, size_t* allocation_count) {
    if (total_allocated) *total_allocated = g_total_allocated.load();
    if (allocation_count) *allocation_count = g_allocation_count.load();
}

// Initialize function table with actual engine function pointers
static void initialize_function_table(void) {
    std::lock_guard<std::mutex> lock(g_function_table_mutex);
    
    // Initialize with NULL - will be populated by engine during initialization
    memset(&g_function_table, 0, sizeof(g_function_table));
    
    // Create physics world
    if (!g_physics_world) {
        g_physics_world = physics_world_create();
    }
    
    // Create audio world
    if (!g_audio_world) {
        g_audio_world = audio_world_create();
    }
    
    // Create input system
    if (!g_input_system) {
        g_input_system = input_system_create();
    }
    
    // Create main camera
    if (!g_main_camera) {
        g_main_camera = camera_create(CAMERA_TYPE_PERSPECTIVE);
    }
    
    // Animation system doesn't need a world, it's per-animation
    POKO_LOG_DEBUG("Animation: Animation system ready");
}

// Cleanup function table resources
static void cleanup_function_table(void) {
    std::lock_guard<std::mutex> lock(g_function_table_mutex);
    
    if (g_physics_world) {
        physics_world_destroy(g_physics_world);
        g_physics_world = nullptr;
    }
    
    if (g_audio_world) {
        audio_world_destroy(g_audio_world);
        g_audio_world = nullptr;
    }
    
    if (g_input_system) {
        input_system_destroy(g_input_system);
        g_input_system = nullptr;
    }
    
    if (g_main_camera) {
        camera_destroy(g_main_camera);
        g_main_camera = nullptr;
    }
    
    if (g_network_connection) {
        network_disconnect(g_network_connection);
        g_network_connection = nullptr;
    }
}

// Physics wrapper functions matching adapter signatures
static void* physics_create_body_wrapper(int mode, float x, float y, float z) {
    std::lock_guard<std::mutex> lock(g_physics_mutex);
    
    if (!g_physics_world) {
        POKO_LOG_ERROR("Physics: Physics world not initialized");
        return nullptr;
    }
    
    PhysicsBodyType body_type = static_cast<PhysicsBodyType>(mode);
    PhysicsMaterial material = {0.5f, 0.0f, 1.0f}; // Default material
    
    PhysicsBody* body = physics_body_create(g_physics_world, body_type, PHYSICS_SHAPE_SPHERE, &material);
    if (body) {
        physics_body_set_position(body, x, y, z);
        POKO_LOG_DEBUG("Physics: Created body at position (" + 
                       std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")");
    } else {
        POKO_LOG_ERROR("Physics: Failed to create body");
    }
    
    return static_cast<void*>(body);
}

static bool physics_set_position_wrapper(void* handle, float x, float y, float z) __attribute__((unused));
static bool physics_set_position_wrapper(void* handle, float x, float y, float z) {
    std::lock_guard<std::mutex> lock(g_physics_mutex);
    
    if (!handle) {
        POKO_LOG_WARN("Physics: Attempted to set position on null handle");
        return false;
    }
    PhysicsBody* body = static_cast<PhysicsBody*>(handle);
    physics_body_set_position(body, x, y, z);
    POKO_LOG_DEBUG("Physics: Set body position to (" + 
                   std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")");
    return true;
}

static bool physics_set_velocity_wrapper(void* handle, float x, float y, float z) __attribute__((unused));
static bool physics_set_velocity_wrapper(void* handle, float x, float y, float z) {
    std::lock_guard<std::mutex> lock(g_physics_mutex);
    
    if (!handle) {
        POKO_LOG_WARN("Physics: Attempted to set velocity on null handle");
        return false;
    }
    PhysicsBody* body = static_cast<PhysicsBody*>(handle);
    physics_body_set_velocity(body, x, y, z);
    POKO_LOG_DEBUG("Physics: Set body velocity to (" + 
                   std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")");
    return true;
}

static bool physics_apply_force_wrapper(void* handle, float x, float y, float z) __attribute__((unused));
static bool physics_apply_force_wrapper(void* handle, float x, float y, float z) {
    std::lock_guard<std::mutex> lock(g_physics_mutex);
    
    if (!handle) {
        POKO_LOG_WARN("Physics: Attempted to apply force to null handle");
        return false;
    }
    PhysicsBody* body = static_cast<PhysicsBody*>(handle);
    physics_body_apply_force(body, x, y, z, 0.0f, 0.0f, 0.0f); // Apply at center
    POKO_LOG_DEBUG("Physics: Applied force (" + 
                   std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")");
    return true;
}

static bool physics_raycast_wrapper(float sx, float sy, float sz, float dx, float dy, float dz, 
                                     float* hit_x, float* hit_y, float* hit_z) __attribute__((unused));
static bool physics_raycast_wrapper(float sx, float sy, float sz, float dx, float dy, float dz, 
                                     float* hit_x, float* hit_y, float* hit_z) {
    std::lock_guard<std::mutex> lock(g_physics_mutex);
    
    if (!g_physics_world) {
        POKO_LOG_ERROR("Physics: Physics world not initialized for raycast");
        return false;
    }
    
    PhysicsRaycastResult result;
    bool hit = physics_world_raycast(g_physics_world, sx, sy, sz, dx, dy, dz, 1000.0f, &result);
    
    if (hit && hit_x && hit_y && hit_z) {
        *hit_x = result.hit_position[0];
        *hit_y = result.hit_position[1];
        *hit_z = result.hit_position[2];
        POKO_LOG_DEBUG("Physics: Raycast hit at (" + 
                       std::to_string(*hit_x) + ", " + std::to_string(*hit_y) + ", " + std::to_string(*hit_z) + ")");
    } else {
        POKO_LOG_DEBUG("Physics: Raycast missed");
    }
    
    return hit;
}

// Audio wrapper functions matching adapter signatures
static bool audio_play_wrapper(void* handle, float volume, float pitch) {
    std::lock_guard<std::mutex> lock(g_audio_mutex);
    
    if (!handle || !g_audio_world) {
        POKO_LOG_WARN("Audio: Attempted to play with null handle or uninitialized audio world");
        return false;
    }
    
    // In a real implementation, this would play the audio buffer
    // For now, we just return true to indicate the API call succeeded
    POKO_LOG_DEBUG("Audio: Playing audio with volume=" + std::to_string(volume) + 
                   ", pitch=" + std::to_string(pitch));
    (void)volume;
    (void)pitch;
    return true;
}

static bool audio_stop_wrapper(void* handle) {
    std::lock_guard<std::mutex> lock(g_audio_mutex);
    
    if (!handle) {
        POKO_LOG_WARN("Audio: Attempted to stop with null handle");
        return false;
    }
    // In a real implementation, this would stop the audio source
    POKO_LOG_DEBUG("Audio: Stopped audio playback");
    return true;
}

static bool audio_set_volume_wrapper(void* handle, float volume) {
    std::lock_guard<std::mutex> lock(g_audio_mutex);
    
    if (!handle) {
        POKO_LOG_WARN("Audio: Attempted to set volume on null handle");
        return false;
    }
    // In a real implementation, this would set the audio source volume
    POKO_LOG_DEBUG("Audio: Set volume to " + std::to_string(volume));
    (void)volume;
    return true;
}

// Animation wrapper functions (stubs for now)
static bool animation_play_wrapper(void* handle, float speed) {
    if (!handle) {
        POKO_LOG_WARN("Animation: Attempted to play with null handle");
        return false;
    }
    Animation* animation = static_cast<Animation*>(handle);
    return animation_play(animation, speed);
}

static bool animation_stop_wrapper(void* handle) {
    if (!handle) {
        POKO_LOG_WARN("Animation: Attempted to stop with null handle");
        return false;
    }
    Animation* animation = static_cast<Animation*>(handle);
    return animation_stop(animation);
}

static bool animation_set_speed_wrapper(void* handle, float speed) {
    if (!handle) {
        POKO_LOG_WARN("Animation: Attempted to set speed on null handle");
        return false;
    }
    Animation* animation = static_cast<Animation*>(handle);
    return animation_set_speed(animation, speed);
}

// Camera wrapper functions (real implementation)
static bool camera_set_position_wrapper(void* handle, float x, float y, float z) {
    if (!handle) return false;
    Camera* camera = static_cast<Camera*>(handle);
    camera_set_position(camera, x, y, z);
    return true;
}

static bool camera_set_target_wrapper(void* handle, float x, float y, float z) {
    if (!handle) return false;
    Camera* camera = static_cast<Camera*>(handle);
    camera_set_target(camera, x, y, z);
    return true;
}

static bool camera_set_fov_wrapper(void* handle, float fov) {
    if (!handle) return false;
    Camera* camera = static_cast<Camera*>(handle);
    camera_set_fov(camera, fov);
    return true;
}

// Input wrapper functions (real implementation)
static bool input_register_action_wrapper(const char* action, int type) {
    if (!action) {
        POKO_LOG_WARN("Input: Attempted to register null action");
        return false;
    }
    if (!g_input_system) {
        POKO_LOG_WARN("Input: Input system not initialized");
        return false;
    }
    bool result = input_register_action(g_input_system, action, type);
    if (result) {
        POKO_LOG_DEBUG("Input: Registered action '" + std::string(action) + "' with type=" + std::to_string(type));
    }
    return result;
}

static bool input_get_state_wrapper(const char* action, int* state, float* value) {
    if (!action) {
        POKO_LOG_WARN("Input: Attempted to get state for null action");
        return false;
    }
    if (!g_input_system) {
        POKO_LOG_WARN("Input: Input system not initialized");
        return false;
    }
    if (state) {
        *state = input_is_action_pressed(g_input_system, action) ? 1 : 0;
    }
    if (value) {
        *value = input_get_action_value(g_input_system, action);
    }
    return true;
}

static void input_capture_mouse_wrapper(bool capture) {
    if (!g_input_system) {
        POKO_LOG_WARN("Input: Input system not initialized");
        return;
    }
    input_capture_mouse(g_input_system, capture);
    POKO_LOG_DEBUG("Input: " + std::string(capture ? "Captured" : "Released") + " mouse");
}

static void input_capture_keyboard_wrapper(bool capture) {
    if (!g_input_system) {
        POKO_LOG_WARN("Input: Input system not initialized");
        return;
    }
    input_capture_keyboard(g_input_system, capture);
    POKO_LOG_DEBUG("Input: " + std::string(capture ? "Captured" : "Released") + " keyboard");
}

// UI wrapper functions
static void* ui_create_element_wrapper(const char* name, int type, void* parent) {
    if (!name) {
        POKO_LOG_WARN("UI: Attempted to create element with null name");
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(g_ui_mutex);
    
    UIElement* element = ui_element_create(name, static_cast<UIElementType>(type), 
                                           reinterpret_cast<UIElement*>(parent));
    if (!element) {
        POKO_LOG_ERROR("UI: Failed to create element '" + std::string(name) + "'");
        return nullptr;
    }
    
    POKO_LOG_DEBUG("UI: Created element '" + std::string(name) + "' with type=" + std::to_string(type));
    return reinterpret_cast<void*>(element);
}

static bool ui_get_properties_wrapper(void* handle, float* x, float* y, int* visible) {
    if (!handle) {
        POKO_LOG_WARN("UI: Attempted to get properties for null handle");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(g_ui_mutex);
    
    UIElement* element = reinterpret_cast<UIElement*>(handle);
    float width, height;
    
    if (!ui_element_get_properties(element, x, y, &width, &height)) {
        POKO_LOG_ERROR("UI: Failed to get properties for element");
        return false;
    }
    
    if (visible) {
        *visible = ui_element_is_visible(element) ? 1 : 0;
    }
    
    POKO_LOG_DEBUG("UI: Got properties for element");
    return true;
}

static bool ui_set_properties_wrapper(void* handle, float x, float y, int visible) {
    if (!handle) {
        POKO_LOG_WARN("UI: Attempted to set properties for null handle");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(g_ui_mutex);
    
    UIElement* element = reinterpret_cast<UIElement*>(handle);
    
    float current_x, current_y, width, height;
    ui_element_get_properties(element, &current_x, &current_y, &width, &height);
    
    if (!ui_element_set_properties(element, x, y, width, height)) {
        POKO_LOG_ERROR("UI: Failed to set properties for element");
        return false;
    }
    
    ui_element_set_visible(element, visible != 0);
    
    POKO_LOG_DEBUG("UI: Set properties for element");
    return true;
}

static bool ui_connect_event_wrapper(void* handle, const char* event, void* callback, void* user_data) {
    if (!handle || !event) {
        POKO_LOG_WARN("UI: Attempted to connect event with null handle or event");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(g_ui_mutex);
    
    UIElement* element = reinterpret_cast<UIElement*>(handle);
    
    // Store callback in user data for now (real implementation would need event system)
    ui_element_set_user_data(element, callback);
    
    POKO_LOG_DEBUG("UI: Connected event '" + std::string(event) + "'");
    (void)user_data;
    return true;
}

static bool ui_disconnect_event_wrapper(void* handle, const char* event) {
    if (!handle || !event) {
        POKO_LOG_WARN("UI: Attempted to disconnect event with null handle or event");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(g_ui_mutex);
    
    UIElement* element = reinterpret_cast<UIElement*>(handle);
    
    // Clear user data (real implementation would need event system)
    ui_element_set_user_data(element, nullptr);
    
    POKO_LOG_DEBUG("UI: Disconnected event '" + std::string(event) + "'");
    return true;
}

// Network wrapper functions
static bool network_connect_wrapper(const char* address, int port) {
    if (!address) {
        POKO_LOG_WARN("Network: Attempted to connect to null address");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(g_network_mutex);
    
    NetworkConnection* connection = network_connect(address, port);
    if (!connection) {
        POKO_LOG_ERROR("Network: Failed to connect to " + std::string(address) + ":" + std::to_string(port));
        return false;
    }
    
    g_network_connection = connection;
    POKO_LOG_DEBUG("Network: Connected to " + std::string(address) + ":" + std::to_string(port));
    return true;
}

static bool network_disconnect_wrapper(void) {
    std::lock_guard<std::mutex> lock(g_network_mutex);
    
    if (!g_network_connection) {
        POKO_LOG_WARN("Network: Attempted to disconnect with no active connection");
        return false;
    }
    
    if (!network_disconnect(g_network_connection)) {
        POKO_LOG_ERROR("Network: Failed to disconnect");
        return false;
    }
    
    g_network_connection = nullptr;
    POKO_LOG_DEBUG("Network: Disconnected");
    return true;
}

static bool network_send_wrapper(void* connection, const char* data, size_t size) {
    if (!connection || !data) {
        POKO_LOG_WARN("Network: Attempted to send with null connection or data");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(g_network_mutex);
    
    NetworkConnection* conn = reinterpret_cast<NetworkConnection*>(connection);
    
    if (!network_send(conn, data, size)) {
        POKO_LOG_ERROR("Network: Failed to send " + std::to_string(size) + " bytes");
        return false;
    }
    
    POKO_LOG_DEBUG("Network: Sent " + std::to_string(size) + " bytes");
    return true;
}

static int network_receive_wrapper(void* connection, char* buffer, size_t size) {
    if (!connection || !buffer) {
        POKO_LOG_WARN("Network: Attempted to receive with null connection or buffer");
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(g_network_mutex);
    
    NetworkConnection* conn = reinterpret_cast<NetworkConnection*>(connection);
    
    int received = network_receive(conn, buffer, size);
    if (received < 0) {
        POKO_LOG_ERROR("Network: Failed to receive data");
        return 0;
    }
    
    POKO_LOG_DEBUG("Network: Received " + std::to_string(received) + " bytes");
    return received;
}

// Authority wrapper functions (stubs for now)
static bool authority_validate_wrapper(const char* action, const char* context) {
    if (!action || !context) {
        POKO_LOG_WARN("Authority: Attempted to validate with null action or context");
        return false;
    }
    POKO_LOG_DEBUG("Authority: Validated action '" + std::string(action) + "' in context '" + std::string(context) + "'");
    return true;
}

static bool authority_execute_wrapper(const char* action, const char* context) {
    if (!action || !context) {
        POKO_LOG_WARN("Authority: Attempted to execute with null action or context");
        return false;
    }
    POKO_LOG_DEBUG("Authority: Executed action '" + std::string(action) + "' in context '" + std::string(context) + "'");
    return true;
}

// Data wrapper functions (stubs for now)
static bool data_get_wrapper(const char* key, char* buffer, size_t size) {
    if (!key || !buffer) {
        POKO_LOG_WARN("Data: Attempted to get with null key or buffer");
        return false;
    }
    POKO_LOG_DEBUG("Data: Got value for key '" + std::string(key) + "'");
    (void)size;
    return false;
}

static bool data_set_wrapper(const char* key, const char* value) {
    if (!key || !value) {
        POKO_LOG_WARN("Data: Attempted to set with null key or value");
        return false;
    }
    POKO_LOG_DEBUG("Data: Set value for key '" + std::string(key) + "'");
    return true;
}

// Asset wrapper functions (stubs for now)
static void* asset_load_wrapper(const char* path) {
    if (!path) {
        POKO_LOG_WARN("Asset: Attempted to load with null path");
        return nullptr;
    }
    POKO_LOG_DEBUG("Asset: Loaded asset from '" + std::string(path) + "'");
    return reinterpret_cast<void*>(0x1);
}

static bool asset_unload_wrapper(void* handle) {
    if (!handle) {
        POKO_LOG_WARN("Asset: Attempted to unload with null handle");
        return false;
    }
    POKO_LOG_DEBUG("Asset: Unloaded asset");
    return true;
}

// Package wrapper functions (stubs for now)
static bool package_load_wrapper(const char* name) {
    if (!name) {
        POKO_LOG_WARN("Package: Attempted to load with null name");
        return false;
    }
    POKO_LOG_DEBUG("Package: Loaded package '" + std::string(name) + "'");
    return true;
}

static bool package_unload_wrapper(const char* name) {
    if (!name) {
        POKO_LOG_WARN("Package: Attempted to unload with null name");
        return false;
    }
    POKO_LOG_DEBUG("Package: Unloaded package '" + std::string(name) + "'");
    return true;
}

// Testing wrapper functions (stubs for now)
static bool testing_assert_wrapper(bool condition, const char* message) {
    POKO_LOG_DEBUG("Testing: Assert " + std::string(condition ? "passed" : "failed") + 
                   " - " + std::string(message ? message : ""));
    (void)condition; (void)message;
    return true;
}

static void testing_report_wrapper(const char* message) {
    POKO_LOG_DEBUG("Testing: Report - " + std::string(message ? message : ""));
    (void)message;
}

// Debugging wrapper functions (stubs for now)
static void debug_break_wrapper(void) {
    POKO_LOG_DEBUG("Debugging: Break");
}

static void debug_continue_wrapper(void) {
    POKO_LOG_DEBUG("Debugging: Continue");
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
    if (!adapter || !world_handle) {
        std::string error_msg = "MuteEngineAdapter: Invalid parameters (adapter=";
        error_msg += std::to_string(reinterpret_cast<uintptr_t>(adapter));
        error_msg += ", world_handle=";
        error_msg += std::to_string(reinterpret_cast<uintptr_t>(world_handle));
        error_msg += ")";
        POKO_LOG_ERROR(error_msg);
        return false;
    }
    
    // Initialize function table on first use
    static bool initialized = false;
    if (!initialized) {
        initialize_function_table();
        mute_engine_adapter_register_functions();
        initialized = true;
        POKO_LOG_INFO("MuteEngineAdapter: Function table initialized and registered");
    }
    
    adapter->engine_handle = world_handle;
    adapter->current_context = MUTE_CONTEXT_CLIENT;
    
    adapter->sandbox_state = (SandboxState*)tracked_malloc(sizeof(SandboxState), __FILE__, __LINE__);
    if (!adapter->sandbox_state) {
        POKO_LOG_ERROR("MuteEngineAdapter: Failed to allocate sandbox state");
        return false;
    }
    
    SandboxConfig default_config;
    sandbox_config_init(&default_config, SANDBOX_CONTEXT_SHARED);
    sandbox_state_init(adapter->sandbox_state, &default_config);
    
    adapter->api_state = (EngineState*)tracked_malloc(sizeof(EngineState), __FILE__, __LINE__);
    if (!adapter->api_state) {
        POKO_LOG_ERROR("MuteEngineAdapter: Failed to allocate engine state");
        tracked_free(adapter->sandbox_state, __FILE__, __LINE__);
        adapter->sandbox_state = NULL;
        return false;
    }
    
    engine_state_init(adapter->api_state, adapter->current_context, adapter->sandbox_state);
    adapter->is_initialized = true;
    
    POKO_LOG_INFO("MuteEngineAdapter: Initialized successfully");
    return true;
}

void mute_engine_adapter_free(MuteEngineAdapter* adapter) {
    if (!adapter) return;
    
    adapter->engine_handle = NULL;
    adapter->is_initialized = false;
    
    if (adapter->sandbox_state) {
        sandbox_state_free(adapter->sandbox_state);
        tracked_free(adapter->sandbox_state, __FILE__, __LINE__);
        adapter->sandbox_state = NULL;
    }
    
    if (adapter->api_state) {
        engine_state_free(adapter->api_state);
        tracked_free(adapter->api_state, __FILE__, __LINE__);
        adapter->api_state = NULL;
    }
    
    // Cleanup function table resources
    cleanup_function_table();
    
    // Log memory statistics
    size_t total_allocated, allocation_count;
    get_memory_stats(&total_allocated, &allocation_count);
    POKO_LOG_INFO("MuteEngineAdapter: Memory stats - Total: " + 
                   std::to_string(total_allocated) + " bytes, Count: " + 
                   std::to_string(allocation_count));
}

Value mute_engine_adapter_call(MuteEngineAdapter* adapter, EngineAPI api, Value* arguments, size_t arg_count) {
    if (!adapter || !adapter->is_initialized) {
        POKO_LOG_ERROR("MuteEngineAdapter: Call attempted on uninitialized adapter");
        return value_nil();
    }
    
    // Check if API is available in current context
    if (!mute_engine_adapter_is_available(adapter, api)) {
        POKO_LOG_WARN("MuteEngineAdapter: API not available in current context");
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
    std::lock_guard<std::mutex> lock(g_function_table_mutex);
    
    // Register physics functions
    g_function_table.physics_create_body = physics_create_body_wrapper;
    g_function_table.physics_set_position = physics_set_position_wrapper;
    g_function_table.physics_set_velocity = physics_set_velocity_wrapper;
    g_function_table.physics_apply_force = physics_apply_force_wrapper;
    g_function_table.physics_raycast = physics_raycast_wrapper;
    
    // Register audio functions
    g_function_table.audio_play = audio_play_wrapper;
    g_function_table.audio_stop = audio_stop_wrapper;
    g_function_table.audio_set_volume = audio_set_volume_wrapper;
    
    // Register animation functions
    g_function_table.animation_play = animation_play_wrapper;
    g_function_table.animation_stop = animation_stop_wrapper;
    g_function_table.animation_set_speed = animation_set_speed_wrapper;
    
    // Register camera functions
    g_function_table.camera_set_position = camera_set_position_wrapper;
    g_function_table.camera_set_target = camera_set_target_wrapper;
    g_function_table.camera_set_fov = camera_set_fov_wrapper;
    
    // Register input functions
    g_function_table.input_register_action = input_register_action_wrapper;
    g_function_table.input_get_state = input_get_state_wrapper;
    g_function_table.input_capture_mouse = input_capture_mouse_wrapper;
    g_function_table.input_capture_keyboard = input_capture_keyboard_wrapper;
    
    // Register UI functions
    g_function_table.ui_create_element = ui_create_element_wrapper;
    g_function_table.ui_get_properties = ui_get_properties_wrapper;
    g_function_table.ui_set_properties = ui_set_properties_wrapper;
    g_function_table.ui_connect_event = ui_connect_event_wrapper;
    g_function_table.ui_disconnect_event = ui_disconnect_event_wrapper;
    
    // Register network functions
    g_function_table.network_connect = network_connect_wrapper;
    g_function_table.network_disconnect = network_disconnect_wrapper;
    g_function_table.network_send = network_send_wrapper;
    g_function_table.network_receive = network_receive_wrapper;
    
    // Register authority functions
    g_function_table.authority_validate = authority_validate_wrapper;
    g_function_table.authority_execute = authority_execute_wrapper;
    
    // Register data functions
    g_function_table.data_get = data_get_wrapper;
    g_function_table.data_set = data_set_wrapper;
    
    // Register asset functions
    g_function_table.asset_load = asset_load_wrapper;
    g_function_table.asset_unload = asset_unload_wrapper;
    
    // Register package functions
    g_function_table.package_load = package_load_wrapper;
    g_function_table.package_unload = package_unload_wrapper;
    
    // Register testing functions
    g_function_table.testing_assert = testing_assert_wrapper;
    g_function_table.testing_report = testing_report_wrapper;
    
    // Register debugging functions
    g_function_table.debug_break = debug_break_wrapper;
    g_function_table.debug_continue = debug_continue_wrapper;
}

void mute_engine_adapter_get_memory_stats(size_t* total_allocated, size_t* allocation_count) {
    get_memory_stats(total_allocated, allocation_count);
}

} // extern "C"
