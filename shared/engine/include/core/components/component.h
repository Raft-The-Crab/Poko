/**
 * @file component.h
 * @brief Runtime component system header
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 * 
 * This module provides a composition-based component system for runtime objects,
 * with type-safe factories, lifecycle management, and integration with the Instance system.
 */

#ifndef POKO_CORE_COMPONENTS_COMPONENT_H
#define POKO_CORE_COMPONENTS_COMPONENT_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include <mutex>
#include <typeindex>

#include "core/handles/handle.h"

namespace poko {
namespace core {
namespace components {

// ============================================================================
// Component ID
// ============================================================================

/**
 * @brief Unique identifier for component types
 */
using ComponentID = uint32_t;

/**
 * @brief Invalid component ID constant
 */
constexpr ComponentID INVALID_COMPONENT_ID = 0;

// ============================================================================
// Component Lifecycle State
// ============================================================================

/**
 * @brief Lifecycle state of a component
 */
enum class ComponentState : uint32_t {
    None = 0,           ///< Component not initialized
    Created,            ///< Component created but not active
    Activating,         ///< Component is activating
    Active,             ///< Component is active and updating
    Deactivating,       ///< Component is deactivating
    Destroyed           ///< Component destroyed
};

// ============================================================================
// Component Base Class
// ============================================================================

/**
 * @brief Base class for all components
 * 
 * Components are composition units that can be attached to Instances.
 * Each component type has a unique ComponentID for type-safe operations.
 */
class Component {
public:
    Component() = default;
    virtual ~Component() = default;
    
    // Copy/move - components should generally not be copied
    Component(const Component&) = delete;
    Component& operator=(const Component&) = delete;
    Component(Component&&) = delete;
    Component& operator=(Component&&) = delete;
    
    /**
     * @brief Get component type ID
     */
    [[nodiscard]] virtual ComponentID getTypeID() const noexcept = 0;
    
    /**
     * @brief Get component type name
     */
    [[nodiscard]] virtual const char* getTypeName() const noexcept = 0;
    
    /**
     * @brief Called when component is created
     */
    virtual void onCreate() {}
    
    /**
     * @brief Called when component is activated
     */
    virtual void onActivate() {}
    
    /**
     * @brief Called when component is deactivated
     */
    virtual void onDeactivate() {}
    
    /**
     * @brief Called when component is destroyed
     */
    virtual void onDestroy() {}
    
    /**
     * @brief Get component state
     */
    [[nodiscard]] ComponentState getState() const noexcept { return m_state; }
    
    /**
     * @brief Set component state
     */
    void setState(ComponentState state) noexcept { m_state = state; }
    
    /**
     * @brief Check if component is active
     */
    [[nodiscard]] bool isActive() const noexcept { return m_state == ComponentState::Active; }
    
protected:
    ComponentState m_state = ComponentState::None;
};

// ============================================================================
// Component Factory
// ============================================================================

/**
 * @brief Factory function for creating components
 */
using ComponentFactory = std::function<std::unique_ptr<Component>()>;

// ============================================================================
// Component Registry
// ============================================================================

/**
 * @brief Registry for component types and factories
 * 
 * Manages component type registration and provides factory functions
 * for creating component instances.
 */
class ComponentRegistry {
public:
    ComponentRegistry() = default;
    ~ComponentRegistry() = default;
    
    // Delete copy/move (registry should be unique)
    ComponentRegistry(const ComponentRegistry&) = delete;
    ComponentRegistry& operator=(const ComponentRegistry&) = delete;
    ComponentRegistry(ComponentRegistry&&) = delete;
    ComponentRegistry& operator=(ComponentRegistry&&) = delete;
    
    /**
     * @brief Register a component type
     * @param typeId Unique component type ID
     * @param typeName Component type name
     * @param factory Factory function for creating instances
     * @return true if registered, false if typeId already exists
     */
    bool registerComponent(ComponentID typeId, const char* typeName, ComponentFactory factory);
    
    /**
     * @brief Unregister a component type
     * @return true if unregistered, false if not found
     */
    bool unregisterComponent(ComponentID typeId);
    
    /**
     * @brief Create a component instance
     * @return Component pointer or nullptr if type not registered
     */
    [[nodiscard]] std::unique_ptr<Component> createComponent(ComponentID typeId) const;
    
    /**
     * @brief Check if component type is registered
     */
    [[nodiscard]] bool isRegistered(ComponentID typeId) const;
    
