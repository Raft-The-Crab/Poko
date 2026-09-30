/**
 * @file physics.h
 * @brief Physics component header
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_PHYSICS_H
#define POKO_CORE_COMPONENTS_PHYSICS_PHYSICS_H

#include <cstdint>
#include "../component.h"

namespace poko {
namespace core {
namespace components {
namespace physics {

// ============================================================================
// Constants
// ============================================================================

/// Maximum friction coefficient
constexpr float MAX_FRICTION = 2.0f;

/// Minimum friction coefficient
constexpr float MIN_FRICTION = 0.0f;

/// Maximum restitution coefficient
constexpr float MAX_RESTITUTION = 1.0f;

/// Minimum restitution coefficient
constexpr float MIN_RESTITUTION = 0.0f;

/// Maximum density (kg/m³)
constexpr float MAX_DENSITY = 10000.0f;

/// Minimum density (kg/m³)
constexpr float MIN_DENSITY = 1.0f;

/// Maximum mass (kg)
constexpr float MAX_MASS = 100000.0f;

/// Minimum mass (kg)
constexpr float MIN_MASS = 0.001f;

/// Default sphere radius
constexpr float DEFAULT_SPHERE_RADIUS = 1.0f;

/// Default box half-extent
constexpr float DEFAULT_BOX_HALF_EXTENT = 1.0f;

/// Default capsule radius
constexpr float DEFAULT_CAPSULE_RADIUS = 0.5f;

/// Default capsule height
constexpr float DEFAULT_CAPSULE_HEIGHT = 2.0f;

/// Default gravity scale
constexpr float DEFAULT_GRAVITY_SCALE = 1.0f;

/// Default sleep threshold
constexpr float DEFAULT_SLEEP_THRESHOLD = 0.01f;

/// Maximum bounding extent (for safety)
constexpr float MAX_BOUND_EXTENT = 100000.0f;

/// Minimum bounding extent (to prevent zero-size shapes)
constexpr float MIN_BOUND_EXTENT = 0.0001f;

/// Maximum damping coefficient
constexpr float MAX_DAMPING = 1.0f;

/// Minimum damping coefficient
constexpr float MIN_DAMPING = 0.0f;

/// Default linear damping
constexpr float DEFAULT_LINEAR_DAMPING = 0.0f;

/// Default angular damping
constexpr float DEFAULT_ANGULAR_DAMPING = 0.05f;

/// Maximum velocity (for safety)
constexpr float MAX_VELOCITY = 1000.0f;

/// Default CCD motion threshold
constexpr float DEFAULT_CCD_MOTION_THRESHOLD = 0.1f;

/// Default maximum linear velocity
constexpr float DEFAULT_MAX_LINEAR_VELOCITY = 100.0f;

/// Default maximum angular velocity
constexpr float DEFAULT_MAX_ANGULAR_VELOCITY = 50.0f;

// ============================================================================
// Physics Body Type
// ============================================================================

/**
 * @brief Physics body type
 */
enum class BodyType : uint32_t {
    Static = 0,      ///< Static body (immovable, infinite mass)
    Kinematic = 1,   ///< Kinematic body (moved by code, affects others)
    Dynamic = 2,     ///< Dynamic body (affected by forces and collisions)
    Character = 3    ///< Character body (specialized for character controllers)
};

// ============================================================================
// Physics Shape Type
// ============================================================================

/**
 * @brief Physics shape type
 */
enum class ShapeType : uint32_t {
    Sphere = 0,          ///< Sphere shape
    Box = 1,              ///< Box (cuboid) shape
    Capsule = 2,          ///< Capsule shape
    Cylinder = 3,         ///< Cylinder shape
    ConvexHull = 4,       ///< Convex hull shape
    TriangleMesh = 5,     ///< Triangle mesh shape
    Heightfield = 6       ///< Heightfield shape
};

// ============================================================================
// Physics Material
// ============================================================================

/**
 * @brief Physics material properties
 */
struct Material {
    float friction;       ///< Friction coefficient (0.0 to 1.0)
    float restitution;    ///< Restitution/bounciness (0.0 to 1.0)
    float density;        ///< Density in kg/m³
    
    /**
     * @brief Default constructor
     */
    constexpr Material() noexcept
        : friction(0.5f)
        , restitution(0.3f)
        , density(1000.0f) {}
    
    /**
     * @brief Component constructor
     */
    constexpr Material(float friction_, float restitution_, float density_) noexcept
        : friction(friction_)
        , restitution(restitution_)
        , density(density_) {}
};

