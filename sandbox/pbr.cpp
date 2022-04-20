#include <pomegranate/pomegranate.hpp>

#include "embed/shader/skybox/equirectangular_to_cubemap_comp_spv.hpp"

#include "embed/shader/ibl/brdf_comp_spv.hpp"
#include "embed/shader/ibl/irradiance_comp_spv.hpp"
#include "embed/shader/ibl/prefiltered_envmap_comp_spv.hpp"

#include "embed/shader/skybox/skybox_frag_spv.hpp"
#include "embed/shader/skybox/skybox_vert_spv.hpp"

#include "embed/shader/plane/plane_frag_spv.hpp"
#include "embed/shader/plane/plane_vert_spv.hpp"

#include "embed/shader/pbr/pbr_frag_spv.hpp"
#include "embed/shader/pbr/pbr_vert_spv.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
                pom::maths::vec2 delta = pom::maths::vec2(lastMousePos - ev->getPosition()) * (rotateSpeed * eye.mag());

                pom::maths::vec2 angle((f32)(delta.x * TAU / width), (f32)(delta.y * PI / height));

                float cosAngle = dot(view.forward(), view.up());
                if (cosAngle * sgn(angle.y) > 0.99f)
                    angle.y = 0;

                eye = (eye - pivot + (view.up() * -angle.y + view.right() * angle.x)).norm() * radius + pivot;

                calculateView();
                lastMousePos = ev->getPosition();
            }
        }
    }
};

struct CameraData {
    pom::maths::mat4 view;
    pom::maths::mat4 projection;
};

struct LightData {
    pom::maths::vec4 positions[3];
    pom::maths::vec4 colors[3];
};

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
    pom::Ref<pom::gfx::CommandBuffer> commandBuffer;

    ArcballCamera camera = ArcballCamera(720.f, 480.f);

    // pipeline
    pom::geometry::GPUMesh<pom::geometry::MeshVertex> sphereMesh;
    pom::Store store;

    pom::Ref<pom::gfx::PipelineLayout> pipelineLayout;
    pom::Ref<pom::gfx::DescriptorSet> sceneDescriptorSets[POM_MAX_FRAMES_IN_FLIGHT];
    pom::Ref<pom::gfx::Pipeline> pipeline;

    pom::Ref<pom::gfx::Buffer> cameraBuffers[POM_MAX_FRAMES_IN_FLIGHT];
    pom::Ref<pom::gfx::Buffer> lightBuffers[POM_MAX_FRAMES_IN_FLIGHT];

    // plane
    pom::Ref<pom::gfx::Pipeline> planePipeline;
    pom::Ref<pom::gfx::DescriptorSet> planeDescriptorSets[POM_MAX_FRAMES_IN_FLIGHT];
    pom::Ref<pom::gfx::PipelineLayout> planePipelineLayout;
    pom::Ref<pom::gfx::Buffer> gridConfigBuffers[POM_MAX_FRAMES_IN_FLIGHT];
    GridConfig gridConfig;

    // sky
    pom::IBLEnvironment environment;
    pom::Ref<pom::gfx::PipelineLayout> skyboxPipelineLayout;
    pom::Ref<pom::gfx::DescriptorSet> skyboxDescriptorSets[POM_MAX_FRAMES_IN_FLIGHT];
    pom::Ref<pom::gfx::Pipeline> skyboxPipeline;

    u32 tex = 0;
    u32 lod = 0;

    pom::Ref<pom::gfx::Texture> brdfLutTexture;
    pom::Ref<pom::gfx::TextureView> brdfLutTextureView;

    f32 metalic = 0.0;
    f32 roughness = 0.0;
};

struct TransformComponent {
    pom::maths::mat4 transform;

    ECS_COMPONENT();
};

struct MeshComponent {
    pom::geometry::GPUMesh<pom::geometry::MeshVertex>* meshptr;

    ECS_COMPONENT();
};

struct RenderComponent {
    pom::Ref<pom::gfx::Buffer> transformBuffers[POM_MAX_FRAMES_IN_FLIGHT];
    pom::Ref<pom::gfx::Buffer> materialBuffers[POM_MAX_FRAMES_IN_FLIGHT];
    pom::Ref<pom::gfx::DescriptorSet> descriptorSets[POM_MAX_FRAMES_IN_FLIGHT];

    ECS_COMPONENT();
};

struct MaterialComponent {
    pom::maths::vec3 albedo;
    float metalic;
    float roughness;

