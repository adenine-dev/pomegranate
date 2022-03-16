#pragma once

#include "../instance.hpp"

#include "contextVk.hpp"
#include "pipelineLayoutVk.hpp"

namespace pom::gfx {
    /// @addtogroup vulkan
    /// @{

    /// Vulkan Instance, should be created implicitly through `Instance::create`. Contains the instance, physical and
    /// logical devices, and queues.
    class POM_API InstanceVk final : public Instance {
    private:
        // Theoretically this is completely wrong, queue family indices can be any valid u32 including u32max.
        // Practically no device will have more than a few queue families, much less 2^32-1 of them.
        static const u32 INVALID_QUEUE_FAMILY_INDEX = UINT32_MAX;

    public:
        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        void determineGPU(Context* ctx) final;

        void update() final;

        [[nodiscard]] bool ready() const final;

        [[nodiscard]] inline VkDevice getVkDevice()
        {
            return device;
        }

        [[nodiscard]] inline VkPhysicalDevice getVkPhysicalDevice()
        {
            return physicalDevice;
        }

        [[nodiscard]] inline u32 getCurrentFrameIndex() const
        {
            return currentFrame;
        }

    private:
        friend class Instance;
        friend class ContextVk;
        friend class CommandBufferVk;
        friend class Pipeline;

        POM_NOCOPY(InstanceVk);
        InstanceVk(const char* appname);
        ~InstanceVk() final;

        void loadPhysicalDevices();

        u32 rateGPU(const GPU& gpu, VkSurfaceKHR surface);

        std::vector<const char*> validationLayers;

        u32 currentFrame = 0;

        VkInstance instance = VK_NULL_HANDLE;
#ifdef POM_DEBUG_SYMBOLS
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
#endif
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        // TODO: compute stuffs
        u32 graphicsQueueFamilyIndex = INVALID_QUEUE_FAMILY_INDEX;
        u32 presentQueueFamilyIndex = INVALID_QUEUE_FAMILY_INDEX;
        u32 transferQueueFamilyIndex = INVALID_QUEUE_FAMILY_INDEX;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;
        VkQueue transferQueue = VK_NULL_HANDLE;

        // FIXME: this is extremely not threadsafe, there should be at least 1 pool per thread.
        VkCommandPool graphicsCommandPool = VK_NULL_HANDLE;
        VkCommandPool transferCommandPool = VK_NULL_HANDLE;

        Ref<PipelineLayoutVk> emptyPipelineLayout;
    };

    ///@}

} // namespace pom::gfx