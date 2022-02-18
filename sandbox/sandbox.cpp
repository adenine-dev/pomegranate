#include <pomegranate/pomegranate.hpp>

#include "embed/shader/basic_frag_spv.hpp"
#include "embed/shader/basic_vert_spv.hpp"

#include "embed/shader/plane_frag_spv.hpp"
#include "embed/shader/plane_vert_spv.hpp"

#include "embed/img/empty_png.hpp"

#include <spirv_reflect.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Vertex {
    pom::maths::vec3 pos;
    pom::Color color;
    pom::maths::vec2 uv;
};

struct ArcballCamera {
    ArcballCamera(f32 width, f32 height) : width(width), height(height)
    {
        calculateProjection();
        calculateView();
    };

    pom::maths::vec3 pivot;
    pom::maths::vec3 eye = pom::maths::vec3(2, 2, 2);
    f32 radius = eye.mag();

    pom::maths::mat4 view;
    pom::maths::mat4 projection;
    f32 width;
    f32 height;
    bool orthographic = false;

    const f32 rotateSpeed = 3.f;
    const f32 panSpeed = 5.f;
    pom::maths::ivec2 lastMousePos;

    void calculateProjection()
    {
        if (orthographic) {
            projection = pom::maths::mat4::ortho(-(width / height), (width / height), -1, 1, -1000, 1000);
        } else {
            projection = pom::maths::mat4::perspective(45, (width / height), 0.01, 1000);
        }
    }

    void calculateView()
    {
        view = pom::maths::mat4::lookAt(eye, pivot, { 0, 1, 0 });
    }

    void handleEvent(pom::InputEvent* ev)
    {
        if (ev->type == pom::InputEventType::MOUSE_SCROLL) {
            radius += ev->getDelta().y;
            radius = std::max(radius, 0.1f);
            eye = eye.norm() * radius;

            calculateView();
        } else if (ev->type == pom::InputEventType::KEY_DOWN) {
            if (ev->getKeycode() == pom::Keycode::O) {
                orthographic = !orthographic;
                calculateProjection();
            }
        } else if (ev->type == pom::InputEventType::WINDOW_RESIZE) {
            width = (f32)ev->getSize().x;
            height = (f32)ev->getSize().y;
            calculateProjection();
        } else if (ev->type == pom::InputEventType::MOUSE_DOWN
                   && ev->getMouseButton() & pom::MouseButton::BUTTON_LEFT) {
            lastMousePos = pom::getMousePostition();
        } else if (ev->type == pom::InputEventType::MOUSE_MOVE
                   && ev->getMouseButton() & pom::MouseButton::BUTTON_LEFT) {
            if (pom::keyDown(pom::KeyHid::KEY_LALT) || pom::keyDown(pom::KeyHid::KEY_RALT)) {
                pom::maths::vec2 delta
                    = pom::maths::vec2(lastMousePos - ev->getPosition()) / pom::maths::vec2(width, height) * panSpeed;
                pivot += view.up() * -delta.y + view.right() * delta.x;
                eye += view.up() * -delta.y + view.right() * delta.x;

                calculateView();
                lastMousePos = ev->getPosition();
            } else {
                pom::maths::vec2 delta = (lastMousePos - ev->getPosition()) * rotateSpeed * eye.mag();

                pom::maths::vec2 angle((f32)(delta.x * TAU / width), (f32)(delta.y * PI / height));

                float cosAngle = dot(view.forward(), view.up());
                if (cosAngle * sgn(angle.y) > 0.99f)
                    angle.y = 0;

                // eye = ((pom::maths::mat3::rotate(angle.x, view.up()) * (eye - pivot)) + pivot).norm() * eye.mag();
                // eye = ((pom::maths::mat3::rotate(angle.y, view.right()) * (eye - pivot)) + pivot).norm() * eye.mag();

                eye = (eye - pivot + (view.up() * -angle.y + view.right() * angle.x)).norm() * radius + pivot;

                calculateView();
                lastMousePos = ev->getPosition();
            }
        }
    }
};

struct UniformMVP {
    pom::maths::mat4 model;
    pom::maths::mat4 view;
    pom::maths::mat4 projection;
};

