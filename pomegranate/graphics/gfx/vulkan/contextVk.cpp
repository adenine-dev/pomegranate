#include "pch.hpp"

#include "contextVk.hpp"

#include "commandBufferVk.hpp"
#include "gfxVk.hpp"

namespace pom::gfx {
    ContextVk::ContextVk(InstanceVk* instance, Window* window) :
        Context(window), instance(instance), surface(window->getVulkanSurface(instance->instance))
    {
        POM_PROFILE_FUNCTION();
        if (!instance->ready()) {
            instance->determineGPU(this);
        } else {
            VkBool32 supported = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(instance->physicalDevice,
                                                 instance->presentQueueFamilyIndex,
                                                 surface,
                                                 &supported);
            POM_ASSERT(supported, "idk. it a failure tho");
        }

        createSwapchain(window->getVulkanDrawableExtent(), true);

        imagesInFlight.resize(swapchainImages.size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
        };

        VkFenceCreateInfo fenceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };

        for (u32 i = 0; i < POM_MAX_FRAMES_IN_FLIGHT; i++) {
            POM_CHECK_VK(
                vkCreateSemaphore(instance->device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]),
                "Failed to create image available semaphore");

            POM_CHECK_VK(
                vkCreateSemaphore(instance->device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]),
                "Failed to create render finish semaphore");

            POM_CHECK_VK(vkCreateFence(instance->device, &fenceCreateInfo, nullptr, &inFlightFences[i]),
                         "Failed to create fence");
        }

        vkResetFences(instance->device, 1, &inFlightFences[frameIndex]);

        acquireNextSwapchainImage();
    }

    ContextVk::~ContextVk()
    {
        POM_PROFILE_FUNCTION();
        vkDeviceWaitIdle(instance->device);

        vkDestroyImageView(instance->device, depthImageView, nullptr);
        vkDestroyImage(instance->device, depthImage, nullptr);
        vkFreeMemory(instance->device, depthImageMemory, nullptr);

        for (u32 i = 0; i < POM_MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(instance->device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(instance->device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(instance->device, inFlightFences[i], nullptr);
        }

        for (auto* imageView : swapchainImageViews) {
            vkDestroyImageView(instance->device, imageView, nullptr);
        }

        for (auto* framebuffer : swapchainFramebuffers) {
            vkDestroyFramebuffer(instance->device, framebuffer, nullptr);
        }

        vkDestroySwapchainKHR(instance->device, swapchain, nullptr);

        vkDestroySurfaceKHR(instance->instance, surface, nullptr);
    }

    bool hasStencilComponent(VkFormat format)
    {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

    VkFormat ContextVk::findSupportedFormat(const std::vector<VkFormat>& candidates,
                                            VkImageTiling tiling,
                                            VkFormatFeatureFlags features) const
    {
        POM_PROFILE_FUNCTION();
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(instance->physicalDevice, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            }
            if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        POM_FATAL("Unable to find suitable format");
        return VK_FORMAT_UNDEFINED;
    }

    SwapchainSupportDetailsVk ContextVk::getSwapchainSupportDetails(VkPhysicalDevice device) const
    {
        POM_PROFILE_FUNCTION();
        SwapchainSupportDetailsVk swapchainSupport;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapchainSupport.capabilities);

        u32 formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            swapchainSupport.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, swapchainSupport.formats.data());
        }

        u32 presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            swapchainSupport.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device,
                                                      surface,
                                                      &presentModeCount,
                                                      swapchainSupport.presentModes.data());
        }

        return swapchainSupport;
    }

    void ContextVk::createSwapchain(const VkExtent2D& extent, bool firstTime)
    {
        POM_PROFILE_FUNCTION();
        SwapchainSupportDetailsVk swapchainSupport = getSwapchainSupportDetails(instance->physicalDevice);

        VkSurfaceFormatKHR surfaceFormat = swapchainSupport.formats[0];
        for (const auto& format : swapchainSupport.formats) {
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                surfaceFormat = format;
                break;
            }
        }

        swapchainImageFormat = surfaceFormat.format;

        VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
        if (window->isVsync()) {
            for (const auto& mode : swapchainSupport.presentModes) {
                if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                    presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                    break;
                }
            }
        } else {
            for (const auto& mode : swapchainSupport.presentModes) {
                if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
                    presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
                    break;
                }
            }
        }

        // vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &support.capabilities);

        swapchainExtent = swapchainSupport.capabilities.currentExtent;
        if (swapchainExtent.width == UINT32_MAX) {
            swapchainExtent = extent;

            swapchainExtent.width = std::clamp(swapchainExtent.width,
                                               swapchainSupport.capabilities.minImageExtent.width,
                                               swapchainSupport.capabilities.maxImageExtent.width);
            swapchainExtent.height = std::clamp(swapchainExtent.height,
                                                swapchainSupport.capabilities.minImageExtent.height,
                                                swapchainSupport.capabilities.maxImageExtent.height);
        }

        swapchainViewport = {
            .x = 0.f,
            .y = 0.f,
            .width = (float)swapchainExtent.width,
            .height = (float)swapchainExtent.height,
            .minDepth = 0.f,
            .maxDepth = 1.f,
        };

        u32 imageCount = swapchainSupport.capabilities.maxImageCount == 0
                             ? swapchainSupport.capabilities.minImageCount + 1
                             : std::min(std::max(swapchainSupport.capabilities.minImageCount + 1, 3u),
                                        swapchainSupport.capabilities.maxImageCount);

        u32 queueFamilyIndicies[] = { instance->graphicsQueueFamilyIndex, instance->presentQueueFamilyIndex };
        bool sharedQueueFamily = instance->graphicsQueueFamilyIndex == instance->presentQueueFamilyIndex;
        VkSwapchainCreateInfoKHR swapchainCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .surface = surface,
            .minImageCount = imageCount,
            .imageFormat = surfaceFormat.format,
            .imageColorSpace = surfaceFormat.colorSpace,
            .imageExtent = swapchainExtent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = sharedQueueFamily ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
            .queueFamilyIndexCount = sharedQueueFamily ? 0u : 2u,
            .pQueueFamilyIndices = sharedQueueFamily ? nullptr : queueFamilyIndicies,
            .preTransform = swapchainSupport.capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = presentMode,
            .clipped = VK_TRUE,
            .oldSwapchain = firstTime ? nullptr : swapchain,
        };

        POM_CHECK_VK(vkCreateSwapchainKHR(instance->device, &swapchainCreateInfo, nullptr, &swapchain),
                     "Failed to create swapchain.");

        vkGetSwapchainImagesKHR(instance->device, swapchain, &imageCount, nullptr);
        swapchainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(instance->device, swapchain, &imageCount, swapchainImages.data());

        swapchainImageViews.resize(swapchainImages.size());

        for (u32 i = 0; i < swapchainImages.size(); i++) {
            VkImageViewCreateInfo imageViewCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .image = swapchainImages[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = swapchainImageFormat,
                .components = { .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                                .a = VK_COMPONENT_SWIZZLE_IDENTITY },
                .subresourceRange = { .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                                      .baseMipLevel = 0,
                                      .levelCount = 1,
                                      .baseArrayLayer = 0,
                                      .layerCount = 1 },
            };

            POM_CHECK_VK(vkCreateImageView(instance->device, &imageViewCreateInfo, nullptr, &swapchainImageViews[i]),
                         "Failed to create swapchain image view");
        }

        depthImageFormat
            = findSupportedFormat({ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
                                  VK_IMAGE_TILING_OPTIMAL,
                                  VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
        VkImageCreateInfo imageInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = depthImageFormat,
            .extent = { swapchainExtent.width, swapchainExtent.height, 1 },
            .mipLevels = 1,
            .arrayLayers = 1,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        POM_CHECK_VK(vkCreateImage(instance->device, &imageInfo, nullptr, &depthImage),
                     "Failed to create depth image.");

        VkMemoryRequirements memoryReqs;
        vkGetImageMemoryRequirements(instance->device, depthImage, &memoryReqs);
        VkPhysicalDeviceMemoryProperties physicalMemoryProps;
        vkGetPhysicalDeviceMemoryProperties(instance->getVkPhysicalDevice(), &physicalMemoryProps);

        u32 memoryTypeIndex = VK_MAX_MEMORY_TYPES;

        VkMemoryPropertyFlags props = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

        for (u32 i = 0; i < physicalMemoryProps.memoryTypeCount; i++) {
            if (memoryReqs.memoryTypeBits & (1 << i)
                && (physicalMemoryProps.memoryTypes[i].propertyFlags & props) == props) {
                memoryTypeIndex = i;
                break;
            }
        }

        POM_ASSERT(memoryTypeIndex != VK_MAX_MEMORY_TYPES, "failed to find suitable memory type.");

        VkMemoryAllocateInfo allocInfo {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memoryReqs.size,
            .memoryTypeIndex = memoryTypeIndex,
        };

        POM_CHECK_VK(vkAllocateMemory(instance->device, &allocInfo, nullptr, &depthImageMemory),
                     "unable to allocate depth image memory.");

        vkBindImageMemory(instance->device, depthImage, depthImageMemory, 0);

        VkImageViewCreateInfo imageViewCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = depthImage,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = depthImageFormat,
            .components = { .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                            .a = VK_COMPONENT_SWIZZLE_IDENTITY },
            .subresourceRange = { .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                                  .baseMipLevel = 0,
                                  .levelCount = 1,
                                  .baseArrayLayer = 0,
                                  .layerCount = 1 },
        };

        POM_CHECK_VK(vkCreateImageView(instance->device, &imageViewCreateInfo, nullptr, &depthImageView),
                     "Failed to create depth image view")

        if (firstTime) {
            swapchainRenderPass = RenderPass::create({ {
                                                         .format = fromVkFormat(swapchainImageFormat),
                                                         .loadOperation = LoadOperation::CLEAR,
                                                         .storeOperation = StoreOperation::STORE,
                                                         .colorClear = Color::BLACK,
                                                     } },
                                                     {
                                                         .format = fromVkFormat(depthImageFormat),
                                                         .loadOperation = LoadOperation::CLEAR,
                                                         .storeOperation = StoreOperation::DONTCARE,
                                                         .depthStencilClear = { 1.f, 0 },
                                                     })
                                      .dynCast<RenderPassVk>();
        }

        // framebuffers
        swapchainFramebuffers.resize(swapchainImageViews.size());

        for (u32 i = 0; i < swapchainFramebuffers.size(); i++) {
            VkImageView attachments[2] = { depthImageView, swapchainImageViews[i] };
            VkFramebufferCreateInfo framebufferCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .renderPass = swapchainRenderPass->getHandle(),
                .attachmentCount = 2,
                .pAttachments = attachments,
                .width = swapchainExtent.width,
                .height = swapchainExtent.height,
                .layers = 1,
            };

            POM_CHECK_VK(
                vkCreateFramebuffer(instance->device, &framebufferCreateInfo, nullptr, &swapchainFramebuffers[i]),
                "failed to create framebuffer");
        }
    }

    void ContextVk::recreateSwapchain(const maths::vec2& extent)
    {
        POM_PROFILE_FUNCTION();
        VkExtent2D ext = { .width = static_cast<u32>(extent.x), .height = static_cast<u32>(extent.y) };

        // TODO: once deletion queues are implemented, this will not be needed.
        vkDeviceWaitIdle(instance->device);

        vkDestroyImageView(instance->device, depthImageView, nullptr);
        vkDestroyImage(instance->device, depthImage, nullptr);
        vkFreeMemory(instance->device, depthImageMemory, nullptr);

        for (auto* framebuffer : swapchainFramebuffers) {
            vkDestroyFramebuffer(instance->device, framebuffer, nullptr);
        }

        for (auto* imageView : swapchainImageViews) {
            vkDestroyImageView(instance->device, imageView, nullptr);
        }

        createSwapchain(ext, false);

        // recreate current image available semaphores to reset it to the unsignaled state, the associated swapchain
        // image may have been previously acquired and the semaphore set to signaled.
        // TODO: reconsider how this works, instead do acquire->render->present instead of render->present->acquire?
        VkSemaphoreCreateInfo semaphoreCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
        };

        vkDestroySemaphore(instance->device, imageAvailableSemaphores[frameIndex], nullptr);
        POM_CHECK_VK(
            vkCreateSemaphore(instance->device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[frameIndex]),
            "Failed to create image available semaphore");

        acquireNextSwapchainImage();
    }

    void ContextVk::present()
    {
        POM_PROFILE_FUNCTION();
        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[frameIndex] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[frameIndex] };

        VkSubmitInfo submitInfo = {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = waitSemaphores,
            .pWaitDstStageMask = waitStages,
            .commandBufferCount = 0,
            .pCommandBuffers = nullptr,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = signalSemaphores,
        };

        POM_CHECK_VK(vkQueueSubmit(instance->graphicsQueue, 1, &submitInfo, inFlightFences[frameIndex]),
                     "Failed to submit to queue");

        VkPresentInfoKHR presentInfo = {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = signalSemaphores,
            .swapchainCount = 1,
            .pSwapchains = &swapchain,
            .pImageIndices = &swapchainImageIndex,
            .pResults = nullptr,
        };

        VkResult result = vkQueuePresentKHR(instance->presentQueue, &presentInfo);

        // if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        //     recreateSwapchain(gamestate, pom::Application::get()->getMainWindow().getVulkanDrawableExtent());
        // } else
        if (result != VK_SUCCESS) {
            POM_FATAL("Failed to get next swapchain image");
        }

        frameIndex = (frameIndex + 1) % POM_MAX_FRAMES_IN_FLIGHT;

        acquireNextSwapchainImage();

        vkWaitForFences(instance->device, 1, &inFlightFences[frameIndex], VK_TRUE, UINT64_MAX);

        if (imagesInFlight[swapchainImageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(instance->device, 1, &imagesInFlight[swapchainImageIndex], VK_TRUE, UINT64_MAX);
        }
        imagesInFlight[swapchainImageIndex] = inFlightFences[frameIndex];

        vkResetFences(instance->device, 1, &inFlightFences[frameIndex]);
    }

    void ContextVk::acquireNextSwapchainImage()
    {
        POM_PROFILE_FUNCTION();
        VkResult result = vkAcquireNextImageKHR(instance->device,
                                                swapchain,
                                                UINT64_MAX,
                                                imageAvailableSemaphores[frameIndex],
                                                VK_NULL_HANDLE,
                                                &swapchainImageIndex);

        // if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        //     POM_DEBUG("next image: ", swapchainImageIndex);
        //     redraw
        // } else
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            POM_FATAL("Failed to get next swapchain image");
        }
    }

} // namespace pom::gfx