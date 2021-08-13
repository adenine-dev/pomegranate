#include "pch.hpp"

#include "gfx.hpp"
#include "instance.hpp"

namespace pom::gfx {
    std::ostream& operator<<(std::ostream& os, const Format& format)
    {
        switch (format) {
        case Format::UNDEFINED: {
            os << "Format::UNDEFINED";
        } break;
        case Format::R8_UNORM: {
            os << "Format::R8_UNORM";
        } break;
        case Format::R8_SNORM: {
            os << "Format::R8_SNORM";
        } break;
        case Format::R8_USCALED: {
            os << "Format::R8_USCALED";
        } break;
        case Format::R8_SSCALED: {
            os << "Format::R8_SSCALED";
        } break;
        case Format::R8_UINT: {
            os << "Format::R8_UINT";
        } break;
        case Format::R8_SINT: {
            os << "Format::R8_SINT";
        } break;
        case Format::R8G8_UNORM: {
            os << "Format::R8G8_UNORM";
        } break;
        case Format::R8G8_SNORM: {
            os << "Format::R8G8_SNORM";
        } break;
        case Format::R8G8_USCALED: {
            os << "Format::R8G8_USCALED";
        } break;
        case Format::R8G8_SSCALED: {
            os << "Format::R8G8_SSCALED";
        } break;
        case Format::R8G8_UINT: {
            os << "Format::R8G8_UINT";
        } break;
        case Format::R8G8_SINT: {
            os << "Format::R8G8_SINT";
        } break;
        case Format::R8G8B8_UNORM: {
            os << "Format::R8G8B8_UNORM";
        } break;
        case Format::R8G8B8_SNORM: {
            os << "Format::R8G8B8_SNORM";
        } break;
        case Format::R8G8B8_USCALED: {
            os << "Format::R8G8B8_USCALED";
        } break;
        case Format::R8G8B8_SSCALED: {
            os << "Format::R8G8B8_SSCALED";
        } break;
        case Format::R8G8B8_UINT: {
            os << "Format::R8G8B8_UINT";
        } break;
        case Format::R8G8B8_SINT: {
            os << "Format::R8G8B8_SINT";
        } break;
        case Format::R8G8B8_SRGB: {
            os << "Format::R8G8B8_SRGB";
        } break;
        case Format::B8G8R8_UNORM: {
            os << "Format::B8G8R8_UNORM";
        } break;
        case Format::B8G8R8_SNORM: {
            os << "Format::B8G8R8_SNORM";
        } break;
        case Format::B8G8R8_USCALED: {
            os << "Format::B8G8R8_USCALED";
        } break;
        case Format::B8G8R8_SSCALED: {
            os << "Format::B8G8R8_SSCALED";
        } break;
        case Format::B8G8R8_UINT: {
            os << "Format::B8G8R8_UINT";
        } break;
        case Format::B8G8R8_SINT: {
            os << "Format::B8G8R8_SINT";
        } break;
        case Format::B8G8R8_SRGB: {
            os << "Format::B8G8R8_SRGB";
        } break;
        case Format::R8G8B8A8_UNORM: {
            os << "Format::R8G8B8A8_UNORM";
        } break;
        case Format::R8G8B8A8_SNORM: {
            os << "Format::R8G8B8A8_SNORM";
        } break;
        case Format::R8G8B8A8_USCALED: {
            os << "Format::R8G8B8A8_USCALED";
        } break;
        case Format::R8G8B8A8_SSCALED: {
            os << "Format::R8G8B8A8_SSCALED";
        } break;
        case Format::R8G8B8A8_UINT: {
            os << "Format::R8G8B8A8_UINT";
        } break;
        case Format::R8G8B8A8_SINT: {
            os << "Format::R8G8B8A8_SINT";
        } break;
        case Format::R8G8B8A8_SRGB: {
            os << "Format::R8G8B8A8_SRGB";
        } break;
        case Format::B8G8R8A8_UNORM: {
            os << "Format::B8G8R8A8_UNORM";
        } break;
        case Format::B8G8R8A8_SNORM: {
            os << "Format::B8G8R8A8_SNORM";
        } break;
        case Format::B8G8R8A8_USCALED: {
            os << "Format::B8G8R8A8_USCALED";
        } break;
        case Format::B8G8R8A8_SSCALED: {
            os << "Format::B8G8R8A8_SSCALED";
        } break;
        case Format::B8G8R8A8_UINT: {
            os << "Format::B8G8R8A8_UINT";
        } break;
        case Format::B8G8R8A8_SINT: {
            os << "Format::B8G8R8A8_SINT";
        } break;
        case Format::B8G8R8A8_SRGB: {
            os << "Format::B8G8R8A8_SRGB";
        } break;
        case Format::R16_UNORM: {
            os << "Format::R16_UNORM";
        } break;
        case Format::R16_SNORM: {
            os << "Format::R16_SNORM";
        } break;
        case Format::R16_USCALED: {
            os << "Format::R16_USCALED";
        } break;
        case Format::R16_SSCALED: {
            os << "Format::R16_SSCALED";
        } break;
        case Format::R16_UINT: {
            os << "Format::R16_UINT";
        } break;
        case Format::R16_SINT: {
            os << "Format::R16_SINT";
        } break;
        case Format::R16_SFLOAT: {
            os << "Format::R16_SFLOAT";
        } break;
        case Format::R16G16_UNORM: {
            os << "Format::R16G16_UNORM";
        } break;
        case Format::R16G16_SNORM: {
            os << "Format::R16G16_SNORM";
        } break;
        case Format::R16G16_USCALED: {
            os << "Format::R16G16_USCALED";
        } break;
        case Format::R16G16_SSCALED: {
            os << "Format::R16G16_SSCALED";
        } break;
        case Format::R16G16_UINT: {
            os << "Format::R16G16_UINT";
        } break;
        case Format::R16G16_SINT: {
            os << "Format::R16G16_SINT";
        } break;
        case Format::R16G16_SFLOAT: {
            os << "Format::R16G16_SFLOAT";
        } break;
        case Format::R16G16B16_UNORM: {
            os << "Format::R16G16B16_UNORM";
        } break;
        case Format::R16G16B16_SNORM: {
            os << "Format::R16G16B16_SNORM";
        } break;
        case Format::R16G16B16_USCALED: {
            os << "Format::R16G16B16_USCALED";
        } break;
        case Format::R16G16B16_SSCALED: {
            os << "Format::R16G16B16_SSCALED";
        } break;
        case Format::R16G16B16_UINT: {
            os << "Format::R16G16B16_UINT";
        } break;
        case Format::R16G16B16_SINT: {
            os << "Format::R16G16B16_SINT";
        } break;
        case Format::R16G16B16_SFLOAT: {
            os << "Format::R16G16B16_SFLOAT";
        } break;
        case Format::R16G16B16A16_UNORM: {
            os << "Format::R16G16B16A16_UNORM";
        } break;
        case Format::R16G16B16A16_SNORM: {
            os << "Format::R16G16B16A16_SNORM";
        } break;
        case Format::R16G16B16A16_USCALED: {
            os << "Format::R16G16B16A16_USCALED";
        } break;
        case Format::R16G16B16A16_SSCALED: {
            os << "Format::R16G16B16A16_SSCALED";
        } break;
        case Format::R16G16B16A16_UINT: {
            os << "Format::R16G16B16A16_UINT";
        } break;
        case Format::R16G16B16A16_SINT: {
            os << "Format::R16G16B16A16_SINT";
        } break;
        case Format::R16G16B16A16_SFLOAT: {
            os << "Format::R16G16B16A16_SFLOAT";
        } break;
        case Format::R32_UINT: {
            os << "Format::R32_UINT";
        } break;
        case Format::R32_SINT: {
            os << "Format::R32_SINT";
        } break;
        case Format::R32_SFLOAT: {
            os << "Format::R32_SFLOAT";
        } break;
        case Format::R32G32_UINT: {
            os << "Format::R32G32_UINT";
        } break;
        case Format::R32G32_SINT: {
            os << "Format::R32G32_SINT";
        } break;
        case Format::R32G32_SFLOAT: {
            os << "Format::R32G32_SFLOAT";
        } break;
        case Format::R32G32B32_UINT: {
            os << "Format::R32G32B32_UINT";
        } break;
        case Format::R32G32B32_SINT: {
            os << "Format::R32G32B32_SINT";
        } break;
        case Format::R32G32B32_SFLOAT: {
            os << "Format::R32G32B32_SFLOAT";
        } break;
        case Format::R32G32B32A32_UINT: {
            os << "Format::R32G32B32A32_UINT";
        } break;
        case Format::R32G32B32A32_SINT: {
            os << "Format::R32G32B32A32_SINT";
        } break;
        case Format::R32G32B32A32_SFLOAT: {
            os << "Format::R32G32B32A32_SFLOAT";
        } break;
        default: {
            os << "Unknown Format";
        }
        }

        return os;
    }

