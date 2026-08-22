/**
 * @file test_jobs.cpp
 * @brief Basic tests for job system
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "core/jobs/job.h"
#include "core/jobs/job_manager.h"
#include <iostream>
#include <cassert>
#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>
#include <vector>

using namespace Poko;
using namespace Jobs;

void test_job_creation()
{
    std::cout << "Testing job creation..." << std::endl;
    
    Job emptyJob;
    assert(!emptyJob.IsValid());
    
    Job validJob([]() { /* do nothing */ });
    assert(validJob.IsValid());
    
    std::cout << "✓ Job creation test passed" << std::endl;
}

void test_job_execution()
{
    std::cout << "Testing job execution..." << std::endl;
    
    std::atomic<bool> executed(false);
    Job job([&executed]() { executed = true; });
    
    assert(!executed);
    job.Execute();
    assert(executed);
    
    std::cout << "✓ Job execution test passed" << std::endl;
}

void test_job_handle_creation()
{
    std::cout << "Testing job handle creation..." << std::endl;
    
    JobHandle emptyHandle;
    assert(!emptyHandle.IsValid());
    
    // For now, just test that we can create handles
    // The ID-based constructor will be tested when needed
    
    std::cout << "✓ Job handle creation test passed" << std::endl;
}

void test_job_manager_creation()
{
    std::cout << "Testing job manager creation..." << std::endl;
    
    JobManager manager;
    assert(!manager.IsInitialized());
    
    std::cout << "✓ Job manager creation test passed" << std::endl;
}

void test_job_manager_initialization()
{
    std::cout << "Testing job manager initialization..." << std::endl;
    
    JobManager manager;
    JobManagerConfig config;
    config.num_workers = 2;
    manager.Initialize(config);
    
    assert(manager.IsInitialized());
    
    manager.Shutdown();
    assert(!manager.IsInitialized());
    
    std::cout << "✓ Job manager initialization test passed" << std::endl;
}

void test_job_manager_auto_workers()
{
    std::cout << "Testing job manager auto worker detection..." << std::endl;
    
    JobManager manager;
    JobManagerConfig config; // num_workers = 0 means auto-detect
    manager.Initialize(config);
    
    assert(manager.IsInitialized());
    
    manager.Shutdown();
    
    std::cout << "✓ Job manager auto worker detection test passed" << std::endl;
}

void test_job_addition()
{
    std::cout << "Testing job addition..." << std::endl;
    
    JobManager manager;
    JobManagerConfig config;
    config.num_workers = 2;
    manager.Initialize(config);
    
    std::atomic<bool> executed(false);
    Job job([&executed]() { executed = true; });
    
    JobHandle handle = manager.AddJob(job);
    assert(handle.IsValid());
    (void)handle; // Suppress unused warning
    
    manager.WaitForAllJobs();
    assert(executed);
    
    manager.Shutdown();
    
    std::cout << "✓ Job addition test passed" << std::endl;
}

void test_multiple_jobs()
{
    std::cout << "Testing multiple jobs..." << std::endl;
    
    JobManager manager;
    JobManagerConfig config;
    config.num_workers = 4;
    manager.Initialize(config);
    
    std::atomic<int> counter(0);
    const int jobCount = 10;
    
    for (int i = 0; i < jobCount; ++i) {
        Job job([&counter]() { counter++; });
        manager.AddJob(job);
    }
    
    manager.WaitForAllJobs();
    assert(counter == jobCount);
    
    manager.Shutdown();
    
    std::cout << "✓ Multiple jobs test passed" << std::endl;
}

void test_job_pending_count()
{
    std::cout << "Testing job pending count..." << std::endl;
    
    JobManager manager;
    JobManagerConfig config;
    config.num_workers = 2;
    manager.Initialize(config);
    
    assert(manager.GetPendingJobCount() == 0);
    
    // Add some jobs
    for (int i = 0; i < 5; ++i) {
        Job job([]() { /* do nothing */ });
        manager.AddJob(job);
    }
    
    // Jobs should be processed quickly, but we can check that the count was > 0
    // For this test, we'll just verify the method works
    (void)manager.GetPendingJobCount();
    
    manager.WaitForAllJobs();
    assert(manager.GetPendingJobCount() == 0);
    
    manager.Shutdown();
    
    std::cout << "✓ Job pending count test passed" << std::endl;
}

