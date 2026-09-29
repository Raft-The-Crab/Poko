/**
 * @file job.h
 * @brief Core jobs system main header
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 * 
 * This module provides the job system for the Poko Engine including
 * parallel task execution, worker threads, job scheduling, and
 * dependency management.
 */

#ifndef POKO_CORE_JOBS_JOB_H
#define POKO_CORE_JOBS_JOB_H

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <condition_variable>
#include <queue>

namespace poko {
namespace core {
namespace jobs {

// ============================================================================
// Type Definitions
// ============================================================================

/**
 * @brief Unique identifier for jobs
 */
using JobId = uint64_t;

/**
 * @brief Priority for job execution
 */
enum class JobPriority : uint8_t {
    Low = 0,        ///< Low priority (background tasks)
    Normal = 1,     ///< Normal priority (standard tasks)
    High = 2,       ///< High priority (important tasks)
    Critical = 3    ///< Critical priority (immediate execution)
};

/**
 * @brief Job execution status
 */
enum class JobStatus : uint8_t {
    Pending = 0,    ///< Job is waiting to be executed
    Running = 1,    ///< Job is currently executing
    Completed = 2,  ///< Job finished successfully
    Failed = 3,     ///< Job failed during execution
    Cancelled = 4   ///< Job was cancelled
};

// ============================================================================
// Constants
// ============================================================================

/// Invalid job ID
constexpr JobId INVALID_JOB_ID = 0;

/// Maximum number of worker threads (adjust based on hardware)
constexpr size_t MAX_WORKER_THREADS = 16;

/// Default number of worker threads
constexpr size_t DEFAULT_WORKER_THREADS = 4;

// ============================================================================
// Job Task
// ============================================================================

/**
 * @brief Function signature for job tasks
 * 
 * Job tasks can return a status or void.
 * Returning false indicates failure.
 */
using JobTask = std::function<bool()>;

// ============================================================================
// Job
// ============================================================================

/**
 * @brief Represents a unit of work to be executed
 * 
 * Jobs contain a task function, priority, and status tracking.
 * They can have dependencies on other jobs.
 * 
 * @section thread_safety Thread Safety
 * Jobs themselves are not thread-safe after creation.
 * The job system provides thread-safe job management.
 */
class Job {
public:
    /**
     * @brief Constructor
     * @param task Function to execute
     * @param priority Job priority
     */
    Job(JobTask task, JobPriority priority = JobPriority::Normal);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~Job() = default;
    
    /**
     * @brief Get job ID
     * @return Unique job identifier
     */
    [[nodiscard]] JobId getId() const noexcept { return m_id; }
    
    /**
     * @brief Get job priority
     * @return Job priority
     */
    [[nodiscard]] JobPriority getPriority() const noexcept { return m_priority; }
    
    /**
     * @brief Get job status
     * @return Current job status
     */
    [[nodiscard]] JobStatus getStatus() const noexcept { return m_status; }
    
    /**
     * @brief Set job status
     * @param status New status
     */
    void setStatus(JobStatus status) noexcept { m_status = status; }
    
    /**
     * @brief Check if job is pending
     * @return True if job is pending
     */
    [[nodiscard]] bool isPending() const noexcept { return m_status == JobStatus::Pending; }
    
    /**
     * @brief Check if job is running
     * @return True if job is running
     */
    [[nodiscard]] bool isRunning() const noexcept { return m_status == JobStatus::Running; }
    
    /**
     * @brief Check if job is completed
     * @return True if job completed successfully
     */
    [[nodiscard]] bool isCompleted() const noexcept { return m_status == JobStatus::Completed; }
    
    /**
     * @brief Check if job failed
     * @return True if job failed
     */
    [[nodiscard]] bool isFailed() const noexcept { return m_status == JobStatus::Failed; }
    
    /**
     * @brief Check if job is cancelled
     * @return True if job was cancelled
     */
    [[nodiscard]] bool isCancelled() const noexcept { return m_status == JobStatus::Cancelled; }
    
    /**
     * @brief Execute the job task
     * @return True if task succeeded, false if failed
     */
    bool execute();
    
    /**
     * @brief Add dependency on another job
     * @param jobId Job ID to depend on
     */
    void addDependency(JobId jobId);
    
    /**
     * @brief Get job dependencies
     * @return Vector of dependency job IDs
     */
    [[nodiscard]] const std::vector<JobId>& getDependencies() const noexcept { return m_dependencies; }
    
    /**
     * @brief Check if job has dependencies
     * @return True if job has dependencies
     */
    [[nodiscard]] bool hasDependencies() const noexcept { return !m_dependencies.empty(); }
    
protected:
    JobId m_id;                          ///< Unique job ID
    JobPriority m_priority;              ///< Job priority
    JobStatus m_status;                  ///< Job status
    JobTask m_task;                      ///< Task function
    std::vector<JobId> m_dependencies;   ///< Job dependencies
    
private:
    static std::atomic<JobId> s_nextJobId;
};

// ============================================================================
// Job System
// ============================================================================

/**
 * @brief Job scheduler and worker pool
 * 
 * Manages job execution across multiple worker threads.
 * Supports job priorities, dependencies, and cancellation.
 * 
 * @section thread_safety Thread Safety
 * - All public methods are thread-safe
 * - Job submission is thread-safe
 * - Worker pool management is thread-safe
 * 
 * @section performance Performance
 * - Job submission: O(log n) for priority queue insertion
 * - Job execution: O(1) per available worker
 * - Dependency checking: O(d) where d is number of dependencies
 */
class JobSystem {
public:
    /**
     * @brief Constructor
     * @param numWorkers Number of worker threads (0 = use hardware concurrency)
     */
    explicit JobSystem(size_t numWorkers = DEFAULT_WORKER_THREADS);
    
