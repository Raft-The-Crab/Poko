/**
 * @file physics.cpp
 * @brief Physics system implementation for Poko Engine
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "physics/physics.h"
#include <cstring>
#include <cmath>
#include <map>
#include <mutex>
#include <vector>

namespace poko {
namespace physics {

// Internal physics body structure
struct PhysicsBodyImpl {
    uint64_t id;
    PhysicsBodyType type;
    PhysicsShapeType shape_type;
    PhysicsMaterial material;
    
    float position[3];
    float rotation[4];
    float velocity[3];
    float angular_velocity[3];
    
    float mass;
    float friction;
    float restitution;
    
    bool is_sleeping;
    void* user_data;
    
    PhysicsBodyImpl* next;
};

// Internal physics world structure
struct PhysicsWorldImpl {
    float gravity[3];
    PhysicsBodyImpl* bodies;
    std::map<uint64_t, PhysicsBodyImpl*> body_map;
    std::mutex mutex;
    uint64_t next_body_id;
};

// Convert to internal implementation
static PhysicsBodyImpl* to_impl(PhysicsBody* body) {
    return reinterpret_cast<PhysicsBodyImpl*>(body);
}

// Convert to internal implementation (const version)
static const PhysicsBodyImpl* to_impl(const PhysicsBody* body) {
    return reinterpret_cast<const PhysicsBodyImpl*>(body);
}

// Convert from internal implementation
static PhysicsBody* from_impl(PhysicsBodyImpl* impl) {
    return reinterpret_cast<PhysicsBody*>(impl);
}

} // namespace physics
} // namespace poko

// C API implementation
extern "C" {

using namespace poko::physics;

PhysicsWorld* physics_world_create(void) {
    PhysicsWorldImpl* world = new PhysicsWorldImpl();
    
    world->gravity[0] = 0.0f;
    world->gravity[1] = -9.81f;
    world->gravity[2] = 0.0f;
    world->bodies = NULL;
    world->next_body_id = 1;
    
    return reinterpret_cast<PhysicsWorld*>(world);
}

void physics_world_destroy(PhysicsWorld* world) {
    if (!world) return;
    
    PhysicsWorldImpl* impl = reinterpret_cast<PhysicsWorldImpl*>(world);
    
    // Free all bodies
    PhysicsBodyImpl* body = impl->bodies;
    while (body) {
        PhysicsBodyImpl* next = body->next;
        delete body;
        body = next;
    }
    
    impl->body_map.clear();
    delete impl;
}

void physics_world_step(PhysicsWorld* world, float delta_time) {
    if (!world || delta_time <= 0.0f) return;
    
    PhysicsWorldImpl* impl = reinterpret_cast<PhysicsWorldImpl*>(world);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    // Integrate bodies
    PhysicsBodyImpl* body = impl->bodies;
    while (body) {
        if (body->type == PHYSICS_BODY_DYNAMIC && !body->is_sleeping) {
            // Apply gravity
            body->velocity[0] += impl->gravity[0] * delta_time;
            body->velocity[1] += impl->gravity[1] * delta_time;
            body->velocity[2] += impl->gravity[2] * delta_time;
            
            // Update position
            body->position[0] += body->velocity[0] * delta_time;
            body->position[1] += body->velocity[1] * delta_time;
            body->position[2] += body->velocity[2] * delta_time;
            
            // Update rotation
            // Simplified rotation update
            float angular_speed = sqrtf(body->angular_velocity[0] * body->angular_velocity[0] +
                                       body->angular_velocity[1] * body->angular_velocity[1] +
                                       body->angular_velocity[2] * body->angular_velocity[2]);
            if (angular_speed > 0.0f) {
                // Simplified quaternion integration
                float half_angle = angular_speed * delta_time * 0.5f;
                float sin_half = sinf(half_angle);
                float cos_half = cosf(half_angle);
                
                float axis_x = body->angular_velocity[0] / angular_speed;
                float axis_y = body->angular_velocity[1] / angular_speed;
                float axis_z = body->angular_velocity[2] / angular_speed;
                
                float qx = axis_x * sin_half;
                float qy = axis_y * sin_half;
                float qz = axis_z * sin_half;
                float qw = cos_half;
                
                // Quaternion multiplication
                float old_x = body->rotation[0];
                float old_y = body->rotation[1];
                float old_z = body->rotation[2];
                float old_w = body->rotation[3];
                
                body->rotation[0] = qw * old_x + qx * old_w + qy * old_z - qz * old_y;
                body->rotation[1] = qw * old_y - qx * old_z + qy * old_w + qz * old_x;
                body->rotation[2] = qw * old_z + qx * old_y - qy * old_x + qz * old_w;
                body->rotation[3] = qw * old_w - qx * old_x - qy * old_y - qz * old_z;
                
                // Normalize quaternion
                float len = sqrtf(body->rotation[0] * body->rotation[0] +
                                 body->rotation[1] * body->rotation[1] +
                                 body->rotation[2] * body->rotation[2] +
                                 body->rotation[3] * body->rotation[3]);
                if (len > 0.0f) {
                    body->rotation[0] /= len;
                    body->rotation[1] /= len;
                    body->rotation[2] /= len;
                    body->rotation[3] /= len;
                }
            }
        }
        
        body = body->next;
    }
    
    // Collision detection and resolution would go here
    // This is a simplified implementation
}

void physics_world_set_gravity(PhysicsWorld* world, float x, float y, float z) {
    if (!world) return;
    
    PhysicsWorldImpl* impl = reinterpret_cast<PhysicsWorldImpl*>(world);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    impl->gravity[0] = x;
    impl->gravity[1] = y;
    impl->gravity[2] = z;
}

void physics_world_get_gravity(const PhysicsWorld* world, float* x, float* y, float* z) {
    if (!world) return;
    
    const PhysicsWorldImpl* impl = reinterpret_cast<const PhysicsWorldImpl*>(world);
    
    if (x) *x = impl->gravity[0];
    if (y) *y = impl->gravity[1];
    if (z) *z = impl->gravity[2];
}

PhysicsBody* physics_body_create(PhysicsWorld* world, PhysicsBodyType type, PhysicsShapeType shape_type, const PhysicsMaterial* material) {
    if (!world) return NULL;
    
    PhysicsWorldImpl* impl = reinterpret_cast<PhysicsWorldImpl*>(world);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    PhysicsBodyImpl* body = new PhysicsBodyImpl();
    body->id = impl->next_body_id++;
    body->type = type;
    body->shape_type = shape_type;
    
    if (material) {
        body->material = *material;
    } else {
        body->material.friction = 0.5f;
        body->material.restitution = 0.0f;
        body->material.density = 1.0f;
    }
    
    memset(body->position, 0, sizeof(body->position));
    body->rotation[0] = 0.0f;
    body->rotation[1] = 0.0f;
    body->rotation[2] = 0.0f;
    body->rotation[3] = 1.0f;
    memset(body->velocity, 0, sizeof(body->velocity));
    memset(body->angular_velocity, 0, sizeof(body->angular_velocity));
    
    body->mass = body->material.density;
    body->friction = body->material.friction;
    body->restitution = body->material.restitution;
    
    body->is_sleeping = false;
    body->user_data = NULL;
    
    // Add to linked list
    body->next = impl->bodies;
    impl->bodies = body;
    
    // Add to map
    impl->body_map[body->id] = body;
    
    return from_impl(body);
}

void physics_body_destroy(PhysicsBody* body) {
    if (!body) return;
    
    PhysicsBodyImpl* impl = to_impl(body);
    delete impl;
}

void physics_body_set_position(PhysicsBody* body, float x, float y, float z) {
    if (!body) return;
    
    PhysicsBodyImpl* impl = to_impl(body);
    impl->position[0] = x;
    impl->position[1] = y;
    impl->position[2] = z;
}

void physics_body_get_position(const PhysicsBody* body, float* x, float* y, float* z) {
    if (!body) return;
    
    const PhysicsBodyImpl* impl = to_impl(body);
    if (x) *x = impl->position[0];
    if (y) *y = impl->position[1];
    if (z) *z = impl->position[2];
}

void physics_body_set_rotation(PhysicsBody* body, float x, float y, float z, float w) {
    if (!body) return;
    
    PhysicsBodyImpl* impl = to_impl(body);
    impl->rotation[0] = x;
    impl->rotation[1] = y;
    impl->rotation[2] = z;
    impl->rotation[3] = w;
}

void physics_body_get_rotation(const PhysicsBody* body, float* x, float* y, float* z, float* w) {
    if (!body) return;
    
    const PhysicsBodyImpl* impl = to_impl(body);
    if (x) *x = impl->rotation[0];
    if (y) *y = impl->rotation[1];
    if (z) *z = impl->rotation[2];
    if (w) *w = impl->rotation[3];
}

void physics_body_set_velocity(PhysicsBody* body, float x, float y, float z) {
    if (!body) return;
    
    PhysicsBodyImpl* impl = to_impl(body);
    impl->velocity[0] = x;
    impl->velocity[1] = y;
    impl->velocity[2] = z;
}

void physics_body_get_velocity(const PhysicsBody* body, float* x, float* y, float* z) {
    if (!body) return;
    
    const PhysicsBodyImpl* impl = to_impl(body);
    if (x) *x = impl->velocity[0];
    if (y) *y = impl->velocity[1];
    if (z) *z = impl->velocity[2];
}

void physics_body_set_angular_velocity(PhysicsBody* body, float x, float y, float z) {
    if (!body) return;
    
    PhysicsBodyImpl* impl = to_impl(body);
    impl->angular_velocity[0] = x;
    impl->angular_velocity[1] = y;
    impl->angular_velocity[2] = z;
}

void physics_body_get_angular_velocity(const PhysicsBody* body, float* x, float* y, float* z) {
    if (!body) return;
    
    const PhysicsBodyImpl* impl = to_impl(body);
    if (x) *x = impl->angular_velocity[0];
    if (y) *y = impl->angular_velocity[1];
    if (z) *z = impl->angular_velocity[2];
}

void physics_body_apply_force(PhysicsBody* body, float fx, float fy, float fz, float px, float py, float pz) {
    if (!body) return;
    
    PhysicsBodyImpl* impl = to_impl(body);
    // F = ma -> a = F/m
    float ax = fx / impl->mass;
    float ay = fy / impl->mass;
    float az = fz / impl->mass;
    
    impl->velocity[0] += ax;
    impl->velocity[1] += ay;
    impl->velocity[2] += az;
    
    // Torque = r x F (simplified)
    // Apply angular velocity change based on torque
    (void)px;
    (void)py;
    (void)pz;
}

void physics_body_apply_impulse(PhysicsBody* body, float ix, float iy, float iz, float px, float py, float pz) {
    if (!body) return;
    
    PhysicsBodyImpl* impl = to_impl(body);
    // Impulse = change in momentum = m * delta_v
    float dvx = ix / impl->mass;
    float dvy = iy / impl->mass;
    float dvz = iz / impl->mass;
    
    impl->velocity[0] += dvx;
    impl->velocity[1] += dvy;
    impl->velocity[2] += dvz;
    
    (void)px;
    (void)py;
    (void)pz;
}

void physics_body_apply_torque(PhysicsBody* body, float tx, float ty, float tz) {
    if (!body) return;
    
    PhysicsBodyImpl* impl = to_impl(body);
    // Torque = I * alpha (simplified: assume unit moment of inertia)
    impl->angular_velocity[0] += tx;
    impl->angular_velocity[1] += ty;
    impl->angular_velocity[2] += tz;
}

void physics_body_set_mass(PhysicsBody* body, float mass) {
    if (!body || mass <= 0.0f) return;
    
    PhysicsBodyImpl* impl = to_impl(body);
    impl->mass = mass;
}

float physics_body_get_mass(const PhysicsBody* body) {
    if (!body) return 0.0f;
    
    const PhysicsBodyImpl* impl = to_impl(body);
    return impl->mass;
}

void physics_body_set_friction(PhysicsBody* body, float friction) {
    if (!body) return;
    
    PhysicsBodyImpl* impl = to_impl(body);
    impl->friction = friction;
}

float physics_body_get_friction(const PhysicsBody* body) {
    if (!body) return 0.0f;
    
    const PhysicsBodyImpl* impl = to_impl(body);
    return impl->friction;
}

void physics_body_set_restitution(PhysicsBody* body, float restitution) {
    if (!body) return;
    
    PhysicsBodyImpl* impl = to_impl(body);
    impl->restitution = restitution;
}

float physics_body_get_restitution(const PhysicsBody* body) {
    if (!body) return 0.0f;
    
    const PhysicsBodyImpl* impl = to_impl(body);
    return impl->restitution;
}

void physics_body_wake_up(PhysicsBody* body) {
    if (!body) return;
    
    PhysicsBodyImpl* impl = to_impl(body);
    impl->is_sleeping = false;
}

void physics_body_put_to_sleep(PhysicsBody* body) {
    if (!body) return;
    
    PhysicsBodyImpl* impl = to_impl(body);
    impl->is_sleeping = true;
    memset(impl->velocity, 0, sizeof(impl->velocity));
    memset(impl->angular_velocity, 0, sizeof(impl->angular_velocity));
}

bool physics_body_is_sleeping(const PhysicsBody* body) {
    if (!body) return false;
    
    const PhysicsBodyImpl* impl = to_impl(body);
    return impl->is_sleeping;
}

bool physics_world_raycast(const PhysicsWorld* world, float sx, float sy, float sz, float dx, float dy, float dz, float max_distance, PhysicsRaycastResult* result) {
    if (!world || !result) return false;
    
    const PhysicsWorldImpl* impl = reinterpret_cast<const PhysicsWorldImpl*>(world);
    
    // Normalize direction
    float len = sqrtf(dx * dx + dy * dy + dz * dz);
    if (len == 0.0f) return false;
    
    float dir_x = dx / len;
    float dir_y = dy / len;
    float dir_z = dz / len;
    
    // Simple raycast implementation
    // In a real implementation, this would use proper collision detection
    float closest_hit = max_distance;
    bool hit_found = false;
    
    PhysicsBodyImpl* body = impl->bodies;
    while (body) {
        // Simple sphere collision check
        float to_body_x = body->position[0] - sx;
        float to_body_y = body->position[1] - sy;
        float to_body_z = body->position[2] - sz;
        
        float projection = to_body_x * dir_x + to_body_y * dir_y + to_body_z * dir_z;
        
        if (projection > 0.0f && projection < closest_hit) {
            float closest_x = sx + dir_x * projection;
            float closest_y = sy + dir_y * projection;
            float closest_z = sz + dir_z * projection;
            
            float dist_x = closest_x - body->position[0];
            float dist_y = closest_y - body->position[1];
            float dist_z = closest_z - body->position[2];
            float dist = sqrtf(dist_x * dist_x + dist_y * dist_y + dist_z * dist_z);
            
            // Assume sphere radius of 1.0 for simplicity
            if (dist < 1.0f) {
                closest_hit = projection;
                hit_found = true;
                
                result->hit = true;
                result->hit_fraction = projection / max_distance;
                result->hit_position[0] = closest_x;
                result->hit_position[1] = closest_y;
                result->hit_position[2] = closest_z;
                result->hit_normal[0] = dist_x / dist;
                result->hit_normal[1] = dist_y / dist;
                result->hit_normal[2] = dist_z / dist;
                result->hit_body = from_impl(body);
            }
        }
        
        body = body->next;
    }
    
    if (!hit_found) {
        result->hit = false;
    }
    
    return hit_found;
}

PhysicsBody* physics_world_get_body_by_instance(PhysicsWorld* world, uint64_t instance_id) {
    if (!world) return NULL;
    
    PhysicsWorldImpl* impl = reinterpret_cast<PhysicsWorldImpl*>(world);
    std::lock_guard<std::mutex> lock(impl->mutex);
    
    auto it = impl->body_map.find(instance_id);
    if (it != impl->body_map.end()) {
        return from_impl(it->second);
    }
    
    return NULL;
}

void physics_body_set_user_data(PhysicsBody* body, void* user_data) {
    if (!body) return;
    
    PhysicsBodyImpl* impl = to_impl(body);
    impl->user_data = user_data;
}

void* physics_body_get_user_data(const PhysicsBody* body) {
    if (!body) return NULL;
    
    const PhysicsBodyImpl* impl = to_impl(body);
    return impl->user_data;
}

} // extern "C"
