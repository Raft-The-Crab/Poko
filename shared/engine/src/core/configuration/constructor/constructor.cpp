/**
 * @file constructor.cpp
 * @brief Configuration constructor and destructor implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/configuration/configuration.h"

namespace poko {
namespace core {
namespace configuration {

/**
 * @brief Constructor
 * 
 * Initializes an empty configuration system with no values or defaults.
 */
Configuration::Configuration()
    : m_values()
    , m_defaults()
{
}

/**
 * @brief Destructor
 * 
 * Cleans up the configuration system.
 */
Configuration::~Configuration() = default;

} // namespace configuration
} // namespace core
} // namespace poko
