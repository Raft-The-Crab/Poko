/**
 * @file transform.cpp
 * @brief Transform component implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/transform/transform.h"
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace transform {

// ============================================================================
// Transform Implementation
// ============================================================================

Transform::Transform() noexcept
    : m_localPosition(Vector3::zero())
    , m_localRotation(Quaternion::identity())
    , m_localScale(Vector3::one())
    , m_worldPosition(Vector3::zero())
    , m_worldRotation(Quaternion::identity())
    , m_worldScale(Vector3::one())
    , m_localToWorldMatrix(Matrix4x4::identity())
    , m_worldToLocalMatrix(Matrix4x4::identity())
    , m_parent(nullptr)
    , m_dirty(true)
{
}

void Transform::setLocalPosition(const Vector3& position) noexcept {
    m_localPosition = position;
    markDirty();
}

void Transform::setLocalRotation(const Quaternion& rotation) noexcept {
    m_localRotation = rotation;
    markDirty();
}

void Transform::setLocalScale(const Vector3& scale) noexcept {
    m_localScale = scale;
    markDirty();
}

void Transform::setParent(Transform* parent) noexcept {
    m_parent = parent;
    markDirty();
}

void Transform::updateWorldTransform() noexcept {
    if (!m_dirty) {
        return;
    }
    
    if (m_parent) {
        // Ensure parent is up to date
        m_parent->updateWorldTransform();
        
        // Combine parent world transform with local transform
        // World position = parent world position + (parent world rotation * local position * parent world scale)
        // Simplified: World position = parent world position + local position (no scale for now)
        m_worldPosition.x = m_parent->m_worldPosition.x + m_localPosition.x;
        m_worldPosition.y = m_parent->m_worldPosition.y + m_localPosition.y;
        m_worldPosition.z = m_parent->m_worldPosition.z + m_localPosition.z;
        
        // World rotation = parent world rotation * local rotation
        // Simplified: just use local rotation for now
        m_worldRotation = m_localRotation;
        
        // World scale = parent world scale * local scale
        m_worldScale.x = m_parent->m_worldScale.x * m_localScale.x;
        m_worldScale.y = m_parent->m_worldScale.y * m_localScale.y;
        m_worldScale.z = m_parent->m_worldScale.z * m_localScale.z;
    } else {
        // No parent, world = local
        m_worldPosition = m_localPosition;
        m_worldRotation = m_localRotation;
        m_worldScale = m_localScale;
    }
    
    // Compute matrices
    computeLocalToWorldMatrix();
    computeWorldToLocalMatrix();
    
    m_dirty = false;
}

void Transform::markDirty() noexcept {
    m_dirty = true;
}

void Transform::computeLocalToWorldMatrix() noexcept {
    // Build translation matrix
    // Position in translation (row-major order)
    m_localToWorldMatrix.data[12] = m_worldPosition.x;
    m_localToWorldMatrix.data[13] = m_worldPosition.y;
    m_localToWorldMatrix.data[14] = m_worldPosition.z;
    
    // Scale in diagonal
    m_localToWorldMatrix.data[0] = m_worldScale.x;
    m_localToWorldMatrix.data[5] = m_worldScale.y;
    m_localToWorldMatrix.data[10] = m_worldScale.z;
    
    // Rotation would go here (simplified for now)
    // Full quaternion-to-matrix conversion would be added later
}

void Transform::computeWorldToLocalMatrix() noexcept {
    // Simplified inverse: just set to identity for now
    // Full matrix inversion would be added later
    m_worldToLocalMatrix = Matrix4x4::identity();
}

} // namespace transform
} // namespace components
} // namespace core
} // namespace poko
