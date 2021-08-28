#include <pomegranate/pomegranate.hpp>

#include <array>
#include <numeric>

#include "embed/shader/basic_frag_spv.hpp"
#include "embed/shader/basic_vert_spv.hpp"

#include "embed/img/empty_png.hpp"

#include <spirv_reflect.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Vertex {
    pom::maths::vec3 pos;
    pom::Color color;
    pom::maths::vec2 uv;
};

struct UniformMVP {
    pom::maths::mat4 model;
    pom::maths::mat4 view;
    pom::maths::mat4 projection;
};

static Vertex VERTEX_DATA[] = {
    { { -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.f, 0.f } },
    { { 0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f }, { 1.f, 0.f } },
    { { 0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f, 1.0f, 1.0f }, { 1.f, 1.f } },
    { { -0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.f, 1.f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f, 1.0f }, { 0.f, 0.f } },
    { { 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.f, 0.f } },
    { { 0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 1.f, 1.f } },
    { { -0.5f, 0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f }, { 0.f, 1.f } },
};

static const f32 SCALE_DATA[] = { 2, 1.5, 1, 0.5 };

static const u16 INDEX_DATA[]
    = { 0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 7, 6, 5, 5, 4, 7, 4, 0, 3, 3, 7, 4, 4, 5, 1, 1, 0, 4, 3, 2, 6, 6, 7, 3 };

struct GameState {
    // instance
    VkInstance instance;
    VkDevice device;
    pom::gfx::CommandBuffer* commandBuffer;
    // vertex buffer
    pom::gfx::Buffer* vertexBuffers[POM_MAX_FRAMES_IN_FLIGHT];
    pom::gfx::Buffer* scaleBuffer;
    pom::gfx::Buffer* indexBuffer;

    // uniform buffers
    pom::maths::vec3 cameraPos = pom::maths::vec3(2, 2, 2);
    pom::gfx::Buffer* uniformBuffers[POM_MAX_FRAMES_IN_FLIGHT];

    // pipeline
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    VkDescriptorPool descriptorPool;
    std::unordered_map<u32, VkDescriptorSet[POM_MAX_FRAMES_IN_FLIGHT]> descriptorSets;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    pom::gfx::Pipeline* pipeline;

    // other context test.
    pom::Window* otherWindow;
    pom::gfx::CommandBuffer* otherCommandBuffer;
    pom::gfx::Buffer* otherVertexBuffer;

    // texture
    pom::gfx::Texture* texture;
    // VkImage textureImage;
    // VkDeviceMemory textureMemory;
    // VkImageView textureView;
    // VkSampler textureSampler;
};

POM_CLIENT_EXPORT const pom::AppCreateInfo* clientGetAppCreateInfo(int /*argc*/, char** /*argv*/)
{
    static const pom::AppCreateInfo aci = {
        .name = "Pomegranate Sandbox Application",
    };

    return &aci;
}

POM_CLIENT_EXPORT GameState* clientCreateState()
{
    auto* gc = new GameState;

    return gc;
}

POM_CLIENT_EXPORT void clientUpdate(GameState* gamestate, pom::DeltaTime dt);

void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    // FIXME: should prob be transfer...somehow
    auto* commandBuffer = pom::gfx::CommandBuffer::create(pom::gfx::CommandBufferSpecialization::GRAPHICS);
    commandBuffer->begin();
    VkCommandBuffer cmd = dynamic_cast<pom::gfx::CommandBufferVk*>(commandBuffer)->getCurrentCommandBuffer();

    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask = 0,
        .dstAccessMask = 0,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_NONE_KHR;
    VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_NONE_KHR;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
               && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        srcStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        POM_FATAL("oof");
    }

    vkCmdPipelineBarrier(cmd, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    commandBuffer->end();
    commandBuffer->submit();
}

