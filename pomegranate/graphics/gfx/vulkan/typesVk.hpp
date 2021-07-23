#pragma once

#include "../gfx.hpp"

namespace pom::gfx {
    /// @addtogroup vulkan
    /// @{

    /// Converts a Format to a VkFormat.
    POM_API constexpr VkFormat toVkFormat(Format f);

    /// Converts a VkFormat to a Format
    POM_API constexpr Format fromVkFormat(VkFormat f);

    /// @}
} // namespace pom::gfx