/**
 * @file mute_binding.h
 * @brief C binding layer for Mute ↔ Engine integration
 * @details Provides C-compatible API for Mute scripts to interact with the engine
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#ifndef MUTE_BINDING_H
#define MUTE_BINDING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @typedef InstanceHandle
 * @brief Opaque handle for engine instances
 */
typedef void* InstanceHandle;

/**
 * @brief Initialize the Mute binding layer
 */
void mute_binding_init(void);

/**
 * @brief Shutdown the Mute binding layer
 */
void mute_binding_shutdown(void);

/**
 * @brief Create a new instance
 * @param name Instance name
 * @param type Instance type (0=Base, 1=Model, 2=Part, 3=Mesh, 4=Light, 5=Camera, 6=Script, 7=UIElement, 8=Custom)
 * @return Handle to the created instance, or NULL on failure
 */
InstanceHandle mute_instance_create(const char* name, int type);

/**
 * @brief Destroy an instance
 * @param handle Instance handle
 */
void mute_instance_destroy(InstanceHandle handle);

/**
 * @brief Get instance ID
 * @param handle Instance handle
 * @return Instance ID, or 0 if invalid
 */
uint64_t mute_instance_get_id(InstanceHandle handle);

/**
 * @brief Get instance name
 * @param handle Instance handle
 * @return Instance name (caller must free), or NULL if invalid
 */
char* mute_instance_get_name(InstanceHandle handle);

/**
 * @brief Set instance name
 * @param handle Instance handle
 * @param name New name
 */
void mute_instance_set_name(InstanceHandle handle, const char* name);

/**
 * @brief Set instance parent
 * @param handle Instance handle
 * @param parent_handle Parent instance handle (NULL for no parent)
 */
void mute_instance_set_parent(InstanceHandle handle, InstanceHandle parent_handle);

/**
 * @brief Get instance parent
 * @param handle Instance handle
 * @return Parent handle, or NULL if no parent
 */
InstanceHandle mute_instance_get_parent(InstanceHandle handle);

/**
 * @brief Add child to instance
 * @param handle Instance handle
 * @param child_handle Child instance handle
 */
void mute_instance_add_child(InstanceHandle handle, InstanceHandle child_handle);

/**
 * @brief Remove child from instance
 * @param handle Instance handle
 * @param child_handle Child instance handle
 */
void mute_instance_remove_child(InstanceHandle handle, InstanceHandle child_handle);

/**
 * @brief Get child count
 * @param handle Instance handle
 * @return Number of children
 */
int mute_instance_get_child_count(InstanceHandle handle);

/**
 * @brief Get child at index
 * @param handle Instance handle
 * @param index Child index
 * @return Child handle, or NULL if invalid
 */
InstanceHandle mute_instance_get_child(InstanceHandle handle, int index);

/**
 * @brief Set instance attribute
 * @param handle Instance handle
 * @param key Attribute key
 * @param value Attribute value
 */
void mute_instance_set_attribute(InstanceHandle handle, const char* key, const char* value);

/**
 * @brief Get instance attribute
 * @param handle Instance handle
 * @param key Attribute key
 * @return Attribute value (caller must free), or NULL if not found
 */
char* mute_instance_get_attribute(InstanceHandle handle, const char* key);

/**
 * @brief Check if instance has attribute
 * @param handle Instance handle
 * @param key Attribute key
 * @return 1 if has attribute, 0 otherwise
 */
int mute_instance_has_attribute(InstanceHandle handle, const char* key);

/**
 * @brief Remove instance attribute
 * @param handle Instance handle
 * @param key Attribute key
 */
void mute_instance_remove_attribute(InstanceHandle handle, const char* key);

/**
 * @brief Add tag to instance
 * @param handle Instance handle
 * @param tag Tag to add
 */
void mute_instance_add_tag(InstanceHandle handle, const char* tag);

/**
 * @brief Remove tag from instance
 * @param handle Instance handle
 * @param tag Tag to remove
 */
void mute_instance_remove_tag(InstanceHandle handle, const char* tag);

/**
 * @brief Check if instance has tag
 * @param handle Instance handle
 * @param tag Tag to check
 * @return 1 if has tag, 0 otherwise
 */
int mute_instance_has_tag(InstanceHandle handle, const char* tag);

/**
 * @brief Get tag count
 * @param handle Instance handle
 * @return Number of tags
 */
int mute_instance_get_tag_count(InstanceHandle handle);

/**
 * @brief Get tag at index
 * @param handle Instance handle
 * @param index Tag index
 * @return Tag (caller must free), or NULL if invalid
 */
char* mute_instance_get_tag(InstanceHandle handle, int index);

#ifdef __cplusplus
}
#endif

#endif // MUTE_BINDING_H
