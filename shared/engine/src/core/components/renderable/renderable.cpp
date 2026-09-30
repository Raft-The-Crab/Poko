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
#include <algorithm>

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

void Renderable::setBounds(const BoundingBox& bounds) noexcept {
    m_bounds = bounds;
    
    // Fix invalid bounds (ensure min <= max)
    m_bounds.fix();
    
    // Clamp extents to safe range
    using namespace std;
    m_bounds.minX = max(m_bounds.minX, -MAX_BOUND_EXTENT);
    m_bounds.minY = max(m_bounds.minY, -MAX_BOUND_EXTENT);
    m_bounds.minZ = max(m_bounds.minZ, -MAX_BOUND_EXTENT);
    m_bounds.maxX = min(m_bounds.maxX, MAX_BOUND_EXTENT);
    m_bounds.maxY = min(m_bounds.maxY, MAX_BOUND_EXTENT);
    m_bounds.maxZ = min(m_bounds.maxZ, MAX_BOUND_EXTENT);
    
    // Ensure minimum extent to prevent zero-size boxes
    if (m_bounds.extentX() < MIN_BOUND_EXTENT) {
        float center = m_bounds.centerX();
        m_bounds.minX = center - MIN_BOUND_EXTENT * 0.5f;
        m_bounds.maxX = center + MIN_BOUND_EXTENT * 0.5f;
    }
    if (m_bounds.extentY() < MIN_BOUND_EXTENT) {
        float center = m_bounds.centerY();
        m_bounds.minY = center - MIN_BOUND_EXTENT * 0.5f;
        m_bounds.maxY = center + MIN_BOUND_EXTENT * 0.5f;
    }
    if (m_bounds.extentZ() < MIN_BOUND_EXTENT) {
        float center = m_bounds.centerZ();
        m_bounds.minZ = center - MIN_BOUND_EXTENT * 0.5f;
        m_bounds.maxZ = center + MIN_BOUND_EXTENT * 0.5f;
    }
}

} // namespace renderable
} // namespace components
} // namespace core
} // namespace poko
