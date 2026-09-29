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

void sleep(Duration duration) {
    if (duration.isNegative() || duration.isZero()) {
        return;
    }
    
    // Convert to milliseconds for std::this_thread::sleep_for
    auto ms = duration.toMilliseconds();
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int64_t>(ms)));
}

} // namespace time
} // namespace core
} // namespace poko
