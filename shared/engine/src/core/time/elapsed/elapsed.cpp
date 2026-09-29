/**
 * @file elapsed.cpp
 * @brief getElapsedTime implementation
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

Duration getElapsedTime() {
    static auto startTime = std::chrono::high_resolution_clock::now();
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - startTime).count();
    return Duration(elapsed);
}

} // namespace time
} // namespace core
} // namespace poko
