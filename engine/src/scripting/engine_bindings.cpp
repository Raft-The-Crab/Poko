/**
 * @file engine_bindings.cpp
 * @brief Engine API bindings implementation for Mute scripts
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-21
 */

#include "scripting/engine_bindings.h"
#include "core/engine/engine.h"
#include "input/input.h"
#include "core/window/window.h"
#include <iostream>

namespace poko {

// Static engine instance pointer
static void* g_engine_instance = nullptr;

void set_engine_instance(void* engine) {
    g_engine_instance = engine;
}

void* get_engine_instance() {
    return g_engine_instance;
}

void register_engine_bindings(MuteVM* vm, void* engine) {
    set_engine_instance(engine);
    
    // Register engine functions
    mute_register_c_function(vm, "exit", engine_exit);
    mute_register_c_function(vm, "get_delta_time", engine_get_delta_time);
    mute_register_c_function(vm, "get_fps", engine_get_fps);
    mute_register_c_function(vm, "get_total_time", engine_get_total_time);
    mute_register_c_function(vm, "is_key_down", engine_is_key_down);
    mute_register_c_function(vm, "is_key_pressed", engine_is_key_pressed);
    mute_register_c_function(vm, "get_mouse_position", engine_get_mouse_position);
    mute_register_c_function(vm, "set_window_title", engine_set_window_title);
    mute_register_c_function(vm, "get_window_width", engine_get_window_width);
    mute_register_c_function(vm, "get_window_height", engine_get_window_height);
}

// Binding implementations

int engine_exit(void* vm) {
    MuteVMContext* ctx = (MuteVMContext*)vm;
    
    // Set engine to exit
    if (g_engine_instance) {
        Engine* engine = static_cast<Engine*>(g_engine_instance);
        engine->set_should_exit();
        mute_vm_push(ctx, mute_value_bool(true));
        return 1;
    }
    
    mute_vm_push(ctx, mute_value_bool(false));
    return 1;
}

int engine_get_delta_time(void* vm) {
    MuteVMContext* ctx = (MuteVMContext*)vm;
    
    if (g_engine_instance) {
        Engine* engine = static_cast<Engine*>(g_engine_instance);
        double delta_time = engine->get_frame_time();
        mute_vm_push(ctx, mute_value_number(delta_time));
        return 1;
    }
    
    mute_vm_push(ctx, mute_value_number(0.0));
    return 1;
}

int engine_get_fps(void* vm) {
    MuteVMContext* ctx = (MuteVMContext*)vm;
    
    if (g_engine_instance) {
        Engine* engine = static_cast<Engine*>(g_engine_instance);
        double fps = engine->get_fps();
        mute_vm_push(ctx, mute_value_number(fps));
        return 1;
    }
    
    mute_vm_push(ctx, mute_value_number(0.0));
    return 1;
}

int engine_get_total_time(void* vm) {
    MuteVMContext* ctx = (MuteVMContext*)vm;
    
    if (g_engine_instance) {
        Engine* engine = static_cast<Engine*>(g_engine_instance);
        double total_time = engine->get_total_time();
        mute_vm_push(ctx, mute_value_number(total_time));
        return 1;
    }
    
    mute_vm_push(ctx, mute_value_number(0.0));
    return 1;
}

int engine_is_key_down(void* vm) {
    MuteVMContext* ctx = (MuteVMContext*)vm;
    
    // Get key code from stack
    MuteValue key_value = mute_vm_pop(ctx);
    if (key_value.type != MUTE_TYPE_NUMBER) {
        mute_vm_push(ctx, mute_value_bool(false));
        return 1;
    }
    
    int key_code = static_cast<int>(key_value.value.as_number);
    
    if (g_engine_instance) {
        Engine* engine = static_cast<Engine*>(g_engine_instance);
        Input* input = engine->get_input();
        if (input) {
            bool is_down = input->is_key_down(static_cast<KeyCode>(key_code));
            mute_vm_push(ctx, mute_value_bool(is_down));
            return 1;
        }
    }
    
    mute_vm_push(ctx, mute_value_bool(false));
    return 1;
}

int engine_is_key_pressed(void* vm) {
    MuteVMContext* ctx = (MuteVMContext*)vm;
    
    // Get key code from stack
    MuteValue key_value = mute_vm_pop(ctx);
    if (key_value.type != MUTE_TYPE_NUMBER) {
        mute_vm_push(ctx, mute_value_bool(false));
        return 1;
    }
    
    int key_code = static_cast<int>(key_value.value.as_number);
    
    if (g_engine_instance) {
        Engine* engine = static_cast<Engine*>(g_engine_instance);
        Input* input = engine->get_input();
        if (input) {
            bool is_pressed = input->is_key_pressed(static_cast<KeyCode>(key_code));
            mute_vm_push(ctx, mute_value_bool(is_pressed));
            return 1;
        }
    }
    
    mute_vm_push(ctx, mute_value_bool(false));
    return 1;
}

int engine_get_mouse_position(void* vm) {
    MuteVMContext* ctx = (MuteVMContext*)vm;
    
    if (g_engine_instance) {
        Engine* engine = static_cast<Engine*>(g_engine_instance);
        Input* input = engine->get_input();
        if (input) {
            auto pos = input->get_mouse_position();
            
            MuteTable* result = mute_table_create(2, NULL);
            if (!result) {
                mute_vm_push(ctx, mute_value_nil());
                return 1;
            }
            
            MuteValue key_x; key_x.type = MUTE_TYPE_STRING;
            key_x.value.as_string = mute_string_create("x", 1, 0, NULL);
            
            MuteValue key_y; key_y.type = MUTE_TYPE_STRING;
            key_y.value.as_string = mute_string_create("y", 1, 0, NULL);
            
            mute_table_set(result, key_x, mute_value_number(pos.first));
            mute_table_set(result, key_y, mute_value_number(pos.second));
            
            MuteValue result_value;
            result_value.type = MUTE_TYPE_TABLE;
            result_value.value.as_table = result;
            mute_vm_push(ctx, result_value);
            return 1;
        }
    }
    
    mute_vm_push(ctx, mute_value_nil());
    return 1;
}

int engine_set_window_title(void* vm) {
    MuteVMContext* ctx = (MuteVMContext*)vm;
    
    // Get title from stack
    MuteValue title_value = mute_vm_pop(ctx);
    if (title_value.type != MUTE_TYPE_STRING) {
        mute_vm_push(ctx, mute_value_bool(false));
        return 1;
    }
    
    const char* title = title_value.value.as_string->chars;
    
    if (g_engine_instance) {
        Engine* engine = static_cast<Engine*>(g_engine_instance);
        Window* window = engine->get_window();
        if (window) {
            window->set_title(title);
            mute_vm_push(ctx, mute_value_bool(true));
            return 1;
        }
    }
    
    mute_vm_push(ctx, mute_value_bool(false));
    return 1;
}

int engine_get_window_width(void* vm) {
    MuteVMContext* ctx = (MuteVMContext*)vm;
    
    if (g_engine_instance) {
        Engine* engine = static_cast<Engine*>(g_engine_instance);
        Window* window = engine->get_window();
        if (window) {
            int width = window->get_width();
            mute_vm_push(ctx, mute_value_number(width));
            return 1;
        }
    }
    
    mute_vm_push(ctx, mute_value_number(0));
    return 1;
}

int engine_get_window_height(void* vm) {
    MuteVMContext* ctx = (MuteVMContext*)vm;
    
    if (g_engine_instance) {
        Engine* engine = static_cast<Engine*>(g_engine_instance);
        Window* window = engine->get_window();
        if (window) {
            int height = window->get_height();
            mute_vm_push(ctx, mute_value_number(height));
            return 1;
        }
    }
    
    mute_vm_push(ctx, mute_value_number(0));
    return 1;
}

} // namespace poko