/**
 * @file worker.cpp
 * @brief JobSystem worker thread implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/jobs/job.h"

namespace poko {
namespace core {
namespace jobs {

void JobSystem::workerThread(size_t workerId) {
    (void)workerId; // Unused in current implementation
    
    while (m_running.load(std::memory_order_relaxed)) {
        std::unique_lock<std::mutex> lock(m_mutex);
        
        // Wait for job or shutdown
        m_condVar.wait(lock, [this] {
            return !m_pendingQueue.empty() || !m_running.load(std::memory_order_relaxed);
        });
        
        if (!m_running.load(std::memory_order_relaxed)) {
            break;
        }
        
        // Get next job
        if (m_pendingQueue.empty()) {
            continue;
        }
        
        size_t jobIndex = m_pendingQueue.front();
        m_pendingQueue.pop();
        
        if (jobIndex >= m_jobs.size()) {
            continue;
        }
        
        JobEntry& entry = m_jobs[jobIndex];
        if (!entry.job || !entry.job->isPending()) {
            continue;
        }
        
        // Check dependencies
        if (!canExecuteJob(entry.job.get())) {
            // Re-queue job
            m_pendingQueue.push(jobIndex);
            continue;
        }
        
        m_pendingCount.fetch_sub(1, std::memory_order_relaxed);
        m_runningCount.fetch_add(1, std::memory_order_relaxed);
        
        lock.unlock();
        
        // Execute job
        processJob(entry.job.get());
        
        lock.lock();
        m_runningCount.fetch_sub(1, std::memory_order_relaxed);
        
        // Notify waiters
        if (entry.condition) {
            entry.condition->notify_all();
        }
        m_condVar.notify_all();
    }
}

bool JobSystem::canExecuteJob(Job* job) const {
    if (!job->hasDependencies()) {
        return true;
    }
    
    for (JobId depId : job->getDependencies()) {
        for (const auto& entry : m_jobs) {
            if (entry.job && entry.job->getId() == depId) {
                if (!entry.job->isCompleted()) {
                    return false;
                }
                break;
            }
        }
    }
    
    return true;
}

void JobSystem::processJob(Job* job) {
    job->execute();
    
    if (job->isCompleted()) {
        m_completedCount.fetch_add(1, std::memory_order_relaxed);
    } else {
        m_failedCount.fetch_add(1, std::memory_order_relaxed);
    }
}

} // namespace jobs
} // namespace core
} // namespace poko
