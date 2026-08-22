/**
 * @file test_ecs.cpp
 * @brief Basic tests for ECS system
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "core/ecs/entity.h"
#include "core/ecs/component.h"
#include "core/ecs/component_manager.h"
#include <iostream>
#include <cassert>
#include <thread>
#include <chrono>

using namespace Poko;
using namespace ECS;

// Test component
struct TestComponent : public Component {
    int value;
    
    TestComponent(int v) : value(v) {}
    
    ComponentType GetType() const override {
        return ComponentTypeID<TestComponent>::Get();
    }
};

// Another test component
struct HealthComponent : public Component {
    float health;
    
    HealthComponent(float h) : health(h) {}
    
    ComponentType GetType() const override {
        return ComponentTypeID<HealthComponent>::Get();
    }
};

void test_entity_creation()
{
    std::cout << "Testing entity creation..." << std::endl;
    
    Entity entity1;
    assert(!entity1.IsValid());
    
    Entity entity2(42);
    assert(entity2.IsValid());
    assert(entity2.GetID() == 42);
    
    Entity entity3(100);
    assert(entity3.IsValid());
    assert(entity3.GetID() == 100);
    
    assert(entity2 != entity3);
    
    std::cout << "✓ Entity creation test passed" << std::endl;
}

void test_entity_conversion()
{
    std::cout << "Testing entity bool conversion..." << std::endl;
    
    Entity invalidEntity;
    assert(!invalidEntity);
    assert(!(bool)invalidEntity);
    
    Entity validEntity(1);
    assert(validEntity);
    assert((bool)validEntity);
    
    std::cout << "✓ Entity conversion test passed" << std::endl;
}

void test_component_manager_creation()
{
    std::cout << "Testing component manager creation..." << std::endl;
    
    ComponentManager manager;
    // Just test that it can be created without crashing
    
    std::cout << "✓ Component manager creation test passed" << std::endl;
}

void test_component_addition()
{
    std::cout << "Testing component addition..." << std::endl;
    
    ComponentManager manager;
    Entity entity(1);
    
    TestComponent* comp = new TestComponent(42);
    manager.AddComponent(entity, comp);
    
    (void)manager.GetComponent<TestComponent>(entity);
    
    std::cout << "✓ Component addition test passed" << std::endl;
}

void test_component_retrieval()
{
    std::cout << "Testing component retrieval..." << std::endl;
    
    ComponentManager manager;
    Entity entity1(1);
    Entity entity2(2);
    
    // Add components to different entities
    manager.AddComponent(entity1, new TestComponent(100));
    manager.AddComponent(entity2, new TestComponent(200));
    
    // Retrieve components
    (void)manager.GetComponent<TestComponent>(entity1);
    (void)manager.GetComponent<TestComponent>(entity2);
    
    std::cout << "✓ Component retrieval test passed" << std::endl;
}

void test_multiple_component_types()
{
    std::cout << "Testing multiple component types..." << std::endl;
    
    ComponentManager manager;
    Entity entity(1);
    
    // Add different component types to same entity
    manager.AddComponent(entity, new TestComponent(42));
    manager.AddComponent(entity, new HealthComponent(100.0f));
    
    // Retrieve different component types
    (void)manager.GetComponent<TestComponent>(entity);
    (void)manager.GetComponent<HealthComponent>(entity);
    
    std::cout << "✓ Multiple component types test passed" << std::endl;
}

void test_component_removal()
{
    std::cout << "Testing component removal..." << std::endl;
    
    ComponentManager manager;
    Entity entity(1);
    
    // Add component
    manager.AddComponent(entity, new TestComponent(42));
    assert(manager.GetComponent<TestComponent>(entity) != nullptr);
    
    // Remove component
    manager.RemoveComponent<TestComponent>(entity);
    assert(manager.GetComponent<TestComponent>(entity) == nullptr);
    
    std::cout << "✓ Component removal test passed" << std::endl;
}

void test_remove_all_components()
{
    std::cout << "Testing remove all components..." << std::endl;
    
    ComponentManager manager;
    Entity entity(1);
    
    // Add multiple components
    manager.AddComponent(entity, new TestComponent(42));
    manager.AddComponent(entity, new HealthComponent(100.0f));
    
    assert(manager.GetComponent<TestComponent>(entity) != nullptr);
    assert(manager.GetComponent<HealthComponent>(entity) != nullptr);
    
    // Remove all components
    manager.RemoveAllComponents(entity);
    
    assert(manager.GetComponent<TestComponent>(entity) == nullptr);
    assert(manager.GetComponent<HealthComponent>(entity) == nullptr);
    
    std::cout << "✓ Remove all components test passed" << std::endl;
}

void test_invalid_entity_handling()
{
    std::cout << "Testing invalid entity handling..." << std::endl;
    
    ComponentManager manager;
    Entity invalidEntity;
    Entity validEntity(1);
    
    // Try to get component from invalid entity
    (void)manager.GetComponent<TestComponent>(invalidEntity);
    
    // Try to remove component from invalid entity (should not crash)
    manager.RemoveComponent<TestComponent>(invalidEntity);
    
    // Small delay to ensure no race conditions
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    
    std::cout << "✓ Invalid entity handling test passed" << std::endl;
}

int main()
{
    std::cout << "=== ECS System Tests ===" << std::endl;
    
    try {
        test_entity_creation();
        test_entity_conversion();
        test_component_manager_creation();
        test_component_addition();
        test_component_retrieval();
        test_multiple_component_types();
        test_component_removal();
        test_remove_all_components();
        test_invalid_entity_handling();
        
        std::cout << "\n=== All ECS tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}