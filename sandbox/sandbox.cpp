#include <pomegranate/pomegranate.hpp>

#include "embed/basic_frag_spv.hpp"
#include "embed/basic_vert_spv.hpp"

#include <spirv_reflect.hpp>

struct Vertex {
    pom::maths::vec3 pos;
    pom::Color color;
};

static Vertex VERTEX_DATA[] = { { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
                                { { 0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f, 1.0f, 1.0f } },
                                { { 0.5f, 0.5f, 0.0f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
                                { { -0.5f, 0.5f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } } };

static const f32 SCALE_DATA[] = { 2, 1.5, 1, 0.5 };

static const u16 INDEX_DATA[] = { 0, 1, 2, 0, 2, 3 };

struct GameState {
    bool paused = false;
    // instance
    VkInstance instance;
    VkDevice device;
    pom::gfx::CommandBuffer* commandBuffer;
    // pipeline
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    // vertex buffer
    pom::gfx::Buffer* vertexBuffer;
    pom::gfx::Buffer* scaleBuffer;
    pom::gfx::Buffer* indexBuffer;
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

    gamestate->device = instanceVk->device;
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

    spirv_cross::CompilerReflection vertShaderModuleReflection(reinterpret_cast<const u32*>(basic_vert_spv_data),
                                                               basic_vert_spv_size / sizeof(u32));

    spirv_cross::ShaderResources vertShaderResources = vertShaderModuleReflection.get_shader_resources();

    auto inputs = vertShaderResources.stage_inputs;

    // std::vector<VkVertexInputBindingDescription> vertexBindingDescs;
    // std::vector<VkVertexInputAttributeDescription> vertexAttribDescs;

    for (auto& input : inputs) {
        u32 set = vertShaderModuleReflection.get_decoration(input.id, spv::DecorationDescriptorSet);
        u32 binding = vertShaderModuleReflection.get_decoration(input.id, spv::DecorationBinding);
        u32 location = vertShaderModuleReflection.get_decoration(input.id, spv::DecorationLocation);
        u32 offset = vertShaderModuleReflection.get_decoration(input.id, spv::DecorationOffset);

        POM_DEBUG(input.name, ", ", set, ", ", binding, ", ", location, ", ", offset);
    }

    VkVertexInputBindingDescription vertexBindingDescs[] = { {
                                                                 .binding = 0, //*
                                                                 .stride = sizeof(Vertex), //*
                                                                 .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                                                             },
                                                             {
                                                                 .binding = 1, //*
                                                                 .stride = sizeof(f32), //*
                                                                 .inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
                                                             } };

    VkVertexInputAttributeDescription vertexAttribDescs[] = { {
                                                                  // position
                                                                  .location = 0, //*
                                                                  .binding = 0,
                                                                  .format = VK_FORMAT_R32G32B32_SFLOAT, //*
                                                                  .offset = offsetof(Vertex, pos),
                                                              },
                                                              {
                                                                  // color
                                                                  .location = 1, //*
                                                                  .binding = 0,
                                                                  .format = VK_FORMAT_R32G32B32A32_SFLOAT, //*
                                                                  .offset = offsetof(Vertex, color),
                                                              },
                                                              {
                                                                  // scale
                                                                  .location = 2, //*
                                                                  .binding = 1,
                                                                  .format = VK_FORMAT_R32_SFLOAT, //*
                                                                  .offset = 0,
                                                              } };

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 2,
        .pVertexBindingDescriptions = vertexBindingDescs,
        .vertexAttributeDescriptionCount = 3,
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

    // vertex buffer
    void* data;
    gamestate->vertexBuffer = pom::gfx::Buffer::create(pom::gfx::BufferUsage::VERTEX, sizeof(VERTEX_DATA), VERTEX_DATA);

    gamestate->scaleBuffer = pom::gfx::Buffer::create(pom::gfx::BufferUsage::VERTEX, sizeof(SCALE_DATA), SCALE_DATA);

    // index buffer
    gamestate->indexBuffer = pom::gfx::Buffer::create(pom::gfx::BufferUsage::INDEX, sizeof(INDEX_DATA), INDEX_DATA);
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

    // TODO: what is a matrix lol
    auto frame = pom::Application::get()->getFrame();
    for (u8 i = 0; i < 4; i++) {
        VERTEX_DATA[i].pos = {
            (f32)cos(i / 4.f * TAU + frame / 100.f) / 2.f,
            (f32)sin(i / 4.f * TAU + frame / 100.f) / 2.f,
            0.f,
        };
    }
    void* data = gamestate->vertexBuffer->map(0, sizeof(VERTEX_DATA));
    memcpy(data, VERTEX_DATA, sizeof(VERTEX_DATA));
    gamestate->vertexBuffer->unmap();

    gamestate->commandBuffer->begin();
    gamestate->commandBuffer->beginRenderPass(context->getSwapchainRenderPass());

    auto* commandBuffer = dynamic_cast<pom::gfx::CommandBufferVk*>(gamestate->commandBuffer)->getCurrentCommandBuffer();

    // calling this every frame doesn't really matter to my knowledge and is way easier than any other
    // alternatives
    gamestate->commandBuffer->setViewport({ context->swapchainViewport.x, context->swapchainViewport.y },
                                          { context->swapchainViewport.width, context->swapchainViewport.height },
                                          context->swapchainViewport.minDepth,
                                          context->swapchainViewport.maxDepth);
    gamestate->commandBuffer->setScissor({ 0, 0 }, { context->swapchainExtent.width, context->swapchainExtent.height });

    gamestate->commandBuffer->bindVertexBuffer(gamestate->vertexBuffer);
    gamestate->commandBuffer->bindVertexBuffer(gamestate->scaleBuffer, 1);

    gamestate->commandBuffer->bindIndexBuffer(gamestate->indexBuffer, pom::gfx::IndexType::U16);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gamestate->graphicsPipeline);

    gamestate->commandBuffer->drawIndexed(gamestate->indexBuffer->getSize() / sizeof(u16));

    gamestate->commandBuffer->endRenderPass();
    gamestate->commandBuffer->end();

    context->submitCommandBuffer(gamestate->commandBuffer);

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

    delete gamestate->vertexBuffer;
    delete gamestate->scaleBuffer;
    delete gamestate->commandBuffer;
    delete gamestate->indexBuffer;

    vkDestroyPipeline(gamestate->device, gamestate->graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(gamestate->device, gamestate->pipelineLayout, nullptr);

    delete gamestate;
}
