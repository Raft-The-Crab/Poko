/**
 * @file component.h
 * @brief Component system header for PokoEngine ECS
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#pragma once

#include <cstdint>
#include <typeindex>

namespace Poko {
namespace ECS {

/**
 * @brief Unique identifier for component types
 */
using ComponentType = std::type_index;

/**
 * @brief Base class for all components
 */
class Component {
public:
    virtual ~Component() = default;
    
    /**
     * @brief Get the type ID of this component
     * @return Type ID for this component type
     */
    virtual ComponentType GetType() const = 0;
};

/**
 * @brief Template to get component type ID
 */
template<typename T>
struct ComponentTypeID {
    static ComponentType Get()
    {
        return ComponentType(typeid(T));
    }
};

} // namespace ECS
} // namespace Poko