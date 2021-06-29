#include <pomegranate/pomegranate.hpp>

#include <bitset>
#include <set>

// Theoretically this is completely wrong, queue family indices can be any valid u32 including u32max. Practically no
// device will have more than a few queue families, much less 2^32-1 of them.
const u32 INVALID_QUEUE_FAMILY_INDEX = UINT32_MAX;

const char* REQUIRED_DEVICE_EXTENSIONS[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
const size_t REQUIRED_DEVICE_EXTENSIONS_COUNT = 1;

struct GameState {
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSwapchainKHR swapchain;
    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
};

struct SwapChainSupport {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

POM_CLIENT_EXPORT const pom::AppCreateInfo* clientGetAppCreateInfo(int /*argc*/, char** /*argv*/)
{
    static const pom::AppCreateInfo aci = { .name = "Pomegranate Sandbox Application" };

    return &aci;
}

POM_CLIENT_EXPORT GameState* clientCreateState()
{
    auto* gc = new GameState;

    return gc;
}

VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
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

SwapChainSupport querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapChainSupport support;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &support.capabilities);

    u32 formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        support.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, support.formats.data());
    }

    u32 presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        support.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, support.presentModes.data());
    }

    return support;
}

u32 rateGPU(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    // definitely worth reconsidering how this works.

    u32 score = 1;

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    bool hasGraphicsQueueFamily = false;
    VkBool32 hasPresentQueueFamily = false;
    for (u32 i = 0; i < queueFamilies.size(); i++) {
        // require graphics capable queue family
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            hasGraphicsQueueFamily = true;
        }

        // require present support
        if (!hasPresentQueueFamily) {
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &hasPresentQueueFamily);
        }

        if (hasGraphicsQueueFamily && hasPresentQueueFamily) {
            break;
        }
    }

    if (!hasGraphicsQueueFamily || !hasPresentQueueFamily) {
        return 0;
    }

    // require required extensions (wording lol)
    // currently as swapchain support is the only required extension, this does nothing because a present queue family
    // implies the existence of swapchain extension support, here for future expansion
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
            return 0;
        }
    }

    // require swapchain capabilities
    SwapChainSupport swapchainSupport = querySwapChainSupport(device, surface);
    if (swapchainSupport.formats.empty() || swapchainSupport.presentModes.empty()) {
        return 0; // need some formats and present modes
    }

    // prefer discrete gpu
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 100;
    }

    return score;
}

POM_CLIENT_EXPORT void clientBegin(GameState* gamestate)
{
    // instance
    std::vector<const char*> validationLayers;
#ifdef _DEBUG
    validationLayers.push_back("VK_LAYER_KHRONOS_validation");
    u32 layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const auto* layer : validationLayers) {
        bool found = false;
        for (u32 i = 0; i < layerCount; i++) {
            if (strcmp(layer, availableLayers[i].layerName) == 0) {
                found = true;
                break;
            }
        }
        POM_ASSERT(found, "was unable to find requested layer: ", layer);
    }
#endif

    pom::Window& window = pom::Application::get()->getMainWindow();

    auto extensions = window.getRequiredVulkanExtensions();

