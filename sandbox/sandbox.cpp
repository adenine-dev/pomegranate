#include <pomegranate/pomegranate.hpp>

#include <array>
#include <numeric>

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
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f, 1.0f } }, { { 0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
    { { 0.5f, 0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f } },   { { -0.5f, 0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f } },
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

    VkDescriptorPoolSize poolSize = {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = POM_MAX_FRAMES_IN_FLIGHT * 3,
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
                                                 descSetLayoutCreateInfos.data(),
                                                 nullptr,
                                                 &gamestate->descriptorSetLayouts[i]),
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

        VkWriteDescriptorSet descriptorSetWrite = {
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
        };

        vkUpdateDescriptorSets(gamestate->device, 1, &descriptorSetWrite, 0, nullptr);
    }

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 1,
        .pSetLayouts = &gamestate->descriptorSetLayouts[0],
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
        { { .binding = 0,
            .attribs = { { .location = 0, .format = pom::gfx::Format::R32G32B32_SFLOAT },
                         { .location = 1, .format = pom::gfx::Format::R32G32B32A32_SFLOAT } } },
          { .binding = 1, .attribs = { { .location = 2, .format = pom::gfx::Format::R32_SFLOAT } } } },
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
