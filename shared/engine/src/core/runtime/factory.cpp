/**
 * @file factory.cpp
 * @brief Instance factory implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/runtime/instance.h"
#include <memory>

namespace poko {
namespace core {
namespace runtime {

std::unique_ptr<Instance> createInstance(InstanceType type, const std::string& name) {
    return std::make_unique<Instance>(type, name);
}

} // namespace runtime
} // namespace core
} // namespace poko
