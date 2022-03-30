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
                  usize initialDataOffset,
                  usize initialDataSize);
        ~TextureVk() final;

        [[nodiscard]] inline usize getSize() const final
        {
            return memorySize;
        }

        [[nodiscard]] VkImage getVkImage()
        {
            return image;
        }

        [[nodiscard]] inline VkImageLayout getVkImageLayout()
        {
            return imageLayout;
        }

    protected:
        POM_NOCOPY(TextureVk);

        friend class CommandBufferVk;
        friend class TextureViewVk;

        InstanceVk* instance;

        VkImageLayout imageLayout;

        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;

        usize memorySize = 0;
    };

    class POM_API TextureViewVk final : public TextureView {
    public:
        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        TextureViewVk(Ref<TextureVk> texture, TextureViewCreateInfo createInfo);
        ~TextureViewVk() final;

        [[nodiscard]] VkImageView getVkImageView()
        {
            return view;
        }

        [[nodiscard]] VkSampler getVkSampler()
        {
            return sampler;
        }

    protected:
        POM_NOCOPY(TextureViewVk);
        friend class CommandBufferVk;

        Ref<TextureVk> texture;
        VkImageView view = VK_NULL_HANDLE;
        VkSampler sampler = VK_NULL_HANDLE;
    };
} // namespace pom::gfx