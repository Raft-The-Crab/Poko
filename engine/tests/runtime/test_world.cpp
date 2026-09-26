/**
 * @file test_world.cpp
 * @brief Tests for World and Scene runtime
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "runtime/world.h"
#include "core/testing/test.h"

using namespace poko::runtime;

POKO_TEST(World, Creation) {
    World world("TestWorld");
    POKO_ASSERT_EQ(world.name(), std::string("TestWorld"));
    POKO_ASSERT_EQ(world.total_instance_count(), size_t(0));
}

POKO_TEST(World, CreateInstance) {
    World world("TestWorld");
    InstanceHandle handle = world.create_instance("TestInstance", InstanceType::Base);
    
    POKO_ASSERT_TRUE(handle.id() > 0);
    POKO_ASSERT_EQ(world.total_instance_count(), size_t(1));
    
    Instance* instance = world.get_instance(handle);
    POKO_ASSERT_NOT_NULL(instance);
    POKO_ASSERT_EQ(instance->name(), std::string("TestInstance"));
}

POKO_TEST(World, DestroyInstance) {
    World world("TestWorld");
    InstanceHandle handle = world.create_instance("TestInstance", InstanceType::Base);
    
    POKO_ASSERT_EQ(world.total_instance_count(), size_t(1));
    
    bool destroyed = world.destroy_instance(handle);
    POKO_ASSERT_TRUE(destroyed);
    POKO_ASSERT_EQ(world.total_instance_count(), size_t(0));
    
    Instance* instance = world.get_instance(handle);
    POKO_ASSERT_NULL(instance);
}

POKO_TEST(World, GetInstancesByType) {
    World world("TestWorld");
    
    world.create_instance("Base1", InstanceType::Base);
    world.create_instance("Base2", InstanceType::Base);
    world.create_instance("Model1", InstanceType::Model);
    
    auto base_instances = world.get_instances_by_type(InstanceType::Base);
    POKO_ASSERT_EQ(base_instances.size(), size_t(2));
    
    auto model_instances = world.get_instances_by_type(InstanceType::Model);
    POKO_ASSERT_EQ(model_instances.size(), size_t(1));
}

POKO_TEST(Scene, Creation) {
    Scene scene("TestScene");
    POKO_ASSERT_EQ(scene.name(), std::string("TestScene"));
}
