/**
 * @file physics_world.h
 * @brief Physics world - primary simulation owner
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_WORLD_PHYSICS_WORLD_H
#define POKO_CORE_COMPONENTS_PHYSICS_WORLD_PHYSICS_WORLD_H

#include "core/components/physics/core/settings.h"
#include "core/components/physics/core/command.h"
#include "core/components/physics/core/handle.h"
#include "core/components/physics/bodies/body_definition.h"
#include "core/components/physics/colliders/collider_definition.h"
#include "core/components/physics/shapes/shape_definition.h"
#include "core/components/physics/materials/material.h"
#include "core/components/physics/broadphase/ibroadphase.h"
#include "core/components/physics/broadphase/dynamic_aabb_tree.h"
#include "core/components/physics/narrowphase/collision_dispatcher.h"
#include "core/components/physics/contacts/contact_manifold.h"
#include "core/components/physics/constraints/constraint_definition.h"
#include "core/components/physics/solver/sequential_impulse.h"
#include "core/components/physics/simulation/island.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace world {

using core::PhysicsWorldSettings;
using core::CommandBuffer;
using core::BodyHandle;
using core::ColliderHandle;
using core::ShapeHandle;
using core::MaterialHandle;
using core::ConstraintHandle;
using bodies::BodyDefinition;
using bodies::MotionType;
using colliders::ColliderDefinition;
using shapes::ShapeDefinition;
using materials::Material;
using broadphase::IBroadphase;
using broadphase::DynamicAABBTree;
using broadphase::CollisionPair;
using narrowphase::CollisionDispatcher;
using contacts::ContactManifold;
using constraints::ConstraintDefinition;
using solver::SequentialImpulseSolver;
using simulation::Island;
using simulation::IslandBuilder;

/**
 * @brief Physics world - primary simulation owner
 * 
 * Owns all physics subsystems and manages the simulation loop.
 * Multiple independent physics worlds must be supported.
 */
class PhysicsWorld {
public:
    /**
     * @brief Constructor
     */
    explicit PhysicsWorld(const PhysicsWorldSettings& settings);
    
    /**
     * @brief Destructor
     */
    ~PhysicsWorld();

    /**
     * @brief Clear all data
     */
    void clear();
    
    /**
     * @brief Step simulation
     */
    void step(float deltaTime);
    
    /**
     * @brief Process command buffer
     */
    void processCommands(CommandBuffer& commands);
    
    /**
     * @brief Create body
     */
    BodyHandle createBody(const BodyDefinition& definition);
    
    /**
     * @brief Destroy body
     */
    void destroyBody(BodyHandle handle);
    
    /**
     * @brief Create collider
     */
    ColliderHandle createCollider(const ColliderDefinition& definition);
    
    /**
     * @brief Destroy collider
     */
    void destroyCollider(ColliderHandle handle);
    
    /**
     * @brief Create shape
     */
    ShapeHandle createShape(const ShapeDefinition& definition);
    
    /**
     * @brief Create material
     */
    MaterialHandle createMaterial(const Material& material);
    
    /**
     * @brief Create constraint
     */
    ConstraintHandle createConstraint(const ConstraintDefinition& definition);
    
    /**
     * @brief Destroy constraint
     */
    void destroyConstraint(ConstraintHandle handle);
    
    /**
     * @brief Get body count
     */
    [[nodiscard]] size_t getBodyCount() const noexcept;
    
    /**
     * @brief Get collider count
     */
    [[nodiscard]] size_t getColliderCount() const noexcept;
    
    /**
     * @brief Get contact count
     */
    [[nodiscard]] size_t getContactCount() const noexcept;
    
    /**
     * @brief Get island count
     */
    [[nodiscard]] size_t getIslandCount() const noexcept;
    
private:
    PhysicsWorldSettings settings;
    
    // Storage
    std::vector<BodyDefinition> bodies;
    std::vector<ColliderDefinition> colliders;
    std::vector<ShapeDefinition> shapes;
    std::vector<Material> materials;
    std::vector<ConstraintDefinition> constraints;
    
    // Handle management
    std::vector<uint32_t> bodyGenerations;
    std::vector<uint32_t> colliderGenerations;
    std::vector<uint32_t> shapeGenerations;
    std::vector<uint32_t> materialGenerations;
    std::vector<uint32_t> constraintGenerations;
    
    std::vector<uint32_t> bodyFreeList;
    std::vector<uint32_t> colliderFreeList;
    std::vector<uint32_t> shapeFreeList;
    std::vector<uint32_t> materialFreeList;
    std::vector<uint32_t> constraintFreeList;
    
    // Subsystems
    std::unique_ptr<DynamicAABBTree> broadphase;
    CollisionDispatcher dispatcher;
    SequentialImpulseSolver solver;
    IslandBuilder islandBuilder;
    
    // Contacts and islands
    std::vector<ContactManifold> manifolds;
    std::vector<Island> islands;
    
    // Time management
    float accumulator;
    
    /**
     * @brief Integrate velocities
     */
    void integrateVelocities(float deltaTime);
    
    /**
     * @brief Integrate positions
     */
    void integratePositions(float deltaTime);
    
    /**
     * @brief Update broadphase
     */
    void updateBroadphase();
    
    /**
     * @brief Narrowphase collision detection
     */
    void narrowphaseCollisionDetection();
    
    /**
     * @brief Build islands
     */
    void buildIslands();
    
    /**
     * @brief Solve constraints
     */
    void solveConstraints();
    
    /**
     * @brief Update sleeping
     */
    void updateSleeping(float deltaTime);
    
    /**
     * @brief Allocate handle
     */
    template<typename T>
    uint32_t allocateHandle(std::vector<T>& storage, std::vector<uint32_t>& generations, std::vector<uint32_t>& freeList);
    
    /**
     * @brief Free handle
     */
    template<typename T>
    void freeHandle(std::vector<T>& storage, std::vector<uint32_t>& generations, std::vector<uint32_t>& freeList, uint32_t index);
};

} // namespace world
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_WORLD_PHYSICS_WORLD_H
