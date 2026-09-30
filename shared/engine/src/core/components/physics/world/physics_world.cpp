/**
 * @file physics_world.cpp
 * @brief Physics world implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/world/physics_world.h"
#include "core/components/physics/bounds/aabb.h"
#include <algorithm>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace world {

using bounds::AABB;

PhysicsWorld::PhysicsWorld(const PhysicsWorldSettings& settings_)
    : settings(settings_)
    , accumulator(0.0f) {
    broadphase = std::make_unique<DynamicAABBTree>();
}

PhysicsWorld::~PhysicsWorld() {
    clear();
}

void PhysicsWorld::clear() {
    bodies.clear();
    colliders.clear();
    shapes.clear();
    materials.clear();
    constraints.clear();
    bodyGenerations.clear();
    colliderGenerations.clear();
    shapeGenerations.clear();
    materialGenerations.clear();
    constraintGenerations.clear();
    bodyFreeList.clear();
    colliderFreeList.clear();
    shapeFreeList.clear();
    materialFreeList.clear();
    constraintFreeList.clear();
    manifolds.clear();
    islands.clear();
    accumulator = 0.0f;
}

void PhysicsWorld::step(float deltaTime) {
    // Clamp delta time
    if (deltaTime > settings.maxAccumulator) {
        deltaTime = settings.maxAccumulator;
    }
    
    accumulator += deltaTime * settings.timeScale;
    
    while (accumulator >= settings.fixedDelta) {
        float dt = settings.fixedDelta;
        
        // Process commands (would be done before integration)
        
        // Integrate velocities
        integrateVelocities(dt);
        
        // Update broadphase
        updateBroadphase();
        
        // Narrowphase collision detection
        narrowphaseCollisionDetection();
        
        // Build islands
        buildIslands();
        
        // Solve constraints
        solveConstraints();
        
        // Integrate positions
        integratePositions(dt);
        
        // Update sleeping
        updateSleeping(dt);
        
        accumulator -= dt;
    }
}

void PhysicsWorld::processCommands(CommandBuffer& commands) {
    size_t count = commands.getCommandCount();
    for (size_t i = 0; i < count; ++i) {
        // Process each command
        // This would handle create/destroy operations
    }
    commands.clear();
}

BodyHandle PhysicsWorld::createBody(const BodyDefinition& definition) {
    uint32_t index = allocateHandle(bodies, bodyGenerations, bodyFreeList);
    bodies[index] = definition;
    bodies[index].handle = BodyHandle(index, bodyGenerations[index]);
    return bodies[index].handle;
}

void PhysicsWorld::destroyBody(BodyHandle handle) {
    if (handle.index >= bodies.size()) return;
    if (bodyGenerations[handle.index] != handle.generation) return;
    
    freeHandle(bodies, bodyGenerations, bodyFreeList, handle.index);
}

ColliderHandle PhysicsWorld::createCollider(const ColliderDefinition& definition) {
    uint32_t index = allocateHandle(colliders, colliderGenerations, colliderFreeList);
    colliders[index] = definition;
    colliders[index].handle = ColliderHandle(index, colliderGenerations[index]);
    return colliders[index].handle;
}

void PhysicsWorld::destroyCollider(ColliderHandle handle) {
    if (handle.index >= colliders.size()) return;
    if (colliderGenerations[handle.index] != handle.generation) return;
    
    freeHandle(colliders, colliderGenerations, colliderFreeList, handle.index);
}

ShapeHandle PhysicsWorld::createShape(const ShapeDefinition& definition) {
    uint32_t index = allocateHandle(shapes, shapeGenerations, shapeFreeList);
    shapes[index] = definition;
    shapes[index].handle = ShapeHandle(index, shapeGenerations[index]);
    return shapes[index].handle;
}

MaterialHandle PhysicsWorld::createMaterial(const Material& material) {
    uint32_t index = allocateHandle(materials, materialGenerations, materialFreeList);
    materials[index] = material;
    materials[index].handle = MaterialHandle(index, materialGenerations[index]);
    return materials[index].handle;
}

ConstraintHandle PhysicsWorld::createConstraint(const ConstraintDefinition& definition) {
    uint32_t index = allocateHandle(constraints, constraintGenerations, constraintFreeList);
    constraints[index] = definition;
    constraints[index].handle = ConstraintHandle(index, constraintGenerations[index]);
    return constraints[index].handle;
}

void PhysicsWorld::destroyConstraint(ConstraintHandle handle) {
    if (handle.index >= constraints.size()) return;
    if (constraintGenerations[handle.index] != handle.generation) return;
    
    freeHandle(constraints, constraintGenerations, constraintFreeList, handle.index);
}

size_t PhysicsWorld::getBodyCount() const noexcept {
    return bodies.size() - bodyFreeList.size();
}

size_t PhysicsWorld::getColliderCount() const noexcept {
    return colliders.size() - colliderFreeList.size();
}

size_t PhysicsWorld::getContactCount() const noexcept {
    return manifolds.size();
}

size_t PhysicsWorld::getIslandCount() const noexcept {
    return islands.size();
}

void PhysicsWorld::integrateVelocities(float deltaTime) {
    for (auto& body : bodies) {
        if (!body.shouldIntegrate()) continue;
        
        // Apply gravity
        // Apply forces
        // Apply damping
        // Clamp velocities
        
        (void)deltaTime;
    }
}

void PhysicsWorld::integratePositions(float deltaTime) {
    for (auto& body : bodies) {
        if (!body.shouldIntegrate()) continue;
        
        // Integrate position: p = p + v * dt
        // Integrate rotation
        // Normalize quaternion
        
        (void)deltaTime;
    }
}

void PhysicsWorld::updateBroadphase() {
    // Update all collider AABBs in broadphase
    // This would iterate through colliders and call broadphase->update()
}

void PhysicsWorld::narrowphaseCollisionDetection() {
    // Get collision pairs from broadphase
    std::vector<CollisionPair> pairs;
    broadphase->generatePairs(pairs);
    
    // Dispatch collision tests for each pair
    for (const auto& pair : pairs) {
        // Get colliders
        // Get shapes
        // Get transforms
        // Dispatch collision test
        // Generate contacts
    }
}

void PhysicsWorld::buildIslands() {
    // Build islands from contacts and constraints
    islandBuilder.buildIslands(manifolds, {}, islands);
}

void PhysicsWorld::solveConstraints() {
    // Solve constraints for each island
    // This would use the SequentialImpulseSolver
}

void PhysicsWorld::updateSleeping(float deltaTime) {
    for (auto& island : islands) {
        if (island.isSleeping) continue;
        
        island.updateSleepTime(deltaTime);
        
        if (island.shouldSleep(settings.sleepThreshold)) {
            island.wake();
        }
    }
}

template<typename T>
uint32_t PhysicsWorld::allocateHandle(std::vector<T>& storage, std::vector<uint32_t>& generations, std::vector<uint32_t>& freeList) {
    if (!freeList.empty()) {
        uint32_t index = freeList.back();
        freeList.pop_back();
        generations[index]++;
        return index;
    }
    
    uint32_t index = static_cast<uint32_t>(storage.size());
    storage.emplace_back();
    generations.push_back(1);
    return index;
}

template<typename T>
void PhysicsWorld::freeHandle(std::vector<T>& storage, std::vector<uint32_t>& generations, std::vector<uint32_t>& freeList, uint32_t index) {
    freeList.push_back(index);
    generations[index]++;
}

} // namespace world
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