void test_parallel_execution()
{
    std::cout << "Testing parallel execution..." << std::endl;
    
    JobManager manager;
    JobManagerConfig config;
    config.num_workers = 4;
    manager.Initialize(config);
    
    std::atomic<int> counter(0);
    const int jobCount = 20;
    
    for (int i = 0; i < jobCount; ++i) {
        Job job([&counter]() { 
            counter++;
            // Small delay to test parallelism
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        });
        manager.AddJob(job);
    }
    
    manager.WaitForAllJobs();
    
    assert(counter == jobCount);
    
    manager.Shutdown();
    
    std::cout << "✓ Parallel execution test passed" << std::endl;
}

void test_double_initialization()
{
    std::cout << "Testing double initialization prevention..." << std::endl;
    
    JobManager manager;
    JobManagerConfig config;
    config.num_workers = 2;
    assert(manager.Initialize(config) == true);
    assert(manager.Initialize(config) == false); // Should fail
    
    manager.Shutdown();
    
    std::cout << "✓ Double initialization prevention test passed" << std::endl;
}

void test_shutdown_without_init()
{
    std::cout << "Testing shutdown without initialization..." << std::endl;
    
    JobManager manager;
    // Should not crash
    manager.Shutdown();
    
    std::cout << "✓ Shutdown without initialization test passed" << std::endl;
}

void test_job_priorities()
{
    std::cout << "Testing job priorities..." << std::endl;
    
    JobManager manager;
    JobManagerConfig config;
    config.num_workers = 1; // Single worker to ensure ordering
    config.enable_priorities = true;
    manager.Initialize(config);
    
    std::vector<int> executionOrder;
    std::mutex orderMutex;
    
    // Add jobs with different priorities
    Job lowJob([&executionOrder, &orderMutex]() {
        std::lock_guard<std::mutex> lock(orderMutex);
        executionOrder.push_back(1);
    });
    lowJob.SetPriority(JobPriority::Low);
    
    Job highJob([&executionOrder, &orderMutex]() {
        std::lock_guard<std::mutex> lock(orderMutex);
        executionOrder.push_back(2);
    });
    highJob.SetPriority(JobPriority::High);
    
    Job normalJob([&executionOrder, &orderMutex]() {
        std::lock_guard<std::mutex> lock(orderMutex);
        executionOrder.push_back(3);
    });
    normalJob.SetPriority(JobPriority::Normal);
    
    manager.AddJob(lowJob);
    manager.AddJob(highJob);
    manager.AddJob(normalJob);
    
    manager.WaitForAllJobs();
    
    // High priority should execute first
    assert(executionOrder.size() == 3);
    assert(executionOrder[0] == 2); // High priority
    
    manager.Shutdown();
    
    std::cout << "✓ Job priorities test passed" << std::endl;
}

void test_job_completion_tracking()
{
    std::cout << "Testing job completion tracking..." << std::endl;
    
    JobManager manager;
    JobManagerConfig config;
    config.num_workers = 2;
    manager.Initialize(config);
    
    std::atomic<bool> executed(false);
    Job job([&executed]() { executed = true; });
    
    JobHandle handle = manager.AddJob(job);
    assert(handle.IsValid());
    
    // Wait for specific job
    bool completed = manager.WaitForJob(handle, 5000); // 5 second timeout
    (void)completed; // Suppress unused warning
    assert(completed);
    assert(executed);
    
    // Check job state
    assert(manager.GetJobState(handle) == JobState::Completed);
    
    manager.Shutdown();
    
    std::cout << "✓ Job completion tracking test passed" << std::endl;
}

void test_job_cancellation()
{
    std::cout << "Testing job cancellation..." << std::endl;
    
    JobManager manager;
    JobManagerConfig config;
    config.num_workers = 1; // Single worker to control execution
    manager.Initialize(config);
    
    std::atomic<bool> executed(false);
    Job job([&executed]() { 
        executed = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });
    
    JobHandle handle = manager.AddJob(job);
    
    // Cancel immediately (might not work if already started)
    bool cancelled = manager.CancelJob(handle);
    
    manager.WaitForAllJobs();
    
    // Either cancelled or executed (race condition)
    assert(cancelled || executed);
    
    if (cancelled) {
        assert(manager.GetJobState(handle) == JobState::Cancelled);
    }
    
    manager.Shutdown();
    
    std::cout << "✓ Job cancellation test passed" << std::endl;
}

