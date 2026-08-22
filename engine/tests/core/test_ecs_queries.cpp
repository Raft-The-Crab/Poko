/**
 * @file test_ecs_queries.cpp
 * @brief Tests for ECS component queries and archetypes
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "core/ecs/component_manager.h"
#include "core/ecs/entity.h"
#include "core/ecs/component.h"
#include <iostream>
#include <cassert>

namespace Poko {
namespace ECS {

// Test component types - properly inherit from Component
struct Position : public Component {
    float x, y, z;
    Position(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}
    ComponentType GetType() const override { return ComponentTypeID<Position>::Get(); }
};

struct Velocity : public Component {
    float vx, vy, vz;
    Velocity(float vx = 0.0f, float vy = 0.0f, float vz = 0.0f) : vx(vx), vy(vy), vz(vz) {}
    ComponentType GetType() const override { return ComponentTypeID<Velocity>::Get(); }
};

struct Health : public Component {
    float current;
    float max;
    Health(float current = 100.0f, float max = 100.0f) : current(current), max(max) {}
    ComponentType GetType() const override { return ComponentTypeID<Health>::Get(); }
};

struct Renderable : public Component {
    uint32_t mesh_id;
    Renderable(uint32_t id = 0) : mesh_id(id) {}
    ComponentType GetType() const override { return ComponentTypeID<Renderable>::Get(); }
};

void test_component_query_basic()
{
    std::cout << "Testing basic component query..." << std::endl;
    
    ComponentManager manager;
    
    // Create entities with different component combinations
    Entity e1(1);
    Entity e2(2);
    Entity e3(3);
    Entity e4(4);
    
    // e1: Position + Velocity
    manager.AddComponent<Position>(e1, new Position(1.0f, 2.0f, 3.0f));
    manager.AddComponent<Velocity>(e1, new Velocity(0.5f, 0.0f, 0.0f));
    
    // e2: Position + Health
    manager.AddComponent<Position>(e2, new Position(5.0f, 6.0f, 7.0f));
    manager.AddComponent<Health>(e2, new Health(75.0f, 100.0f));
    
    // e3: Position + Velocity + Health
    manager.AddComponent<Position>(e3, new Position(10.0f, 11.0f, 12.0f));
    manager.AddComponent<Velocity>(e3, new Velocity(1.0f, 1.0f, 1.0f));
    manager.AddComponent<Health>(e3, new Health(50.0f, 100.0f));
    
    // e4: Position only
    manager.AddComponent<Position>(e4, new Position(15.0f, 16.0f, 17.0f));
    
    // Query for entities with Position + Velocity
    ComponentQuery query;
    query.Require<Position>().Require<Velocity>();
    
    auto entities = manager.Query(query);
    
    assert(entities.size() == 2 && "Query should return 2 entities with Position + Velocity");
    
    // Verify the entities are e1 and e3
    bool found_e1 = false;
    bool found_e3 = false;
    for (const auto& entity : entities) {
        if (entity.GetID() == 1) found_e1 = true;
        if (entity.GetID() == 3) found_e3 = true;
    }
    (void)found_e1; // Suppress unused warning
    (void)found_e3; // Suppress unused warning
    assert(found_e1 && "e1 should be in query results");
    assert(found_e3 && "e3 should be in query results");
    
    std::cout << "✓ Basic component query test passed" << std::endl;
}

void test_component_query_with_exclusion()
{
    std::cout << "Testing component query with exclusion..." << std::endl;
    
    ComponentManager manager;
    
    Entity e1(1);
    Entity e2(2);
    Entity e3(3);
    
    // e1: Position + Velocity
    manager.AddComponent<Position>(e1, new Position(1.0f, 2.0f, 3.0f));
    manager.AddComponent<Velocity>(e1, new Velocity(0.5f, 0.0f, 0.0f));
    
    // e2: Position + Velocity + Health
    manager.AddComponent<Position>(e2, new Position(5.0f, 6.0f, 7.0f));
    manager.AddComponent<Velocity>(e2, new Velocity(1.0f, 1.0f, 1.0f));
    manager.AddComponent<Health>(e2, new Health(75.0f, 100.0f));
    
    // e3: Position + Velocity + Renderable
    manager.AddComponent<Position>(e3, new Position(10.0f, 11.0f, 12.0f));
    manager.AddComponent<Velocity>(e3, new Velocity(2.0f, 2.0f, 2.0f));
    manager.AddComponent<Renderable>(e3, new Renderable(42));
    
    // Query for entities with Position + Velocity but NOT Health
    ComponentQuery query;
    query.Require<Position>().Require<Velocity>().Exclude<Health>();
    
    auto entities = manager.Query(query);
    
    assert(entities.size() == 2 && "Query should return 2 entities excluding Health");
    
    // Verify e1 and e3 are included, e2 is excluded
    bool found_e1 = false;
    bool found_e2 = false;
    bool found_e3 = false;
    for (const auto& entity : entities) {
        if (entity.GetID() == 1) found_e1 = true;
        if (entity.GetID() == 2) found_e2 = true;
        if (entity.GetID() == 3) found_e3 = true;
    }
    (void)found_e1; // Suppress unused warning
    (void)found_e2; // Suppress unused warning
    (void)found_e3; // Suppress unused warning
    assert(found_e1 && "e1 should be in query results");
    assert(!found_e2 && "e2 should be excluded (has Health)");
    assert(found_e3 && "e3 should be in query results");
    
    std::cout << "✓ Component query with exclusion test passed" << std::endl;
}

void test_component_view_iteration()
{
    std::cout << "Testing component query iteration..." << std::endl;
    
    ComponentManager manager;
    
    Entity e1(1);
    Entity e2(2);
    Entity e3(3);
    
    manager.AddComponent<Position>(e1, new Position(1.0f, 2.0f, 3.0f));
    manager.AddComponent<Velocity>(e1, new Velocity(0.5f, 0.0f, 0.0f));
    
    manager.AddComponent<Position>(e2, new Position(5.0f, 6.0f, 7.0f));
    manager.AddComponent<Velocity>(e2, new Velocity(1.0f, 1.0f, 1.0f));
    
    manager.AddComponent<Position>(e3, new Position(10.0f, 11.0f, 12.0f));
    manager.AddComponent<Velocity>(e3, new Velocity(2.0f, 2.0f, 2.0f));
    
    ComponentQuery query;
    query.Require<Position>().Require<Velocity>();
    
    auto entities = manager.Query(query);
    
    int count = 0;
    for (Entity entity : entities) {
        count++;
        // Verify components are valid
        Position* pos = manager.GetComponent<Position>(entity);
        Velocity* vel = manager.GetComponent<Velocity>(entity);
        (void)pos; // Suppress unused warning
        (void)vel; // Suppress unused warning
        assert(pos != nullptr && "Position should not be null");
        assert(vel != nullptr && "Velocity should not be null");
    }
    
    (void)count; // Suppress unused warning
    assert(count == 3 && "Should iterate over 3 entities");
    
    std::cout << "✓ Component query iteration test passed" << std::endl;
}

void test_get_entities_with_component()
{
    std::cout << "Testing GetEntitiesWithComponent..." << std::endl;
    
    ComponentManager manager;
    
    Entity e1(1);
    Entity e2(2);
    Entity e3(3);
    Entity e4(4);
    
    manager.AddComponent<Position>(e1, new Position(1.0f, 2.0f, 3.0f));
    manager.AddComponent<Velocity>(e1, new Velocity(0.5f, 0.0f, 0.0f));
    
    manager.AddComponent<Position>(e2, new Position(5.0f, 6.0f, 7.0f));
    manager.AddComponent<Health>(e2, new Health(75.0f, 100.0f));
    
    manager.AddComponent<Position>(e3, new Position(10.0f, 11.0f, 12.0f));
    
    // e4 has no Position
    
    auto entities_with_position = manager.GetEntitiesWithComponent<Position>();
    
    assert(entities_with_position.size() == 3 && "Should find 3 entities with Position");
    
    auto entities_with_velocity = manager.GetEntitiesWithComponent<Velocity>();
    assert(entities_with_velocity.size() == 1 && "Should find 1 entity with Velocity");
    
    auto entities_with_health = manager.GetEntitiesWithComponent<Health>();
    assert(entities_with_health.size() == 1 && "Should find 1 entity with Health");
    
    std::cout << "✓ GetEntitiesWithComponent test passed" << std::endl;
}

void test_query_empty_results()
{
    std::cout << "Testing query with empty results..." << std::endl;
    
    ComponentManager manager;
    
    Entity e1(1);
    manager.AddComponent<Position>(e1, new Position(1.0f, 2.0f, 3.0f));
    
    // Query for component that doesn't exist
    ComponentQuery query;
    query.Require<Health>();
    
    auto entities = manager.Query(query);
    
    assert(entities.empty() && "Query should return empty results");
    
    std::cout << "✓ Empty query results test passed" << std::endl;
}

void test_component_removal_updates_signature()
{
    std::cout << "Testing component removal updates signature..." << std::endl;
    
    ComponentManager manager;
    
    Entity e1(1);
    manager.AddComponent<Position>(e1, new Position(1.0f, 2.0f, 3.0f));
    manager.AddComponent<Velocity>(e1, new Velocity(0.5f, 0.0f, 0.0f));
    
    // Query for Position + Velocity
    ComponentQuery query;
    query.Require<Position>().Require<Velocity>();
    
    auto entities1 = manager.Query(query);
    assert(entities1.size() == 1 && "Should find 1 entity initially");
    
    // Remove Velocity
    manager.RemoveComponent<Velocity>(e1);
    
    // Query again
    auto entities2 = manager.Query(query);
    assert(entities2.empty() && "Should find 0 entities after removal");
    
    // Query for Position only
    ComponentQuery query2;
    query2.Require<Position>();
    
    auto entities3 = manager.Query(query2);
    assert(entities3.size() == 1 && "Should find 1 entity with Position only");
    
    std::cout << "✓ Component removal signature update test passed" << std::endl;
}

void test_remove_all_updates_signature()
{
    std::cout << "Testing RemoveAllComponents updates signature..." << std::endl;
    
    ComponentManager manager;
    
    Entity e1(1);
    manager.AddComponent<Position>(e1, new Position(1.0f, 2.0f, 3.0f));
    manager.AddComponent<Velocity>(e1, new Velocity(0.5f, 0.0f, 0.0f));
    manager.AddComponent<Health>(e1, new Health(100.0f, 100.0f));
    
    // Query for any component
    ComponentQuery query;
    query.Require<Position>();
    
    auto entities1 = manager.Query(query);
    assert(entities1.size() == 1 && "Should find 1 entity initially");
    
    // Remove all components
    manager.RemoveAllComponents(e1);
    
    // Query again
    auto entities2 = manager.Query(query);
    assert(entities2.empty() && "Should find 0 entities after RemoveAllComponents");
    
    std::cout << "✓ RemoveAllComponents signature update test passed" << std::endl;
}

void run_ecs_query_tests()
{
    std::cout << "=== ECS Query System Tests ===" << std::endl;
    
    test_component_query_basic();
    test_component_query_with_exclusion();
    test_component_view_iteration();
    test_get_entities_with_component();
    test_query_empty_results();
    test_component_removal_updates_signature();
    test_remove_all_updates_signature();
    
    std::cout << "=== All ECS query tests passed! ===" << std::endl;
}

} // namespace ECS
} // namespace Poko

int main()
{
    Poko::ECS::run_ecs_query_tests();
    return 0;
}