// ============================================================================
// Physics Component
// ============================================================================

/**
 * @brief Physics component for physics simulation
 * 
 * This component handles:
 * - Body type (Static, Kinematic, Dynamic, Character)
 * - Shape type and dimensions
 * - Material properties (friction, restitution, density)
 * - Mass and inertia
 * - Velocity and angular velocity
 * - Gravity scale
 * - Collision layer and mask
 * - Sleeping/wake behavior
 * 
 * @section thread_safety Thread Safety
 * Physics component is not thread-safe by default.
 * Access must be synchronized externally if used from multiple threads.
 */
class Physics : public Component {
public:
    // Component constants
    static constexpr ComponentID COMPONENT_ID = 2;
    static constexpr const char* COMPONENT_NAME = "Physics";
    
    /**
     * @brief Constructor
     */
    Physics() noexcept;
    
    /**
     * @brief Destructor
     */
    ~Physics() override = default;
    
    // ============================================================================
    // Component Interface
    // ============================================================================
    
    /**
     * @brief Get component type ID
     */
    [[nodiscard]] ComponentID getTypeID() const noexcept override { return COMPONENT_ID; }
    
    /**
     * @brief Get component type name
     */
    [[nodiscard]] const char* getTypeName() const noexcept override { return COMPONENT_NAME; }
    
    /**
     * @brief Called when component is created
     */
    void onCreate() override;
    
    /**
     * @brief Called when component is activated
     */
    void onActivate() override;
    
    /**
     * @brief Called when component is deactivated
     */
    void onDeactivate() override;
    
    /**
     * @brief Called when component is destroyed
     */
    void onDestroy() override;
    
    // ============================================================================
    // Body Type
    // ============================================================================
    
    /**
     * @brief Set body type
     * @param type Body type
     */
    void setBodyType(BodyType type) noexcept;
    
    /**
     * @brief Get body type
     * @return Body type
     */
    [[nodiscard]] BodyType getBodyType() const noexcept;
    
    // ============================================================================
    // Shape Type
    // ============================================================================
    
    /**
     * @brief Set shape type
     * @param type Shape type
     */
    void setShapeType(ShapeType type) noexcept;
    
    /**
     * @brief Get shape type
     * @return Shape type
     */
    [[nodiscard]] ShapeType getShapeType() const noexcept;
    
    // ============================================================================
    // Shape Dimensions
    // ============================================================================
    
    /**
     * @brief Set sphere radius with validation
     * @param radius Sphere radius
     */
    void setSphereRadius(float radius) noexcept;
    
    /**
     * @brief Get sphere radius
     * @return Sphere radius
     */
    [[nodiscard]] float getSphereRadius() const noexcept;
    
    /**
     * @brief Set box half-extents
     * @param halfExtX Half extent X
     * @param halfExtY Half extent Y
     * @param halfExtZ Half extent Z
     */
    void setBoxHalfExtents(float halfExtX, float halfExtY, float halfExtZ) noexcept;
    
    /**
     * @brief Get box half-extent X
     * @return Half extent X
     */
    [[nodiscard]] float getBoxHalfExtX() const noexcept;
    
    /**
     * @brief Get box half-extent Y
     * @return Half extent Y
     */
    [[nodiscard]] float getBoxHalfExtY() const noexcept;
    
    /**
     * @brief Get box half-extent Z
     * @return Half extent Z
     */
    [[nodiscard]] float getBoxHalfExtZ() const noexcept;
    
    /**
     * @brief Set capsule radius
     * @param radius Capsule radius
     */
    void setCapsuleRadius(float radius) noexcept;
    
    /**
     * @brief Get capsule radius
     * @return Capsule radius
     */
    [[nodiscard]] float getCapsuleRadius() const noexcept;
    
    /**
     * @brief Set capsule height
     * @param height Capsule height
     */
    void setCapsuleHeight(float height) noexcept;
    
    /**
     * @brief Get capsule height
     * @return Capsule height
     */
    [[nodiscard]] float getCapsuleHeight() const noexcept;
    
    // ============================================================================
    // Material
    // ============================================================================
    
    /**
     * @brief Set material
     * @param material Material properties
     */
    void setMaterial(const Material& material) noexcept;
    
    /**
     * @brief Get material
     * @return Material properties
     */
    [[nodiscard]] const Material& getMaterial() const noexcept;
    
    /**
     * @brief Set friction with clamping
     * @param friction Friction coefficient
     */
    void setFriction(float friction) noexcept;
    
