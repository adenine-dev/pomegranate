#pragma once

#include "base.hpp"

#include "../commandBuffer.hpp"

namespace pom::gfx {
    class InstanceVk;
    class TextureVk;

    /// @addtogroup vulkan
    /// @{

    /// Vulkan CommandBuffer, should be created implicitly through `Context::createCommandBuffer`. Contains a list of
    /// vulkan command buffers and fences corresponding to the number of swapchain images.
    class POM_API CommandBufferVk final : public CommandBuffer {
    public:
        CommandBufferVk(InstanceVk* instance, CommandBufferSpecialization specialization, u32 count);
        ~CommandBufferVk() final;

        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        void begin() final;

        void beginRenderPass(RenderPass* renderPass, Context* context) final;

        void endRenderPass() final;

        void end() final;

        void submit() final;

        void setViewport(const Viewport& viewport) final;

        void setScissor(const maths::ivec2& offset, const maths::uvec2& extent) final;

        void draw(u32 vertexCount, u32 vertexOffset = 0) final;

        void drawIndexed(u32 indexCount, u32 firstIndex = 0, i32 vertexOffset = 0) final;

        void bindVertexBuffer(Buffer* vertexBuffer, u32 bindPoint = 0, size_t offset = 0) final;

        void bindIndexBuffer(Buffer* indexBuffer, IndexType type, size_t offset = 0) final;

        void bindPipeline(Pipeline* pipeline) final;

        void bindDescriptorSet(PipelineLayout* pipelineLayout, u32 set, DescriptorSet* descriptorSet) final;

        void copyBuffer(Buffer* src, Buffer* dst, size_t size, size_t srcOffset, size_t dstOffset) final;

        void copyBufferToTexture(Buffer* src,
                                 Texture* dst,
                                 size_t size,
                                 size_t srcOffset,
                                 maths::ivec3 dstOffset,
                                 maths::uvec3 dstExtent) final;

        void transitionImageLayoutVk(TextureVk* texture, VkImageLayout oldLayout, VkImageLayout newLayout);

        [[nodiscard]] inline VkCommandBuffer& getCurrentCommandBuffer()
        {
            return commandBuffers[currentIndex];
        }

        [[nodiscard]] inline VkFence& getCurrentRecordingFence()
        {
            return recordingFences[currentIndex];
        }

    protected:
        friend class ContextVk;

        InstanceVk* instance;
        VkCommandPool pool;
        u32 count;
        VkCommandBuffer commandBuffers[POM_MAX_FRAMES_IN_FLIGHT] {};
        VkFence recordingFences[POM_MAX_FRAMES_IN_FLIGHT] {};
        u32 currentIndex;
    };

    /// @}
} // namespace pom::gfx