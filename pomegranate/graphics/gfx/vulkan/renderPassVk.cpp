#include "pch.hpp"

#include "renderPassVk.hpp"

#include "instanceVk.hpp"

namespace pom::gfx {
    RenderPassVk::RenderPassVk(InstanceVk* instance, std::initializer_list<RenderPassAttachment> attachments) :
        instance(instance), renderPass(VK_NULL_HANDLE)
    {
        std::vector<VkAttachmentDescription> attachmentDescs(attachments.size());

        u32 i = 0;
        for (auto attachment : attachments) {
            attachmentDescs[i] = {
                .flags = 0,
                .format = toVkFormat(attachment.format),
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .loadOp = toVkAttachmentLoadOp(attachment.loadOperation),
                .storeOp = toVkAttachmentStoreOp(attachment.storeOperation),
                .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, // FIXME: stencil buffer support.
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            };

            i++;
        }

        VkAttachmentReference colorAttachmentRef = {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };

        VkSubpassDescription subpassDesc = {
            .flags = 0,
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .inputAttachmentCount = 0,
            .pInputAttachments = nullptr,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachmentRef,
            .pResolveAttachments = nullptr,
            .pDepthStencilAttachment = nullptr,
            .preserveAttachmentCount = 0,
            .pPreserveAttachments = nullptr,
        };

        VkSubpassDependency subpassDependency = {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = 0,
        };

        VkRenderPassCreateInfo renderPassCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .attachmentCount = static_cast<u32>(attachmentDescs.size()),
            .pAttachments = attachmentDescs.data(),
            .subpassCount = 1,
            .pSubpasses = &subpassDesc,
            .dependencyCount = 1,
            .pDependencies = &subpassDependency,
        };

        POM_ASSERT(vkCreateRenderPass(instance->device, &renderPassCreateInfo, nullptr, &renderPass) == VK_SUCCESS,
                   "failed to create swapchain renderpass");
    }

    RenderPassVk::~RenderPassVk()
    {
        vkDestroyRenderPass(instance->device, renderPass, nullptr);
    }
} // namespace pom::gfx