/**
 * @file world.h
 * @brief Physics world simulation manager
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_WORLD_WORLD_H
#define POKO_CORE_COMPONENTS_PHYSICS_WORLD_WORLD_H

#include "../math/vector3.h"
#include "../math/quaternion.h"
#include "../math/matrix3x3.h"
#include "../broadphase/dynamic_aabb_tree.h"
#include "../narrowphase/contact.h"
#include "../solver/sequential_impulse.h"
#include <vector>
#include <cstdint>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace world {

using math::Vector3;
using math::Quaternion;
using math::Matrix3x3;
using math::AABB;

/**
 * @brief Physics world configuration
 */
struct WorldConfig {
    math::Vector3 gravity = math::Vector3(0.0f, -9.81f, 0.0f);
    float fixedDeltaTime = 1.0f / 60.0f;
    uint32_t velocityIterations = 8;
    uint32_t positionIterations = 3;
    bool enableSleeping = true;
    float sleepThreshold = 0.01f;
};

/**
 * @brief Rigid body handle
 */
using BodyHandle = uint32_t;
static constexpr BodyHandle INVALID_BODY = 0xFFFFFFFF;

/**
 * @brief Rigid body data
 */
struct RigidBody {
    math::Vector3 position;
    math::Vector3 linearVelocity;
    math::Vector3 angularVelocity;
    math::Quaternion rotation;
    float mass;
    float inverseMass;
    math::Matrix3x3 inertia;
    math::Matrix3x3 inverseInertia;
    uint32_t collisionLayer;
    uint32_t collisionMask;
    bool isStatic;
    bool isSleeping;
    uint64_t userData;
    
    RigidBody() noexcept
        : mass(1.0f)
        , inverseMass(1.0f)
        , inertia(Matrix3x3::identity())
        , inverseInertia(Matrix3x3::identity())
        , collisionLayer(1)
        , collisionMask(0xFFFFFFFF)
        , isStatic(false)
        , isSleeping(false)
        , userData(0) {}
};

/**
 * @brief Physics world
 * 
 * Main simulation manager that coordinates:
 * - Broadphase collision detection
 * - Narrowphase collision detection
 * - Constraint solving
 * - Integration
 */
class World {
public:
    /**
     * @brief Constructor
     */
    explicit World(const WorldConfig& config = WorldConfig());
    
    /**
     * @brief Destructor
     */
    ~World() = default;
    
    /**
     * @brief Create a rigid body
     */
    [[nodiscard]] BodyHandle createBody(const RigidBody& body);
    
    /**
     * @brief Destroy a rigid body
     */
    void destroyBody(BodyHandle handle);
    
    /**
     * @brief Get rigid body
     */
    [[nodiscard]] RigidBody* getBody(BodyHandle handle);
    
    /**
     * @brief Get rigid body (const)
     */
    [[nodiscard]] const RigidBody* getBody(BodyHandle handle) const;
    
    /**
     * @brief Step simulation
     */
    void step(float deltaTime);
    
    /**
     * @brief Get gravity
     */
    [[nodiscard]] math::Vector3 getGravity() const noexcept { return m_config.gravity; }
    
    /**
     * @brief Set gravity
     */
    void setGravity(const math::Vector3& gravity) noexcept { m_config.gravity = gravity; }
    
    /**
     * @brief Get contact manifolds
     */
    [[nodiscard]] const std::vector<narrowphase::ContactManifold>& getContactManifolds() const noexcept {
        return m_manifolds;
    }
    
    /**
     * @brief Get body count
     */
    [[nodiscard]] uint32_t getBodyCount() const noexcept { return m_bodies.size(); }
    
private:
    /**
     * @brief Update broadphase
     */
    void updateBroadphase();
    
    /**
     * @ Detect collisions
     */
    void detectCollisions();
    
    /**
     * @brief Solve constraints
     */
    void solveConstraints(float deltaTime);
    
    /**
     * @brief Integrate velocities
     */
    void integrateVelocities(float deltaTime);
    
    /**
     * @brief Integrate positions
     */
    void integratePositions(float deltaTime);
    
    WorldConfig m_config;
    std::vector<RigidBody> m_bodies;
    std::vector<broadphase::DynamicAABBTree::ProxyId> m_proxies;
    broadphase::DynamicAABBTree m_broadphase;
    std::vector<narrowphase::ContactManifold> m_manifolds;
    solver::SequentialImpulse m_solver;
};

} // namespace world
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_WORLD_WORLD_H
