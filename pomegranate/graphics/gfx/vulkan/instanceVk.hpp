#pragma once

#include "../instance.hpp"

#include "contextVk.hpp"

namespace pom::gfx {
    /// @addtogroup gfx
    /// @{

    class POM_API InstanceVk final : public Instance {
    public:
        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        void determineGPU(Context* ctx) final;

        [[nodiscard]] bool ready() const final;

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
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

        // TODO: compute/transfer stuffs
        u32 graphicsQueueFamilyIndex;
        u32 presentQueueFamilyIndex;
        VkDevice device;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
    };

    ///@}

} // namespace pom::gfx