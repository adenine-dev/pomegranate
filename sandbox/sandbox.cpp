#include <pomegranate/pomegranate.hpp>

#include "embed/basic_frag_spv.hpp"
#include "embed/basic_vert_spv.hpp"

#include <spirv_reflect.hpp>

struct Vertex {
    pom::maths::vec3 pos;
    pom::Color color;
};

struct UniformMVP {
    pom::maths::mat4 model;
    pom::maths::mat4 view;
    pom::maths::mat4 projection;
};

static Vertex VERTEX_DATA[] = {
    { { -0.5f, -0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } },  { { 0.5f, -0.5f, 0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f } },
    { { 0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f, 1.0f, 1.0f } },    { { -0.5f, 0.5f, 0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, { { 0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
    { { 0.5f, 0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f } },   { { -0.5f, 0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f, 1.0f } },
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
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet descriptorSets[POM_MAX_FRAMES_IN_FLIGHT];
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    // other context test.
    pom::Window* otherWindow;
    pom::gfx::CommandBuffer* otherCommandBuffer;
    pom::gfx::Buffer* otherVertexBuffer;
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

POM_CLIENT_EXPORT void clientUpdate(GameState* gamestate, pom::DeltaTime dt);

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

    // descriptor set
    VkDescriptorSetLayoutBinding uboLayoutBinding = {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .pImmutableSamplers = nullptr,
    };

    VkDescriptorSetLayoutCreateInfo uboLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .bindingCount = 1,
        .pBindings = &uboLayoutBinding,
    };

    for (auto& ub : gamestate->uniformBuffers) {
        ub = pom::gfx::Buffer::create(pom::gfx::BufferUsage::UNIFORM,
                                      pom::gfx::BufferMemoryAccess::CPU_WRITE,
                                      sizeof(UniformMVP));
    }

    POM_ASSERT(
        vkCreateDescriptorSetLayout(gamestate->device, &uboLayoutCreateInfo, nullptr, &gamestate->descriptorSetLayout)
            == VK_SUCCESS,
        "Failed to create descriptor set layout.");

    VkDescriptorPoolSize poolSize = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = POM_MAX_FRAMES_IN_FLIGHT,
    };

    VkDescriptorPoolCreateInfo poolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .maxSets = POM_MAX_FRAMES_IN_FLIGHT,
        .poolSizeCount = 1,
        .pPoolSizes = &poolSize,
    };

    vkCreateDescriptorPool(gamestate->device, &poolCreateInfo, nullptr, &gamestate->descriptorPool);

    VkDescriptorSetLayout descriptorSetLayouts[POM_MAX_FRAMES_IN_FLIGHT]
        = { gamestate->descriptorSetLayout, gamestate->descriptorSetLayout };

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = gamestate->descriptorPool,
        .descriptorSetCount = POM_MAX_FRAMES_IN_FLIGHT,
        .pSetLayouts = descriptorSetLayouts,
    };

    POM_ASSERT(vkAllocateDescriptorSets(gamestate->device, &descriptorSetAllocateInfo, gamestate->descriptorSets)
                   == VK_SUCCESS,
               "Failed to allocate descriptor sets");

    for (u8 i = 0; i < POM_MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo = {
            .buffer = dynamic_cast<pom::gfx::BufferVk*>(gamestate->uniformBuffers[i])->getBuffer(),
            .offset = 0,
            .range = sizeof(UniformMVP),
        };
        VkWriteDescriptorSet descriptorSetWrite = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = gamestate->descriptorSets[i],
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pImageInfo = nullptr,
            .pBufferInfo = &bufferInfo,
            .pTexelBufferView = nullptr,
        };

        vkUpdateDescriptorSets(gamestate->device, 1, &descriptorSetWrite, 0, nullptr);
    }

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
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
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
        .setLayoutCount = 1,
        .pSetLayouts = &gamestate->descriptorSetLayout,
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

                data->model = pom::maths::mat4::rotate({ TAU / 100.f * (f32)frame, 0, 0 });
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

            // calling this every frame doesn't really matter to my knowledge and is way easier than any other
            // alternatives
            gamestate->commandBuffer->setViewport(
                { context->swapchainViewport.x, context->swapchainViewport.y },
                { context->swapchainViewport.width, context->swapchainViewport.height },
                context->swapchainViewport.minDepth,
                context->swapchainViewport.maxDepth);
            gamestate->commandBuffer->setScissor({ 0, 0 },
                                                 { context->swapchainExtent.width, context->swapchainExtent.height });

            gamestate->commandBuffer->bindVertexBuffer(gamestate->otherVertexBuffer);
            gamestate->commandBuffer->bindVertexBuffer(gamestate->scaleBuffer, 1);

            gamestate->commandBuffer->bindIndexBuffer(gamestate->indexBuffer, pom::gfx::IndexType::U16);

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gamestate->graphicsPipeline);
            vkCmdBindDescriptorSets(commandBuffer,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    gamestate->pipelineLayout,
                                    0,
                                    1,
                                    &gamestate->descriptorSets[frame % POM_MAX_FRAMES_IN_FLIGHT],
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
    //                                                { ctx->swapchainViewport.width, ctx->swapchainViewport.height },
    //                                                ctx->swapchainViewport.minDepth,
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
    vkDestroyDescriptorSetLayout(gamestate->device, gamestate->descriptorSetLayout, nullptr);
    vkDestroyPipeline(gamestate->device, gamestate->graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(gamestate->device, gamestate->pipelineLayout, nullptr);

    delete gamestate->otherCommandBuffer;
    delete gamestate->otherVertexBuffer;
    delete gamestate->otherWindow;

    delete gamestate;
}
