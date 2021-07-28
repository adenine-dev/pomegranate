#pragma once

#include "../buffer.hpp"
#include "../commandBuffer.hpp"
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

    /// Converts a `BufferUsage` to a `VkAttachmentStoreOp`.
    POM_API constexpr VkBufferUsageFlags toVkBufferUsageFlags(BufferUsage u);

    /// Converts a `VkBufferUsageFlags` to a `BufferUsage`.
    POM_API constexpr BufferUsage fromVkBufferUsageFlags(VkBufferUsageFlags u);

    /// Converts a `IndexType` to a `VkIndexType`.
    POM_API constexpr VkIndexType toVkIndexType(IndexType t);

    /// Converts a `VkIndexType` to a `IndexType`.
    POM_API constexpr IndexType fromVkIndexType(VkIndexType t);

    /// @}
} // namespace pom::gfx