/**
 * @file mute_binding.cpp
 * @brief C binding implementation for Mute ↔ Engine integration
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "binding/mute_binding.h"
#include "runtime/instance.h"
#include <cstring>
#include <map>
#include <mutex>

namespace poko {
namespace binding {

// Map from opaque handles to actual Instance pointers
static std::map<void*, runtime::Instance*> instance_map;
static std::mutex instance_map_mutex;

// Convert Instance pointer to opaque handle
static void* instance_to_handle(runtime::Instance* instance) {
    return static_cast<void*>(instance);
}

// Convert opaque handle to Instance pointer
static runtime::Instance* handle_to_instance(void* handle) {
    std::lock_guard<std::mutex> lock(instance_map_mutex);
    auto it = instance_map.find(handle);
    if (it != instance_map.end()) {
        return it->second;
    }
    return nullptr;
}

// Register instance in map
static void register_instance(void* handle, runtime::Instance* instance) {
    std::lock_guard<std::mutex> lock(instance_map_mutex);
    instance_map[handle] = instance;
}

// Unregister instance from map
static void unregister_instance(void* handle) {
    std::lock_guard<std::mutex> lock(instance_map_mutex);
    instance_map.erase(handle);
}

} // namespace binding
} // namespace poko

// C API implementation
extern "C" {

using namespace poko::binding;
using namespace poko::runtime;

void mute_binding_init(void) {
    // Initialize binding layer
}

void mute_binding_shutdown(void) {
    // Clean up all instances
    std::lock_guard<std::mutex> lock(poko::binding::instance_map_mutex);
    for (auto& pair : poko::binding::instance_map) {
        delete pair.second;
    }
    poko::binding::instance_map.clear();
}

InstanceHandle mute_instance_create(const char* name, int type) {
    poko::runtime::InstanceType instance_type = static_cast<poko::runtime::InstanceType>(type);
    poko::runtime::Instance* instance = new poko::runtime::Instance(name ? name : "", instance_type);

    void* handle = poko::binding::instance_to_handle(instance);
    poko::binding::register_instance(handle, instance);

    return handle;
}

void mute_instance_destroy(InstanceHandle handle) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    if (instance) {
        poko::binding::unregister_instance(handle);
        delete instance;
    }
}

uint64_t mute_instance_get_id(InstanceHandle handle) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    if (instance) {
        return instance->id();
    }
    return 0;
}

char* mute_instance_get_name(InstanceHandle handle) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    if (instance) {
        const std::string& name = instance->name();
        char* result = new char[name.length() + 1];
        std::strcpy(result, name.c_str());
        return result;
    }
    return nullptr;
}

void mute_instance_set_name(InstanceHandle handle, const char* name) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    if (instance && name) {
        instance->set_name(name);
    }
}

void mute_instance_set_parent(InstanceHandle handle, InstanceHandle parent_handle) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    poko::runtime::Instance* parent = poko::binding::handle_to_instance(parent_handle);
    if (instance) {
        instance->set_parent(parent);
    }
}

InstanceHandle mute_instance_get_parent(InstanceHandle handle) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    if (instance) {
        poko::runtime::Instance* parent = instance->parent();
        return poko::binding::instance_to_handle(parent);
    }
    return nullptr;
}

void mute_instance_add_child(InstanceHandle handle, InstanceHandle child_handle) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    poko::runtime::Instance* child = poko::binding::handle_to_instance(child_handle);
    if (instance && child) {
        instance->add_child(child);
    }
}

void mute_instance_remove_child(InstanceHandle handle, InstanceHandle child_handle) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    poko::runtime::Instance* child = poko::binding::handle_to_instance(child_handle);
    if (instance && child) {
        instance->remove_child(child);
    }
}

int mute_instance_get_child_count(InstanceHandle handle) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    if (instance) {
        return static_cast<int>(instance->children().size());
    }
    return 0;
}

InstanceHandle mute_instance_get_child(InstanceHandle handle, int index) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    if (instance) {
        const auto& children = instance->children();
        if (index >= 0 && index < static_cast<int>(children.size())) {
            return poko::binding::instance_to_handle(children[index]);
        }
    }
    return nullptr;
}

void mute_instance_set_attribute(InstanceHandle handle, const char* key, const char* value) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    if (instance && key && value) {
        instance->set_attribute(key, value);
    }
}

char* mute_instance_get_attribute(InstanceHandle handle, const char* key) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    if (instance && key) {
        std::string value = instance->get_attribute(key);
        if (!value.empty()) {
            char* result = new char[value.length() + 1];
            std::strcpy(result, value.c_str());
            return result;
        }
    }
    return nullptr;
}

int mute_instance_has_attribute(InstanceHandle handle, const char* key) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    if (instance && key) {
        return instance->has_attribute(key) ? 1 : 0;
    }
    return 0;
}

void mute_instance_remove_attribute(InstanceHandle handle, const char* key) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    if (instance && key) {
        instance->remove_attribute(key);
    }
}

void mute_instance_add_tag(InstanceHandle handle, const char* tag) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    if (instance && tag) {
        instance->add_tag(tag);
    }
}

void mute_instance_remove_tag(InstanceHandle handle, const char* tag) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    if (instance && tag) {
        instance->remove_tag(tag);
    }
}

int mute_instance_has_tag(InstanceHandle handle, const char* tag) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    if (instance && tag) {
        return instance->has_tag(tag) ? 1 : 0;
    }
    return 0;
}

int mute_instance_get_tag_count(InstanceHandle handle) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    if (instance) {
        return static_cast<int>(instance->tags().size());
    }
    return 0;
}

char* mute_instance_get_tag(InstanceHandle handle, int index) {
    poko::runtime::Instance* instance = poko::binding::handle_to_instance(handle);
    if (instance) {
        const auto& tags = instance->tags();
        if (index >= 0 && index < static_cast<int>(tags.size())) {
            const std::string& tag = tags[index];
            char* result = new char[tag.length() + 1];
            std::strcpy(result, tag.c_str());
            return result;
        }
    }
    return nullptr;
}

} // extern "C"
