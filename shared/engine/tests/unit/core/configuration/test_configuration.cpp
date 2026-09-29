/**
 * @file test_configuration.cpp
 * @brief Unit tests for core configuration system
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/configuration/configuration.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>

namespace poko {
namespace core {
namespace configuration {
namespace test {

void test_configuration_basics() {
    std::cout << "Testing Configuration basics..." << std::endl;
    
    Configuration config;
    
    // Test set and get
    config.set("test_bool", true);
    config.set("test_int", 42);
    config.set("test_double", 3.14);
    config.set("test_string", "hello");
    
    assert(config.has("test_bool"));
    assert(config.has("test_int"));
    assert(config.has("test_double"));
    assert(config.has("test_string"));
    
    auto boolVal = config.getBool("test_bool");
    assert(boolVal && *boolVal == true);
    
    auto intVal = config.getInt("test_int");
    assert(intVal && *intVal == 42);
    
    auto doubleVal = config.getDouble("test_double");
    assert(doubleVal && *doubleVal == 3.14);
    
    auto stringVal = config.getString("test_string");
    assert(stringVal && *stringVal == "hello");
    
    std::cout << "✓ Configuration basics tests passed" << std::endl;
}

void test_configuration_defaults() {
    std::cout << "Testing Configuration defaults..." << std::endl;
    
    Configuration config;
    
    // Set defaults
    config.setDefault("default_int", 100);
    config.setDefault("default_string", "default");
    
    // Get default values when key doesn't exist
    auto defaultInt = config.getInt("default_int");
    assert(defaultInt && *defaultInt == 100);
    
    auto defaultString = config.getString("default_string");
    assert(defaultString && *defaultString == "default");
    
    // Override default with actual value
    config.set("default_int", 200);
    auto overriddenInt = config.getInt("default_int");
    assert(overriddenInt && *overriddenInt == 200);
    
    std::cout << "✓ Configuration defaults tests passed" << std::endl;
}

void test_configuration_remove() {
    std::cout << "Testing Configuration remove..." << std::endl;
    
    Configuration config;
    
    config.set("temp", "value");
    assert(config.has("temp"));
    
    bool removed = config.remove("temp");
    assert(removed);
    assert(!config.has("temp"));
    
    // Remove non-existent key
    bool removedAgain = config.remove("temp");
    assert(!removedAgain);
    
    std::cout << "✓ Configuration remove tests passed" << std::endl;
}

void test_configuration_clear() {
    std::cout << "Testing Configuration clear..." << std::endl;
    
    Configuration config;
    
    config.set("key1", 1);
    config.set("key2", 2);
    config.set("key3", 3);
    
    assert(config.size() == 3);
    
    config.clear();
    assert(config.size() == 0);
    assert(!config.has("key1"));
    assert(!config.has("key2"));
    assert(!config.has("key3"));
    
    std::cout << "✓ Configuration clear tests passed" << std::endl;
}

void test_configuration_get_or_default() {
    std::cout << "Testing Configuration getOrDefault..." << std::endl;
    
    Configuration config;
    
    config.set("existing", 42);
    
    int existingVal = config.getOrDefault<int>("existing", 0);
    assert(existingVal == 42);
    
    int nonExistingVal = config.getOrDefault<int>("non_existing", 100);
    assert(nonExistingVal == 100);
    
    std::cout << "✓ Configuration getOrDefault tests passed" << std::endl;
}

void test_configuration_get_all_keys() {
    std::cout << "Testing Configuration getAllKeys..." << std::endl;
    
    Configuration config;
    
    config.set("key1", 1);
    config.set("key2", 2);
    config.set("key3", 3);
    
    auto keys = config.getAllKeys();
    assert(keys.size() == 3);
    
    std::cout << "✓ Configuration getAllKeys tests passed" << std::endl;
}

void test_configuration_file_io() {
    std::cout << "Testing Configuration file I/O..." << std::endl;
    
    Configuration config;
    
    config.set("file_bool", true);
    config.set("file_int", 123);
    config.set("file_double", 2.718);
    config.set("file_string", "test");
    
    // Save to file
    std::string testFile = "test_config.txt";
    bool saved = config.saveToFile(testFile);
    assert(saved);
    
    // Load into new config
    Configuration config2;
    bool loaded = config2.loadFromFile(testFile);
    assert(loaded);
    
    // Verify loaded values
    assert(config2.getBool("file_bool") == true);
    assert(config2.getInt("file_int") == 123);
    assert(config2.getDouble("file_double") == 2.718);
    assert(config2.getString("file_string") == "test");
    
    // Clean up
    std::filesystem::remove(testFile);
    
    std::cout << "✓ Configuration file I/O tests passed" << std::endl;
}

void test_global_configuration() {
    std::cout << "Testing global configuration..." << std::endl;
    
    Configuration& globalConfig = getGlobalConfiguration();
    
    globalConfig.set("global_test", "value");
    assert(globalConfig.has("global_test"));
    
    auto val = globalConfig.getString("global_test");
    assert(val && *val == "value");
    
    // Clean up
    globalConfig.remove("global_test");
    
    std::cout << "✓ Global configuration tests passed" << std::endl;
}

void test_configuration_thread_safety() {
    std::cout << "Testing Configuration thread safety..." << std::endl;
    
    Configuration config;
    
    // Multiple threads setting values
    std::thread t1([&config]() {
        for (int i = 0; i < 100; ++i) {
            config.set("thread1_" + std::to_string(i), i);
        }
    });
    
    std::thread t2([&config]() {
        for (int i = 0; i < 100; ++i) {
            config.set("thread2_" + std::to_string(i), i);
        }
    });
    
    t1.join();
    t2.join();
    
    // Verify values
    assert(config.size() == 200);
    
    std::cout << "✓ Configuration thread safety tests passed" << std::endl;
}

} // namespace test
} // namespace configuration
} // namespace core
} // namespace poko

int main() {
    std::cout << "=== Core Configuration Unit Tests ===" << std::endl;
    std::cout << std::endl;
    
    poko::core::configuration::test::test_configuration_basics();
    poko::core::configuration::test::test_configuration_defaults();
    poko::core::configuration::test::test_configuration_remove();
    poko::core::configuration::test::test_configuration_clear();
    poko::core::configuration::test::test_configuration_get_or_default();
    poko::core::configuration::test::test_configuration_get_all_keys();
    poko::core::configuration::test::test_configuration_file_io();
    poko::core::configuration::test::test_global_configuration();
    poko::core::configuration::test::test_configuration_thread_safety();
    
    std::cout << std::endl;
    std::cout << "=== All tests passed! ===" << std::endl;
    
    return 0;
}
