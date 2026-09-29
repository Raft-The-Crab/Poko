/**
 * @file shutdown.cpp
 * @brief JobSystem shutdown implementation
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

void JobSystem::shutdown() {
    m_running.store(false, std::memory_order_relaxed);
    m_shutdown.store(true, std::memory_order_relaxed);
    m_condVar.notify_all();
}

bool JobSystem::isRunning() const {
    return m_running.load(std::memory_order_relaxed);
}

} // namespace jobs
} // namespace core
} // namespace poko
