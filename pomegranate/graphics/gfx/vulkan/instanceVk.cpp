#include "pch.hpp"

#include "instanceVk.hpp"

#include "gfxVk.hpp"

#include <set>

#ifdef POM_DEBUG_SYMBOLS
VkBool32 debugCallbackVk(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                         VkDebugUtilsMessageTypeFlagsEXT /*types*/,
                         const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                         void* /*userdata*/)
{
    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        ::pom::_log(::std::cout,
                    ::pom::LogLevel::TRACE,
                    ::pom::terminal::grey,
                    "[TRACE] ",
                    ::pom::terminal::reset,
                    "vulkan : ",
                    pCallbackData->pMessage);
    } else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        ::pom::_log(::std::cout,
                    ::pom::LogLevel::INFO,
                    ::pom::terminal::blue,
                    "[INFO] ",
                    ::pom::terminal::reset,
                    "vulkan : ",
                    pCallbackData->pMessage);
    } else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        ::pom::_log(::std::cout,
                    ::pom::LogLevel::WARN,
                    ::pom::terminal::yellow,
                    "[WARN] ",
                    ::pom::terminal::reset,
                    "vulkan : ",
                    pCallbackData->pMessage);
    } else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        ::pom::_log(::std::cout,
                    ::pom::LogLevel::INFO,
                    ::pom::terminal::red,
                    "[ERROR] ",
                    ::pom::terminal::reset,
                    "vulkan : ",
                    pCallbackData->pMessage);
    }

    return VK_FALSE;
}
#endif

namespace pom::gfx {
    InstanceVk::InstanceVk(const char* appname)
    {
        POM_PROFILE_FUNCTION();
        // load desired api version
        u32 instanceApiVersion = 0;
        auto vkEnumerateInstanceVersion
            = PFN_vkEnumerateInstanceVersion(vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));
        if (vkEnumerateInstanceVersion) {
            vkEnumerateInstanceVersion(&instanceApiVersion);
        } else {
            instanceApiVersion = VK_API_VERSION_1_0;
        }

        // load validation layers
#ifdef POM_DEBUG_SYMBOLS
        validationLayers.push_back("VK_LAYER_KHRONOS_validation");
        u32 layerCount = 0;
        POM_CHECK_VK(vkEnumerateInstanceLayerProperties(&layerCount, nullptr), "Failed to get validation layers");

        std::vector<VkLayerProperties> availableLayers(layerCount);
        POM_CHECK_VK(vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data()),
                     "Failed to get validation layers");

        for (const auto* layer : validationLayers) {
            bool found = false;
            for (u32 i = 0; i < layerCount; i++) {
                if (strcmp(layer, availableLayers[i].layerName) == 0) {
                    found = true;
                    break;
                }
            }
            POM_ASSERT(found, "Unable to find requested validation layer: ", layer);
        }
#endif

        // load extensions
        u32 extensionCount = 0;
        POM_CHECK_VK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr),
                     "Failed to get vulkan extensions.");

        std::vector<VkExtensionProperties> extensionProps(extensionCount);
        std::vector<const char*> extensions;

        POM_CHECK_VK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProps.data()),
                     "Failed to get vulkan extensions.");

        // currently we aren't doing headless things so this is always required, do this manually instead of through SDL
        // so we don't mandate window creation first because that's annoying lmao.
        extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#if defined(_WIN32)
        extensions.push_back("VK_KHR_win32_surface");
// currently only support win32, but here for future reference.
#elif defined(__ANDROID__)
        extensions.push_back("VK_KHR_android_surface");
#elif defined(_DIRECT2DISPLAY)
        extensions.push_back("VK_KHR_display");
#elif defined(__linux__)
        // wayland, x11?
        extensions.push_back("VK_KHR_xcb_surface");
#elif defined(__APPLE__)
        extensions.push_back("VK_EXT_metal_surface");
#endif

#ifdef POM_DEBUG_SYMBOLS
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        for (const auto* extension : extensions) {
            bool found = false;
            for (u32 i = 0; i < extensionCount; i++) {
                if (strcmp(extension, extensionProps[i].extensionName) == 0) {
                    found = true;
                    break;
                }
            }
            POM_ASSERT(found, "Unable to find requested extension: ", extension);
        }

#ifdef POM_DEBUG_SYMBOLS
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                               | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                               | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debugCallbackVk,
            .pUserData = nullptr,
        };
