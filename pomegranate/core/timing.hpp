#pragma once

#include <chrono>

#include "base.hpp"

namespace pom {
    /// @addtogroup core
    /// @{

    /// Used for small timesteps from `Timer`. Implicitly converts to `f32` as miliseconds.
    struct DeltaTime {
        /// converts to seconds.
        [[nodiscard]] f32 sec() const;

        /// converts to milliseconds. Note that DeltaTime implicitly converts to milliseconds when this used as a `f32`.
        [[nodiscard]] f32 milli() const;

        /// converts to microseconds.
        /// @warning This does not necessarily mean that the measured time will have microsecond precision. Most modern
        /// hardware does though.
        [[nodiscard]] f32 micro() const;

        /// converts to microseconds.
        /// @warning This does not necessarily mean that the measured time will have nanosecond precision. Some modern
        /// hardware does however, the accuracy here may not be perfect, modern hardware usually only has 10ns
        /// resolution.
        [[nodiscard]] f32 nano() const;

        operator f32() const;

        std::chrono::duration<f32, std::nano> delta;
    };

    /// Timer used for measuring rapid changes in times. On construction it measures the current time, then each call to
    /// `elapsed` returns a `DeltaTime` containing the amount of time elapsed since its last measured time. This is most
    /// notably used to measure the update times.
    class Timer {
    public:
        /// Resets the last measured time to the current time.
        void reset();

        /// Gets the elapsed time since last measured time.
        [[nodiscard]] DeltaTime elapsed() const;

    private:
        // NOTE: maybe switch to high_resolution_clock?
        using clock = std::chrono::steady_clock;

        std::chrono::time_point<clock> lastTime = clock::now();
    };

    /// @}
} // namespace pom