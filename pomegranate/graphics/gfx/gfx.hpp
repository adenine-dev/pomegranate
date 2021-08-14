#pragma once

#include "base.hpp"

#ifndef POM_MAX_FRAMES_IN_FLIGHT
#    define POM_MAX_FRAMES_IN_FLIGHT 2
#endif

namespace pom::gfx {
    /// @addtogroup gfx
    /// @{

    /// Determines the low level graphics api. Currently only supports vulkan.
    enum class GraphicsAPI { VULKAN };

    struct Viewport {
        f32 x, y;
        f32 width, height;
        f32 near, far;
    };

    // clang-format off
    // all documentation on 1 line even if it overflows 120 characters.

    /// Specifies image format, used for textures, framebuffers, etc. Currently maps directly to vulkan's `VkFormat` however this may change in the future.\n
    /// Normalized and scaled formats (`_*NORM` and `_*SCALED`) are integers sampled as floats, where values map to [-1, 1] (signed) or [0, 1] (unsigned).\n
    /// Int formats (`_*INT`) are integers sampled as integers.\n
    /// Float formats (`_*FLOAT`) are floats sampled as floats.\n
    /// SRGB formats (`_SRGB`) are unsigned integers with R, G, and B in the sRGB colorspace sampled as floats, the A component is sampled as an unsigned integer if present.
    enum class Format {
        //NOTE: maybe have these be defined as the API specific values for single api builds?
        //NOTE: when adding other apis add their data here.
        //TODO: when all the other apis are in place make this more concrete, should it include api specific formats?
        UNDEFINED, ///< Undefined image format (duh).

        R8_UNORM,   ///< 8 bit normalized unsigned int R channel, corresponds to `VK_FORMAT_R8_UNORM` in vulkan.
        R8_SNORM,   ///< 8 bit normalized signed int R channel, corresponds to `VK_FORMAT_R8_SNORM` in vulkan.
        R8_USCALED, ///< 8 bit scaled unsigned int R channel, corresponds to `VK_FORMAT_R8_USCALED` in vulkan.
        R8_SSCALED, ///< 8 bit scaled unsigned int R channel, corresponds to `VK_FORMAT_R8_SSCALED` in vulkan.
        R8_UINT,    ///< 8 bit unsigned int R channel, corresponds to `VK_FORMAT_R8_UINT` in vulkan.
        R8_SINT,    ///< 8 bit signed int R channel, corresponds to `VK_FORMAT_R8_SINT` in vulkan.
        // R8_SRGB, ///< corresponds to `VK_FORMAT_R8_SRGB` in vulkan. //NOTE: gl doesn't have this?

        R8G8_UNORM,   ///< 8 bit normalized unsigned int R and G channels, corresponds to `VK_FORMAT_R8G8_UNORM` in vulkan.
        R8G8_SNORM,   ///< 8 bit normalized signed int R and G channels, corresponds to `VK_FORMAT_R8G8_SNORM` in vulkan.
        R8G8_USCALED, ///< 8 bit scaled unsigned int R and G channels, corresponds to `VK_FORMAT_R8G8_USCALED` in vulkan.
        R8G8_SSCALED, ///< 8 bit scaled unsigned int R and G channels corresponds to `VK_FORMAT_R8G8_SSCALED` in vulkan.
        R8G8_UINT,    ///< 8 bit unsigned int R and G channels, corresponds to `VK_FORMAT_R8G8_UINT` in vulkan.
        R8G8_SINT,    ///< 8 bit signed int R and G channels, corresponds to `VK_FORMAT_R8G8_SINT` in vulkan.
        // R8G8_SRGB, ///< corresponds to `VK_FORMAT_R8G8_SRGB` in vulkan.

        R8G8B8_UNORM,   ///< 8 bit normalized unsigned int R, G, and B channels, corresponds to `VK_FORMAT_R8G8B8_UNORM` in vulkan.
        R8G8B8_SNORM,   ///< 8 bit normalized signed int R, G, and B channels, corresponds to `VK_FORMAT_R8G8B8_SNORM` in vulkan.
        R8G8B8_USCALED, ///< 8 bit scaled unsigned int R, G, and B channels, corresponds to `VK_FORMAT_R8G8B8_USCALED` in vulkan.
        R8G8B8_SSCALED, ///< 8 bit scaled unsigned int R, G, and B channels corresponds to `VK_FORMAT_R8G8B8_SSCALED` in vulkan.
        R8G8B8_UINT,    ///< 8 bit unsigned int R, G, and B channels, corresponds to `VK_FORMAT_R8G8B8_UINT` in vulkan.
        R8G8B8_SINT,    ///< 8 bit signed int R, G, and B channels, corresponds to `VK_FORMAT_R8G8B8_SINT` in vulkan.
        R8G8B8_SRGB,    ///< sRGB R, G, and B channels, corresponds to `VK_FORMAT_R8G8B8_SRGB` in vulkan.