    /**
     * @brief Get component type name
     * @return Type name or empty string if not registered
     */
    [[nodiscard]] const char* getTypeName(ComponentID typeId) const;
    
    /**
     * @brief Get all registered component type IDs
     */
    [[nodiscard]] std::vector<ComponentID> getRegisteredTypes() const;
    
    /**
     * @brief Get registered component count
     */
    [[nodiscard]] size_t getRegisteredCount() const noexcept;
    
    /**
     * @brief Clear all registered components
     */
    void clear();
    
private:
    struct ComponentTypeInfo {
        const char* typeName;
        ComponentFactory factory;
    };
    
    std::unordered_map<ComponentID, ComponentTypeInfo> m_types;
    mutable std::mutex m_mutex;
};

// ============================================================================
// Component Manager
// ============================================================================

/**
 * @brief Manages components attached to instances
 * 
 * Provides thread-safe storage and lookup for components attached to instances.
 * Each instance can have multiple components of different types.
 */
class ComponentManager {
public:
    ComponentManager() = default;
    ~ComponentManager() = default;
    
    // Delete copy/move (manager should be unique)
    ComponentManager(const ComponentManager&) = delete;
    ComponentManager& operator=(const ComponentManager&) = delete;
    ComponentManager(ComponentManager&&) = delete;
    ComponentManager& operator=(ComponentManager&&) = delete;
    
    /**
     * @brief Attach a component to an instance
     * @param instanceId Instance ID (from handle)
     * @param component Component to attach
     * @return true if attached, false if component already exists for this instance
     */
    bool attachComponent(uint64_t instanceId, std::unique_ptr<Component> component);
    
    /**
     * @brief Detach a component from an instance
     * @param instanceId Instance ID (from handle)
     * @param typeId Component type ID
     * @return true if detached, false if component not found
     */
    bool detachComponent(uint64_t instanceId, ComponentID typeId);
    
    /**
     * @brief Get a component from an instance
     * @return Component pointer or nullptr if not found
     */
    [[nodiscard]] Component* getComponent(uint64_t instanceId, ComponentID typeId);
    [[nodiscard]] const Component* getComponent(uint64_t instanceId, ComponentID typeId) const;
    
    /**
     * @brief Check if instance has a component
     */
    [[nodiscard]] bool hasComponent(uint64_t instanceId, ComponentID typeId) const;
    
    /**
     * @brief Get all components for an instance
     */
    [[nodiscard]] std::vector<Component*> getComponents(uint64_t instanceId);
    [[nodiscard]] std::vector<const Component*> getComponents(uint64_t instanceId) const;
    
    /**
     * @brief Get all component type IDs for an instance
     */
    [[nodiscard]] std::vector<ComponentID> getComponentTypes(uint64_t instanceId) const;
    
    /**
     * @brief Get component count for an instance
     */
    [[nodiscard]] size_t getComponentCount(uint64_t instanceId) const;
    
    /**
     * @brief Remove all components from an instance
     */
    void removeComponents(uint64_t instanceId);
    
    /**
     * @brief Clear all components from all instances
     */
    void clear();
    
private:
    // Instance ID -> ComponentID -> Component
    using ComponentMap = std::unordered_map<ComponentID, std::unique_ptr<Component>>;
    std::map<uint64_t, ComponentMap> m_instanceComponents;
    mutable std::mutex m_mutex;
};

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * @brief Generate a unique component ID from type
 * 
 * Uses std::hash<std::type_index> to generate a stable, portable component ID
 * for each type. The ID is consistent within a single process run.
 * 
 * @tparam T Component type
 * @return ComponentID Unique identifier for the component type
 */
template<typename T>
ComponentID generateComponentID() noexcept {
    static const std::type_index typeIndex(typeid(T));
    static const ComponentID id = static_cast<ComponentID>(std::hash<std::type_index>{}(typeIndex));
    return id;
}

/**
 * @brief Register a component type (convenience template)
 */
template<typename T>
bool registerComponentType(ComponentRegistry& registry, const char* typeName) {
    return registry.registerComponent(
        generateComponentID<T>(),
        typeName,
        []() -> std::unique_ptr<Component> { return std::make_unique<T>(); }
    );
}

// ============================================================================
// Global Registry Interface
// ============================================================================

/**
 * @brief Get the global component registry
 * Creates the registry on first call
 */
ComponentRegistry& getGlobalComponentRegistry();

/**
 * @brief Destroy the global component registry
 * Called during engine shutdown
 */
void destroyGlobalComponentRegistry();

} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_COMPONENT_H
