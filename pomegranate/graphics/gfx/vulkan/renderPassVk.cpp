#include "pch.hpp"

#include "renderPassVk.hpp"

#include "gfxVk.hpp"
#include "instanceVk.hpp"

namespace pom::gfx {
    RenderPassVk::RenderPassVk(InstanceVk* instance,
                               std::initializer_list<RenderPassAttachment> colorAttachments,
                               RenderPassAttachment depthStencilAttachment) :
        instance(instance),
        renderPass(VK_NULL_HANDLE), clearColors(colorAttachments.size() + 1)
    {
        POM_PROFILE_FUNCTION();
        std::vector<VkAttachmentDescription> attachmentDescs(colorAttachments.size() + 1);
        std::vector<VkAttachmentReference> colorAttachmentRefs;

        attachmentDescs[0] = {
            .flags = 0,
            .format = toVkFormat(depthStencilAttachment.format),
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = toVkAttachmentLoadOp(depthStencilAttachment.loadOperation),
            .storeOp = toVkAttachmentStoreOp(depthStencilAttachment.storeOperation),
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE, // FIXME: stencil buffer support.
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        };

        VkAttachmentReference depthStencilAttachmentRef = {
            .attachment = 0,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };

        clearColors[0] = { .depthStencil = { depthStencilAttachment.depthStencilClear.depth,
                                             depthStencilAttachment.depthStencilClear.stencil } };

        u32 i = 1;
        for (auto attachment : colorAttachments) {
            clearColors[i] = {
                .color = { attachment.colorClear.r,
                           attachment.colorClear.g,
                           attachment.colorClear.b,
                           attachment.colorClear.a },
            };

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

            colorAttachmentRefs.push_back({
                .attachment = i,
                .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            });

            i++;
        }

        VkSubpassDescription subpassDesc = {
            .flags = 0,
            .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
            .inputAttachmentCount = 0,
            .pInputAttachments = nullptr,
            .colorAttachmentCount = static_cast<u32>(colorAttachmentRefs.size()),
            .pColorAttachments = colorAttachmentRefs.data(),
            .pResolveAttachments = nullptr,
            .pDepthStencilAttachment = &depthStencilAttachmentRef,
            .preserveAttachmentCount = 0,
            .pPreserveAttachments = nullptr,
        };

        VkSubpassDependency subpassDependency = {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
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

        POM_CHECK_VK(vkCreateRenderPass(instance->getVkDevice(), &renderPassCreateInfo, nullptr, &renderPass),
                     "failed to create swapchain renderpass");
    }

    RenderPassVk::~RenderPassVk()
    {
        POM_PROFILE_FUNCTION();
        vkDestroyRenderPass(instance->getVkDevice(), renderPass, nullptr);
    }
} // namespace pom::gfx