        B8G8R8_UNORM,   ///< 8 bit normalized unsigned int B, G, and R channels, corresponds to `VK_FORMAT_B8G8R8_UNORM` in vulkan.
        B8G8R8_SNORM,   ///< 8 bit normalized signed int B, G, and R channels, corresponds to `VK_FORMAT_B8G8R8_SNORM` in vulkan.
        B8G8R8_USCALED, ///< 8 bit scaled unsigned int B, G, and R channels, corresponds to `VK_FORMAT_B8G8R8_USCALED` in vulkan.
        B8G8R8_SSCALED, ///< 8 bit scaled unsigned int B, G, and R channel, corresponds to `VK_FORMAT_B8G8R8_SSCALED` in vulkan.
        B8G8R8_UINT,    ///< 8 bit unsigned int B, G, and R channels, corresponds to `VK_FORMAT_B8G8R8_UINT` in vulkan.
        B8G8R8_SINT,    ///< 8 bit signed int B, G, and R channels, corresponds to `VK_FORMAT_B8G8R8_SINT` in vulkan.
        B8G8R8_SRGB,    ///< sRGB B, G, and R channels, corresponds to `VK_FORMAT_B8G8R8_SRGB` in vulkan.

        R8G8B8A8_UNORM,   ///< 8 bit normalized unsigned int R, G, B, and A channels, corresponds to `VK_FORMAT_R8G8B8A8_UNORM` in vulkan.
        R8G8B8A8_SNORM,   ///< 8 bit normalized signed int R, G, B, and A channels, corresponds to `VK_FORMAT_R8G8B8A8_SNORM` in vulkan.
        R8G8B8A8_USCALED, ///< 8 bit scaled unsigned int R, G, B, and A channels, corresponds to `VK_FORMAT_R8G8B8A8_USCALED` in vulkan.
        R8G8B8A8_SSCALED, ///< 8 bit scaled unsigned int R, G, B, and A channel, corresponds to `VK_FORMAT_R8G8B8A8_SSCALED` in vulkan.
        R8G8B8A8_UINT,    ///< 8 bit unsigned int R, G, B, and A channels, corresponds to `VK_FORMAT_R8G8B8A8_UINT` in vulkan.
        R8G8B8A8_SINT,    ///< 8 bit signed int R, G, B, and A channels, corresponds to `VK_FORMAT_R8G8B8A8_SINT` in vulkan.
        R8G8B8A8_SRGB,    ///< sRGB R, G, B, and 8 bit A channels, corresponds to `VK_FORMAT_R8G8B8A8_SRGB` in vulkan.

        B8G8R8A8_UNORM,   ///< 8 bit normalized unsigned int B, G, R, A channels, corresponds to `VK_FORMAT_B8G8R8A8_UNORM` in vulkan.
        B8G8R8A8_SNORM,   ///< 8 bit normalized signed int B, G, R, A channels, corresponds to `VK_FORMAT_B8G8R8A8_SNORM` in vulkan.
        B8G8R8A8_USCALED, ///< 8 bit scaled unsigned int B, G, R, A channels, corresponds to `VK_FORMAT_B8G8R8A8_USCALED` in vulkan.
        B8G8R8A8_SSCALED, ///< 8 bit scaled unsigned int B, G, R, A channel, corresponds to `VK_FORMAT_B8G8R8A8_SSCALED` in vulkan.
        B8G8R8A8_UINT,    ///< 8 bit unsigned int B, G, R, A channels, corresponds to `VK_FORMAT_B8G8R8A8_UINT` in vulkan.
        B8G8R8A8_SINT,    ///< 8 bit signed int B, G, R, A channels, corresponds to `VK_FORMAT_B8G8R8A8_SINT` in vulkan.
        B8G8R8A8_SRGB,    ///< sRGB B, G, R, and 8 bit A channels, corresponds to `VK_FORMAT_B8G8R8A8_SRGB` in vulkan.

