#include <pomegranate/pomegranate.hpp>

#include "embed/basic_frag_spv.hpp"
#include "embed/basic_vert_spv.hpp"

#include <set>

// Theoretically this is completely wrong, queue family indices can be any valid u32 including u32max. Practically no
// device will have more than a few queue families, much less 2^32-1 of them.
const u32 INVALID_QUEUE_FAMILY_INDEX = UINT32_MAX;

const char* REQUIRED_DEVICE_EXTENSIONS[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
const size_t REQUIRED_DEVICE_EXTENSIONS_COUNT = 1;

const u32 MAX_FRAMES_IN_FLIGHT = 3;

struct Vertex {
    pom::maths::vec3 pos;
    pom::Color color;
};

static const Vertex VERTEX_DATA[] = { { { 0.0f, -0.5f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
                                      { { 0.5f, 0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
                                      { { -0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } } };

struct GameState {
    bool paused = false;
    // instance
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    u32 graphicsQueueFamily;
    VkDevice device;
    VkQueue graphicsQueue;
    // context
    // std::vector<VkSemaphore> imageAvailableSemaphores;
    // std::vector<VkSemaphore> renderFinishedSemaphores;
    // std::vector<VkFence> inFlightFences;
    // std::vector<VkFence> imagesInFlight;
    // u32 frameIndex = 0;
    // context -> swapchain
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    std::vector<VkFramebuffer> swapchainFramebuffers;
    VkViewport swapchainViewport;
    // u32 imageIndex = 0;
    // command buffer
    // VkCommandPool commandPool;
    // std::vector<VkCommandBuffer> commandBuffers;
    // std::vector<VkFence> commandBufferRecordFences;
    pom::gfx::CommandBuffer* commandBuffer;
    // pipeline
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    // vertex buffer
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
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

u32 findMemoryType(VkPhysicalDevice physicalDevice, u32 filter, VkMemoryPropertyFlags props)
{
    VkPhysicalDeviceMemoryProperties physicalMemoryProps;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalMemoryProps);

    for (u32 i = 0; i < physicalMemoryProps.memoryTypeCount; i++) {
        if (filter & (1 << i) && (physicalMemoryProps.memoryTypes[i].propertyFlags & props) == props) {
            return i;
        }
    }

    POM_FATAL("failed to find suitable memory type.");
    return -1;
}

VkShaderModule createShaderModule(VkDevice device, size_t size, const unsigned char* bytes)
{
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = size,
        // FIXME: this won't be safe for general use
        .pCode = reinterpret_cast<const u32*>(bytes) // NOLINT this code will always be aligned to 4 bytes so it is safe
    };

    VkShaderModule shaderModule = VK_NULL_HANDLE;
    POM_ASSERT(vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule) == VK_SUCCESS,
               "Failed to create shader module");

    return shaderModule;
}

POM_CLIENT_EXPORT void clientBegin(GameState* gamestate)
{
    // super hacky test stuff
    auto* instanceVk = dynamic_cast<pom::gfx::InstanceVk*>(pom::Application::get()->getGraphicsInstance());
    auto* contextVk = dynamic_cast<pom::gfx::ContextVk*>(pom::Application::get()->getMainWindow().getContext());

    gamestate->physicalDevice = instanceVk->physicalDevice;
    gamestate->graphicsQueueFamily = instanceVk->graphicsQueueFamilyIndex;
    gamestate->device = instanceVk->device;
    gamestate->graphicsQueue = instanceVk->graphicsQueue;

    gamestate->swapchain = contextVk->swapchain;
    gamestate->swapchainImageFormat = contextVk->swapchainImageFormat;
    gamestate->swapchainExtent = contextVk->swapchainExtent;
    gamestate->swapchainImages = contextVk->swapchainImages;
    gamestate->swapchainImageViews = contextVk->swapchainImageViews;
    gamestate->swapchainFramebuffers = contextVk->swapchainFramebuffers;
    gamestate->swapchainViewport = contextVk->swapchainViewport;
    // end super hacky test stuff

    // pipeline
    // TODO: as much of this as possible should load from the shaders themselves.
    VkShaderModule vertShaderModule = createShaderModule(gamestate->device, basic_vert_spv_size, basic_vert_spv_data);
    VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr,
    };

    VkShaderModule fragShaderModule = createShaderModule(gamestate->device, basic_frag_spv_size, basic_frag_spv_data);
    VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr,
    };

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageCreateInfo, fragShaderStageCreateInfo };

    VkVertexInputBindingDescription vertexBindingDesc = {
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
    };

    VkVertexInputAttributeDescription vertexAttribDescs[] = { {
                                                                  // position
                                                                  .location = 0,
                                                                  .binding = 0,
                                                                  .format = VK_FORMAT_R32G32B32_SFLOAT,
                                                                  .offset = offsetof(Vertex, pos),
                                                              },
                                                              {
                                                                  // color
                                                                  .location = 1,
                                                                  .binding = 0,
                                                                  .format = VK_FORMAT_R32G32B32A32_SFLOAT,
                                                                  .offset = offsetof(Vertex, color),
                                                              } };

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 1,
        .pVertexBindingDescriptions = &vertexBindingDesc,
        .vertexAttributeDescriptionCount = 2,
        .pVertexAttributeDescriptions = vertexAttribDescs,
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    VkPipelineViewportStateCreateInfo viewportCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = nullptr,
        .scissorCount = 1,
        .pScissors = nullptr,
    };

    VkPipelineRasterizationStateCreateInfo rasterizationCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.f,
        .depthBiasClamp = 0.f,
        .depthBiasSlopeFactor = 0.f,
        .lineWidth = 1.f,
    };

    VkPipelineMultisampleStateCreateInfo multisampleCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    // mix based on opacity
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask
        = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    };

    VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
        .blendConstants = { 0.f, 0.f, 0.f, 0.f },
    };

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    POM_ASSERT(vkCreatePipelineLayout(gamestate->device, &pipelineLayoutCreateInfo, nullptr, &gamestate->pipelineLayout)
                   == VK_SUCCESS,
               "failed to create pipeline");

    VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = 2,
        .pDynamicStates = dynamicStates,
    };

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputCreateInfo,
        .pInputAssemblyState = &inputAssemblyCreateInfo,
        .pTessellationState = nullptr,
        .pViewportState = &viewportCreateInfo,
        .pRasterizationState = &rasterizationCreateInfo,
        .pMultisampleState = &multisampleCreateInfo,
        .pDepthStencilState = nullptr,
        .pColorBlendState = &colorBlendCreateInfo,
        .pDynamicState = &dynamicStateCreateInfo,
        .layout = gamestate->pipelineLayout,
        .renderPass = dynamic_cast<const pom::gfx::RenderPassVk*>(contextVk->getSwapchainRenderPass())->getHandle(),
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    POM_ASSERT(vkCreateGraphicsPipelines(gamestate->device,
                                         VK_NULL_HANDLE,
                                         1,
                                         &pipelineCreateInfo,
                                         nullptr,
                                         &gamestate->graphicsPipeline)
                   == VK_SUCCESS,
               "Failed to create graphics pipeline");

    vkDestroyShaderModule(gamestate->device, fragShaderModule, nullptr);
    vkDestroyShaderModule(gamestate->device, vertShaderModule, nullptr);

    // command buffer
    gamestate->commandBuffer = contextVk->createCommandBuffer(pom::gfx::CommandBufferSpecialization::GRAPHICS);

    // VkCommandPoolCreateInfo commandPoolCreateInfo = {
    //     .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    //     .pNext = nullptr,
    //     .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    //     .queueFamilyIndex = gamestate->graphicsQueueFamily,
    // };

    // POM_ASSERT(vkCreateCommandPool(gamestate->device, &commandPoolCreateInfo, nullptr, &gamestate->commandPool)
    //                == VK_SUCCESS,
    //            "Failed to create command pool.");

    // gamestate->commandBuffers.resize(gamestate->swapchainFramebuffers.size());

    // VkCommandBufferAllocateInfo commandBufferAllocateCreateInfo = {
    //     .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    //     .pNext = nullptr,
    //     .commandPool = gamestate->commandPool,
    //     .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    //     .commandBufferCount = static_cast<uint32_t>(gamestate->commandBuffers.size()),
    // };

    // POM_ASSERT(
    //     vkAllocateCommandBuffers(gamestate->device, &commandBufferAllocateCreateInfo,
    //     gamestate->commandBuffers.data())
    //         == VK_SUCCESS,
    //     "Failed to allocate command buffers.");

    // // syncronization
    // gamestate->commandBufferRecordFences.resize(gamestate->commandBuffers.size());

    // VkFenceCreateInfo fenceCreateInfo = {
    //     .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    //     .pNext = nullptr,
    //     .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    // };

    // for (u32 i = 0; i < gamestate->commandBufferRecordFences.size(); i++) {
    //     POM_ASSERT(vkCreateFence(gamestate->device, &fenceCreateInfo, nullptr,
    //     &gamestate->commandBufferRecordFences[i])
    //                    == VK_SUCCESS,
    //                "Failed to create fence");
    // }

    // vertex buffer
    VkBufferCreateInfo vertexBufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = sizeof(VERTEX_DATA),
        .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
    };

    POM_ASSERT(vkCreateBuffer(gamestate->device, &vertexBufferCreateInfo, nullptr, &gamestate->vertexBuffer)
                   == VK_SUCCESS,
               "Failed to create vertex buffer");

    VkMemoryRequirements memoryReqs;
    vkGetBufferMemoryRequirements(gamestate->device, gamestate->vertexBuffer, &memoryReqs);

    VkMemoryAllocateInfo memoryAllocInfo = {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = memoryReqs.size,
        .memoryTypeIndex = findMemoryType(gamestate->physicalDevice,
                                          memoryReqs.memoryTypeBits,
                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
    };

    POM_ASSERT(vkAllocateMemory(gamestate->device, &memoryAllocInfo, nullptr, &gamestate->vertexBufferMemory)
                   == VK_SUCCESS,
               "Failed to allocate vertex buffer memory.")

    vkBindBufferMemory(gamestate->device, gamestate->vertexBuffer, gamestate->vertexBufferMemory, 0);

    void* data;
    vkMapMemory(gamestate->device, gamestate->vertexBufferMemory, 0, sizeof(VERTEX_DATA), 0, &data);
    memcpy(data, VERTEX_DATA, sizeof(VERTEX_DATA));
    vkUnmapMemory(gamestate->device, gamestate->vertexBufferMemory);
}

