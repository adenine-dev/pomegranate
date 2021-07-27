#pragma once

#include "base.hpp"

#include "../commandBuffer.hpp"

namespace pom::gfx {
    /// @addtogroup vulkan
    /// @{

    /// Vulkan CommandBuffer, should be created implicitly through `Context::createCommandBuffer`. Contains a list of
    /// vulkan command buffers and fences corresponding to the number of swapchain images.
    class POM_API CommandBufferVk final : public CommandBuffer {
    public:
        ~CommandBufferVk();

        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        void begin() final;

        void beginRenderPass(RenderPass* renderPass) final;

        void endRenderPass() final;

        void end() final;

        [[nodiscard]] inline VkCommandBuffer& getCurrentCommandBuffer()
        {
            return commandBuffers[currentIndex];
        }

        [[nodiscard]] inline VkFence& getCurrentRecordingFence()
        {
            return recordingFences[currentIndex];
        }

    private:
        friend class ContextVk;

        CommandBufferVk(ContextVk* context, VkDevice device, VkCommandPool pool, u32 count);

        ContextVk* context;
        VkDevice device;
        std::vector<VkCommandBuffer> commandBuffers;
        std::vector<VkFence> recordingFences;
        u32 currentIndex;
    };

    /// @}
} // namespace pom::gfx