/**
 * @file interface.cpp
 * @brief Core jobs system interface implementation
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
// Global Job System
// ============================================================================

namespace {
    std::mutex g_jobSystemMutex;
    JobSystem g_jobSystemInstance;
}

/**
 * @brief Global job system instance
 * 
 * Points to the globally shared JobSystem instance.
 * Used throughout the engine for parallel task execution.
 */
JobSystem* g_jobSystem = &g_jobSystemInstance;

/**
 * @brief Get global job system
 * 
 * Returns a reference to the globally shared JobSystem instance.
 * This system is intended for general-purpose job execution throughout the engine.
 * 
 * @return Reference to global job system
 * 
 * @note Thread-safe access is guaranteed
 * @note Do not destroy this system - it's globally managed
 */
JobSystem& getJobSystem() {
    std::lock_guard<std::mutex> lock(g_jobSystemMutex);
    return *g_jobSystem;
}

} // namespace jobs
} // namespace core
} // namespace poko
