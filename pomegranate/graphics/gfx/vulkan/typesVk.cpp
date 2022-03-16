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
        case Format::D16_UNORM: {
            return VK_FORMAT_D16_UNORM;
        } break;
        case Format::X8_D24_UNORM_PACK32: {
            return VK_FORMAT_X8_D24_UNORM_PACK32;
        } break;
        case Format::D32_SFLOAT: {
            return VK_FORMAT_D32_SFLOAT;
        } break;
        case Format::S8_UINT: {
            return VK_FORMAT_S8_UINT;
        } break;
        case Format::D16_UNORM_S8_UINT: {
            return VK_FORMAT_D16_UNORM_S8_UINT;
        } break;
        case Format::D24_UNORM_S8_UINT: {
            return VK_FORMAT_D24_UNORM_S8_UINT;
        } break;
        case Format::D32_SFLOAT_S8_UINT: {
            return VK_FORMAT_D32_SFLOAT_S8_UINT;
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
        case VK_FORMAT_D16_UNORM: {
            return Format::D16_UNORM;
        } break;
        case VK_FORMAT_X8_D24_UNORM_PACK32: {
            return Format::X8_D24_UNORM_PACK32;
        } break;
        case VK_FORMAT_D32_SFLOAT: {
            return Format::D32_SFLOAT;
        } break;
        case VK_FORMAT_S8_UINT: {
            return Format::S8_UINT;
        } break;
        case VK_FORMAT_D16_UNORM_S8_UINT: {
            return Format::D16_UNORM_S8_UINT;
        } break;
        case VK_FORMAT_D24_UNORM_S8_UINT: {
            return Format::D24_UNORM_S8_UINT;
        } break;
        case VK_FORMAT_D32_SFLOAT_S8_UINT: {
            return Format::D32_SFLOAT_S8_UINT;
        } break;
        default: {
            return Format::UNDEFINED;
        }
        };
    }

    constexpr VkAttachmentLoadOp toVkAttachmentLoadOp(LoadOperation o)
    {
        switch (o) {
        case LoadOperation::LOAD: {
            return VK_ATTACHMENT_LOAD_OP_LOAD;
        } break;
        case LoadOperation::CLEAR: {
            return VK_ATTACHMENT_LOAD_OP_CLEAR;
        } break;
        case LoadOperation::DONTCARE: {
            return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        } break;
        }
    }

    constexpr LoadOperation fromVkAttachmentLoadOp(VkAttachmentLoadOp o)
    {
        switch (o) {
        case VK_ATTACHMENT_LOAD_OP_LOAD: {
            return LoadOperation::LOAD;
        } break;
        case VK_ATTACHMENT_LOAD_OP_CLEAR: {
            return LoadOperation::CLEAR;
        } break;
        case VK_ATTACHMENT_LOAD_OP_DONT_CARE: {
            return LoadOperation::DONTCARE;
        } break;
        default: {
            // FIXME: there has to be a better way to do this.
#ifdef POM_DEBUG_SYMBOLS
            POM_ERROR("Bad vk attachment load operation.");
#endif
            return LoadOperation::LOAD;
        }
        }
    }

    constexpr VkAttachmentStoreOp toVkAttachmentStoreOp(StoreOperation o)
    {
        switch (o) {
        case StoreOperation::STORE: {
            return VK_ATTACHMENT_STORE_OP_STORE;
        } break;
        case StoreOperation::DONTCARE: {
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;
        } break;
        }
    }

    constexpr StoreOperation fromVkAttachmentStoreOp(VkAttachmentStoreOp o)
    {
        switch (o) {
        case VK_ATTACHMENT_STORE_OP_STORE: {
            return StoreOperation::STORE;
        } break;
        case VK_ATTACHMENT_STORE_OP_DONT_CARE: {
            return StoreOperation::DONTCARE;
        } break;
        default: {
            // FIXME: there has to be a better way to do this.
#ifdef POM_DEBUG_SYMBOLS
            POM_ERROR("Bad vk attachment store operation.");
#endif
            return StoreOperation::STORE;
        }
        }
    }

    constexpr VkBufferUsageFlags toVkBufferUsageFlags(BufferUsage u)
    {
        VkBufferUsageFlags ret = 0;

        if (u & BufferUsage::VERTEX) {
            ret |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        }
        if (u & BufferUsage::INDEX) {
            ret |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }
        if (u & BufferUsage::UNIFORM) {
            ret |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        }
        if (u & BufferUsage::TRANSFER_SRC) {
            ret |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        }
        if (u & BufferUsage::TRANSFER_DST) {
            ret |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }

        return ret;
    }

    constexpr BufferUsage fromVkBufferUsageFlags(VkBufferUsageFlags u)
    {
        auto ret = static_cast<BufferUsage>(0);

        if (u & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) {
            ret |= BufferUsage::VERTEX;
        }
        if (u & VK_BUFFER_USAGE_INDEX_BUFFER_BIT) {
            ret |= BufferUsage::INDEX;
        }
        if (u & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
            ret |= BufferUsage::UNIFORM;
        }
        if (u & VK_BUFFER_USAGE_TRANSFER_SRC_BIT) {
            ret |= BufferUsage::TRANSFER_SRC;
        }
        if (u & VK_BUFFER_USAGE_TRANSFER_DST_BIT) {
            ret |= BufferUsage::TRANSFER_DST;
        }

        return ret;
    }

    constexpr VkIndexType toVkIndexType(IndexType t)
    {
        switch (t) {
        case IndexType::U16: {
            return VK_INDEX_TYPE_UINT16;
        } break;
        case IndexType::U32: {
            return VK_INDEX_TYPE_UINT32;
        } break;
        }
    }

    constexpr IndexType fromVkIndexType(VkIndexType t)
    {
        switch (t) {
        case VK_INDEX_TYPE_UINT16: {
            return IndexType::U16;
        } break;
        case VK_INDEX_TYPE_UINT32: {
            return IndexType::U32;
        } break;
        default: {
            // FIXME: there has to be a better way to do this.
#ifdef POM_DEBUG_SYMBOLS
            POM_ERROR("Bad vk index type.");
#endif
            return IndexType::U32;
        }
        }
    }

    POM_API constexpr VkShaderStageFlagBits toVkShaderStageFlagBits(ShaderStage s)
    {
        switch (s) {
        case ShaderStage::VERTEX: {
            return VK_SHADER_STAGE_VERTEX_BIT;
        } break;
        case ShaderStage::FRAGMENT: {
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        } break;
        }
    }

    POM_API constexpr ShaderStage fromVkShaderStageFlagBits(VkShaderStageFlagBits s)
    {
        switch (s) {
        case VK_SHADER_STAGE_VERTEX_BIT: {
            return ShaderStage::VERTEX;
        } break;
        case VK_SHADER_STAGE_FRAGMENT_BIT: {
            return ShaderStage::FRAGMENT;
        } break;
        default: {
            // FIXME: there has to be a better way to do this.
#ifdef POM_DEBUG_SYMBOLS
            POM_ERROR("Bad vk Shader stage flags.");
#endif
            return ShaderStage::VERTEX;
        }
        }
    }

    constexpr VkPrimitiveTopology toVkPrimitiveTopology(PrimitiveTopology t)
    {
        switch (t) {
        case PrimitiveTopology::POINTS: {
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        } break;
        case PrimitiveTopology::LINES: {
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        } break;
        case PrimitiveTopology::LINE_STRIP: {
            return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        } break;
        case PrimitiveTopology::TRIANGLES: {
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        } break;
        case PrimitiveTopology::TRIANGLE_STRIP: {
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        } break;
        case PrimitiveTopology::TRIANGLE_FAN: {
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        } break;
        };
    }

    constexpr PrimitiveTopology fromVkPrimitiveTopology(VkPrimitiveTopology t)
    {
        switch (t) {
        case VK_PRIMITIVE_TOPOLOGY_POINT_LIST: {
            return PrimitiveTopology::POINTS;
        } break;
        case VK_PRIMITIVE_TOPOLOGY_LINE_LIST: {
            return PrimitiveTopology::LINES;
        } break;
        case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP: {
            return PrimitiveTopology::LINE_STRIP;
        } break;
        case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST: {
            return PrimitiveTopology::TRIANGLES;
        } break;
        case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP: {
            return PrimitiveTopology::TRIANGLE_STRIP;
        } break;
        case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN: {
            return PrimitiveTopology::TRIANGLE_FAN;
        } break;
        default: {
            POM_ERROR("Bad vk primitive topology.");
            return PrimitiveTopology::POINTS;
        }
        };
    }

    POM_API constexpr VkCullModeFlags toVkCullMode(CullMode c)
    {
        switch (c) {
        case CullMode::NONE: {
            return VK_CULL_MODE_NONE;
        } break;
        case CullMode::FRONT: {
            return VK_CULL_MODE_FRONT_BIT;
        } break;
        case CullMode::BACK: {
            return VK_CULL_MODE_BACK_BIT;
        } break;
        case CullMode::BOTH: {
            return VK_CULL_MODE_FRONT_AND_BACK;
        } break;
        }
    }

    POM_API constexpr CullMode fromVkCullMode(VkCullModeFlagBits c)
    {
        switch (c) {
        case VK_CULL_MODE_NONE: {
            return CullMode::NONE;
        } break;
        case VK_CULL_MODE_FRONT_BIT: {
            return CullMode::FRONT;
        } break;
        case VK_CULL_MODE_BACK_BIT: {
            return CullMode::BACK;
        } break;
        case VK_CULL_MODE_FRONT_AND_BACK: {
            return CullMode::BOTH;
        } break;
        default: {
            POM_ERROR("Bad vk cull mode.");
            return CullMode::BOTH;
        }
        }
    }

    POM_API constexpr VkViewport toVkViewport(const Viewport& v)
    {
        return { v.x, v.y, v.width, v.height, v.near, v.far };
    }

    POM_API constexpr Viewport fromVkViewport(const VkViewport& v)
    {
        return { v.x, v.y, v.width, v.height, v.minDepth, v.maxDepth };
    }

    POM_API constexpr VkImageType toVkImageType(TextureType t)
    {
        switch (t) {
        case TextureType::IMAGE_1D: {
            return VK_IMAGE_TYPE_1D;
        } break;
        case TextureType::IMAGE_2D: {
            return VK_IMAGE_TYPE_2D;
        } break;
        case TextureType::IMAGE_3D: {
            return VK_IMAGE_TYPE_3D;
        } break;
        }
    }

    POM_API constexpr TextureType fromVkImageType(VkImageType t)
    {
        switch (t) {
        case VK_IMAGE_TYPE_1D: {
            return TextureType::IMAGE_1D;
        } break;
        case VK_IMAGE_TYPE_2D: {
            return TextureType::IMAGE_2D;
        } break;
        case VK_IMAGE_TYPE_3D: {
            return TextureType::IMAGE_3D;
        } break;
        default: {
            POM_FATAL("bad vkImageType");
            return TextureType::IMAGE_2D;
        }
        }
    }

    POM_API constexpr VkImageViewType toVkImageViewType(TextureType t)
    {
        switch (t) {
        case TextureType::IMAGE_1D: {
            return VK_IMAGE_VIEW_TYPE_1D;
        } break;
        case TextureType::IMAGE_2D: {
            return VK_IMAGE_VIEW_TYPE_2D;
        } break;
        case TextureType::IMAGE_3D: {
            return VK_IMAGE_VIEW_TYPE_3D;
        } break;
        }
    }

    POM_API constexpr TextureType fromVkImageViewType(VkImageViewType t)
    {
        switch (t) {
        case VK_IMAGE_VIEW_TYPE_1D: {
            return TextureType::IMAGE_1D;
        } break;
        case VK_IMAGE_VIEW_TYPE_2D: {
            return TextureType::IMAGE_2D;
        } break;
        case VK_IMAGE_VIEW_TYPE_3D: {
            return TextureType::IMAGE_3D;
        } break;
        default: {
            POM_FATAL("bad vk image view type");
            return TextureType::IMAGE_2D;
        }
        }
    }

    POM_API constexpr VkImageUsageFlags toVkImageUsageFlags(TextureUsage u)
    {
        auto ret = static_cast<VkImageUsageFlags>(0);

        if (u & TextureUsage::SAMPLED) {
            ret |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }
        if (u & TextureUsage::STORAGE) {
            ret |= VK_IMAGE_USAGE_STORAGE_BIT;
        }
        if (u & TextureUsage::TRANSFER_SRC) {
            ret |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }
        if (u & TextureUsage::TRANSFER_DST) {
            ret |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }

        return ret;
    }

    POM_API constexpr TextureUsage fromVkImageUsageFlags(VkImageUsageFlags u)
    {
        auto ret = static_cast<TextureUsage>(0);

        if (u & VK_IMAGE_USAGE_SAMPLED_BIT) {
            ret |= TextureUsage::SAMPLED;
        }
        if (u & VK_IMAGE_USAGE_STORAGE_BIT) {
            ret |= TextureUsage::STORAGE;
        }
        if (u & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
            ret |= TextureUsage::TRANSFER_SRC;
        }
        if (u & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
            ret |= TextureUsage::TRANSFER_DST;
        }

        return ret;
    }

    POM_API constexpr VkImageLayout getTheoreticallyIdealImageLayout(TextureUsage u)
    {
        if (u == TextureUsage::SAMPLED) {
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        }
        if (u & TextureUsage::STORAGE) { // All textures used as storage images must be general
            return VK_IMAGE_LAYOUT_GENERAL;
        }
        if (u == TextureUsage::TRANSFER_SRC) {
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        }
        if (u == TextureUsage::TRANSFER_DST) {
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        }

        return VK_IMAGE_LAYOUT_GENERAL;
    }

    POM_API constexpr const char* fromVkResultToString(VkResult r)
    {
        switch (r) {
        case VK_SUCCESS: {
            return "VK_SUCCESS";
        } break;
        case VK_NOT_READY: {
            return "VK_NOT_READY";
        } break;
        case VK_TIMEOUT: {
            return "VK_TIMEOUT";
        } break;
        case VK_EVENT_SET: {
            return "VK_EVENT_SET";
        } break;
        case VK_EVENT_RESET: {
            return "VK_EVENT_RESET";
        } break;
        case VK_INCOMPLETE: {
            return "VK_INCOMPLETE";
        } break;
        case VK_ERROR_OUT_OF_HOST_MEMORY: {
            return "VK_ERROR_OUT_OF_HOST_MEMORY";
        } break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: {
            return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
        } break;
        case VK_ERROR_INITIALIZATION_FAILED: {
            return "VK_ERROR_INITIALIZATION_FAILED";
        } break;
        case VK_ERROR_DEVICE_LOST: {
            return "VK_ERROR_DEVICE_LOST";
        } break;
        case VK_ERROR_MEMORY_MAP_FAILED: {
            return "VK_ERROR_MEMORY_MAP_FAILED";
        } break;
        case VK_ERROR_LAYER_NOT_PRESENT: {
            return "VK_ERROR_LAYER_NOT_PRESENT";
        } break;
        case VK_ERROR_EXTENSION_NOT_PRESENT: {
            return "VK_ERROR_EXTENSION_NOT_PRESENT";
        } break;
        case VK_ERROR_FEATURE_NOT_PRESENT: {
            return "VK_ERROR_FEATURE_NOT_PRESENT";
        } break;
        case VK_ERROR_INCOMPATIBLE_DRIVER: {
            return "VK_ERROR_INCOMPATIBLE_DRIVER";
        } break;
        case VK_ERROR_TOO_MANY_OBJECTS: {
            return "VK_ERROR_TOO_MANY_OBJECTS";
        } break;
        case VK_ERROR_FORMAT_NOT_SUPPORTED: {
            return "VK_ERROR_FORMAT_NOT_SUPPORTED";
        } break;
        case VK_ERROR_FRAGMENTED_POOL: {
            return "VK_ERROR_FRAGMENTED_POOL";
        } break;
        case VK_ERROR_UNKNOWN: {
            return "VK_ERROR_UNKNOWN";
        } break;
        case VK_ERROR_OUT_OF_POOL_MEMORY: {
            return "VK_ERROR_OUT_OF_POOL_MEMORY";
        } break;
        case VK_ERROR_INVALID_EXTERNAL_HANDLE: {
            return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
        } break;
        case VK_ERROR_FRAGMENTATION: {
            return "VK_ERROR_FRAGMENTATION";
        } break;
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS: {
            return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
        } break;
        case VK_ERROR_SURFACE_LOST_KHR: {
            return "VK_ERROR_SURFACE_LOST_KHR";
        } break;
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: {
            return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
        } break;
        case VK_SUBOPTIMAL_KHR: {
            return "VK_SUBOPTIMAL_KHR";
        } break;
        case VK_ERROR_OUT_OF_DATE_KHR: {
            return "VK_ERROR_OUT_OF_DATE_KHR";
        } break;
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: {
            return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
        } break;
        case VK_ERROR_VALIDATION_FAILED_EXT: {
            return "VK_ERROR_VALIDATION_FAILED_EXT";
        } break;
        case VK_ERROR_INVALID_SHADER_NV: {
            return "VK_ERROR_INVALID_SHADER_NV";
        } break;
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: {
            return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
        } break;
        case VK_ERROR_NOT_PERMITTED_EXT: {
            return "VK_ERROR_NOT_PERMITTED_EXT";
        } break;
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT: {
            return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
        } break;
        case VK_THREAD_IDLE_KHR: {
            return "VK_THREAD_IDLE_KHR";
        } break;
        case VK_THREAD_DONE_KHR: {
            return "VK_THREAD_DONE_KHR";
        } break;
        case VK_OPERATION_DEFERRED_KHR: {
            return "VK_OPERATION_DEFERRED_KHR";
        } break;
        case VK_OPERATION_NOT_DEFERRED_KHR: {
            return "VK_OPERATION_NOT_DEFERRED_KHR";
        } break;
        case VK_PIPELINE_COMPILE_REQUIRED_EXT: {
            return "VK_PIPELINE_COMPILE_REQUIRED_EXT";
        } break;
        default: {
            return "Uknown VkResult";
        }
        }
    }

    POM_API constexpr VkDescriptorType toVkDescriptorType(DescriptorType t)
    {
        switch (t) {
        case DescriptorType::COMBINED_TEXTURE_SAMPLER: {
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        } break;
        case DescriptorType::UNIFORM_BUFFER: {
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        } break;
        }
    }

    POM_API constexpr DescriptorType fromVkDescriptorType(VkDescriptorType t)
    {
        switch (t) {
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER: {
            return DescriptorType::COMBINED_TEXTURE_SAMPLER;
        } break;
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: {
            return DescriptorType::UNIFORM_BUFFER;
        } break;
        default: {
            POM_FATAL("unknown vk descriptor type.");
            return DescriptorType::UNIFORM_BUFFER;
        }
        }
    }

    POM_API constexpr VkShaderStageFlags toVkShaderStageFlags(ShaderStageFlags f)
    {
        VkShaderStageFlags ret = 0;

        if (f & ShaderStageFlags::VERTEX) {
            ret |= VK_SHADER_STAGE_VERTEX_BIT;
        }

        if (f & ShaderStageFlags::FRAGMENT) {
            ret |= VK_SHADER_STAGE_FRAGMENT_BIT;
        }

        return ret;
    }

    POM_API constexpr ShaderStageFlags fromVkShaderStageFlags(VkShaderStageFlags f)
    {
        auto ret = static_cast<ShaderStageFlags>(0);

        if (f & VK_SHADER_STAGE_VERTEX_BIT) {
            ret |= ShaderStageFlags::VERTEX;
        }

        if (f & VK_SHADER_STAGE_FRAGMENT_BIT) {
            ret |= ShaderStageFlags::FRAGMENT;
        }

        return ret;
    }

} // namespace pom::gfx