    /**
     * @brief Get friction
     * @return Friction coefficient
     */
    [[nodiscard]] float getFriction() const noexcept;
    
    /**
     * @brief Set restitution with clamping
     * @param restitution Restitution coefficient
     */
    void setRestitution(float restitution) noexcept;
    
    /**
     * @brief Get restitution
     * @return Restitution coefficient
     */
    [[nodiscard]] float getRestitution() const noexcept;
    
    /**
     * @brief Set density with clamping
     * @param density Density in kg/m³
     */
    void setDensity(float density) noexcept;
    
    /**
     * @brief Get density
     * @return Density in kg/m³
     */
    [[nodiscard]] float getDensity() const noexcept;
    
    // ============================================================================
    // Mass
    // ============================================================================
    
    /**
     * @brief Set mass with validation
     * @param mass Mass in kg
     */
    void setMass(float mass) noexcept;
    
    /**
     * @brief Get mass
     * @return Mass in kg
     */
    [[nodiscard]] float getMass() const noexcept;
    
    /**
     * @brief Calculate mass from density and shape volume
     * @return Calculated mass in kg
     */
    [[nodiscard]] float calculateMassFromDensity() const noexcept;
    
    /**
     * @brief Calculate inertia tensor from mass and shape
     */
    void calculateInertiaTensor() noexcept;
    
    /**
     * @brief Get inertia tensor diagonal element XX
     * @return Inertia XX
     */
    [[nodiscard]] float getInertiaXX() const noexcept { return m_inertiaXX; }
    
    /**
     * @brief Get inertia tensor diagonal element YY
     * @return Inertia YY
     */
    [[nodiscard]] float getInertiaYY() const noexcept { return m_inertiaYY; }
    
    /**
     * @brief Get inertia tensor diagonal element ZZ
     * @return Inertia ZZ
     */
    [[nodiscard]] float getInertiaZZ() const noexcept { return m_inertiaZZ; }
    
    // ============================================================================
    // Velocity
    // ============================================================================
    
    /**
     * @brief Set linear velocity
     * @param x Velocity X
     * @param y Velocity Y
     * @param z Velocity Z
     */
    void setLinearVelocity(float x, float y, float z) noexcept;
    
    /**
     * @brief Get linear velocity X
     * @return Velocity X
     */
    [[nodiscard]] float getLinearVelocityX() const noexcept;
    
    /**
     * @brief Get linear velocity Y
     * @return Velocity Y
     */
    [[nodiscard]] float getLinearVelocityY() const noexcept;
    
    /**
     * @brief Get linear velocity Z
     * @return Velocity Z
     */
    [[nodiscard]] float getLinearVelocityZ() const noexcept;
    
    /**
     * @brief Set angular velocity
     * @param x Angular velocity X
     * @param y Angular velocity Y
     * @param z Angular velocity Z
     */
    void setAngularVelocity(float x, float y, float z) noexcept;
    
    /**
     * @brief Get angular velocity X
     * @return Angular velocity X
     */
    [[nodiscard]] float getAngularVelocityX() const noexcept;
    
    /**
     * @brief Get angular velocity Y
     * @return Angular velocity Y
     */
    [[nodiscard]] float getAngularVelocityY() const noexcept;
    
    /**
     * @brief Get angular velocity Z
     * @return Angular velocity Z
     */
    [[nodiscard]] float getAngularVelocityZ() const noexcept;
    
    // ============================================================================
    // Gravity
    // ============================================================================
    
    /**
     * @brief Set gravity scale
     * @param scale Gravity scale multiplier
     */
    void setGravityScale(float scale) noexcept;
    
    /**
     * @brief Get gravity scale
     * @return Gravity scale multiplier
     */
    [[nodiscard]] float getGravityScale() const noexcept;
    
    // ============================================================================
    // Collision
    // ============================================================================
    
    /**
     * @brief Set collision layer
     * @param layer Collision layer (32-bit mask)
     */
    void setCollisionLayer(uint32_t layer) noexcept;
    
    /**
     * @brief Get collision layer
     * @return Collision layer
     */
    [[nodiscard]] uint32_t getCollisionLayer() const noexcept;
    
    /**
     * @brief Set collision mask
     * @param mask Collision mask (32-bit mask)
     */
    void setCollisionMask(uint32_t mask) noexcept;
    
    /**
     * @brief Get collision mask
     * @return Collision mask
     */
    [[nodiscard]] uint32_t getCollisionMask() const noexcept;
    
