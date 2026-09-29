/**
 * @file cancel.cpp
 * @brief JobSystem job cancellation implementation
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

bool JobSystem::cancel(JobId jobId) noexcept {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    for (auto& entry : m_jobs) {
        if (entry.job && entry.job->getId() == jobId) {
            if (entry.job->isPending()) {
                entry.job->setStatus(JobStatus::Cancelled);
                m_pendingCount.fetch_sub(1, std::memory_order_relaxed);
                return true;
            }
            return false;
        }
    }
    
    return false;
}

} // namespace jobs
} // namespace core
} // namespace poko
