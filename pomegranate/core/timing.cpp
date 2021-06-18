#include "pch.hpp"

#include "timing.hpp"

namespace pom {
    f32 DeltaTime::sec() const
    {
        return std::chrono::duration_cast<std::chrono::duration<float>>(delta).count();
    }

    f32 DeltaTime::milli() const
    {
        return std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(delta).count();
    }

    f32 DeltaTime::micro() const
    {
        return std::chrono::duration_cast<std::chrono::duration<float, std::micro>>(delta).count();
    }

    f32 DeltaTime::nano() const
    {
        return delta.count();
    }

    DeltaTime::operator f32() const
    {
        return milli();
    };

    void Timer::reset()
    {
        lastTime = clock::now();
    }

    DeltaTime Timer::elapsed() const
    {
        return { std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - lastTime) };
    }

} // namespace pom