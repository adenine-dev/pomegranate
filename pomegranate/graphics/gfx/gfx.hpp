#pragma once

#include "base.hpp"

namespace pom::gfx {
    /// @addtogroup gfx
    /// @{

    /// Determines the low level graphics api. Currently only supports vulkan.
    enum class GraphicsAPI { VULKAN };

    /// Shuts down all instance realted things, all Contexts and resources should be destroyed before calling this.
    POM_API void shutdown();

    /// @}
} // namespace pom::gfx