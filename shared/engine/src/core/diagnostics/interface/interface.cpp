/**
 * @file interface.cpp
 * @brief Global diagnostics manager interface implementation
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/diagnostics/diagnostics.h"
#include <memory>

namespace poko {
namespace core {
namespace diagnostics {

namespace {
    std::unique_ptr<DiagnosticsManager> g_globalDiagnosticsManager;
    std::mutex g_globalMutex;
}

DiagnosticsManager& getGlobalDiagnosticsManager() {
    std::lock_guard<std::mutex> lock(g_globalMutex);
    
    if (!g_globalDiagnosticsManager) {
        g_globalDiagnosticsManager = std::make_unique<DiagnosticsManager>();
    }
    
    return *g_globalDiagnosticsManager;
}

void destroyGlobalDiagnosticsManager() {
    std::lock_guard<std::mutex> lock(g_globalMutex);
    g_globalDiagnosticsManager.reset();
}

} // namespace diagnostics
} // namespace core
} // namespace poko