POM_CLIENT_EXPORT void clientBegin(GameState* gamestate)
{
    // super hacky test stuff
    auto* instanceVk = dynamic_cast<pom::gfx::InstanceVk*>(pom::Application::get()->getGraphicsInstance());
    auto* contextVk = dynamic_cast<pom::gfx::ContextVk*>(pom::Application::get()->getMainWindow().getContext());

    gamestate->device = instanceVk->device;
    // end super hacky test stuff

    // gamestate->otherWindow = new pom::Window("other window", pom::gfx::GraphicsAPI::VULKAN, true);
    // gamestate->otherWindow->setEventHandler([gamestate](pom::InputEvent ev) {
    //     if (ev.type == pom::InputEventType::WINDOW_RESIZE) {
    //         clientUpdate(gamestate, {});
    //     }
    // });

    gamestate->otherCommandBuffer = pom::gfx::CommandBuffer::create(pom::gfx::CommandBufferSpecialization::GRAPHICS);
    gamestate->otherVertexBuffer = pom::gfx::Buffer::create(pom::gfx::BufferUsage::VERTEX,
                                                            pom::gfx::BufferMemoryAccess::GPU_ONLY,
                                                            sizeof(VERTEX_DATA),
                                                            VERTEX_DATA);

    // texture
    i32 width, height, channels;
    const unsigned char* pixels = stbi_load_from_memory(empty_png_data,
                                                        static_cast<int>(empty_png_size),
                                                        &width,
                                                        &height,
                                                        &channels,
                                                        STBI_rgb_alpha);
    POM_ASSERT(pixels, "oof");

    size_t textureSize = width * height * 4;

    gamestate->texture = pom::gfx::Texture::create(
        {
            .type = pom::gfx::TextureType::IMAGE_2D,
            .usage = pom::gfx::TextureUsage::SAMPLED | pom::gfx::TextureUsage::TRANSFER_DST,
            .textureFormat = pom::gfx::Format::R8G8B8A8_SRGB,
            .viewFormat = pom::gfx::Format::R8G8B8A8_SRGB,
        },
        width,
        height);
    auto* textureTransferBuffer = pom::gfx::Buffer::create(pom::gfx::BufferUsage::TRANSFER_SRC,
                                                           pom::gfx::BufferMemoryAccess::CPU_WRITE,
                                                           textureSize);
    void* data = textureTransferBuffer->map();
    memcpy(data, pixels, textureSize);
    textureTransferBuffer->unmap();

    {
        auto* transferCmdBuffer = pom::gfx::CommandBuffer::create(pom::gfx::CommandBufferSpecialization::GENERAL);
        transferCmdBuffer->begin();
        const u32 col = 4;
        transferCmdBuffer->copyBufferToTexture(textureTransferBuffer,
                                               gamestate->texture,
                                               textureSize - (col * width * 4),
                                               0,
                                               { 0, col, 0 },
                                               gamestate->texture->getExtent() - pom::maths::ivec3 { 0, col, 0 });
        transferCmdBuffer->end();
        transferCmdBuffer->submit();
        delete transferCmdBuffer;
    }

    POM_DEBUG(dynamic_cast<pom::gfx::TextureVk*>(gamestate->texture)->getVkImageLayout());

    // VkImageCreateInfo imageCreateInfo = {
    //     .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    //     .pNext = nullptr,
    //     .flags = 0,
    //     .imageType = VK_IMAGE_TYPE_2D,
    //     .format = VK_FORMAT_R8G8B8A8_SRGB,
    //     .extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1u },
    //     .mipLevels = 1,
    //     .arrayLayers = 1,
    //     .samples = VK_SAMPLE_COUNT_1_BIT,
    //     .tiling = VK_IMAGE_TILING_OPTIMAL,
    //     .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
    //     .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    //     .queueFamilyIndexCount = 0,
    //     .pQueueFamilyIndices = nullptr,
    //     .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    // };

    // POM_CHECK_VK(vkCreateImage(gamestate->device, &imageCreateInfo, nullptr, &gamestate->textureImage),
    //              "failed to create image.");

    // VkMemoryRequirements memoryReqs;
    // vkGetImageMemoryRequirements(gamestate->device, gamestate->textureImage, &memoryReqs);
    // VkPhysicalDeviceMemoryProperties physicalMemoryProps;
    // vkGetPhysicalDeviceMemoryProperties(instanceVk->physicalDevice, &physicalMemoryProps);

    // u32 memoryTypeIndex = VK_MAX_MEMORY_TYPES;

    // for (u32 i = 0; i < physicalMemoryProps.memoryTypeCount; i++) {
    //     if (memoryReqs.memoryTypeBits & (1 << i)
    //         && (physicalMemoryProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    //                == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
    //         memoryTypeIndex = i;
    //         break;
    //     }
    // }

    // POM_ASSERT(memoryTypeIndex != VK_MAX_MEMORY_TYPES, "failed to find suitable memory type.");

    // VkMemoryAllocateInfo imageAllocInfo = {
    //     .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
    //     .pNext = nullptr,
    //     .allocationSize = memoryReqs.size,
    //     .memoryTypeIndex = memoryTypeIndex,
    // };

    // POM_CHECK_VK(vkAllocateMemory(gamestate->device, &imageAllocInfo, nullptr, &gamestate->textureMemory),
    //              "failed to allocate memory");

    // vkBindImageMemory(gamestate->device, gamestate->textureImage, gamestate->textureMemory, 0);

    // auto* transferCmdBuffer = pom::gfx::CommandBuffer::create(pom::gfx::CommandBufferSpecialization::TRANSFER);
    // transferCmdBuffer->begin();
    // VkCommandBuffer cmd = dynamic_cast<pom::gfx::CommandBufferVk*>(transferCmdBuffer)->getCurrentCommandBuffer();

    // transitionImageLayout(gamestate->textureImage,
    //                       VK_FORMAT_R8G8B8A8_SRGB,
    //                       VK_IMAGE_LAYOUT_UNDEFINED,
    //                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // VkBufferImageCopy region = {
    //     .bufferOffset = 0,
    //     .bufferRowLength = 0,
    //     .bufferImageHeight = 0,
    //     .imageSubresource = {
    //         .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    //         .mipLevel = 0,
    //         .baseArrayLayer = 0,
    //         .layerCount = 1,
    //     },
    //     .imageOffset = { 0, 0, 0 },
    //     .imageExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 },
    // };

    // vkCmdCopyBufferToImage(cmd,
    //                        dynamic_cast<pom::gfx::BufferVk*>(textureTransferBuffer)->getBuffer(),
    //                        gamestate->textureImage,
    //                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    //                        1,
    //                        &region);

    // transferCmdBuffer->end();
    // transferCmdBuffer->submit();

    // transitionImageLayout(gamestate->textureImage,
    //                       VK_FORMAT_R8G8B8A8_SRGB,
    //                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    //                       VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // VkImageViewCreateInfo imageViewCreateInfo = {
    //     .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    //     .pNext = nullptr,
    //     .flags = 0,
    //     .image = gamestate->textureImage,
    //     .viewType = VK_IMAGE_VIEW_TYPE_2D,
    //     .format = VK_FORMAT_R8G8B8A8_SRGB,
    //     .components = VK_COMPONENT_SWIZZLE_IDENTITY,
    //     .subresourceRange = {
    //         .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
    //         .baseMipLevel = 0,
    //         .levelCount = 1,
    //         .baseArrayLayer = 0,
    //         .layerCount = 1,
    //     },
    // };

    // POM_CHECK_VK(vkCreateImageView(gamestate->device, &imageViewCreateInfo, nullptr, &gamestate->textureView),
    //              "failed to create image view");

    // VkSamplerCreateInfo samplerCreateInfo = {
    //     .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
    //     .pNext = nullptr,
    //     .flags = 0,
    //     .magFilter = VK_FILTER_NEAREST,
    //     .minFilter = VK_FILTER_NEAREST,
    //     .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
    //     .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    //     .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    //     .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
    //     .mipLodBias = 0.f,
    //     .anisotropyEnable = VK_FALSE, // FIXME: request this from physical device features
    //     .maxAnisotropy = 1.f, // FIXME: should be acquired from physical device caps
    //     .compareEnable = VK_FALSE,
    //     .compareOp = VK_COMPARE_OP_ALWAYS,
    //     .minLod = 0.f,
    //     .maxLod = 0.f,
    //     .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
    //     .unnormalizedCoordinates = VK_FALSE,
    // };

    // POM_CHECK_VK(vkCreateSampler(gamestate->device, &samplerCreateInfo, nullptr, &gamestate->textureSampler),
    //              "failed to create sampler.");

    // pipeline
    spirv_cross::CompilerReflection vertShaderModuleReflection(reinterpret_cast<const u32*>(basic_vert_spv_data),
                                                               basic_vert_spv_size / sizeof(u32));

    spirv_cross::ShaderResources vertShaderResources = vertShaderModuleReflection.get_shader_resources();

    auto inputs = vertShaderResources.stage_inputs;

    for (auto& input : inputs) {
        u32 set = vertShaderModuleReflection.get_decoration(input.id, spv::DecorationDescriptorSet);
        u32 location = vertShaderModuleReflection.get_decoration(input.id, spv::DecorationLocation);
        u32 offset = vertShaderModuleReflection.get_decoration(input.id, spv::DecorationOffset);

        POM_DEBUG("vertex attribute: ", input.name, ", ", set, ", ", location, ", ", offset);
    }

    // descriptor set
    for (auto& ub : gamestate->uniformBuffers) {
        ub = pom::gfx::Buffer::create(pom::gfx::BufferUsage::UNIFORM,
                                      pom::gfx::BufferMemoryAccess::CPU_WRITE,
                                      sizeof(UniformMVP));
    }

    VkDescriptorPoolSize poolSizes[] = {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = POM_MAX_FRAMES_IN_FLIGHT * 3,
        },
        {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = POM_MAX_FRAMES_IN_FLIGHT,
        },
    };

    VkDescriptorPoolCreateInfo poolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .maxSets = POM_MAX_FRAMES_IN_FLIGHT,
        .poolSizeCount = 2,
        .pPoolSizes = poolSizes,
    };

    vkCreateDescriptorPool(gamestate->device, &poolCreateInfo, nullptr, &gamestate->descriptorPool);

    auto uniformBuffers = vertShaderResources.uniform_buffers;

    std::unordered_map<u32, std::vector<VkDescriptorSetLayoutBinding>> descriptorSetBindings;

    for (auto& resource : uniformBuffers) {
        u32 set = vertShaderModuleReflection.get_decoration(resource.id, spv::Decoration::DecorationDescriptorSet);
        if (!descriptorSetBindings.contains(set)) {
            descriptorSetBindings.emplace(std::make_pair(set, std::vector<VkDescriptorSetLayoutBinding>()));
        }

        const spirv_cross::SPIRType& type = vertShaderModuleReflection.get_type(resource.type_id);
        size_t size = vertShaderModuleReflection.get_declared_struct_size(type);
        POM_DEBUG(resource.name, ": ", std::accumulate(type.array.begin(), type.array.end(), 1u, std::multiplies<>()));

        descriptorSetBindings[set].push_back({
            .binding = vertShaderModuleReflection.get_decoration(resource.id, spv::Decoration::DecorationBinding),
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            // NOTE: Vulkan doesn't support multi dimensional arrays for uniform buffers.
            .descriptorCount = type.array.size() == 1 ? type.array[0] : 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT, // this will need to be changed, maybe just VK_SHADER_STAGE_ALL?
            .pImmutableSamplers = nullptr,
        });
    }

    descriptorSetBindings[0].push_back({
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .pImmutableSamplers = nullptr,
    });

    gamestate->descriptorSetLayouts.resize(descriptorSetBindings.size());
    std::vector<VkDescriptorSetLayoutCreateInfo> descSetLayoutCreateInfos(descriptorSetBindings.size());
    u32 i = 0;
    for (auto& descSetBinding : descriptorSetBindings) {
        descSetLayoutCreateInfos[i] = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .bindingCount = static_cast<u32>(descSetBinding.second.size()),
            .pBindings = descSetBinding.second.data(),
        };

        POM_CHECK_VK(vkCreateDescriptorSetLayout(gamestate->device,
                                                 &descSetLayoutCreateInfos[i],
                                                 nullptr,
                                                 gamestate->descriptorSetLayouts.data()),
                     "Failed to create descriptor set layout.");

        VkDescriptorSetLayout descriptorSetLayouts[POM_MAX_FRAMES_IN_FLIGHT]
            = { gamestate->descriptorSetLayouts[i], gamestate->descriptorSetLayouts[i] };

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool = gamestate->descriptorPool,
            .descriptorSetCount = POM_MAX_FRAMES_IN_FLIGHT,
            .pSetLayouts = descriptorSetLayouts,
        };

        POM_CHECK_VK(
            vkAllocateDescriptorSets(gamestate->device, &descriptorSetAllocateInfo, gamestate->descriptorSets[i]),
            "Failed to allocate descriptor sets");

        i++;
    }

    // for array
    // for (u8 i = 0; i < POM_MAX_FRAMES_IN_FLIGHT; i++) {
    //     VkDescriptorBufferInfo bufferInfos[3];
    //     for (u32 j = 0; j < 3; j++) {
    //         bufferInfos[j] = {
    //             .buffer = dynamic_cast<pom::gfx::BufferVk*>(gamestate->uniformBuffers[i])->getBuffer(),
    //             .offset = j * sizeof(pom::maths::mat4),
    //             .range = sizeof(pom::maths::mat4),
    //         };
    //     }

    //     VkWriteDescriptorSet descriptorSetWrite = {
    //         .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    //         .pNext = nullptr,
    //         .dstSet = gamestate->descriptorSets[0][i],
    //         .dstBinding = 0,
    //         .dstArrayElement = 0,
    //         .descriptorCount = 3,
    //         .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    //         .pImageInfo = nullptr,
    //         .pBufferInfo = bufferInfos,
    //         .pTexelBufferView = nullptr,
    //     };

    //     vkUpdateDescriptorSets(gamestate->device, 1, &descriptorSetWrite, 0, nullptr);
    // }

    // for struct
    for (u8 i = 0; i < POM_MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo = {
            .buffer = dynamic_cast<pom::gfx::BufferVk*>(gamestate->uniformBuffers[i])->getBuffer(),
            .offset = 0,
            .range = sizeof(UniformMVP),
        };

        VkDescriptorImageInfo imageInfo = {
            .sampler = dynamic_cast<pom::gfx::TextureVk*>(gamestate->texture)->getVkSampler(),
            .imageView = dynamic_cast<pom::gfx::TextureVk*>(gamestate->texture)->getVkImageView(),
            // .sampler = gamestate->textureSampler,
            // .imageView = gamestate->textureView,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        VkWriteDescriptorSet descriptorSetWrites[2] = {
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = gamestate->descriptorSets[0][i],
                .dstBinding = 0,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .pImageInfo = nullptr,
                .pBufferInfo = &bufferInfo,
                .pTexelBufferView = nullptr,
            },
            {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = gamestate->descriptorSets[0][i],
                .dstBinding = 1,
                .dstArrayElement = 0,
                .descriptorCount = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .pImageInfo = &imageInfo,
                .pBufferInfo = nullptr,
                .pTexelBufferView = nullptr,
            },
        };

        vkUpdateDescriptorSets(gamestate->device, 2, descriptorSetWrites, 0, nullptr);
    }

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = static_cast<u32>(gamestate->descriptorSetLayouts.size()),
        .pSetLayouts = gamestate->descriptorSetLayouts.data(),
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    POM_CHECK_VK(
        vkCreatePipelineLayout(gamestate->device, &pipelineLayoutCreateInfo, nullptr, &gamestate->pipelineLayout),
        "failed to create pipeline");

    pom::gfx::ShaderModule* vertShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStage::VERTEX,
                                         basic_vert_spv_size,
                                         reinterpret_cast<const u32*>(basic_vert_spv_data));

    pom::gfx::ShaderModule* fragShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStage::FRAGMENT,
                                         basic_frag_spv_size,
                                         reinterpret_cast<const u32*>(basic_frag_spv_data));

    pom::gfx::Shader* shader = pom::gfx::Shader::create(true, { vertShader, fragShader });

    gamestate->pipeline = pom::gfx::Pipeline::create(
        {},
        shader,
        contextVk->getSwapchainRenderPass(),
        { {
              .binding = 0,
              .attribs = { { .location = 0, .format = pom::gfx::Format::R32G32B32_SFLOAT },
                           { .location = 1, .format = pom::gfx::Format::R32G32B32A32_SFLOAT },
                           { .location = 2, .format = pom::gfx::Format::R32G32_SFLOAT } },
          },
          {
              .binding = 1,
              .attribs = { { .location = 3, .format = pom::gfx::Format::R32_SFLOAT } },
          } },
        gamestate->pipelineLayout);

    // command buffer
    gamestate->commandBuffer = pom::gfx::CommandBuffer::create(pom::gfx::CommandBufferSpecialization::GRAPHICS);

    // vertex buffer
    for (auto& vertexBuffer : gamestate->vertexBuffers) {
        vertexBuffer = pom::gfx::Buffer::create(pom::gfx::BufferUsage::VERTEX,
                                                pom::gfx::BufferMemoryAccess::CPU_WRITE,
                                                sizeof(VERTEX_DATA));
    }

    gamestate->scaleBuffer = pom::gfx::Buffer::create(pom::gfx::BufferUsage::VERTEX,
                                                      pom::gfx::BufferMemoryAccess::GPU_ONLY,
                                                      sizeof(SCALE_DATA),
                                                      SCALE_DATA);

    // index buffer
    gamestate->indexBuffer = pom::gfx::Buffer::create(pom::gfx::BufferUsage::INDEX,
                                                      pom::gfx::BufferMemoryAccess::GPU_ONLY,
                                                      sizeof(INDEX_DATA),
                                                      INDEX_DATA);
}

