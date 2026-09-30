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
    float maxLinearVelocity = 100.0f;
    float maxAngularVelocity = 100.0f;
    
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

    /**
     * @brief Validate settings
     */
    [[nodiscard]] bool isValid() const noexcept {
        return fixedDelta > 0.0f &&
               maxSubsteps > 0 &&
               maxAccumulator > 0.0f &&
               timeScale > 0.0f &&
               velocityIterations > 0 &&
               positionIterations > 0 &&
               maxContactsPerPair > 0 &&
               maxLinearVelocity > 0.0f &&
               maxAngularVelocity > 0.0f &&
               sleepThreshold >= 0.0f &&
               sleepTime >= 0.0f &&
               maxBodies > 0 &&
               maxColliders > 0 &&
               maxShapes > 0 &&
               maxContacts > 0 &&
               maxPairs > 0 &&
               maxConstraints > 0 &&
               maxIslands > 0 &&
               maxBroadphaseNodes > 0;
    }

    /**
     * @brief Clamp values to safe ranges
     */
    void clamp() noexcept {
        if (fixedDelta <= 0.0f) fixedDelta = 1.0f / 60.0f;
        if (maxSubsteps == 0) maxSubsteps = 8;
        if (maxAccumulator <= 0.0f) maxAccumulator = 0.2f;
        if (timeScale <= 0.0f) timeScale = 1.0f;
        if (velocityIterations == 0) velocityIterations = 8;
        if (positionIterations == 0) positionIterations = 3;
        if (maxContactsPerPair == 0) maxContactsPerPair = 4;
        if (maxLinearVelocity <= 0.0f) maxLinearVelocity = 100.0f;
        if (maxAngularVelocity <= 0.0f) maxAngularVelocity = 100.0f;
        if (sleepThreshold < 0.0f) sleepThreshold = 0.01f;
        if (sleepTime < 0.0f) sleepTime = 1.0f;
    }

    /**
     * @brief Apply quality preset
     */
    void setQualityPreset(QualityLevel level) noexcept {
        qualityLevel = level;
        switch (level) {
            case QualityLevel::Performance:
                velocityIterations = 4;
                positionIterations = 1;
                maxContactsPerPair = 2;
                determinismMode = DeterminismMode::Performance;
                break;
            case QualityLevel::Balanced:
                velocityIterations = 8;
                positionIterations = 3;
                maxContactsPerPair = 4;
                determinismMode = DeterminismMode::Performance;
                break;
            case QualityLevel::Quality:
                velocityIterations = 16;
                positionIterations = 6;
                maxContactsPerPair = 6;
                determinismMode = DeterminismMode::Deterministic;
                break;
            case QualityLevel::Custom:
                // Keep current values
                break;
        }
    }

    /**
     * @brief Get effective delta time
     */
    [[nodiscard]] float getEffectiveDelta() const noexcept {
        return fixedDelta * timeScale;
    }
};

} // namespace core
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_CORE_SETTINGS_H
