#include <pomegranate/pomegranate.hpp>

#include "embed/basic_frag_spv.hpp"
#include "embed/basic_vert_spv.hpp"

#include <set>

// Theoretically this is completely wrong, queue family indices can be any valid u32 including u32max. Practically no
// device will have more than a few queue families, much less 2^32-1 of them.
const u32 INVALID_QUEUE_FAMILY_INDEX = UINT32_MAX;

const char* REQUIRED_DEVICE_EXTENSIONS[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
const size_t REQUIRED_DEVICE_EXTENSIONS_COUNT = 1;

const u32 MAX_FRAMES_IN_FLIGHT = 2;

struct Vertex {
    pom::maths::vec3 pos;
    pom::maths::vec4 color;
};

static const Vertex VERTEX_DATA[] = { { { 0.0f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
                                      { { 0.5f, 0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
                                      { { -0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } } };

struct GameState {
    bool paused = false;
    // instance
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice;
    // context
    VkSurfaceKHR surface;
    VkDevice device;
    u32 graphicsQueueFamily;
    u32 presentQueueFamily;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkCommandPool commandPool;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    std::vector<VkCommandBuffer> commandBuffers;
    // context -> swapchain
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    std::vector<VkFramebuffer> swapchainFramebuffers;
    VkRenderPass renderPass;
    VkViewport swapchainViewport;
    // pipeline
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    // vertex buffer
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
};

struct SwapChainSupport {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

POM_CLIENT_EXPORT const pom::AppCreateInfo* clientGetAppCreateInfo(int /*argc*/, char** /*argv*/)
{
    static const pom::AppCreateInfo aci = {
        .name = "Pomegranate Sandbox Application",
    };

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
        return 0; // need at least one format and present mode
    }

    // prefer discrete gpu
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 100;
    }

    return score;
}

u32 findMemoryType(VkPhysicalDevice physicalDevice, u32 filter, VkMemoryPropertyFlags props)
{
    VkPhysicalDeviceMemoryProperties physicalMemoryProps;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalMemoryProps);

    for (u32 i = 0; i < physicalMemoryProps.memoryTypeCount; i++) {
        if (filter & (1 << i) && (physicalMemoryProps.memoryTypes[i].propertyFlags & props) == props) {
            return i;
        }
    }

    POM_LOG_FATAL("failed to find suitable memory type.");
    return -1;
}

VkShaderModule createShaderModule(VkDevice device, size_t size, const unsigned char* bytes)
{
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = size,
        // FIXME: this won't be safe for general use
        .pCode = reinterpret_cast<const u32*>(bytes) // NOLINT this code will always be aligned to 4 bytes so it is safe
    };

    VkShaderModule shaderModule = VK_NULL_HANDLE;
    POM_ASSERT(vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule) == VK_SUCCESS,
               "Failed to create shader module");

    return shaderModule;
}

void createSwapchain(GameState* gamestate, const VkExtent2D& extent)
{
    SwapChainSupport swapchainSupport = querySwapChainSupport(gamestate->physicalDevice, gamestate->surface);

    VkSurfaceFormatKHR surfaceFormat = swapchainSupport.formats[0];
    for (const auto& format : swapchainSupport.formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surfaceFormat = format;
            break;
        }
    }

    gamestate->swapchainImageFormat = surfaceFormat.format;

    // TODO: only do this if vsync is desired
    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto& mode : swapchainSupport.presentModes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
    }
    // if vsync is not desired
    // for (const auto& mode : swapchainSupport.presentModes) {
    //     if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
    //         presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
    //         break;
    //     }
    // }

    // vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &support.capabilities);

    gamestate->swapchainExtent = swapchainSupport.capabilities.currentExtent;
    if (gamestate->swapchainExtent.width == UINT32_MAX) {
        gamestate->swapchainExtent = extent;

        gamestate->swapchainExtent.width = std::clamp(gamestate->swapchainExtent.width,
                                                      swapchainSupport.capabilities.minImageExtent.width,
                                                      swapchainSupport.capabilities.maxImageExtent.width);
        gamestate->swapchainExtent.height = std::clamp(gamestate->swapchainExtent.height,
                                                       swapchainSupport.capabilities.minImageExtent.height,
                                                       swapchainSupport.capabilities.maxImageExtent.height);
    }

    gamestate->swapchainViewport = {
        .x = 0.f,
        .y = 0.f,
        .width = (float)gamestate->swapchainExtent.width,
        .height = (float)gamestate->swapchainExtent.height,
        .minDepth = 0.f,
        .maxDepth = 1.f,
    };

    u32 imageCount
        = swapchainSupport.capabilities.maxImageCount == 0
              ? swapchainSupport.capabilities.minImageCount + 1
              : std::min(swapchainSupport.capabilities.minImageCount + 1, swapchainSupport.capabilities.maxImageCount);

    u32 queueFamilyIndicies[] = { gamestate->graphicsQueueFamily, gamestate->presentQueueFamily };
    bool sharedQueueFamily = gamestate->graphicsQueueFamily == gamestate->presentQueueFamily;
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
        .oldSwapchain = gamestate->swapchain, // default initialized to 0 which is also VK_NULL_HANDLE
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

    // swapchain framebuffers
    gamestate->swapchainFramebuffers.resize(gamestate->swapchainImageViews.size());

    for (u32 i = 0; i < gamestate->swapchainFramebuffers.size(); i++) {
        VkFramebufferCreateInfo framebufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .renderPass = gamestate->renderPass,
            .attachmentCount = 1,
            .pAttachments = &gamestate->swapchainImageViews[i],
            .width = gamestate->swapchainExtent.width,
            .height = gamestate->swapchainExtent.height,
            .layers = 1,
        };

        POM_ASSERT(vkCreateFramebuffer(gamestate->device,
                                       &framebufferCreateInfo,
                                       nullptr,
                                       &gamestate->swapchainFramebuffers[i])
                       == VK_SUCCESS,
                   "failed to create framebuffer");
    }
}

