/**
 * @file job_manager.h
 * @brief Job manager header for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#pragma once

#include "core/jobs/job.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <vector>
#include <unordered_map>
#include <memory>

namespace Poko {
namespace Jobs {

/**
 * @brief Job manager configuration
 */
struct JobManagerConfig {
    size_t num_workers = 0; // 0 = auto-detect
    size_t max_queue_size = 1000; // Maximum pending jobs
    bool enable_priorities = true; // Enable priority queue
    bool wait_for_running_on_shutdown = true; // Wait for running jobs to complete on shutdown
    size_t completion_cleanup_threshold = 1000; // Clean up completion records after this many completions
};

/**
 * @brief Priority queue comparator for jobs
 */
struct JobComparator {
    bool operator()(const std::pair<Job, uint32_t>& a, 
                   const std::pair<Job, uint32_t>& b) const {
        return a.first.GetPriority() < b.first.GetPriority();
    }
};

/**
 * @brief Manages job execution across worker threads
 */
class JobManager {
public:
    JobManager();
    ~JobManager();

    /**
     * @brief Initialize the job manager with specified configuration
     * @param config Job manager configuration
     * @return true if initialization succeeded
     */
    bool Initialize(const JobManagerConfig& config = JobManagerConfig{});

    /**
     * @brief Shutdown the job manager and wait for all jobs to complete
     */
    void Shutdown();

    /**
     * @brief Add a job to the queue
     * @param job Job to execute
     * @return Job handle for tracking
     */
    JobHandle AddJob(Job job);

    /**
     * @brief Wait for a specific job to complete
     * @param handle Job handle to wait for
     * @param timeout_ms Timeout in milliseconds (0 = infinite)
     * @return true if job completed, false if timeout
     */
    bool WaitForJob(JobHandle handle, uint32_t timeout_ms = 0);

    /**
     * @brief Cancel a job
     * @param handle Job handle to cancel
     * @return true if job was cancelled, false if not found or already completed
     */
    bool CancelJob(JobHandle handle);

    /**
     * @brief Wait for all jobs to complete
     * @param timeout_ms Timeout in milliseconds (0 = infinite)
     * @return true if all jobs completed, false if timeout
     */
    bool WaitForAllJobs(uint32_t timeout_ms = 0);

    /**
     * @brief Get number of pending jobs
     * @return Number of jobs in queue
     */
    size_t GetPendingJobCount() const;

    /**
     * @brief Get number of running jobs
     * @return Number of jobs currently executing
     */
    size_t GetRunningJobCount() const;

    /**
     * @brief Get job state
     * @param handle Job handle
     * @return Job state
     */
    JobState GetJobState(JobHandle handle) const;

    /**
     * @brief Check if job manager is initialized
     * @return true if initialized
     */
    bool IsInitialized() const;

    /**
     * @brief Clear all pending jobs (does not cancel running jobs)
     */
    void ClearPendingJobs();

private:
    void WorkerThread();
    bool GetNextJob(Job& out_job, uint32_t& out_job_id);
    void MarkJobComplete(uint32_t job_id, bool success);
    std::shared_ptr<JobCompletion> GetJobCompletion(uint32_t job_id) const;
    void CleanupOldCompletions();

    std::priority_queue<std::pair<Job, uint32_t>, 
                      std::vector<std::pair<Job, uint32_t>>, 
                      JobComparator> m_priorityQueue;
    std::queue<std::pair<Job, uint32_t>> m_fifoQueue;
    
    mutable std::mutex m_queueMutex;
    std::condition_variable m_queueCondition;
    
    std::vector<std::thread> m_workerThreads;
    std::atomic<bool> m_shouldStop;
    std::atomic<bool> m_isInitialized;
    
    std::atomic<uint32_t> m_nextJobID;
    std::atomic<size_t> m_pendingJobs;
    std::atomic<size_t> m_runningJobs;
    
    JobManagerConfig m_config;
    
    mutable std::mutex m_completionMutex;
    std::unordered_map<uint32_t, std::shared_ptr<JobCompletion>> m_jobCompletions;
    std::atomic<uint32_t> m_totalCompletions; // Track for cleanup
    std::vector<uint32_t> m_completedJobIDs; // Track completed job IDs for cleanup
};

} // namespace Jobs
} // namespace Poko