    ECS_COMPONENT();
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

void initGrid(GameState* gs)
{
    pom::Ref<pom::gfx::ShaderModule> vertShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStageFlags::VERTEX,
                                         plane_vert_spv_size,
                                         reinterpret_cast<const u32*>(plane_vert_spv_data)); // NOLINT

    pom::Ref<pom::gfx::ShaderModule> fragShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStageFlags::FRAGMENT,
                                         plane_frag_spv_size,
                                         reinterpret_cast<const u32*>(plane_frag_spv_data)); // NOLINT

    pom::Ref<pom::gfx::Shader> planeShader = pom::gfx::Shader::create({ vertShader, fragShader });

    gs->planePipelineLayout = pom::gfx::PipelineLayout::create(POM_MAX_FRAMES_IN_FLIGHT,
                                                               {
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
                                                               },
                                                               {});

    for (u32 i = 0; i < POM_MAX_FRAMES_IN_FLIGHT; i++) {
        gs->planeDescriptorSets[i] = pom::gfx::DescriptorSet::create(gs->planePipelineLayout, 0);
        gs->planeDescriptorSets[i]->setBuffer(pom::gfx::DescriptorType::UNIFORM_BUFFER, 0, gs->cameraBuffers[i]);

        gs->gridConfigBuffers[i] = pom::gfx::Buffer::create(pom::gfx::BufferUsage::UNIFORM,
                                                            pom::gfx::BufferMemoryAccess::CPU_WRITE,
                                                            sizeof(GridConfig));
        gs->planeDescriptorSets[i]->setBuffer(pom::gfx::DescriptorType::UNIFORM_BUFFER, 1, gs->gridConfigBuffers[i]);
    }

    gs->gridConfig = {
        .xColor = pom::Color::rgb(255, 0, 0),
        .zColor = pom::Color::rgb(0, 0, 255),
        .gridColor = pom::Color::rgb(100, 100, 100),
        .majorLineScale = 10.f,
        .minorLineScale = 1.f,
        .fadeStrength = 0.15f,
        .axisIntensity = 50.f,
    };

    gs->planePipeline
        = pom::gfx::Pipeline::create({},
                                     planeShader,
                                     pom::Application::get()->getMainWindow().getContext()->getSwapchainRenderPass(),
                                     {},
                                     gs->planePipelineLayout);
}

void initBrdfLut(GameState* gs)
{
    gs->brdfLutTexture = pom::gfx::Texture::create(
        {
            .type = pom::gfx::TextureType::IMAGE_2D,
            .usage = pom::gfx::TextureUsage::SAMPLED | pom::gfx::TextureUsage::STORAGE,
            .format = pom::gfx::Format::R16G16_SFLOAT,
        },
        512,
        512);

    gs->brdfLutTextureView = pom::gfx::TextureView::create(gs->brdfLutTexture,
                                                           {
                                                               .type = pom::gfx::TextureViewType::VIEW_2D,
                                                               .format = pom::gfx::Format::R16G16_SFLOAT,
                                                           });

    auto brdfComputeShader = pom::gfx::ShaderModule::create(pom::gfx::ShaderStageFlags::COMPUTE,
                                                            brdf_comp_spv_size,
                                                            reinterpret_cast<const u32*>(brdf_comp_spv_data)); // NOLINT
    auto brdfShader = pom::gfx::Shader::create({ brdfComputeShader });
    auto brdfPipelineLayout = pom::gfx::PipelineLayout::create(1,
                                                               {
                                                                   {
                                                                       .type = pom::gfx::DescriptorType::STORAGE_IMAGE,
                                                                       .set = 0,
                                                                       .binding = 0,
                                                                       .stages = pom::gfx::ShaderStageFlags::COMPUTE,
                                                                   },
                                                               },
                                                               {});

    auto brdfPipeline = pom::gfx::Pipeline::createCompute(brdfShader, brdfPipelineLayout);
    auto brdfDescriptorSet = pom::gfx::DescriptorSet::create(brdfPipelineLayout, 0);
    brdfDescriptorSet->setTextureView(pom::gfx::DescriptorType::STORAGE_IMAGE, 0, gs->brdfLutTextureView);

    auto commandBuffer = pom::gfx::CommandBuffer::create(pom::gfx::CommandBufferSpecialization::GENERAL, 1);
    commandBuffer->begin();
    commandBuffer->bindPipeline(brdfPipeline);
    commandBuffer->bindDescriptorSet(pom::gfx::PipelineBindPoint::COMPUTE, brdfPipelineLayout, 0, brdfDescriptorSet);
    commandBuffer->dispatch(gs->brdfLutTexture->getWidth() / 16, gs->brdfLutTexture->getHeight() / 16);
    commandBuffer->end();
    commandBuffer->submit();
}

