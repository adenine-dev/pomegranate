#pragma once

#include "base.hpp"

#include "gfx.hpp"
#include "maths/vector.hpp"
#include "util/bitflags.hpp"

namespace pom::gfx {

#ifndef DOXYGEN // Hide this because technically it doesn't matter, the namespace is only really here so that the
                // members of the enum do not pollute the namespace.
    namespace TextureUsageNamespace {
#endif
        enum TextureUsage {
            SAMPLED = 1 << 0,
            STORAGE = 1 << 1,
            TRANSFER_SRC = 1 << 2,
            TRANSFER_DST = 1 << 3,
        };

#ifndef DOXYGEN
    } // namespace TextureUsageNamespace

    using TextureUsage = TextureUsageNamespace::TextureUsage;
#endif

    enum class TextureType {
        IMAGE_1D,
        IMAGE_2D, // NOTE: this type is compatable with both VK_IMAGE_TYPE_2D and VK_IMAGE_TYPE_3D, default to 2D?
        IMAGE_3D,
        // TEXTURE_1D_ARRAY,
        // TEXTURE_2D_ARRAY, // NOTE: this type is compatable with VK_IMAGE_TYPE_2D and VK_IMAGE_TYPE_3D, default to 3D?
        // TEXTURE_3D_ARRAY,
        // CUBE,
        // CUBE_ARRAY,
    };

    struct TextureCreateInfo {
        TextureType type;
        TextureUsage usage;
        Format textureFormat;
        Format viewFormat;
        // TODO: swizzle, subresource range, sampler params, etc
    };

    class POM_API Texture {
    public:
        /// Returns the GraphicsAPI associated with this texture.
        [[nodiscard]] constexpr virtual GraphicsAPI getAPI() const = 0;

        [[nodiscard]] static Ref<Texture> create(TextureCreateInfo createInfo,
                                                 u32 width,
                                                 u32 height = 1u,
                                                 u32 depth = 1u,
                                                 const void* initialData = nullptr,
                                                 size_t initialDataOffset = 0,
                                                 size_t initialDataSize = 0);

        [[nodiscard]] inline TextureUsage getUsage() const
        {
            return createInfo.usage;
        }
        [[nodiscard]] inline u32 getWidth() const
        {
            return extent.x;
        }
        [[nodiscard]] inline u32 getHeight() const
        {
            return extent.y;
        }

        [[nodiscard]] inline u32 getDepth() const
        {
            return extent.z;
        }

        [[nodiscard]] const maths::uvec3& getExtent() const
        {
            return extent;
        }

        [[nodiscard]] virtual inline size_t getSize() const = 0;

        virtual ~Texture() = default;

    protected:
        POM_NOCOPY(Texture);

        Texture(TextureCreateInfo createInfo, u32 width, u32 height, u32 depth);

        maths::uvec3 extent;

        TextureCreateInfo createInfo;
    };
} // namespace pom::gfx