        // A8B8G8R8_UNORM_PACK32,   ///< corresponds to `VK_FORMAT_A8B8G8R8_UNORM_PACK32` in vulkan.
        // A8B8G8R8_SNORM_PACK32,   ///< corresponds to `VK_FORMAT_A8B8G8R8_SNORM_PACK32` in vulkan.
        // A8B8G8R8_USCALED_PACK32, ///< corresponds to `VK_FORMAT_A8B8G8R8_USCALED_PACK32` in vulkan.
        // A8B8G8R8_SSCALED_PACK32, ///< corresponds to `VK_FORMAT_A8B8G8R8_SSCALED_PACK32` in vulkan.
        // A8B8G8R8_UINT_PACK32,    ///< corresponds to `VK_FORMAT_A8B8G8R8_UINT_PACK32` in vulkan.
        // A8B8G8R8_SINT_PACK32,    ///< corresponds to `VK_FORMAT_A8B8G8R8_SINT_PACK32` in vulkan.
        // A8B8G8R8_SRGB_PACK32, ///< corresponds to `VK_FORMAT_A8B8G8R8_SRGB_PACK32` in vulkan.

        // A2R10G10B10_UNORM_PACK32, ///< corresponds to `VK_FORMAT_A2R10G10B10_UNORM_PACK32` in vulkan.
        // A2R10G10B10_SNORM_PACK32, ///< corresponds to `VK_FORMAT_A2R10G10B10_SNORM_PACK32` in vulkan.
        // A2R10G10B10_USCALED_PACK32, ///< corresponds to `VK_FORMAT_A2R10G10B10_USCALED_PACK32` in vulkan.
        // A2R10G10B10_SSCALED_PACK32, ///< corresponds to `VK_FORMAT_A2R10G10B10_SSCALED_PACK32` in vulkan.
        // A2R10G10B10_UINT_PACK32, ///< corresponds to `VK_FORMAT_A2R10G10B10_UINT_PACK32` in vulkan.
        // A2R10G10B10_SINT_PACK32, ///< corresponds to `VK_FORMAT_A2R10G10B10_SINT_PACK32` in vulkan.

        // A2B10G10R10_UNORM_PACK32, ///< corresponds to `VK_FORMAT_A2B10G10R10_UNORM_PACK32` in vulkan.
        // A2B10G10R10_SNORM_PACK32, ///< corresponds to `VK_FORMAT_A2B10G10R10_SNORM_PACK32` in vulkan.
        // A2B10G10R10_USCALED_PACK32, ///< corresponds to `VK_FORMAT_A2B10G10R10_USCALED_PACK32` in vulkan.
        // A2B10G10R10_SSCALED_PACK32, ///< corresponds to `VK_FORMAT_A2B10G10R10_SSCALED_PACK32` in vulkan.
        // A2B10G10R10_UINT_PACK32, ///< corresponds to `VK_FORMAT_A2B10G10R10_UINT_PACK32` in vulkan.
        // A2B10G10R10_SINT_PACK32, ///< corresponds to `VK_FORMAT_A2B10G10R10_SINT_PACK32` in vulkan.

        R16_UNORM,   ///< 16 bit normalized unsigned int R channel, corresponds to `VK_FORMAT_R16_UNORM` in vulkan.
        R16_SNORM,   ///< 16 bit normalized signed int R channel, corresponds to `VK_FORMAT_R16_SNORM` in vulkan.
        R16_USCALED, ///< 16 bit scaled unsigned int R channel, corresponds to `VK_FORMAT_R16_USCALED` in vulkan.
        R16_SSCALED, ///< 16 bit scaled unsigned int R channel, corresponds to `VK_FORMAT_R16_SSCALED` in vulkan.
        R16_UINT,    ///< 16 bit unsigned int R channel, corresponds to `VK_FORMAT_R16_UINT` in vulkan.
        R16_SINT,    ///< 16 bit signed int R channel, corresponds to `VK_FORMAT_R16_SINT` in vulkan.
        R16_SFLOAT,  ///< 16 bit floating point R channel, corresponds to `VK_FORMAT_R16_SFLOAT` in vulkan.

