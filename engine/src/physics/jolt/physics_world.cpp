/**
 * @file physics_world.cpp
 * @brief Physics world implementation using Jolt Physics for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "physics/jolt/physics_world.h"
#include "core/logging/logger.h"
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
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/StateRecorderImpl.h>
#include <thread>
#include <algorithm>

namespace Poko {
namespace Physics {

// Custom broad phase layer interface
class BPLayerInterfaceImpl : public JPH::BroadPhaseLayerInterface, public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
    BPLayerInterfaceImpl()
    {
        // Map collision layers to broad phase layers
        m_object_to_broad_phase[COLLISION_LAYER_DEFAULT] = 0;
        m_object_to_broad_phase[COLLISION_LAYER_STATIC] = 1;
        m_object_to_broad_phase[COLLISION_LAYER_DYNAMIC] = 2;
        m_object_to_broad_phase[COLLISION_LAYER_TRIGGER] = 3;
    }

    virtual uint32_t GetNumBroadPhaseLayers() const override
    {
        return 4;
    }

    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
    {
        JPH_ASSERT(inLayer < 4);
        return JPH::BroadPhaseLayer(m_object_to_broad_phase[inLayer]);
    }

    virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
    {
        (void)inLayer1;
        (void)inLayer2;
        return true;
    }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
    {
        switch ((JPH::BroadPhaseLayer::Type)inLayer)
        {
        case 0: return "DEFAULT";
        case 1: return "STATIC";
        case 2: return "DYNAMIC";
        case 3: return "TRIGGER";
        default: return "UNKNOWN";
        }
    }
#endif

private:
    uint8_t m_object_to_broad_phase[4];
};

// Object layer filter that determines if two objects can collide
class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
{
public:
    virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
    {
        (void)inObject1;
        (void)inObject2;
        // All layers collide with all layers for now
        return true;
    }
};

PhysicsWorld::PhysicsWorld()
    : m_initialized(false)
    , m_body_id_counter(0)
    , m_accumulated_time(0.0f)
    , m_fixed_time_step(1.0f / 60.0f)
    , m_temp_allocator(nullptr)
    , m_job_system(nullptr)
    , m_physics_system(nullptr)
    , m_broad_phase_layer_interface(nullptr)
{
}

PhysicsWorld::~PhysicsWorld()
{
    if (m_initialized) {
        Shutdown();
    }
}

bool PhysicsWorld::Initialize(const PhysicsConfig& config)
{
    if (m_initialized) {
        LOG_WARNING("PhysicsWorld already initialized");
        return false;
    }

    LOG_INFO("Initializing PhysicsWorld with Jolt Physics...");
    LOG_INFO("Max bodies: " + std::to_string(config.max_bodies));
    LOG_INFO("Gravity: " + std::to_string(config.gravity));

    m_config = config;

    // Register Jolt types
    JPH::Factory::sInstance = new JPH::Factory();
    JPH::RegisterTypes();

    // Create temp allocator
    m_temp_allocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);

    // Create job system
    uint32_t num_threads = config.num_threads;
    if (num_threads == 0) {
        num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) {
            num_threads = 4;
        }
    }
    m_job_system = new JPH::JobSystemThreadPool(num_threads, num_threads, 0);

    // Create broad phase layer interface
    m_broad_phase_layer_interface = new BPLayerInterfaceImpl();

    // Create object layer filter
    ObjectLayerPairFilterImpl* object_layer_filter = new ObjectLayerPairFilterImpl();

    // Create physics system
    m_physics_system = new JPH::PhysicsSystem();
    m_physics_system->Init(
        config.max_bodies,
        0, // Num body mutexes
        config.max_body_pairs,
        config.max_contact_constraints,
        *m_broad_phase_layer_interface,
        *m_broad_phase_layer_interface,
        *object_layer_filter
    );

    // Set gravity
    m_physics_system->SetGravity(JPH::Vec3(0.0f, config.gravity, 0.0f));

    LOG_INFO("PhysicsWorld initialized successfully with Jolt Physics");
    m_initialized = true;
    return true;
}

void PhysicsWorld::Shutdown()
{
    if (!m_initialized) {
        return;
    }

    LOG_INFO("Shutting down PhysicsWorld...");

    std::unique_lock<std::shared_mutex> lock(m_mutex);

    // Remove all bodies
    JPH::BodyInterface& body_interface = m_physics_system->GetBodyInterface();
    for (const auto& body : m_bodies) {
        if (body.jolt_body_id.IsInvalid() == false) {
            body_interface.RemoveBody(body.jolt_body_id);
        }
    }
    m_bodies.clear();
    m_body_id_counter = 0;
    m_accumulated_time = 0.0f;

    // Destroy Jolt components
    delete m_physics_system;
    m_physics_system = nullptr;

    delete m_job_system;
    m_job_system = nullptr;

    delete m_temp_allocator;
    m_temp_allocator = nullptr;

    delete m_broad_phase_layer_interface;
    m_broad_phase_layer_interface = nullptr;
    
    delete m_object_layer_filter;
    m_object_layer_filter = nullptr;

    // Unregister Jolt types
    delete JPH::Factory::sInstance;
    JPH::Factory::sInstance = nullptr;
    JPH::UnregisterTypes();

    m_initialized = false;
    LOG_INFO("PhysicsWorld shutdown complete");
}

void PhysicsWorld::Update(float delta_time)
{
    if (!m_initialized) {
        return;
    }

    std::unique_lock<std::shared_mutex> lock(m_mutex);

    // Accumulate time for fixed timestep
    m_accumulated_time += delta_time;

    // Physics simulation with fixed timestep
    while (m_accumulated_time >= m_fixed_time_step) {
        m_physics_system->Update(m_fixed_time_step, 1, m_temp_allocator, m_job_system);
        m_accumulated_time -= m_fixed_time_step;
    }
}

uint32_t PhysicsWorld::CreateBody(float x, float y, float z, float mass, bool is_static)
{
    return CreateBody(x, y, z, mass, 1.0f, is_static);
}

uint32_t PhysicsWorld::CreateBody(float x, float y, float z, float mass, float radius, bool is_static)
{
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    if (!m_initialized) {
        return 0;
    }

    if (m_bodies.size() >= m_config.max_bodies) {
        LOG_ERROR("PhysicsWorld: Max bodies reached");
        return 0;
    }

    // Create sphere shape
    JPH::SphereShape* shape = new JPH::SphereShape(radius);
    shape->AddRef();

    // Create body settings
    JPH::BodyCreationSettings body_settings(
        shape,
        JPH::RVec3(x, y, z),
        JPH::Quat::sIdentity(),
        is_static ? JPH::EMotionType::Static : JPH::EMotionType::Dynamic,
        is_static ? COLLISION_LAYER_STATIC : COLLISION_LAYER_DYNAMIC
    );

    body_settings.mFriction = 0.5f;
    body_settings.mRestitution = 0.3f;
    body_settings.mLinearDamping = 0.05f;
    body_settings.mAngularDamping = 0.05f;

    if (!is_static && mass > 0.0f) {
        body_settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateMassAndInertia;
        body_settings.mMassPropertiesOverride.mMass = mass;
    }

    // Create body using body interface
    JPH::BodyInterface& body_interface = m_physics_system->GetBodyInterface();
    JPH::Body* body = body_interface.CreateBody(body_settings);
    if (body == nullptr) {
        LOG_ERROR("PhysicsWorld: Failed to create Jolt body");
        shape->Release();
        return 0;
    }

    // Add to physics system
    JPH::BodyID body_id = body->GetID();
    body_interface.AddBody(body_id, JPH::EActivation::Activate);

    // Create our wrapper
    PhysicsBody physics_body;
    physics_body.id = ++m_body_id_counter;
    physics_body.jolt_body_id = body_id;
    physics_body.mass = mass;
    physics_body.radius = radius;
    physics_body.is_static = is_static;
    physics_body.collision_layer = is_static ? COLLISION_LAYER_STATIC : COLLISION_LAYER_DYNAMIC;
    physics_body.collision_mask = COLLISION_LAYER_ALL;

    m_bodies.push_back(physics_body);

    shape->Release();

    return physics_body.id;
}

void PhysicsWorld::RemoveBody(uint32_t body_id)
{
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    if (!m_initialized) {
        return;
    }

    auto it = std::find_if(m_bodies.begin(), m_bodies.end(),
        [body_id](const PhysicsBody& body) { return body.id == body_id; });

    if (it != m_bodies.end()) {
        // Remove from Jolt physics system
        if (it->jolt_body_id.IsInvalid() == false) {
            JPH::BodyInterface& body_interface = m_physics_system->GetBodyInterface();
            body_interface.RemoveBody(it->jolt_body_id);
        }
        m_bodies.erase(it);
    }
}

void PhysicsWorld::SetBodyVelocity(uint32_t body_id, float vx, float vy, float vz)
{
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    if (!m_initialized) {
        return;
    }

    auto it = std::find_if(m_bodies.begin(), m_bodies.end(),
        [body_id](const PhysicsBody& body) { return body.id == body_id; });

    if (it != m_bodies.end() && !it->is_static) {
        JPH::BodyInterface& body_interface = m_physics_system->GetBodyInterface();
        body_interface.SetLinearVelocity(it->jolt_body_id, JPH::Vec3(vx, vy, vz));
    }
}

void PhysicsWorld::GetBodyPosition(uint32_t body_id, float& x, float& y, float& z) const
{
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;

    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    if (!m_initialized) {
        return;
    }

    auto it = std::find_if(m_bodies.begin(), m_bodies.end(),
        [body_id](const PhysicsBody& body) { return body.id == body_id; });

    if (it != m_bodies.end()) {
        JPH::BodyInterface& body_interface = m_physics_system->GetBodyInterface();
        JPH::RVec3 position = body_interface.GetPosition(it->jolt_body_id);
        x = position.GetX();
        y = position.GetY();
        z = position.GetZ();
    }
}

size_t PhysicsWorld::GetBodyCount() const
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    return m_bodies.size();
}

bool PhysicsWorld::CheckCollision(uint32_t body_id1, uint32_t body_id2) const
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    if (!m_initialized) {
        return false;
    }

    const PhysicsBody* body1 = nullptr;
    const PhysicsBody* body2 = nullptr;

    for (const auto& body : m_bodies) {
        if (body.id == body_id1) body1 = &body;
        if (body.id == body_id2) body2 = &body;
    }

    if (!body1 || !body2) {
        return false;
    }

    // Use Jolt's collision detection
    JPH::BodyInterface& body_interface = m_physics_system->GetBodyInterface();
    return body_interface.IsActive(body1->jolt_body_id) && body_interface.IsActive(body2->jolt_body_id);
}

bool PhysicsWorld::IsInitialized() const
{
    return m_initialized;
}

RaycastHit PhysicsWorld::Raycast(float start_x, float start_y, float start_z,
                                  float dir_x, float dir_y, float dir_z,
                                  float max_distance, uint32_t collision_mask) const
{
    RaycastHit result;
    result.hit = false;
    result.body_id = 0;
    result.position_x = 0.0f;
    result.position_y = 0.0f;
    result.position_z = 0.0f;
    result.normal_x = 0.0f;
    result.normal_y = 0.0f;
    result.normal_z = 0.0f;
    result.distance = max_distance;

    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    if (!m_initialized) {
        return result;
    }

    (void)collision_mask; // TODO: Implement collision filtering

    // Normalize direction
    float dir_length = std::sqrt(dir_x * dir_x + dir_y * dir_y + dir_z * dir_z);
    if (dir_length == 0.0f) {
        return result;
    }

    float ndx = dir_x / dir_length;
    float ndy = dir_y / dir_length;
    float ndz = dir_z / dir_length;

    // Use Jolt's body interface for raycasting
    JPH::BodyInterface& body_interface = m_physics_system->GetBodyInterface();
    
    // Cast ray and get closest hit
    JPH::Vec3 start(start_x, start_y, start_z);
    JPH::Vec3 direction(ndx, ndy, ndz);
    
    // Create a simple raycast using body interface
    // For now, we'll use a simplified approach since the full raycast API requires complex setup
    // TODO: Implement proper raycast with JPH::RayCast and collector
    
    // Check each body for ray-sphere intersection as a fallback
    for (const auto& body : m_bodies) {
        if (body.jolt_body_id.IsInvalid()) continue;
        
        JPH::RVec3 body_pos = body_interface.GetPosition(body.jolt_body_id);
        
        // Ray-sphere intersection
        JPH::Vec3 ox(start_x - body_pos.GetX(), start_y - body_pos.GetY(), start_z - body_pos.GetZ());
        
        float a = ndx * ndx + ndy * ndy + ndz * ndz;
        float b = 2.0f * (ox.GetX() * ndx + ox.GetY() * ndy + ox.GetZ() * ndz);
        float c = (ox.GetX() * ox.GetX() + ox.GetY() * ox.GetY() + ox.GetZ() * ox.GetZ()) - (body.radius * body.radius);
        
        float discriminant = b * b - 4.0f * a * c;
        
        if (discriminant >= 0.0f) {
            float sqrt_discriminant = std::sqrt(discriminant);
            float t1 = (-b - sqrt_discriminant) / (2.0f * a);
            float t2 = (-b + sqrt_discriminant) / (2.0f * a);
            
            float t = max_distance;
            if (t1 > 0.0f && t1 < t) t = t1;
            if (t2 > 0.0f && t2 < t) t = t2;
            
            if (t < result.distance) {
                result.hit = true;
                result.body_id = body.id;
                result.distance = t;
                result.position_x = start_x + ndx * t;
                result.position_y = start_y + ndy * t;
                result.position_z = start_z + ndz * t;
                
                // Calculate normal (from sphere center to hit point)
                JPH::Vec3 hit_ox(result.position_x - body_pos.GetX(), result.position_y - body_pos.GetY(), result.position_z - body_pos.GetZ());
                float hit_dist = std::sqrt(hit_ox.GetX() * hit_ox.GetX() + hit_ox.GetY() * hit_ox.GetY() + hit_ox.GetZ() * hit_ox.GetZ());
                
                if (hit_dist > 0.0f) {
                    result.normal_x = hit_ox.GetX() / hit_dist;
                    result.normal_y = hit_ox.GetY() / hit_dist;
                    result.normal_z = hit_ox.GetZ() / hit_dist;
                }
            }
        }
    }

    return result;
}

void PhysicsWorld::SetBodyCollisionLayer(uint32_t body_id, uint32_t layer, uint32_t mask)
{
    std::unique_lock<std::shared_mutex> lock(m_mutex);
    
    if (!m_initialized) {
        return;
    }

    auto it = std::find_if(m_bodies.begin(), m_bodies.end(),
        [body_id](const PhysicsBody& body) { return body.id == body_id; });

    if (it != m_bodies.end()) {
        it->collision_layer = static_cast<uint8_t>(layer);
        it->collision_mask = static_cast<uint8_t>(mask);
        
        // Update Jolt body layer
        JPH::BodyInterface& body_interface = m_physics_system->GetBodyInterface();
        body_interface.SetObjectLayer(it->jolt_body_id, static_cast<JPH::ObjectLayer>(layer));
    }
}

uint32_t PhysicsWorld::GetBodyCollisionLayer(uint32_t body_id) const
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    if (!m_initialized) {
        return 0;
    }

    auto it = std::find_if(m_bodies.begin(), m_bodies.end(),
        [body_id](const PhysicsBody& body) { return body.id == body_id; });

    if (it != m_bodies.end()) {
        return it->collision_layer;
    }

    return 0;
}

uint32_t PhysicsWorld::GetBodyCollisionMask(uint32_t body_id) const
{
    std::shared_lock<std::shared_mutex> lock(m_mutex);
    
    if (!m_initialized) {
        return 0;
    }

    auto it = std::find_if(m_bodies.begin(), m_bodies.end(),
        [body_id](const PhysicsBody& body) { return body.id == body_id; });

    if (it != m_bodies.end()) {
        return it->collision_mask;
    }

    return 0;
}

} // namespace Physics
} // namespace Poko
