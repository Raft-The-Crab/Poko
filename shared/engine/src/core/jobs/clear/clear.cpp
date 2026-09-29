/**
 * @file clear.cpp
 * @brief JobSystem clear operations implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/jobs/job.h"
#include <algorithm>

namespace poko {
namespace core {
namespace jobs {

/**
 * @brief Clear finished jobs from the job list
 * 
 * Removes jobs that have completed, failed, or been cancelled.
 * This keeps the job list from growing indefinitely.
 * 
 * @note Thread-safe via mutex lock
 * @note Only removes finished jobs, not pending or running jobs
 */
void JobSystem::clearFinishedJobs() noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_jobs.erase(
        std::remove_if(m_jobs.begin(), m_jobs.end(),
            [](const JobEntry& entry) {
                return entry.job && (entry.job->isCompleted() || entry.job->isFailed() || entry.job->isCancelled());
            }),
        m_jobs.end()
    );
}

} // namespace jobs
} // namespace core
} // namespace poko
