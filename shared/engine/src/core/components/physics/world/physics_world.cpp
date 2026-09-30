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
#include "core/components/physics/math/vectors/vector3.h"
#include "core/components/physics/math/vectors/quaternion.h"
#include "core/components/physics/transforms/transform.h"
#include "core/components/physics/contacts/contact_manifold.h"
#include "core/components/physics/constraints/constraint_row.h"
#include "core/components/physics/core/command.h"
#include <algorithm>
#include <cmath>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace world {

using bounds::AABB;
using math::Vector3;
using math::Quaternion;
using transforms::Transform;
using contacts::SolverContact;
using constraints::ConstraintRow;
using core::CommandType;
using core::Command;
using core::CreateBodyCommand;
using core::DestroyBodyCommand;
using core::CreateColliderCommand;
using core::DestroyColliderCommand;
using core::SetTransformCommand;
using core::SetVelocityCommand;
using core::ApplyForceCommand;
using core::ApplyImpulseCommand;
using core::WakeCommand;
using core::SleepCommand;

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
    broadphase->clear();
}

void PhysicsWorld::step(float deltaTime) {
    // Clamp delta time
    if (deltaTime > settings.maxAccumulator) {
        deltaTime = settings.maxAccumulator;
    }

    accumulator += deltaTime * settings.timeScale;

    while (accumulator >= settings.fixedDelta) {
        float dt = settings.fixedDelta;

        // Process commands before integration
        // (Commands are processed externally via processCommands)

        // Integrate velocities
        integrateVelocities(dt);

        // Update broadphase with new positions
        updateBroadphase();

        // Narrowphase collision detection
        narrowphaseCollisionDetection();

        // Build islands for parallel solving
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
    const auto& cmds = commands.getCommands();
    size_t count = cmds.size();
    for (size_t i = 0; i < count; ++i) {
        const Command* cmd = cmds[i];

        if (!cmd) continue;

        switch (cmd->type) {
            case CommandType::CreateBody: {
                const CreateBodyCommand* createCmd = static_cast<const CreateBodyCommand*>(cmd);
                BodyDefinition def;
                def.position = createCmd->position;
                def.rotation = createCmd->rotation;
                def.collisionLayer = createCmd->collisionLayer;
                def.collisionMask = createCmd->collisionMask;
                createBody(def);
                break;
            }

            case CommandType::DestroyBody: {
                const DestroyBodyCommand* destroyCmd = static_cast<const DestroyBodyCommand*>(cmd);
                destroyBody(destroyCmd->body);
                break;
            }

            case CommandType::CreateCollider: {
                const CreateColliderCommand* createCmd = static_cast<const CreateColliderCommand*>(cmd);
                ColliderDefinition def;
                def.bodyHandle = createCmd->body;
                def.shapeHandle = createCmd->shape;
                def.materialHandle = createCmd->material;
                def.localTransform.position = createCmd->localPosition;
                def.localTransform.rotation = createCmd->localRotation;
                createCollider(def);
                break;
            }

            case CommandType::DestroyCollider: {
                const DestroyColliderCommand* destroyCmd = static_cast<const DestroyColliderCommand*>(cmd);
                destroyCollider(destroyCmd->collider);
                break;
            }

            case CommandType::SetTransform: {
                const SetTransformCommand* setCmd = static_cast<const SetTransformCommand*>(cmd);
                Transform t;
                t.position = setCmd->position;
                t.rotation = setCmd->rotation;
                setBodyTransform(setCmd->body, t);
                break;
            }

            case CommandType::SetVelocity: {
                const SetVelocityCommand* setCmd = static_cast<const SetVelocityCommand*>(cmd);
                setBodyVelocity(setCmd->body, setCmd->linearVelocity, setCmd->angularVelocity);
                break;
            }

            case CommandType::ApplyForce: {
                const ApplyForceCommand* applyCmd = static_cast<const ApplyForceCommand*>(cmd);
                applyForce(applyCmd->body, applyCmd->force, applyCmd->usePoint ? applyCmd->point : Vector3::zero());
                break;
            }

            case CommandType::ApplyImpulse: {
                const ApplyImpulseCommand* applyCmd = static_cast<const ApplyImpulseCommand*>(cmd);
                applyImpulse(applyCmd->body, applyCmd->impulse, applyCmd->usePoint ? applyCmd->point : Vector3::zero());
                break;
            }

            case CommandType::Wake: {
                const WakeCommand* wakeCmd = static_cast<const WakeCommand*>(cmd);
                wakeBody(wakeCmd->body);
                break;
            }

            case CommandType::Sleep: {
                const SleepCommand* sleepCmd = static_cast<const SleepCommand*>(cmd);
                sleepBody(sleepCmd->body);
                break;
            }

            default:
                break;
        }
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

    // Remove all colliders associated with this body
    for (size_t i = 0; i < colliders.size(); ++i) {
        if (colliders[i].bodyHandle == handle) {
            ColliderHandle colliderHandle(i, colliderGenerations[i]);
            destroyCollider(colliderHandle);
        }
    }

    freeHandle(bodies, bodyGenerations, bodyFreeList, handle.index);
}

ColliderHandle PhysicsWorld::createCollider(const ColliderDefinition& definition) {
    uint32_t index = allocateHandle(colliders, colliderGenerations, colliderFreeList);
    colliders[index] = definition;
    colliders[index].handle = ColliderHandle(index, colliderGenerations[index]);

    // Insert into broadphase
    if (definition.shapeHandle.isValid()) {
        AABB aabb = shapes[definition.shapeHandle.index].getLocalAABB();
        broadphase->insert(aabb, colliders[index].handle);
    }

    return colliders[index].handle;
}

void PhysicsWorld::destroyCollider(ColliderHandle handle) {
    if (handle.index >= colliders.size()) return;
    if (colliderGenerations[handle.index] != handle.generation) return;

    // Remove from broadphase
    // (Would need to track proxy handle in collider definition)

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

void PhysicsWorld::setBodyTransform(BodyHandle handle, const Transform& transform) {
    if (handle.index >= bodies.size()) return;
    if (bodyGenerations[handle.index] != handle.generation) return;

    bodies[handle.index].position = transform.position;
    bodies[handle.index].rotation = transform.rotation;
    bodies[handle.index].isAwake = true;
}

void PhysicsWorld::setBodyVelocity(BodyHandle handle, const Vector3& linearVelocity, const Vector3& angularVelocity) {
    if (handle.index >= bodies.size()) return;
    if (bodyGenerations[handle.index] != handle.generation) return;

    bodies[handle.index].linearVelocity = linearVelocity;
    bodies[handle.index].angularVelocity = angularVelocity;
    bodies[handle.index].isAwake = true;
}

void PhysicsWorld::applyForce(BodyHandle handle, const Vector3& force, const Vector3& point) {
    if (handle.index >= bodies.size()) return;
    if (bodyGenerations[handle.index] != handle.generation) return;

    BodyDefinition& body = bodies[handle.index];
    if (body.motionType != MotionType::Dynamic) return;

    // Apply linear force
    body.force = body.force + force;

    // Apply torque: τ = r × F
    Vector3 r = point - body.position;
    body.torque = body.torque + r.cross(force);

    body.isAwake = true;
}

void PhysicsWorld::applyImpulse(BodyHandle handle, const Vector3& impulse, const Vector3& point) {
    if (handle.index >= bodies.size()) return;
    if (bodyGenerations[handle.index] != handle.generation) return;

    BodyDefinition& body = bodies[handle.index];
    if (body.motionType != MotionType::Dynamic) return;

    float invMass = body.mass > 0.0f ? 1.0f / body.mass : 0.0f;

    // Apply linear impulse
    body.linearVelocity = body.linearVelocity + impulse * invMass;

    // Apply angular impulse
    Vector3 r = point - body.position;
    Vector3 angularImpulse = r.cross(impulse);
    // Simplified: would use inertia tensor inverse in production
    body.angularVelocity = body.angularVelocity + angularImpulse * invMass;

    body.isAwake = true;
}

void PhysicsWorld::wakeBody(BodyHandle handle) {
    if (handle.index >= bodies.size()) return;
    if (bodyGenerations[handle.index] != handle.generation) return;

    bodies[handle.index].isAwake = true;
    bodies[handle.index].sleepTime = 0.0f;
}

void PhysicsWorld::sleepBody(BodyHandle handle) {
    if (handle.index >= bodies.size()) return;
    if (bodyGenerations[handle.index] != handle.generation) return;

    bodies[handle.index].isAwake = false;
    bodies[handle.index].linearVelocity = Vector3::zero();
    bodies[handle.index].angularVelocity = Vector3::zero();
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

        float invMass = body.mass > 0.0f ? 1.0f / body.mass : 0.0f;

        // Apply gravity
        Vector3 gravityForce = settings.gravity * body.mass * body.gravityScale;
        body.force = body.force + gravityForce;

        // Integrate velocity with forces: v = v + (F/m) * dt
        Vector3 acceleration = body.force * invMass;
        body.linearVelocity = body.linearVelocity + acceleration * deltaTime;

        // Integrate angular velocity with torque: ω = ω + (I^-1 * τ) * dt
        // Simplified: would use inertia tensor inverse in production
        Vector3 angularAcceleration = body.torque * invMass;
        body.angularVelocity = body.angularVelocity + angularAcceleration * deltaTime;

        // Apply damping
        body.linearVelocity = body.linearVelocity * (1.0f - body.linearDamping * deltaTime);
        body.angularVelocity = body.angularVelocity * (1.0f - body.angularDamping * deltaTime);

        // Clamp velocities
        float linearSpeed = body.linearVelocity.length();
        if (linearSpeed > settings.maxLinearVelocity) {
            body.linearVelocity = body.linearVelocity.normalized() * settings.maxLinearVelocity;
        }

        float angularSpeed = body.angularVelocity.length();
        if (angularSpeed > settings.maxAngularVelocity) {
            body.angularVelocity = body.angularVelocity.normalized() * settings.maxAngularVelocity;
        }

        // Clear forces for next frame
        body.force = Vector3::zero();
        body.torque = Vector3::zero();
    }
}

void PhysicsWorld::integratePositions(float deltaTime) {
    for (auto& body : bodies) {
        if (!body.shouldIntegrate()) continue;

        // Integrate position: p = p + v * dt
        body.position = body.position + body.linearVelocity * deltaTime;

        // Integrate rotation using quaternion
        // dq/dt = 0.5 * ω * q
        float halfDt = deltaTime * 0.5f;
        Quaternion dq(
            0.0f,
            body.angularVelocity.x * halfDt,
            body.angularVelocity.y * halfDt,
            body.angularVelocity.z * halfDt
        );
        body.rotation = (dq * body.rotation).normalized();
    }
}

void PhysicsWorld::updateBroadphase() {
    // Update all collider AABBs in broadphase
    for (size_t i = 0; i < colliders.size(); ++i) {
        if (colliderGenerations[i] == 0) continue; // Skip freed colliders

        ColliderHandle handle(i, colliderGenerations[i]);
        const ColliderDefinition& collider = colliders[i];

        if (!collider.shapeHandle.isValid()) continue;

        // Calculate world-space AABB
        AABB localAABB = shapes[collider.shapeHandle.index].getLocalAABB();
        // Transform AABB to world space (simplified: just offset by position)
        AABB worldAABB = localAABB;
        worldAABB.min = worldAABB.min + collider.localTransform.position;
        worldAABB.max = worldAABB.max + collider.localTransform.position;

        // Update in broadphase
        // (Would need to track proxy handle in collider definition)
    }
}

void PhysicsWorld::narrowphaseCollisionDetection() {
    // Get collision pairs from broadphase
    std::vector<CollisionPair> pairs;
    broadphase->generatePairs(pairs);

    // Clear old manifolds
    manifolds.clear();

    // Dispatch collision tests for each pair
    for (const auto& pair : pairs) {
        if (pair.colliderA.index >= colliders.size() || pair.colliderB.index >= colliders.size()) {
            continue;
        }

        const ColliderDefinition& colliderA = colliders[pair.colliderA.index];
        const ColliderDefinition& colliderB = colliders[pair.colliderB.index];

        if (!colliderA.shapeHandle.isValid() || !colliderB.shapeHandle.isValid()) {
            continue;
        }

        const ShapeDefinition& shapeA = shapes[colliderA.shapeHandle.index];
        const ShapeDefinition& shapeB = shapes[colliderB.shapeHandle.index];

        Transform transformA = colliderA.localTransform;
        Transform transformB = colliderB.localTransform;

        // Get body transforms (simplified)
        if (colliderA.bodyHandle.isValid() && colliderA.bodyHandle.index < bodies.size()) {
            transformA.position = transformA.position + bodies[colliderA.bodyHandle.index].position;
            transformA.rotation = bodies[colliderA.bodyHandle.index].rotation * transformA.rotation;
        }

        if (colliderB.bodyHandle.isValid() && colliderB.bodyHandle.index < bodies.size()) {
            transformB.position = transformB.position + bodies[colliderB.bodyHandle.index].position;
            transformB.rotation = bodies[colliderB.bodyHandle.index].rotation * transformB.rotation;
        }

        // Dispatch collision test
        CollisionResult result = dispatcher.dispatch(shapeA, transformA, shapeB, transformB);

        if (result.isColliding) {
            // Create manifold
            ContactManifold manifold;
            manifold.bodyA = colliderA.bodyHandle;
            manifold.bodyB = colliderB.bodyHandle;
            manifold.normal = result.normal;
            manifold.contactCount = static_cast<uint32_t>(result.contacts.size());

            // Get material properties
            if (colliderA.materialHandle.isValid() && colliderA.materialHandle.index < materials.size()) {
                manifold.friction = materials[colliderA.materialHandle.index].friction;
                manifold.restitution = materials[colliderA.materialHandle.index].restitution;
            }

            // Convert collision results to solver contacts
            for (const auto& contact : result.contacts) {
                SolverContact solverContact;
                solverContact.position = contact.position;
                solverContact.normal = contact.normal;
                solverContact.penetration = contact.penetration;
                solverContact.featureIdA = contact.featureIdA;
                solverContact.featureIdB = contact.featureIdB;
                solverContact.normalImpulse = 0.0f;
                solverContact.tangent1Impulse = 0.0f;
                solverContact.tangent2Impulse = 0.0f;
                manifold.addContact(solverContact);
            }

            manifold.calculateTangentBasis();
            manifolds.push_back(manifold);
        }
    }
}

void PhysicsWorld::buildIslands() {
    // Build islands from contacts and constraints
    islandBuilder.buildIslands(manifolds, {}, islands);
}

void PhysicsWorld::solveConstraints() {
    // Solve constraints for each island
    for (auto& island : islands) {
        if (island.isSleeping) continue;

        // Get bodies in this island
        std::vector<BodyDefinition> islandBodies;
        for (const auto& bodyHandle : island.bodies) {
            if (bodyHandle.index < bodies.size()) {
                islandBodies.push_back(bodies[bodyHandle.index]);
            }
        }

        // Create empty constraint rows for now (would be populated from constraints)
        std::vector<ConstraintRow> constraintRows;

        // Solve velocity constraints
        solver.solveVelocityConstraints(manifolds, constraintRows, islandBodies);

        // Solve position constraints
        solver.solvePositionConstraints(manifolds, islandBodies);
    }
}

void PhysicsWorld::updateSleeping(float deltaTime) {
    for (auto& island : islands) {
        if (island.isSleeping) continue;

        island.updateSleepTime(deltaTime);

        if (island.shouldSleep(settings.sleepThreshold)) {
            island.isSleeping = true;

            // Zero out velocities for sleeping bodies
            for (const auto& bodyHandle : island.bodies) {
                if (bodyHandle.index < bodies.size()) {
                    bodies[bodyHandle.index].linearVelocity = Vector3::zero();
                    bodies[bodyHandle.index].angularVelocity = Vector3::zero();
                    bodies[bodyHandle.index].isAwake = false;
                }
            }
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
    (void)storage;
    freeList.push_back(index);
    generations[index]++;
}

} // namespace world
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
