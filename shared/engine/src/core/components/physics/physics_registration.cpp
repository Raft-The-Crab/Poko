/**
 * @file physics_registration.cpp
 * @brief Physics component registration
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/physics.h"
#include "core/components/component.h"

namespace poko {
namespace core {
namespace components {
namespace physics {

/**
 * @brief Register Physics component with the component system
 */
void registerPhysicsComponent() {
    auto& registry = getGlobalComponentRegistry();
    
    auto factory = []() -> std::unique_ptr<Component> {
        return std::make_unique<Physics>();
    };
    
    bool registered = registry.registerComponent(
        Physics::COMPONENT_ID,
        Physics::COMPONENT_NAME,
        factory
    );
    
    // In production, registration failure would be logged
    (void)registered;
}

} // namespace physics

/**
 * @brief Register Physics component with global registry
 */
void registerPhysicsComponent() {
    physics::registerPhysicsComponent();
}

} // namespace components
} // namespace core
} // namespace poko