// clang-format off
static Vertex VERTEX_DATA[] = {
    { { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.f, 0.f } },
    { {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f }, { 1.f, 0.f } },
    { {  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 1.0f, 1.0f }, { 1.f, 1.f } },
    { { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.f, 1.f } },
    { { -0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f, 1.0f }, { 0.f, 0.f } },
    { {  0.5f, -0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.f, 0.f } },
    { {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 1.f, 1.f } },
    { { -0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f, 1.0f }, { 0.f, 1.f } },
};


// static Vertex VERTEX_DATA[] = {
//     { { -0.5f, -0.5f,  0 }, { 0.0f, 0.0f, 1.0f, 1.0f }, { 0.f, 0.f } },
//     { {  0.5f, -0.5f,  0 }, { 1.0f, 0.0f, 1.0f, 1.0f }, { 1.f, 0.f } },
//     { {  0.5f,  0.5f,  0 }, { 0.0f, 1.0f, 1.0f, 1.0f }, { 1.f, 1.f } },
//     { { -0.5f,  0.5f,  0 }, { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.f, 1.f } },
// };

// clang-format on

// static const u16 INDEX_DATA[] = { 0, 1, 2, 0, 2, 3 };
static const u16 INDEX_DATA[]
    = { 0, 1, 2, 2, 3, 0, 1, 5, 6, 6, 2, 1, 7, 6, 5, 5, 4, 7, 4, 0, 3, 3, 7, 4, 4, 5, 1, 1, 0, 4, 3, 2, 6, 6, 7, 3 };

struct GridConfig {
    pom::Color xColor;
    pom::Color zColor;
    pom::Color gridColor;
    f32 majorLineScale;
    f32 minorLineScale;
    f32 fadeStrength;
    f32 axisIntensity;
};

struct GameState {
    // instance
    pom::Ref<pom::gfx::CommandBuffer> commandBuffer;
    // vertex buffer
    pom::Ref<pom::gfx::Buffer> vertexBuffers[POM_MAX_FRAMES_IN_FLIGHT];
    pom::Ref<pom::gfx::Buffer> scaleBuffer;
    pom::Ref<pom::gfx::Buffer> indexBuffer;

    // uniform buffers
    pom::Ref<pom::gfx::Buffer> uniformBuffers[POM_MAX_FRAMES_IN_FLIGHT];
    ArcballCamera camera = ArcballCamera(720.f, 480.f);

    // pipeline
    pom::Ref<pom::gfx::PipelineLayout> pipelineLayout;
    pom::Ref<pom::gfx::DescriptorSet> descriptorSets[POM_MAX_FRAMES_IN_FLIGHT];
    pom::Ref<pom::gfx::Pipeline> pipeline;

    // plane
    pom::Ref<pom::gfx::Pipeline> planePipeline;
    pom::Ref<pom::gfx::DescriptorSet> planeDescriptorSets[POM_MAX_FRAMES_IN_FLIGHT];
    pom::Ref<pom::gfx::PipelineLayout> planePipelineLayout;
    pom::Ref<pom::gfx::Buffer> gridConfigBuffers[POM_MAX_FRAMES_IN_FLIGHT];
    GridConfig gridConfig;

    // texture
    pom::Ref<pom::gfx::Texture> texture;
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
    // gamestate->camera
    //     = ArcballCamera(pom::Application::get()->getMainWindow().getContext()->getSwapchainViewport().width
    //                     / pom::Application::get()->getMainWindow().getContext()->getSwapchainViewport().height);

    // gamestate->otherWindow = new pom::Window("other window", pom::gfx::GraphicsAPI::VULKAN, true);
    // gamestate->otherWindow->setEventHandler([gamestate](pom::InputEvent ev) {
    //     if (ev.type == pom::InputEventType::WINDOW_RESIZE) {
    //         clientUpdate(gamestate, {});
    //     }
    // });

    // texture
    i32 width, height, channels;
    const unsigned char* pixels = stbi_load_from_memory(empty_png_data,
                                                        static_cast<int>(empty_png_size),
                                                        &width,
                                                        &height,
                                                        &channels,
                                                        STBI_rgb_alpha);

