#include "pch.hpp"

#include "core.hpp"

#include "graphics/gfx/gfx.hpp"
#include "platform/platform.hpp"

namespace pom {
    bool init()
    {
        return platform::init();
    }

    void shutdown()
    {
        gfx::shutdown();
    }
} // namespace pom