/**
 * @file component_manager.h
 * @brief Component manager header for PokoEngine ECS
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#pragma once

#include "core/ecs/entity.h"
#include "core/ecs/component.h"
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <vector>
#include <functional>
#include <typeindex>
#include <mutex>
#include <shared_mutex>
#include <atomic>

namespace Poko {
namespace ECS {

/**
 * @brief Component query signature for finding entities with specific components
 */
class ComponentQuery {
public:
    using ComponentID = std::type_index;

    ComponentQuery() = default;

    /**
     * @brief Add a required component type to the query
     */
    template<typename T>
    ComponentQuery& Require()
    {
        m_required.emplace(typeid(T));
        return *this;
    }

    /**
     * @brief Add an excluded component type to the query
     */
    template<typename T>
    ComponentQuery& Exclude()
    {
        m_excluded.emplace(typeid(T));
        return *this;
    }

    const std::unordered_set<ComponentID>& GetRequired() const { return m_required; }
    const std::unordered_set<ComponentID>& GetExcluded() const { return m_excluded; }

private:
    std::unordered_set<ComponentID> m_required;
    std::unordered_set<ComponentID> m_excluded;
};

/**
 * @brief Manages components for entities
 */
class ComponentManager {
public:
    ComponentManager();
    ~ComponentManager();

    /**
     * @brief Add a component to an entity
     * @param entity Entity to add component to
     * @param component Component to add
     * @return true if component was added, false if entity already had this component
     */
    template<typename T>
    bool AddComponent(Entity entity, T* component)
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        
        ComponentType type = ComponentTypeID<T>::Get();
        
        // Check if component already exists
        auto it = m_components.find(type);
        if (it != m_components.end()) {
            if (it->second.find(entity.GetID()) != it->second.end()) {
                return false; // Component already exists
            }
        }
        
        m_components[type][entity.GetID()] = std::unique_ptr<Component>(component);
        m_entity_signatures[entity.GetID()].insert(typeid(T));
        return true;
    }

    /**
     * @brief Get a component from an entity
     * @param entity Entity to get component from
     * @return Component pointer or nullptr if not found
     */
    template<typename T>
    T* GetComponent(Entity entity)
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        
        ComponentType type = ComponentTypeID<T>::Get();
        auto it = m_components.find(type);
        if (it != m_components.end()) {
            auto entityIt = it->second.find(entity.GetID());
            if (entityIt != it->second.end()) {
                return static_cast<T*>(entityIt->second.get());
            }
        }
        return nullptr;
    }

    /**
     * @brief Remove a component from an entity
     * @param entity Entity to remove component from
     * @return true if component was removed, false if not found
     */
    template<typename T>
    bool RemoveComponent(Entity entity)
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        
        ComponentType type = ComponentTypeID<T>::Get();
        auto it = m_components.find(type);
        bool removed = false;
        
        if (it != m_components.end()) {
            removed = it->second.erase(entity.GetID()) > 0;
        }
        
        m_entity_signatures[entity.GetID()].erase(typeid(T));
        
        // Clean up empty signature
        if (m_entity_signatures[entity.GetID()].empty()) {
            m_entity_signatures.erase(entity.GetID());
        }
        
        return removed;
    }

    /**
     * @brief Remove all components from an entity
     * @param entity Entity to remove components from
     */
    void RemoveAllComponents(Entity entity);

    /**
     * @brief Query for entities matching a component signature
     * @param query Component query specification
     * @return Vector of matching entities
     */
    std::vector<Entity> Query(const ComponentQuery& query) const;

    /**
     * @brief Get all entities that have a specific component type
     */
    template<typename T>
    std::vector<Entity> GetEntitiesWithComponent()
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        
        std::vector<Entity> entities;
        ComponentType type = ComponentTypeID<T>::Get();
        auto it = m_components.find(type);
        if (it != m_components.end()) {
            for (const auto& pair : it->second) {
                entities.emplace_back(pair.first);
            }
        }
        return entities;
    }

private:
    mutable std::shared_mutex m_mutex;
    std::unordered_map<ComponentType, std::unordered_map<EntityID, std::unique_ptr<Component>>> m_components;
    std::unordered_map<EntityID, std::unordered_set<ComponentQuery::ComponentID>> m_entity_signatures;
};

} // namespace ECS
} // namespace Poko