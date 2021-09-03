#include <pomegranate.hpp>

#include "embed/triangle_frag_spv.hpp"
#include "embed/triangle_vert_spv.hpp"

struct Vertex {
    pom::maths::vec3 pos;
    pom::Color color;
};

struct State {
    pom::Ref<pom::gfx::CommandBuffer> commandBuffer;
    pom::Ref<pom::gfx::Buffer> vertexBuffer;
    pom::Ref<pom::gfx::Pipeline> pipeline;
    pom::Ref<pom::gfx::PipelineLayout> pipelineLayout;
};

static Vertex VERTEX_DATA[] = {
    { { 0.0f, -0.5f, 0.f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
    { { -0.5f, 0.5f, 0.f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
    { { 0.5f, 0.5f, 0.f }, { 0.0f, 0.0f, 1.0f, 1.0f } },
};

POM_CLIENT_EXPORT const pom::AppCreateInfo* clientGetAppCreateInfo(int /*argc*/, char** /*argv*/)
{
    static const pom::AppCreateInfo aci = {
        .name = "Triangle",
    };

    return &aci;
}

POM_CLIENT_EXPORT State* clientCreateState()
{
    return new State;
}

POM_CLIENT_EXPORT void clientMount(State* state)
{
    state->commandBuffer = pom::gfx::CommandBuffer::create(pom::gfx::CommandBufferSpecialization::GRAPHICS);

    state->vertexBuffer = pom::gfx::Buffer::create(pom::gfx::BufferUsage::VERTEX,
                                                   pom::gfx::BufferMemoryAccess::GPU_ONLY,
                                                   sizeof(VERTEX_DATA),
                                                   VERTEX_DATA);

    pom::Ref<pom::gfx::ShaderModule> vertShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStage::VERTEX,
                                         triangle_vert_spv_size,
                                         reinterpret_cast<const u32*>(triangle_vert_spv_data));

    pom::Ref<pom::gfx::ShaderModule> fragShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStage::FRAGMENT,
                                         triangle_frag_spv_size,
                                         reinterpret_cast<const u32*>(triangle_frag_spv_data));

    pom::Ref<pom::gfx::Shader> shader = pom::gfx::Shader::create({ vertShader, fragShader });

    state->pipelineLayout = pom::gfx::PipelineLayout::create({});

    state->pipeline = pom::gfx::Pipeline::create(
        {},
        shader,
        pom::Application::get()->getMainWindow().getContext()->getSwapchainRenderPass(),
        {
            {
                .binding = 0,
                .attribs = { { .location = 0, .format = pom::gfx::Format::R32G32B32_SFLOAT },
                             { .location = 1, .format = pom::gfx::Format::R32G32B32A32_SFLOAT } },
            },
        },
        state->pipelineLayout);
}

POM_CLIENT_EXPORT void clientUpdate(State* state, pom::DeltaTime dt)
{
    if (!pom::Application::get()->getMainWindow().isMinimized()) {
        auto* context = pom::Application::get()->getMainWindow().getContext();

        state->commandBuffer->begin();
        state->commandBuffer->beginRenderPass(context->getSwapchainRenderPass(), context);

        state->commandBuffer->setViewport(context->getSwapchainViewport());
        state->commandBuffer->setScissor({ 0, 0 }, context->getSwapchainExtent());

        state->commandBuffer->bindVertexBuffer(state->vertexBuffer);
        state->commandBuffer->bindPipeline(state->pipeline);

        state->commandBuffer->draw(3);

        state->commandBuffer->endRenderPass();
        state->commandBuffer->end();
        state->commandBuffer->submit();

        context->present();
    }
}

POM_CLIENT_EXPORT void clientEnd(State* state)
{
    delete state;
}
