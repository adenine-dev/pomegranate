#include "chione/chione.hpp"
#include "debug/logging.hpp"
#include "debug/profiler.hpp"

VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                       VkDebugUtilsMessageTypeFlagsEXT,
                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                       void*)
{
    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        ce::_log(std::cout, ce::terminal::grey, "[VK_TRACE]", ce::terminal::reset, ": ", pCallbackData->pMessage);
    else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        ce::_log(std::cout, ce::terminal::yellow, "[VK_WARN]", ce::terminal::reset, ": ", pCallbackData->pMessage);
    else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        ce::_log(std::cout, ce::terminal::red, "[VK_ERROR]", ce::terminal::reset, ": ", pCallbackData->pMessage);

    return VK_FALSE;
}

int main(int, char*[])
{
    // init
    ce::Window window("Chione Test Window");
    // window.setEventHandler([](const ce::InputEvent& ev) { ev.debugPrint(); });

    // TODO: conditionally enable validation layers.

    VkInstance instance = [&]() {
        const char* validationLayers[] = { "VK_LAYER_KHRONOS_validation" };

        u32 layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        VkLayerProperties* availableLayers = new VkLayerProperties[layerCount];
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

        for (auto layer : validationLayers) {
            bool found = false;
            for (u16 i = 0; i < layerCount; i++) {
                if (strcmp(layer, availableLayers[i].layerName) == 0) {
                    found = true;
                    break;
                }
            }
            CE_ASSERT(found, "was unable to find requested layer: ", layer);
        }

        VkApplicationInfo appInfo = {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = "test",
            .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
            .pEngineName = "chione",
            .engineVersion = VK_MAKE_VERSION(0, 1, 0),
            .apiVersion = VK_API_VERSION_1_2,
        };

        unsigned int extensionCount = 0;

        CE_ASSERT(SDL_Vulkan_GetInstanceExtensions(window.getSDLHandle(), &extensionCount, nullptr),
                  "Failed to get SDL required vulkan extensions.");

        const char** extensions = new const char*[extensionCount + 1];
        extensions[0] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

        CE_ASSERT(SDL_Vulkan_GetInstanceExtensions(window.getSDLHandle(), &extensionCount, &(extensions[1])),
                  "Failed to get SDL required vulkan extensions.");

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debugCallback,
            .pUserData = nullptr,
        };

        VkInstanceCreateInfo createInfo = {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo,
            .flags = 0,
            .pApplicationInfo = &appInfo,
            .enabledLayerCount = 1,
            .ppEnabledLayerNames = validationLayers,
            .enabledExtensionCount = static_cast<u32>(extensionCount) + 1,
            .ppEnabledExtensionNames = extensions,
        };

        CE_ASSERT(vkCreateInstance(&createInfo, nullptr, &instance) == VK_SUCCESS, "Failed to initialize vulkan.");

        // cleanup..
        delete[] extensions;
        delete[] availableLayers;

        return instance;
    }();

    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT
        = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    CE_ASSERT(vkCreateDebugUtilsMessengerEXT, "Unable to find vkCreateDebugUtilsMessengerEXT");

    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT
        = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    CE_ASSERT(vkDestroyDebugUtilsMessengerEXT, "Unable to find vkDestroyDebugUtilsMessengerEXT");

    VkDebugUtilsMessengerEXT debugMessenger = [&instance, &vkCreateDebugUtilsMessengerEXT]() {
        VkDebugUtilsMessengerCreateInfoEXT messangerCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .pNext = nullptr,
            .flags = 0,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debugCallback,
            .pUserData = nullptr,
        };

        VkDebugUtilsMessengerEXT debugMessenger;
        vkCreateDebugUtilsMessengerEXT(instance, &messangerCreateInfo, nullptr, &debugMessenger);
        return debugMessenger;
    }();

    // update
    while (!window.shouldClose()) {
        ce::Window::pollEvents();
    }

    // cleanup
    vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    vkDestroyInstance(instance, nullptr);

    return 0;
}