#include <pomegranate/pomegranate.hpp>

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
    pom::gfx::CommandBuffer* commandBuffer;
    // vertex buffer
    pom::gfx::Buffer* vertexBuffers[POM_MAX_FRAMES_IN_FLIGHT];
    pom::gfx::Buffer* scaleBuffer;
    pom::gfx::Buffer* indexBuffer;

    // uniform buffers
    pom::maths::vec3 cameraPos = pom::maths::vec3(2, 2, 2);
    pom::gfx::Buffer* uniformBuffers[POM_MAX_FRAMES_IN_FLIGHT];

    // pipeline
    pom::gfx::PipelineLayout* pipelineLayout;
    pom::gfx::DescriptorSet* descriptorSets[POM_MAX_FRAMES_IN_FLIGHT];
    pom::gfx::Pipeline* pipeline;

    // other context test.
    pom::Window* otherWindow;
    pom::gfx::CommandBuffer* otherCommandBuffer;
    pom::gfx::Buffer* otherVertexBuffer;

    // texture
    pom::gfx::Texture* texture;
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
    auto* contextVk = dynamic_cast<pom::gfx::ContextVk*>(pom::Application::get()->getMainWindow().getContext());

    gamestate->otherWindow = new pom::Window("other window", pom::gfx::GraphicsAPI::VULKAN, true);
    gamestate->otherWindow->setEventHandler([gamestate](pom::InputEvent ev) {
        if (ev.type == pom::InputEventType::WINDOW_RESIZE) {
            clientUpdate(gamestate, {});
        }
    });

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

    size_t textureSize = width * height * 4;

    gamestate->texture = pom::gfx::Texture::create(
        {
            .type = pom::gfx::TextureType::IMAGE_2D,
            .usage = pom::gfx::TextureUsage::SAMPLED | pom::gfx::TextureUsage::TRANSFER_DST,
            .textureFormat = pom::gfx::Format::R8G8B8A8_SRGB,
            .viewFormat = pom::gfx::Format::R8G8B8A8_SRGB,
        },
        width,
        height,
        1,
        pixels,
        0,
        textureSize);

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

    pom::gfx::ShaderModule* vertShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStage::VERTEX,
                                         basic_vert_spv_size,
                                         reinterpret_cast<const u32*>(basic_vert_spv_data));

    pom::gfx::ShaderModule* fragShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStage::FRAGMENT,
                                         basic_frag_spv_size,
                                         reinterpret_cast<const u32*>(basic_frag_spv_data));

    pom::gfx::Shader* shader = pom::gfx::Shader::create({ vertShader, fragShader });

    gamestate->pipelineLayout = pom::gfx::PipelineLayout::create({
        {
            .type = pom::gfx::DescriptorType::UNIFORM_BUFFER,
            .set = 0,
            .binding = 0,
            .stages = pom::gfx::ShaderStageFlags::VERTEX,
        },
        {
            .type = pom::gfx::DescriptorType::COMBINED_TEXTURE_SAMPLER,
            .set = 0,
            .binding = 1,
            .stages = pom::gfx::ShaderStageFlags::FRAGMENT,
        },
    });

    // descriptor set
    for (u32 i = 0; i < POM_MAX_FRAMES_IN_FLIGHT; i++) {
        gamestate->uniformBuffers[i] = pom::gfx::Buffer::create(pom::gfx::BufferUsage::UNIFORM,
                                                                pom::gfx::BufferMemoryAccess::CPU_WRITE,
                                                                sizeof(UniformMVP));

        gamestate->descriptorSets[i] = pom::gfx::DescriptorSet::create(gamestate->pipelineLayout, 0);
        gamestate->descriptorSets[i]->setBuffer(0, gamestate->uniformBuffers[i]);
        gamestate->descriptorSets[i]->setTexture(1, gamestate->texture);
    }

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

    delete shader;
    delete vertShader;
    delete fragShader;

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
    auto frame = pom::Application::get()->getFrame();
    POM_PROFILE_SCOPE("update");
    {
        if (!pom::Application::get()->getMainWindow().isMinimized()) {
            auto* context = dynamic_cast<pom::gfx::ContextVk*>(pom::Application::get()->getMainWindow().getContext());

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

            gamestate->commandBuffer->setViewport(context->getSwapchainViewport());
            gamestate->commandBuffer->setScissor({ 0, 0 },
                                                 { context->swapchainExtent.width, context->swapchainExtent.height });

            gamestate->commandBuffer->bindVertexBuffer(gamestate->otherVertexBuffer);
            gamestate->commandBuffer->bindVertexBuffer(gamestate->scaleBuffer, 1);

            gamestate->commandBuffer->bindIndexBuffer(gamestate->indexBuffer, pom::gfx::IndexType::U16);

            gamestate->commandBuffer->bindPipeline(gamestate->pipeline);

            gamestate->commandBuffer->bindDescriptorSet(gamestate->pipelineLayout,
                                                        0,
                                                        gamestate->descriptorSets[frame % POM_MAX_FRAMES_IN_FLIGHT]);

            gamestate->commandBuffer->drawIndexed(gamestate->indexBuffer->getSize() / sizeof(u16));

            gamestate->commandBuffer->endRenderPass();
            gamestate->commandBuffer->end();

            gamestate->commandBuffer->submit();

            pom::Application::get()->getMainWindow().getContext()->present();
        }
    }

    if (!gamestate->otherWindow->isMinimized()) {
        auto* ctx = dynamic_cast<pom::gfx::ContextVk*>(gamestate->otherWindow->getContext());
        gamestate->otherCommandBuffer->begin();
        gamestate->otherCommandBuffer->beginRenderPass(ctx->getSwapchainRenderPass(), ctx);

        gamestate->otherCommandBuffer->setViewport(ctx->getSwapchainViewport());
        gamestate->otherCommandBuffer->setScissor({ 0, 0 },
                                                  { ctx->swapchainExtent.width, ctx->swapchainExtent.height });

        auto* commandBuffer
            = dynamic_cast<pom::gfx::CommandBufferVk*>(gamestate->otherCommandBuffer)->getCurrentCommandBuffer();

        gamestate->otherCommandBuffer->bindVertexBuffer(gamestate->otherVertexBuffer);
        gamestate->otherCommandBuffer->bindVertexBuffer(gamestate->scaleBuffer, 1);

        gamestate->otherCommandBuffer->bindIndexBuffer(gamestate->indexBuffer, pom::gfx::IndexType::U16);

        gamestate->otherCommandBuffer->bindPipeline(gamestate->pipeline);

        gamestate->otherCommandBuffer->bindDescriptorSet(gamestate->pipelineLayout,
                                                         0,
                                                         gamestate->descriptorSets[frame % POM_MAX_FRAMES_IN_FLIGHT]);

        gamestate->otherCommandBuffer->drawIndexed(gamestate->indexBuffer->getSize() / sizeof(u16));

        gamestate->otherCommandBuffer->endRenderPass();
        gamestate->otherCommandBuffer->end();

        gamestate->otherCommandBuffer->submit();

        ctx->present();
    }

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
    delete gamestate->texture;

    for (auto& vb : gamestate->vertexBuffers) {
        delete vb;
    }
    delete gamestate->scaleBuffer;
    delete gamestate->commandBuffer;
    delete gamestate->indexBuffer;

    for (int i = 0; i < POM_MAX_FRAMES_IN_FLIGHT; i++) {
        delete gamestate->uniformBuffers[i];
        delete gamestate->descriptorSets[i];
    }

    delete gamestate->pipeline;
    delete gamestate->pipelineLayout;

    delete gamestate->otherCommandBuffer;
    delete gamestate->otherVertexBuffer;
    delete gamestate->otherWindow;

    delete gamestate;
}
