/**
 * @file trigger.h
 * @brief Trigger/sensor system with enter/stay/exit events
 *
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 *
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_TRIGGERS_TRIGGER_H
#define POKO_CORE_COMPONENTS_PHYSICS_TRIGGERS_TRIGGER_H

#include "core/components/physics/core/handle.h"
#include "core/components/physics/events/event.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace triggers {

using core::ColliderHandle;
using events::EventBuffer;
using events::TriggerEvent;
using events::EventType;

/**
 * @brief Trigger state
 */
enum class TriggerState : uint32_t {
    None,
    Enter,
    Stay,
    Exit
};

/**
 * @brief Trigger pair tracking
 */
struct TriggerPair {
    ColliderHandle trigger;
    ColliderHandle other;
    TriggerState state;

    TriggerPair() noexcept
        : trigger()
        , other()
        , state(TriggerState::None) {}

    TriggerPair(ColliderHandle trigger_, ColliderHandle other_) noexcept
        : trigger(trigger_)
        , other(other_)
        , state(TriggerState::None) {}

    [[nodiscard]] bool operator==(const TriggerPair& other) const noexcept {
        return (trigger == other.trigger && this->other == other.other) ||
               (trigger == other.other && this->other == other.trigger);
    }
};

/**
 * @brief Trigger pair hash
 */
struct TriggerPairHash {
    size_t operator()(const TriggerPair& pair) const noexcept {
        size_t h1 = std::hash<uint32_t>{}(pair.trigger.index ^ pair.trigger.generation);
        size_t h2 = std::hash<uint32_t>{}(pair.other.index ^ pair.other.generation);
        return h1 ^ (h2 << 1);
    }
};

/**
 * @brief Trigger system
 */
class TriggerSystem {
public:
    /**
     * @brief Constructor
     */
    TriggerSystem() noexcept = default;

    /**
     * @brief Update trigger states
     */
    void update(
        const std::vector<ColliderHandle>& activePairs,
        EventBuffer& eventBuffer
    );

    /**
     * @brief Clear all trigger states
     */
    void clear() noexcept {
        currentPairs.clear();
        previousPairs.clear();
    }

    /**
     * @brief Get current trigger count
     */
    [[nodiscard]] size_t getTriggerCount() const noexcept {
        return currentPairs.size();
    }

    /**
     * @brief Check if pair is currently triggering
     */
    [[nodiscard]] bool isTriggering(const TriggerPair& pair) const noexcept {
        return currentPairs.find(pair) != currentPairs.end();
    }

    /**
     * @brief Get trigger state for pair
     */
    [[nodiscard]] TriggerState getState(const TriggerPair& pair) const noexcept {
        auto it = currentPairs.find(pair);
        if (it != currentPairs.end()) {
            return it->second;
        }
        return TriggerState::None;
    }

private:
    std::unordered_map<TriggerPair, TriggerState, TriggerPairHash> currentPairs;
    std::unordered_set<TriggerPair, TriggerPairHash> previousPairs;

    /**
     * @brief Add enter event
     */
    void addEnterEvent(ColliderHandle trigger, ColliderHandle other, EventBuffer& eventBuffer);

    /**
     * @brief Add stay event
     */
    void addStayEvent(ColliderHandle trigger, ColliderHandle other, EventBuffer& eventBuffer);

    /**
     * @brief Add exit event
     */
    void addExitEvent(ColliderHandle trigger, ColliderHandle other, EventBuffer& eventBuffer);
};

} // namespace triggers
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_TRIGGERS_TRIGGER_H