        R16G16_UNORM,   ///< 16 bit normalized unsigned int R and G channels, corresponds to `VK_FORMAT_R16G16_UNORM` in vulkan.
        R16G16_SNORM,   ///< 16 bit normalized signed int R and G channels, corresponds to `VK_FORMAT_R16G16_SNORM` in vulkan.
        R16G16_USCALED, ///< 16 bit scaled unsigned int R and G channels, corresponds to `VK_FORMAT_R16G16_USCALED` in vulkan.
        R16G16_SSCALED, ///< 16 bit scaled unsigned int R and G channels, corresponds to `VK_FORMAT_R16G16_SSCALED` in vulkan.
        R16G16_UINT,    ///< 16 bit unsigned int R and G channels, corresponds to `VK_FORMAT_R16G16_UINT` in vulkan.
        R16G16_SINT,    ///< 16 bit signed int R and G channels, corresponds to `VK_FORMAT_R16G16_SINT` in vulkan.
        R16G16_SFLOAT,  ///< 16 bit floating point R and G channels, corresponds to `VK_FORMAT_R16G16_SFLOAT` in vulkan.

        R16G16B16_UNORM,   ///< 16 bit normalized unsigned int R and G channels, corresponds to `VK_FORMAT_R16G16B16_UNORM` in vulkan.
        R16G16B16_SNORM,   ///< 16 bit normalized signed int R and G channels, corresponds to `VK_FORMAT_R16G16B16_SNORM` in vulkan.
        R16G16B16_USCALED, ///< 16 bit scaled unsigned int R and G channels, corresponds to `VK_FORMAT_R16G16B16_USCALED` in vulkan.
        R16G16B16_SSCALED, ///< 16 bit scaled unsigned int R and G channels, corresponds to `VK_FORMAT_R16G16B16_SSCALED` in vulkan.
        R16G16B16_UINT,    ///< 16 bit unsigned int R and G channels, corresponds to `VK_FORMAT_R16G16B16_UINT` in vulkan.
        R16G16B16_SINT,    ///< 16 bit signed int R and G channels, corresponds to `VK_FORMAT_R16G16B16_SINT` in vulkan.
        R16G16B16_SFLOAT,  ///< 16 bit floating point R and G channels, corresponds to `VK_FORMAT_R16G16B16_SFLOAT` in vulkan.

        R16G16B16A16_UNORM,   ///< 16 bit normalized unsigned int R, G, and B channels, corresponds to `VK_FORMAT_R16G16B16A16_UNORM` in vulkan.
        R16G16B16A16_SNORM,   ///< 16 bit normalized signed int R, G, and B channels, corresponds to `VK_FORMAT_R16G16B16A16_SNORM` in vulkan.
        R16G16B16A16_USCALED, ///< 16 bit scaled unsigned int R, G, and B channels, corresponds to `VK_FORMAT_R16G16B16A16_USCALED` in vulkan.
        R16G16B16A16_SSCALED, ///< 16 bit scaled unsigned int R, G, and B channels, corresponds to `VK_FORMAT_R16G16B16A16_SSCALED` in vulkan.
        R16G16B16A16_UINT,    ///< 16 bit unsigned int R, G, and B channels, corresponds to `VK_FORMAT_R16G16B16A16_UINT` in vulkan.
        R16G16B16A16_SINT,    ///< 16 bit signed int R, G, and B channels, corresponds to `VK_FORMAT_R16G16B16A16_SINT` in vulkan.
        R16G16B16A16_SFLOAT,  ///< 16 bit floating point R, G, and B channels,  corresponds to `VK_FORMAT_R16G16B16A16_SFLOAT` in vulkan.

        R32_UINT,   ///< 32 bit unsigned integer R channel, corresponds to `VK_FORMAT_R32_UINT` in vulkan.
        R32_SINT,   ///< 32 bit integer R channel, corresponds to `VK_FORMAT_R32_SINT` in vulkan.
        R32_SFLOAT, ///< 32 bit floating point R channel, corresponds to `VK_FORMAT_R32_SFLOAT` in vulkan.
        
        R32G32_UINT,   ///< 32 bit unsigned integer R and G channels, corresponds to `VK_FORMAT_R32G32_UINT` in vulkan.
        R32G32_SINT,   ///< 32 bit integer R and G channels, corresponds to `VK_FORMAT_R32G32_SINT` in vulkan.
        R32G32_SFLOAT, ///< 32 bit floating point R and G channels, corresponds to `VK_FORMAT_R32G32_SFLOAT` in vulkan.

        R32G32B32_UINT,   ///< 32 bit unsigned integer R, G, and B channels, corresponds to `VK_FORMAT_R32G32B32_UINT` in vulkan.
        R32G32B32_SINT,   ///< 32 bit integer R, G, and B channels, corresponds to `VK_FORMAT_R32G32B32_SINT` in vulkan.
        R32G32B32_SFLOAT, ///< 32 bit floating point R, G, and B channels, corresponds to `VK_FORMAT_R32G32B32_SFLOAT` in vulkan.