void initSkybox(GameState* gamestate)
{
    // my computer isn't great so we use 2k to not crash
    gamestate->environment = pom::loadIBLEnvironment("kiara_1_dawn_2k.hdr");

    pom::Ref<pom::gfx::ShaderModule> skyboxVertShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStageFlags::VERTEX,
                                         skybox_vert_spv_size,
                                         reinterpret_cast<const u32*>(skybox_vert_spv_data)); // NOLINT

    pom::Ref<pom::gfx::ShaderModule> skyboxFragShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStageFlags::FRAGMENT,
                                         skybox_frag_spv_size,
                                         reinterpret_cast<const u32*>(skybox_frag_spv_data)); // NOLINT

    pom::Ref<pom::gfx::Shader> skyboxShader = pom::gfx::Shader::create({ skyboxVertShader, skyboxFragShader });

    gamestate->skyboxPipelineLayout = pom::gfx::PipelineLayout::create(
        POM_MAX_FRAMES_IN_FLIGHT,
        {
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
        },
        { { .stages = pom::gfx::ShaderStageFlags::FRAGMENT, .size = sizeof(u32), .offset = 0 } });

    for (u32 i = 0; i < POM_MAX_FRAMES_IN_FLIGHT; i++) {
        gamestate->skyboxDescriptorSets[i] = pom::gfx::DescriptorSet::create(gamestate->skyboxPipelineLayout, 0);
        gamestate->skyboxDescriptorSets[i]->setBuffer(pom::gfx::DescriptorType::UNIFORM_BUFFER,
                                                      0,
                                                      gamestate->cameraBuffers[i]);
        gamestate->skyboxDescriptorSets[i]->setTextureView(pom::gfx::DescriptorType::COMBINED_TEXTURE_SAMPLER,
                                                           1,
                                                           gamestate->environment.irradianceMapView);
    }

    gamestate->skyboxPipeline
        = pom::gfx::Pipeline::create({},
                                     skyboxShader,
                                     pom::Application::get()->getMainWindow().getContext()->getSwapchainRenderPass(),
                                     {},
                                     gamestate->skyboxPipelineLayout);
}

void initIrradiance(GameState* gamestate)
{
}

void initStore(GameState* gamestate)
{
    auto createEntity = [](GameState* gs, pom::maths::vec3 albedo, f32 roughness, f32 metalness, f32 x, f32 y) {
        auto e = gs->store.createEntity();
        gs->store.addComponent<TransformComponent>(e) = {
            pom::maths::mat4::translate({ (f32)x * 2.3f, (f32)y * 2.3f, 0.f }),
        };
        gs->store.addComponent<MeshComponent>(e) = {
            .meshptr = &gs->sphereMesh,
        };
        gs->store.addComponent<MaterialComponent>(e) = {
            .albedo = albedo,
            .metalic = metalness,
            .roughness = roughness,
        };
        auto& r = gs->store.addComponent<RenderComponent>(e);

        for (u32 i = 0; i < POM_MAX_FRAMES_IN_FLIGHT; i++) {
            r.transformBuffers[i] = pom::gfx::Buffer::create(pom::gfx::BufferUsage::UNIFORM,
                                                             pom::gfx::BufferMemoryAccess::CPU_WRITE,
                                                             sizeof(TransformComponent));
            r.descriptorSets[i] = pom::gfx::DescriptorSet::create(gs->pipelineLayout, 1);
            r.descriptorSets[i]->setBuffer(pom::gfx::DescriptorType::UNIFORM_BUFFER, 0, r.transformBuffers[i]);

            r.materialBuffers[i] = pom::gfx::Buffer::create(pom::gfx::BufferUsage::UNIFORM,
                                                            pom::gfx::BufferMemoryAccess::CPU_WRITE,
                                                            sizeof(MaterialComponent));
            r.descriptorSets[i]->setBuffer(pom::gfx::DescriptorType::UNIFORM_BUFFER, 1, r.materialBuffers[i]);
        }
    };

    createEntity(gamestate, pom::maths::vec3(0.2), gamestate->roughness, gamestate->metalic, 0, 0);

    createEntity(gamestate, pom::maths::vec3(1, 0.86, 0.57), 0, 0, 1, 0);
    createEntity(gamestate, pom::maths::vec3(0.98, 0.82, 0.76), 0, 1, -1, 0);
    createEntity(gamestate, pom::maths::vec3(0, 0, 1), 0.7, 0.3, 2, 0);
    createEntity(gamestate, pom::maths::vec3(0.3, 0.0, 0.3), 0, 0, -2, 0);
    createEntity(gamestate, pom::maths::vec3(0.7, 0, 0), 0.3, 1, 3, 0);
    createEntity(gamestate, pom::maths::vec3(0, 0, 0), 0.4, 0, -3, 0);
}

