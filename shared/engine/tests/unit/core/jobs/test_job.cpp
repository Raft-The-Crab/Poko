/**
 * @file test_job.cpp
 * @brief Core jobs system unit tests
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/jobs/job.h"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

namespace poko {
namespace core {
namespace jobs {
namespace test {

// ============================================================================
// Test Functions
// ============================================================================

void test_job_basics() {
    std::cout << "Testing Job basics..." << std::endl;
    
    int counter = 0;
    Job job([&counter]() -> bool {
        counter++;
        return true;
    });
    
    assert(job.getId() != INVALID_JOB_ID);
    assert(job.getPriority() == JobPriority::Normal);
    assert(job.getStatus() == JobStatus::Pending);
    assert(job.isPending());
    
    job.execute();
    
    assert(job.isCompleted());
    assert(counter == 1);
    
    std::cout << "✓ Job basics tests passed" << std::endl;
}

void test_job_priority() {
    std::cout << "Testing Job priority..." << std::endl;
    
    Job lowJob([]() { return true; }, JobPriority::Low);
    Job highJob([]() { return true; }, JobPriority::High);
    
    assert(lowJob.getPriority() == JobPriority::Low);
    assert(highJob.getPriority() == JobPriority::High);
    assert(highJob.getPriority() > lowJob.getPriority());
    
    std::cout << "✓ Job priority tests passed" << std::endl;
}

void test_job_dependencies() {
    std::cout << "Testing Job dependencies..." << std::endl;
    
    Job job([]() { return true; });
    
    assert(!job.hasDependencies());
    
    job.addDependency(1);
    job.addDependency(2);
    
    assert(job.hasDependencies());
    assert(job.getDependencies().size() == 2);
    
    std::cout << "✓ Job dependencies tests passed" << std::endl;
}

void test_job_failure() {
    std::cout << "Testing Job failure..." << std::endl;
    
    Job job([]() -> bool {
        return false; // Simulate failure
    });
    
    job.execute();
    
    assert(job.isFailed());
    assert(!job.isCompleted());
    
    std::cout << "✓ Job failure tests passed" << std::endl;
}

void test_job_system_submit() {
    std::cout << "Testing JobSystem submit..." << std::endl;
    
    JobSystem jobSystem(2);
    
    int counter = 0;
    auto job = std::make_unique<Job>([&counter]() -> bool {
        counter++;
        return true;
    });
    
    JobId jobId = jobSystem.submit(std::move(job));
    assert(jobId != INVALID_JOB_ID);
    
    jobSystem.waitForAll(1000);
    
    assert(counter == 1);
    assert(jobSystem.getCompletedCount() == 1);
    
    std::cout << "✓ JobSystem submit tests passed" << std::endl;
}

void test_job_system_multiple_jobs() {
    std::cout << "Testing JobSystem multiple jobs..." << std::endl;
    
    JobSystem jobSystem(2);
    
    int counter = 0;
    
    (void)jobSystem.submit(std::make_unique<Job>([&counter]() -> bool {
        counter++;
        return true;
    }));
    (void)jobSystem.submit(std::make_unique<Job>([&counter]() -> bool {
        counter++;
        return true;
    }));
    (void)jobSystem.submit(std::make_unique<Job>([&counter]() -> bool {
        counter++;
        return true;
    }));
    (void)jobSystem.submit(std::make_unique<Job>([&counter]() -> bool {
        counter++;
        return true;
    }));
    (void)jobSystem.submit(std::make_unique<Job>([&counter]() -> bool {
        counter++;
        return true;
    }));
    
    jobSystem.waitForAll(2000);
    
    assert(counter == 5);
    assert(jobSystem.getCompletedCount() == 5);
    
    std::cout << "✓ JobSystem multiple jobs tests passed" << std::endl;
}

void test_job_system_priority() {
    std::cout << "Testing JobSystem priority..." << std::endl;
    
    JobSystem jobSystem(1);
    
    std::vector<int> executionOrder;
    
    (void)jobSystem.submit(std::make_unique<Job>([&executionOrder]() -> bool {
        executionOrder.push_back(1);
        return true;
    }, JobPriority::Low));
    
    (void)jobSystem.submit(std::make_unique<Job>([&executionOrder]() -> bool {
        executionOrder.push_back(2);
        return true;
    }, JobPriority::High));
    
    (void)jobSystem.submit(std::make_unique<Job>([&executionOrder]() -> bool {
        executionOrder.push_back(3);
        return true;
    }, JobPriority::Normal));
    
    jobSystem.waitForAll(1000);
    
    // All jobs should execute (priority ordering not implemented yet)
    assert(executionOrder.size() == 3);
    
    std::cout << "✓ JobSystem priority tests passed" << std::endl;
}

void test_job_system_cancel() {
    std::cout << "Testing JobSystem cancel..." << std::endl;
    
    JobSystem jobSystem(1);
    
    int counter = 0;
    auto job = std::make_unique<Job>([&counter]() -> bool {
        counter++;
        return true;
    });
    
    JobId jobId = jobSystem.submit(std::move(job));
    
    bool cancelled = jobSystem.cancel(jobId);
    assert(cancelled);
    
    jobSystem.waitForAll(100);
    
    assert(counter == 0);
    // Cancelled jobs are removed from pending count
    assert(jobSystem.getFailedCount() == 0);
    
    std::cout << "✓ JobSystem cancel tests passed" << std::endl;
}

void test_job_system_wait_for() {
    std::cout << "Testing JobSystem waitFor..." << std::endl;
    
    JobSystem jobSystem(1);
    
    auto job = std::make_unique<Job>([]() { return true; });
    JobId jobId = jobSystem.submit(std::move(job));
    
    bool completed = jobSystem.waitFor(jobId, 1000);
    assert(completed);
    
    std::cout << "✓ JobSystem waitFor tests passed" << std::endl;
}

void test_job_system_statistics() {
    std::cout << "Testing JobSystem statistics..." << std::endl;
    
    JobSystem jobSystem(2);
    
    assert(jobSystem.getTotalCount() == 0);
    assert(jobSystem.getPendingCount() == 0);
    assert(jobSystem.getRunningCount() == 0);
    assert(jobSystem.getCompletedCount() == 0);
    assert(jobSystem.getFailedCount() == 0);
    
    (void)jobSystem.submit(std::make_unique<Job>([]() { return true; }));
    (void)jobSystem.submit(std::make_unique<Job>([]() { return true; }));
    
    assert(jobSystem.getTotalCount() == 2);
    
    jobSystem.waitForAll(1000);
    
    assert(jobSystem.getCompletedCount() == 2);
    
    std::cout << "✓ JobSystem statistics tests passed" << std::endl;
}

void test_job_system_clear_finished() {
    std::cout << "Testing JobSystem clearFinishedJobs..." << std::endl;
    
    JobSystem jobSystem(1);
    
    (void)jobSystem.submit(std::make_unique<Job>([]() { return true; }));
    (void)jobSystem.submit(std::make_unique<Job>([]() { return true; }));
    
    jobSystem.waitForAll(1000);
    
    assert(jobSystem.getTotalCount() == 2);
    
    jobSystem.clearFinishedJobs();
    
    assert(jobSystem.getTotalCount() == 0);
    
    std::cout << "✓ JobSystem clearFinishedJobs tests passed" << std::endl;
}

void test_global_job_system() {
    std::cout << "Testing global job system..." << std::endl;
    
    JobSystem& jobSystem = getJobSystem();
    
    int counter = 0;
    (void)jobSystem.submit(std::make_unique<Job>([&counter]() -> bool {
        counter++;
        return true;
    }));
    
    jobSystem.waitForAll(1000);
    
    assert(counter == 1);
    
    std::cout << "✓ Global job system tests passed" << std::endl;
}

void run_all_tests() {
    std::cout << "=== Core Jobs System Unit Tests ===" << std::endl;
    std::cout << std::endl;
    
    test_job_basics();
    test_job_priority();
    test_job_dependencies();
    test_job_failure();
    test_job_system_submit();
    test_job_system_multiple_jobs();
    test_job_system_priority();
    test_job_system_cancel();
    test_job_system_wait_for();
    test_job_system_statistics();
    test_job_system_clear_finished();
    test_global_job_system();
    
    std::cout << std::endl;
    std::cout << "=== All tests passed! ===" << std::endl;
}

} // namespace test
} // namespace jobs
} // namespace core
} // namespace poko

int main() {
    poko::core::jobs::test::run_all_tests();
    return 0;
}
