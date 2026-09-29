/**
 * @file current.cpp
 * @brief getCurrentTime implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/time/time.h"
#include <chrono>

namespace poko {
namespace core {
namespace time {

TimePoint getCurrentTime() noexcept {
    // Use high-resolution clock for best precision
    // These operations are noexcept for standard library chrono functions
    auto now = std::chrono::high_resolution_clock::now();
    auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
    return TimePoint(nanos);
}

} // namespace time
} // namespace core
} // namespace poko