    usize textureSize = width * height * 4;

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

    pom::Ref<pom::gfx::ShaderModule> vertShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStage::VERTEX,
                                         basic_vert_spv_size,
                                         reinterpret_cast<const u32*>(basic_vert_spv_data));

    pom::Ref<pom::gfx::ShaderModule> fragShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStage::FRAGMENT,
                                         basic_frag_spv_size,
                                         reinterpret_cast<const u32*>(basic_frag_spv_data));

    pom::Ref<pom::gfx::Shader> shader = pom::gfx::Shader::create({ vertShader, fragShader });

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
        {
            .cullMode = pom::gfx::CullMode::NONE
        },
        shader,
        pom::Application::get()->getMainWindow().getContext()->getSwapchainRenderPass(),
        {
            {
                .binding = 0,
                .attribs = { { .location = 0, .format = pom::gfx::Format::R32G32B32_SFLOAT },
                             { .location = 1, .format = pom::gfx::Format::R32G32B32A32_SFLOAT },
                             { .location = 2, .format = pom::gfx::Format::R32G32_SFLOAT }, },
            },
        },
        gamestate->pipelineLayout);

    // command buffer
    gamestate->commandBuffer = pom::gfx::CommandBuffer::create(pom::gfx::CommandBufferSpecialization::GRAPHICS);

    // vertex buffer
    for (auto& vertexBuffer : gamestate->vertexBuffers) {
        vertexBuffer = pom::gfx::Buffer::create(pom::gfx::BufferUsage::VERTEX,
                                                pom::gfx::BufferMemoryAccess::CPU_WRITE,
                                                sizeof(VERTEX_DATA),
                                                VERTEX_DATA);
    }

    // index buffer
    gamestate->indexBuffer = pom::gfx::Buffer::create(pom::gfx::BufferUsage::INDEX,
                                                      pom::gfx::BufferMemoryAccess::GPU_ONLY,
                                                      sizeof(INDEX_DATA),
                                                      INDEX_DATA);

    // plane test

    pom::Ref<pom::gfx::ShaderModule> planeVertShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStage::VERTEX,
                                         plane_vert_spv_size,
                                         reinterpret_cast<const u32*>(plane_vert_spv_data));

    pom::Ref<pom::gfx::ShaderModule> planeFragShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStage::FRAGMENT,
                                         plane_frag_spv_size,
                                         reinterpret_cast<const u32*>(plane_frag_spv_data));

    pom::Ref<pom::gfx::Shader> planeShader = pom::gfx::Shader::create({ planeVertShader, planeFragShader });

    gamestate->planePipelineLayout = pom::gfx::PipelineLayout::create({
        {
            .type = pom::gfx::DescriptorType::UNIFORM_BUFFER,
            .set = 0,
            .binding = 0,
            .stages = pom::gfx::ShaderStageFlags::VERTEX,
        },
        {
            .type = pom::gfx::DescriptorType::UNIFORM_BUFFER,
            .set = 0,
            .binding = 1,
            .stages = pom::gfx::ShaderStageFlags::FRAGMENT,
        },
    });

    for (u32 i = 0; i < POM_MAX_FRAMES_IN_FLIGHT; i++) {
        gamestate->planeDescriptorSets[i] = pom::gfx::DescriptorSet::create(gamestate->planePipelineLayout, 0);
        gamestate->planeDescriptorSets[i]->setBuffer(0,
                                                     gamestate->uniformBuffers[i],
                                                     sizeof(pom::maths::mat4),
                                                     sizeof(pom::maths::mat4) * 2);

        gamestate->gridConfigBuffers[i] = pom::gfx::Buffer::create(pom::gfx::BufferUsage::UNIFORM,
                                                                   pom::gfx::BufferMemoryAccess::CPU_WRITE,
                                                                   sizeof(GridConfig));
        gamestate->planeDescriptorSets[i]->setBuffer(1, gamestate->gridConfigBuffers[i]);
    }

    gamestate->gridConfig = {
        .xColor = pom::Color::rgb(255, 0, 0),
        .zColor = pom::Color::rgb(0, 0, 255),
        .gridColor = pom::Color::rgb(50, 50, 50),
        .majorLineScale = 10.f,
        .minorLineScale = 1.f,
        .fadeStrength = 0.08f,
        .axisIntensity = 50.f,
    };

    gamestate->planePipeline
        = pom::gfx::Pipeline::create({},
                                     planeShader,
                                     pom::Application::get()->getMainWindow().getContext()->getSwapchainRenderPass(),
                                     {},
                                     gamestate->planePipelineLayout);
}

