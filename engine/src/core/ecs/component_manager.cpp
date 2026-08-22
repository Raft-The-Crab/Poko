/**
 * @file component_manager.cpp
 * @brief Component manager implementation
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-21
 */

#include "core/ecs/component_manager.h"

namespace Poko {
namespace ECS {

ComponentManager::ComponentManager()
{
}

ComponentManager::~ComponentManager()
{
}

void ComponentManager::RemoveAllComponents(Entity entity)
{
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    for (auto& typePair : m_components) {
        typePair.second.erase(entity.GetID());
    }
    m_entity_signatures.erase(entity.GetID());
}

std::vector<Entity> ComponentManager::Query(const ComponentQuery& query) const
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    std::vector<Entity> matching_entities;
    
    for (const auto& sig_pair : m_entity_signatures) {
        EntityID entity_id = sig_pair.first;
        const auto& signature = sig_pair.second;
        
        // Check if entity has all required components
        bool has_required = true;
        for (const auto& required : query.GetRequired()) {
            if (signature.find(required) == signature.end()) {
                has_required = false;
                break;
            }
        }
        
        // Check if entity has no excluded components
        bool has_excluded = false;
        for (const auto& excluded : query.GetExcluded()) {
            if (signature.find(excluded) != signature.end()) {
                has_excluded = true;
                break;
            }
        }
        
        if (has_required && !has_excluded) {
            matching_entities.emplace_back(entity_id);
        }
    }
    
    return matching_entities;
}

} // namespace ECS
} // namespace Poko