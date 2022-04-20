#include "pch.hpp"

#include "textureVk.hpp"

#include "bufferVk.hpp"
#include "commandBufferVk.hpp"
#include "gfxVk.hpp"
#include "instanceVk.hpp"

namespace pom::gfx {
    TextureVk::TextureVk(InstanceVk* instance,
                         TextureCreateInfo createInfo,
                         u32 width,
                         u32 height,
                         u32 depth,
                         const void* initialData,
                         usize initialDataOffset,
                         usize initialDataSize) :
        Texture(createInfo, width, height, depth),
        instance(instance), imageLayout(VK_IMAGE_LAYOUT_UNDEFINED)
    {
        POM_PROFILE_FUNCTION();
        if (initialData) {
            createInfo.usage |= TextureUsage::TRANSFER_DST;
        }

        if (createInfo.mipLevels) {
            createInfo.usage |= TextureUsage::TRANSFER_SRC | TextureUsage::TRANSFER_DST;
        }

        VkImageCreateInfo imageCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = static_cast<VkImageCreateFlags>(createInfo.cubeCompatable ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT
                                                                               : 0), // TODO
            .imageType = toVkImageType(createInfo.type),
            .format = toVkFormat(createInfo.format),
            .extent = { width, height, depth },
            .mipLevels = createInfo.mipLevels,
            .arrayLayers = createInfo.arrayLayers,
            .samples = VK_SAMPLE_COUNT_1_BIT, // TODO
            .tiling = VK_IMAGE_TILING_OPTIMAL, // TODO: don't do this on CPU visible memory
            .usage = toVkImageUsageFlags(createInfo.usage),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE, // FIXME: probably an error here
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

        POM_CHECK_VK(vkCreateImage(instance->getVkDevice(), &imageCreateInfo, nullptr, &image),
                     "failed to create image.");

        VkMemoryRequirements memoryReqs;
        vkGetImageMemoryRequirements(instance->getVkDevice(), image, &memoryReqs);
        VkPhysicalDeviceMemoryProperties physicalMemoryProps;
        vkGetPhysicalDeviceMemoryProperties(instance->getVkPhysicalDevice(), &physicalMemoryProps);

        // FIXME: props should be gpu only
        VkMemoryPropertyFlags props = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        u32 memoryTypeIndex = VK_MAX_MEMORY_TYPES;

        for (u32 i = 0; i < physicalMemoryProps.memoryTypeCount; i++) {
            if (memoryReqs.memoryTypeBits & (1 << i)
                && (physicalMemoryProps.memoryTypes[i].propertyFlags & props) == props) {
                memoryTypeIndex = i;
                break;
            }
        }

        POM_ASSERT(memoryTypeIndex != VK_MAX_MEMORY_TYPES, "failed to find suitable memory type.");
        memorySize = memoryReqs.size;

        VkMemoryAllocateInfo imageAllocInfo = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .pNext = nullptr,
            .allocationSize = memoryReqs.size,
            .memoryTypeIndex = memoryTypeIndex,
        };

        POM_CHECK_VK(vkAllocateMemory(instance->getVkDevice(), &imageAllocInfo, nullptr, &memory),
                     "failed to allocate texture memory");

        POM_CHECK_VK(vkBindImageMemory(instance->getVkDevice(), image, memory, 0), "Failed to bind image memory");

        if (initialData) {
            auto* transferBuffer = new BufferVk(instance,
                                                BufferUsage::TRANSFER_SRC,
                                                BufferMemoryAccess::CPU_WRITE,
                                                getSize(),
                                                initialData,
                                                initialDataOffset,
                                                initialDataSize);
            auto* commandBuffer = new CommandBufferVk(instance, CommandBufferSpecialization::GENERAL, 1);
            commandBuffer->begin();
            commandBuffer->copyBufferToTexture(transferBuffer,
                                               this,
                                               initialDataSize,
                                               initialDataOffset,
                                               { 0, 0, 0 },
                                               getExtent());
            commandBuffer->end();
            commandBuffer->submit();

            delete commandBuffer;
            delete transferBuffer;
        } else {
            auto* commandBuffer = new CommandBufferVk(instance, CommandBufferSpecialization::GENERAL, 1);
            commandBuffer->begin();
            commandBuffer->transitionImageLayoutVk(this,
                                                   VK_IMAGE_LAYOUT_UNDEFINED,
                                                   VK_IMAGE_LAYOUT_GENERAL,
                                                   createInfo.mipLevels);
            commandBuffer->end();
            commandBuffer->submit();

            delete commandBuffer;
        }
    }

    TextureVk::~TextureVk()
    {
        POM_PROFILE_FUNCTION();
        vkDestroyImage(instance->getVkDevice(), image, nullptr);
        vkFreeMemory(instance->getVkDevice(), memory, nullptr);
    }

    TextureViewVk::TextureViewVk(const Ref<TextureVk>& texture, TextureViewCreateInfo createInfo) :
        TextureView(createInfo), texture(texture)
    {
        VkImageViewCreateInfo imageViewCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = texture->getVkImage(),
            .viewType = toVkImageViewType(createInfo.type),
            .format = toVkFormat(createInfo.format),
            .components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = createInfo.subresourceRange.baseMip,
                .levelCount = createInfo.subresourceRange.mipCount,
                .baseArrayLayer = createInfo.subresourceRange.baseArrayLayer,
                .layerCount = createInfo.subresourceRange.layerCount,
            },
        };

        POM_CHECK_VK(vkCreateImageView(texture->instance->getVkDevice(), &imageViewCreateInfo, nullptr, &view),
                     "failed to create image view");

        VkSamplerCreateInfo samplerCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .magFilter = VK_FILTER_LINEAR,
            .minFilter = VK_FILTER_LINEAR,
            .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            .mipLodBias = 0.f,
            .anisotropyEnable = VK_FALSE, // FIXME: request this from physical device features
            .maxAnisotropy = 1.f, // FIXME: should be acquired from physical device caps
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_NEVER,
            .minLod = static_cast<f32>(createInfo.subresourceRange.baseMip),
            .maxLod = static_cast<f32>(createInfo.subresourceRange.baseMip + createInfo.subresourceRange.mipCount),
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
            .unnormalizedCoordinates = VK_FALSE,
        };

        POM_CHECK_VK(vkCreateSampler(texture->instance->getVkDevice(), &samplerCreateInfo, nullptr, &sampler),
                     "failed to create sampler.");
    }

    TextureViewVk::~TextureViewVk()
    {
        vkDestroySampler(texture->instance->getVkDevice(), sampler, nullptr);
        vkDestroyImageView(texture->instance->getVkDevice(), view, nullptr);
    }

} // namespace pom::gfx