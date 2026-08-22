/**
 * @file job_manager.cpp
 * @brief Job manager implementation
 * @author Moby
 * @version 1.0.0
 * @date 2026-08-22
 */

#include "core/jobs/job_manager.h"
#include "core/logging/logger.h"
#include <algorithm>
#include <thread>
#include <vector>

namespace Poko {
namespace Jobs {

JobManager::JobManager()
    : m_shouldStop(false)
    , m_isInitialized(false)
    , m_nextJobID(0)
    , m_pendingJobs(0)
    , m_runningJobs(0)
    , m_totalCompletions(0)
{
}

JobManager::~JobManager()
{
    if (m_isInitialized) {
        Shutdown();
    }
}

bool JobManager::Initialize(const JobManagerConfig& config)
{
    if (m_isInitialized) {
        LOG_WARNING("JobManager already initialized");
        return false;
    }

    m_config = config;

    // Auto-detect worker count if not specified
    size_t numWorkers = m_config.num_workers;
    if (numWorkers == 0) {
        numWorkers = std::thread::hardware_concurrency();
        if (numWorkers == 0) {
            numWorkers = 4; // Fallback to 4 workers
        }
    }

    LOG_INFO("Initializing JobManager with " + std::to_string(numWorkers) + " workers...");

    // Create worker threads
    m_workerThreads.reserve(numWorkers);
    for (size_t i = 0; i < numWorkers; ++i) {
        m_workerThreads.emplace_back(&JobManager::WorkerThread, this);
    }

    m_isInitialized = true;
    LOG_INFO("JobManager initialized successfully");
    return true;
}

void JobManager::Shutdown()
{
    if (!m_isInitialized) {
        return;
    }

    LOG_INFO("Shutting down JobManager...");

    // Signal all worker threads to stop
    m_shouldStop = true;
    m_queueCondition.notify_all();

    // Optionally wait for running jobs to complete
    if (m_config.wait_for_running_on_shutdown) {
        LOG_INFO("Waiting for running jobs to complete...");
        while (m_runningJobs > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }

    // Wait for all worker threads to finish
    for (auto& thread : m_workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    m_workerThreads.clear();
    m_shouldStop = false;
    m_isInitialized = false;

    // Clear any remaining jobs and completions
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        while (!m_priorityQueue.empty()) {
            m_priorityQueue.pop();
        }
        while (!m_fifoQueue.empty()) {
            m_fifoQueue.pop();
        }
        m_pendingJobs = 0;
    }

    {
        std::lock_guard<std::mutex> lock(m_completionMutex);
        m_jobCompletions.clear();
        m_completedJobIDs.clear();
    }

    m_totalCompletions = 0;

    LOG_INFO("JobManager shutdown complete");
}

JobHandle JobManager::AddJob(Job job)
{
    if (!m_isInitialized) {
        LOG_ERROR("JobManager not initialized");
        return JobHandle{0};
    }

    // Check queue size limit
    if (m_pendingJobs >= m_config.max_queue_size) {
        LOG_WARNING("Job queue full, cannot add job");
        return JobHandle{0};
    }

    // Assign job ID
    uint32_t jobID = ++m_nextJobID;

    // Create completion tracker
    auto completion = std::make_shared<JobCompletion>();
    {
        std::lock_guard<std::mutex> lock(m_completionMutex);
        m_jobCompletions[jobID] = completion;
    }

    // Add job to appropriate queue
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        if (m_config.enable_priorities) {
            m_priorityQueue.push({job, jobID});
        } else {
            m_fifoQueue.push({job, jobID});
        }
        m_pendingJobs++;
    }

    // Notify one worker thread
    m_queueCondition.notify_one();

    LOG_DEBUG("Added job " + std::to_string(jobID) + " to queue");
    return JobHandle{jobID};
}

bool JobManager::WaitForJob(JobHandle handle, uint32_t timeout_ms)
{
    if (!m_isInitialized || !handle.IsValid()) {
        return false;
    }

    auto completion = GetJobCompletion(handle.GetID());
    if (!completion) {
        return false;
    }

    return completion->Wait(timeout_ms);
}

bool JobManager::CancelJob(JobHandle handle)
{
    if (!m_isInitialized || !handle.IsValid()) {
        return false;
    }

    uint32_t jobID = handle.GetID();
    
    // Try to cancel from queue
    {
        std::lock_guard<std::mutex> lock(m_queueMutex);
        // Note: Priority queue doesn't support easy removal, so we check completion state instead
    }

    // Mark as cancelled if not already running/completed
    auto completion = GetJobCompletion(jobID);
    if (completion && completion->GetState() == JobState::Pending) {
        completion->Cancel();
        m_pendingJobs--;
        LOG_DEBUG("Cancelled job " + std::to_string(jobID));
        return true;
    }

    return false;
}

bool JobManager::WaitForAllJobs(uint32_t timeout_ms)
{
    if (!m_isInitialized) {
        return false;
    }

    auto start = std::chrono::steady_clock::now();
    
    while (m_pendingJobs > 0 || m_runningJobs > 0) {
        if (timeout_ms > 0) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - start).count();
            if (elapsed >= timeout_ms) {
                return false;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    LOG_DEBUG("All jobs completed");
    return true;
}

size_t JobManager::GetPendingJobCount() const
{
    return m_pendingJobs.load();
}

size_t JobManager::GetRunningJobCount() const
{
    return m_runningJobs.load();
}

JobState JobManager::GetJobState(JobHandle handle) const
{
    if (!handle.IsValid()) {
        return JobState::Failed;
    }

    auto completion = GetJobCompletion(handle.GetID());
    if (!completion) {
        return JobState::Failed;
    }

    return completion->GetState();
}

bool JobManager::IsInitialized() const
{
    return m_isInitialized.load();
}

void JobManager::ClearPendingJobs()
{
    if (!m_isInitialized) {
        return;
    }

    std::lock_guard<std::mutex> lock(m_queueMutex);
    
    // Cancel all pending jobs
    while (!m_priorityQueue.empty()) {
        auto& jobPair = const_cast<std::pair<Job, uint32_t>&>(m_priorityQueue.top());
        auto completion = GetJobCompletion(jobPair.second);
        if (completion) {
            completion->Cancel();
        }
        m_priorityQueue.pop();
    }
    
    while (!m_fifoQueue.empty()) {
        auto& jobPair = m_fifoQueue.front();
        auto completion = GetJobCompletion(jobPair.second);
        if (completion) {
            completion->Cancel();
        }
        m_fifoQueue.pop();
    }
    
    m_pendingJobs = 0;
    LOG_DEBUG("Cleared all pending jobs");
}

void JobManager::WorkerThread()
{
    LOG_DEBUG("Worker thread started");

    while (!m_shouldStop) {
        Job job;
        uint32_t jobID = 0;
        
        if (GetNextJob(job, jobID)) {
            m_runningJobs++;
            
            // Execute the job
            try {
                job.Execute();
                MarkJobComplete(jobID, true);
            } catch (const std::exception& e) {
                LOG_ERROR("Job execution failed: " + std::string(e.what()));
                MarkJobComplete(jobID, false);
            } catch (...) {
                LOG_ERROR("Job execution failed with unknown exception");
                MarkJobComplete(jobID, false);
            }
            
            m_runningJobs--;
        }
    }

    LOG_DEBUG("Worker thread stopped");
}

bool JobManager::GetNextJob(Job& out_job, uint32_t& out_job_id)
{
    std::unique_lock<std::mutex> lock(m_queueMutex);

    // Wait for job or stop signal
    m_queueCondition.wait(lock, [this] {
        return (!m_priorityQueue.empty() || !m_fifoQueue.empty()) || m_shouldStop;
    });

    if (m_shouldStop) {
        return false;
    }

    // Get job from priority queue or FIFO queue
    if (m_config.enable_priorities && !m_priorityQueue.empty()) {
        auto jobPair = const_cast<std::pair<Job, uint32_t>&>(m_priorityQueue.top());
        out_job = jobPair.first;
        out_job_id = jobPair.second;
        m_priorityQueue.pop();
    } else if (!m_fifoQueue.empty()) {
        auto jobPair = m_fifoQueue.front();
        out_job = jobPair.first;
        out_job_id = jobPair.second;
        m_fifoQueue.pop();
    } else {
        return false;
    }

    m_pendingJobs--;
    return true;
}

void JobManager::MarkJobComplete(uint32_t job_id, bool success)
{
    auto completion = GetJobCompletion(job_id);
    if (completion) {
        if (success) {
            completion->Complete();
        } else {
            completion->Fail();
        }

        // Track completed job for cleanup
        {
            std::lock_guard<std::mutex> lock(m_completionMutex);
            m_completedJobIDs.push_back(job_id);
            m_totalCompletions++;

            // Cleanup old completion records periodically
            if (m_totalCompletions % m_config.completion_cleanup_threshold == 0) {
                CleanupOldCompletions();
            }
        }
    }
}

std::shared_ptr<JobCompletion> JobManager::GetJobCompletion(uint32_t job_id) const
{
    std::lock_guard<std::mutex> lock(m_completionMutex);
    auto it = m_jobCompletions.find(job_id);
    if (it != m_jobCompletions.end()) {
        return it->second;
    }
    return nullptr;
}

void JobManager::CleanupOldCompletions()
{
    // Remove completion records for jobs that have been completed for a while
    // Keep only the most recent half of completed jobs
    if (m_completedJobIDs.empty()) {
        return;
    }

    size_t keep_count = m_completedJobIDs.size() / 2;
    std::vector<uint32_t> to_erase;

    // Find completed jobs to erase (already holding lock from caller)
    for (size_t i = 0; i < m_completedJobIDs.size() - keep_count; ++i) {
        uint32_t job_id = m_completedJobIDs[i];
        auto it = m_jobCompletions.find(job_id);
        if (it != m_jobCompletions.end() && it->second->IsCompleted()) {
            to_erase.push_back(job_id);
        }
    }

    // Erase the old completions (safe since we collected IDs first)
    for (uint32_t job_id : to_erase) {
        m_jobCompletions.erase(job_id);
    }

    // Clear old IDs, keep recent ones
    if (m_completedJobIDs.size() > keep_count) {
        m_completedJobIDs.erase(m_completedJobIDs.begin(), 
                               m_completedJobIDs.begin() + (m_completedJobIDs.size() - keep_count));
    }

    LOG_DEBUG("Cleaned up old job completions, remaining: " + std::to_string(m_jobCompletions.size()));
}

} // namespace Jobs
} // namespace Poko