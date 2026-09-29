/**
 * @file submit.cpp
 * @brief JobSystem job submission implementation
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

JobId JobSystem::submit(std::unique_ptr<Job> job) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    if (!m_running.load(std::memory_order_relaxed)) {
        return INVALID_JOB_ID;
    }
    
    JobId jobId = job->getId();
    m_jobs.push_back({std::move(job), nullptr});
    m_pendingQueue.push(m_jobs.size() - 1);
    m_pendingCount.fetch_add(1, std::memory_order_relaxed);
    
    m_condVar.notify_one();
    
    return jobId;
}

} // namespace jobs
} // namespace core
} // namespace poko
