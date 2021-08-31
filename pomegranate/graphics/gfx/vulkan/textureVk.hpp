#pragma once

#include "base.hpp"

#include "../texture.hpp"

namespace pom::gfx {
    class InstanceVk;

    class POM_API TextureVk final : public Texture {
    public:
        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        TextureVk(InstanceVk* instance,
                  TextureCreateInfo createInfo,
                  u32 width,
                  u32 height,
                  u32 depth,
                  const void* initialData,
                  size_t initialDataOffset,
                  size_t initialDataSize);
        ~TextureVk() final;

        [[nodiscard]] inline size_t getSize() const final
        {
            return memorySize;
        }

        [[nodiscard]] VkImage getVkImage()
        {
            return image;
        }

        [[nodiscard]] VkImageView getVkImageView()
        {
            return view;
        }

        [[nodiscard]] VkSampler getVkSampler()
        {
            return sampler;
        }

        [[nodiscard]] inline VkImageLayout getVkImageLayout()
        {
            return imageLayout;
        }

    protected:
        POM_NOCOPY(TextureVk);

        friend class CommandBufferVk;

        InstanceVk* instance;

        VkImageLayout imageLayout;

        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkImageView view = VK_NULL_HANDLE;
        VkSampler sampler = VK_NULL_HANDLE;
        size_t memorySize = 0;
    };
} // namespace pom::gfx