/**
 * @file event.h
 * @brief Physics events
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 */

#ifndef POKO_CORE_COMPONENTS_PHYSICS_EVENTS_EVENT_H
#define POKO_CORE_COMPONENTS_PHYSICS_EVENTS_EVENT_H

#include "../core/handle.h"
#include "../math/vectors/vector3.h"
#include <vector>
#include <cstdint>

namespace poko {
namespace core {
namespace components {
namespace physics {
namespace events {

using core::BodyHandle;
using core::ColliderHandle;
using core::ConstraintHandle;
using math::Vector3;

/**
 * @brief Event type
 */
enum class EventType : uint32_t {
    ContactBegin,
    ContactPersist,
    ContactEnd,
    TriggerBegin,
    TriggerPersist,
    TriggerEnd,
    BodyWake,
    BodySleep,
    ConstraintBreak,
    CCDImpact
};

/**
 * @brief Base event
 */
struct Event {
    EventType type;
    
    explicit Event(EventType type_) noexcept : type(type_) {}
    virtual ~Event() = default;
};

/**
 * @brief Contact event
 */
struct ContactEvent : public Event {
    ColliderHandle colliderA;
    ColliderHandle colliderB;
    Vector3 normal;
    float penetration;
    
    ContactEvent(EventType type_, ColliderHandle a, ColliderHandle b) noexcept
        : Event(type_)
        , colliderA(a)
        , colliderB(b)
        , normal(0.0f, 1.0f, 0.0f)
        , penetration(0.0f) {}
};

/**
 * @brief Trigger event
 */
struct TriggerEvent : public Event {
    ColliderHandle trigger;
    ColliderHandle other;
    
    TriggerEvent(EventType type_, ColliderHandle trigger_, ColliderHandle other_) noexcept
        : Event(type_)
        , trigger(trigger_)
        , other(other_) {}
};

/**
 * @brief Body sleep/wake event
 */
struct BodySleepEvent : public Event {
    BodyHandle body;
    
    explicit BodySleepEvent(EventType type_, BodyHandle body_) noexcept
        : Event(type_)
        , body(body_) {}
};

/**
 * @brief Constraint break event
 */
struct ConstraintBreakEvent : public Event {
    ConstraintHandle constraint;
    
    explicit ConstraintBreakEvent(ConstraintHandle constraint_) noexcept
        : Event(EventType::ConstraintBreak)
        , constraint(constraint_) {}
};

/**
 * @brief Event buffer
 */
class EventBuffer {
public:
    /**
     * @brief Constructor
     */
    EventBuffer() noexcept = default;

    /**
     * @brief Add event
     */
    void addEvent(Event* event) {
        events.push_back(event);
    }

    /**
     * @brief Get events
     */
    [[nodiscard]] const std::vector<Event*>& getEvents() const noexcept {
        return events;
    }

    /**
     * @brief Clear events
     */
    void clear() {
        for (Event* event : events) {
            delete event;
        }
        events.clear();
    }

    /**
     * @brief Get event count
     */
    [[nodiscard]] size_t getEventCount() const noexcept {
        return events.size();
    }

    /**
     * @brief Check if buffer is empty
     */
    [[nodiscard]] bool isEmpty() const noexcept {
        return events.empty();
    }

    /**
     * @brief Reserve capacity for events
     */
    void reserve(size_t capacity) noexcept {
        events.reserve(capacity);
    }

    /**
     * @brief Get event count by type
     */
    [[nodiscard]] size_t getEventCountByType(EventType type) const noexcept {
        size_t count = 0;
        for (const Event* event : events) {
            if (event->type == type) {
                ++count;
            }
        }
        return count;
    }

private:
    std::vector<Event*> events;
};

} // namespace events
} // namespace physics
} // namespace components
} // namespace core
} // namespace poko

#endif // POKO_CORE_COMPONENTS_PHYSICS_EVENTS_EVENT_H
