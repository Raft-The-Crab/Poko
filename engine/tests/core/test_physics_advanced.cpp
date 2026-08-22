/**
 * @file test_physics_advanced.cpp
 * @brief Tests for advanced physics features (raycasting, collision filtering)
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "physics/jolt/physics_world.h"
#include <iostream>
#include <cassert>
#include <cmath>

namespace Poko {
namespace Physics {

void test_collision_layer_setup()
{
    std::cout << "Testing collision layer setup..." << std::endl;
    
    PhysicsWorld world;
    PhysicsConfig config;
    config.max_bodies = 100;
    
    assert(world.Initialize(config) && "World should initialize");
    
    // Create a body and check default layer
    uint32_t body1 = world.CreateBody(0.0f, 5.0f, 0.0f, 1.0f, 1.0f, false);
    assert(body1 != 0 && "Body should be created");
    
    uint32_t layer = world.GetBodyCollisionLayer(body1);
    uint32_t mask = world.GetBodyCollisionMask(body1);
    
    assert(layer == COLLISION_LAYER_DYNAMIC && "Dynamic body should have default dynamic layer");
    assert(mask == COLLISION_LAYER_ALL && "Body should collide with all layers by default");
    
    // Create a static body
    uint32_t body2 = world.CreateBody(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, true);
    assert(body2 != 0 && "Static body should be created");
    
    layer = world.GetBodyCollisionLayer(body2);
    assert(layer == COLLISION_LAYER_STATIC && "Static body should have static layer");
    
    world.Shutdown();
    std::cout << "✓ Collision layer setup test passed" << std::endl;
}

void test_collision_filtering()
{
    std::cout << "Testing collision filtering..." << std::endl;
    
    PhysicsWorld world;
    PhysicsConfig config;
    config.max_bodies = 100;
    config.gravity = 0.0f; // Disable gravity for this test
    
    assert(world.Initialize(config) && "World should initialize");
    
    // Create two bodies on different layers that shouldn't collide
    uint32_t body1 = world.CreateBody(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, false);
    uint32_t body2 = world.CreateBody(1.5f, 0.0f, 0.0f, 1.0f, 1.0f, false);
    
    // Set body1 to only collide with LAYER_DEFAULT
    world.SetBodyCollisionLayer(body1, COLLISION_LAYER_DEFAULT, COLLISION_LAYER_DEFAULT);
    
    // Set body2 to LAYER_TRIGGER only
    world.SetBodyCollisionLayer(body2, COLLISION_LAYER_TRIGGER, COLLISION_LAYER_ALL);
    
    // They should not collide due to layer mismatch
    bool collides = world.CheckCollision(body1, body2);
    assert(!collides && "Bodies on non-overlapping layers should not collide");
    
    // Now make them compatible
    world.SetBodyCollisionLayer(body2, COLLISION_LAYER_DEFAULT, COLLISION_LAYER_DEFAULT);
    
    // They should now collide
    collides = world.CheckCollision(body1, body2);
    assert(collides && "Bodies on compatible layers should collide");
    
    world.Shutdown();
    std::cout << "✓ Collision filtering test passed" << std::endl;
}

void test_raycast_hit()
{
    std::cout << "Testing raycast hit..." << std::endl;
    
    PhysicsWorld world;
    PhysicsConfig config;
    config.max_bodies = 100;
    config.gravity = 0.0f;
    
    assert(world.Initialize(config) && "World should initialize");
    
    // Create a sphere at (0, 0, 0) with radius 1.0
    uint32_t body = world.CreateBody(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, true);
    assert(body != 0 && "Body should be created");
    
    // Cast a ray from (-5, 0, 0) in +X direction
    RaycastHit hit = world.Raycast(-5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f);
    
    assert(hit.hit && "Ray should hit the sphere");
    assert(hit.body_id == body && "Hit should be on the correct body");
    assert(std::abs(hit.distance - 4.0f) < 0.01f && "Hit distance should be ~4.0");
    assert(std::abs(hit.position_x - (-1.0f)) < 0.01f && "Hit position should be at sphere surface");
    assert(std::abs(hit.normal_x - (-1.0f)) < 0.01f && "Normal should point back toward ray origin");
    
    world.Shutdown();
    std::cout << "✓ Raycast hit test passed" << std::endl;
}

void test_raycast_miss()
{
    std::cout << "Testing raycast miss..." << std::endl;
    
    PhysicsWorld world;
    PhysicsConfig config;
    config.max_bodies = 100;
    config.gravity = 0.0f;
    
    assert(world.Initialize(config) && "World should initialize");
    
    // Create a sphere at (0, 0, 0) with radius 1.0
    uint32_t body = world.CreateBody(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, true);
    assert(body != 0 && "Body should be created");
    
    // Cast a ray that misses the sphere
    RaycastHit hit = world.Raycast(-5.0f, 5.0f, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f);
    
    assert(!hit.hit && "Ray should miss the sphere");
    
    world.Shutdown();
    std::cout << "✓ Raycast miss test passed" << std::endl;
}

void test_raycast_layer_filter()
{
    std::cout << "Testing raycast layer filtering..." << std::endl;
    
    PhysicsWorld world;
    PhysicsConfig config;
    config.max_bodies = 100;
    config.gravity = 0.0f;
    
    assert(world.Initialize(config) && "World should initialize");
    
    // Create a sphere on TRIGGER layer
    uint32_t body = world.CreateBody(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, true);
    world.SetBodyCollisionLayer(body, COLLISION_LAYER_TRIGGER, COLLISION_LAYER_ALL);
    
    // Cast ray with mask that doesn't include TRIGGER
    RaycastHit hit = world.Raycast(-5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f, COLLISION_LAYER_DEFAULT);
    
    assert(!hit.hit && "Ray should not hit body on non-matching layer");
    
    // Cast ray with mask that includes TRIGGER
    hit = world.Raycast(-5.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f, COLLISION_LAYER_TRIGGER);
    
    assert(hit.hit && "Ray should hit body on matching layer");
    
    world.Shutdown();
    std::cout << "✓ Raycast layer filtering test passed" << std::endl;
}

void test_raycast_closest_hit()
{
    std::cout << "Testing raycast closest hit..." << std::endl;
    
    PhysicsWorld world;
    PhysicsConfig config;
    config.max_bodies = 100;
    config.gravity = 0.0f;
    
    assert(world.Initialize(config) && "World should initialize");
    
    // Create two spheres along the ray path
    uint32_t body1 = world.CreateBody(2.0f, 0.0f, 0.0f, 1.0f, 1.0f, true);
    uint32_t body2 = world.CreateBody(6.0f, 0.0f, 0.0f, 1.0f, 1.0f, true);
    
    // Cast ray from origin in +X direction
    RaycastHit hit = world.Raycast(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f);
    
    assert(hit.hit && "Ray should hit");
    assert(hit.body_id == body1 && "Ray should hit the closer body");
    assert(std::abs(hit.distance - 1.0f) < 0.01f && "Hit distance should be ~1.0");
    
    world.Shutdown();
    std::cout << "✓ Raycast closest hit test passed" << std::endl;
}

void test_raycast_max_distance()
{
    std::cout << "Testing raycast max distance..." << std::endl;
    
    PhysicsWorld world;
    PhysicsConfig config;
    config.max_bodies = 100;
    config.gravity = 0.0f;
    
    assert(world.Initialize(config) && "World should initialize");
    
    // Create a sphere far away
    uint32_t body = world.CreateBody(20.0f, 0.0f, 0.0f, 1.0f, 1.0f, true);
    
    // Cast ray with max distance shorter than the sphere
    RaycastHit hit = world.Raycast(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 10.0f);
    
    assert(!hit.hit && "Ray should not hit beyond max distance");
    
    // Cast ray with sufficient max distance
    hit = world.Raycast(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 30.0f);
    
    assert(hit.hit && "Ray should hit within max distance");
    
    world.Shutdown();
    std::cout << "✓ Raycast max distance test passed" << std::endl;
}

void test_collision_layer_with_simulation()
{
    std::cout << "Testing collision layer with simulation..." << std::endl;
    
    PhysicsWorld world;
    PhysicsConfig config;
    config.max_bodies = 100;
    config.gravity = -9.81f;
    
    assert(world.Initialize(config) && "World should initialize");
    
    // Create a static ground
    uint32_t ground = world.CreateBody(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, true);
    world.SetBodyCollisionLayer(ground, COLLISION_LAYER_STATIC, COLLISION_LAYER_ALL);
    
    // Create a dynamic body that should collide with ground
    uint32_t body1 = world.CreateBody(0.0f, 5.0f, 0.0f, 1.0f, 1.0f, false);
    world.SetBodyCollisionLayer(body1, COLLISION_LAYER_DYNAMIC, COLLISION_LAYER_STATIC);
    
    // Create another dynamic body that should NOT collide with ground
    uint32_t body2 = world.CreateBody(2.0f, 5.0f, 0.0f, 1.0f, 1.0f, false);
    world.SetBodyCollisionLayer(body2, COLLISION_LAYER_DYNAMIC, COLLISION_LAYER_DEFAULT); // No STATIC in mask
    
    // Simulate
    world.Update(0.5f);
    
    // Check positions
    float x1, y1, z1;
    float x2, y2, z2;
    world.GetBodyPosition(body1, x1, y1, z1);
    world.GetBodyPosition(body2, x2, y2, z2);
    
    // body1 should have stopped at ground level (radius)
    assert(y1 >= 0.9f && y1 <= 1.1f && "Body1 should be near ground after collision");
    
    // body2 should have fallen through (no collision)
    assert(y2 < 2.0f && "Body2 should have fallen through without collision");
    
    world.Shutdown();
    std::cout << "✓ Collision layer with simulation test passed" << std::endl;
}

void run_physics_advanced_tests()
{
    std::cout << "=== Advanced Physics System Tests ===" << std::endl;
    
    test_collision_layer_setup();
    test_collision_filtering();
    test_raycast_hit();
    test_raycast_miss();
    test_raycast_layer_filter();
    test_raycast_closest_hit();
    test_raycast_max_distance();
    test_collision_layer_with_simulation();
    
    std::cout << "=== All advanced physics tests passed! ===" << std::endl;
}

} // namespace Physics
} // namespace Poko

int main()
{
    Poko::Physics::run_physics_advanced_tests();
    return 0;
}
