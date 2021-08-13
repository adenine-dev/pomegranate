#pragma once

#include "../buffer.hpp"
#include "../commandBuffer.hpp"
#include "../gfx.hpp"
#include "../pipeline.hpp"
#include "../renderPass.hpp"
#include "../shader.hpp"

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

    /// Converts a `ShaderStage` to a `VkShaderStageFlagBits`.
    POM_API constexpr VkShaderStageFlagBits toVkShaderStageFlagBits(ShaderStage s);

    /// Converts a `VkShaderStageFlagBits` to a `ShaderStage`.
    POM_API constexpr ShaderStage fromVkShaderStageFlagBits(VkShaderStageFlagBits s);

    /// Converts a `PrimitiveTopology` to a `VkPrimitiveTopology`.
    POM_API constexpr VkPrimitiveTopology toVkPrimitiveTopology(PrimitiveTopology t);

    /// Converts a `VkPrimitiveTopology` to a `PrimitiveTopology`.
    POM_API constexpr PrimitiveTopology fromVkPrimitiveTopology(VkPrimitiveTopology t);

    POM_API constexpr VkCullModeFlags toVkCullMode(CullMode c);

    POM_API constexpr CullMode fromVkCullMode(VkCullModeFlags c);

    /// Converts a `VkResult` to a string.
    POM_API constexpr const char* fromVkResultToString(VkResult r);

    /// @}
} // namespace pom::gfx