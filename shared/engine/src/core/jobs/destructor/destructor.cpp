/**
 * @file destructor.cpp
 * @brief JobSystem destructor implementation
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

JobSystem::~JobSystem() {
    shutdown();
    
    for (auto& worker : m_workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

} // namespace jobs
} // namespace core
} // namespace poko
