#include "pch.hpp"

#include "contextVk.hpp"

#include "commandBufferVk.hpp"

namespace pom::gfx {
    ContextVk::ContextVk(InstanceVk* instance, Window* window) :
        Context(window), instance(instance), surface(window->getVulkanSurface(instance->instance))
    {
        if (!instance->ready()) {
            instance->determineGPU(this);
        } else {
            VkBool32 supported;
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

        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            POM_ASSERT(vkCreateSemaphore(instance->device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i])
                           == VK_SUCCESS,
                       "Failed to create image available semaphore");

            POM_ASSERT(vkCreateSemaphore(instance->device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i])
                           == VK_SUCCESS,
                       "Failed to create render finish semaphore");

            POM_ASSERT(vkCreateFence(instance->device, &fenceCreateInfo, nullptr, &inFlightFences[i]) == VK_SUCCESS,
                       "Failed to create fence");
        }

        vkResetFences(instance->device, 1, &inFlightFences[frameIndex]);

        acquireNextSwapchainImage();
    }

    ContextVk::~ContextVk()
    {
        vkDeviceWaitIdle(instance->device);

        for (u32 i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(instance->device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(instance->device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(instance->device, inFlightFences[i], nullptr);
        }

        delete swapchainRenderPass;

        for (auto* imageView : swapchainImageViews) {
            vkDestroyImageView(instance->device, imageView, nullptr);
        }

        for (auto* framebuffer : swapchainFramebuffers) {
            vkDestroyFramebuffer(instance->device, framebuffer, nullptr);
        }

        vkDestroySwapchainKHR(instance->device, swapchain, nullptr);

        vkDestroySurfaceKHR(instance->instance, surface, nullptr);
    }

    SwapchainSupportDetailsVk ContextVk::getSwapchainSupportDetails(VkPhysicalDevice device) const
    {
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

        POM_ASSERT(vkCreateSwapchainKHR(instance->device, &swapchainCreateInfo, nullptr, &swapchain) == VK_SUCCESS,
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

            POM_ASSERT(vkCreateImageView(instance->device, &imageViewCreateInfo, nullptr, &swapchainImageViews[i])
                           == VK_SUCCESS,
                       "Failed to create image view")
        }

        if (firstTime) {
            swapchainRenderPass = dynamic_cast<RenderPassVk*>(RenderPass::create({ {
                .format = fromVkFormat(swapchainImageFormat),
                .loadOperation = LoadOperation::CLEAR,
                .storeOperation = StoreOperation::STORE,
                .clearColor = Color::BLACK,
            } }));
        }

        // framebuffers
        swapchainFramebuffers.resize(swapchainImageViews.size());

        for (u32 i = 0; i < swapchainFramebuffers.size(); i++) {
            VkFramebufferCreateInfo framebufferCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .renderPass = swapchainRenderPass->getHandle(),
                .attachmentCount = 1,
                .pAttachments = &swapchainImageViews[i],
                .width = swapchainExtent.width,
                .height = swapchainExtent.height,
                .layers = 1,
            };

            POM_ASSERT(vkCreateFramebuffer(instance->device, &framebufferCreateInfo, nullptr, &swapchainFramebuffers[i])
                           == VK_SUCCESS,
                       "failed to create framebuffer");
        }
    }

    void ContextVk::recreateSwapchain(const maths::vec2& extent)
    {
        VkExtent2D ext = { .width = static_cast<u32>(extent.x), .height = static_cast<u32>(extent.y) };

        // TODO: once deletion queues are implemented, this will not be needed.
        vkDeviceWaitIdle(instance->device);

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
        POM_ASSERT(
            vkCreateSemaphore(instance->device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[frameIndex])
                == VK_SUCCESS,
            "Failed to create image available semaphore");

        acquireNextSwapchainImage();
    }

    void ContextVk::present()
    {
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

        POM_ASSERT(vkQueueSubmit(instance->graphicsQueue, 1, &submitInfo, inFlightFences[frameIndex]) == VK_SUCCESS,
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

        frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;

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