void test_job_timeout()
{
    std::cout << "Testing job timeout..." << std::endl;
    
    JobManager manager;
    JobManagerConfig config;
    config.num_workers = 0; // No workers to simulate timeout
    manager.Initialize(config);
    
    std::atomic<bool> executed(false);
    Job job([&executed]() { executed = true; });
    
    JobHandle handle = manager.AddJob(job);
    
    // Wait with timeout
    bool completed = manager.WaitForJob(handle, 100); // 100ms timeout
    (void)completed; // Suppress unused warning
    assert(!completed); // Should timeout
    assert(!executed);
    
    manager.Shutdown();
    
    std::cout << "✓ Job timeout test passed" << std::endl;
}

void test_clear_pending_jobs()
{
    std::cout << "Testing clear pending jobs..." << std::endl;
    
    JobManager manager;
    JobManagerConfig config;
    config.num_workers = 0; // No workers to keep jobs pending
    manager.Initialize(config);
    
    // Add several jobs
    for (int i = 0; i < 5; ++i) {
        Job job([]() {});
        manager.AddJob(job);
    }
    
    assert(manager.GetPendingJobCount() == 5);
    
    // Clear pending jobs
    manager.ClearPendingJobs();
    
    assert(manager.GetPendingJobCount() == 0);
    
    manager.Shutdown();
    
    std::cout << "✓ Clear pending jobs test passed" << std::endl;
}

void test_running_job_count()
{
    std::cout << "Testing running job count..." << std::endl;
    
    JobManager manager;
    JobManagerConfig config;
    config.num_workers = 2;
    manager.Initialize(config);
    
    assert(manager.GetRunningJobCount() == 0);
    
    // Add a slow job
    std::atomic<bool> started(false);
    std::atomic<bool> finished(false);
    
    Job job([&started, &finished]() {
        started = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        finished = true;
    });
    
    manager.AddJob(job);
    
    // Wait a bit for job to start
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // Check running count (should be 0 or 1 depending on timing)
    size_t running = manager.GetRunningJobCount();
    (void)running; // Just verify the method works
    
    manager.WaitForAllJobs();
    assert(manager.GetRunningJobCount() == 0);
    
    manager.Shutdown();
    
    std::cout << "✓ Running job count test passed" << std::endl;
}

void test_job_failure_handling()
{
    std::cout << "Testing job failure handling..." << std::endl;
    
    JobManager manager;
    JobManagerConfig config;
    config.num_workers = 2;
    manager.Initialize(config);
    
    // Add a job that throws an exception
    Job job([]() {
        throw std::runtime_error("Test exception");
    });
    
    JobHandle handle = manager.AddJob(job);
    
    // Should complete without crashing
    bool completed = manager.WaitForJob(handle, 5000);
    (void)completed; // Suppress unused warning
    assert(completed);
    
    // Job should be marked as failed
    assert(manager.GetJobState(handle) == JobState::Failed);
    
    manager.Shutdown();
    
    std::cout << "✓ Job failure handling test passed" << std::endl;
}

void test_queue_size_limit()
{
    std::cout << "Testing queue size limit..." << std::endl;
    
    JobManager manager;
    JobManagerConfig config;
    config.num_workers = 0; // No workers
    config.max_queue_size = 5;
    manager.Initialize(config);
    
    // Add jobs up to limit
    for (int i = 0; i < 5; ++i) {
        Job job([]() {});
        JobHandle handle = manager.AddJob(job);
        (void)handle; // Suppress unused warning
        assert(handle.IsValid());
    }
    
    // Try to add one more - should fail
    Job extraJob([]() {});
    JobHandle extraHandle = manager.AddJob(extraJob);
    (void)extraHandle; // Suppress unused warning
    assert(!extraHandle.IsValid());
    
    manager.Shutdown();
    
    std::cout << "✓ Queue size limit test passed" << std::endl;
}

int main()
{
    std::cout << "=== Job System Tests ===" << std::endl;
    
    try {
        test_job_creation();
        test_job_execution();
        test_job_handle_creation();
        test_job_manager_creation();
        test_job_manager_initialization();
        test_job_manager_auto_workers();
        test_job_addition();
        test_multiple_jobs();
        test_job_pending_count();
        test_parallel_execution();
        test_double_initialization();
        test_shutdown_without_init();
        test_job_priorities();
        test_job_completion_tracking();
        test_job_cancellation();
        test_job_timeout();
        test_clear_pending_jobs();
        test_running_job_count();
        test_job_failure_handling();
        test_queue_size_limit();
        
        std::cout << "\n=== All job system tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}