POM_CLIENT_EXPORT void clientMount(GameState* gamestate)
{
}

POM_CLIENT_EXPORT void clientUpdate(GameState* gamestate, pom::DeltaTime dt)
{
    if (gamestate->paused) {
        return;
    }
    auto* context = dynamic_cast<pom::gfx::ContextVk*>(pom::Application::get()->getMainWindow().getContext());

    // VkCommandBuffer commandBuffer = gamestate->commandBuffers[context->swapchainImageIndex];
    // VkFence recordFence = gamestate->commandBufferRecordFences[context->swapchainImageIndex];

    // vkWaitForFences(gamestate->device, 1, &recordFence, VK_TRUE, UINT64_MAX);
    // vkResetFences(gamestate->device, 1, &recordFence);

    // VkCommandBufferBeginInfo commandBufferBeginInfo = {
    //     .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    //     .pNext = nullptr,
    //     .flags = 0,
    //     .pInheritanceInfo = nullptr,
    // };

    // POM_ASSERT(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS,
    //            "Failed to begin recording command buffer.");

    // const auto f = [](f32 n) -> f32 {
    //     const f32 h = (f32)(pom::Application::get()->getFrame() % 255);
    //     const u32 s = 100;
    //     const u32 v = 100;
    //     const f32 k = std::fmod((n + h * 360 / (255 * 60)), 6);
    //     return v / 255.f - (v / 255.f) * (s / 255.f) * std::max(std::min(std::min(k, 4 - k), 1.f), 0.f);
    // };
    // VkClearValue clearColor = { f(5), f(3), f(1), 1.f };

    // VkRenderPassBeginInfo renderPassBeginInfo = {
    //     .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
    //     .pNext = nullptr,
    //     .renderPass = dynamic_cast<const
    //     pom::gfx::RenderPassVk*>(context->getSwapchainRenderPass())->getHandle(), .framebuffer =
    //     context->swapchainFramebuffers[context->swapchainImageIndex], .renderArea = { .offset = { 0, 0 },
    //     .extent = context->swapchainExtent }, .clearValueCount = 1, .pClearValues = &clearColor,
    // };

    // vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    gamestate->commandBuffer->begin();
    gamestate->commandBuffer->beginRenderPass(context->getSwapchainRenderPass());

    auto* commandBuffer = dynamic_cast<pom::gfx::CommandBufferVk*>(gamestate->commandBuffer)->getCurrentCommandBuffer();

    // calling this every frame doesn't really matter to my knowledge and is way easier than any other
    // alternatives
    vkCmdSetViewport(commandBuffer, 0, 1, &context->swapchainViewport);
    VkRect2D scissor { .offset = { 0, 0 }, .extent = context->swapchainExtent };
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    size_t offset = 0;
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &gamestate->vertexBuffer, &offset);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gamestate->graphicsPipeline);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    gamestate->commandBuffer->endRenderPass();
    gamestate->commandBuffer->end();

    context->submitCommandBuffer(gamestate->commandBuffer);

    // vkCmdEndRenderPass(commandBuffer);

    // POM_ASSERT(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS, "Failed to end recording command buffer.");

    // VkSubmitInfo submitInfo = {
    //     .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
    //     .pNext = nullptr,
    //     .waitSemaphoreCount = 0,
    //     .pWaitSemaphores = nullptr,
    //     .pWaitDstStageMask = nullptr,
    //     .commandBufferCount = 1,
    //     .pCommandBuffers = &commandBuffer,
    //     .signalSemaphoreCount = 0,
    //     .pSignalSemaphores = nullptr,
    // };

    // POM_ASSERT(vkQueueSubmit(context->instance->graphicsQueue, 1, &submitInfo, recordFence) == VK_SUCCESS,
    //            "Failed to submit to queue");

    pom::Application::get()->getMainWindow().getContext()->present();

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
    delete gamestate->commandBuffer;

    vkDestroyBuffer(gamestate->device, gamestate->vertexBuffer, nullptr);
    vkFreeMemory(gamestate->device, gamestate->vertexBufferMemory, nullptr);

    // for (size_t i = 0; i < gamestate->commandBufferRecordFences.size(); i++) {
    //     vkDestroyFence(gamestate->device, gamestate->commandBufferRecordFences[i], nullptr);
    // }

    // vkDestroyCommandPool(gamestate->device, gamestate->commandPool, nullptr);

    vkDestroyPipeline(gamestate->device, gamestate->graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(gamestate->device, gamestate->pipelineLayout, nullptr);

    delete gamestate;
}
