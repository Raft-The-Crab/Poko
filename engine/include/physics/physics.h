/**
 * @file physics.h
 * @brief Physics system for Poko Engine
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#ifndef POKO_PHYSICS_H
#define POKO_PHYSICS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum PhysicsBodyType
 * @brief Types of physics bodies
 */
typedef enum {
    PHYSICS_BODY_STATIC,
    PHYSICS_BODY_KINEMATIC,
    PHYSICS_BODY_DYNAMIC,
    PHYSICS_BODY_CHARACTER
} PhysicsBodyType;

/**
 * @enum PhysicsShapeType
 * @brief Types of collision shapes
 */
typedef enum {
    PHYSICS_SHAPE_SPHERE,
    PHYSICS_SHAPE_BOX,
    PHYSICS_SHAPE_CAPSULE,
    PHYSICS_SHAPE_CYLINDER,
    PHYSICS_SHAPE_CONVEX_HULL,
    PHYSICS_SHAPE_TRIANGLE_MESH,
    PHYSICS_SHAPE_HEIGHTFIELD
} PhysicsShapeType;

/**
 * @struct PhysicsBody
 * @brief Physics body handle
 */
typedef struct PhysicsBody PhysicsBody;

/**
 * @struct PhysicsWorld
 * @brief Physics simulation world
 */
typedef struct PhysicsWorld PhysicsWorld;

/**
 * @struct PhysicsMaterial
 * @brief Physics material properties
 */
typedef struct {
    float friction;
    float restitution;
    float density;
} PhysicsMaterial;

/**
 * @struct PhysicsRaycastResult
 * @brief Result of a raycast query
 */
typedef struct {
    bool hit;
    float hit_fraction;
    float hit_position[3];
    float hit_normal[3];
    PhysicsBody* hit_body;
} PhysicsRaycastResult;

/**
 * @brief Create physics world
 * @return Physics world handle
 */
PhysicsWorld* physics_world_create(void);

/**
 * @brief Destroy physics world
 * @param world Physics world to destroy
 */
void physics_world_destroy(PhysicsWorld* world);

/**
 * @brief Step physics simulation
 * @param world Physics world
 * @param delta_time Time step in seconds
 */
void physics_world_step(PhysicsWorld* world, float delta_time);

/**
 * @brief Set gravity
 * @param world Physics world
 * @param x Gravity X component
 * @param y Gravity Y component
 * @param z Gravity Z component
 */
void physics_world_set_gravity(PhysicsWorld* world, float x, float y, float z);

/**
 * @brief Get gravity
 * @param world Physics world
 * @param x Output gravity X
 * @param y Output gravity Y
 * @param z Output gravity Z
 */
void physics_world_get_gravity(const PhysicsWorld* world, float* x, float* y, float* z);

/**
 * @brief Create physics body
 * @param world Physics world
 * @param type Body type
 * @param shape_type Collision shape type
 * @param material Material properties
 * @return Physics body handle
 */
PhysicsBody* physics_body_create(PhysicsWorld* world, PhysicsBodyType type, PhysicsShapeType shape_type, const PhysicsMaterial* material);

/**
 * @brief Destroy physics body
 * @param body Physics body to destroy
 */
void physics_body_destroy(PhysicsBody* body);

/**
 * @brief Set body position
 * @param body Physics body
 * @param x Position X
 * @param y Position Y
 * @param z Position Z
 */
void physics_body_set_position(PhysicsBody* body, float x, float y, float z);

/**
 * @brief Get body position
 * @param body Physics body
 * @param x Output position X
 * @param y Output position Y
 * @param z Output position Z
 */
void physics_body_get_position(const PhysicsBody* body, float* x, float* y, float* z);

/**
 * @brief Set body rotation
 * @param body Physics body
 * @param x Rotation quaternion X
 * @param y Rotation quaternion Y
 * @param z Rotation quaternion Z
 * @param w Rotation quaternion W
 */
void physics_body_set_rotation(PhysicsBody* body, float x, float y, float z, float w);

/**
 * @brief Get body rotation
 * @param body Physics body
 * @param x Output rotation quaternion X
 * @param y Output rotation quaternion Y
 * @param z Output rotation quaternion Z
 * @param w Output rotation quaternion W
 */
void physics_body_get_rotation(const PhysicsBody* body, float* x, float* y, float* z, float* w);

/**
 * @brief Set body velocity
 * @param body Physics body
 * @param x Velocity X
 * @param y Velocity Y
 * @param z Velocity Z
 */