    /**
     * @brief Check if this body should collide with another body
     * @param otherLayer Other body's collision layer
     * @param otherMask Other body's collision mask
     * @return True if bodies should collide
     */
    [[nodiscard]] bool shouldCollideWith(uint32_t otherLayer, uint32_t otherMask) const noexcept;
    
    // ============================================================================
    // Sleeping
    // ============================================================================
    
    /**
     * @brief Set sleep threshold
     * @param threshold Sleep threshold velocity
     */
    void setSleepThreshold(float threshold) noexcept;
    
    /**
     * @brief Get sleep threshold
     * @return Sleep threshold velocity
     */
    [[nodiscard]] float getSleepThreshold() const noexcept;
    
    /**
     * @brief Set sleep enabled
     * @param enabled True if sleeping enabled
     */
    void setSleepEnabled(bool enabled) noexcept;
    
    /**
     * @brief Get sleep enabled
     * @return True if sleeping enabled
     */
    [[nodiscard]] bool isSleepEnabled() const noexcept;
    
    /**
     * @brief Wake up the body
     */
    void wakeUp() noexcept;
    
    /**
     * @brief Put body to sleep
     */
    void sleep() noexcept;
    
    /**
     * @brief Check if body is sleeping
     * @return True if sleeping
     */
    [[nodiscard]] bool isSleeping() const noexcept;
    
    // ============================================================================
    // Damping
    // ============================================================================
    
    /**
     * @brief Set linear damping
     * @param damping Linear damping coefficient (0.0 to 1.0)
     */
    void setLinearDamping(float damping) noexcept;
    
    /**
     * @brief Get linear damping
     * @return Linear damping coefficient
     */
    [[nodiscard]] float getLinearDamping() const noexcept;
    
    /**
     * @brief Set angular damping
     * @param damping Angular damping coefficient (0.0 to 1.0)
     */
    void setAngularDamping(float damping) noexcept;
    
    /**
     * @brief Get angular damping
     * @return Angular damping coefficient
     */
    [[nodiscard]] float getAngularDamping() const noexcept;
    
    /**
     * @brief Apply damping to velocities
     * @param deltaTime Time step in seconds
     */
    void applyDamping(float deltaTime) noexcept;
    
    // ============================================================================
    // CCD (Continuous Collision Detection)
    // ============================================================================
    
    /**
     * @brief Set CCD enabled
     * @param enabled True if CCD enabled
     */
    void setCCDEnabled(bool enabled) noexcept;
    
    /**
     * @brief Get CCD enabled
     * @return True if CCD enabled
     */
    [[nodiscard]] bool isCCDEnabled() const noexcept;
    
    /**
     * @brief Set CCD motion threshold
     * @param threshold Motion threshold for CCD
     */
    void setCCDMotionThreshold(float threshold) noexcept;
    
    /**
     * @brief Get CCD motion threshold
     * @return CCD motion threshold
     */
    [[nodiscard]] float getCCDMotionThreshold() const noexcept;
    
    // ============================================================================
    // Velocity Limits
    // ============================================================================
    
    /**
     * @brief Set maximum linear velocity
     * @param maxVelocity Maximum linear velocity
     */
    void setMaxLinearVelocity(float maxVelocity) noexcept;
    
    /**
     * @brief Get maximum linear velocity
     * @return Maximum linear velocity
     */
    [[nodiscard]] float getMaxLinearVelocity() const noexcept;
    
    /**
     * @brief Set maximum angular velocity
     * @param maxVelocity Maximum angular velocity
     */
    void setMaxAngularVelocity(float maxVelocity) noexcept;
    
    /**
     * @brief Get maximum angular velocity
     * @return Maximum angular velocity
     */
    [[nodiscard]] float getMaxAngularVelocity() const noexcept;
    
    /**
     * @brief Clamp velocities to maximum limits
     */
    void clampVelocities() noexcept;
    
    // ============================================================================
    // Force Application
    // ============================================================================
    
    /**
     * @brief Apply force at center of mass
     * @param fx Force X
     * @param fy Force Y
     * @param fz Force Z
     */
    void applyForce(float fx, float fy, float fz) noexcept;
    
    /**
     * @brief Apply force at a specific point (generates torque)
     * @param fx Force X
     * @param fy Force Y
     * @param fz Force Z
     * @param px Point X (world space)
     * @param py Point Y (world space)
     * @param pz Point Z (world space)
     */
    void applyForceAtPoint(float fx, float fy, float fz, float px, float py, float pz) noexcept;
    
