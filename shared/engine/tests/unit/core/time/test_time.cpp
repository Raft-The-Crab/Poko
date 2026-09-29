/**
 * @file test_time.cpp
 * @brief Unit tests for core time system
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/time/time.h"
#include <cassert>
#include <iostream>
#include <thread>

namespace poko {
namespace core {
namespace time {
namespace test {

void test_time_point_basics() {
    std::cout << "Testing TimePoint basics..." << std::endl;
    
    // Default constructor
    TimePoint t1;
    assert(t1.nanos == 0);
    assert(t1.toSeconds() == 0.0);
    
    // Construct from nanoseconds using factory method
    TimePoint t2 = TimePoint::fromNanoseconds(1000000000); // 1 second in nanos
    assert(t2.toSeconds() == 1.0);
    
    // Construct from seconds using factory method
    TimePoint t3 = TimePoint::fromSeconds(2.5);
    assert(t3.toSeconds() == 2.5);
    
    std::cout << "✓ TimePoint basics tests passed" << std::endl;
}

void test_time_point_conversions() {
    std::cout << "Testing TimePoint conversions..." << std::endl;
    
    TimePoint t = TimePoint::fromSeconds(1.0);
    
    assert(t.toSeconds() == 1.0);
    assert(t.toMilliseconds() == 1000.0);
    assert(t.toMicroseconds() == 1000000);
    assert(t.toNanoseconds() == 1000000000);
    
    std::cout << "✓ TimePoint conversions tests passed" << std::endl;
}

void test_time_point_comparison() {
    std::cout << "Testing TimePoint comparison..." << std::endl;
    
    TimePoint t1 = TimePoint::fromNanoseconds(100);
    TimePoint t2 = TimePoint::fromNanoseconds(200);
    TimePoint t3 = TimePoint::fromNanoseconds(100);
    
    assert(t1 == t3);
    assert(t1 != t2);
    assert(t1 < t2);
    assert(t2 > t1);
    assert(t1 <= t2);
    assert(t2 >= t1);
    
    std::cout << "✓ TimePoint comparison tests passed" << std::endl;
}

void test_time_point_arithmetic() {
    std::cout << "Testing TimePoint arithmetic..." << std::endl;
    
    TimePoint t1 = TimePoint::fromNanoseconds(100);
    Duration d = Duration::fromNanoseconds(50);
    
    TimePoint t2 = t1 + d;
    assert(t2.nanos == 150);
    
    // TimePoint - Duration is not defined, only TimePoint - TimePoint
    TimePoint t3 = TimePoint::fromNanoseconds(50);
    Duration diff = t1 - t3;
    assert(diff.nanos == 50);
    
    t1 += d;
    assert(t1.nanos == 150);
    
    t1 -= d;
    assert(t1.nanos == 100);
    
    std::cout << "✓ TimePoint arithmetic tests passed" << std::endl;
}

void test_duration_basics() {
    std::cout << "Testing Duration basics..." << std::endl;
    
    // Default constructor
    Duration d1;
    assert(d1.nanos == 0);
    assert(d1.isZero());
    
    // Construct from nanoseconds using factory method
    Duration d2 = Duration::fromNanoseconds(1000000000); // 1 second
    assert(d2.toSeconds() == 1.0);
    
    // Construct from seconds using factory method
    Duration d3 = Duration::fromSeconds(2.5);
    assert(d3.toSeconds() == 2.5);
    
    std::cout << "✓ Duration basics tests passed" << std::endl;
}

void test_duration_factory_methods() {
    std::cout << "Testing Duration factory methods..." << std::endl;
    
    Duration d1 = Duration::fromSeconds(1.0);
    assert(d1.toSeconds() == 1.0);
    
    Duration d2 = Duration::fromMilliseconds(500.0);
    assert(d2.toMilliseconds() == 500.0);
    
    Duration d3 = Duration::fromMicroseconds(1000);
    assert(d3.toMicroseconds() == 1000);
    
    std::cout << "✓ Duration factory methods tests passed" << std::endl;
}

void test_duration_conversions() {
    std::cout << "Testing Duration conversions..." << std::endl;
    
    Duration d = Duration::fromSeconds(1.0);
    
    assert(d.toSeconds() == 1.0);
    assert(d.toMilliseconds() == 1000.0);
    assert(d.toMicroseconds() == 1000000);
    assert(d.toNanoseconds() == 1000000000);
    
    std::cout << "✓ Duration conversions tests passed" << std::endl;
}

void test_duration_checks() {
    std::cout << "Testing Duration checks..." << std::endl;
    
    Duration d1(100);
    Duration d2(0);
    Duration d3(-100);
    
    assert(d1.isPositive());
    assert(d2.isZero());
    assert(d3.isNegative());
    
    std::cout << "✓ Duration checks tests passed" << std::endl;
}

void test_duration_comparison() {
    std::cout << "Testing Duration comparison..." << std::endl;
    
    Duration d1 = Duration::fromNanoseconds(100);
    Duration d2 = Duration::fromNanoseconds(200);
    Duration d3 = Duration::fromNanoseconds(100);
    
    assert(d1 == d3);
    assert(d1 != d2);
    assert(d1 < d2);
    assert(d2 > d1);
    
    std::cout << "✓ Duration comparison tests passed" << std::endl;
}

void test_duration_arithmetic() {
    std::cout << "Testing Duration arithmetic..." << std::endl;
    
    Duration d1 = Duration::fromNanoseconds(100);
    Duration d2 = Duration::fromNanoseconds(50);
    
    Duration d3 = d1 + d2;
    assert(d3.nanos == 150);
    
    Duration d4 = d1 - d2;
    assert(d4.nanos == 50);
    
    Duration d5 = d1 * 2.0;
    assert(d5.nanos == 200);
    
    Duration d6 = d1 / 2.0;
    assert(d6.nanos == 50);
    
    d1 += d2;
    assert(d1.nanos == 150);
    
    d1 -= d2;
    assert(d1.nanos == 100);
    
    std::cout << "✓ Duration arithmetic tests passed" << std::endl;
}

void test_get_current_time() {
    std::cout << "Testing getCurrentTime..." << std::endl;
    
    TimePoint t1 = getCurrentTime();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    TimePoint t2 = getCurrentTime();
    
    assert(t2 > t1);
    assert((t2 - t1).toMilliseconds() >= 10.0);
    
    std::cout << "✓ getCurrentTime tests passed" << std::endl;
}

void test_sleep() {
    std::cout << "Testing sleep..." << std::endl;
    
    TimePoint t1 = getCurrentTime();
    sleep(Duration::fromMilliseconds(100));
    TimePoint t2 = getCurrentTime();
    
    Duration elapsed = t2 - t1;
    assert(elapsed.toMilliseconds() >= 100.0);
    
    std::cout << "✓ sleep tests passed" << std::endl;
}

void test_sleep_zero_duration() {
    std::cout << "Testing sleep with zero duration..." << std::endl;
    
    TimePoint t1 = getCurrentTime();
    sleep(Duration::fromMilliseconds(0));
    TimePoint t2 = getCurrentTime();
    
    // Should return immediately
    Duration elapsed = t2 - t1;
    assert(elapsed.toMilliseconds() < 10); // Should be very fast
    
    std::cout << "✓ sleep zero duration tests passed" << std::endl;
}

void test_sleep_negative_duration() {
    std::cout << "Testing sleep with negative duration..." << std::endl;
    
    TimePoint t1 = getCurrentTime();
    sleep(Duration::fromNanoseconds(-100));
    TimePoint t2 = getCurrentTime();
    
    // Should return immediately
    Duration elapsed = t2 - t1;
    assert(elapsed.toMilliseconds() < 10);
    
    std::cout << "✓ sleep negative duration tests passed" << std::endl;
}

void test_get_elapsed_time() {
    std::cout << "Testing getElapsedTime..." << std::endl;
    
    Duration elapsed1 = getElapsedTime();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    Duration elapsed2 = getElapsedTime();
    
    assert(elapsed2 > elapsed1);
    assert((elapsed2 - elapsed1).toMilliseconds() >= 50.0);
    
    std::cout << "✓ getElapsedTime tests passed" << std::endl;
}

void test_constants() {
    std::cout << "Testing time constants..." << std::endl;
    
    assert(NANOS_PER_MICRO == 1000);
    assert(MICROS_PER_MILLI == 1000);
    assert(MILLIS_PER_SECOND == 1000);
    assert(MICROS_PER_SECOND == 1000000);
    assert(NANOS_PER_SECOND == 1000000000);
    
    std::cout << "✓ Time constants tests passed" << std::endl;
}

void run_all_tests() {
    std::cout << "=== Core Time System Unit Tests ===" << std::endl;
    std::cout << std::endl;
    
    test_time_point_basics();
    test_time_point_conversions();
    test_time_point_comparison();
    test_time_point_arithmetic();
    test_duration_basics();
    test_duration_factory_methods();
    test_duration_conversions();
    test_duration_checks();
    test_duration_comparison();
    test_duration_arithmetic();
    test_get_current_time();
    test_sleep();
    test_sleep_zero_duration();
    test_sleep_negative_duration();
    test_get_elapsed_time();
    test_constants();
    
    std::cout << std::endl;
    std::cout << "=== All tests passed! ===" << std::endl;
}

} // namespace test
} // namespace time
} // namespace core
} // namespace poko

int main() {
    poko::core::time::test::run_all_tests();
    return 0;
}
