/**
 * @file time.h
 * @brief Core time system main header
 * 
 * Copyright (c) 2026 Poko Productions. All rights reserved.
 * 
 * This file is part of the Poko Engine project.
 * Unauthorized copying, modification, or distribution is prohibited.
 * 
 * This module provides time abstractions for the Poko Engine including
 * high-resolution timers, time deltas, and clock management.
 */

#ifndef POKO_CORE_TIME_TIME_H
#define POKO_CORE_TIME_TIME_H

#include <cstdint>

namespace poko {
namespace core {
namespace time {

// ============================================================================
// Type Definitions
// ============================================================================

/**
 * @brief Type for time values in seconds
 */
using Seconds = double;

/**
 * @brief Type for time values in milliseconds
 */
using Milliseconds = double;

/**
 * @brief Type for time values in microseconds
 */
using Microseconds = int64_t;

/**
 * @brief Type for time values in nanoseconds
 */
using Nanoseconds = int64_t;

// ============================================================================
// Constants
// ============================================================================

/// Number of nanoseconds per microsecond
constexpr int64_t NANOS_PER_MICRO = 1000;

/// Number of microseconds per millisecond
constexpr int64_t MICROS_PER_MILLI = 1000;

/// Number of milliseconds per second
constexpr int64_t MILLIS_PER_SECOND = 1000;

/// Number of microseconds per second
constexpr int64_t MICROS_PER_SECOND = 1000000;

/// Number of nanoseconds per second
constexpr int64_t NANOS_PER_SECOND = 1000000000;

/// Maximum time value in seconds (to prevent overflow)
constexpr Seconds MAX_TIME_SECONDS = 3600.0; // 1 hour

// ============================================================================
// Duration
// ============================================================================

/**
 * @brief Represents a time duration
 * 
 * Used for measuring time intervals between time points.
 * Use factory methods to construct from different time units.
 */
struct Duration {
    Nanoseconds nanos;  ///< Duration in nanoseconds
    
    /**
     * @brief Default constructor - zero duration
     */
    constexpr Duration() noexcept : nanos(0) {}
    
    /**
     * @brief Construct from nanoseconds (only direct constructor)
     * @param ns Duration in nanoseconds
     */
    constexpr explicit Duration(Nanoseconds ns) noexcept : nanos(ns) {}
    
    /**
     * @brief Create duration from seconds
     * @param s Duration in seconds
     * @return Duration object
     */
    static constexpr Duration fromSeconds(Seconds s) noexcept {
        return Duration(static_cast<Nanoseconds>(s * NANOS_PER_SECOND));
    }
    
    /**
     * @brief Create duration from milliseconds
     * @param ms Duration in milliseconds
     * @return Duration object
     */
    static constexpr Duration fromMilliseconds(Milliseconds ms) noexcept {
        return Duration(static_cast<Nanoseconds>(ms * MICROS_PER_MILLI * NANOS_PER_MICRO));
    }
    
    /**
     * @brief Create duration from microseconds
     * @param us Duration in microseconds
     * @return Duration object
     */
    static constexpr Duration fromMicroseconds(Microseconds us) noexcept {
        return Duration(us * NANOS_PER_MICRO);
    }
    
    /**
     * @brief Create duration from nanoseconds
     * @param ns Duration in nanoseconds
     * @return Duration object
     */
    static constexpr Duration fromNanoseconds(Nanoseconds ns) noexcept {
        return Duration(ns);
    }
    
    /**
     * @brief Get duration in seconds
     * @return Duration in seconds
     */
    [[nodiscard]] constexpr Seconds toSeconds() const noexcept {
        return static_cast<Seconds>(nanos) / NANOS_PER_SECOND;
    }
    
    /**
     * @brief Get duration in milliseconds
     * @return Duration in milliseconds
     */
    [[nodiscard]] constexpr Milliseconds toMilliseconds() const noexcept {
        return static_cast<Milliseconds>(nanos) / MICROS_PER_MILLI / NANOS_PER_MICRO;
    }
    
