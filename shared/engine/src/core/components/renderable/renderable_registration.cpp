/**
 * @file renderable_registration.cpp
 * @brief Renderable component registration
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/renderable/renderable.h"
#include "core/components/component.h"

namespace poko {
namespace core {
namespace components {
namespace renderable {

/**
 * @brief Register Renderable component with the component system
 */
void registerRenderableComponent() {
    auto& registry = getGlobalComponentRegistry();
    
    auto factory = []() -> std::unique_ptr<Component> {
        return std::make_unique<Renderable>();
    };
    
    bool registered = registry.registerComponent(
        RENDERABLE_COMPONENT_ID,
        RENDERABLE_COMPONENT_NAME,
        factory
    );
    
    // In production, registration failure would be logged
    (void)registered;
}

} // namespace renderable

/**
 * @brief Register Renderable component with global registry
 */
void registerRenderableComponent() {
    renderable::registerRenderableComponent();
}

} // namespace components
} // namespace core
} // namespace poko
