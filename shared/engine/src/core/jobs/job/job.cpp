/**
 * @file job.cpp
 * @brief Job class implementation
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

// ============================================================================
// Job Implementation
// ============================================================================

std::atomic<JobId> Job::s_nextJobId{1};

Job::Job(JobTask task, JobPriority priority)
    : m_id(s_nextJobId.fetch_add(1, std::memory_order_relaxed))
    , m_priority(priority)
    , m_status(JobStatus::Pending)
    , m_task(std::move(task))
{
}

bool Job::execute() {
    if (m_status != JobStatus::Pending) {
        return false;
    }
    
    m_status = JobStatus::Running;
    
    bool success = m_task();
    
    m_status = success ? JobStatus::Completed : JobStatus::Failed;
    
    return success;
}

void Job::addDependency(JobId jobId) {
    m_dependencies.push_back(jobId);
}

} // namespace jobs
} // namespace core
} // namespace poko
