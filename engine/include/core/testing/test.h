/**
 * @file test.h
 * @brief Minimal testing framework for Poko Engine
 * @details Provides a simple test framework without external dependencies
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <sstream>
#include <cstdint>

namespace poko {
namespace core {
namespace testing {

/**
 * @brief Simple test function type
 */
using TestFunc = std::function<void()>;

/**
 * @brief Test registration
 */
struct TestRegistration {
    std::string suite_name;
    std::string test_name;
    TestFunc test_func;
};

/**
 * @brief Get all registered tests
 * @return Reference to test registry
 */
std::vector<TestRegistration>& get_test_registry();

/**
 * @brief Register a test
 * @param suite_name Name of the test suite
 * @param test_name Name of the test
 * @param test_func Test function
 */
void register_test(const std::string& suite_name, const std::string& test_name, TestFunc test_func);

/**
 * @brief Run all registered tests
 * @return Number of failed tests
 */
int run_all_tests();

// Assertion macros
#define POKO_ASSERT_TRUE(expr) \
    do { \
        if (!(expr)) { \
            std::cout << "[  FAILED  ] Assertion failed: " << #expr << std::endl; \
            std::cout << "            at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return; \
        } \
    } while(0)

#define POKO_ASSERT_FALSE(expr) \
    do { \
        if (expr) { \
            std::cout << "[  FAILED  ] Assertion failed: " << #expr << " should be false" << std::endl; \
            std::cout << "            at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return; \
        } \
    } while(0)

#define POKO_ASSERT_EQ(expected, actual) \
    do { \
        auto _expected = (expected); \
        auto _actual = (actual); \
        if (_expected != _actual) { \
            std::cout << "[  FAILED  ] Expected: " << _expected << ", Actual: " << _actual << std::endl; \
            std::cout << "            at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return; \
        } \
    } while(0)

#define POKO_ASSERT_NE(val1, val2) \
    do { \
        auto _val1 = (val1); \
        auto _val2 = (val2); \
        if (_val1 == _val2) { \
            std::cout << "[  FAILED  ] Values should not be equal: " << _val1 << " == " << _val2 << std::endl; \
            std::cout << "            at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return; \
        } \
    } while(0)

#define POKO_ASSERT_LT(val1, val2) \
    do { \
        auto _val1 = (val1); \
        auto _val2 = (val2); \
        if (!(_val1 < _val2)) { \
            std::cout << "[  FAILED  ] " << _val1 << " should be less than " << _val2 << std::endl; \
            std::cout << "            at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return; \
        } \
    } while(0)

#define POKO_ASSERT_GT(val1, val2) \
    do { \
        auto _val1 = (val1); \
        auto _val2 = (val2); \
        if (!(_val1 > _val2)) { \
            std::cout << "[  FAILED  ] " << _val1 << " should be greater than " << _val2 << std::endl; \
            std::cout << "            at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return; \
        } \
    } while(0)

#define POKO_ASSERT_NULL(ptr) \
    do { \
        if ((ptr) != nullptr) { \
            std::cout << "[  FAILED  ] Expected null, got: " << (ptr) << std::endl; \
            std::cout << "            at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return; \
        } \
    } while(0)

#define POKO_ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == nullptr) { \
            std::cout << "[  FAILED  ] Expected non-null" << std::endl; \
            std::cout << "            at " << __FILE__ << ":" << __LINE__ << std::endl; \
            return; \
        } \
    } while(0)

/**
 * @brief Helper macro to define a test
 * @param suite_name Name of the test suite
 * @param test_name Name of the test
 */
#define POKO_TEST(suite_name, test_name) \
    void test_##suite_name##_##test_name(); \
    \
    struct TestRegistrar_##suite_name##_##test_name { \
        TestRegistrar_##suite_name##_##test_name() { \
            poko::core::testing::register_test(#suite_name, #test_name, test_##suite_name##_##test_name); \
        } \
    }; \
    static TestRegistrar_##suite_name##_##test_name registrar_##suite_name##_##test_name; \
    \
    void test_##suite_name##_##test_name()

} // namespace testing
} // namespace core
} // namespace poko