#endif

        VkApplicationInfo appInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = appname,
            .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
            .pEngineName = "pomegranate",
            .engineVersion = VK_MAKE_VERSION(0, 1, 0),
            .apiVersion = instanceApiVersion,
        };

        VkInstanceCreateInfo instanceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
#ifdef POM_DEBUG_SYMBOLS
            .pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo,
#else
            .pNext = nullptr,
#endif
            .flags = 0,
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = static_cast<u32>(validationLayers.size()),
            .ppEnabledLayerNames = validationLayers.data(),
            .enabledExtensionCount = static_cast<u32>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data(),
        };

        POM_CHECK_VK(vkCreateInstance(&instanceCreateInfo, nullptr, &instance), "Failed to create instance.");

// debug messanger
#ifdef POM_DEBUG_SYMBOLS
        auto vkCreateDebugUtilsMessengerEXT
            = PFN_vkCreateDebugUtilsMessengerEXT(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        POM_ASSERT(vkCreateDebugUtilsMessengerEXT, "Unable to find vkCreateDebugUtilsMessengerEXT");
        VkDebugUtilsMessengerCreateInfoEXT messangerCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity
            = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
              | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debugCallbackVk,
            .pUserData = nullptr,
        };

        vkCreateDebugUtilsMessengerEXT(instance, &messangerCreateInfo, nullptr, &debugMessenger);