POM_CLIENT_EXPORT void clientMount(GameState* gamestate)
{
}

POM_CLIENT_EXPORT void clientUpdate(GameState* gamestate, pom::DeltaTime dt)
{
    POM_PROFILE_SCOPE("update");
    {
        if (!pom::Application::get()->getMainWindow().isMinimized()) {
            auto* context = dynamic_cast<pom::gfx::ContextVk*>(pom::Application::get()->getMainWindow().getContext());

            auto frame = pom::Application::get()->getFrame();

            pom::gfx::Buffer* vertexBuffer = gamestate->vertexBuffers[frame % POM_MAX_FRAMES_IN_FLIGHT];
            {
                // Vertex* data = (Vertex*)vertexBuffer->map();
                // memcpy(data, VERTEX_DATA, sizeof(VERTEX_DATA));

                // pom::maths::mat3 m = pom::maths::mat3::rotate({ (f32)(TAU / 100.f * (f32)frame) });
                // for (u8 i = 0; i < 4; i++) {
                //     data[i].pos = m * VERTEX_DATA[i].pos;
                // }

                // vertexBuffer->unmap();
            }

            const f32 cameraSpeed = 0.01f;
            if (pom::keyDown(pom::KeyHid::KEY_W)) {
                gamestate->cameraPos.y += cameraSpeed * dt;
            } else if (pom::keyDown(pom::KeyHid::KEY_S)) {
                gamestate->cameraPos.y -= cameraSpeed * dt;
            }

            if (pom::keyDown(pom::KeyHid::KEY_D)) {
                gamestate->cameraPos.x += cameraSpeed * dt;
            } else if (pom::keyDown(pom::KeyHid::KEY_A)) {
                gamestate->cameraPos.x -= cameraSpeed * dt;
            }

            if (pom::keyDown(pom::KeyHid::KEY_Q)) {
                gamestate->cameraPos.z += cameraSpeed * dt;
            } else if (pom::keyDown(pom::KeyHid::KEY_E)) {
                gamestate->cameraPos.z -= cameraSpeed * dt;
            }

            {
                pom::gfx::Buffer* uniformBuffer = gamestate->uniformBuffers[frame % POM_MAX_FRAMES_IN_FLIGHT];
                UniformMVP* data = (UniformMVP*)uniformBuffer->map();

                data->model = pom::maths::mat4::rotate({ TAU / 100.f * 0, 0, 0 });
                data->projection = pom::maths::mat4::perspective(TAU / 8.f,
                                                                 context->swapchainViewport.width
                                                                     / context->swapchainViewport.height,
                                                                 0.01f,
                                                                 1000.f);
                data->view = pom::maths::mat4::lookAt(gamestate->cameraPos,
                                                      pom::maths::vec3(0.f, 0.f, 0.f),
                                                      pom::maths::vec3(0.f, 0.f, 1.f));

                uniformBuffer->unmap();
            }

            gamestate->commandBuffer->begin();
            gamestate->commandBuffer->beginRenderPass(context->getSwapchainRenderPass(), context);

            auto* commandBuffer
                = dynamic_cast<pom::gfx::CommandBufferVk*>(gamestate->commandBuffer)->getCurrentCommandBuffer();

            gamestate->commandBuffer->setViewport(context->getSwapchainViewport());
            gamestate->commandBuffer->setScissor({ 0, 0 },
                                                 { context->swapchainExtent.width, context->swapchainExtent.height });

            gamestate->commandBuffer->bindVertexBuffer(gamestate->otherVertexBuffer);
            gamestate->commandBuffer->bindVertexBuffer(gamestate->scaleBuffer, 1);

            gamestate->commandBuffer->bindIndexBuffer(gamestate->indexBuffer, pom::gfx::IndexType::U16);

            gamestate->commandBuffer->bindPipeline(gamestate->pipeline);
            vkCmdBindDescriptorSets(commandBuffer,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    gamestate->pipelineLayout,
                                    0,
                                    1,
                                    &gamestate->descriptorSets[0][frame % POM_MAX_FRAMES_IN_FLIGHT],
                                    0,
                                    nullptr);

            gamestate->commandBuffer->drawIndexed(gamestate->indexBuffer->getSize() / sizeof(u16));

            gamestate->commandBuffer->endRenderPass();
            gamestate->commandBuffer->end();

            gamestate->commandBuffer->submit();

            pom::Application::get()->getMainWindow().getContext()->present();
        }
    }

    // if (!gamestate->otherWindow->isMinimized()) {
    //     auto* ctx = dynamic_cast<pom::gfx::ContextVk*>(gamestate->otherWindow->getContext());
    //     gamestate->otherCommandBuffer->begin();
    //     gamestate->otherCommandBuffer->beginRenderPass(ctx->getSwapchainRenderPass(), ctx);

    //     gamestate->otherCommandBuffer->setViewport({ ctx->swapchainViewport.x, ctx->swapchainViewport.y },
    //                                                { ctx->swapchainViewport.width, ctx->swapchainViewport.height
    //                                                }, ctx->swapchainViewport.minDepth,
    //                                                ctx->swapchainViewport.maxDepth);
    //     gamestate->otherCommandBuffer->setScissor({ 0, 0 },
    //                                               { ctx->swapchainExtent.width, ctx->swapchainExtent.height });

    //     auto* commandBuffer
    //         = dynamic_cast<pom::gfx::CommandBufferVk*>(gamestate->otherCommandBuffer)->getCurrentCommandBuffer();

    //     gamestate->otherCommandBuffer->bindVertexBuffer(gamestate->otherVertexBuffer);
    //     gamestate->otherCommandBuffer->bindVertexBuffer(gamestate->scaleBuffer, 1);

    //     gamestate->otherCommandBuffer->bindIndexBuffer(gamestate->indexBuffer, pom::gfx::IndexType::U16);

    //     vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gamestate->graphicsPipeline);

    //     gamestate->otherCommandBuffer->drawIndexed(gamestate->indexBuffer->getSize() / sizeof(u16));

    //     gamestate->otherCommandBuffer->endRenderPass();
    //     gamestate->otherCommandBuffer->end();

    //     gamestate->otherCommandBuffer->submit();

    //     ctx->present();
    // }

    // POM_DEBUG("dt: ", dt, "ms");
}

