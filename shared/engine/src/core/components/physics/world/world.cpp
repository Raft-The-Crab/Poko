/**
 * @file world.cpp
 * @brief Physics world implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/world/world.h"
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace world {

World::World(const WorldConfig& config)
    : m_config(config)
    , m_solver({config.velocityIterations, config.positionIterations}) {
}

BodyHandle World::createBody(const RigidBody& body) {
    BodyHandle handle = static_cast<BodyHandle>(m_bodies.size());
    m_bodies.push_back(body);
    
    // Add to broadphase
    math::AABB aabb;
    aabb.expand(body.position);
    m_proxies.push_back(m_broadphase.addProxy(aabb, handle));
    
    return handle;
}

void World::destroyBody(BodyHandle handle) {
    if (handle >= m_bodies.size()) {
        return;
    }
    
    // Remove from broadphase
    m_broadphase.removeProxy(m_proxies[handle]);
    
    // Remove body (in a real implementation, use handle manager)
    m_bodies.erase(m_bodies.begin() + handle);
    m_proxies.erase(m_proxies.begin() + handle);
}

RigidBody* World::getBody(BodyHandle handle) {
    if (handle >= m_bodies.size()) {
        return nullptr;
    }
    return &m_bodies[handle];
}

const RigidBody* World::getBody(BodyHandle handle) const {
    if (handle >= m_bodies.size()) {
        return nullptr;
    }
    return &m_bodies[handle];
}

void World::step(float deltaTime) {
    // Fixed timestep accumulator
    static float accumulator = 0.0f;
    accumulator += deltaTime;
    
    while (accumulator >= m_config.fixedDeltaTime) {
        float dt = m_config.fixedDeltaTime;
        
        // Integration - Semi-implicit Euler
        integrateVelocities(dt);
        
        // Collision detection
        updateBroadphase();
        detectCollisions();
        
        // Constraint solving
        solveConstraints(dt);
        
        // Position integration
        integratePositions(dt);
        
        accumulator -= dt;
    }
}

void World::updateBroadphase() {
    for (size_t i = 0; i < m_bodies.size(); ++i) {
        math::AABB aabb;
        aabb.expand(m_bodies[i].position);
        m_broadphase.updateProxy(m_proxies[i], aabb);
    }
}

void World::detectCollisions() {
    m_manifolds.clear();
    
    // Get broadphase pairs
    std::vector<broadphase::DynamicAABBTree::Pair> pairs;
    m_broadphase.getPairs(pairs);
    
    // Narrowphase collision detection
    // For now, this is a placeholder - in a full implementation,
    // this would dispatch to shape-specific collision tests
    (void)pairs;
}

void World::solveConstraints(float deltaTime) {
    m_solver.solve(m_manifolds, deltaTime);
}

void World::integrateVelocities(float deltaTime) {
    for (auto& body : m_bodies) {
        if (body.isStatic || body.isSleeping) {
            continue;
        }
        
        // Apply gravity
        math::Vector3 gravityAccel = m_config.gravity * body.inverseMass;
        body.linearVelocity = body.linearVelocity + gravityAccel * deltaTime;
        
        // Apply damping
        body.linearVelocity = body.linearVelocity * (1.0f - 0.01f);
        body.angularVelocity = body.angularVelocity * (1.0f - 0.01f);
    }
}

void World::integratePositions(float deltaTime) {
    for (auto& body : m_bodies) {
        if (body.isStatic || body.isSleeping) {
            continue;
        }
        
        // Integrate position
        body.position = body.position + body.linearVelocity * deltaTime;
        
        // Integrate rotation (simplified)
        // In a full implementation, this would use quaternion integration
        (void)deltaTime;
    }
}

} // namespace world
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