    /**
     * @brief Destructor
     * 
     * Shuts down all workers and waits for completion.
     */
    ~JobSystem();
    
    /**
     * @brief Copy constructor (deleted)
     */
    JobSystem(const JobSystem&) = delete;
    
    /**
     * @brief Copy assignment (deleted)
     */
    JobSystem& operator=(const JobSystem&) = delete;
    
    /**
     * @brief Move constructor (deleted - job system must remain in place)
     */
    JobSystem(JobSystem&&) = delete;
    
    /**
     * @brief Move assignment (deleted - job system must remain in place)
     */
    JobSystem& operator=(JobSystem&&) = delete;
    
    /**
     * @brief Submit a job for execution
     * 
     * @param job Job to execute (takes ownership)
     * @return Job ID
     * 
     * @note Thread-safe
     * @note Job will be queued and executed when dependencies are satisfied
     */
    [[nodiscard]] JobId submit(std::unique_ptr<Job> job);
    
    /**
     * @brief Cancel a job
     * 
     * @param jobId Job ID to cancel
     * @return True if job was found and cancelled
     * 
     * @note Thread-safe
     * @note Cannot cancel jobs that are already running
     */
    bool cancel(JobId jobId) noexcept;
    
    /**
     * @brief Wait for a job to complete
     * 
     * @param jobId Job ID to wait for
     * @param timeoutMs Timeout in milliseconds (0 = wait forever)
     * @return True if job completed, false if timeout
     * 
     * @note Thread-safe
     */
    bool waitFor(JobId jobId, uint32_t timeoutMs = 0);
    
    /**
     * @brief Wait for all jobs to complete
     * 
     * @param timeoutMs Timeout in milliseconds (0 = wait forever)
     * @return True if all jobs completed, false if timeout
     * 
     * @note Thread-safe
     */
    bool waitForAll(uint32_t timeoutMs = 0);
    
    /**
     * @brief Get job status
     * 
     * @param jobId Job ID to check
     * @return Job status, or JobStatus::Failed if job not found
     * 
     * @note Thread-safe
     */
    [[nodiscard]] JobStatus getJobStatus(JobId jobId) const;
    
    /**
     * @brief Get number of pending jobs
     * @return Pending job count
     * 
     * @note Thread-safe
     */
    [[nodiscard]] size_t getPendingCount() const;
    
    /**
     * @brief Get number of running jobs
     * @return Running job count
     * 
     * @note Thread-safe
     */
    [[nodiscard]] size_t getRunningCount() const;
    
    /**
     * @brief Get number of completed jobs
     * @return Completed job count
     * 
     * @note Thread-safe
     */
    [[nodiscard]] size_t getCompletedCount() const;
    
    /**
     * @brief Get number of failed jobs
     * @return Failed job count
     * 
     * @note Thread-safe
     */
    [[nodiscard]] size_t getFailedCount() const;
    
    /**
     * @brief Get number of cancelled jobs
     * @return Cancelled job count
     * 
     * @note Thread-safe
     */
    [[nodiscard]] size_t getCancelledCount() const;
    
    /**
     * @brief Get total number of jobs submitted
     * @return Total job count
     * 
     * @note Thread-safe
     */
    [[nodiscard]] size_t getTotalCount() const;
    
    /**
     * @brief Clear all completed and failed jobs
     * 
     * @note Thread-safe
     * @note Does not affect pending or running jobs
     */
    void clearFinishedJobs() noexcept;
    
    /**
     * @brief Shutdown the job system
     * 
     * Stops accepting new jobs and waits for all workers to finish.
     * 
     * @note Thread-safe
     * @note Once shutdown, the system cannot be restarted
     */
    void shutdown();
    
    /**
     * @brief Check if system is running
     * @return True if system is running
     */
    [[nodiscard]] bool isRunning() const;
    
private:
    struct JobEntry {
        std::unique_ptr<Job> job;
        std::condition_variable* condition;
    };
    
    void workerThread(size_t workerId);
    [[nodiscard]] bool canExecuteJob(Job* job) const;
    void processJob(Job* job);
    
    mutable std::mutex m_mutex;
    std::condition_variable m_condVar;
    
    std::vector<std::thread> m_workers;
    std::vector<JobEntry> m_jobs;
    std::queue<size_t> m_pendingQueue;  // Indices into m_jobs
    
    std::atomic<bool> m_running{true};
    std::atomic<bool> m_shutdown{false};
    
    std::atomic<size_t> m_pendingCount{0};
    std::atomic<size_t> m_runningCount{0};
    std::atomic<size_t> m_completedCount{0};
    std::atomic<size_t> m_failedCount{0};
};

// ============================================================================
// Global Job System
// ============================================================================

/**
 * @brief Global job system instance
 * 
 * Points to the globally shared JobSystem instance.
 * Used throughout the engine for parallel task execution.
 */
extern JobSystem* g_jobSystem;

/**
 * @brief Get global job system
 * 
 * Returns a reference to the globally shared JobSystem instance.
 * This system is intended for general-purpose job execution throughout the engine.
 * 
 * @return Reference to global job system
 * 
 * @note Thread-safe access is guaranteed
 * @note Do not destroy this system - it's globally managed
 */
JobSystem& getJobSystem();

} // namespace jobs
} // namespace core
} // namespace poko

#endif // POKO_CORE_JOBS_JOB_H
