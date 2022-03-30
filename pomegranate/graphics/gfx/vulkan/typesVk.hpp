#pragma once

#include "../buffer.hpp"
#include "../commandBuffer.hpp"
#include "../gfx.hpp"
#include "../pipeline.hpp"
#include "../pipelineLayout.hpp"
#include "../renderPass.hpp"
#include "../shader.hpp"
#include "../texture.hpp"

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

    /// Converts a `BufferUsage` to a `VkBufferUsageFlags`.
    POM_API constexpr VkBufferUsageFlags toVkBufferUsageFlags(BufferUsage u);

    /// Converts a `VkBufferUsageFlags` to a `BufferUsage`.
    POM_API constexpr BufferUsage fromVkBufferUsageFlags(VkBufferUsageFlags u);

    /// Converts a `IndexType` to a `VkIndexType`.
    POM_API constexpr VkIndexType toVkIndexType(IndexType t);

    /// Converts a `VkIndexType` to a `IndexType`.
    POM_API constexpr IndexType fromVkIndexType(VkIndexType t);

    /// Converts a `PrimitiveTopology` to a `VkPrimitiveTopology`.
    POM_API constexpr VkPrimitiveTopology toVkPrimitiveTopology(PrimitiveTopology t);

    /// Converts a `VkPrimitiveTopology` to a `PrimitiveTopology`.
    POM_API constexpr PrimitiveTopology fromVkPrimitiveTopology(VkPrimitiveTopology t);

    /// Converts a `CullMode` to a `VkCullModeFlags`.
    POM_API constexpr VkCullModeFlags toVkCullMode(CullMode c);

    /// Converts a `VkCullModeFlags` to a `CullMode`.
    POM_API constexpr CullMode fromVkCullMode(VkCullModeFlags c);

    /// Converts a `Viewport` to a `VkViewport`.
    POM_API constexpr VkViewport toVkViewport(const Viewport& v);

    /// Converts a `VkViewport` to a `Viewport`.
    POM_API constexpr Viewport fromVkViewport(const VkViewport& v);

    /// Converts a `TextureType` to a `VkImageType`.
    POM_API constexpr VkImageType toVkImageType(TextureType t);

    /// Converts a `VkImageType` to a `TextureType`.
    POM_API constexpr TextureType fromVkImageType(VkImageType t);

    /// Converts a `TextureViewType` to a `VkImageViewType`.
    POM_API constexpr VkImageViewType toVkImageViewType(TextureViewType t);

    /// Converts a `VkImageViewType` to a `TextureViewType`.
    POM_API constexpr TextureViewType fromVkImageViewType(VkImageViewType t);

    // FIXME: This is probably dumb because it can toVkImageUsageFlags(fromVkImageUsageFlags(t)) != t

    /// Converts a `TextureUsage` to a `VkImageUsageFlags`.
    POM_API constexpr VkImageUsageFlags toVkImageUsageFlags(TextureUsage u);

    /// Converts a `VkImageUsageFlags` to a `TextureUsage`.
    POM_API constexpr TextureUsage fromVkImageUsageFlags(VkImageUsageFlags u);

    /// Converts a `DescriptorType` to a `VkDescriptorType`.
    POM_API constexpr VkDescriptorType toVkDescriptorType(DescriptorType t);

    /// Converts a `VkDescriptorType` to a `DescriptorType`.
    POM_API constexpr DescriptorType fromVkDescriptorType(VkDescriptorType t);

    /// Converts a `ShaderStageFlags` to a `VkShaderStageFlags`.
    POM_API constexpr VkShaderStageFlags toVkShaderStageFlags(ShaderStageFlags f);

    /// Converts a `VkShaderStageFlags` to a `ShaderStageFlags`.
    POM_API constexpr ShaderStageFlags fromVkShaderStageFlags(VkShaderStageFlags f);

    /// Converts a `ShaderStageFlags` to a `VkShaderStageFlagBits`.
    POM_API constexpr VkShaderStageFlagBits toVkShaderStageFlagBits(ShaderStageFlags f);

    /// Converts a `VkShaderStageFlagBits` to a `ShaderStageFlags`.
    POM_API constexpr ShaderStageFlags fromVkShaderStageFlagBits(VkShaderStageFlagBits f);

    /// Converts a `PipelineBindPoint` to a `VkPipelineBindPoint`.
    POM_API constexpr VkPipelineBindPoint toVkPipelineBindPoint(PipelineBindPoint p);

    /// Converts a `VkPipelineBindPoint` to a `PipelineBindPoint`.
    POM_API constexpr PipelineBindPoint fromVkPipelineBindPoint(VkPipelineBindPoint p);

    /// Returns the theoretically ideal image layout from a `TextureUsage`. This may vary from the actual best layout
    /// for a given image at a given time but its good enough.
    POM_API constexpr VkImageLayout getTheoreticallyIdealImageLayout(TextureUsage u);

    /// Converts a `VkResult` to a string.
    POM_API constexpr const char* fromVkResultToString(VkResult r);

    /// @}
} // namespace pom::gfx