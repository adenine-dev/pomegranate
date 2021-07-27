#include "pch.hpp"

#include "commandBufferVk.hpp"

#include "contextVk.hpp"

namespace pom::gfx {
    CommandBufferVk::CommandBufferVk(CommandBufferSpecialization specialization,
                                     ContextVk* context,
                                     VkDevice device,
                                     VkCommandPool pool,
                                     u32 count) :
        CommandBuffer(specialization),
        context(context), device(device), commandBuffers(count), recordingFences(count),
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
            POM_ASSERT(vkCreateFence(device, &fenceCreateInfo, nullptr, &recordingFences[i]) == VK_SUCCESS,
                       "Failed to create recording fence");

            POM_ASSERT(vkAllocateCommandBuffers(device, &commandBufferAllocateCreateInfo, commandBuffers.data())
                           == VK_SUCCESS,
                       "Failed to allocate command buffers.");
        }
    }

    CommandBufferVk::~CommandBufferVk()
    {
        for (auto& recordingFence : recordingFences) {
            vkDestroyFence(device, recordingFence, nullptr);
        }
    }

    void CommandBufferVk::begin()
    {
        // NOTE: vkAcquireNextImageKHR doesn't guarantee images are processed in order so we can't simply increment
        // this. maybe change it to get the oldest (and therefore most likely to be done) command buffer. This is the
        // only thing that locks it into being Context specific, and it may be worth recondiering this choice.
        currentIndex = context->getSwapchainImageIndex();

        vkWaitForFences(device, 1, &getCurrentRecordingFence(), VK_TRUE, UINT32_MAX);
        vkResetFences(device, 1, &getCurrentRecordingFence());

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

} // namespace pom::gfx