/**
 * @file test_mute_engine_adapter.cpp
 * @brief Integration test for Mute Engine Adapter
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "binding/mute_engine_adapter.h"
#include "runtime/instance.h"
#include "runtime/world.h"
#include "physics/physics.h"
#include "audio/audio.h"
#include "core/testing/test.h"
#include <cassert>
#include <iostream>

namespace poko {
namespace test {

// Test fixture for Mute Engine Adapter tests
class MuteEngineAdapterTest {
public:
    MuteEngineAdapter adapter;
    runtime::World* world;
    
    void setup() {
        // Create a dummy world with a name
        world = new runtime::World("TestWorld");
        
        // Initialize the adapter
        bool success = mute_engine_adapter_init(&adapter, world);
        assert(success && "Failed to initialize adapter");
    }
    
    void teardown() {
        mute_engine_adapter_free(&adapter);
        delete world;
    }
};

// Test adapter initialization
void test_adapter_initialization() {
    MuteEngineAdapterTest test;
    test.setup();
    
    assert(test.adapter.is_initialized == true);
    assert(test.adapter.engine_handle != nullptr);
    assert(test.adapter.sandbox_state != nullptr);
    assert(test.adapter.api_state != nullptr);
    
    test.teardown();
    std::cout << "✓ test_adapter_initialization passed\n";
}

// Test context switching
void test_context_switching() {
    MuteEngineAdapterTest test;
    test.setup();
    
    // Test setting context
    mute_engine_adapter_set_context(&test.adapter, MUTE_CONTEXT_SERVER);
    assert(mute_engine_adapter_get_context(&test.adapter) == MUTE_CONTEXT_SERVER);
    
    mute_engine_adapter_set_context(&test.adapter, MUTE_CONTEXT_CLIENT);
    assert(mute_engine_adapter_get_context(&test.adapter) == MUTE_CONTEXT_CLIENT);
    
    test.teardown();
    std::cout << "✓ test_context_switching passed\n";
}

// Test API availability checks
void test_api_availability() {
    MuteEngineAdapterTest test;
    test.setup();
    
    // Test client context
    mute_engine_adapter_set_context(&test.adapter, MUTE_CONTEXT_CLIENT);
    assert(mute_engine_adapter_is_available(&test.adapter, ENGINE_API_PHYSICS) == false);
    assert(mute_engine_adapter_is_available(&test.adapter, ENGINE_API_UI) == true);
    assert(mute_engine_adapter_is_available(&test.adapter, ENGINE_API_AUDIO) == true);
    
    // Test server context
    mute_engine_adapter_set_context(&test.adapter, MUTE_CONTEXT_SERVER);
    assert(mute_engine_adapter_is_available(&test.adapter, ENGINE_API_PHYSICS) == true);
    assert(mute_engine_adapter_is_available(&test.adapter, ENGINE_API_UI) == false);
    assert(mute_engine_adapter_is_available(&test.adapter, ENGINE_API_AUTHORITY) == true);
    
    // Test shared context
    mute_engine_adapter_set_context(&test.adapter, MUTE_CONTEXT_SHARED);
    assert(mute_engine_adapter_is_available(&test.adapter, ENGINE_API_PHYSICS) == true);
    assert(mute_engine_adapter_is_available(&test.adapter, ENGINE_API_UI) == true);
    
    test.teardown();
    std::cout << "✓ test_api_availability passed\n";
}

// Test handle retrieval
void test_handle_retrieval() {
    MuteEngineAdapterTest test;
    test.setup();
    
    void* handle = mute_engine_adapter_get_handle(&test.adapter, ENGINE_API_PHYSICS);
    assert(handle == test.world);
    
    test.teardown();
    std::cout << "✓ test_handle_retrieval passed\n";
}

// Run all tests
void run_all_mute_engine_adapter_tests() {
    std::cout << "\n=== Running Mute Engine Adapter Tests ===\n";
    
    test_adapter_initialization();
    test_context_switching();
    test_api_availability();
    test_handle_retrieval();
    
    std::cout << "=== All Mute Engine Adapter Tests Passed ===\n\n";
}

} // namespace test
} // namespace poko

// Main test entry point
int main() {
    poko::test::run_all_mute_engine_adapter_tests();
    return 0;
}