        R32G32B32A32_UINT,   ///< 32 bit unsigned integer R, G, B, and A channels, corresponds to `VK_FORMAT_R32G32B32A32_UINT` in vulkan.
        R32G32B32A32_SINT,   ///< 32 bit integer R, G, B, and A channels, corresponds to `VK_FORMAT_R32G32B32A32_SINT` in vulkan.
        R32G32B32A32_SFLOAT, ///< 32 bit floating point R, G, B, and A channels, corresponds to `VK_FORMAT_R32G32B32A32_SFLOAT` in vulkan.

        // R64_UINT,   ///< corresponds to `VK_FORMAT_R64_UINT` in vulkan.
        // R64_SINT,   ///< corresponds to `VK_FORMAT_R64_SINT` in vulkan.
        // R64_SFLOAT, ///< corresponds to `VK_FORMAT_R64_SFLOAT` in vulkan.

        // R64G64_UINT,   ///< corresponds to `VK_FORMAT_R64G64_UINT` in vulkan.
        // R64G64_SINT,   ///< corresponds to `VK_FORMAT_R64G64_SINT` in vulkan.
        // R64G64_SFLOAT, ///< corresponds to `VK_FORMAT_R64G64_SFLOAT` in vulkan.

        // R64G64B64_UINT,   ///< corresponds to `VK_FORMAT_R64G64B64_UINT` in vulkan.
        // R64G64B64_SINT,   ///< corresponds to `VK_FORMAT_R64G64B64_SINT` in vulkan.
        // R64G64B64_SFLOAT, ///< corresponds to `VK_FORMAT_R64G64B64_SFLOAT` in vulkan.

        // R64G64B64A64_UINT,   ///< corresponds to `VK_FORMAT_R64G64B64A64_UINT` in vulkan.
        // R64G64B64A64_SINT,   ///< corresponds to `VK_FORMAT_R64G64B64A64_SINT` in vulkan.
        // R64G64B64A64_SFLOAT, ///< corresponds to `VK_FORMAT_R64G64B64A64_SFLOAT` in vulkan.