#endif
        loadPhysicalDevices();
    }

    void InstanceVk::loadPhysicalDevices()
    {
        POM_PROFILE_FUNCTION();
        u32 deviceCount = 0;
        POM_CHECK_VK(vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr), "Failed to get device count.");
        POM_ASSERT(deviceCount, "No GPU with vulkan support available");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        POM_CHECK_VK(vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data()), "Failed to get devices.");

        for (auto* device : devices) {
            VkPhysicalDeviceProperties props;
            vkGetPhysicalDeviceProperties(device, &props);

            // TODO: other properties.
            GPUs.push_back({
                .name = props.deviceName,
                .discrete = props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU,
                .handle = device,
            });
        }
    }

    InstanceVk::~InstanceVk()
    {
        POM_PROFILE_FUNCTION();
        emptyPipelineLayout.free();

        if (ready()) {
            vkDestroyCommandPool(device, graphicsCommandPool, nullptr);
            vkDestroyCommandPool(device, transferCommandPool, nullptr);

            vkDestroyDevice(device, nullptr);
        }
#ifdef POM_DEBUG_SYMBOLS
        auto vkDestroyDebugUtilsMessengerEXT
            = PFN_vkDestroyDebugUtilsMessengerEXT(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        POM_ASSERT(vkDestroyDebugUtilsMessengerEXT, "Unable to find vkDestroyDebugUtilsMessengerEXT");
        vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
#endif
        vkDestroyInstance(instance, nullptr);
    }

    const char* REQUIRED_DEVICE_EXTENSIONS[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    const usize REQUIRED_DEVICE_EXTENSIONS_COUNT = 1;

    void InstanceVk::determineGPU(Context* ctx)
    {
        POM_PROFILE_FUNCTION();
        POM_ASSERT(ctx->getAPI() == getAPI(), "Attempting to use graphics context with mismatched api.");

        auto* context = dynamic_cast<ContextVk*>(ctx);

        // select best gpu.
        u32 maxScore = 0;
        u32 i = 0;
        for (auto& gpu : GPUs) {
            u32 score = 1;

            auto* device = static_cast<VkPhysicalDevice>(gpu.handle);

            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

            bool hasGraphicsQueueFamily = false;
            bool dedicatedTransferQueueFamily = false;
            VkBool32 hasPresentQueueFamily = false;
            u32 graphicsQueueFamily = INVALID_QUEUE_FAMILY_INDEX;
            u32 presentQueueFamily = INVALID_QUEUE_FAMILY_INDEX;
            u32 transferQueueFamily = INVALID_QUEUE_FAMILY_INDEX;
            for (u32 i = 0; i < queueFamilies.size(); i++) {
                // require graphics capable queue family and default the transfer queue to be the same (all graphics
                // queue families implicitly support transfer operations)
                if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    hasGraphicsQueueFamily = true;
                    graphicsQueueFamily = i;
                    transferQueueFamily = graphicsQueueFamily;
                }

                // require present support
                if (!hasPresentQueueFamily) {
                    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, context->surface, &hasPresentQueueFamily);
                    presentQueueFamily = i;
                }

                if (!dedicatedTransferQueueFamily && queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT
                    && !(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT
                         || queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)) {
                    transferQueueFamily = i;
                    dedicatedTransferQueueFamily = true;
                }
            }

            // prefer to share graphics and present in one queue.
            if (graphicsQueueFamily != presentQueueFamily) {
                vkGetPhysicalDeviceSurfaceSupportKHR(device,
                                                     graphicsQueueFamily,
                                                     context->surface,
                                                     &hasPresentQueueFamily);
                if (hasPresentQueueFamily) {
                    presentQueueFamily = graphicsQueueFamily;
                    score += 10;
                }
            } else {
                score += 10; // NOTE: these are arbitrary values currently, prob shouldn't be.
            }

            // prefer dedicated transfer queue.
            if (dedicatedTransferQueueFamily) {
                score += 50;
            }

            // Require all queue families to be found.
            if (!hasGraphicsQueueFamily || !hasPresentQueueFamily) {
                continue;
            }

            // require required extensions (wording lol)
            // currently as swapchain support is the only required extension, this does nothing because a present queue
            // family implies the existence of swapchain extension support, here for future expansion
            u32 extensionCount = 0;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

            for (const auto* extension : REQUIRED_DEVICE_EXTENSIONS) {
                bool found = false;
                for (u32 i = 0; i < extensionCount; i++) {
                    if (strcmp(extension, availableExtensions[i].extensionName) == 0) {
                        found = true;
                        break;
                    }
                }

                // if a given extension is not found the device is unusable.
                if (!found) {
                    continue;
                }
            }

            // require swapchain capabilities
            SwapchainSupportDetailsVk swapchainSupport = context->getSwapchainSupportDetails(device);

            if (swapchainSupport.formats.empty() || swapchainSupport.presentModes.empty()) {
                continue; // need at least one format and present mode
            }

            // prefer discrete gpu
            if (gpu.discrete) {
                score += 100;
            }

            if (score > maxScore) {
                maxScore = score;
                physicalDevice = device;
                graphicsQueueFamilyIndex = graphicsQueueFamily;
                presentQueueFamilyIndex = presentQueueFamily;
                transferQueueFamilyIndex = transferQueueFamily;
                selectedGPUIndex = i;
            }

            i++;
        }

        // create logical device and queues
        const f32 queuePriorities[] = { 1.f };

        std::set<u32> uniqueQueueFamilies
            = { graphicsQueueFamilyIndex, presentQueueFamilyIndex, transferQueueFamilyIndex };
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        queueCreateInfos.reserve(uniqueQueueFamilies.size());

        for (auto queueFamily : uniqueQueueFamilies) {
            queueCreateInfos.push_back({
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = queueFamily,
                .queueCount = 1,
                .pQueuePriorities = queuePriorities,
            });
        }

        VkPhysicalDeviceFeatures deviceFeatures {};

        VkDeviceCreateInfo deviceCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size()),
            .pQueueCreateInfos = queueCreateInfos.data(),
            .enabledLayerCount = static_cast<u32>(validationLayers.size()),
            .ppEnabledLayerNames = validationLayers.data(),
            .enabledExtensionCount = REQUIRED_DEVICE_EXTENSIONS_COUNT,
            .ppEnabledExtensionNames = REQUIRED_DEVICE_EXTENSIONS,
            .pEnabledFeatures = &deviceFeatures,
        };

        POM_ASSERT(vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) == VK_SUCCESS,
                   "Failed to create logical device.");

        vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
        vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, &presentQueue);
        vkGetDeviceQueue(device, transferQueueFamilyIndex, 0, &transferQueue);

        VkCommandPoolCreateInfo commandPoolCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = graphicsQueueFamilyIndex,
        };

        POM_CHECK_VK(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &graphicsCommandPool),
                     "Failed to create graphics command pool.");

        commandPoolCreateInfo.queueFamilyIndex = transferQueueFamilyIndex;
        POM_CHECK_VK(vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &transferCommandPool),
                     "Failed to create transfer command pool.");

        emptyPipelineLayout = Ref<PipelineLayoutVk>(new PipelineLayoutVk(this, {}));
    }

    void InstanceVk::update()
    {
        currentFrame = currentFrame + 1 % POM_MAX_FRAMES_IN_FLIGHT;
    }

    bool InstanceVk::ready() const
    {
        return physicalDevice != VK_NULL_HANDLE;
    }
} // namespace pom::gfx