    /**
     * @brief Get duration in microseconds
     * @return Duration in microseconds
     */
    [[nodiscard]] constexpr Microseconds toMicroseconds() const noexcept {
        return nanos / NANOS_PER_MICRO;
    }
    
    /**
     * @brief Get duration in nanoseconds
     * @return Duration in nanoseconds
     */
    [[nodiscard]] constexpr Nanoseconds toNanoseconds() const noexcept {
        return nanos;
    }
    
    /**
     * @brief Check if duration is zero
     * @return True if duration is zero
     */
    [[nodiscard]] constexpr bool isZero() const noexcept {
        return nanos == 0;
    }
    
    /**
     * @brief Check if duration is positive
     * @return True if duration > 0
     */
    [[nodiscard]] constexpr bool isPositive() const noexcept {
        return nanos > 0;
    }
    
    /**
     * @brief Check if duration is negative
     * @return True if duration < 0
     */
    [[nodiscard]] constexpr bool isNegative() const noexcept {
        return nanos < 0;
    }
    
    /**
     * @brief Equality operator
     */
    constexpr bool operator==(const Duration& other) const noexcept {
        return nanos == other.nanos;
    }
    
    /**
     * @brief Inequality operator
     */
    constexpr bool operator!=(const Duration& other) const noexcept {
        return !(*this == other);
    }
    
    /**
     * @brief Less than operator
     */
    constexpr bool operator<(const Duration& other) const noexcept {
        return nanos < other.nanos;
    }
    
    /**
     * @brief Less than or equal operator
     */
    constexpr bool operator<=(const Duration& other) const noexcept {
        return nanos <= other.nanos;
    }
    
    /**
     * @brief Greater than operator
     */
    constexpr bool operator>(const Duration& other) const noexcept {
        return nanos > other.nanos;
    }
    
    /**
     * @brief Greater than or equal operator
     */
    constexpr bool operator>=(const Duration& other) const noexcept {
        return nanos >= other.nanos;
    }
    
    /**
     * @brief Addition operator
     */
    constexpr Duration operator+(const Duration& other) const noexcept {
        return Duration(nanos + other.nanos);
    }
    
    /**
     * @brief Subtraction operator
     */
    constexpr Duration operator-(const Duration& other) const noexcept {
        return Duration(nanos - other.nanos);
    }
    
    /**
     * @brief Multiplication operator
     */
    constexpr Duration operator*(double scalar) const noexcept {
        return Duration(static_cast<Nanoseconds>(nanos * scalar));
    }
    
    /**
     * @brief Division operator
     */
    constexpr Duration operator/(double scalar) const noexcept {
        return Duration(static_cast<Nanoseconds>(nanos / scalar));
    }
    
    /**
     * @brief Add assignment operator
     */
    Duration& operator+=(const Duration& other) noexcept {
        nanos += other.nanos;
        return *this;
    }
    
    /**
     * @brief Subtract assignment operator
     */
    Duration& operator-=(const Duration& other) noexcept {
        nanos -= other.nanos;
        return *this;
    }
};

// ============================================================================
// Time Point
// ============================================================================

/**
 * @brief Represents a point in time
 * 
 * Used for measuring time intervals and creating timestamps.
 * Internally stored as nanoseconds for high precision.
 */
struct TimePoint {
    Nanoseconds nanos;  ///< Time in nanoseconds since epoch
    
    /**
     * @brief Default constructor - creates time point at zero
     */
    constexpr TimePoint() noexcept : nanos(0) {}
    
    /**
     * @brief Construct from nanoseconds (only direct constructor)
     * @param ns Time in nanoseconds
     */
    constexpr explicit TimePoint(Nanoseconds ns) noexcept : nanos(ns) {}
    
    /**
     * @brief Create time point from seconds
     * @param s Time in seconds
     * @return TimePoint object
     */
    static constexpr TimePoint fromSeconds(Seconds s) noexcept {
        return TimePoint(static_cast<Nanoseconds>(s * NANOS_PER_SECOND));
    }
    