void physics_body_set_velocity(PhysicsBody* body, float x, float y, float z);

/**
 * @brief Get body velocity
 * @param body Physics body
 * @param x Output velocity X
 * @param y Output velocity Y
 * @param z Output velocity Z
 */
void physics_body_get_velocity(const PhysicsBody* body, float* x, float* y, float* z);

/**
 * @brief Set body angular velocity
 * @param body Physics body
 * @param x Angular velocity X
 * @param y Angular velocity Y
 * @param z Angular velocity Z
 */
void physics_body_set_angular_velocity(PhysicsBody* body, float x, float y, float z);

/**
 * @brief Get body angular velocity
 * @param body Physics body
 * @param x Output angular velocity X
 * @param y Output angular velocity Y
 * @param z Output angular velocity Z
 */
void physics_body_get_angular_velocity(const PhysicsBody* body, float* x, float* y, float* z);

/**
 * @brief Apply force to body
 * @param body Physics body
 * @param fx Force X
 * @param fy Force Y
 * @param fz Force Z
 * @param px Application point X
 * @param py Application point Y
 * @param pz Application point Z
 */
void physics_body_apply_force(PhysicsBody* body, float fx, float fy, float fz, float px, float py, float pz);

/**
 * @brief Apply impulse to body
 * @param body Physics body
 * @param ix Impulse X
 * @param iy Impulse Y
 * @param iz Impulse Z
 * @param px Application point X
 * @param py Application point Y
 * @param pz Application point Z
 */
void physics_body_apply_impulse(PhysicsBody* body, float ix, float iy, float iz, float px, float py, float pz);

/**
 * @brief Apply torque to body
 * @param body Physics body
 * @param tx Torque X
 * @param ty Torque Y
 * @param tz Torque Z
 */
void physics_body_apply_torque(PhysicsBody* body, float tx, float ty, float tz);

/**
 * @brief Set body mass
 * @param body Physics body
 * @param mass Mass value
 */
void physics_body_set_mass(PhysicsBody* body, float mass);

/**
 * @brief Get body mass
 * @param body Physics body
 * @return Mass value
 */
float physics_body_get_mass(const PhysicsBody* body);

/**
 * @brief Set body friction
 * @param body Physics body
 * @param friction Friction coefficient
 */
void physics_body_set_friction(PhysicsBody* body, float friction);

/**
 * @brief Get body friction
 * @param body Physics body
 * @return Friction coefficient
 */
float physics_body_get_friction(const PhysicsBody* body);

/**
 * @brief Set body restitution
 * @param body Physics body
 * @param restitution Restitution coefficient
 */
void physics_body_set_restitution(PhysicsBody* body, float restitution);

/**
 * @brief Get body restitution
 * @param body Physics body
 * @return Restitution coefficient
 */
float physics_body_get_restitution(const PhysicsBody* body);

/**
 * @brief Wake up body
 * @param body Physics body
 */
void physics_body_wake_up(PhysicsBody* body);

/**
 * @brief Put body to sleep
 * @param body Physics body
 */
void physics_body_put_to_sleep(PhysicsBody* body);

/**
 * @brief Check if body is sleeping
 * @param body Physics body
 * @return true if sleeping
 */
bool physics_body_is_sleeping(const PhysicsBody* body);

/**
 * @brief Raycast in physics world
 * @param world Physics world
 * @param sx Start X
 * @param sy Start Y
 * @param sz Start Z
 * @param dx Direction X
 * @param dy Direction Y
 * @param dz Direction Z
 * @param max_distance Maximum distance
 * @param result Output result
 * @return true if hit
 */
bool physics_world_raycast(const PhysicsWorld* world, float sx, float sy, float sz, float dx, float dy, float dz, float max_distance, PhysicsRaycastResult* result);

/**
 * @brief Get body from instance ID
 * @param world Physics world
 * @param instance_id Instance ID
 * @return Physics body or NULL
 */
PhysicsBody* physics_world_get_body_by_instance(PhysicsWorld* world, uint64_t instance_id);

/**
 * @brief Set body user data
 * @param body Physics body
 * @param user_data User data pointer
 */
void physics_body_set_user_data(PhysicsBody* body, void* user_data);

/**
 * @brief Get body user data
 * @param body Physics body
 * @return User data pointer
 */
void* physics_body_get_user_data(const PhysicsBody* body);

#ifdef __cplusplus
}
#endif

#endif // POKO_PHYSICS_H