    size_t sizeofFormat(Format format)
    {
        switch (format) {
        case Format::UNDEFINED: {
            return 0;
        } break;
        case Format::R8_UNORM:
        case Format::R8_SNORM:
        case Format::R8_USCALED:
        case Format::R8_SSCALED:
        case Format::R8_UINT:
        case Format::R8_SINT: {
            return 1;
        } break;
        case Format::R8G8_UNORM:
        case Format::R8G8_SNORM:
        case Format::R8G8_USCALED:
        case Format::R8G8_SSCALED:
        case Format::R8G8_UINT:
        case Format::R8G8_SINT: {
            return 2;
        } break;
        case Format::R8G8B8_UNORM:
        case Format::R8G8B8_SNORM:
        case Format::R8G8B8_USCALED:
        case Format::R8G8B8_SSCALED:
        case Format::R8G8B8_UINT:
        case Format::R8G8B8_SINT:
        case Format::R8G8B8_SRGB:
        case Format::B8G8R8_UNORM:
        case Format::B8G8R8_SNORM:
        case Format::B8G8R8_USCALED:
        case Format::B8G8R8_SSCALED:
        case Format::B8G8R8_UINT:
        case Format::B8G8R8_SINT:
        case Format::B8G8R8_SRGB: {
            return 3;
        } break;
        case Format::R8G8B8A8_UNORM:
        case Format::R8G8B8A8_SNORM:
        case Format::R8G8B8A8_USCALED:
        case Format::R8G8B8A8_SSCALED:
        case Format::R8G8B8A8_UINT:
        case Format::R8G8B8A8_SINT:
        case Format::R8G8B8A8_SRGB:
        case Format::B8G8R8A8_UNORM:
        case Format::B8G8R8A8_SNORM:
        case Format::B8G8R8A8_USCALED:
        case Format::B8G8R8A8_SSCALED:
        case Format::B8G8R8A8_UINT:
        case Format::B8G8R8A8_SINT:
        case Format::B8G8R8A8_SRGB: {
            return 4;
        } break;
        case Format::R16_UNORM:
        case Format::R16_SNORM:
        case Format::R16_USCALED:
        case Format::R16_SSCALED:
        case Format::R16_UINT:
        case Format::R16_SINT:
        case Format::R16_SFLOAT: {
            return 2;
        } break;
        case Format::R16G16_UNORM:
        case Format::R16G16_SNORM:
        case Format::R16G16_USCALED:
        case Format::R16G16_SSCALED:
        case Format::R16G16_UINT:
        case Format::R16G16_SINT:
        case Format::R16G16_SFLOAT: {
            return 4;
        } break;
        case Format::R16G16B16_UNORM:
        case Format::R16G16B16_SNORM:
        case Format::R16G16B16_USCALED:
        case Format::R16G16B16_SSCALED:
        case Format::R16G16B16_UINT:
        case Format::R16G16B16_SINT:
        case Format::R16G16B16_SFLOAT: {
            return 6;
        } break;
        case Format::R16G16B16A16_UNORM:
        case Format::R16G16B16A16_SNORM:
        case Format::R16G16B16A16_USCALED:
        case Format::R16G16B16A16_SSCALED:
        case Format::R16G16B16A16_UINT:
        case Format::R16G16B16A16_SINT:
        case Format::R16G16B16A16_SFLOAT: {
            return 8;
        } break;
        case Format::R32_UINT:
        case Format::R32_SINT:
        case Format::R32_SFLOAT: {
            return 4;
        } break;
        case Format::R32G32_UINT:
        case Format::R32G32_SINT:
        case Format::R32G32_SFLOAT: {
            return 8;
        } break;
        case Format::R32G32B32_UINT:
        case Format::R32G32B32_SINT:
        case Format::R32G32B32_SFLOAT: {
            return 12;
        } break;
        case Format::R32G32B32A32_UINT:
        case Format::R32G32B32A32_SINT:
        case Format::R32G32B32A32_SFLOAT: {
            return 16;
        } break;
        default: {
            return 0;
        }
        }
    }

    void shutdown()
    {
        Instance::destroy();
    }
} // namespace pom::gfx