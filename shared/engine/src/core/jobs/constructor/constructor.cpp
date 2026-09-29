/**
 * @file constructor.cpp
 * @brief JobSystem constructor implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/jobs/job.h"
#include <algorithm>
#include <thread>

namespace poko {
namespace core {
namespace jobs {

JobSystem::JobSystem(size_t numWorkers) {
    if (numWorkers == 0) {
        numWorkers = std::thread::hardware_concurrency();
    }
    
    if (numWorkers > MAX_WORKER_THREADS) {
        numWorkers = MAX_WORKER_THREADS;
    }
    
    if (numWorkers == 0) {
        numWorkers = DEFAULT_WORKER_THREADS;
    }
    
    m_workers.reserve(numWorkers);
    for (size_t i = 0; i < numWorkers; ++i) {
        m_workers.emplace_back(&JobSystem::workerThread, this, i);
    }
}

} // namespace jobs
} // namespace core
} // namespace poko
