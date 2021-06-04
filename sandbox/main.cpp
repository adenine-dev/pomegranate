#include "debug/logging.hpp"
#include "pomegranate/pomegranate.hpp"

#include <memory>

#include <vulkan/vulkan.h>

VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                       VkDebugUtilsMessageTypeFlagsEXT /*types*/,
                       const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                       void* /*userdata*/)
{
    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        POM_LOG_TRACE("vulkan: ", pCallbackData->pMessage);
    } else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        POM_LOG_WARN("vulkan: ", pCallbackData->pMessage);
    } else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        POM_LOG_ERROR("vulkan: ", pCallbackData->pMessage);
    }

    return VK_FALSE;
}

u32 rateDevice(VkPhysicalDevice /*device*/)
{
    return 1;
}

class Sandbox : public pom::Application {
public:
    Sandbox() :
        window("Pomegranate Test Window"), instance([&]() {
            const char* validationLayers[] = { "VK_LAYER_KHRONOS_validation" };

            u32 layerCount = 0;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            auto* availableLayers = new VkLayerProperties[layerCount];
            vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

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

            VkApplicationInfo appInfo = {
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pNext = nullptr,
                .pApplicationName = "test",
                .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
                .pEngineName = "pomegranate",
                .engineVersion = VK_MAKE_VERSION(0, 1, 0),
                .apiVersion = VK_API_VERSION_1_2,
            };

            unsigned int extensionCount = 0;

            POM_ASSERT(SDL_Vulkan_GetInstanceExtensions(window.getSDLHandle(), &extensionCount, nullptr),
                       "Failed to get SDL required vulkan extensions.");

            const char** extensions = new const char*[extensionCount + 1];
            extensions[0] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

            POM_ASSERT(SDL_Vulkan_GetInstanceExtensions(window.getSDLHandle(), &extensionCount, &(extensions[1])),
                       "Failed to get SDL required vulkan extensions.");

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .pNext = nullptr,
                .flags = 0,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                               | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
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

            POM_ASSERT(vkCreateInstance(&createInfo, nullptr, &instance) == VK_SUCCESS, "Failed to initialize vulkan.");

            // cleanup..
            delete[] extensions;
            delete[] availableLayers;

            return instance;
        }()),
        debugMessenger([&]() {
            auto vkCreateDebugUtilsMessengerEXT
                = PFN_vkCreateDebugUtilsMessengerEXT(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
            POM_ASSERT(vkCreateDebugUtilsMessengerEXT, "Unable to find vkCreateDebugUtilsMessengerEXT");
            VkDebugUtilsMessengerCreateInfoEXT messangerCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .pNext = nullptr,
                .flags = 0,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                   | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                               | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                               | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
                .pfnUserCallback = debugCallback,
                .pUserData = nullptr,
            };

            VkDebugUtilsMessengerEXT debugMessenger = nullptr;
            vkCreateDebugUtilsMessengerEXT(instance, &messangerCreateInfo, nullptr, &debugMessenger);
            return debugMessenger;
        }())
    {
        // window.setEventHandler([](const pom::InputEvent& ev) { ev.debugPrint();
        // });

        // TODO: conditionally enable validation layers.

        // device
        VkPhysicalDevice physicalDevice = [&]() -> VkPhysicalDevice {
            u32 deviceCount = 0;
            vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

            POM_ASSERT(deviceCount != 0, "No GPUs to choose from");

            return VK_NULL_HANDLE;
        }();
    }

    void update(float dt) final
    {
    }

    void run()
    {
        // update
        while (!window.shouldClose()) {
            pom::Window::pollEvents();
        }
    }

    ~Sandbox() override
    {
        // cleanup
        auto vkDestroyDebugUtilsMessengerEXT
            = PFN_vkDestroyDebugUtilsMessengerEXT(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        POM_ASSERT(vkDestroyDebugUtilsMessengerEXT, "Unable to find vkDestroyDebugUtilsMessengerEXT");
        vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);

        vkDestroyInstance(instance, nullptr);
    }

private:
    pom::Window window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
};

int main(int /*argc*/, char** /*argv*/)
{
    std::unique_ptr<Sandbox> sandbox = std::make_unique<Sandbox>();

    sandbox->run();

    return 0;
}