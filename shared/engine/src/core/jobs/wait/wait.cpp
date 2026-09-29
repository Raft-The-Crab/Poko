/**
 * @file wait.cpp
 * @brief JobSystem wait operations implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/jobs/job.h"
#include <chrono>

namespace poko {
namespace core {
namespace jobs {

bool JobSystem::waitFor(JobId jobId, uint32_t timeoutMs) {
    std::unique_lock<std::mutex> lock(m_mutex);
    
    for (auto& entry : m_jobs) {
        if (entry.job && entry.job->getId() == jobId) {
            if (!entry.condition) {
                entry.condition = new std::condition_variable();
            }
            
            if (timeoutMs == 0) {
                entry.condition->wait(lock, [&entry] {
                    return entry.job->isCompleted() || entry.job->isFailed() || entry.job->isCancelled();
                });
            } else {
                auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
                entry.condition->wait_until(lock, deadline, [&entry] {
                    return entry.job->isCompleted() || entry.job->isFailed() || entry.job->isCancelled();
                });
            }
            
            delete entry.condition;
            entry.condition = nullptr;
            
            return entry.job->isCompleted();
        }
    }
    
    return false;
}

bool JobSystem::waitForAll(uint32_t timeoutMs) {
    std::unique_lock<std::mutex> lock(m_mutex);
    
    auto predicate = [this] {
        return m_pendingCount.load(std::memory_order_relaxed) == 0 &&
               m_runningCount.load(std::memory_order_relaxed) == 0;
    };
    
    if (timeoutMs == 0) {
        m_condVar.wait(lock, predicate);
    } else {
        auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);
        m_condVar.wait_until(lock, deadline, predicate);
    }
    
    return predicate();
}

} // namespace jobs
} // namespace core
} // namespace poko