POM_CLIENT_EXPORT void clientOnInputEvent(GameState* gamestate, pom::InputEvent* ev)
{
}

POM_CLIENT_EXPORT void clientUnmount(GameState* gamestate)
{
}

POM_CLIENT_EXPORT void clientEnd(GameState* gamestate)
{
    // vkDestroySampler(gamestate->device, gamestate->textureSampler, nullptr);
    // vkDestroyImageView(gamestate->device, gamestate->textureView, nullptr);
    // vkDestroyImage(gamestate->device, gamestate->textureImage, nullptr);
    // vkFreeMemory(gamestate->device, gamestate->textureMemory, nullptr);

    for (auto& vb : gamestate->vertexBuffers) {
        delete vb;
    }
    delete gamestate->scaleBuffer;
    delete gamestate->commandBuffer;
    delete gamestate->indexBuffer;

    for (auto& ub : gamestate->uniformBuffers) {
        delete ub;
    }

    vkDestroyDescriptorPool(gamestate->device, gamestate->descriptorPool, nullptr);
    for (auto& layout : gamestate->descriptorSetLayouts) {
        vkDestroyDescriptorSetLayout(gamestate->device, layout, nullptr);
    }

    delete gamestate->pipeline;
    vkDestroyPipelineLayout(gamestate->device, gamestate->pipelineLayout, nullptr);

    delete gamestate->otherCommandBuffer;
    delete gamestate->otherVertexBuffer;
    delete gamestate->otherWindow;

    delete gamestate;
}
