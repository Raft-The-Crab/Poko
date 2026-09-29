/**
 * @file statistics.cpp
 * @brief JobSystem statistics operations implementation
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

JobStatus JobSystem::getJobStatus(JobId jobId) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (const auto& entry : m_jobs) {
        if (entry.job && entry.job->getId() == jobId) {
            return entry.job->getStatus();
        }
    }
    
    return JobStatus::Failed;
}

size_t JobSystem::getPendingCount() const {
    return m_pendingCount.load(std::memory_order_relaxed);
}

size_t JobSystem::getRunningCount() const {
    return m_runningCount.load(std::memory_order_relaxed);
}

size_t JobSystem::getCompletedCount() const {
    return m_completedCount.load(std::memory_order_relaxed);
}

size_t JobSystem::getFailedCount() const {
    return m_failedCount.load(std::memory_order_relaxed);
}

size_t JobSystem::getCancelledCount() const {
    // Cancelled jobs are not tracked separately in the current implementation
    // They are included in the failed count or removed from pending
    return 0;
}

size_t JobSystem::getTotalCount() const {
    return m_jobs.size();
}

} // namespace jobs
} // namespace core
} // namespace poko
