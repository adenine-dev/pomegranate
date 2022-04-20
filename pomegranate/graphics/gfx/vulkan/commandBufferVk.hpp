#pragma once

#include "base.hpp"

#include "../commandBuffer.hpp"

#include "instanceVk.hpp"

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

        void beginRenderPass(Ref<RenderPass> renderPass, Context* context) final;

        void endRenderPass() final;

        void end() final;

        void submit() final;

        void setViewport(const Viewport& viewport) final;

        void setScissor(const maths::ivec2& offset, const maths::uvec2& extent) final;

        void draw(u32 vertexCount, u32 vertexOffset = 0) final;

        void drawInstanced(u32 vertexCount, u32 instanceCount, u32 vertexOffset = 0, u32 instanceOffset = 0) final;

        void drawIndexed(u32 indexCount, u32 firstIndex = 0, i32 vertexOffset = 0) final;

        void dispatch(u32 groupCountX, u32 groupCountY = 1, u32 groupCountZ = 1) final;

        void bindVertexBuffer(Ref<Buffer> vertexBuffer, u32 bindPoint = 0, usize offset = 0) final;

        void bindIndexBuffer(Ref<Buffer> indexBuffer, IndexType type, usize offset = 0) final;

        void bindPipeline(Ref<Pipeline> pipeline) final;

        void bindDescriptorSet(PipelineBindPoint bindPoint,
                               Ref<PipelineLayout> pipelineLayout,
                               u32 set,
                               Ref<DescriptorSet> descriptorSet) final;

        void setPushConstants(Ref<PipelineLayout> pipelineLayout,
                              ShaderStageFlags stages,
                              usize size,
                              const void* data,
                              usize offset = 0) final;

        void copyBufferToBuffer(Buffer* src, Buffer* dst, usize size, usize srcOffset, usize dstOffset) final;

        void copyBufferToTexture(Buffer* src,
                                 Texture* dst,
                                 usize size,
                                 usize srcOffset,
                                 const maths::ivec3& dstOffset,
                                 const maths::uvec3& dstExtent) final;

        void
        transitionImageLayoutVk(TextureVk* texture, VkImageLayout oldLayout, VkImageLayout newLayout, u32 mips = 1);

        [[nodiscard]] inline VkCommandBuffer& getCurrentCommandBuffer()
        {
            return commandBuffers[instance->getCurrentFrameIndex() % count];
        }

        [[nodiscard]] inline VkFence& getCurrentRecordingFence()
        {
            return recordingFences[instance->getCurrentFrameIndex() % count];
        }

    protected:
        friend class ContextVk;

        InstanceVk* instance;
        VkCommandPool pool;
        u32 count;
        VkCommandBuffer commandBuffers[POM_MAX_FRAMES_IN_FLIGHT] {};
        VkFence recordingFences[POM_MAX_FRAMES_IN_FLIGHT] {};
    };

    /// @}
} // namespace pom::gfx