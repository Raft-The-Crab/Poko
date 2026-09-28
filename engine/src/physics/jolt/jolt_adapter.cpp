/**
 * @file jolt_adapter.cpp
 * @brief Jolt Physics adapter for Poko Engine
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "physics/physics.h"
#include "core/logging/logger.h"
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <unordered_map>
#include <mutex>

JPH_SUPPRESS_WARNINGS

using namespace JPH;
using namespace JPH::literals;

namespace poko {
namespace physics {

// Jolt-specific layers
namespace Layers {
    static constexpr ObjectLayer NON_MOVING = 0;
    static constexpr ObjectLayer MOVING = 1;
    static constexpr ObjectLayer NUM_LAYERS = 2;
}

// Broadphase layer interface
class BPLayerInterfaceImpl : public BroadPhaseLayerInterface {
public:
    virtual uint GetNumBroadPhaseLayers() const override {
        return 2;
    }

    virtual BroadPhaseLayer GetBroadPhaseLayer(ObjectLayer inLayer) const override {
        JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
        if (inLayer == Layers::NON_MOVING) return BroadPhaseLayer(0);
        else return BroadPhaseLayer(1);
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char* GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const override {
        return (inLayer == BroadPhaseLayer(0)) ? "NON_MOVING" : "MOVING";
    }
#endif
};

// Object layer interface
class ObjectLayerPairFilterImpl : public ObjectLayerPairFilter {
public:
    virtual bool ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const override {
        switch (inObject1) {
        case Layers::NON_MOVING:
            return inObject2 == Layers::MOVING;
        case Layers::MOVING:
            return true;
        default:
            JPH_ASSERT(false);
            return false;
        }
    }
};

// Broadphase layer filter
class BroadPhaseLayerPairFilterImpl : public BroadPhaseLayerPairFilter {
public:
    virtual bool ShouldCollide(BroadPhaseLayer inLayer1, BroadPhaseLayer inLayer2) const override {
        switch (inLayer1) {
        case BroadPhaseLayer(0):
            return inLayer2 == BroadPhaseLayer(1);
        case BroadPhaseLayer(1):
            return true;
        default:
            JPH_ASSERT(false);
            return false;
        }
    }
};

// Body activation listener
class BodyActivationListenerImpl : public BodyActivationListener {
public:
    virtual void OnBodyActivated(const BodyID& inBodyID, uint64 inBodyUserData) override {
        POKO_LOG_DEBUG("Jolt: Body activated " + std::to_string(inBodyID.GetIndexAndSequenceNumber()));
    }

    virtual void OnBodyDeactivated(const BodyID& inBodyID, uint64 inBodyUserData) override {
        POKO_LOG_DEBUG("Jolt: Body deactivated " + std::to_string(inBodyID.GetIndexAndSequenceNumber()));
    }
};

// Jolt world implementation
struct JoltWorldImpl {
    PhysicsSystem* physics_system;
    TempAllocatorImpl* temp_allocator;
    JobSystemThreadPool* job_system;
    BPLayerInterfaceImpl* bp_layer_interface;
    ObjectLayerPairFilterImpl* object_layer_filter;
    BroadPhaseLayerPairFilterImpl* bp_layer_filter;
    BodyActivationListenerImpl* activation_listener;
    std::unordered_map<uint64_t, BodyID> body_map;
    std::mutex mutex;
    uint64_t next_body_id;
    bool is_initialized;
};

// Convert Poko body type to Jolt motion type
static EMotionType to_jolt_motion_type(PhysicsBodyType type) {
    switch (type) {
    case PHYSICS_BODY_STATIC:
        return EMotionType::Static;
    case PHYSICS_BODY_KINEMATIC:
        return EMotionType::Kinematic;
    case PHYSICS_BODY_DYNAMIC:
        return EMotionType::Dynamic;
    default:
        return EMotionType::Static;
    }
}

// Convert Poko shape type to Jolt shape
static ShapeRefC create_jolt_shape(PhysicsShapeType shape_type, float size_x, float size_y, float size_z) {
    switch (shape_type) {
    case PHYSICS_SHAPE_BOX:
        return new BoxShape(Vec3(size_x * 0.5f, size_y * 0.5f, size_z * 0.5f));
    case PHYSICS_SHAPE_SPHERE:
        return new SphereShape(size_x * 0.5f);
    case PHYSICS_SHAPE_CAPSULE:
        return new CapsuleShape(size_y * 0.5f, size_x * 0.5f);
    default:
        POKO_LOG_WARN("Jolt: Unknown shape type, defaulting to box");
        return new BoxShape(Vec3(size_x * 0.5f, size_y * 0.5f, size_z * 0.5f));
    }
}

} // namespace physics
} // namespace poko

// C API implementation using Jolt
extern "C" {

using namespace poko::physics;

PhysicsWorld* physics_world_create(void) {
    JoltWorldImpl* world = new JoltWorldImpl();
    world->physics_system = nullptr;
    world->temp_allocator = nullptr;
    world->job_system = nullptr;
    world->bp_layer_interface = nullptr;
    world->object_layer_filter = nullptr;
    world->bp_layer_filter = nullptr;
    world->activation_listener = nullptr;
    world->next_body_id = 1;
    world->is_initialized = false;

    // Register Jolt types
    RegisterTypes();

    // Create allocator
    world->temp_allocator = new TempAllocatorImpl(10 * 1024 * 1024);

    // Create job system
    world->job_system = new JobSystemThreadPool(cMaxPhysicsJobs, cMaxPhysicsBarriers, 4);

    // Create physics system
    world->physics_system = new PhysicsSystem();
    world->physics_system->Init(1024, 0, 65536, 10240, *world->temp_allocator, *world->job_system);

    // Create layer interfaces
    world->bp_layer_interface = new BPLayerInterfaceImpl();
    world->object_layer_filter = new ObjectLayerPairFilterImpl();
    world->bp_layer_filter = new BroadPhaseLayerPairFilterImpl();
    world->activation_listener = new BodyActivationListenerImpl();

    // Set up layers
    world->physics_system->SetBroadPhaseLayerInterface(world->bp_layer_interface);
    world->physics_system->SetObjectLayerPairFilter(world->object_layer_filter);
    world->physics_system->SetBroadPhaseLayerPairFilter(world->bp_layer_filter);
    world->physics_system->SetBodyActivationListener(world->activation_listener);

    // Set gravity
    world->physics_system->SetGravity(Vec3(0.0f, -9.81f, 0.0f));

    world->is_initialized = true;

    POKO_LOG_INFO("Jolt: Physics world created successfully");
    return reinterpret_cast<PhysicsWorld*>(world);
}

void physics_world_destroy(PhysicsWorld* world) {
    if (!world) return;

    JoltWorldImpl* impl = reinterpret_cast<JoltWorldImpl*>(world);

    std::lock_guard<std::mutex> lock(impl->mutex);

    delete impl->activation_listener;
    delete impl->bp_layer_filter;
    delete impl->object_layer_filter;
    delete impl->bp_layer_interface;
    delete impl->physics_system;
    delete impl->job_system;
    delete impl->temp_allocator;

    impl->body_map.clear();

    delete impl;

    POKO_LOG_INFO("Jolt: Physics world destroyed");
}

PhysicsBody* physics_body_create(PhysicsWorld* world, PhysicsBodyType type, PhysicsShapeType shape_type,
                                   const PhysicsMaterial* material) {
    if (!world) return nullptr;

    JoltWorldImpl* impl = reinterpret_cast<JoltWorldImpl*>(world);
    if (!impl->is_initialized) return nullptr;

    std::lock_guard<std::mutex> lock(impl->mutex);

    // Create shape with default size
    JPH::ShapeRefC shape = create_jolt_shape(shape_type, 1.0f, 1.0f, 1.0f);

    // Create body settings
    BodyCreationSettings body_settings(shape, RVec3(0, 0, 0), Quat::sIdentity(), to_jolt_motion_type(type), Layers::MOVING);

    // Set mass for dynamic bodies
    if (type == PHYSICS_BODY_DYNAMIC) {
        body_settings.mOverrideMassProperties = EOverrideMassProperties::CalculateMassAndInertia;
        body_settings.mMassPropertiesOverride.mMass = 1.0f;
    }

    // Create body
    BodyInterface& body_interface = impl->physics_system->GetBodyInterface();
    Body* body = body_interface.CreateBody(body_settings);

    if (!body) {
        POKO_LOG_ERROR("Jolt: Failed to create body");
        return nullptr;
    }

    // Add to physics system
    body_interface.AddBody(body->GetID(), EActivation::Activate);

    // Map body ID
    uint64_t body_id = impl->next_body_id++;
    impl->body_map[body_id] = body->GetID();

    POKO_LOG_INFO("Jolt: Created body with ID " + std::to_string(body_id));
    return reinterpret_cast<PhysicsBody*>(body_id);
}

void physics_body_destroy(PhysicsBody* body) {
    // For simplicity, we don't implement per-body destruction with Jolt
    // Bodies are destroyed when the world is destroyed
    (void)body;
}

void physics_body_set_position(PhysicsBody* body, float x, float y, float z) {
    // For simplicity, placeholder implementation
    (void)body; (void)x; (void)y; (void)z;
}

void physics_body_get_position(const PhysicsBody* body, float* x, float* y, float* z) {
    // For simplicity, placeholder implementation
    (void)body; (void)x; (void)y; (void)z;
}

void physics_body_set_velocity(PhysicsBody* body, float x, float y, float z) {
    // For simplicity, placeholder implementation
    (void)body; (void)x; (void)y; (void)z;
}

void physics_body_get_velocity(const PhysicsBody* body, float* x, float* y, float* z) {
    // For simplicity, placeholder implementation
    (void)body; (void)x; (void)y; (void)z;
}

void physics_body_apply_force(PhysicsBody* body, float fx, float fy, float fz, float px, float py, float pz) {
    // For simplicity, placeholder implementation
    (void)body; (void)fx; (void)fy; (void)fz; (void)px; (void)py; (void)pz;
}

void physics_world_update(PhysicsWorld* world, float delta_time) {
    if (!world) return;

    JoltWorldImpl* impl = reinterpret_cast<JoltWorldImpl*>(world);
    if (!impl->is_initialized) return;

    std::lock_guard<std::mutex> lock(impl->mutex);

    impl->physics_system->Update(delta_time, 1, impl->temp_allocator, impl->job_system);
}

void physics_body_set_rotation(PhysicsBody* body, float x, float y, float z, float w) {
    (void)body; (void)x; (void)y; (void)z; (void)w;
}

void physics_body_get_rotation(const PhysicsBody* body, float* x, float* y, float* z, float* w) {
    if (!body || !x || !y || !z || !w) return;
    *x = 0.0f; *y = 0.0f; *z = 0.0f; *w = 1.0f;
}

void physics_body_set_angular_velocity(PhysicsBody* body, float x, float y, float z) {
    (void)body; (void)x; (void)y; (void)z;
}

void physics_body_get_angular_velocity(const PhysicsBody* body, float* x, float* y, float* z) {
    if (!body || !x || !y || !z) return;
    *x = 0.0f; *y = 0.0f; *z = 0.0f;
}

void physics_body_apply_force(PhysicsBody* body, float fx, float fy, float fz, float px, float py, float pz) {
    (void)body; (void)fx; (void)fy; (void)fz; (void)px; (void)py; (void)pz;
}

void physics_body_apply_torque(PhysicsBody* body, float tx, float ty, float tz) {
    (void)body; (void)tx; (void)ty; (void)tz;
}

void physics_body_set_mass(PhysicsBody* body, float mass) {
    (void)body; (void)mass;
}

float physics_body_get_mass(const PhysicsBody* body) {
    if (!body) return 1.0f;
    return 1.0f;
}

void physics_body_set_friction(PhysicsBody* body, float friction) {
    (void)body; (void)friction;
}

float physics_body_get_friction(const PhysicsBody* body) {
    if (!body) return 0.5f;
    return 0.5f;
}

void physics_body_set_restitution(PhysicsBody* body, float restitution) {
    (void)body; (void)restitution;
}

float physics_body_get_restitution(const PhysicsBody* body) {
    if (!body) return 0.0f;
    return 0.0f;
}

void physics_body_set_linear_damping(PhysicsBody* body, float damping) {
    (void)body; (void)damping;
}

float physics_body_get_linear_damping(const PhysicsBody* body) {
    if (!body) return 0.0f;
    return 0.0f;
}

void physics_body_set_angular_damping(PhysicsBody* body, float damping) {
    (void)body; (void)damping;
}

float physics_body_get_angular_damping(const PhysicsBody* body) {
    if (!body) return 0.0f;
    return 0.0f;
}

void physics_body_set_gravity_scale(PhysicsBody* body, float scale) {
    (void)body; (void)scale;
}

float physics_body_get_gravity_scale(const PhysicsBody* body) {
    if (!body) return 1.0f;
    return 1.0f;
}

void physics_body_wake_up(PhysicsBody* body) {
    (void)body;
}

void physics_body_put_to_sleep(PhysicsBody* body) {
    (void)body;
}

bool physics_body_is_awake(const PhysicsBody* body) {
    if (!body) return false;
    return true;
}

void physics_body_set_collision_group(PhysicsBody* body, uint32_t group) {
    (void)body; (void)group;
}

uint32_t physics_body_get_collision_group(const PhysicsBody* body) {
    if (!body) return 0;
    return 0;
}

void physics_body_set_collision_mask(PhysicsBody* body, uint32_t mask) {
    (void)body; (void)mask;
}

uint32_t physics_body_get_collision_mask(const PhysicsBody* body) {
    if (!body) return 0xFFFFFFFF;
    return 0xFFFFFFFF;
}

} // extern "C"