void recreateSwapchain(GameState* gamestate, const VkExtent2D& extent)
{
    vkDeviceWaitIdle(gamestate->device);

    for (auto* framebuffer : gamestate->swapchainFramebuffers) {
        vkDestroyFramebuffer(gamestate->device, framebuffer, nullptr);
    }

    for (auto* imageView : gamestate->swapchainImageViews) {
        vkDestroyImageView(gamestate->device, imageView, nullptr);
    }

    createSwapchain(gamestate, extent);
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
        .apiVersion = VK_API_VERSION_1_0,
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

    gamestate->graphicsQueueFamily = INVALID_QUEUE_FAMILY_INDEX;
    gamestate->presentQueueFamily = INVALID_QUEUE_FAMILY_INDEX;
    for (u32 i = 0; i < queueFamilies.size(); i++) {
        if (gamestate->graphicsQueueFamily == INVALID_QUEUE_FAMILY_INDEX
            && queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            gamestate->graphicsQueueFamily = i;
        }

        VkBool32 hasPresentQueueFamily = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(gamestate->physicalDevice, i, gamestate->surface, &hasPresentQueueFamily);
        if (gamestate->presentQueueFamily == INVALID_QUEUE_FAMILY_INDEX && hasPresentQueueFamily) {
            gamestate->presentQueueFamily = i;
        }

        // TODO: make this prefer to be the same family as the graphics family
        if (gamestate->graphicsQueueFamily != INVALID_QUEUE_FAMILY_INDEX
            && gamestate->presentQueueFamily != INVALID_QUEUE_FAMILY_INDEX) {
            break;
        }
    }

    POM_ASSERT(gamestate->graphicsQueueFamily != INVALID_QUEUE_FAMILY_INDEX, "graphics queue family not found");
    POM_ASSERT(gamestate->presentQueueFamily != INVALID_QUEUE_FAMILY_INDEX, "present queue family not found");

    // logical device & queues
    const f32 queuePriorities[] = { 1.f };

    std::set<u32> uniqueQueueFamilies = { gamestate->graphicsQueueFamily, gamestate->presentQueueFamily };
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

    vkGetDeviceQueue(gamestate->device, gamestate->graphicsQueueFamily, 0, &gamestate->graphicsQueue);
    vkGetDeviceQueue(gamestate->device, gamestate->presentQueueFamily, 0, &gamestate->presentQueue);

    // swapchain setup
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

    // renderpass
    VkAttachmentDescription colorAttachment = {
        .flags = 0,
        .format = gamestate->swapchainImageFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

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
        .attachmentCount = 1,
        .pAttachments = &colorAttachment,
        .subpassCount = 1,
        .pSubpasses = &subpassDesc,
        .dependencyCount = 1,
        .pDependencies = &subpassDependency,
    };

    POM_ASSERT(vkCreateRenderPass(gamestate->device, &renderPassCreateInfo, nullptr, &gamestate->renderPass)
                   == VK_SUCCESS,
               "failed to create renderpass");

    // swapchain
    createSwapchain(gamestate, window.getVulkanDrawableExtent());

    // pipeline
    // TODO: as much of this as possible should load from the shaders themselves.
    VkShaderModule vertShaderModule = createShaderModule(gamestate->device, basic_vert_spv_size, basic_vert_spv_data);
    VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr,
    };

    VkShaderModule fragShaderModule = createShaderModule(gamestate->device, basic_frag_spv_size, basic_frag_spv_data);
    VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr,
    };

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageCreateInfo, fragShaderStageCreateInfo };

    VkVertexInputBindingDescription vertexBindingDesc = {
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    VkVertexInputAttributeDescription vertexAttribDescs[] = { {
                                                                  // position
                                                                  .location = 0,
                                                                  .binding = 0,
                                                                  .format = VK_FORMAT_R32G32B32_SFLOAT,
                                                                  .offset = offsetof(Vertex, pos),
                                                              },
                                                              {
                                                                  // color
                                                                  .location = 1,
                                                                  .binding = 0,
                                                                  .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                                                                  .offset = offsetof(Vertex, color),
                                                              } };

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &vertexBindingDesc,
        .vertexAttributeDescriptionCount = 2,
        .pVertexAttributeDescriptions = vertexAttribDescs,
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    gamestate->swapchainViewport = {
        .x = 0.f,
        .y = 0.f,
        .width = (float)gamestate->swapchainExtent.width,
        .height = (float)gamestate->swapchainExtent.height,
        .minDepth = 0.f,
        .maxDepth = 1.f,
    };

    VkRect2D scissor { .offset = { 0, 0 }, .extent = gamestate->swapchainExtent };

    VkPipelineViewportStateCreateInfo viewportCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = nullptr,
        .scissorCount = 1,
        .pScissors = nullptr,
    };

    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.f,
        .depthBiasClamp = 0.f,
        .depthBiasSlopeFactor = 0.f,
        .lineWidth = 1.f,
    };

    VkPipelineMultisampleStateCreateInfo multisampleCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    // mix based on opacity
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask
        = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
        .blendConstants = { 0.f, 0.f, 0.f, 0.f },
    };

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    POM_ASSERT(vkCreatePipelineLayout(gamestate->device, &pipelineLayoutCreateInfo, nullptr, &gamestate->pipelineLayout)
                   == VK_SUCCESS,
               "failed to create pipeline");

    VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = 2,
        .pDynamicStates = dynamicStates,
    };

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputCreateInfo,
        .pInputAssemblyState = &inputAssemblyCreateInfo,
        .pTessellationState = nullptr,
        .pViewportState = &viewportCreateInfo,
        .pRasterizationState = &rasterizationCreateInfo,
        .pMultisampleState = &multisampleCreateInfo,
        .pDepthStencilState = nullptr,
        .pColorBlendState = &colorBlendCreateInfo,
        .pDynamicState = &dynamicStateCreateInfo,
        .layout = gamestate->pipelineLayout,
        .renderPass = gamestate->renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    POM_ASSERT(vkCreateGraphicsPipelines(gamestate->device,
                                         VK_NULL_HANDLE,
                                         1,
                                         &pipelineCreateInfo,
                                         nullptr,
                                         &gamestate->graphicsPipeline)
                   == VK_SUCCESS,
               "Failed to create graphics pipeline");

    vkDestroyShaderModule(gamestate->device, fragShaderModule, nullptr);
    vkDestroyShaderModule(gamestate->device, vertShaderModule, nullptr);

    // command buffer
    VkCommandPoolCreateInfo commandPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = gamestate->graphicsQueueFamily,
    };

    POM_ASSERT(vkCreateCommandPool(gamestate->device, &commandPoolCreateInfo, nullptr, &gamestate->commandPool)
                   == VK_SUCCESS,
               "Failed to create command pool.");

    gamestate->commandBuffers.resize(gamestate->swapchainFramebuffers.size());

    VkCommandBufferAllocateInfo commandBufferAllocateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = gamestate->commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = static_cast<uint32_t>(gamestate->commandBuffers.size()),
    };

    POM_ASSERT(
        vkAllocateCommandBuffers(gamestate->device, &commandBufferAllocateCreateInfo, gamestate->commandBuffers.data())
            == VK_SUCCESS,
        "Failed to allocate command buffers.");

    // syncronization
    gamestate->imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    gamestate->renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    gamestate->inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    gamestate->imagesInFlight.resize(gamestate->swapchainImages.size(), VK_NULL_HANDLE);

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
        POM_ASSERT(
            vkCreateSemaphore(gamestate->device, &semaphoreCreateInfo, nullptr, &gamestate->imageAvailableSemaphores[i])
                == VK_SUCCESS,
            "Failed to create image available semaphore");

        POM_ASSERT(
            vkCreateSemaphore(gamestate->device, &semaphoreCreateInfo, nullptr, &gamestate->renderFinishedSemaphores[i])
                == VK_SUCCESS,
            "Failed to create render finish semaphore");

        POM_ASSERT(vkCreateFence(gamestate->device, &fenceCreateInfo, nullptr, &gamestate->inFlightFences[i])
                       == VK_SUCCESS,
                   "Failed to create fence");
    }

    // vertex buffer
    VkBufferCreateInfo vertexBufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = sizeof(VERTEX_DATA),
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
    };

    POM_ASSERT(vkCreateBuffer(gamestate->device, &vertexBufferCreateInfo, nullptr, &gamestate->vertexBuffer)
                   == VK_SUCCESS,
               "Failed to create vertex buffer");

    VkMemoryRequirements memoryReqs;
    vkGetBufferMemoryRequirements(gamestate->device, gamestate->vertexBuffer, &memoryReqs);

    VkMemoryAllocateInfo memoryAllocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = memoryReqs.size,
        .memoryTypeIndex = findMemoryType(gamestate->physicalDevice,
                                          memoryReqs.memoryTypeBits,
                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
    };

    POM_ASSERT(vkAllocateMemory(gamestate->device, &memoryAllocInfo, nullptr, &gamestate->vertexBufferMemory)
                   == VK_SUCCESS,
               "Failed to allocate vertex buffer memory.")

    vkBindBufferMemory(gamestate->device, gamestate->vertexBuffer, gamestate->vertexBufferMemory, 0);

    void* data;
    vkMapMemory(gamestate->device, gamestate->vertexBufferMemory, 0, sizeof(VERTEX_DATA), 0, &data);
    memcpy(data, VERTEX_DATA, sizeof(VERTEX_DATA));
    vkUnmapMemory(gamestate->device, gamestate->vertexBufferMemory);
}

