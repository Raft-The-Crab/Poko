/**
 * @file settings.h
 * @brief Physics world configuration
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_CORE_SETTINGS_H
#define POKO_CORE_COMPONENTS_PHYSICS_CORE_SETTINGS_H

#include "core/components/physics/math/vectors/vector3.h"
#include <cstdint>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace core {

using math::Vector3;

/**
 * @brief Physics world configuration
 * 
 * Immutable configuration object for physics world.
 * Runtime configuration changes must be classified as:
 * - Safe immediately
 * - Safe at simulation boundary
 * - Requires world rebuild
 */
struct PhysicsWorldSettings {
    // ============================================================================
    // Simulation
    // ============================================================================
    
    Vector3 gravity = Vector3(0.0f, -9.81f, 0.0f);
    float fixedDelta = 1.0f / 60.0f;
    uint32_t maxSubsteps = 8;
    float maxAccumulator = 0.2f;
    float timeScale = 1.0f;
    
    // ============================================================================
    // Solver
    // ============================================================================
    
    uint32_t velocityIterations = 8;
    uint32_t positionIterations = 3;
    float warmStartFactor = 0.8f;
    float slop = 0.01f;
    float baumgarte = 0.2f;
    float restitutionThreshold = 1.0f;
    
    // ============================================================================
    // Collision
    // ============================================================================
    
    float collisionTolerance = 0.001f;
    float penetrationCorrectionPercentage = 0.8f;
    float contactSlop = 0.01f;
    uint32_t maxContactsPerPair = 4;
    
    // ============================================================================
    // CCD
    // ============================================================================
    
    bool ccdEnabled = false;
    float ccdMotionThreshold = 0.1f;
    uint32_t ccdMaxSubsteps = 4;
    float ccdTimeBudget = 0.002f;
    
    // ============================================================================
    // Sleeping
    // ============================================================================
    
    bool sleepingEnabled = true;
    float sleepThreshold = 0.01f;
    float sleepTime = 1.0f;
    
    // ============================================================================
    // World Bounds
    // ============================================================================
    
    Vector3 worldMin = Vector3(-1000.0f, -1000.0f, -1000.0f);
    Vector3 worldMax = Vector3(1000.0f, 1000.0f, 1000.0f);
    
    // ============================================================================
    // Query Limits
    // ============================================================================
    
    uint32_t maxQueriesPerFrame = 1000;
    uint32_t maxHitsPerQuery = 100;
    
    // ============================================================================
    // Memory Budgets
    // ============================================================================
    
    uint32_t maxBodies = 10000;
    uint32_t maxColliders = 20000;
    uint32_t maxShapes = 5000;
    uint32_t maxContacts = 50000;
    uint32_t maxPairs = 50000;
    uint32_t maxConstraints = 5000;
    uint32_t maxIslands = 1000;
    uint32_t maxBroadphaseNodes = 40000;
    
    // ============================================================================
    // Threading
    // ============================================================================
    
    uint32_t numWorkerThreads = 0; // 0 = auto-detect
    bool enableParallelBroadphase = true;
    bool enableParallelNarrowphase = true;
    bool enableParallelSolver = true;
    
    // ============================================================================
    // Determinism
    // ============================================================================
    
    enum class DeterminismMode {
        Performance,
        Deterministic
    };
    
    DeterminismMode determinismMode = DeterminismMode::Performance;
    
    // ============================================================================
    // Quality Level
    // ============================================================================
    
    enum class QualityLevel {
        Performance,
        Balanced,
        Quality,
        Custom
    };
    
    QualityLevel qualityLevel = QualityLevel::Balanced;
    
    // ============================================================================
    // Debug
    // ============================================================================
    
    bool enableDebugVisualization = false;
    bool enableProfiling = false;
    bool enableValidation = true;
};

} // namespace core
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_CORE_SETTINGS_H