    /**
     * @brief Create time point from milliseconds
     * @param ms Time in milliseconds
     * @return TimePoint object
     */
    static constexpr TimePoint fromMilliseconds(Milliseconds ms) noexcept {
        return TimePoint(static_cast<Nanoseconds>(ms * MICROS_PER_MILLI * NANOS_PER_MICRO));
    }
    
    /**
     * @brief Create time point from microseconds
     * @param us Time in microseconds
     * @return TimePoint object
     */
    static constexpr TimePoint fromMicroseconds(Microseconds us) noexcept {
        return TimePoint(us * NANOS_PER_MICRO);
    }
    
    /**
     * @brief Create time point from nanoseconds
     * @param ns Time in nanoseconds
     * @return TimePoint object
     */
    static constexpr TimePoint fromNanoseconds(Nanoseconds ns) noexcept {
        return TimePoint(ns);
    }
    
    /**
     * @brief Get time in seconds
     * @return Time in seconds
     */
    [[nodiscard]] constexpr Seconds toSeconds() const noexcept {
        return static_cast<Seconds>(nanos) / NANOS_PER_SECOND;
    }
    
    /**
     * @brief Get time in milliseconds
     * @return Time in milliseconds
     */
    [[nodiscard]] constexpr Milliseconds toMilliseconds() const noexcept {
        return static_cast<Milliseconds>(nanos) / MICROS_PER_MILLI / NANOS_PER_MICRO;
    }
    
    /**
     * @brief Get time in microseconds
     * @return Time in microseconds
     */
    [[nodiscard]] constexpr Microseconds toMicroseconds() const noexcept {
        return nanos / NANOS_PER_MICRO;
    }
    
    /**
     * @brief Get time in nanoseconds
     * @return Time in nanoseconds
     */
    [[nodiscard]] constexpr Nanoseconds toNanoseconds() const noexcept {
        return nanos;
    }
    
    /**
     * @brief Equality operator
     */
    constexpr bool operator==(const TimePoint& other) const noexcept {
        return nanos == other.nanos;
    }
    
    /**
     * @brief Inequality operator
     */
    constexpr bool operator!=(const TimePoint& other) const noexcept {
        return !(*this == other);
    }
    
    /**
     * @brief Less than operator
     */
    constexpr bool operator<(const TimePoint& other) const noexcept {
        return nanos < other.nanos;
    }
    
    /**
     * @brief Less than or equal operator
     */
    constexpr bool operator<=(const TimePoint& other) const noexcept {
        return nanos <= other.nanos;
    }
    
    /**
     * @brief Greater than operator
     */
    constexpr bool operator>(const TimePoint& other) const noexcept {
        return nanos > other.nanos;
    }
    
    /**
     * @brief Greater than or equal operator
     */
    constexpr bool operator>=(const TimePoint& other) const noexcept {
        return nanos >= other.nanos;
    }
    
    /**
     * @brief Subtraction operator (returns duration)
     */
    constexpr Duration operator-(const TimePoint& other) const noexcept {
        return Duration::fromNanoseconds(nanos - other.nanos);
    }
    
    /**
     * @brief Addition operator (returns time point)
     */
    constexpr TimePoint operator+(const Duration& other) const noexcept {
        return TimePoint(nanos + other.nanos);
    }
    
    /**
     * @brief Add assignment operator
     */
    TimePoint& operator+=(const Duration& other) noexcept {
        nanos += other.nanos;
        return *this;
    }
    
    /**
     * @brief Subtract assignment operator
     */
    TimePoint& operator-=(const Duration& other) noexcept {
        nanos -= other.nanos;
        return *this;
    }
};

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * @brief Get current time as TimePoint
 * @return Current time point
 */
[[nodiscard]] TimePoint getCurrentTime() noexcept;

/**
 * @brief Sleep for a duration
 * @param duration Duration to sleep
 * @note Thread interruption exceptions are caught and ignored for game engine robustness
 */
void sleep(Duration duration) noexcept;

/**
 * @brief Get time since program start
 * @return Time since program started
 */
[[nodiscard]] Duration getElapsedTime() noexcept;

} // namespace time
} // namespace core
} // namespace poko

#endif // POKO_CORE_TIME_TIME_H
