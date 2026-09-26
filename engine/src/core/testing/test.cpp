/**
 * @file test.cpp
 * @brief Implementation of the testing framework
 * @author Moby Productions
 * @date 2026
 * @version 0.1.0
 */

#include "core/testing/test.h"
#include <iostream>
#include <iomanip>
#include <map>

namespace poko {
namespace core {
namespace testing {

std::vector<TestRegistration>& get_test_registry() {
    static std::vector<TestRegistration> registry;
    return registry;
}

void register_test(const std::string& suite_name, const std::string& test_name, TestFunc test_func) {
    get_test_registry().push_back({suite_name, test_name, test_func});
}

int run_all_tests() {
    auto& registry = get_test_registry();

    if (registry.empty()) {
        std::cout << "[==========] No tests registered" << std::endl;
        return 0;
    }

    // Group tests by suite
    std::map<std::string, std::vector<TestRegistration>> suites;
    for (const auto& test : registry) {
        suites[test.suite_name].push_back(test);
    }

    int total_failed = 0;
    size_t total_tests = registry.size();

    std::cout << "[==========] Running " << total_tests << " tests from " << suites.size() << " test suites" << std::endl;

    for (const auto& [suite_name, tests] : suites) {
        std::cout << "[----------] Running " << tests.size() << " tests from " << suite_name << std::endl;

        for (const auto& test : tests) {
            std::cout << "[ RUN      ] " << suite_name << "." << test.test_name << std::endl;

            try {
                test.test_func();
                std::cout << "[       OK ] " << suite_name << "." << test.test_name << std::endl;
            } catch (...) {
                std::cout << "[  FAILED  ] " << suite_name << "." << test.test_name << std::endl;
                total_failed++;
            }
        }

        std::cout << "[----------] " << tests.size() << " tests from " << suite_name << " finished" << std::endl;
    }

    std::cout << "[==========] " << total_tests << " tests from " << suites.size()
              << " test suites ran." << std::endl;

    if (total_failed == 0) {
        std::cout << "[  PASSED  ] All tests passed!" << std::endl;
    } else {
        std::cout << "[  FAILED  ] " << total_failed << " test(s) failed." << std::endl;
    }

    return total_failed;
}

} // namespace testing
} // namespace core
} // namespace poko
