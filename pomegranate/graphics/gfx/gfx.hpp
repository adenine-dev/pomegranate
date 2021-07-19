#pragma once

#include "base.hpp"

namespace pom::gfx {
    /// @addtogroup gfx
    /// @{

    /// Determines the low level graphics api. Currently only supports vulkan.
    enum class GraphicsAPI { VULKAN };

    /// shuts down any instance realted things.
    POM_API void shutdown();

    /// @}
} // namespace pom::gfx