#include "pch.hpp"

#include "typesVk.hpp"

namespace pom::gfx {
    constexpr VkFormat toVkFormat(Format f)
    {
        switch (f) {
        case Format::R8_UNORM: {
            return VK_FORMAT_R8_UNORM;
        } break;
        case Format::R8_SNORM: {
            return VK_FORMAT_R8_SNORM;
        } break;
        case Format::R8_USCALED: {
            return VK_FORMAT_R8_USCALED;
        } break;
        case Format::R8_SSCALED: {
            return VK_FORMAT_R8_SSCALED;
        } break;
        case Format::R8_UINT: {
            return VK_FORMAT_R8_UINT;
        } break;
        case Format::R8_SINT: {
            return VK_FORMAT_R8_SINT;
        } break;
        case Format::R8G8_UNORM: {
            return VK_FORMAT_R8G8_UNORM;
        } break;
        case Format::R8G8_SNORM: {
            return VK_FORMAT_R8G8_SNORM;
        } break;
        case Format::R8G8_USCALED: {
            return VK_FORMAT_R8G8_USCALED;
        } break;
        case Format::R8G8_SSCALED: {
            return VK_FORMAT_R8G8_SSCALED;
        } break;
        case Format::R8G8_UINT: {
            return VK_FORMAT_R8G8_UINT;
        } break;
        case Format::R8G8_SINT: {
            return VK_FORMAT_R8G8_SINT;
        } break;
        case Format::R8G8B8_UNORM: {
            return VK_FORMAT_R8G8B8_UNORM;
        } break;
        case Format::R8G8B8_SNORM: {
            return VK_FORMAT_R8G8B8_SNORM;
        } break;
        case Format::R8G8B8_USCALED: {
            return VK_FORMAT_R8G8B8_USCALED;
        } break;
        case Format::R8G8B8_SSCALED: {
            return VK_FORMAT_R8G8B8_SSCALED;
        } break;
        case Format::R8G8B8_UINT: {
            return VK_FORMAT_R8G8B8_UINT;
        } break;
        case Format::R8G8B8_SINT: {
            return VK_FORMAT_R8G8B8_SINT;
        } break;
        case Format::R8G8B8_SRGB: {
            return VK_FORMAT_R8G8B8_SRGB;
        } break;
        case Format::B8G8R8_UNORM: {
            return VK_FORMAT_B8G8R8_UNORM;
        } break;
        case Format::B8G8R8_SNORM: {
            return VK_FORMAT_B8G8R8_SNORM;
        } break;
        case Format::B8G8R8_USCALED: {
            return VK_FORMAT_B8G8R8_USCALED;
        } break;
        case Format::B8G8R8_SSCALED: {
            return VK_FORMAT_B8G8R8_SSCALED;
        } break;
        case Format::B8G8R8_UINT: {
            return VK_FORMAT_B8G8R8_UINT;
        } break;
        case Format::B8G8R8_SINT: {
            return VK_FORMAT_B8G8R8_SINT;
        } break;
        case Format::B8G8R8_SRGB: {
            return VK_FORMAT_B8G8R8_SRGB;
        } break;
        case Format::R8G8B8A8_UNORM: {
            return VK_FORMAT_R8G8B8A8_UNORM;
        } break;
        case Format::R8G8B8A8_SNORM: {
            return VK_FORMAT_R8G8B8A8_SNORM;
        } break;
        case Format::R8G8B8A8_USCALED: {
            return VK_FORMAT_R8G8B8A8_USCALED;
        } break;
        case Format::R8G8B8A8_SSCALED: {
            return VK_FORMAT_R8G8B8A8_SSCALED;
        } break;
        case Format::R8G8B8A8_UINT: {
            return VK_FORMAT_R8G8B8A8_UINT;
        } break;
        case Format::R8G8B8A8_SINT: {
            return VK_FORMAT_R8G8B8A8_SINT;
        } break;
        case Format::R8G8B8A8_SRGB: {
            return VK_FORMAT_R8G8B8A8_SRGB;
        } break;
        case Format::B8G8R8A8_UNORM: {
            return VK_FORMAT_B8G8R8A8_UNORM;
        } break;
        case Format::B8G8R8A8_SNORM: {
            return VK_FORMAT_B8G8R8A8_SNORM;
        } break;
        case Format::B8G8R8A8_USCALED: {
            return VK_FORMAT_B8G8R8A8_USCALED;
        } break;
        case Format::B8G8R8A8_SSCALED: {
            return VK_FORMAT_B8G8R8A8_SSCALED;
        } break;
        case Format::B8G8R8A8_UINT: {
            return VK_FORMAT_B8G8R8A8_UINT;
        } break;
        case Format::B8G8R8A8_SINT: {
            return VK_FORMAT_B8G8R8A8_SINT;
        } break;
        case Format::B8G8R8A8_SRGB: {
            return VK_FORMAT_B8G8R8A8_SRGB;
        } break;
        case Format::R16_UNORM: {
            return VK_FORMAT_R16_UNORM;
        } break;
        case Format::R16_SNORM: {
            return VK_FORMAT_R16_SNORM;
        } break;
        case Format::R16_USCALED: {
            return VK_FORMAT_R16_USCALED;
        } break;
        case Format::R16_SSCALED: {
            return VK_FORMAT_R16_SSCALED;
        } break;
        case Format::R16_UINT: {
            return VK_FORMAT_R16_UINT;
        } break;
        case Format::R16_SINT: {
            return VK_FORMAT_R16_SINT;
        } break;
        case Format::R16_SFLOAT: {
            return VK_FORMAT_R16_SFLOAT;
        } break;
        case Format::R16G16_UNORM: {
            return VK_FORMAT_R16G16_UNORM;
        } break;
        case Format::R16G16_SNORM: {
            return VK_FORMAT_R16G16_SNORM;
        } break;
        case Format::R16G16_USCALED: {
            return VK_FORMAT_R16G16_USCALED;
        } break;
        case Format::R16G16_SSCALED: {
            return VK_FORMAT_R16G16_SSCALED;
        } break;
        case Format::R16G16_UINT: {
            return VK_FORMAT_R16G16_UINT;
        } break;
        case Format::R16G16_SINT: {
            return VK_FORMAT_R16G16_SINT;
        } break;
        case Format::R16G16_SFLOAT: {
            return VK_FORMAT_R16G16_SFLOAT;
        } break;
        case Format::R16G16B16_UNORM: {
            return VK_FORMAT_R16G16B16_UNORM;
        } break;
        case Format::R16G16B16_SNORM: {
            return VK_FORMAT_R16G16B16_SNORM;
        } break;
        case Format::R16G16B16_USCALED: {
            return VK_FORMAT_R16G16B16_USCALED;
        } break;
        case Format::R16G16B16_SSCALED: {
            return VK_FORMAT_R16G16B16_SSCALED;
        } break;
        case Format::R16G16B16_UINT: {
            return VK_FORMAT_R16G16B16_UINT;
        } break;
        case Format::R16G16B16_SINT: {
            return VK_FORMAT_R16G16B16_SINT;
        } break;
        case Format::R16G16B16_SFLOAT: {
            return VK_FORMAT_R16G16B16_SFLOAT;
        } break;
        case Format::R16G16B16A16_UNORM: {
            return VK_FORMAT_R16G16B16A16_UNORM;
        } break;
        case Format::R16G16B16A16_SNORM: {
            return VK_FORMAT_R16G16B16A16_SNORM;
        } break;
        case Format::R16G16B16A16_USCALED: {
            return VK_FORMAT_R16G16B16A16_USCALED;
        } break;
        case Format::R16G16B16A16_SSCALED: {
            return VK_FORMAT_R16G16B16A16_SSCALED;
        } break;
        case Format::R16G16B16A16_UINT: {
            return VK_FORMAT_R16G16B16A16_UINT;
        } break;
        case Format::R16G16B16A16_SINT: {
            return VK_FORMAT_R16G16B16A16_SINT;
        } break;
        case Format::R16G16B16A16_SFLOAT: {
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        } break;
        case Format::R32_UINT: {
            return VK_FORMAT_R32_UINT;
        } break;
        case Format::R32_SINT: {
            return VK_FORMAT_R32_SINT;
        } break;
        case Format::R32_SFLOAT: {
            return VK_FORMAT_R32_SFLOAT;
        } break;
        case Format::R32G32_UINT: {
            return VK_FORMAT_R32G32_UINT;
        } break;
        case Format::R32G32_SINT: {
            return VK_FORMAT_R32G32_SINT;
        } break;
        case Format::R32G32_SFLOAT: {
            return VK_FORMAT_R32G32_SFLOAT;
        } break;
        case Format::R32G32B32_UINT: {
            return VK_FORMAT_R32G32B32_UINT;
        } break;
        case Format::R32G32B32_SINT: {
            return VK_FORMAT_R32G32B32_SINT;
        } break;
        case Format::R32G32B32_SFLOAT: {
            return VK_FORMAT_R32G32B32_SFLOAT;
        } break;
        case Format::R32G32B32A32_UINT: {
            return VK_FORMAT_R32G32B32A32_UINT;
        } break;
        case Format::R32G32B32A32_SINT: {
            return VK_FORMAT_R32G32B32A32_SINT;
        } break;
        case Format::R32G32B32A32_SFLOAT: {
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        } break;
        default: {
            return VK_FORMAT_UNDEFINED;
        }
        };
    }