    /**
     * @brief Apply impulse at center of mass
     * @param ix Impulse X
     * @param iy Impulse Y
     * @param iz Impulse Z
     */
    void applyImpulse(float ix, float iy, float iz) noexcept;
    
    /**
     * @brief Apply impulse at a specific point (generates angular impulse)
     * @param ix Impulse X
     * @param iy Impulse Y
     * @param iz Impulse Z
     * @param px Point X (world space)
     * @param py Point Y (world space)
     * @param pz Point Z (world space)
     */
    void applyImpulseAtPoint(float ix, float iy, float iz, float px, float py, float pz) noexcept;
    
    /**
     * @brief Apply torque
     * @param tx Torque X
     * @param ty Torque Y
     * @param tz Torque Z
     */
    void applyTorque(float tx, float ty, float tz) noexcept;
    
    // ============================================================================
    // Accumulated Forces
    // ============================================================================
    
    /**
     * @brief Get accumulated force X
     * @return Force X
     */
    [[nodiscard]] float getAccumulatedForceX() const noexcept;
    
    /**
     * @brief Get accumulated force Y
     * @return Force Y
     */
    [[nodiscard]] float getAccumulatedForceY() const noexcept;
    
    /**
     * @brief Get accumulated force Z
     * @return Force Z
     */
    [[nodiscard]] float getAccumulatedForceZ() const noexcept;
    
    /**
     * @brief Get accumulated torque X
     * @return Torque X
     */
    [[nodiscard]] float getAccumulatedTorqueX() const noexcept;
    
    /**
     * @brief Get accumulated torque Y
     * @return Torque Y
     */
    [[nodiscard]] float getAccumulatedTorqueY() const noexcept;
    
    /**
     * @brief Get accumulated torque Z
     * @return Torque Z
     */
    [[nodiscard]] float getAccumulatedTorqueZ() const noexcept;
    
    /**
     * @brief Clear accumulated forces
     */
    void clearAccumulatedForces() noexcept;
    
    // ============================================================================
    // Integration
    // ============================================================================
    
    /**
     * @brief Integrate velocity using semi-implicit Euler
     * @param deltaTime Time step in seconds
     * @param gravityX Gravity X component
     * @param gravityY Gravity Y component
     * @param gravityZ Gravity Z component
     */
    void integrateVelocity(float deltaTime, float gravityX, float gravityY, float gravityZ) noexcept;

private:
    // Body properties
    BodyType m_bodyType;
    ShapeType m_shapeType;
    
    // Shape dimensions
    float m_sphereRadius;
    float m_boxHalfExtX;
    float m_boxHalfExtY;
    float m_boxHalfExtZ;
    float m_capsuleRadius;
    float m_capsuleHeight;
    
    // Material
    Material m_material;
    
    // Mass
    float m_mass;
    
    // Inertia tensor (3x3 matrix stored as 9 floats)
    float m_inertiaXX;
    float m_inertiaXY;
    float m_inertiaXZ;
    float m_inertiaYX;
    float m_inertiaYY;
    float m_inertiaYZ;
    float m_inertiaZX;
    float m_inertiaZY;
    float m_inertiaZZ;
    
    // Velocity
    float m_linearVelocityX;
    float m_linearVelocityY;
    float m_linearVelocityZ;
    float m_angularVelocityX;
    float m_angularVelocityY;
    float m_angularVelocityZ;
    
    // Gravity
    float m_gravityScale;
    
    // Collision
    uint32_t m_collisionLayer;
    uint32_t m_collisionMask;
    
    // Sleeping
    float m_sleepThreshold;
    bool m_sleepEnabled;
    bool m_isSleeping;
    
    // Damping
    float m_linearDamping;
    float m_angularDamping;
    
    // CCD
    bool m_ccdEnabled;
    float m_ccdMotionThreshold;
    float m_ccdSweptSphereRadius;
    
    // Velocity limits
    float m_maxLinearVelocity;
    float m_maxAngularVelocity;
    
    // Accumulated forces
    float m_accumulatedForceX;
    float m_accumulatedForceY;
    float m_accumulatedForceZ;
    float m_accumulatedTorqueX;
    float m_accumulatedTorqueY;
    float m_accumulatedTorqueZ;
};

} // namespace physics

/**
 * @brief Register Physics component with global registry
 */
void registerPhysicsComponent();

} // namespace components
} // namespace core
} // namespace poko

// Include inline implementations
#include "physics.inl"

#endif // POKO_CORE_COMPONENTS_PHYSICS_PHYSICS_H
