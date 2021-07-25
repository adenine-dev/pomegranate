#pragma once

#include "../gfx.hpp"
#include "../renderPass.hpp"

namespace pom::gfx {
    /// @addtogroup vulkan
    /// @{

    /// Converts a `Format` to a `VkFormat`.
    POM_API constexpr VkFormat toVkFormat(Format f);

    /// Converts a `VkFormat` to a `Format`.
    POM_API constexpr Format fromVkFormat(VkFormat f);

    /// Converts a `LoadOperation` to a `VkAttachmentLoadOp`.
    POM_API constexpr VkAttachmentLoadOp toVkAttachmentLoadOp(LoadOperation o);

    /// Converts a `VkAttachmentLoadOp` to a `LoadOperation`.
    POM_API constexpr LoadOperation fromVkAttachmentLoadOp(VkAttachmentLoadOp o);

    /// Converts a `StoreOperation` to a `VkAttachmentStoreOp`.
    POM_API constexpr VkAttachmentStoreOp toVkAttachmentStoreOp(StoreOperation o);

    /// Converts a `VkAttachmentStoreOp` to a `StoreOperation`.
    POM_API constexpr StoreOperation fromVkAttachmentStoreOp(VkAttachmentStoreOp o);

    /// @}
} // namespace pom::gfx