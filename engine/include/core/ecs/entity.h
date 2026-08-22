/**
 * @file entity.h
 * @brief Entity system header for PokoEngine ECS
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#pragma once

#include <cstdint>

namespace Poko {
namespace ECS {

/**
 * @brief Unique identifier for entities
 */
using EntityID = uint32_t;

/**
 * @brief Invalid entity ID constant
 */
constexpr EntityID INVALID_ENTITY = 0;

/**
 * @brief Entity class representing a game object
 */
class Entity {
public:
    Entity();
    Entity(EntityID id);
    
    EntityID GetID() const;
    bool IsValid() const;
    
    operator bool() const;
    bool operator==(const Entity& other) const;
    bool operator!=(const Entity& other) const;
    
private:
    EntityID m_id;
};

} // namespace ECS
} // namespace Poko