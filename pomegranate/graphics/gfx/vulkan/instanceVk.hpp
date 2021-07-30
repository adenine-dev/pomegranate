#pragma once

#include "../instance.hpp"

#include "contextVk.hpp"

namespace pom::gfx {
    /// @addtogroup vulkan
    /// @{

    /// Vulkan Instance, should be created implicitly through `Instance::create`. Contains the instance, physical and
    /// logical devices, and queues.
    class POM_API InstanceVk final : public Instance {
    public:
        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        void determineGPU(Context* ctx) final;

        [[nodiscard]] bool ready() const final;

        [[nodiscard]] VkDevice getDevice() const
        {
            return device;
        }

    public:
        friend class Instance;
        friend class ContextVk;

        POM_NOCOPY(InstanceVk);
        InstanceVk(const char* appname);
        ~InstanceVk() final;

        void loadPhysicalDevices();

        u32 rateGPU(const GPU& gpu, VkSurfaceKHR surface);

        std::vector<const char*> validationLayers;

        VkInstance instance = VK_NULL_HANDLE;

#ifdef _DEBUG
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
#endif

        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        // TODO: compute stuffs
        u32 graphicsQueueFamilyIndex;
        u32 presentQueueFamilyIndex;
        u32 transferQueueFamilyIndex;
        VkDevice device;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkQueue transferQueue;

        // FIXME: this is extremely not threadsafe, there should be at least 1 pool per thread.
        VkCommandPool graphicsCommandPool;
        VkCommandPool transferCommandPool;
    };

    ///@}

} // namespace pom::gfx