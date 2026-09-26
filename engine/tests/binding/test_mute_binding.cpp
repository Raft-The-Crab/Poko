/**
 * @file test_mute_binding.cpp
 * @brief Tests for Mute ↔ Engine binding
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "binding/mute_binding.h"
#include "core/testing/test.h"
#include <cstring>

POKO_TEST(MuteBinding, CreateDestroy) {
    InstanceHandle handle = mute_instance_create("TestInstance", 0);
    POKO_ASSERT_NOT_NULL(handle);

    uint64_t id = mute_instance_get_id(handle);
    POKO_ASSERT_TRUE(id != 0);

    mute_instance_destroy(handle);
}

POKO_TEST(MuteBinding, Name) {
    InstanceHandle handle = mute_instance_create("OriginalName", 0);
    POKO_ASSERT_NOT_NULL(handle);

    char* name = mute_instance_get_name(handle);
    POKO_ASSERT_NOT_NULL(name);
    POKO_ASSERT_EQ(std::string(name), std::string("OriginalName"));
    delete[] name;

    mute_instance_set_name(handle, "NewName");
    name = mute_instance_get_name(handle);
    POKO_ASSERT_NOT_NULL(name);
    POKO_ASSERT_EQ(std::string(name), std::string("NewName"));
    delete[] name;

    mute_instance_destroy(handle);
}

POKO_TEST(MuteBinding, ParentChild) {
    InstanceHandle parent = mute_instance_create("Parent", 0);
    InstanceHandle child = mute_instance_create("Child", 0);
    POKO_ASSERT_NOT_NULL(parent);
    POKO_ASSERT_NOT_NULL(child);

    mute_instance_set_parent(child, parent);

    InstanceHandle child_parent = mute_instance_get_parent(child);
    POKO_ASSERT_EQ(child_parent, parent);

    POKO_ASSERT_EQ(mute_instance_get_child_count(parent), 1);
    InstanceHandle retrieved_child = mute_instance_get_child(parent, 0);
    POKO_ASSERT_EQ(retrieved_child, child);

    mute_instance_set_parent(child, nullptr);
    POKO_ASSERT_NULL(mute_instance_get_parent(child));
    POKO_ASSERT_EQ(mute_instance_get_child_count(parent), 0);

    mute_instance_destroy(parent);
    mute_instance_destroy(child);
}

POKO_TEST(MuteBinding, Attributes) {
    InstanceHandle handle = mute_instance_create("Test", 0);
    POKO_ASSERT_NOT_NULL(handle);

    mute_instance_set_attribute(handle, "key1", "value1");
    mute_instance_set_attribute(handle, "key2", "value2");

    POKO_ASSERT_TRUE(mute_instance_has_attribute(handle, "key1"));
    POKO_ASSERT_TRUE(mute_instance_has_attribute(handle, "key2"));
    POKO_ASSERT_FALSE(mute_instance_has_attribute(handle, "key3"));

    char* value = mute_instance_get_attribute(handle, "key1");
    POKO_ASSERT_NOT_NULL(value);
    POKO_ASSERT_EQ(std::string(value), std::string("value1"));
    delete[] value;

    mute_instance_remove_attribute(handle, "key1");
    POKO_ASSERT_FALSE(mute_instance_has_attribute(handle, "key1"));

    mute_instance_destroy(handle);
}

POKO_TEST(MuteBinding, Tags) {
    InstanceHandle handle = mute_instance_create("Test", 0);
    POKO_ASSERT_NOT_NULL(handle);

    mute_instance_add_tag(handle, "tag1");
    mute_instance_add_tag(handle, "tag2");
    mute_instance_add_tag(handle, "tag1"); // Duplicate

    POKO_ASSERT_TRUE(mute_instance_has_tag(handle, "tag1"));
    POKO_ASSERT_TRUE(mute_instance_has_tag(handle, "tag2"));
    POKO_ASSERT_EQ(mute_instance_get_tag_count(handle), 2);

    char* tag = mute_instance_get_tag(handle, 0);
    POKO_ASSERT_NOT_NULL(tag);
    delete[] tag;

    mute_instance_remove_tag(handle, "tag1");
    POKO_ASSERT_FALSE(mute_instance_has_tag(handle, "tag1"));
    POKO_ASSERT_EQ(mute_instance_get_tag_count(handle), 1);

    mute_instance_destroy(handle);
}
