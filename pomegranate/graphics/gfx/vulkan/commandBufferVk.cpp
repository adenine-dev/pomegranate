#include "pch.hpp"

#include "commandBufferVk.hpp"

#include "bufferVk.hpp"
#include "contextVk.hpp"
#include "gfxVk.hpp"
#include "instanceVk.hpp"
#include "pipelineVk.hpp"

namespace pom::gfx {
    CommandBufferVk::CommandBufferVk(InstanceVk* instance, CommandBufferSpecialization specialization, u32 count) :
        CommandBuffer(specialization), instance(instance),
        pool(specialization == CommandBufferSpecialization::TRANSFER ? instance->transferCommandPool
                                                                     : instance->graphicsCommandPool),
        count(count), currentIndex(0)
    {
        POM_ASSERT(count <= POM_MAX_FRAMES_IN_FLIGHT,
                   "Command buffers are only meant to hold less than the number of frames in flight, try using "
                   "multiple command buffers.");

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
            POM_CHECK_VK(vkCreateFence(instance->device, &fenceCreateInfo, nullptr, &recordingFences[i]),
                         "Failed to create recording fence");

            POM_CHECK_VK(vkAllocateCommandBuffers(instance->device, &commandBufferAllocateCreateInfo, commandBuffers),
                         "Failed to allocate command buffers.");
        }
    }

    CommandBufferVk::~CommandBufferVk()
    {
        vkWaitForFences(instance->device, count, recordingFences, VK_TRUE, UINT64_MAX);
        for (auto& recordingFence : recordingFences) {
            vkDestroyFence(instance->device, recordingFence, nullptr);
        }

        vkFreeCommandBuffers(instance->device, pool, count, commandBuffers);
    }

    void CommandBufferVk::begin()
    {
        currentIndex = (currentIndex + 1) % count;

        vkWaitForFences(instance->device, 1, &getCurrentRecordingFence(), VK_TRUE, UINT32_MAX);
        vkResetFences(instance->device, 1, &getCurrentRecordingFence());

        VkCommandBufferBeginInfo commandBufferBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pInheritanceInfo = nullptr,
        };

        POM_CHECK_VK(vkBeginCommandBuffer(getCurrentCommandBuffer(), &commandBufferBeginInfo),
                     "Failed to begin recording command buffer.");
    }

    void CommandBufferVk::beginRenderPass(RenderPass* renderPass, Context* context)
    {
        POM_ASSERT(renderPass->getAPI() == GraphicsAPI::VULKAN, "Attempting to use mismatched render pass api.");
        POM_ASSERT(context->getAPI() == GraphicsAPI::VULKAN, "Attempting to use mismatched context api.");
        POM_ASSERT(specialization == CommandBufferSpecialization::GRAPHICS,
                   "Attempting to use graphics command with a command buffer without that ability.");

        auto* rp = dynamic_cast<RenderPassVk*>(renderPass);
        auto* ctx = dynamic_cast<ContextVk*>(context);

        VkRenderPassBeginInfo renderPassBeginInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .pNext = nullptr,
            .renderPass = rp->getHandle(),
            .framebuffer = ctx->swapchainFramebuffers[ctx->swapchainImageIndex],
            .renderArea = { .offset = { 0, 0 }, .extent = ctx->swapchainExtent },
            .clearValueCount = rp->getClearColorCountVk(),
            .pClearValues = rp->getClearColorsVk(),
        };

        vkCmdBeginRenderPass(getCurrentCommandBuffer(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void CommandBufferVk::endRenderPass()
    {
        POM_ASSERT(specialization == CommandBufferSpecialization::GRAPHICS,
                   "Attempting to use graphics command with a command buffer without that ability.");

        vkCmdEndRenderPass(getCurrentCommandBuffer());
    }

    void CommandBufferVk::end()
    {
        POM_CHECK_VK(vkEndCommandBuffer(getCurrentCommandBuffer()), "Failed to end recording command buffer.");
    }

    void CommandBufferVk::submit()
    {
        VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 0,
            .pWaitSemaphores = nullptr,
            .pWaitDstStageMask = nullptr,
            .commandBufferCount = 1,
            .pCommandBuffers = &getCurrentCommandBuffer(),
            .signalSemaphoreCount = 0,
            .pSignalSemaphores = nullptr,
        };

        VkQueue queue = specialization == CommandBufferSpecialization::TRANSFER ? instance->transferQueue
                                                                                : instance->graphicsQueue;

        POM_CHECK_VK(vkQueueSubmit(queue, 1, &submitInfo, getCurrentRecordingFence()), "Failed to submit to queue");
    }

    void CommandBufferVk::setViewport(const maths::vec2& offset, const maths::vec2& extent, f32 mindepth, f32 maxdepth)
    {
        POM_ASSERT(specialization == CommandBufferSpecialization::GRAPHICS,
                   "Attempting to use graphics command with a command buffer without that ability.");

        VkViewport viewport { offset.x, offset.y, extent.x, extent.y, mindepth, maxdepth };
        vkCmdSetViewport(getCurrentCommandBuffer(), 0, 1, &viewport);
    }

    void CommandBufferVk::setScissor(const maths::ivec2& offset, const maths::uvec2& extent)
    {
        POM_ASSERT(specialization == CommandBufferSpecialization::GRAPHICS,
                   "Attempting to use graphics command with a command buffer without that ability.");

        VkRect2D scissor { .offset = { offset.x, offset.y }, .extent = { extent.x, extent.y } };
        vkCmdSetScissor(getCurrentCommandBuffer(), 0, 1, &scissor);
    }

    void CommandBufferVk::draw(u32 vertexCount, u32 vertexOffset)
    {
        POM_ASSERT(specialization == CommandBufferSpecialization::GRAPHICS,
                   "Attempting to use graphics command with a command buffer without that ability.");

        vkCmdDraw(getCurrentCommandBuffer(), vertexCount, 1, vertexOffset, 0);
    }

    void CommandBufferVk::drawIndexed(u32 indexCount, u32 firstIndex, i32 vertexOffset)
    {
        POM_ASSERT(specialization == CommandBufferSpecialization::GRAPHICS,
                   "Attempting to use graphics command with a command buffer without that ability.");

        vkCmdDrawIndexed(getCurrentCommandBuffer(), indexCount, 1, firstIndex, vertexOffset, 0);
    }

    void CommandBufferVk::bindVertexBuffer(Buffer* vertexBuffer, u32 bindPoint, size_t offset)
    {
        POM_ASSERT(vertexBuffer->getAPI() == GraphicsAPI::VULKAN, "Attempting to use mismatched vertex buffer api");
        POM_ASSERT(specialization == CommandBufferSpecialization::GRAPHICS,
                   "Attempting to use graphics command with a command buffer without that ability.");
        POM_ASSERT(vertexBuffer->getUsage() & BufferUsage::VERTEX,
                   "Attempting to use a buffer created without the vertex BufferUsage.")

        VkBuffer buffer = dynamic_cast<BufferVk*>(vertexBuffer)->getBuffer();
        vkCmdBindVertexBuffers(getCurrentCommandBuffer(), bindPoint, 1, &buffer, &offset);
    }

    void CommandBufferVk::bindIndexBuffer(Buffer* indexBuffer, IndexType type, size_t offset)
    {
        POM_ASSERT(indexBuffer->getAPI() == GraphicsAPI::VULKAN, "Attempting to use mismatched index buffer api");
        POM_ASSERT(specialization == CommandBufferSpecialization::GRAPHICS,
                   "Attempting to use graphics command with a command buffer without that ability.");
        POM_ASSERT(indexBuffer->getUsage() & BufferUsage::INDEX,
                   "Attempting to use a buffer created without the index BufferUsage.")

        vkCmdBindIndexBuffer(getCurrentCommandBuffer(),
                             dynamic_cast<BufferVk*>(indexBuffer)->getBuffer(),
                             offset,
                             toVkIndexType(type));
    }

    void CommandBufferVk::bindPipeline(Pipeline* pipeline)
    {
        POM_ASSERT(pipeline->getAPI() == GraphicsAPI::VULKAN, "Attempting to use mismatched index buffer api");
        POM_ASSERT(specialization == CommandBufferSpecialization::GRAPHICS,
                   "Attempting to use graphics command with a command buffer without that ability.");

        vkCmdBindPipeline(getCurrentCommandBuffer(),
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          dynamic_cast<PipelineVk*>(pipeline)->getHandle());
    }

    void CommandBufferVk::copyBuffer(Buffer* src, Buffer* dst, size_t size, size_t srcOffset, size_t dstOffset)
    {
        POM_ASSERT(src->getAPI() == GraphicsAPI::VULKAN, "Attempting to use mismatched buffer api");
        POM_ASSERT(dst->getAPI() == GraphicsAPI::VULKAN, "Attempting to use mismatched buffer api");
        POM_ASSERT(src->getSize() - srcOffset >= size, "Attempting to copy from a buffer of insufficient size.");
        POM_ASSERT(dst->getSize() - dstOffset >= size, "Attempting to copy into a buffer of insufficient size.");

        VkBuffer srcBuffer = dynamic_cast<BufferVk*>(src)->getBuffer();
        VkBuffer dstBuffer = dynamic_cast<BufferVk*>(dst)->getBuffer();

        VkBufferCopy region = {
            .srcOffset = srcOffset,
            .dstOffset = dstOffset,
            .size = size,
        };

        vkCmdCopyBuffer(getCurrentCommandBuffer(), srcBuffer, dstBuffer, 1, &region);
    }

} // namespace pom::gfx