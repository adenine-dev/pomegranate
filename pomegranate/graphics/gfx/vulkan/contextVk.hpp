#pragma once

#include "base.hpp"

#include "../context.hpp"

#include "instanceVk.hpp"
#include "renderPassVk.hpp"
#include "typesVk.hpp"

#include "platform/window.hpp"

namespace pom::gfx {
    /// @addtogroup vulkan
    /// @{

    /// @private
    struct SwapchainSupportDetailsVk {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    /// Vulkan Context, should be created implicitly through `Context::create`. Contains swapchain information, a
    /// swapchain render pass, and presentation syncronization objects.
    class POM_API ContextVk final : public Context {
    public:
        [[nodiscard]] inline GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        [[nodiscard]] Format getSwapchainFormat() const final
        {
            return fromVkFormat(swapchainImageFormat);
        }

        [[nodiscard]] const RenderPass* getSwapchainRenderPass() const final
        {
            return swapchainRenderPass;
        }

        ~ContextVk() final;

        void recreateSwapchain(const maths::vec2& extent) final;

        void present() final;

    public:
        friend class Context;
        friend class InstanceVk;

        SwapchainSupportDetailsVk getSwapchainSupportDetails(VkPhysicalDevice device) const;

        ContextVk(InstanceVk* instance, Window* window);

        void createSwapchain(const VkExtent2D& extent, bool firstTime);

        void acquireNextSwapchainImage();

        POM_NOCOPY(ContextVk);

        InstanceVk* instance;
        VkSurfaceKHR surface;

        RenderPassVk* swapchainRenderPass;

        VkSwapchainKHR swapchain;
        VkFormat swapchainImageFormat;
        VkExtent2D swapchainExtent;
        std::vector<VkImage> swapchainImages;
        std::vector<VkImageView> swapchainImageViews;
        std::vector<VkFramebuffer> swapchainFramebuffers;
        VkViewport swapchainViewport;
        u32 swapchainImageIndex = 0;

        static const u32 MAX_FRAMES_IN_FLIGHT = 3;

        VkSemaphore imageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
        VkSemaphore renderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
        VkFence inFlightFences[MAX_FRAMES_IN_FLIGHT];
        std::vector<VkFence> imagesInFlight;
        u32 frameIndex = 0;
    };

    /// @}
} // namespace pom::gfx