#ifdef _DEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                           | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debugCallback,
        .pUserData = nullptr,
    };

    VkApplicationInfo appInfo = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "test",
        .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
        .pEngineName = "pomegranate",
        .engineVersion = VK_MAKE_VERSION(0, 1, 0),
        .apiVersion = VK_API_VERSION_1_2,
    };

    VkInstanceCreateInfo instanceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo,
        .flags = 0,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<u32>(validationLayers.size()),
        .ppEnabledLayerNames = validationLayers.data(),
        .enabledExtensionCount = static_cast<u32>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    POM_ASSERT(vkCreateInstance(&instanceCreateInfo, nullptr, &gamestate->instance) == VK_SUCCESS,
               "Failed to create instance.");

    // debug messanger
    auto vkCreateDebugUtilsMessengerEXT = PFN_vkCreateDebugUtilsMessengerEXT(
        vkGetInstanceProcAddr(gamestate->instance, "vkCreateDebugUtilsMessengerEXT"));
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
        .pfnUserCallback = debugCallback,
        .pUserData = nullptr,
    };

    vkCreateDebugUtilsMessengerEXT(gamestate->instance, &messangerCreateInfo, nullptr, &gamestate->debugMessenger);

    // window surface
    gamestate->surface = window.getVulkanSurface(gamestate->instance);

    // physical device
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(gamestate->instance, &deviceCount, nullptr);
    POM_ASSERT(deviceCount, "No GPU with vulkan support available");
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(gamestate->instance, &deviceCount, devices.data());

    u32 highestScore = 0;
    for (auto* device : devices) {
        u32 rating = rateGPU(device, gamestate->surface);
        if (highestScore < rating) {
            highestScore = rating;
            gamestate->physicalDevice = device;
        }
    }

    // queue families
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gamestate->physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(gamestate->physicalDevice, &queueFamilyCount, queueFamilies.data());

    u32 graphicsQueueFamily = INVALID_QUEUE_FAMILY_INDEX;
    u32 presentQueueFamily = INVALID_QUEUE_FAMILY_INDEX;
    for (u32 i = 0; i < queueFamilies.size(); i++) {
        if (graphicsQueueFamily == INVALID_QUEUE_FAMILY_INDEX && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueFamily = i;
        }

        VkBool32 hasPresentQueueFamily = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(gamestate->physicalDevice, i, gamestate->surface, &hasPresentQueueFamily);
        if (presentQueueFamily == INVALID_QUEUE_FAMILY_INDEX && hasPresentQueueFamily) {
            presentQueueFamily = i;
        }

        // TODO: make this prefer to be the same family as the graphics family
        if (graphicsQueueFamily != INVALID_QUEUE_FAMILY_INDEX && presentQueueFamily != INVALID_QUEUE_FAMILY_INDEX) {
            break;
        }
    }

    POM_ASSERT(graphicsQueueFamily != INVALID_QUEUE_FAMILY_INDEX, "graphics queue family not found");
    POM_ASSERT(presentQueueFamily != INVALID_QUEUE_FAMILY_INDEX, "present queue family not found");

    // logical device & queues
    const f32 queuePriorities[] = { 1.f };

    std::set<u32> uniqueQueueFamilies = { graphicsQueueFamily, presentQueueFamily };
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

    POM_ASSERT(vkCreateDevice(gamestate->physicalDevice, &deviceCreateInfo, nullptr, &gamestate->device) == VK_SUCCESS,
               "Failed to create logical device.");

    vkGetDeviceQueue(gamestate->device, graphicsQueueFamily, 0, &gamestate->graphicsQueue);
    vkGetDeviceQueue(gamestate->device, presentQueueFamily, 0, &gamestate->presentQueue);

    // swapchain
    SwapChainSupport swapchainSupport = querySwapChainSupport(gamestate->physicalDevice, gamestate->surface);

    VkSurfaceFormatKHR surfaceFormat = swapchainSupport.formats[0];
    for (const auto& format : swapchainSupport.formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surfaceFormat = format;
            break;
        }
    }

    gamestate->swapchainImageFormat = surfaceFormat.format;

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    // TODO: only do this if vsync is desired
    for (const auto& mode : swapchainSupport.presentModes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
    }

    gamestate->swapchainExtent = swapchainSupport.capabilities.currentExtent;
    if (gamestate->swapchainExtent.width == UINT32_MAX) {
        pom::maths::ivec2 extent = window.getVulkanDrawableExtent();
        gamestate->swapchainExtent.width = static_cast<u32>(extent.x);
        gamestate->swapchainExtent.height = static_cast<u32>(extent.y);

        gamestate->swapchainExtent.width = std::clamp(gamestate->swapchainExtent.width,
                                                      swapchainSupport.capabilities.minImageExtent.width,
                                                      swapchainSupport.capabilities.maxImageExtent.width);
        gamestate->swapchainExtent.height = std::clamp(gamestate->swapchainExtent.height,
                                                       swapchainSupport.capabilities.minImageExtent.height,
                                                       swapchainSupport.capabilities.maxImageExtent.height);
    }

    u32 imageCount
        = swapchainSupport.capabilities.maxImageCount == 0
              ? swapchainSupport.capabilities.minImageCount + 1
              : std::min(swapchainSupport.capabilities.minImageCount + 1, swapchainSupport.capabilities.maxImageCount);

    u32 queueFamilyIndicies[] = { graphicsQueueFamily, presentQueueFamily };
    bool sharedQueueFamily = graphicsQueueFamily == presentQueueFamily;
    VkSwapchainCreateInfoKHR swapchainCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = gamestate->surface,
        .minImageCount = imageCount,
        .imageFormat = surfaceFormat.format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = gamestate->swapchainExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = sharedQueueFamily ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = sharedQueueFamily ? 0u : 2u,
        .pQueueFamilyIndices = sharedQueueFamily ? nullptr : queueFamilyIndicies,
        .preTransform = swapchainSupport.capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    POM_ASSERT(vkCreateSwapchainKHR(gamestate->device, &swapchainCreateInfo, nullptr, &gamestate->swapchain)
                   == VK_SUCCESS,
               "Failed to create swapchain.");

    vkGetSwapchainImagesKHR(gamestate->device, gamestate->swapchain, &imageCount, nullptr);
    gamestate->swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(gamestate->device, gamestate->swapchain, &imageCount, gamestate->swapchainImages.data());

    gamestate->swapchainImageViews.resize(gamestate->swapchainImages.size());

    for (u32 i = 0; i < gamestate->swapchainImages.size(); i++) {
        VkImageViewCreateInfo imageViewCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = gamestate->swapchainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = gamestate->swapchainImageFormat,
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

        POM_ASSERT(
            vkCreateImageView(gamestate->device, &imageViewCreateInfo, nullptr, &gamestate->swapchainImageViews[i])
                == VK_SUCCESS,
            "Failed to create image view")
    }

    // pipeline
}

POM_CLIENT_EXPORT void clientMount(GameState* gamestate)
{
}

POM_CLIENT_EXPORT void clientUpdate(GameState* gamestate, pom::DeltaTime dt)
{
}

POM_CLIENT_EXPORT void clientOnInputEvent(GameState* gamestate, pom::InputEvent* ev)
{
}

POM_CLIENT_EXPORT void clientUnmount(GameState* gamestate)
{
}

POM_CLIENT_EXPORT void clientEnd(GameState* gamestate)
{
    for (auto* imageView : gamestate->swapchainImageViews) {
        vkDestroyImageView(gamestate->device, imageView, nullptr);
    }

    vkDestroySwapchainKHR(gamestate->device, gamestate->swapchain, nullptr);

    vkDestroySurfaceKHR(gamestate->instance, gamestate->surface, nullptr);

    vkDestroyDevice(gamestate->device, nullptr);

    auto vkDestroyDebugUtilsMessengerEXT = PFN_vkDestroyDebugUtilsMessengerEXT(
        vkGetInstanceProcAddr(gamestate->instance, "vkDestroyDebugUtilsMessengerEXT"));
    POM_ASSERT(vkDestroyDebugUtilsMessengerEXT, "Unable to find vkDestroyDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessengerEXT(gamestate->instance, gamestate->debugMessenger, nullptr);

    vkDestroyInstance(gamestate->instance, nullptr);

    delete gamestate;
}