        // B10G11R11_UFLOAT_PACK32, ///< corresponds to `VK_FORMAT_B10G11R11_UFLOAT_PACK32` in vulkan.
        // E5B9G9R9_UFLOAT_PACK32, ///< corresponds to `VK_FORMAT_E5B9G9R9_UFLOAT_PACK32` in vulkan.
        // D16_UNORM, ///< corresponds to `VK_FORMAT_D16_UNORM` in vulkan.
        // X8_D24_UNORM_PACK32, ///< corresponds to `VK_FORMAT_X8_D24_UNORM_PACK32` in vulkan.
        // D32_SFLOAT, ///< corresponds to `VK_FORMAT_D32_SFLOAT` in vulkan.
        // S8_UINT, ///< corresponds to `VK_FORMAT_S8_UINT` in vulkan.
        // D16_UNORM_S8_UINT, ///< corresponds to `VK_FORMAT_D16_UNORM_S8_UINT` in vulkan.
        // D24_UNORM_S8_UINT, ///< corresponds to `VK_FORMAT_D24_UNORM_S8_UINT` in vulkan.
        // D32_SFLOAT_S8_UINT, ///< corresponds to `VK_FORMAT_D32_SFLOAT_S8_UINT` in vulkan.
        // BC1_RGB_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_BC1_RGB_UNORM_BLOCK` in vulkan.
        // BC1_RGB_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_BC1_RGB_SRGB_BLOCK` in vulkan.
        // BC1_RGBA_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_BC1_RGBA_UNORM_BLOCK` in vulkan.
        // BC1_RGBA_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_BC1_RGBA_SRGB_BLOCK` in vulkan.
        // BC2_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_BC2_UNORM_BLOCK` in vulkan.
        // BC2_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_BC2_SRGB_BLOCK` in vulkan.
        // BC3_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_BC3_UNORM_BLOCK` in vulkan.
        // BC3_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_BC3_SRGB_BLOCK` in vulkan.
        // BC4_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_BC4_UNORM_BLOCK` in vulkan.
        // BC4_SNORM_BLOCK, ///< corresponds to `VK_FORMAT_BC4_SNORM_BLOCK` in vulkan.
        // BC5_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_BC5_UNORM_BLOCK` in vulkan.
        // BC5_SNORM_BLOCK, ///< corresponds to `VK_FORMAT_BC5_SNORM_BLOCK` in vulkan.
        // BC6H_UFLOAT_BLOCK, ///< corresponds to `VK_FORMAT_BC6H_UFLOAT_BLOCK` in vulkan.
        // BC6H_SFLOAT_BLOCK, ///< corresponds to `VK_FORMAT_BC6H_SFLOAT_BLOCK` in vulkan.
        // BC7_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_BC7_UNORM_BLOCK` in vulkan.
        // BC7_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_BC7_SRGB_BLOCK` in vulkan.
        // ETC2_R8G8B8_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK` in vulkan.
        // ETC2_R8G8B8_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK` in vulkan.
        // ETC2_R8G8B8A1_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK` in vulkan.
        // ETC2_R8G8B8A1_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK` in vulkan.
        // ETC2_R8G8B8A8_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK` in vulkan.
        // ETC2_R8G8B8A8_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK` in vulkan.
        // EAC_R11_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_EAC_R11_UNORM_BLOCK` in vulkan.
        // EAC_R11_SNORM_BLOCK, ///< corresponds to `VK_FORMAT_EAC_R11_SNORM_BLOCK` in vulkan.
        // EAC_R11G11_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_EAC_R11G11_UNORM_BLOCK` in vulkan.
        // EAC_R11G11_SNORM_BLOCK, ///< corresponds to `VK_FORMAT_EAC_R11G11_SNORM_BLOCK` in vulkan.
        // ASTC_4x4_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_4x4_UNORM_BLOCK` in vulkan.
        // ASTC_4x4_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_4x4_SRGB_BLOCK` in vulkan.
        // ASTC_5x4_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_5x4_UNORM_BLOCK` in vulkan.
        // ASTC_5x4_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_5x4_SRGB_BLOCK` in vulkan.
        // ASTC_5x5_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_5x5_UNORM_BLOCK` in vulkan.
        // ASTC_5x5_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_5x5_SRGB_BLOCK` in vulkan.
        // ASTC_6x5_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_6x5_UNORM_BLOCK` in vulkan.
        // ASTC_6x5_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_6x5_SRGB_BLOCK` in vulkan.
        // ASTC_6x6_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_6x6_UNORM_BLOCK` in vulkan.
        // ASTC_6x6_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_6x6_SRGB_BLOCK` in vulkan.
        // ASTC_8x5_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_8x5_UNORM_BLOCK` in vulkan.
        // ASTC_8x5_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_8x5_SRGB_BLOCK` in vulkan.
        // ASTC_8x6_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_8x6_UNORM_BLOCK` in vulkan.
        // ASTC_8x6_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_8x6_SRGB_BLOCK` in vulkan.
        // ASTC_8x8_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_8x8_UNORM_BLOCK` in vulkan.
        // ASTC_8x8_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_8x8_SRGB_BLOCK` in vulkan.
        // ASTC_10x5_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_10x5_UNORM_BLOCK` in vulkan.
        // ASTC_10x5_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_10x5_SRGB_BLOCK` in vulkan.
        // ASTC_10x6_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_10x6_UNORM_BLOCK` in vulkan.
        // ASTC_10x6_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_10x6_SRGB_BLOCK` in vulkan.
        // ASTC_10x8_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_10x8_UNORM_BLOCK` in vulkan.
        // ASTC_10x8_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_10x8_SRGB_BLOCK` in vulkan.
        // ASTC_10x10_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_10x10_UNORM_BLOCK` in vulkan.
        // ASTC_10x10_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_10x10_SRGB_BLOCK` in vulkan.
        // ASTC_12x10_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_12x10_UNORM_BLOCK` in vulkan.
        // ASTC_12x10_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_12x10_SRGB_BLOCK` in vulkan.
        // ASTC_12x12_UNORM_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_12x12_UNORM_BLOCK` in vulkan.
        // ASTC_12x12_SRGB_BLOCK, ///< corresponds to `VK_FORMAT_ASTC_12x12_SRGB_BLOCK` in vulkan.
    };

    // clang-format on

    /// Returns the size of a `Format` in bytes.
    POM_API size_t sizeofFormat(Format f);

    POM_API std::ostream& operator<<(std::ostream& os, const Format& format);

    /// Shuts down all instance realted things, all Contexts and resources should be destroyed before calling this.
    POM_API void shutdown();

    /// @}
} // namespace pom::gfx