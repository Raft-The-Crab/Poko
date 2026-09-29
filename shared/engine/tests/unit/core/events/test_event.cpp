/**
 * @file test_event.cpp
 * @brief Core events system unit tests
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/events/event.h"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

// Forward declaration of global function
namespace poko {
namespace core {
namespace events {
EventDispatcher& getEventDispatcher();
}
}
}

namespace poko {
namespace core {
namespace events {
namespace test {

// ============================================================================
// Test Event Types
// ============================================================================

REGISTER_EVENT(TestEvent1)
class TestEvent1 : public Event {
public:
    TestEvent1() : Event(EVENT_ID_TestEvent1) {}
    int data = 42;
    
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<TestEvent1>(*this);
    }
};

REGISTER_EVENT(TestEvent2)
class TestEvent2 : public Event {
public:
    TestEvent2() : Event(EVENT_ID_TestEvent2, EventPriority::High) {}
    std::string message = "test";
    
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<TestEvent2>(*this);
    }
};

// ============================================================================
// Test Functions
// ============================================================================

void test_event_basics() {
    std::cout << "Testing Event basics..." << std::endl;
    
    TestEvent1 event;
    assert(event.getId() == EVENT_ID_TestEvent1);
    assert(event.getPriority() == EventPriority::Normal);
    assert(!event.isHandled());
    
    event.markHandled();
    assert(event.isHandled());
    
    event.setTimestamp(12345);
    assert(event.getTimestamp() == 12345);
    
    std::cout << "✓ Event basics tests passed" << std::endl;
}

void test_event_cloning() {
    std::cout << "Testing Event cloning..." << std::endl;
    
    TestEvent1 original;
    original.data = 100;
    
    auto cloned = original.clone();
    assert(cloned != nullptr);
    assert(cloned->getId() == original.getId());
    
    auto* clonedTest = static_cast<TestEvent1*>(cloned.get());
    assert(clonedTest->data == original.data);
    
    std::cout << "✓ Event cloning tests passed" << std::endl;
}

void test_event_priority() {
    std::cout << "Testing Event priority..." << std::endl;
    
    TestEvent1 normalEvent;
    TestEvent2 highEvent;
    
    assert(normalEvent.getPriority() == EventPriority::Normal);
    assert(highEvent.getPriority() == EventPriority::High);
    
    assert(highEvent.getPriority() > normalEvent.getPriority());
    
    std::cout << "✓ Event priority tests passed" << std::endl;
}

void test_event_type_registry() {
    std::cout << "Testing EventTypeRegistry..." << std::endl;
    
    EventId id1 = EventTypeRegistry::getNextId();
    EventId id2 = EventTypeRegistry::getNextId();
    EventId id3 = EventTypeRegistry::getNextId();
    
    assert(id1 < id2);
    assert(id2 < id3);
    assert(id3 == id1 + 2);
    
    EventTypeRegistry::reset();
    EventId id4 = EventTypeRegistry::getNextId();
    assert(id4 == 1);
    
    std::cout << "✓ EventTypeRegistry tests passed" << std::endl;
}

void test_signal_connect() {
    std::cout << "Testing Signal connect..." << std::endl;
    
    Signal signal;
    bool called = false;
    
    size_t id = signal.connect([&called](Event*) {
        called = true;
        return true;
    });
    
    assert(id > 0);
    assert(signal.getConnectionCount() == 1);
    
    TestEvent1 event;
    signal.emit(&event);
    
    assert(called);
    
    std::cout << "✓ Signal connect tests passed" << std::endl;
}

void test_signal_disconnect() {
    std::cout << "Testing Signal disconnect..." << std::endl;
    
    Signal signal;
    bool called = false;
    
    size_t id = signal.connect([&called](Event*) {
        called = true;
        return true;
    });
    
    signal.disconnect(id);
    assert(signal.getConnectionCount() == 0);
    
    TestEvent1 event;
    signal.emit(&event);
    
    assert(!called);
    
    std::cout << "✓ Signal disconnect tests passed" << std::endl;
}

void test_signal_multiple_connections() {
    std::cout << "Testing Signal multiple connections..." << std::endl;
    
    Signal signal;
    int callCount = 0;
    
    (void)signal.connect([&callCount](Event*) { callCount++; return true; });
    (void)signal.connect([&callCount](Event*) { callCount++; return true; });
    (void)signal.connect([&callCount](Event*) { callCount++; return true; });
    
    assert(signal.getConnectionCount() == 3);
    
    TestEvent1 event;
    size_t handled = signal.emit(&event);
    
    assert(handled == 3);
    assert(callCount == 3);
    
    std::cout << "✓ Signal multiple connections tests passed" << std::endl;
}

void test_signal_disconnect_all() {
    std::cout << "Testing Signal disconnect all..." << std::endl;
    
    Signal signal;
    
    (void)signal.connect([](Event*) { return true; });
    (void)signal.connect([](Event*) { return true; });
    (void)signal.connect([](Event*) { return true; });
    
    assert(signal.getConnectionCount() == 3);
    
    signal.disconnectAll();
    assert(signal.getConnectionCount() == 0);
    
    std::cout << "✓ Signal disconnect all tests passed" << std::endl;
}

void test_event_queue_push_pop() {
    std::cout << "Testing EventQueue push/pop..." << std::endl;
    
    EventQueue queue;
    
    assert(queue.empty());
    assert(queue.size() == 0);
    
    auto event1 = std::make_unique<TestEvent1>();
    queue.push(std::move(event1));
    
    assert(!queue.empty());
    assert(queue.size() == 1);
    
    auto popped = queue.pop();
    assert(popped != nullptr);
    assert(popped->getId() == EVENT_ID_TestEvent1);
    assert(queue.empty());
    
    std::cout << "✓ EventQueue push/pop tests passed" << std::endl;
}

void test_event_queue_priority() {
    std::cout << "Testing EventQueue priority ordering..." << std::endl;
    
    EventQueue queue;
    
    queue.push(std::make_unique<TestEvent1>()); // Normal priority
    queue.push(std::make_unique<TestEvent2>()); // High priority
    queue.push(std::make_unique<TestEvent1>()); // Normal priority
    
    // High priority should come out first
    auto first = queue.pop();
    assert(first->getPriority() == EventPriority::High);
    
    auto second = queue.pop();
    assert(second->getPriority() == EventPriority::Normal);
    
    auto third = queue.pop();
    assert(third->getPriority() == EventPriority::Normal);
    
    assert(queue.empty());
    
    std::cout << "✓ EventQueue priority ordering tests passed" << std::endl;
}

void test_event_queue_clear() {
    std::cout << "Testing EventQueue clear..." << std::endl;
    
    EventQueue queue;
    
    queue.push(std::make_unique<TestEvent1>());
    queue.push(std::make_unique<TestEvent2>());
    queue.push(std::make_unique<TestEvent1>());
    
    assert(queue.size() == 3);
    
    queue.clear();
    
    assert(queue.empty());
    assert(queue.size() == 0);
    
    std::cout << "✓ EventQueue clear tests passed" << std::endl;
}

void test_event_dispatcher_register() {
    std::cout << "Testing EventDispatcher register..." << std::endl;
    
    EventDispatcher dispatcher;
    bool called = false;
    
    size_t id = dispatcher.registerHandler(EVENT_ID_TestEvent1, [&called](Event*) {
        called = true;
        return true;
    });
    
    assert(id > 0);
    
    TestEvent1 event;
    size_t handled = dispatcher.dispatch(&event);
    
    assert(handled == 1);
    assert(called);
    
    std::cout << "✓ EventDispatcher register tests passed" << std::endl;
}

void test_event_dispatcher_dispatch() {
    std::cout << "Testing EventDispatcher dispatch..." << std::endl;
    
    EventDispatcher dispatcher;
    int callCount = 0;
    
    (void)dispatcher.registerHandler(EVENT_ID_TestEvent1, [&callCount](Event*) {
        callCount++;
        return true;
    });
    
    (void)dispatcher.registerHandler(EVENT_ID_TestEvent1, [&callCount](Event*) {
        callCount++;
        return true;
    });
    
    TestEvent1 event;
    size_t handled = dispatcher.dispatch(&event);
    
    assert(handled == 2);
    assert(callCount == 2);
    
    std::cout << "✓ EventDispatcher dispatch tests passed" << std::endl;
}

void test_event_dispatcher_queue() {
    std::cout << "Testing EventDispatcher queue..." << std::endl;
    
    EventDispatcher dispatcher;
    int callCount = 0;
    
    (void)dispatcher.registerHandler(EVENT_ID_TestEvent1, [&callCount](Event*) {
        callCount++;
        return true;
    });
    
    dispatcher.queue(std::make_unique<TestEvent1>());
    dispatcher.queue(std::make_unique<TestEvent1>());
    
    assert(dispatcher.getQueue().size() == 2);
    
    size_t processed = dispatcher.processQueue();
    
    assert(processed == 2);
    assert(callCount == 2);
    assert(dispatcher.getQueue().empty());
    
    std::cout << "✓ EventDispatcher queue tests passed" << std::endl;
}

void test_event_dispatcher_multiple_types() {
    std::cout << "Testing EventDispatcher multiple event types..." << std::endl;
    
    EventDispatcher dispatcher;
    int test1Count = 0;
    int test2Count = 0;
    
    (void)dispatcher.registerHandler(EVENT_ID_TestEvent1, [&test1Count](Event*) {
        test1Count++;
        return true;
    });
    
    (void)dispatcher.registerHandler(EVENT_ID_TestEvent2, [&test2Count](Event*) {
        test2Count++;
        return true;
    });
    
    TestEvent1 event1;
    TestEvent2 event2;
    
    dispatcher.dispatch(&event1);
    dispatcher.dispatch(&event2);
    
    assert(test1Count == 1);
    assert(test2Count == 1);
    
    std::cout << "✓ EventDispatcher multiple event types tests passed" << std::endl;
}

void test_global_event_dispatcher() {
    std::cout << "Testing global event dispatcher..." << std::endl;
    
    EventDispatcher& dispatcher = getEventDispatcher();
    bool called = false;
    
    (void)dispatcher.registerHandler(EVENT_ID_TestEvent1, [&called](Event*) {
        called = true;
        return true;
    });
    
    TestEvent1 event;
    dispatcher.dispatch(&event);
    
    assert(called);
    
    std::cout << "✓ Global event dispatcher tests passed" << std::endl;
}

void run_all_tests() {
    std::cout << "=== Core Events System Unit Tests ===" << std::endl;
    std::cout << std::endl;
    
    test_event_basics();
    test_event_cloning();
    test_event_priority();
    test_event_type_registry();
    test_signal_connect();
    test_signal_disconnect();
    test_signal_multiple_connections();
    test_signal_disconnect_all();
    test_event_queue_push_pop();
    test_event_queue_priority();
    test_event_queue_clear();
    test_event_dispatcher_register();
    test_event_dispatcher_dispatch();
    test_event_dispatcher_queue();
    test_event_dispatcher_multiple_types();
    test_global_event_dispatcher();
    
    std::cout << std::endl;
    std::cout << "=== All tests passed! ===" << std::endl;
}

} // namespace test
} // namespace events
} // namespace core
} // namespace poko

int main() {
    poko::core::events::test::run_all_tests();
    return 0;
}