POM_CLIENT_EXPORT void clientMount(GameState* gamestate)
{
}

POM_CLIENT_EXPORT void clientUpdate(GameState* gs, pom::DeltaTime dt)
{
    auto frame = pom::Application::get()->getFrame();
    POM_PROFILE_SCOPE("update");
    {
        if (!pom::Application::get()->getMainWindow().isMinimized()) {
            auto* context = dynamic_cast<pom::gfx::ContextVk*>(pom::Application::get()->getMainWindow().getContext());

            {
                // POM_DEBUG(gs->camera.position);
                pom::Ref<pom::gfx::Buffer> uniformBuffer = gs->uniformBuffers[frame % POM_MAX_FRAMES_IN_FLIGHT];
                UniformMVP* data = (UniformMVP*)uniformBuffer->map();

                // data->model = pom::maths::mat4::translate(gs->camera.pivot);
                data->model = pom::maths::mat4::identity();

                data->projection = gs->camera.projection;
                data->view = gs->camera.view;

                uniformBuffer->unmap();
            }

            gs->commandBuffer->begin();
            gs->commandBuffer->beginRenderPass(context->getSwapchainRenderPass(), context);

            gs->commandBuffer->setViewport(context->getSwapchainViewport());
            gs->commandBuffer->setScissor({ 0, 0 },
                                          { context->swapchainExtent.width, context->swapchainExtent.height });

            gs->commandBuffer->bindVertexBuffer(gs->vertexBuffers[frame % POM_MAX_FRAMES_IN_FLIGHT]);
            gs->commandBuffer->bindIndexBuffer(gs->indexBuffer, pom::gfx::IndexType::U16);
            gs->commandBuffer->bindPipeline(gs->pipeline);

            gs->commandBuffer->bindDescriptorSet(gs->pipelineLayout,
                                                 0,
                                                 gs->descriptorSets[frame % POM_MAX_FRAMES_IN_FLIGHT]);

            gs->commandBuffer->drawIndexed(gs->indexBuffer->getSize() / sizeof(u16));

            pom::Ref<pom::gfx::Buffer> gridConfigBuffer = gs->gridConfigBuffers[frame % POM_MAX_FRAMES_IN_FLIGHT];
            auto* config = (GridConfig*)gridConfigBuffer->map();
            *config = gs->gridConfig;
            gridConfigBuffer->unmap();

            gs->commandBuffer->bindPipeline(gs->planePipeline);

            gs->commandBuffer->bindDescriptorSet(gs->planePipelineLayout,
                                                 0,
                                                 gs->planeDescriptorSets[frame % POM_MAX_FRAMES_IN_FLIGHT]);

            gs->commandBuffer->draw(6);

            gs->commandBuffer->endRenderPass();
            gs->commandBuffer->end();

            gs->commandBuffer->submit();

            pom::Application::get()->getMainWindow().getContext()->present();
        }
    }

    // POM_DEBUG("dt: ", dt, "ms");
}

POM_CLIENT_EXPORT void clientOnInputEvent(GameState* gs, pom::InputEvent* ev)
{
    gs->camera.handleEvent(ev);

    if (ev->type == pom::InputEventType::KEY_UP) {
        if (ev->getKeycode() == pom::Keycode::P) {
            if (!pom::Profiler::active())
                pom::Profiler::begin();
            else
                pom::Profiler::end();
        }
    }
}

POM_CLIENT_EXPORT void clientUnmount(GameState* gamestate)
{
}

POM_CLIENT_EXPORT void clientEnd(GameState* gamestate)
{
    // delete gamestate->otherWindow;

    delete gamestate;
}
