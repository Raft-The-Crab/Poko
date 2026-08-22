/**
 * @file physics_world.h
 * @brief Physics world interface for PokoEngine using Jolt Physics
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#pragma once

#include <cstdint>
#include <vector>
#include <cstdint>
#include <mutex>
#include <shared_mutex>
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

namespace Poko {
namespace Physics {

// Forward declarations
class BPLayerInterfaceImpl;

// Collision layer bit masks
constexpr uint8_t COLLISION_LAYER_DEFAULT = 0;
constexpr uint8_t COLLISION_LAYER_STATIC = 1;
constexpr uint8_t COLLISION_LAYER_DYNAMIC = 2;
constexpr uint8_t COLLISION_LAYER_TRIGGER = 3;
constexpr uint8_t COLLISION_LAYER_ALL = 0xFF;

// Physics body structure
struct PhysicsBody {
    uint32_t id;
    JPH::BodyID jolt_body_id;
    float mass;
    float radius;
    bool is_static;
    uint8_t collision_layer;
    uint8_t collision_mask;
};

// Raycast result structure
struct RaycastHit {
    bool hit;
    uint32_t body_id;
    float position_x, position_y, position_z;
    float normal_x, normal_y, normal_z;
    float distance;
};

/**
 * @brief Physics world configuration
 */
struct PhysicsConfig {
    uint32_t max_bodies = 1024;
    uint32_t max_body_pairs = 1024;
    uint32_t max_contact_constraints = 1024;
    float gravity = -9.81f;
    uint32_t num_threads = 0; // 0 = auto-detect
};

/**
 * @brief Physics world using Jolt Physics
 */
class PhysicsWorld {
public:
    PhysicsWorld();
    ~PhysicsWorld();

    /**
     * @brief Initialize the physics world
     * @param config Physics configuration
     * @return true if initialization succeeded
     */
    bool Initialize(const PhysicsConfig& config);

    /**
     * @brief Shutdown the physics world
     */
    void Shutdown();

    /**
     * @brief Update physics simulation
     * @param delta_time Time step in seconds
     */
    void Update(float delta_time);

    /**
     * @brief Create a physics body
     * @param x Initial X position
     * @param y Initial Y position
     * @param z Initial Z position
     * @param mass Body mass (0 for static)
     * @param is_static Whether body is static
     * @return Body ID
     */
    uint32_t CreateBody(float x, float y, float z, float mass, bool is_static = false);

    /**
     * @brief Create a physics body with radius
     * @param x Initial X position
     * @param y Initial Y position
     * @param z Initial Z position
     * @param mass Body mass (0 for static)
     * @param radius Collision radius
     * @param is_static Whether body is static
     * @return Body ID
     */
    uint32_t CreateBody(float x, float y, float z, float mass, float radius, bool is_static = false);

    /**
     * @brief Remove a physics body
     * @param body_id Body ID to remove
     */
    void RemoveBody(uint32_t body_id);

    /**
     * @brief Set body velocity
     * @param body_id Body ID
     * @param x X velocity
     * @param y Y velocity
     * @param z Z velocity
     */
    void SetBodyVelocity(uint32_t body_id, float x, float y, float z);

    /**
     * @brief Get body position
     * @param body_id Body ID
     * @param x Output X position
     * @param y Output Y position
     * @param z Output Z position
     */
    void GetBodyPosition(uint32_t body_id, float& x, float& y, float& z) const;

    /**
     * @brief Get number of bodies
     * @return Body count
     */
    size_t GetBodyCount() const;

    /**
     * @brief Check collision between two bodies
     * @param body_id1 First body ID
     * @param body_id2 Second body ID
     * @return true if bodies are colliding
     */
    bool CheckCollision(uint32_t body_id1, uint32_t body_id2) const;

    /**
     * @brief Check if initialized
     * @return true if initialized
     */
    bool IsInitialized() const;

    /**
     * @brief Cast a ray into the physics world
     * @param start_x Ray start X
     * @param start_y Ray start Y
     * @param start_z Ray start Z
     * @param dir_x Ray direction X (normalized)
     * @param dir_y Ray direction Y (normalized)
     * @param dir_z Ray direction Z (normalized)
     * @param max_distance Maximum ray distance
     * @param collision_mask Collision layers to test against
     * @return Raycast hit result
     */
    RaycastHit Raycast(float start_x, float start_y, float start_z,
                      float dir_x, float dir_y, float dir_z,
                      float max_distance, uint32_t collision_mask = COLLISION_LAYER_ALL) const;

    /**
     * @brief Set body collision layer and mask
     * @param body_id Body ID
     * @param layer Collision layer
     * @param mask Collision mask (layers this body collides with)
     */
    void SetBodyCollisionLayer(uint32_t body_id, uint32_t layer, uint32_t mask);

    /**
     * @brief Get body collision layer
     * @param body_id Body ID
     * @return Collision layer
     */
    uint32_t GetBodyCollisionLayer(uint32_t body_id) const;

    /**
     * @brief Get body collision mask
     * @param body_id Body ID
     * @return Collision mask
     */
    uint32_t GetBodyCollisionMask(uint32_t body_id) const;

private:
    mutable std::shared_mutex m_mutex;
    bool m_initialized;
    PhysicsConfig m_config;
    uint32_t m_body_id_counter;
    float m_accumulated_time;
    float m_fixed_time_step;
    std::vector<PhysicsBody> m_bodies;
    
    // Jolt Physics components
    JPH::TempAllocatorImpl* m_temp_allocator;
    JPH::JobSystemThreadPool* m_job_system;
    JPH::PhysicsSystem* m_physics_system;
    BPLayerInterfaceImpl* m_broad_phase_layer_interface;
    JPH::ObjectLayerPairFilter* m_object_layer_filter;
};

} // namespace Physics
} // namespace Poko