POM_CLIENT_EXPORT void clientMount(GameState* gamestate)
{
}

POM_CLIENT_EXPORT void clientUpdate(GameState* gamestate, pom::DeltaTime dt)
{
    if (gamestate->paused) {
        return;
    }

    const u32 frame = pom::Application::get()->getFrame() % MAX_FRAMES_IN_FLIGHT;

    vkWaitForFences(gamestate->device, 1, &gamestate->inFlightFences[frame], VK_TRUE, UINT64_MAX);

    u32 imageIndex = 0;
    VkResult result = vkAcquireNextImageKHR(gamestate->device,
                                            gamestate->swapchain,
                                            UINT64_MAX,
                                            gamestate->imageAvailableSemaphores[frame],
                                            VK_NULL_HANDLE,
                                            &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        POM_LOG_DEBUG("next image: ", imageIndex);
        recreateSwapchain(gamestate, pom::Application::get()->getMainWindow().getVulkanDrawableExtent());
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        POM_LOG_FATAL("Failed to get next swapchain image");
    }

    if (gamestate->imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
        vkWaitForFences(gamestate->device, 1, &gamestate->imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    gamestate->imagesInFlight[imageIndex] = gamestate->inFlightFences[frame];

    VkSemaphore waitSemaphores[] = { gamestate->imageAvailableSemaphores[frame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[] = { gamestate->renderFinishedSemaphores[frame] };

    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pInheritanceInfo = nullptr,
    };

    POM_ASSERT(vkBeginCommandBuffer(gamestate->commandBuffers[imageIndex], &commandBufferBeginInfo) == VK_SUCCESS,
               "Failed to begin recording command buffer.");

    const auto f = [](f32 n) -> f32 {
        const f32 h = (f32)(pom::Application::get()->getFrame() % 255);
        const u32 s = 100;
        const u32 v = 100;
        const f32 k = std::fmod((n + h * 360 / (255 * 60)), 6);
        return v / 255.f - (v / 255.f) * (s / 255.f) * std::max(std::min(std::min(k, 4 - k), 1.f), 0.f);
    };
    VkClearValue clearColor = { f(5), f(3), f(1), 1.f };

    VkRenderPassBeginInfo renderPassBeginInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = gamestate->renderPass,
        .framebuffer = gamestate->swapchainFramebuffers[imageIndex],
        .renderArea = { .offset = { 0, 0 }, .extent = gamestate->swapchainExtent },
        .clearValueCount = 1,
        .pClearValues = &clearColor,
    };

    vkCmdBeginRenderPass(gamestate->commandBuffers[imageIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    // calling this every frame doesn't really matter to my knowledge and is way easier than any other alternatives
    vkCmdSetViewport(gamestate->commandBuffers[imageIndex], 0, 1, &gamestate->swapchainViewport);
    VkRect2D scissor { .offset = { 0, 0 }, .extent = gamestate->swapchainExtent };
    vkCmdSetScissor(gamestate->commandBuffers[imageIndex], 0, 1, &scissor);

    size_t offset = 0;
    vkCmdBindVertexBuffers(gamestate->commandBuffers[imageIndex], 0, 1, &gamestate->vertexBuffer, &offset);

    vkCmdBindPipeline(gamestate->commandBuffers[imageIndex],
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      gamestate->graphicsPipeline);

    vkCmdDraw(gamestate->commandBuffers[imageIndex], 3, 1, 0, 0);

    vkCmdEndRenderPass(gamestate->commandBuffers[imageIndex]);

    POM_ASSERT(vkEndCommandBuffer(gamestate->commandBuffers[imageIndex]) == VK_SUCCESS,
               "Failed to end recording command buffer.");

    VkSubmitInfo submitInfo = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &gamestate->commandBuffers[imageIndex],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores,
    };

    vkResetFences(gamestate->device, 1, &gamestate->inFlightFences[frame]);
    POM_ASSERT(vkQueueSubmit(gamestate->graphicsQueue, 1, &submitInfo, gamestate->inFlightFences[frame]) == VK_SUCCESS,
               "Failed to submit to queue");

    VkSwapchainKHR swapchains[] = { gamestate->swapchain };

    VkPresentInfoKHR presentInfo {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapchains,
        .pImageIndices = &imageIndex,
        .pResults = nullptr,
    };

    result = vkQueuePresentKHR(gamestate->presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        POM_LOG_DEBUG("queue: ", imageIndex);
        recreateSwapchain(gamestate, pom::Application::get()->getMainWindow().getVulkanDrawableExtent());
    } else if (result != VK_SUCCESS) {
        POM_LOG_FATAL("Failed to get next swapchain image");
    }

    vkQueueWaitIdle(gamestate->presentQueue);

    vkDeviceWaitIdle(gamestate->device);

    // POM_LOG_DEBUG("dt: ", dt, "ms");
}

POM_CLIENT_EXPORT void clientOnInputEvent(GameState* gamestate, pom::InputEvent* ev)
{
    if (ev->type == pom::InputEventType::WINDOW_RESIZE) {
        VkExtent2D extent = {
            .width = (u32)ev->getSize().x,
            .height = (u32)ev->getSize().y,
        };
        recreateSwapchain(gamestate, extent);
        clientUpdate(gamestate, {}); // FIXME: this is kinda dumb, this only needs to redraw not re-update
    } else if (ev->type == pom::InputEventType::WINDOW_MINIMIZE) {
        gamestate->paused = true;
    } else if (ev->type == pom::InputEventType::WINDOW_FOCUS) {
        gamestate->paused = false;
    }
}

POM_CLIENT_EXPORT void clientUnmount(GameState* gamestate)
{
}

POM_CLIENT_EXPORT void clientEnd(GameState* gamestate)
{
    vkDestroyBuffer(gamestate->device, gamestate->vertexBuffer, nullptr);
    vkFreeMemory(gamestate->device, gamestate->vertexBufferMemory, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(gamestate->device, gamestate->renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(gamestate->device, gamestate->imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(gamestate->device, gamestate->inFlightFences[i], nullptr);
    }

    vkDestroyCommandPool(gamestate->device, gamestate->commandPool, nullptr);

    for (auto* framebuffer : gamestate->swapchainFramebuffers) {
        vkDestroyFramebuffer(gamestate->device, framebuffer, nullptr);
    }

    vkDestroyPipeline(gamestate->device, gamestate->graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(gamestate->device, gamestate->pipelineLayout, nullptr);

    vkDestroyRenderPass(gamestate->device, gamestate->renderPass, nullptr);

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
