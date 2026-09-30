/**
 * @file test_physics.cpp
 * @brief Physics component unit tests
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/physics.h"
#include "core/components/component.h"
#include <cassert>
#include <iostream>

using namespace poko::core::components;

void test_material() {
    std::cout << "Testing Material..." << std::endl;
    
    // Default constructor
    physics::Material mat1;
    assert(mat1.friction == 0.5f);
    assert(mat1.restitution == 0.3f);
    assert(mat1.density == 1000.0f);
    
    // Component constructor
    physics::Material mat2(0.8f, 0.5f, 2000.0f);
    assert(mat2.friction == 0.8f);
    assert(mat2.restitution == 0.5f);
    assert(mat2.density == 2000.0f);
    
    std::cout << "✓ Material tests passed" << std::endl;
}

void test_physics_basics() {
    std::cout << "Testing Physics basics..." << std::endl;
    
    physics::Physics physics;
    
    // Check component type
    assert(physics.getTypeID() == physics::Physics::COMPONENT_ID);
    assert(std::string(physics.getTypeName()) == physics::Physics::COMPONENT_NAME);
    
    // Default values
    assert(physics.getBodyType() == physics::BodyType::Dynamic);
    assert(physics.getShapeType() == physics::ShapeType::Box);
    assert(physics.getSphereRadius() == physics::DEFAULT_SPHERE_RADIUS);
    assert(physics.getBoxHalfExtX() == physics::DEFAULT_BOX_HALF_EXTENT);
    assert(physics.getBoxHalfExtY() == physics::DEFAULT_BOX_HALF_EXTENT);
    assert(physics.getBoxHalfExtZ() == physics::DEFAULT_BOX_HALF_EXTENT);
    assert(physics.getCapsuleRadius() == physics::DEFAULT_CAPSULE_RADIUS);
    assert(physics.getCapsuleHeight() == physics::DEFAULT_CAPSULE_HEIGHT);
    assert(physics.getFriction() == 0.5f);
    assert(physics.getRestitution() == 0.3f);
    assert(physics.getDensity() == 1000.0f);
    assert(physics.getMass() == 1.0f);
    assert(physics.getLinearVelocityX() == 0.0f);
    assert(physics.getLinearVelocityY() == 0.0f);
    assert(physics.getLinearVelocityZ() == 0.0f);
    assert(physics.getAngularVelocityX() == 0.0f);
    assert(physics.getAngularVelocityY() == 0.0f);
    assert(physics.getAngularVelocityZ() == 0.0f);
    assert(physics.getGravityScale() == physics::DEFAULT_GRAVITY_SCALE);
    assert(physics.getCollisionLayer() == 1);
    assert(physics.getCollisionMask() == 0xFFFFFFFF);
    assert(physics.getSleepThreshold() == physics::DEFAULT_SLEEP_THRESHOLD);
    assert(physics.isSleepEnabled());
    assert(!physics.isSleeping());
    
    std::cout << "✓ Physics basics tests passed" << std::endl;
}

void test_physics_body_type() {
    std::cout << "Testing Physics body type..." << std::endl;
    
    physics::Physics physics;
    
    // Default dynamic
    assert(physics.getBodyType() == physics::BodyType::Dynamic);
    
    // Set static
    physics.setBodyType(physics::BodyType::Static);
    assert(physics.getBodyType() == physics::BodyType::Static);
    
    // Set kinematic
    physics.setBodyType(physics::BodyType::Kinematic);
    assert(physics.getBodyType() == physics::BodyType::Kinematic);
    
    // Set character
    physics.setBodyType(physics::BodyType::Character);
    assert(physics.getBodyType() == physics::BodyType::Character);
    
    std::cout << "✓ Physics body type tests passed" << std::endl;
}

void test_physics_shape_type() {
    std::cout << "Testing Physics shape type..." << std::endl;
    
    physics::Physics physics;
    
    // Default box
    assert(physics.getShapeType() == physics::ShapeType::Box);
    
    // Set sphere
    physics.setShapeType(physics::ShapeType::Sphere);
    assert(physics.getShapeType() == physics::ShapeType::Sphere);
    
    // Set capsule
    physics.setShapeType(physics::ShapeType::Capsule);
    assert(physics.getShapeType() == physics::ShapeType::Capsule);
    
    std::cout << "✓ Physics shape type tests passed" << std::endl;
}

void test_physics_material_clamping() {
    std::cout << "Testing Physics material clamping..." << std::endl;
    
    physics::Physics physics;
    
    // Test friction clamping
    physics.setFriction(-1.0f);
    assert(physics.getFriction() == physics::MIN_FRICTION);
    
    physics.setFriction(10.0f);
    assert(physics.getFriction() == physics::MAX_FRICTION);
    
    physics.setFriction(0.7f);
    assert(physics.getFriction() == 0.7f);
    
    // Test restitution clamping
    physics.setRestitution(-0.5f);
    assert(physics.getRestitution() == physics::MIN_RESTITUTION);
    
    physics.setRestitution(2.0f);
    assert(physics.getRestitution() == physics::MAX_RESTITUTION);
    
    physics.setRestitution(0.8f);
    assert(physics.getRestitution() == 0.8f);
    
    // Test density clamping
    physics.setDensity(0.0f);
    assert(physics.getDensity() == physics::MIN_DENSITY);
    
    physics.setDensity(100000.0f);
    assert(physics.getDensity() == physics::MAX_DENSITY);
    
    physics.setDensity(1500.0f);
    assert(physics.getDensity() == 1500.0f);
    
    std::cout << "✓ Physics material clamping tests passed" << std::endl;
}

void test_physics_mass_clamping() {
    std::cout << "Testing Physics mass clamping..." << std::endl;
    
    physics::Physics physics;
    
    // Test mass clamping
    physics.setMass(0.0f);
    assert(physics.getMass() == physics::MIN_MASS);
    
    physics.setMass(1000000.0f);
    assert(physics.getMass() == physics::MAX_MASS);
    
    physics.setMass(50.0f);
    assert(physics.getMass() == 50.0f);
    
    std::cout << "✓ Physics mass clamping tests passed" << std::endl;
}

void test_physics_shape_dimensions() {
    std::cout << "Testing Physics shape dimensions..." << std::endl;
    
    physics::Physics physics;
    
    // Test sphere radius clamping
    physics.setSphereRadius(0.0f);
    assert(physics.getSphereRadius() == physics::MIN_BOUND_EXTENT);
    
    physics.setSphereRadius(999999.0f);
    assert(physics.getSphereRadius() == physics::MAX_BOUND_EXTENT);
    
    physics.setSphereRadius(2.5f);
    assert(physics.getSphereRadius() == 2.5f);
    
    // Test box half-extents
    physics.setBoxHalfExtents(1.0f, 2.0f, 3.0f);
    assert(physics.getBoxHalfExtX() == 1.0f);
    assert(physics.getBoxHalfExtY() == 2.0f);
    assert(physics.getBoxHalfExtZ() == 3.0f);
    
    // Test capsule dimensions
    physics.setCapsuleRadius(0.3f);
    physics.setCapsuleHeight(4.0f);
    assert(physics.getCapsuleRadius() == 0.3f);
    assert(physics.getCapsuleHeight() == 4.0f);
    
    std::cout << "✓ Physics shape dimensions tests passed" << std::endl;
}

void test_physics_velocity() {
    std::cout << "Testing Physics velocity..." << std::endl;
    
    physics::Physics physics;
    
    // Set linear velocity
    physics.setLinearVelocity(1.0f, 2.0f, 3.0f);
    assert(physics.getLinearVelocityX() == 1.0f);
    assert(physics.getLinearVelocityY() == 2.0f);
    assert(physics.getLinearVelocityZ() == 3.0f);
    
    // Set angular velocity
    physics.setAngularVelocity(0.5f, 1.0f, 1.5f);
    assert(physics.getAngularVelocityX() == 0.5f);
    assert(physics.getAngularVelocityY() == 1.0f);
    assert(physics.getAngularVelocityZ() == 1.5f);
    
    std::cout << "✓ Physics velocity tests passed" << std::endl;
}

void test_physics_gravity() {
    std::cout << "Testing Physics gravity..." << std::endl;
    
    physics::Physics physics;
    
    // Default gravity scale
    assert(physics.getGravityScale() == physics::DEFAULT_GRAVITY_SCALE);
    
    // Set gravity scale
    physics.setGravityScale(0.5f);
    assert(physics.getGravityScale() == 0.5f);
    
    physics.setGravityScale(2.0f);
    assert(physics.getGravityScale() == 2.0f);
    
    std::cout << "✓ Physics gravity tests passed" << std::endl;
}

void test_physics_collision() {
    std::cout << "Testing Physics collision..." << std::endl;
    
    physics::Physics physics;
    
    // Default collision layer/mask
    assert(physics.getCollisionLayer() == 1);
    assert(physics.getCollisionMask() == 0xFFFFFFFF);
    
    // Set collision layer
    physics.setCollisionLayer(0x00000002);
    assert(physics.getCollisionLayer() == 0x00000002);
    
    // Set collision mask
    physics.setCollisionMask(0x000000FF);
    assert(physics.getCollisionMask() == 0x000000FF);
    
    std::cout << "✓ Physics collision tests passed" << std::endl;
}

void test_physics_sleeping() {
    std::cout << "Testing Physics sleeping..." << std::endl;
    
    physics::Physics physics;
    
    // Default sleep enabled
    assert(physics.isSleepEnabled());
    assert(!physics.isSleeping());
    
    // Set sleep threshold
    physics.setSleepThreshold(0.02f);
    assert(physics.getSleepThreshold() == 0.02f);
    
    // Disable sleep
    physics.setSleepEnabled(false);
    assert(!physics.isSleepEnabled());
    
    // Enable sleep
    physics.setSleepEnabled(true);
    assert(physics.isSleepEnabled());
    
    // Wake up
    physics.wakeUp();
    assert(!physics.isSleeping());
    
    // Sleep
    physics.sleep();
    assert(physics.isSleeping());
    
    std::cout << "✓ Physics sleeping tests passed" << std::endl;
}

void test_physics_forces() {
    std::cout << "Testing Physics forces..." << std::endl;
    
    physics::Physics physics;
    
    // Apply force
    physics.applyForce(10.0f, 20.0f, 30.0f);
    assert(physics.getAccumulatedForceX() == 10.0f);
    assert(physics.getAccumulatedForceY() == 20.0f);
    assert(physics.getAccumulatedForceZ() == 30.0f);
    
    // Apply impulse
    physics.applyImpulse(5.0f, 10.0f, 15.0f);
    assert(physics.getAccumulatedForceX() == 15.0f);
    assert(physics.getAccumulatedForceY() == 30.0f);
    assert(physics.getAccumulatedForceZ() == 45.0f);
    
    // Apply torque
    physics.applyTorque(1.0f, 2.0f, 3.0f);
    assert(physics.getAccumulatedTorqueX() == 1.0f);
    assert(physics.getAccumulatedTorqueY() == 2.0f);
    assert(physics.getAccumulatedTorqueZ() == 3.0f);
    
    // Clear forces
    physics.clearAccumulatedForces();
    assert(physics.getAccumulatedForceX() == 0.0f);
    assert(physics.getAccumulatedForceY() == 0.0f);
    assert(physics.getAccumulatedForceZ() == 0.0f);
    assert(physics.getAccumulatedTorqueX() == 0.0f);
    assert(physics.getAccumulatedTorqueY() == 0.0f);
    assert(physics.getAccumulatedTorqueZ() == 0.0f);
    
    std::cout << "✓ Physics forces tests passed" << std::endl;
}

void test_physics_lifecycle() {
    std::cout << "Testing Physics lifecycle..." << std::endl;
    
    physics::Physics physics;
    
    // Initial state
    assert(physics.getState() == ComponentState::None);
    
    // On create
    physics.onCreate();
    assert(physics.getState() == ComponentState::Created);
    
    // On activate
    physics.onActivate();
    assert(physics.getState() == ComponentState::Active);
    assert(physics.isActive());
    
    // On deactivate
    physics.onDeactivate();
    assert(physics.getState() == ComponentState::Deactivating);
    assert(!physics.isActive());
    
    // On destroy
    physics.onDestroy();
    assert(physics.getState() == ComponentState::Destroyed);
    
    std::cout << "✓ Physics lifecycle tests passed" << std::endl;
}

void test_physics_registration() {
    std::cout << "Testing Physics registration..." << std::endl;
    
    // Register with global registry
    registerPhysicsComponent();
    
    // Get global registry
    auto& registry = getGlobalComponentRegistry();
    
    // Create physics component
    auto component = registry.createComponent(physics::Physics::COMPONENT_ID);
    assert(component != nullptr);
    
    // Check type
    assert(component->getTypeID() == physics::Physics::COMPONENT_ID);
    assert(std::string(component->getTypeName()) == physics::Physics::COMPONENT_NAME);
    
    std::cout << "✓ Physics registration tests passed" << std::endl;
}

void test_physics_constants() {
    std::cout << "Testing Physics constants..." << std::endl;
    
    // Check material limits
    assert(physics::MAX_FRICTION == 2.0f);
    assert(physics::MIN_FRICTION == 0.0f);
    assert(physics::MAX_RESTITUTION == 1.0f);
    assert(physics::MIN_RESTITUTION == 0.0f);
    assert(physics::MAX_DENSITY == 10000.0f);
    assert(physics::MIN_DENSITY == 1.0f);
    
    // Check mass limits
    assert(physics::MAX_MASS == 100000.0f);
    assert(physics::MIN_MASS == 0.001f);
    
    // Check shape constants
    assert(physics::DEFAULT_SPHERE_RADIUS == 1.0f);
    assert(physics::DEFAULT_BOX_HALF_EXTENT == 1.0f);
    assert(physics::DEFAULT_CAPSULE_RADIUS == 0.5f);
    assert(physics::DEFAULT_CAPSULE_HEIGHT == 2.0f);
    
    // Check other constants
    assert(physics::DEFAULT_GRAVITY_SCALE == 1.0f);
    assert(physics::DEFAULT_SLEEP_THRESHOLD == 0.01f);
    assert(physics::MAX_BOUND_EXTENT == 100000.0f);
    assert(physics::MIN_BOUND_EXTENT == 0.0001f);
    
    std::cout << "✓ Physics constants tests passed" << std::endl;
}

int main() {
    std::cout << "=== Physics Component Unit Tests ===" << std::endl;
    
    test_material();
    test_physics_basics();
    test_physics_body_type();
    test_physics_shape_type();
    test_physics_material_clamping();
    test_physics_mass_clamping();
    test_physics_shape_dimensions();
    test_physics_velocity();
    test_physics_gravity();
    test_physics_collision();
    test_physics_sleeping();
    test_physics_forces();
    test_physics_lifecycle();
    test_physics_registration();
    test_physics_constants();
    
    std::cout << "\n=== All tests passed! ===" << std::endl;
    
    return 0;
}
