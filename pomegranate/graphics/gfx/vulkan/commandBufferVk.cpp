#include "pch.hpp"

#include "commandBufferVk.hpp"

#include "bufferVk.hpp"
#include "contextVk.hpp"
#include "instanceVk.hpp"

namespace pom::gfx {
    CommandBufferVk::CommandBufferVk(CommandBufferSpecialization specialization,
                                     ContextVk* context,
                                     InstanceVk* instance,
                                     VkCommandPool pool,
                                     u32 count) :
        CommandBuffer(specialization),
        context(context), instance(instance), commandBuffers(count), recordingFences(count),
        currentIndex(context->getSwapchainImageIndex())
    {
        VkFenceCreateInfo fenceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };

        VkCommandBufferAllocateInfo commandBufferAllocateCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY, // TODO: secondary command buffers how? doesn't make sense in gl?
            .commandBufferCount = count,
        };

        for (u32 i = 0; i < count; i++) {
            POM_ASSERT(vkCreateFence(instance->device, &fenceCreateInfo, nullptr, &recordingFences[i]) == VK_SUCCESS,
                       "Failed to create recording fence");

            POM_ASSERT(
                vkAllocateCommandBuffers(instance->device, &commandBufferAllocateCreateInfo, commandBuffers.data())
                    == VK_SUCCESS,
                "Failed to allocate command buffers.");
        }
    }

    CommandBufferVk::~CommandBufferVk()
    {
        for (auto& recordingFence : recordingFences) {
            vkDestroyFence(instance->device, recordingFence, nullptr);
        }
    }

    void CommandBufferVk::begin()
    {
        // NOTE: vkAcquireNextImageKHR doesn't guarantee images are processed in order so we can't simply increment
        // this. maybe change it to get the oldest (and therefore most likely to be done) command buffer. This is the
        // only thing that locks it into being Context specific, and it may be worth recondiering this choice.
        currentIndex = context->getSwapchainImageIndex();

        vkWaitForFences(instance->device, 1, &getCurrentRecordingFence(), VK_TRUE, UINT32_MAX);
        vkResetFences(instance->device, 1, &getCurrentRecordingFence());

        VkCommandBufferBeginInfo commandBufferBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr,
        };

        POM_ASSERT(vkBeginCommandBuffer(getCurrentCommandBuffer(), &commandBufferBeginInfo) == VK_SUCCESS,
                   "Failed to begin recording command buffer.");
    }

    void CommandBufferVk::beginRenderPass(RenderPass* renderPass)
    {
        POM_ASSERT(renderPass->getAPI() == GraphicsAPI::VULKAN, "Attempting to use mismatched render pass api.");

        auto* rp = dynamic_cast<RenderPassVk*>(renderPass);

        VkRenderPassBeginInfo renderPassBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = rp->getHandle(),
            .framebuffer = context->swapchainFramebuffers[context->swapchainImageIndex],
            .renderArea = { .offset = { 0, 0 }, .extent = context->swapchainExtent },
            .clearValueCount = rp->getClearColorCountVk(),
            .pClearValues = rp->getClearColorsVk(),
        };

        vkCmdBeginRenderPass(getCurrentCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void CommandBufferVk::endRenderPass()
    {
        vkCmdEndRenderPass(getCurrentCommandBuffer());
    }

    void CommandBufferVk::end()
    {
        POM_ASSERT(vkEndCommandBuffer(getCurrentCommandBuffer()) == VK_SUCCESS,
                   "Failed to end recording command buffer.");
    }

    void CommandBufferVk::setViewport(const maths::vec2& offset, const maths::vec2& extent, f32 mindepth, f32 maxdepth)
    {
        VkViewport viewport { offset.x, offset.y, extent.x, extent.y, mindepth, maxdepth };
        vkCmdSetViewport(getCurrentCommandBuffer(), 0, 1, &viewport);
    }

    void CommandBufferVk::setScissor(const maths::ivec2& offset, const maths::uvec2& extent)
    {
        VkRect2D scissor { .offset = { offset.x, offset.y }, .extent = { extent.x, extent.y } };
        vkCmdSetScissor(getCurrentCommandBuffer(), 0, 1, &scissor);
    }

    void CommandBufferVk::draw(u32 vertexCount, u32 vertexOffset)
    {
        vkCmdDraw(getCurrentCommandBuffer(), vertexCount, 1, vertexOffset, 0);
    }

    void CommandBufferVk::drawIndexed(u32 indexCount, u32 firstIndex, i32 vertexOffset)
    {
        vkCmdDrawIndexed(getCurrentCommandBuffer(), indexCount, 1, firstIndex, vertexOffset, 0);
    }

    void CommandBufferVk::bindVertexBuffer(Buffer* vertexBuffer, u32 bindPoint, size_t offset)
    {
        POM_ASSERT(vertexBuffer->getAPI() == GraphicsAPI::VULKAN, "Attempting to use mismatched vertex buffer api");
        POM_ASSERT(vertexBuffer->getUsage() & BufferUsage::VERTEX,
                   "Attempting to use a buffer created without the vertex BufferUsage.")

        VkBuffer buffer = dynamic_cast<BufferVk*>(vertexBuffer)->getBuffer();
        vkCmdBindVertexBuffers(getCurrentCommandBuffer(), bindPoint, 1, &buffer, &offset);
    }

    void CommandBufferVk::bindIndexBuffer(Buffer* indexBuffer, IndexType type, size_t offset)
    {
        POM_ASSERT(indexBuffer->getAPI() == GraphicsAPI::VULKAN, "Attempting to use mismatched index buffer api");
        POM_ASSERT(indexBuffer->getUsage() & BufferUsage::INDEX,
                   "Attempting to use a buffer created without the index BufferUsage.")

        vkCmdBindIndexBuffer(getCurrentCommandBuffer(),
                             dynamic_cast<BufferVk*>(indexBuffer)->getBuffer(),
                             offset,
                             toVkIndexType(type));
    }

} // namespace pom::gfx