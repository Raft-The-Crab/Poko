/**
 * @file ui_element.cpp
 * @brief UI element management
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "ui/ui.h"
#include "core/logging/logger.h"
#include <cstring>
#include <map>
#include <mutex>

namespace poko {
namespace ui {

// UI element implementation
struct UIElementImpl {
    uint64_t id;
    std::string name;
    UIElementType type;
    UIElementImpl* parent;
    UIElementImpl* children;
    UIElementImpl* next;
    UIElementImpl* prev;
    float x;
    float y;
    float width;
    float height;
    bool visible;
    bool enabled;
    void* user_data;
};

// UI manager
struct UIManagerImpl {
    std::map<uint64_t, UIElementImpl*> elements;
    std::map<std::string, UIElementImpl*> name_map;
    std::mutex mutex;
    uint64_t next_element_id;
};

// Convert to internal implementation
static UIElementImpl* to_element_impl(UIElement* element) {
    return reinterpret_cast<UIElementImpl*>(element);
}

static const UIElementImpl* to_element_impl(const UIElement* element) {
    return reinterpret_cast<const UIElementImpl*>(element);
}

static UIElement* from_element_impl(UIElementImpl* impl) {
    return reinterpret_cast<UIElement*>(impl);
}

} // namespace ui
} // namespace poko

// C API implementation
extern "C" {

using namespace poko::ui;

UIElement* ui_element_create(const char* name, UIElementType type, UIElement* parent) {
    if (!name) return nullptr;
    
    static UIManagerImpl* manager = nullptr;
    if (!manager) {
        manager = new UIManagerImpl();
        manager->next_element_id = 1;
    }
    
    std::lock_guard<std::mutex> lock(manager->mutex);
    
    UIElementImpl* element = new UIElementImpl();
    element->id = manager->next_element_id++;
    element->name = name;
    element->type = type;
    element->parent = to_element_impl(parent);
    element->children = nullptr;
    element->next = nullptr;
    element->prev = nullptr;
    element->x = 0.0f;
    element->y = 0.0f;
    element->width = 100.0f;
    element->height = 100.0f;
    element->visible = true;
    element->enabled = true;
    element->user_data = nullptr;
    
    // Add to parent's children list
    if (element->parent) {
        if (element->parent->children) {
            element->parent->children->prev = element;
            element->next = element->parent->children;
            element->parent->children = element;
        } else {
            element->parent->children = element;
        }
    }
    
    // Add to maps
    manager->elements[element->id] = element;
    manager->name_map[name] = element;
    
    POKO_LOG_INFO("UI: Created element '" + std::string(name) + "' with type " + std::to_string(type));
    return from_element_impl(element);
}

void ui_element_destroy(UIElement* element) {
    if (!element) return;
    
    static UIManagerImpl* manager = nullptr;
    if (!manager) return;
    
    std::lock_guard<std::mutex> lock(manager->mutex);
    
    UIElementImpl* impl = to_element_impl(element);
    
    // Remove from parent's children list
    if (impl->parent) {
        if (impl->prev) {
            impl->prev->next = impl->next;
        } else {
            impl->parent->children = impl->next;
        }
        if (impl->next) {
            impl->next->prev = impl->prev;
        }
    }
    
    // Destroy children
    UIElementImpl* child = impl->children;
    while (child) {
        UIElementImpl* next = child->next;
        ui_element_destroy(from_element_impl(child));
        child = next;
    }
    
    // Remove from maps
    manager->elements.erase(impl->id);
    manager->name_map.erase(impl->name);
    
    delete impl;
    
    POKO_LOG_DEBUG("UI: Destroyed element '" + impl->name + "'");
}

bool ui_element_get_properties(const UIElement* element, float* x, float* y, float* width, float* height) {
    if (!element) return false;
    
    const UIElementImpl* impl = to_element_impl(element);
    if (x) *x = impl->x;
    if (y) *y = impl->y;
    if (width) *width = impl->width;
    if (height) *height = impl->height;
    
    return true;
}

bool ui_element_set_properties(UIElement* element, float x, float y, float width, float height) {
    if (!element) return false;
    
    UIElementImpl* impl = to_element_impl(element);
    impl->x = x;
    impl->y = y;
    impl->width = std::max(0.0f, width);
    impl->height = std::max(0.0f, height);
    
    return true;
}

void ui_element_set_visible(UIElement* element, bool visible) {
    if (!element) return;
    
    UIElementImpl* impl = to_element_impl(element);
    impl->visible = visible;
}

bool ui_element_is_visible(const UIElement* element) {
    if (!element) return false;
    
    const UIElementImpl* impl = to_element_impl(element);
    return impl->visible;
}

void ui_element_set_enabled(UIElement* element, bool enabled) {
    if (!element) return;
    
    UIElementImpl* impl = to_element_impl(element);
    impl->enabled = enabled;
}

bool ui_element_is_enabled(const UIElement* element) {
    if (!element) return false;
    
    const UIElementImpl* impl = to_element_impl(element);
    return impl->enabled;
}

void ui_element_set_user_data(UIElement* element, void* user_data) {
    if (!element) return;
    
    UIElementImpl* impl = to_element_impl(element);
    impl->user_data = user_data;
}

void* ui_element_get_user_data(const UIElement* element) {
    if (!element) return nullptr;
    
    const UIElementImpl* impl = to_element_impl(element);
    return impl->user_data;
}

} // extern "C"