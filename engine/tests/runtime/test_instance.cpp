/**
 * @file test_instance.cpp
 * @brief Tests for Instance system
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "runtime/instance.h"
#include "core/testing/test.h"

using namespace poko::runtime;

// Test component
class TestComponent : public Component {
public:
    explicit TestComponent(Instance* owner) : Component(owner), value(0) {}
    int value;
};

POKO_TEST(Instance, Creation) {
    Instance instance("TestInstance", InstanceType::Base);

    POKO_ASSERT_TRUE(instance.id() != 0);
    POKO_ASSERT_EQ(instance.name(), std::string("TestInstance"));
    POKO_ASSERT_TRUE(instance.type() == InstanceType::Base);
    POKO_ASSERT_NULL(instance.parent());
    POKO_ASSERT_TRUE(instance.children().empty());
}

POKO_TEST(Instance, ParentChildHierarchy) {
    Instance parent("Parent", InstanceType::Base);
    Instance child("Child", InstanceType::Base);

    child.set_parent(&parent);

    POKO_ASSERT_EQ(child.parent(), &parent);
    POKO_ASSERT_EQ(parent.children().size(), size_t(1));
    POKO_ASSERT_EQ(parent.children()[0], &child);

    child.set_parent(nullptr);

    POKO_ASSERT_NULL(child.parent());
    POKO_ASSERT_TRUE(parent.children().empty());
}

POKO_TEST(Instance, Attributes) {
    Instance instance("Test", InstanceType::Base);

    instance.set_attribute("key1", "value1");
    instance.set_attribute("key2", "value2");

    POKO_ASSERT_TRUE(instance.has_attribute("key1"));
    POKO_ASSERT_TRUE(instance.has_attribute("key2"));
    POKO_ASSERT_EQ(instance.get_attribute("key1"), std::string("value1"));
    POKO_ASSERT_EQ(instance.get_attribute("key2"), std::string("value2"));
    POKO_ASSERT_FALSE(instance.has_attribute("key3"));

    instance.remove_attribute("key1");
    POKO_ASSERT_FALSE(instance.has_attribute("key1"));
}

POKO_TEST(Instance, Tags) {
    Instance instance("Test", InstanceType::Base);

    instance.add_tag("tag1");
    instance.add_tag("tag2");
    instance.add_tag("tag1"); // Duplicate

    POKO_ASSERT_TRUE(instance.has_tag("tag1"));
    POKO_ASSERT_TRUE(instance.has_tag("tag2"));
    POKO_ASSERT_EQ(instance.tags().size(), size_t(2));

    instance.remove_tag("tag1");
    POKO_ASSERT_FALSE(instance.has_tag("tag1"));
    POKO_ASSERT_EQ(instance.tags().size(), size_t(1));
}

POKO_TEST(Instance, Components) {
    Instance instance("Test", InstanceType::Base);

    TestComponent* comp = instance.add_component<TestComponent>();
    POKO_ASSERT_NOT_NULL(comp);
    POKO_ASSERT_EQ(comp->owner(), &instance);

    comp->value = 42;

    TestComponent* retrieved = instance.get_component<TestComponent>();
    POKO_ASSERT_NOT_NULL(retrieved);
    POKO_ASSERT_EQ(retrieved->value, 42);

    instance.remove_component<TestComponent>();
    POKO_ASSERT_NULL(instance.get_component<TestComponent>());
}
