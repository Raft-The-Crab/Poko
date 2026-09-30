/**
 * @file physics.inl
 * @brief Physics component inline implementations
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_PHYSICS_INL
#define POKO_CORE_COMPONENTS_PHYSICS_PHYSICS_INL

namespace poko {
namespace core {
namespace components {
namespace physics {

// ============================================================================
// Body Type - Inline Implementation
// ============================================================================

inline void Physics::setBodyType(BodyType type) noexcept {
    m_bodyType = type;
}

inline BodyType Physics::getBodyType() const noexcept {
    return m_bodyType;
}

// ============================================================================
// Shape Type - Inline Implementation
// ============================================================================

inline void Physics::setShapeType(ShapeType type) noexcept {
    m_shapeType = type;
}

inline ShapeType Physics::getShapeType() const noexcept {
    return m_shapeType;
}

// ============================================================================
// Shape Dimensions - Inline Implementation
// ============================================================================

inline void Physics::setSphereRadius(float radius) noexcept {
    m_sphereRadius = radius;
    if (m_sphereRadius < MIN_BOUND_EXTENT) m_sphereRadius = MIN_BOUND_EXTENT;
    if (m_sphereRadius > MAX_BOUND_EXTENT) m_sphereRadius = MAX_BOUND_EXTENT;
}

inline float Physics::getSphereRadius() const noexcept {
    return m_sphereRadius;
}

inline void Physics::setBoxHalfExtents(float halfExtX, float halfExtY, float halfExtZ) noexcept {
    m_boxHalfExtX = halfExtX;
    m_boxHalfExtY = halfExtY;
    m_boxHalfExtZ = halfExtZ;
}

inline float Physics::getBoxHalfExtX() const noexcept {
    return m_boxHalfExtX;
}

inline float Physics::getBoxHalfExtY() const noexcept {
    return m_boxHalfExtY;
}

inline float Physics::getBoxHalfExtZ() const noexcept {
    return m_boxHalfExtZ;
}

inline void Physics::setCapsuleRadius(float radius) noexcept {
    m_capsuleRadius = radius;
}

inline float Physics::getCapsuleRadius() const noexcept {
    return m_capsuleRadius;
}

inline void Physics::setCapsuleHeight(float height) noexcept {
    m_capsuleHeight = height;
}

inline float Physics::getCapsuleHeight() const noexcept {
    return m_capsuleHeight;
}

// ============================================================================
// Material - Inline Implementation
// ============================================================================

inline void Physics::setMaterial(const Material& material) noexcept {
    m_material = material;
}

inline const Material& Physics::getMaterial() const noexcept {
    return m_material;
}

inline void Physics::setFriction(float friction) noexcept {
    m_material.friction = friction;
    if (m_material.friction < MIN_FRICTION) m_material.friction = MIN_FRICTION;
    if (m_material.friction > MAX_FRICTION) m_material.friction = MAX_FRICTION;
}

inline float Physics::getFriction() const noexcept {
    return m_material.friction;
}

inline void Physics::setRestitution(float restitution) noexcept {
    m_material.restitution = restitution;
    if (m_material.restitution < MIN_RESTITUTION) m_material.restitution = MIN_RESTITUTION;
    if (m_material.restitution > MAX_RESTITUTION) m_material.restitution = MAX_RESTITUTION;
}

inline float Physics::getRestitution() const noexcept {
    return m_material.restitution;
}

inline void Physics::setDensity(float density) noexcept {
    m_material.density = density;
    if (m_material.density < MIN_DENSITY) m_material.density = MIN_DENSITY;
    if (m_material.density > MAX_DENSITY) m_material.density = MAX_DENSITY;
}

inline float Physics::getDensity() const noexcept {
    return m_material.density;
}

// ============================================================================
// Mass - Inline Implementation
// ============================================================================

inline void Physics::setMass(float mass) noexcept {
    m_mass = mass;
    if (m_mass < MIN_MASS) m_mass = MIN_MASS;
    if (m_mass > MAX_MASS) m_mass = MAX_MASS;
}

inline float Physics::getMass() const noexcept {
    return m_mass;
}

// ============================================================================
// Velocity - Inline Implementation
// ============================================================================

inline float Physics::getLinearVelocityX() const noexcept {
    return m_linearVelocityX;
}

inline float Physics::getLinearVelocityY() const noexcept {
    return m_linearVelocityY;
}

inline float Physics::getLinearVelocityZ() const noexcept {
    return m_linearVelocityZ;
}

inline float Physics::getAngularVelocityX() const noexcept {
    return m_angularVelocityX;
}

inline float Physics::getAngularVelocityY() const noexcept {
    return m_angularVelocityY;
}

inline float Physics::getAngularVelocityZ() const noexcept {
    return m_angularVelocityZ;
}

// ============================================================================
// Gravity - Inline Implementation
// ============================================================================

inline void Physics::setGravityScale(float scale) noexcept {
    m_gravityScale = scale;
}

inline float Physics::getGravityScale() const noexcept {
    return m_gravityScale;
}

// ============================================================================
// Collision - Inline Implementation
// ============================================================================

inline void Physics::setCollisionLayer(uint32_t layer) noexcept {
    m_collisionLayer = layer;
}

inline uint32_t Physics::getCollisionLayer() const noexcept {
    return m_collisionLayer;
}

inline void Physics::setCollisionMask(uint32_t mask) noexcept {
    m_collisionMask = mask;
}

inline uint32_t Physics::getCollisionMask() const noexcept {
    return m_collisionMask;
}

// ============================================================================
// Sleeping - Inline Implementation
// ============================================================================

inline void Physics::setSleepThreshold(float threshold) noexcept {
    m_sleepThreshold = threshold;
}

inline float Physics::getSleepThreshold() const noexcept {
    return m_sleepThreshold;
}

inline void Physics::setSleepEnabled(bool enabled) noexcept {
    m_sleepEnabled = enabled;
}

inline bool Physics::isSleepEnabled() const noexcept {
    return m_sleepEnabled;
}

inline void Physics::wakeUp() noexcept {
    m_isSleeping = false;
}

inline void Physics::sleep() noexcept {
    m_isSleeping = true;
}

inline bool Physics::isSleeping() const noexcept {
    return m_isSleeping;
}

// ============================================================================
// Damping - Inline Implementation
// ============================================================================

inline float Physics::getLinearDamping() const noexcept {
    return m_linearDamping;
}

inline float Physics::getAngularDamping() const noexcept {
    return m_angularDamping;
}

// ============================================================================
// CCD - Inline Implementation
// ============================================================================

inline void Physics::setCCDEnabled(bool enabled) noexcept {
    m_ccdEnabled = enabled;
}

inline bool Physics::isCCDEnabled() const noexcept {
    return m_ccdEnabled;
}

inline float Physics::getCCDMotionThreshold() const noexcept {
    return m_ccdMotionThreshold;
}

// ============================================================================
// Velocity Limits - Inline Implementation
// ============================================================================

inline float Physics::getMaxLinearVelocity() const noexcept {
    return m_maxLinearVelocity;
}

inline float Physics::getMaxAngularVelocity() const noexcept {
    return m_maxAngularVelocity;
}

// ============================================================================
// Accumulated Forces - Inline Implementation
// ============================================================================

inline float Physics::getAccumulatedForceX() const noexcept {
    return m_accumulatedForceX;
}

inline float Physics::getAccumulatedForceY() const noexcept {
    return m_accumulatedForceY;
}

inline float Physics::getAccumulatedForceZ() const noexcept {
    return m_accumulatedForceZ;
}

inline float Physics::getAccumulatedTorqueX() const noexcept {
    return m_accumulatedTorqueX;
}

inline float Physics::getAccumulatedTorqueY() const noexcept {
    return m_accumulatedTorqueY;
}

inline float Physics::getAccumulatedTorqueZ() const noexcept {
    return m_accumulatedTorqueZ;
}

} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_PHYSICS_INL
