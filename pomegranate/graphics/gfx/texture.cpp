#include "pch.hpp"

#include "texture.hpp"

#include "instance.hpp"

#include "vulkan/instanceVk.hpp"
#include "vulkan/textureVk.hpp"

namespace pom::gfx {
    Ref<Texture> Texture::create(TextureCreateInfo createInfo,
                                 u32 width,
                                 u32 height,
                                 u32 depth,
                                 const void* initialData,
                                 usize initialDataOffset,
                                 usize initialDataSize)
    {
        POM_PROFILE_FUNCTION();
        switch (Instance::get()->getAPI()) {
        case GraphicsAPI::VULKAN: {
            return Ref<Texture>(new TextureVk(dynamic_cast<InstanceVk*>(Instance::get()),
                                              createInfo,
                                              width,
                                              height,
                                              depth,
                                              initialData,
                                              initialDataOffset,
                                              initialDataSize));
        }
        }
    }

    Texture::Texture(TextureCreateInfo createInfo, u32 width, u32 height, u32 depth) :
        extent(width, height, depth), createInfo(createInfo)
    {
    }
} // namespace pom::gfx