    constexpr Format fromVkFormat(VkFormat f)
    {
        switch (f) {
        case VK_FORMAT_R8_UNORM: {
            return Format::R8_UNORM;
        } break;
        case VK_FORMAT_R8_SNORM: {
            return Format::R8_SNORM;
        } break;
        case VK_FORMAT_R8_USCALED: {
            return Format::R8_USCALED;
        } break;
        case VK_FORMAT_R8_SSCALED: {
            return Format::R8_SSCALED;
        } break;
        case VK_FORMAT_R8_UINT: {
            return Format::R8_UINT;
        } break;
        case VK_FORMAT_R8_SINT: {
            return Format::R8_SINT;
        } break;
        case VK_FORMAT_R8G8_UNORM: {
            return Format::R8G8_UNORM;
        } break;
        case VK_FORMAT_R8G8_SNORM: {
            return Format::R8G8_SNORM;
        } break;
        case VK_FORMAT_R8G8_USCALED: {
            return Format::R8G8_USCALED;
        } break;
        case VK_FORMAT_R8G8_SSCALED: {
            return Format::R8G8_SSCALED;
        } break;
        case VK_FORMAT_R8G8_UINT: {
            return Format::R8G8_UINT;
        } break;
        case VK_FORMAT_R8G8_SINT: {
            return Format::R8G8_SINT;
        } break;
        case VK_FORMAT_R8G8B8_UNORM: {
            return Format::R8G8B8_UNORM;
        } break;
        case VK_FORMAT_R8G8B8_SNORM: {
            return Format::R8G8B8_SNORM;
        } break;
        case VK_FORMAT_R8G8B8_USCALED: {
            return Format::R8G8B8_USCALED;
        } break;
        case VK_FORMAT_R8G8B8_SSCALED: {
            return Format::R8G8B8_SSCALED;
        } break;
        case VK_FORMAT_R8G8B8_UINT: {
            return Format::R8G8B8_UINT;
        } break;
        case VK_FORMAT_R8G8B8_SINT: {
            return Format::R8G8B8_SINT;
        } break;
        case VK_FORMAT_R8G8B8_SRGB: {
            return Format::R8G8B8_SRGB;
        } break;
        case VK_FORMAT_B8G8R8_UNORM: {
            return Format::B8G8R8_UNORM;
        } break;
        case VK_FORMAT_B8G8R8_SNORM: {
            return Format::B8G8R8_SNORM;
        } break;
        case VK_FORMAT_B8G8R8_USCALED: {
            return Format::B8G8R8_USCALED;
        } break;
        case VK_FORMAT_B8G8R8_SSCALED: {
            return Format::B8G8R8_SSCALED;
        } break;
        case VK_FORMAT_B8G8R8_UINT: {
            return Format::B8G8R8_UINT;
        } break;
        case VK_FORMAT_B8G8R8_SINT: {
            return Format::B8G8R8_SINT;
        } break;
        case VK_FORMAT_B8G8R8_SRGB: {
            return Format::B8G8R8_SRGB;
        } break;
        case VK_FORMAT_R8G8B8A8_UNORM: {
            return Format::R8G8B8A8_UNORM;
        } break;
        case VK_FORMAT_R8G8B8A8_SNORM: {
            return Format::R8G8B8A8_SNORM;
        } break;
        case VK_FORMAT_R8G8B8A8_USCALED: {
            return Format::R8G8B8A8_USCALED;
        } break;
        case VK_FORMAT_R8G8B8A8_SSCALED: {
            return Format::R8G8B8A8_SSCALED;
        } break;
        case VK_FORMAT_R8G8B8A8_UINT: {
            return Format::R8G8B8A8_UINT;
        } break;
        case VK_FORMAT_R8G8B8A8_SINT: {
            return Format::R8G8B8A8_SINT;
        } break;
        case VK_FORMAT_R8G8B8A8_SRGB: {
            return Format::R8G8B8A8_SRGB;
        } break;
        case VK_FORMAT_B8G8R8A8_UNORM: {
            return Format::B8G8R8A8_UNORM;
        } break;
        case VK_FORMAT_B8G8R8A8_SNORM: {
            return Format::B8G8R8A8_SNORM;
        } break;
        case VK_FORMAT_B8G8R8A8_USCALED: {
            return Format::B8G8R8A8_USCALED;
        } break;
        case VK_FORMAT_B8G8R8A8_SSCALED: {
            return Format::B8G8R8A8_SSCALED;
        } break;
        case VK_FORMAT_B8G8R8A8_UINT: {
            return Format::B8G8R8A8_UINT;
        } break;
        case VK_FORMAT_B8G8R8A8_SINT: {
            return Format::B8G8R8A8_SINT;
        } break;
        case VK_FORMAT_B8G8R8A8_SRGB: {
            return Format::B8G8R8A8_SRGB;
        } break;
        case VK_FORMAT_R16_UNORM: {
            return Format::R16_UNORM;
        } break;
        case VK_FORMAT_R16_SNORM: {
            return Format::R16_SNORM;
        } break;
        case VK_FORMAT_R16_USCALED: {
            return Format::R16_USCALED;
        } break;
        case VK_FORMAT_R16_SSCALED: {
            return Format::R16_SSCALED;
        } break;
        case VK_FORMAT_R16_UINT: {
            return Format::R16_UINT;
        } break;
        case VK_FORMAT_R16_SINT: {
            return Format::R16_SINT;
        } break;
        case VK_FORMAT_R16_SFLOAT: {
            return Format::R16_SFLOAT;
        } break;
        case VK_FORMAT_R16G16_UNORM: {
            return Format::R16G16_UNORM;
        } break;
        case VK_FORMAT_R16G16_SNORM: {
            return Format::R16G16_SNORM;
        } break;
        case VK_FORMAT_R16G16_USCALED: {
            return Format::R16G16_USCALED;
        } break;
        case VK_FORMAT_R16G16_SSCALED: {
            return Format::R16G16_SSCALED;
        } break;
        case VK_FORMAT_R16G16_UINT: {
            return Format::R16G16_UINT;
        } break;
        case VK_FORMAT_R16G16_SINT: {
            return Format::R16G16_SINT;
        } break;
        case VK_FORMAT_R16G16_SFLOAT: {
            return Format::R16G16_SFLOAT;
        } break;
        case VK_FORMAT_R16G16B16_UNORM: {
            return Format::R16G16B16_UNORM;
        } break;
        case VK_FORMAT_R16G16B16_SNORM: {
            return Format::R16G16B16_SNORM;
        } break;
        case VK_FORMAT_R16G16B16_USCALED: {
            return Format::R16G16B16_USCALED;
        } break;
        case VK_FORMAT_R16G16B16_SSCALED: {
            return Format::R16G16B16_SSCALED;
        } break;
        case VK_FORMAT_R16G16B16_UINT: {
            return Format::R16G16B16_UINT;
        } break;
        case VK_FORMAT_R16G16B16_SINT: {
            return Format::R16G16B16_SINT;
        } break;
        case VK_FORMAT_R16G16B16_SFLOAT: {
            return Format::R16G16B16_SFLOAT;
        } break;
        case VK_FORMAT_R16G16B16A16_UNORM: {
            return Format::R16G16B16A16_UNORM;
        } break;
        case VK_FORMAT_R16G16B16A16_SNORM: {
            return Format::R16G16B16A16_SNORM;
        } break;
        case VK_FORMAT_R16G16B16A16_USCALED: {
            return Format::R16G16B16A16_USCALED;
        } break;
        case VK_FORMAT_R16G16B16A16_SSCALED: {
            return Format::R16G16B16A16_SSCALED;
        } break;
        case VK_FORMAT_R16G16B16A16_UINT: {
            return Format::R16G16B16A16_UINT;
        } break;
        case VK_FORMAT_R16G16B16A16_SINT: {
            return Format::R16G16B16A16_SINT;
        } break;
        case VK_FORMAT_R16G16B16A16_SFLOAT: {
            return Format::R16G16B16A16_SFLOAT;
        } break;
        case VK_FORMAT_R32_UINT: {
            return Format::R32_UINT;
        } break;
        case VK_FORMAT_R32_SINT: {
            return Format::R32_SINT;
        } break;
        case VK_FORMAT_R32_SFLOAT: {
            return Format::R32_SFLOAT;
        } break;
        case VK_FORMAT_R32G32_UINT: {
            return Format::R32G32_UINT;
        } break;
        case VK_FORMAT_R32G32_SINT: {
            return Format::R32G32_SINT;
        } break;
        case VK_FORMAT_R32G32_SFLOAT: {
            return Format::R32G32_SFLOAT;
        } break;
        case VK_FORMAT_R32G32B32_UINT: {
            return Format::R32G32B32_UINT;
        } break;
        case VK_FORMAT_R32G32B32_SINT: {
            return Format::R32G32B32_SINT;
        } break;
        case VK_FORMAT_R32G32B32_SFLOAT: {
            return Format::R32G32B32_SFLOAT;
        } break;
        case VK_FORMAT_R32G32B32A32_UINT: {
            return Format::R32G32B32A32_UINT;
        } break;
        case VK_FORMAT_R32G32B32A32_SINT: {
            return Format::R32G32B32A32_SINT;
        } break;
        case VK_FORMAT_R32G32B32A32_SFLOAT: {
            return Format::R32G32B32A32_SFLOAT;
        } break;
        default: {
            return Format::UNDEFINED;
        }
        };
    }
} // namespace pom::gfx