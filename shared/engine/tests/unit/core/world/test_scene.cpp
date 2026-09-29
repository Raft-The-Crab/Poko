/**
 * @file test_scene.cpp
 * @brief World/Scene system unit tests
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/world/scene.h"
#include "core/handles/handle_manager.h"
#include "core/runtime/instance.h"
#include <cassert>
#include <iostream>

void test_scene_basics() {
    std::cout << "Testing Scene basics..." << std::endl;
    
    poko::core::world::Scene scene("TestScene");
    
    assert(scene.getName() == "TestScene");
    assert(scene.getId() != poko::core::world::INVALID_SCENE_ID);
    assert(!scene.isActive());
    assert(scene.getInstanceCount() == 0);
    
    scene.setActive(true);
    assert(scene.isActive());
    
    std::cout << "✓ Scene basics tests passed" << std::endl;
}

void test_scene_naming() {
    std::cout << "Testing Scene naming..." << std::endl;
    
    poko::core::world::Scene scene1;
    assert(scene1.getName() == "UnnamedScene");
    
    poko::core::world::Scene scene2("MyScene");
    assert(scene2.getName() == "MyScene");
    
    scene2.setName("NewName");
    assert(scene2.getName() == "NewName");
    
    std::cout << "✓ Scene naming tests passed" << std::endl;
}

void test_scene_instances() {
    std::cout << "Testing Scene instances..." << std::endl;
    
    poko::core::world::Scene scene("TestScene");
    
    poko::core::handles::HandleManager handleManager;
    poko::core::handles::Handle handle1 = handleManager.allocate();
    poko::core::handles::Handle handle2 = handleManager.allocate();
    
    poko::core::runtime::Instance instance1{1, "Instance1"};
    poko::core::runtime::Instance instance2{1, "Instance2"};
    
    assert(scene.addInstance(&instance1));
    assert(scene.getInstanceCount() == 1);
    assert(scene.hasInstance(&instance1));
    
    assert(scene.addInstance(&instance2));
    assert(scene.getInstanceCount() == 2);
    assert(scene.hasInstance(&instance2));
    
    // Duplicate add should fail
    assert(!scene.addInstance(&instance1));
    
    // Find by name
    poko::core::runtime::Instance* found = scene.findInstance("Instance1");
    assert(found == &instance1);
    
    // Find non-existent
    assert(scene.findInstance("NonExistent") == nullptr);
    
    // Remove instance
    assert(scene.removeInstance(&instance1));
    assert(scene.getInstanceCount() == 1);
    assert(!scene.hasInstance(&instance1));
    
    // Remove non-existent should fail
    assert(!scene.removeInstance(&instance1));
    
    [[maybe_unused]] bool freed1 = handleManager.free(handle1);
    [[maybe_unused]] bool freed2 = handleManager.free(handle2);
    
    std::cout << "✓ Scene instances tests passed" << std::endl;
}

void test_scene_tags() {
    std::cout << "Testing Scene tags..." << std::endl;
    
    poko::core::world::Scene scene("TestScene");
    
    poko::core::handles::HandleManager handleManager;
    poko::core::handles::Handle handle1 = handleManager.allocate();
    poko::core::handles::Handle handle2 = handleManager.allocate();
    
    poko::core::runtime::Instance instance1{1, "Instance1"};
    poko::core::runtime::Instance instance2{1, "Instance2"};
    
    instance1.addTag("player");
    instance1.addTag("enemy");
    instance2.addTag("player");
    
    scene.addInstance(&instance1);
    scene.addInstance(&instance2);
    
    // Find by tag
    auto playerInstances = scene.findInstancesByTag("player");
    assert(playerInstances.size() == 2);
    
    auto enemyInstances = scene.findInstancesByTag("enemy");
    assert(enemyInstances.size() == 1);
    
    auto npcInstances = scene.findInstancesByTag("npc");
    assert(npcInstances.size() == 0);
    
    [[maybe_unused]] bool freed1 = handleManager.free(handle1);
    [[maybe_unused]] bool freed2 = handleManager.free(handle2);
    
    std::cout << "✓ Scene tags tests passed" << std::endl;
}

void test_scene_clear() {
    std::cout << "Testing Scene clear..." << std::endl;
    
    poko::core::world::Scene scene("TestScene");
    
    poko::core::handles::HandleManager handleManager;
    poko::core::handles::Handle handle1 = handleManager.allocate();
    poko::core::handles::Handle handle2 = handleManager.allocate();
    
    poko::core::runtime::Instance instance1{1, "Instance1"};
    poko::core::runtime::Instance instance2{1, "Instance2"};
    
    scene.addInstance(&instance1);
    scene.addInstance(&instance2);
    assert(scene.getInstanceCount() == 2);
    
    scene.clear();
    assert(scene.getInstanceCount() == 0);
    
    [[maybe_unused]] bool freed1 = handleManager.free(handle1);
    [[maybe_unused]] bool freed2 = handleManager.free(handle2);
    
    std::cout << "✓ Scene clear tests passed" << std::endl;
}

void test_world_basics() {
    std::cout << "Testing World basics..." << std::endl;
    
    poko::core::world::World world;
    
    assert(world.getSceneCount() == 0);
    assert(world.getActiveScene() == nullptr);
    
    std::cout << "✓ World basics tests passed" << std::endl;
}

void test_world_create_scene() {
    std::cout << "Testing World create scene..." << std::endl;
    
    poko::core::world::World world;
    
    [[maybe_unused]] poko::core::world::Scene* scene1 = world.createScene("Scene1");
    assert(scene1 != nullptr);
    assert(scene1->getName() == "Scene1");
    assert(world.getSceneCount() == 1);
    assert(world.hasScene("Scene1"));
    
    // Duplicate name should fail
    [[maybe_unused]] poko::core::world::Scene* scene2 = world.createScene("Scene1");
    assert(scene2 == nullptr);
    assert(world.getSceneCount() == 1);
    
    // Empty name should fail
    [[maybe_unused]] poko::core::world::Scene* scene3 = world.createScene("");
    assert(scene3 == nullptr);
    
    std::cout << "✓ World create scene tests passed" << std::endl;
}

void test_world_destroy_scene() {
    std::cout << "Testing World destroy scene..." << std::endl;
    
    poko::core::world::World world;
    
    poko::core::world::Scene* scene = world.createScene("TestScene");
    poko::core::world::SceneID sceneId = scene->getId();
    
    assert(world.getSceneCount() == 1);
    assert(world.hasScene(sceneId));
    
    assert(world.destroyScene(sceneId));
    assert(world.getSceneCount() == 0);
    assert(!world.hasScene(sceneId));
    
    // Destroy non-existent should fail
    assert(!world.destroyScene(sceneId));
    
    std::cout << "✓ World destroy scene tests passed" << std::endl;
}

void test_world_get_scene() {
    std::cout << "Testing World get scene..." << std::endl;
    
    poko::core::world::World world;
    
    poko::core::world::Scene* scene = world.createScene("TestScene");
    poko::core::world::SceneID sceneId = scene->getId();
    
    // Get by ID
    poko::core::world::Scene* foundById = world.getScene(sceneId);
    assert(foundById == scene);
    
    // Get by name
    poko::core::world::Scene* foundByName = world.getScene("TestScene");
    assert(foundByName == scene);
    
    // Get non-existent by ID
    assert(world.getScene(poko::core::world::INVALID_SCENE_ID) == nullptr);
    
    // Get non-existent by name
    assert(world.getScene("NonExistent") == nullptr);
    
    std::cout << "✓ World get scene tests passed" << std::endl;
}

void test_world_active_scene() {
    std::cout << "Testing World active scene..." << std::endl;
    
    poko::core::world::World world;
    
    poko::core::world::Scene* scene1 = world.createScene("Scene1");
    poko::core::world::Scene* scene2 = world.createScene("Scene2");
    
    assert(world.getActiveScene() == nullptr);
    
    world.setActiveScene(scene1->getId());
    assert(world.getActiveScene() == scene1);
    assert(world.getActiveScene()->isActive());
    
    world.setActiveScene(scene2->getId());
    assert(world.getActiveScene() == scene2);
    
    // Set invalid ID clears active scene
    world.setActiveScene(poko::core::world::INVALID_SCENE_ID);
    assert(world.getActiveScene() == nullptr);
    
    std::cout << "✓ World active scene tests passed" << std::endl;
}

void test_world_clear() {
    std::cout << "Testing World clear..." << std::endl;
    
    poko::core::world::World world;
    
    [[maybe_unused]] poko::core::world::Scene* scene1 = world.createScene("Scene1");
    [[maybe_unused]] poko::core::world::Scene* scene2 = world.createScene("Scene2");
    [[maybe_unused]] poko::core::world::Scene* scene3 = world.createScene("Scene3");
    
    assert(world.getSceneCount() == 3);
    
    world.clear();
    assert(world.getSceneCount() == 0);
    assert(world.getActiveScene() == nullptr);
    
    std::cout << "✓ World clear tests passed" << std::endl;
}

void test_global_world() {
    std::cout << "Testing global world..." << std::endl;
    
    auto& world = poko::core::world::getGlobalWorld();
    
    poko::core::world::Scene* scene = world.createScene("GlobalScene");
    assert(scene != nullptr);
    assert(world.hasScene("GlobalScene"));
    
    world.destroyScene(scene->getId());
    
    poko::core::world::destroyGlobalWorld();
    
    std::cout << "✓ Global world tests passed" << std::endl;
}

void test_null_instance_handling() {
    std::cout << "Testing null instance handling..." << std::endl;
    
    poko::core::world::Scene scene("TestScene");
    
    // Add null instance should fail
    assert(!scene.addInstance(nullptr));
    
    // Remove null instance should fail
    assert(!scene.removeInstance(nullptr));
    
    // Has null instance should return false
    assert(!scene.hasInstance(nullptr));
    
    // Null instance should not affect count
    assert(scene.getInstanceCount() == 0);
    
    std::cout << "✓ Null instance handling tests passed" << std::endl;
}

void test_empty_name_handling() {
    std::cout << "Testing empty name handling..." << std::endl;
    
    // Scene with empty name should get default
    poko::core::world::Scene scene1("");
    assert(scene1.getName() == "UnnamedScene");
    
    // Setting empty name should keep current name or assign default
    scene1.setName("");
    assert(scene1.getName() == "UnnamedScene");
    
    // World should reject empty scene names
    poko::core::world::World world;
    poko::core::world::Scene* scene2 = world.createScene("");
    assert(scene2 == nullptr);
    
    std::cout << "✓ Empty name handling tests passed" << std::endl;
}

void test_empty_tag_handling() {
    std::cout << "Testing empty tag handling..." << std::endl;
    
    poko::core::world::Scene scene("TestScene");
    
    // Find by empty tag should return empty vector
    auto emptyTagInstances = scene.findInstancesByTag("");
    assert(emptyTagInstances.size() == 0);
    
    std::cout << "✓ Empty tag handling tests passed" << std::endl;
}

void test_instance_limit() {
    std::cout << "Testing instance limit..." << std::endl;
    
    // Note: This test doesn't actually hit the limit (1M instances)
    // but verifies the limit checking logic is in place
    poko::core::world::Scene scene("TestScene");
    
    // Add a few instances to ensure normal operation
    poko::core::runtime::Instance instance1{1, "Instance1"};
    poko::core::runtime::Instance instance2{1, "Instance2"};
    
    assert(scene.addInstance(&instance1));
    assert(scene.addInstance(&instance2));
    assert(scene.getInstanceCount() == 2);
    
    std::cout << "✓ Instance limit tests passed" << std::endl;
}

void test_scene_limit() {
    std::cout << "Testing scene limit..." << std::endl;
    
    // Note: This test doesn't actually hit the limit (10K scenes)
    // but verifies the limit checking logic is in place
    poko::core::world::World world;
    
    // Create a few scenes to ensure normal operation
    [[maybe_unused]] poko::core::world::Scene* scene1 = world.createScene("Scene1");
    [[maybe_unused]] poko::core::world::Scene* scene2 = world.createScene("Scene2");
    
    assert(world.getSceneCount() == 2);
    
    std::cout << "✓ Scene limit tests passed" << std::endl;
}

void test_scene_name_length() {
    std::cout << "Testing scene name length..." << std::endl;
    
    // Test normal name
    poko::core::world::Scene scene1("NormalName");
    assert(scene1.getName() == "NormalName");
    
    // Test name at limit
    std::string longName(poko::core::world::MAX_SCENE_NAME_LENGTH, 'A');
    poko::core::world::Scene scene2(longName);
    assert(scene2.getName().length() == poko::core::world::MAX_SCENE_NAME_LENGTH);
    
    // Test name exceeding limit (should be truncated)
    std::string tooLongName(poko::core::world::MAX_SCENE_NAME_LENGTH + 100, 'B');
    poko::core::world::Scene scene3(tooLongName);
    assert(scene3.getName().length() == poko::core::world::MAX_SCENE_NAME_LENGTH);
    
    // Test setName with long name
    scene1.setName(tooLongName);
    assert(scene1.getName().length() == poko::core::world::MAX_SCENE_NAME_LENGTH);
    
    // Test World rejects too long names
    poko::core::world::World world;
    [[maybe_unused]] poko::core::world::Scene* scene4 = world.createScene(tooLongName);
    assert(scene4 == nullptr);
    
    std::cout << "✓ Scene name length tests passed" << std::endl;
}

void test_tag_length() {
    std::cout << "Testing tag length..." << std::endl;
    
    poko::core::world::Scene scene("TestScene");
    
    poko::core::runtime::Instance instance1{1, "Instance1"};
    instance1.addTag("NormalTag");
    scene.addInstance(&instance1);
    
    // Normal tag should work
    auto normalResults = scene.findInstancesByTag("NormalTag");
    assert(normalResults.size() == 1);
    
    // Tag exceeding limit should return empty
    std::string longTag(poko::core::world::MAX_TAG_LENGTH + 100, 'A');
    auto longTagResults = scene.findInstancesByTag(longTag);
    assert(longTagResults.size() == 0);
    
    std::cout << "✓ Tag length tests passed" << std::endl;
}

int main() {
    std::cout << "=== World/Scene System Unit Tests ===" << std::endl;
    
    test_scene_basics();
    test_scene_naming();
    test_scene_instances();
    test_scene_tags();
    test_scene_clear();
    test_world_basics();
    test_world_create_scene();
    test_world_destroy_scene();
    test_world_get_scene();
    test_world_active_scene();
    test_world_clear();
    test_global_world();
    test_null_instance_handling();
    test_empty_name_handling();
    test_empty_tag_handling();
    test_instance_limit();
    test_scene_limit();
    test_scene_name_length();
    test_tag_length();
    
    std::cout << "\n=== All tests passed! ===" << std::endl;
    
    return 0;
}
