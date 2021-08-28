#include "pch.hpp"

#include "texture.hpp"

#include "instance.hpp"

#include "vulkan/instanceVk.hpp"
#include "vulkan/textureVk.hpp"

namespace pom::gfx {
    Texture* Texture::create(TextureCreateInfo createInfo, u32 width, u32 height, u32 depth)
    {
        switch (Instance::get()->getAPI()) {
        case GraphicsAPI::VULKAN: {
            return new TextureVk(dynamic_cast<InstanceVk*>(Instance::get()), createInfo, width, height, depth);
        }
        }
    }

    Texture::Texture(TextureCreateInfo createInfo, u32 width, u32 height, u32 depth) :
        extent(width, height, depth), createInfo(createInfo)
    {
    }
} // namespace pom::gfx