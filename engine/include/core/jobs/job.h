/**
 * @file job.h
 * @brief Job system header for PokoEngine
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#pragma once

#include <functional>
#include <memory>
#include <cstdint>
#include <atomic>
#include <condition_variable>
#include <mutex>

namespace Poko {
namespace Jobs {

/**
 * @brief Job priority levels
 */
enum class JobPriority : uint8_t {
    Low = 0,
    Normal = 1,
    High = 2,
    Critical = 3
};

/**
 * @brief Job state
 */
enum class JobState : uint8_t {
    Pending = 0,
    Running = 1,
    Completed = 2,
    Failed = 3,
    Cancelled = 4
};

/**
 * @brief Job function type
 */
using JobFunction = std::function<void()>;

/**
 * @brief Represents a unit of work to be executed
 */
class Job {
public:
    Job();
    Job(JobFunction function, JobPriority priority = JobPriority::Normal);
    
    /**
     * @brief Execute the job
     */
    void Execute();
    
    /**
     * @brief Check if job is valid
     * @return true if job has a function to execute
     */
    bool IsValid() const;
    
    /**
     * @brief Get job priority
     */
    JobPriority GetPriority() const;
    
    /**
     * @brief Set job priority
     */
    void SetPriority(JobPriority priority);
    
private:
    JobFunction m_function;
    JobPriority m_priority;
};

/**
 * @brief Job handle for tracking job completion
 */
class JobHandle {
public:
    JobHandle();
    JobHandle(uint32_t id);
    
    uint32_t GetID() const;
    bool IsValid() const;
    
private:
    uint32_t m_id;
};

/**
 * @brief Job completion tracking
 */
class JobCompletion {
public:
    JobCompletion();
    
    /**
     * @brief Mark job as completed
     */
    void Complete();
    
    /**
     * @brief Mark job as failed
     */
    void Fail();
    
    /**
     * @brief Mark job as cancelled
     */
    void Cancel();
    
    /**
     * @brief Wait for job completion
     * @param timeout_ms Timeout in milliseconds (0 = infinite)
     * @return true if job completed, false if timeout
     */
    bool Wait(uint32_t timeout_ms = 0);
    
    /**
     * @brief Check if job is completed
     */
    bool IsCompleted() const;
    
    /**
     * @brief Check if job failed
     */
    bool IsFailed() const;
    
    /**
     * @brief Check if job was cancelled
     */
    bool IsCancelled() const;
    
    /**
     * @brief Get job state
     */
    JobState GetState() const;
    
private:
    std::atomic<JobState> m_state;
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
};

} // namespace Jobs
} // namespace Poko