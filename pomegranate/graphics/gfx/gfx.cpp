#include "pch.hpp"

#include "gfx.hpp"
#include "instance.hpp"

namespace pom::gfx {
    void shutdown()
    {
        Instance::destroy();
    }
} // namespace pom::gfx