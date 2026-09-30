/**
 * @file trigger.cpp
 * @brief Trigger/sensor system implementation
 *
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 *
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#include "core/components/physics/triggers/trigger.h"
#include "core/components/physics/colliders/collider_definition.h"
#include <algorithm>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace triggers {

void TriggerSystem::update(
    const std::vector<ColliderHandle>& activePairs,
    EventBuffer& eventBuffer
) {
    // Swap current to previous
    previousPairs.clear();
    for (const auto& [pair, state] : currentPairs) {
        previousPairs.insert(pair);
    }

    // Clear current and rebuild from active pairs
    currentPairs.clear();

    for (size_t i = 0; i < activePairs.size(); i += 2) {
        if (i + 1 >= activePairs.size()) break;

        ColliderHandle trigger = activePairs[i];
        ColliderHandle other = activePairs[i + 1];

        TriggerPair pair(trigger, other);

        // Check if this was active in previous frame
        bool wasActive = previousPairs.find(pair) != previousPairs.end();

        if (wasActive) {
            // Still active - stay event
            currentPairs[pair] = TriggerState::Stay;
            addStayEvent(trigger, other, eventBuffer);
        } else {
            // New overlap - enter event
            currentPairs[pair] = TriggerState::Enter;
            addEnterEvent(trigger, other, eventBuffer);
        }
    }

    // Check for exited pairs (in previous but not in current)
    for (const auto& pair : previousPairs) {
        if (currentPairs.find(pair) == currentPairs.end()) {
            // No longer active - exit event
            addExitEvent(pair.trigger, pair.other, eventBuffer);
        }
    }
}

void TriggerSystem::addEnterEvent(ColliderHandle trigger, ColliderHandle other, EventBuffer& eventBuffer) {
    TriggerEvent* event = new TriggerEvent(EventType::TriggerBegin, trigger, other);
    eventBuffer.addEvent(event);
}

void TriggerSystem::addStayEvent(ColliderHandle trigger, ColliderHandle other, EventBuffer& eventBuffer) {
    TriggerEvent* event = new TriggerEvent(EventType::TriggerPersist, trigger, other);
    eventBuffer.addEvent(event);
}

void TriggerSystem::addExitEvent(ColliderHandle trigger, ColliderHandle other, EventBuffer& eventBuffer) {
    TriggerEvent* event = new TriggerEvent(EventType::TriggerEnd, trigger, other);
    eventBuffer.addEvent(event);
}

} // namespace triggers
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko
