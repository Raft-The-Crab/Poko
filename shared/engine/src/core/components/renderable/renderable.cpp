/**
 * @file renderable.cpp
 * @brief Renderable component implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/renderable/renderable.h"

namespace poko {
namespace core {
namespace components {
namespace renderable {

// ============================================================================
// Renderable Implementation
// ============================================================================

Renderable::Renderable() noexcept
    : m_visible(true)
    , m_materialId(INVALID_RESOURCE_ID)
    , m_meshId(INVALID_RESOURCE_ID)
    , m_renderLayer(RenderLayer::Default)
    , m_renderQueue(RenderQueue::Opaque)
    , m_bounds()
    , m_castShadows(true)
    , m_receiveShadows(true)
    , m_userData(nullptr)
{
}

void Renderable::onCreate() {
    // Initialize renderable resources
    m_state = ComponentState::Created;
}

void Renderable::onActivate() {
    // Activate rendering
    m_state = ComponentState::Active;
}

void Renderable::onDeactivate() {
    // Deactivate rendering
    m_state = ComponentState::Deactivating;
}

void Renderable::onDestroy() {
    // Cleanup renderable resources
    m_materialId = INVALID_RESOURCE_ID;
    m_meshId = INVALID_RESOURCE_ID;
    m_userData = nullptr;
    m_state = ComponentState::Destroyed;
}

} // namespace renderable
} // namespace components
} // namespace core
} // namespace poko
