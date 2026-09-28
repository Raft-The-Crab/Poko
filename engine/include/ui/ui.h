/**
 * @file ui.h
 * @brief UI system interface for Poko Engine
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#ifndef POKO_UI_H
#define POKO_UI_H

#ifdef __cplusplus
extern "C" {
#endif

// UI element handle (opaque)
typedef struct UIElement UIElement;

// UI element types
typedef enum {
    UI_ELEMENT_CONTAINER = 0,
    UI_ELEMENT_BUTTON = 1,
    UI_ELEMENT_TEXT = 2,
    UI_ELEMENT_IMAGE = 3,
    UI_ELEMENT_SLIDER = 4,
    UI_ELEMENT_CHECKBOX = 5,
    UI_ELEMENT_DROPDOWN = 6,
    UI_ELEMENT_TEXT_INPUT = 7,
    UI_ELEMENT_PROGRESS_BAR = 8,
    UI_ELEMENT_PANEL = 9
} UIElementType;

/**
 * Create a UI element
 * @param name Element name
 * @param type Element type
 * @param parent Parent element (or NULL for root)
 * @return UI element handle, or NULL on failure
 */
UIElement* ui_element_create(const char* name, UIElementType type, UIElement* parent);

/**
 * Destroy a UI element
 * @param element UI element handle
 */
void ui_element_destroy(UIElement* element);

/**
 * Get element properties
 * @param element UI element handle
 * @param x Output X position
 * @param y Output Y position
 * @param width Output width
 * @param height Output height
 * @return true on success, false on failure
 */
bool ui_element_get_properties(const UIElement* element, float* x, float* y, float* width, float* height);

/**
 * Set element properties
 * @param element UI element handle
 * @param x X position
 * @param y Y position
 * @param width Width
 * @param height Height
 * @return true on success, false on failure
 */
bool ui_element_set_properties(UIElement* element, float x, float y, float width, float height);

/**
 * Set element visibility
 * @param element UI element handle
 * @param visible true to show, false to hide
 */
void ui_element_set_visible(UIElement* element, bool visible);

/**
 * Check if element is visible
 * @param element UI element handle
 * @return true if visible, false otherwise
 */
bool ui_element_is_visible(const UIElement* element);

/**
 * Set element enabled state
 * @param element UI element handle
 * @param enabled true to enable, false to disable
 */
void ui_element_set_enabled(UIElement* element, bool enabled);

/**
 * Check if element is enabled
 * @param element UI element handle
 * @return true if enabled, false otherwise
 */
bool ui_element_is_enabled(const UIElement* element);

/**
 * Set user data for element
 * @param element UI element handle
 * @param user_data User data pointer
 */
void ui_element_set_user_data(UIElement* element, void* user_data);

/**
 * Get user data from element
 * @param element UI element handle
 * @return User data pointer
 */
void* ui_element_get_user_data(const UIElement* element);

#ifdef __cplusplus
}
#endif

#endif // POKO_UI_H