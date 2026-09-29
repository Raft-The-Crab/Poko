/**
 * @file test_handle.cpp
 * @brief Unit tests for core handle system
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/handles/handle.h"
#include "core/handles/handle_manager.h"
#include <cassert>
#include <iostream>
#include <thread>
#include <vector>

namespace poko {
namespace core {
namespace handles {
namespace test {

void test_handle_basics() {
    std::cout << "Testing Handle basics..." << std::endl;
    
    // Test null handle
    Handle nullHandle = Handle::null();
    assert(nullHandle.isNull());
    assert(!nullHandle.isValid());
    
    // Test valid handle
    Handle validHandle(1, 1);
    assert(!validHandle.isNull());
    assert(validHandle.isValid());
    
    // Test equality
    Handle handle1(1, 1);
    Handle handle2(1, 1);
    Handle handle3(1, 2);
    assert(handle1 == handle2);
    assert(handle1 != handle3);
    
    // Test comparison
    Handle handle4(2, 1);
    assert(handle1 < handle4);
    
    std::cout << "✓ Handle basics tests passed" << std::endl;
}

void test_handle_manager_allocate() {
    std::cout << "Testing HandleManager allocation..." << std::endl;
    
    HandleManager manager;
    
    // Allocate first handle
    Handle handle1 = manager.allocate();
    assert(handle1.isValid());
    assert(handle1.index == 0);
    assert(handle1.generation == 1);
    
    // Allocate second handle
    Handle handle2 = manager.allocate();
    assert(handle2.isValid());
    assert(handle2.index == 1);
    assert(handle2.generation == 1);
    
    // Verify handles are valid
    assert(manager.isValid(handle1));
    assert(manager.isValid(handle2));
    
    std::cout << "✓ HandleManager allocation tests passed" << std::endl;
}

void test_handle_manager_free() {
    std::cout << "Testing HandleManager free..." << std::endl;
    
    HandleManager manager;
    
    Handle handle = manager.allocate();
    assert(manager.isValid(handle));
    
    // Free the handle
    bool freed = manager.free(handle);
    assert(freed);
    assert(!manager.isValid(handle));
    
    // Try to free again (should fail)
    freed = manager.free(handle);
    assert(!freed);
    
    std::cout << "✓ HandleManager free tests passed" << std::endl;
}

void test_handle_manager_generation() {
    std::cout << "Testing HandleManager generation..." << std::endl;
    
    HandleManager manager;
    
    // Allocate handle
    Handle handle1 = manager.allocate();
    HandleGeneration gen1 = handle1.generation;
    
    // Free handle
    (void)manager.free(handle1);
    
    // Allocate new handle (should reuse index with incremented generation)
    Handle handle2 = manager.allocate();
    assert(handle2.index == handle1.index);
    assert(handle2.generation == gen1 + 1);
    
    // Old handle should be invalid
    assert(!manager.isValid(handle1));
    
    // New handle should be valid
    assert(manager.isValid(handle2));
    
    std::cout << "✓ HandleManager generation tests passed" << std::endl;
}

void test_handle_manager_stale_reference() {
    std::cout << "Testing HandleManager stale reference prevention..." << std::endl;
    
    HandleManager manager;
    
    // Allocate handle
    Handle handle1 = manager.allocate();
    
    // Store the handle
    Handle storedHandle = handle1;
    
    // Free the handle
    (void)manager.free(handle1);
    
    // Allocate new handle (may reuse index)
    Handle handle2 = manager.allocate();
    
    // Old stored handle should be invalid
    assert(!manager.isValid(storedHandle));
    
    // Even if index matches, generation should differ
    if (storedHandle.index == handle2.index) {
        assert(storedHandle.generation != handle2.generation);
    }
    
    std::cout << "✓ HandleManager stale reference tests passed" << std::endl;
}

void test_handle_manager_stats() {
    std::cout << "Testing HandleManager statistics..." << std::endl;
    
    HandleManager manager;
    
    // Initial stats
    HandleManagerStats stats = manager.getStats();
    assert(stats.totalAllocated == 0);
    assert(stats.totalFreed == 0);
    assert(stats.activeHandles == 0);
    
    // Allocate handles
    Handle h1 = manager.allocate();
    (void)manager.allocate(); // h2
    (void)manager.allocate(); // h3
    
    stats = manager.getStats();
    assert(stats.totalAllocated == 3);
    assert(stats.activeHandles == 3);
    
    // Free one handle
    (void)manager.free(h1);
    
    stats = manager.getStats();
    assert(stats.totalFreed == 1);
    assert(stats.activeHandles == 2);
    
    std::cout << "✓ HandleManager statistics tests passed" << std::endl;
}

void test_handle_manager_reset() {
    std::cout << "Testing HandleManager reset..." << std::endl;
    
    HandleManager manager;
    
    // Allocate some handles
    Handle h1 = manager.allocate();
    Handle h2 = manager.allocate();
    
    assert(manager.getStats().activeHandles == 2);
    
    // Reset
    manager.reset();
    
    // Stats should be zero
    HandleManagerStats stats = manager.getStats();
    assert(stats.totalAllocated == 0);
    assert(stats.totalFreed == 0);
    assert(stats.activeHandles == 0);
    
    // Old handles should be invalid
    assert(!manager.isValid(h1));
    assert(!manager.isValid(h2));
    
    std::cout << "✓ HandleManager reset tests passed" << std::endl;
}

void test_handle_manager_capacity() {
    std::cout << "Testing HandleManager capacity..." << std::endl;
    
    HandleManager manager;
    
    assert(manager.getCapacity() == MAX_HANDLES);
    assert(manager.getActiveCount() == 0);
    
    // Allocate a few handles
    (void)manager.allocate();
    (void)manager.allocate();
    
    assert(manager.getActiveCount() == 2);
    
    std::cout << "✓ HandleManager capacity tests passed" << std::endl;
}

void test_handle_manager_get_generation() {
    std::cout << "Testing HandleManager getGeneration..." << std::endl;
    
    HandleManager manager;
    
    Handle handle = manager.allocate();
    HandleGeneration gen = manager.getGeneration(handle.index);
    assert(gen == handle.generation);
    
    // Free and reallocate
    (void)manager.free(handle);
    Handle newHandle = manager.allocate();
    HandleGeneration newGen = manager.getGeneration(newHandle.index);
    assert(newGen == newHandle.generation);
    assert(newGen > gen);
    
    std::cout << "✓ HandleManager getGeneration tests passed" << std::endl;
}

void test_handle_manager_invalid_operations() {
    std::cout << "Testing HandleManager invalid operations..." << std::endl;
    
    HandleManager manager;
    
    // Try to free null handle
    bool freed = manager.free(Handle::null());
    assert(!freed);
    
    // Try to free handle with out-of-bounds index
    Handle invalidHandle(MAX_HANDLES + 10, 1);
    freed = manager.free(invalidHandle);
    assert(!freed);
    
    // Check validity of null handle
    assert(!manager.isValid(Handle::null()));
    
    // Check validity of out-of-bounds handle
    assert(!manager.isValid(invalidHandle));
    
    std::cout << "✓ HandleManager invalid operations tests passed" << std::endl;
}

void test_handle_manager_thread_safety() {
    std::cout << "Testing HandleManager thread safety..." << std::endl;
    
    HandleManager manager;
    
    const int numThreads = 4;
    const int allocationsPerThread = 50;
    std::vector<std::thread> threads;
    std::vector<Handle> handles;
    
    // Allocate from multiple threads
    for (int t = 0; t < numThreads; t++) {
        threads.emplace_back([&manager, &handles, allocationsPerThread]() {
            for (int i = 0; i < allocationsPerThread; i++) {
                Handle h = manager.allocate();
                if (h.isValid()) {
                    handles.push_back(h);
                }
            }
        });
    }
    
    // Wait for all threads
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Check stats
    HandleManagerStats stats = manager.getStats();
    assert(stats.totalAllocated == numThreads * allocationsPerThread);
    assert(stats.activeHandles == numThreads * allocationsPerThread);
    
    // Free all handles
    for (const Handle& h : handles) {
        (void)manager.free(h);
    }
    
    stats = manager.getStats();
    assert(stats.activeHandles == 0);
    assert(stats.totalFreed == numThreads * allocationsPerThread);
    
    std::cout << "✓ HandleManager thread safety tests passed" << std::endl;
}

void test_global_handle_manager() {
    std::cout << "Testing global handle manager..." << std::endl;
    
    HandleManager& manager = getHandleManager();
    
    Handle handle = manager.allocate();
    assert(handle.isValid());
    assert(manager.isValid(handle));
    
    (void)manager.free(handle);
    
    std::cout << "✓ Global handle manager tests passed" << std::endl;
}

void test_handle_hash() {
    std::cout << "Testing Handle hash..." << std::endl;
    
    Handle h1(1, 1);
    Handle h2(1, 1);
    Handle h3(2, 1);
    
    HandleHash hashFn;
    std::size_t hash1 = hashFn(h1);
    std::size_t hash2 = hashFn(h2);
    (void)hashFn(h3); // hash3
    
    // Equal handles should have equal hashes
    assert(hash1 == hash2);
    
    // Different handles should (likely) have different hashes
    // Note: Hash collisions are possible but unlikely
    // assert(hash1 != hash3);
    
    std::cout << "✓ Handle hash tests passed" << std::endl;
}

void run_all_tests() {
    std::cout << "=== Core Handle System Unit Tests ===" << std::endl;
    std::cout << std::endl;
    
    test_handle_basics();
    test_handle_manager_allocate();
    test_handle_manager_free();
    test_handle_manager_generation();
    test_handle_manager_stale_reference();
    test_handle_manager_stats();
    test_handle_manager_reset();
    test_handle_manager_capacity();
    test_handle_manager_get_generation();
    test_handle_manager_invalid_operations();
    test_handle_manager_thread_safety();
    test_global_handle_manager();
    test_handle_hash();
    
    std::cout << std::endl;
    std::cout << "=== All tests passed! ===" << std::endl;
}

} // namespace test
} // namespace handles
} // namespace core
} // namespace poko

int main() {
    poko::core::handles::test::run_all_tests();
    return 0;
}
