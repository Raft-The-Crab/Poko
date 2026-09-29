/**
 * @file sleep.cpp
 * @brief sleep implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/time/time.h"
#include <chrono>
#include <thread>

namespace poko {
namespace core {
namespace time {

void sleep(Duration duration) noexcept {
    if (duration.isNegative() || duration.isZero()) {
        return;
    }
    
    // Validate sleep duration to prevent excessive blocking
    auto seconds = duration.toSeconds();
    if (seconds < MIN_SLEEP_SECONDS || seconds > MAX_SLEEP_SECONDS) {
        return;
    }
    
    // Convert to milliseconds for std::this_thread::sleep_for
    // Exception handling: if thread interruption occurs, we catch and ignore
    // This is acceptable for game engine use cases where thread interruption is rare
    try {
        auto ms = duration.toMilliseconds();
        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int64_t>(ms)));
    } catch (...) {
        // Ignore thread interruption exceptions
    }
}

} // namespace time
} // namespace core
} // namespace poko