POM_CLIENT_EXPORT void clientBegin(GameState* gamestate)
{
    for (u32 i = 0; i < POM_MAX_FRAMES_IN_FLIGHT; i++) {
        gamestate->cameraBuffers[i] = pom::gfx::Buffer::create(pom::gfx::BufferUsage::UNIFORM,
                                                               pom::gfx::BufferMemoryAccess::CPU_WRITE,
                                                               sizeof(CameraData));
    }

    gamestate->commandBuffer = pom::gfx::CommandBuffer::create(pom::gfx::CommandBufferSpecialization::GRAPHICS);
    gamestate->sphereMesh = pom::geometry::sphere();

    initSkybox(gamestate);
    initBrdfLut(gamestate);
    initGrid(gamestate);

    // pipeline
    pom::Ref<pom::gfx::ShaderModule> vertShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStageFlags::VERTEX,
                                         pbr_vert_spv_size,
                                         reinterpret_cast<const u32*>(pbr_vert_spv_data)); // NOLINT

    pom::Ref<pom::gfx::ShaderModule> fragShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStageFlags::FRAGMENT,
                                         pbr_frag_spv_size,
                                         reinterpret_cast<const u32*>(pbr_frag_spv_data)); // NOLINT

    pom::Ref<pom::gfx::Shader> shader = pom::gfx::Shader::create({ vertShader, fragShader });

    gamestate->pipelineLayout
        = pom::gfx::PipelineLayout::create(POM_MAX_FRAMES_IN_FLIGHT * 100,
                                           {
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
                                               {
                                                   .type = pom::gfx::DescriptorType::COMBINED_TEXTURE_SAMPLER,
                                                   .set = 0,
                                                   .binding = 2,
                                                   .stages = pom::gfx::ShaderStageFlags::FRAGMENT,
                                               },

                                               {
                                                   .type = pom::gfx::DescriptorType::COMBINED_TEXTURE_SAMPLER,
                                                   .set = 0,
                                                   .binding = 3,
                                                   .stages = pom::gfx::ShaderStageFlags::FRAGMENT,
                                               },
                                               {
                                                   .type = pom::gfx::DescriptorType::COMBINED_TEXTURE_SAMPLER,
                                                   .set = 0,
                                                   .binding = 4,
                                                   .stages = pom::gfx::ShaderStageFlags::FRAGMENT,
                                               },
                                               {
                                                   .type = pom::gfx::DescriptorType::UNIFORM_BUFFER,
                                                   .set = 1,
                                                   .binding = 0,
                                                   .stages = pom::gfx::ShaderStageFlags::VERTEX,
                                               },
                                               {
                                                   .type = pom::gfx::DescriptorType::UNIFORM_BUFFER,
                                                   .set = 1,
                                                   .binding = 1,
                                                   .stages = pom::gfx::ShaderStageFlags::FRAGMENT,
                                               },
                                           },
                                           {});

    // descriptor set
    for (u32 i = 0; i < POM_MAX_FRAMES_IN_FLIGHT; i++) {
        gamestate->lightBuffers[i] = pom::gfx::Buffer::create(pom::gfx::BufferUsage::UNIFORM,
                                                              pom::gfx::BufferMemoryAccess::CPU_WRITE,
                                                              sizeof(LightData));

        gamestate->sceneDescriptorSets[i] = pom::gfx::DescriptorSet::create(gamestate->pipelineLayout, 0);
        gamestate->sceneDescriptorSets[i]->setBuffer(pom::gfx::DescriptorType::UNIFORM_BUFFER,
                                                     0,
                                                     gamestate->cameraBuffers[i]);
        gamestate->sceneDescriptorSets[i]->setBuffer(pom::gfx::DescriptorType::UNIFORM_BUFFER,
                                                     1,
                                                     gamestate->lightBuffers[i]);
        gamestate->sceneDescriptorSets[i]->setTextureView(pom::gfx::DescriptorType::COMBINED_TEXTURE_SAMPLER,
                                                          2,
                                                          gamestate->environment.irradianceMapView);
        gamestate->sceneDescriptorSets[i]->setTextureView(pom::gfx::DescriptorType::COMBINED_TEXTURE_SAMPLER,
                                                          3,
                                                          gamestate->environment.prefilteredEnvMapView);
        gamestate->sceneDescriptorSets[i]->setTextureView(pom::gfx::DescriptorType::COMBINED_TEXTURE_SAMPLER,
                                                          4,
                                                          gamestate->brdfLutTextureView);
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
                             { .location = 1, .format = pom::gfx::Format::R32G32B32_SFLOAT },
                             { .location = 2, .format = pom::gfx::Format::R32G32_SFLOAT }, 
                             { .location = 3, .format = pom::gfx::Format::R32G32B32A32_SFLOAT },},
            }
        },
        gamestate->pipelineLayout);

    initStore(gamestate);
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
                pom::Ref<pom::gfx::Buffer> cameraBuffer = gs->cameraBuffers[frame % POM_MAX_FRAMES_IN_FLIGHT];
                CameraData* data = (CameraData*)cameraBuffer->map();

                data->projection = gs->camera.projection;
                data->view = gs->camera.view;

                cameraBuffer->unmap();
            }

            pom::Ref<pom::gfx::Buffer> lightBuffer = gs->lightBuffers[frame % POM_MAX_FRAMES_IN_FLIGHT];
            f32 f = frame;
            *((LightData*)lightBuffer->map()) = {
                .positions = { 
                    pom::maths::vec4(10, 10, 10, 0),
                    // pom::maths::vec4(cos(f / 10.f + TAU * 0.f / 3.f), sin(f / 10.f + TAU * 0.f / 3.f), 1, 0) * 10.f,
                    pom::maths::vec4(cos(f / 10.f + TAU * 1.f / 3.f), sin(f / 10.f + TAU * 1.f / 3.f), 1, 0) * 10.f,
                    pom::maths::vec4(cos(f / 10.f + TAU * 2.f / 3.f), sin(f / 10.f + TAU * 2.f / 3.f), 1, 0) * 10.f, 
                },
                .colors = { 
                    pom::maths::vec4(300, 300, 300, 0), 
                    // pom::maths::vec4(900, 0, 0, 0), 
                    pom::maths::vec4(0, 900, 0, 0), 
                    pom::maths::vec4(0, 0, 900, 0), 
                },
            };
            lightBuffer->unmap();

            gs->commandBuffer->begin();
            gs->commandBuffer->beginRenderPass(context->getSwapchainRenderPass(), context);
            {

                gs->commandBuffer->setViewport(context->getSwapchainViewport());
                gs->commandBuffer->setScissor({ 0, 0 },
                                              { context->swapchainExtent.width, context->swapchainExtent.height });

                gs->commandBuffer->bindPipeline(gs->pipeline);

                gs->commandBuffer->bindDescriptorSet(pom::gfx::PipelineBindPoint::GRAPHICS,
                                                     gs->pipelineLayout,
                                                     0,
                                                     gs->sceneDescriptorSets[frame % POM_MAX_FRAMES_IN_FLIGHT]);

                for (auto [e, t, r, m, a] :
                     gs->store.view<TransformComponent, RenderComponent, MeshComponent, MaterialComponent>()) {
                    if (e == 0) {
                        a.metalic = gs->metalic;
                        a.roughness = gs->roughness;
                    }
                    // t.transform[3][2] = sin(f / 20.f + e);
                    *(TransformComponent*)r.transformBuffers[frame % POM_MAX_FRAMES_IN_FLIGHT]->map() = t;
                    r.transformBuffers[frame % POM_MAX_FRAMES_IN_FLIGHT]->unmap();
                    *(MaterialComponent*)r.materialBuffers[frame % POM_MAX_FRAMES_IN_FLIGHT]->map() = a;
                    r.materialBuffers[frame % POM_MAX_FRAMES_IN_FLIGHT]->unmap();

                    gs->commandBuffer->bindVertexBuffer(m.meshptr->vertexBuffer);
                    gs->commandBuffer->bindIndexBuffer(m.meshptr->indexBuffer, m.meshptr->indexType);
                    gs->commandBuffer->bindDescriptorSet(pom::gfx::PipelineBindPoint::GRAPHICS,
                                                         gs->pipelineLayout,
                                                         1,
                                                         r.descriptorSets[frame % POM_MAX_FRAMES_IN_FLIGHT]);

                    gs->commandBuffer->drawIndexed(m.meshptr->indexBuffer->getSize() / sizeof(u16));
                }

                // skybox
                gs->skyboxDescriptorSets[frame % POM_MAX_FRAMES_IN_FLIGHT]->setTextureView(
                    pom::gfx::DescriptorType::COMBINED_TEXTURE_SAMPLER,
                    1,
                    gs->tex == 0   ? gs->environment.skyboxView
                    : gs->tex == 1 ? gs->environment.irradianceMapView
                                   : gs->environment.prefilteredEnvMapView);
                gs->commandBuffer->bindPipeline(gs->skyboxPipeline);
                gs->commandBuffer->bindDescriptorSet(pom::gfx::PipelineBindPoint::GRAPHICS,
                                                     gs->skyboxPipelineLayout,
                                                     0,
                                                     gs->skyboxDescriptorSets[frame % POM_MAX_FRAMES_IN_FLIGHT]);
                gs->commandBuffer->setPushConstants(gs->skyboxPipelineLayout,
                                                    pom::gfx::ShaderStageFlags::FRAGMENT,
                                                    sizeof(u32),
                                                    &gs->lod);
                gs->commandBuffer->draw(36);

                // grid
                pom::Ref<pom::gfx::Buffer> gridConfigBuffer = gs->gridConfigBuffers[frame % POM_MAX_FRAMES_IN_FLIGHT];
                auto* config = (GridConfig*)gridConfigBuffer->map();
                *config = gs->gridConfig;
                gridConfigBuffer->unmap();
                gs->commandBuffer->bindPipeline(gs->planePipeline);
                gs->commandBuffer->bindDescriptorSet(pom::gfx::PipelineBindPoint::GRAPHICS,
                                                     gs->planePipelineLayout,
                                                     0,
                                                     gs->planeDescriptorSets[frame % POM_MAX_FRAMES_IN_FLIGHT]);
                // gs->commandBuffer->draw(6);
            }

            gs->commandBuffer->endRenderPass();
            gs->commandBuffer->end();

            gs->commandBuffer->submit();

            pom::Application::get()->getMainWindow().getContext()->present();
        }
    }
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
    } else if (ev->type == pom::InputEventType::KEY_DOWN) {
        if (ev->getKeycode() == pom::Keycode::UP) {
            gs->lod++;
        } else if (ev->getKeycode() == pom::Keycode::DOWN) {
            gs->lod--;
        } else if (ev->getKeycode() == pom::Keycode::LEFT) {
            gs->tex--;
        } else if (ev->getKeycode() == pom::Keycode::RIGHT) {
            gs->tex++;
        }
        gs->lod = gs->lod < 0 ? 0 : gs->lod > 4 ? 4 : gs->lod;
        gs->tex = gs->tex < 0 ? 0 : gs->tex > 2 ? 2 : gs->tex;
        POM_DEBUG("lod: ", gs->lod);
        POM_DEBUG("tex: ", gs->tex);

        if (ev->getKeycode() == pom::Keycode::W) {
            gs->metalic += 0.01;
        } else if (ev->getKeycode() == pom::Keycode::S) {
            gs->metalic -= 0.01;
        } else if (ev->getKeycode() == pom::Keycode::A) {
            gs->roughness -= 0.01;
        } else if (ev->getKeycode() == pom::Keycode::D) {
            gs->roughness += 0.01;
        }
        gs->metalic = std::clamp(gs->metalic, 0.f, 1.f);
        gs->roughness = std::clamp(gs->roughness, 0.f, 1.f);
        POM_DEBUG("metalic: ", gs->metalic);
        POM_DEBUG("roughness: ", gs->roughness);
    }
}

POM_CLIENT_EXPORT void clientUnmount(GameState* gamestate)
{
}

POM_CLIENT_EXPORT void clientEnd(GameState* gamestate)
{
    for (auto [e, r] : gamestate->store.view<RenderComponent>()) {
        for (u32 i = 0; i < POM_MAX_FRAMES_IN_FLIGHT; i++) {
            r.transformBuffers[i].free();
            r.materialBuffers[i].free();
            r.descriptorSets[i].free();
        }
    }
    delete gamestate;
}
