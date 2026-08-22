/**
 * @file test_physics.cpp
 * @brief Basic tests for physics system
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "physics/jolt/physics_world.h"
#include <iostream>
#include <cassert>

using namespace Poko;
using namespace Poko::Physics;

void test_physics_creation()
{
    std::cout << "Testing physics world creation..." << std::endl;

    PhysicsWorld world;
    assert(!world.IsInitialized());

    std::cout << "✓ Physics world creation test passed" << std::endl;
}

void test_physics_initialization()
{
    std::cout << "Testing physics world initialization..." << std::endl;

    PhysicsConfig config;
    config.max_bodies = 100;
    config.gravity = -9.81f;

    PhysicsWorld world;
    world.Initialize(config);

    assert(world.IsInitialized());

    world.Shutdown();
    assert(!world.IsInitialized());

    std::cout << "✓ Physics world initialization test passed" << std::endl;
}

void test_physics_config()
{
    std::cout << "Testing physics configuration..." << std::endl;

    PhysicsConfig config;
    config.max_bodies = 2048;
    config.max_body_pairs = 2048;
    config.max_contact_constraints = 2048;
    config.gravity = -9.81f;

    assert(config.max_bodies == 2048);
    assert(config.max_body_pairs == 2048);
    assert(config.max_contact_constraints == 2048);
    assert(config.gravity == -9.81f);

    std::cout << "✓ Physics configuration test passed" << std::endl;
}

void test_physics_body_creation()
{
    std::cout << "Testing physics body creation..." << std::endl;

    PhysicsConfig config;
    PhysicsWorld world;
    world.Initialize(config);

    world.CreateBody(0.0f, 10.0f, 0.0f, 1.0f, false);
    assert(world.GetBodyCount() == 1);

    world.Shutdown();

    std::cout << "✓ Physics body creation test passed" << std::endl;
}

void test_physics_body_removal()
{
    std::cout << "Testing physics body removal..." << std::endl;

    PhysicsConfig config;
    PhysicsWorld world;
    world.Initialize(config);

    uint32_t body_id = world.CreateBody(0.0f, 10.0f, 0.0f, 1.0f, false);
    assert(world.GetBodyCount() == 1);

    world.RemoveBody(body_id);
    assert(world.GetBodyCount() == 0);

    world.Shutdown();

    std::cout << "✓ Physics body removal test passed" << std::endl;
}

void test_physics_velocity()
{
    std::cout << "Testing physics velocity..." << std::endl;

    PhysicsConfig config;
    PhysicsWorld world;
    world.Initialize(config);

    uint32_t body_id = world.CreateBody(0.0f, 10.0f, 0.0f, 1.0f, false);

    world.SetBodyVelocity(body_id, 5.0f, 0.0f, 0.0f);

    float x, y, z;
    world.GetBodyPosition(body_id, x, y, z);
    assert(x == 0.0f); // Position hasn't changed yet

    world.Shutdown();

    std::cout << "✓ Physics velocity test passed" << std::endl;
}

void test_physics_simulation()
{
    std::cout << "Testing physics simulation..." << std::endl;

    PhysicsConfig config;
    config.gravity = -9.81f;
    PhysicsWorld world;
    world.Initialize(config);

    uint32_t body_id = world.CreateBody(0.0f, 10.0f, 0.0f, 1.0f, false);

    float x, y, z;
    world.GetBodyPosition(body_id, x, y, z);
    assert(y == 10.0f);

    // Simulate 1 second (60 frames at 60fps)
    for (int i = 0; i < 60; i++) {
        world.Update(0.016f);
    }

    world.GetBodyPosition(body_id, x, y, z);
    // Body should have fallen due to gravity
    assert(y < 10.0f);

    world.Shutdown();

    std::cout << "✓ Physics simulation test passed" << std::endl;
}

void test_physics_static_body()
{
    std::cout << "Testing static body..." << std::endl;

    PhysicsConfig config;
    config.gravity = -9.81f;
    PhysicsWorld world;
    world.Initialize(config);

    uint32_t static_id = world.CreateBody(0.0f, 10.0f, 0.0f, 0.0f, true);
    uint32_t dynamic_id = world.CreateBody(0.0f, 10.0f, 0.0f, 1.0f, false);

    float sx, sy, sz;
    float dx, dy, dz;

    world.GetBodyPosition(static_id, sx, sy, sz);
    world.GetBodyPosition(dynamic_id, dx, dy, dz);

    // Simulate
    for (int i = 0; i < 60; i++) {
        world.Update(0.016f);
    }

    world.GetBodyPosition(static_id, sx, sy, sz);
    world.GetBodyPosition(dynamic_id, dx, dy, dz);

    // Static body should not move
    assert(sy == 10.0f);
    // Dynamic body should fall
    assert(dy < 10.0f);

    world.Shutdown();

    std::cout << "✓ Static body test passed" << std::endl;
}

void test_physics_collision()
{
    std::cout << "Testing physics collision..." << std::endl;

    PhysicsConfig config;
    PhysicsWorld world;
    world.Initialize(config);

    // Create two bodies close to each other
    uint32_t body1 = world.CreateBody(0.0f, 5.0f, 0.0f, 1.0f, 1.0f, false);
    uint32_t body2 = world.CreateBody(1.5f, 5.0f, 0.0f, 1.0f, 1.0f, false);

    // Bodies should be colliding (distance 1.5 < radius sum 2.0)
    assert(world.CheckCollision(body1, body2));

    // Give them velocity to move apart
    world.SetBodyVelocity(body1, -10.0f, 0.0f, 0.0f);
    for (int i = 0; i < 10; i++) {
        world.Update(0.016f);
    }

    // Bodies should no longer be colliding
    assert(!world.CheckCollision(body1, body2));

    world.Shutdown();

    std::cout << "✓ Physics collision test passed" << std::endl;
}

void test_physics_collision_response()
{
    std::cout << "Testing physics collision response..." << std::endl;

    PhysicsConfig config;
    config.gravity = 0.0f; // No gravity for this test
    PhysicsWorld world;
    world.Initialize(config);

    // Create two bodies moving towards each other
    uint32_t body1 = world.CreateBody(-5.0f, 5.0f, 0.0f, 1.0f, 1.0f, false);
    uint32_t body2 = world.CreateBody(5.0f, 5.0f, 0.0f, 1.0f, 1.0f, false);

    world.SetBodyVelocity(body1, 5.0f, 0.0f, 0.0f);
    world.SetBodyVelocity(body2, -5.0f, 0.0f, 0.0f);

    // Simulate collision
    for (int i = 0; i < 30; i++) {
        world.Update(0.016f);
    }

    // Bodies should have bounced and moved apart
    float x1, y1, z1;
    float x2, y2, z2;
    world.GetBodyPosition(body1, x1, y1, z1);
    world.GetBodyPosition(body2, x2, y2, z2);

    // They should have bounced and moved past each other
    assert(x1 < -5.0f); // Body 1 should have bounced left
    assert(x2 > 5.0f);  // Body 2 should have bounced right

    world.Shutdown();

    std::cout << "✓ Physics collision response test passed" << std::endl;
}

int main()
{
    std::cout << "=== Physics System Tests ===" << std::endl;

    try {
        test_physics_creation();
        test_physics_initialization();
        test_physics_config();
        test_physics_body_creation();
        test_physics_body_removal();
        test_physics_velocity();
        test_physics_simulation();
        test_physics_static_body();
        test_physics_collision();
        test_physics_collision_response();

        std::cout << "\n=== All physics tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}