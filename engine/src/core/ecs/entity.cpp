/**
 * @file entity.cpp
 * @brief Entity implementation
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-21
 */

#include "core/ecs/entity.h"

namespace Poko {
namespace ECS {

Entity::Entity()
    : m_id(INVALID_ENTITY)
{
}

Entity::Entity(EntityID id)
    : m_id(id)
{
}

EntityID Entity::GetID() const
{
    return m_id;
}

bool Entity::IsValid() const
{
    return m_id != INVALID_ENTITY;
}

Entity::operator bool() const
{
    return IsValid();
}

bool Entity::operator==(const Entity& other) const
{
    return m_id == other.m_id;
}

bool Entity::operator!=(const Entity& other) const
{
    return m_id != other.m_id;
}

} // namespace ECS
} // namespace Poko