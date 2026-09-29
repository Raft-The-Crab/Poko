/**
 * @file instance.h
 * @brief Runtime object system header
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_RUNTIME_INSTANCE_H
#define POKO_CORE_RUNTIME_INSTANCE_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include "core/handles/handle.h"

namespace poko {
namespace core {
namespace runtime {

// ============================================================================
// Type Definitions
// ============================================================================

/**
 * @brief Instance type identifier
 */
using InstanceType = uint32_t;

/**
 * @brief Instance lifecycle state
 */
enum class InstanceState : uint8_t {
    Created = 0,      ///< Instance created but not initialized
    Initializing = 1, ///< Instance is being initialized
    Active = 2,       ///< Instance is active and running
    Deactivating = 3, ///< Instance is being deactivated
    Destroyed = 4,     ///< Instance has been destroyed
    Error = 5          ///< Instance is in error state
};

/**
 * @brief Invalid instance type constant
 */
constexpr InstanceType INVALID_INSTANCE_TYPE = 0;

/**
 * @brief Maximum instance name length (for safety)
 */
constexpr size_t MAX_INSTANCE_NAME_LENGTH = 256;

/**
 * @brief Maximum property key length (for safety)
 */
constexpr size_t MAX_PROPERTY_KEY_LENGTH = 128;

/**
 * @brief Maximum property value length (for safety)
 */
constexpr size_t MAX_PROPERTY_VALUE_LENGTH = 1024;

/**
 * @brief Maximum tag length (for safety)
 */
constexpr size_t MAX_TAG_LENGTH = 64;

/**
 * @brief Maximum tags per instance (for safety)
 */
constexpr size_t MAX_TAGS_PER_INSTANCE = 64;

/**
 * @brief Maximum children per instance (for safety)
 */
constexpr size_t MAX_CHILDREN_PER_INSTANCE = 128;

// ============================================================================
// Instance Class
// ============================================================================

/**
 * @brief Runtime instance - creator-facing object abstraction
 * 
 * Instances represent game objects in the scene hierarchy.
 * They contain metadata, properties, and lifecycle state.
 * 
 * @section structure Instance Structure
 * - Stable internal ID (for lookups)
 * - Class/type information
 * - Name (human-readable identifier)
 * - Parent/children hierarchy
 * - Properties (key-value data)
 * - Attributes (engine flags)
 * - Tags (user-defined labels)
 * - Lifecycle state
 * 
 * @section performance Performance
 * - Instance hierarchy is separate from heavy data storage
 * - Runtime storage may use compact component/archetype-like layouts
 * - Do not expose ECS terminology to game creators
 */
class Instance {
public:
    /**
     * @brief Constructor
     * @param type Instance type
     * @param name Instance name
     */
    Instance(InstanceType type, const std::string& name);
    
    /**
     * @brief Destructor
     */
    ~Instance();
    
    /**
     * @brief Copy constructor (deleted)
     */
    Instance(const Instance&) = delete;
    
    /**
     * @brief Copy assignment (deleted)
     */
    Instance& operator=(const Instance&) = delete;
    
    /**
     * @brief Move constructor
     */
    Instance(Instance&&) noexcept = default;
    
    /**
     * @brief Move assignment
     */
    Instance& operator=(Instance&&) noexcept = default;
    
    // ============================================================================
    // Accessors
    // ============================================================================
    
    /**
     * @brief Get instance ID
     * @return Instance handle
     */
    [[nodiscard]] handles::Handle getId() const noexcept;
    
    /**
     * @brief Get instance type
     * @return Instance type
     */
    [[nodiscard]] InstanceType getType() const noexcept;
    
    /**
     * @brief Get instance name
     * @return Instance name
     */
    [[nodiscard]] const std::string& getName() const noexcept;
    
    /**
     * @brief Set instance name
     * @param name New name
     */
    void setName(const std::string& name);
    
    /**
     * @brief Set instance name (move overload)
     * @param name New name (moved)
     */
    void setName(std::string&& name) noexcept;
    
    /**
     * @brief Get lifecycle state
     * @return Current state
     */
    [[nodiscard]] InstanceState getState() const noexcept;
    
    /**
     * @brief Set lifecycle state
     * @param state New state
     */
    void setState(InstanceState state) noexcept;
    
    /**
     * @brief Get parent instance
     * @return Parent handle (null if no parent)
     */
    [[nodiscard]] handles::Handle getParent() const noexcept;
    
    /**
     * @brief Set parent instance
     * @param parent Parent handle
     */
    void setParent(handles::Handle parent) noexcept;
    
    /**
     * @brief Get children
     * @return Vector of child handles
     */
    [[nodiscard]] const std::vector<handles::Handle>& getChildren() const noexcept;
    
    /**
     * @brief Add child instance
     * @param child Child handle
     */
    void addChild(handles::Handle child) noexcept;
    
    /**
     * @brief Remove child instance
     * @param child Child handle
     */
    void removeChild(handles::Handle child) noexcept;
    
    // ============================================================================
    // Properties
    // ============================================================================
    
    /**
     * @brief Set a property
     * @param key Property key
     * @param value Property value
     */
    void setProperty(const std::string& key, const std::string& value);
    
    /**
     * @brief Set a property (move overload)
     * @param key Property key (moved)
     * @param value Property value (moved)
     */
    void setProperty(std::string&& key, std::string&& value);
    
    /**
     * @brief Get a property
     * @param key Property key
     * @return Property value (empty if not found)
     */
    [[nodiscard]] std::string getProperty(const std::string& key) const noexcept;
    
    /**
     * @brief Check if property exists
     * @param key Property key
     * @return True if property exists
     */
    [[nodiscard]] bool hasProperty(const std::string& key) const noexcept;
    
    /**
     * @brief Remove a property
     * @param key Property key
     */
    void removeProperty(const std::string& key) noexcept;
    
    // ============================================================================
    // Tags
    // ============================================================================
    
    /**
     * @brief Add a tag
     * @param tag Tag to add
     */
    void addTag(const std::string& tag);
    
    /**
     * @brief Add a tag (move overload)
     * @param tag Tag to add (moved)
     */
    void addTag(std::string&& tag) noexcept;
    
    /**
     * @brief Remove a tag
     * @param tag Tag to remove
     */
    void removeTag(const std::string& tag) noexcept;
    
    /**
     * @brief Check if has tag
     * @param tag Tag to check
     * @return True if has tag
     */
    [[nodiscard]] bool hasTag(const std::string& tag) const noexcept;
    
    /**
     * @brief Get all tags
     * @return Vector of tags
     */
    [[nodiscard]] const std::vector<std::string>& getTags() const noexcept;
    
private:
    handles::Handle m_id;
    InstanceType m_type;
    std::string m_name;
    InstanceState m_state;
    handles::Handle m_parent;
    std::vector<handles::Handle> m_children;
    std::unordered_map<std::string, std::string> m_properties;
    std::vector<std::string> m_tags;
};

// ============================================================================
// Instance Factory
// ============================================================================

/**
 * @brief Create a new instance
 * @param type Instance type
 * @param name Instance name
 * @return Unique pointer to instance
 */
[[nodiscard]] std::unique_ptr<Instance> createInstance(InstanceType type, const std::string& name);

} // namespace runtime
} // namespace core
} // namespace poko

#endif // POKO_CORE_RUNTIME_INSTANCE_H
