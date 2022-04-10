#include <pomegranate/pomegranate.hpp>

#include "embed/shader/skybox/equirectangular_to_cubemap_comp_spv.hpp"

#include "embed/shader/ibl/irradiance_comp_spv.hpp"

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
    pom::Ref<pom::gfx::Texture> skyboxTexture;
    pom::Ref<pom::gfx::TextureView> skyboxTextureView;

    pom::Ref<pom::gfx::PipelineLayout> skyboxPipelineLayout;
    pom::Ref<pom::gfx::DescriptorSet> skyboxDescriptorSets[POM_MAX_FRAMES_IN_FLIGHT];
    pom::Ref<pom::gfx::Pipeline> skyboxPipeline;

    pom::Ref<pom::gfx::Texture> irradianceMapTexture;
    pom::Ref<pom::gfx::TextureView> irradianceMapTextureView;
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

pom::Ref<pom::gfx::Texture> equirectangularToCubemap(const pom::Ref<pom::gfx::Texture>& equirectangular,
                                                     const pom::Ref<pom::gfx::TextureView>& equirectangularView)
{
    POM_ASSERT(equirectangular->getWidth() == 2 * equirectangular->getHeight(), "equirectangular image must be 2:1");
    pom::Ref<pom::gfx::Texture> cubemap = pom::gfx::Texture::create(
        {
            .type = pom::gfx::TextureType::IMAGE_2D,
            .usage = pom::gfx::TextureUsage::SAMPLED | pom::gfx::TextureUsage::STORAGE,
            .format = pom::gfx::Format::R32G32B32A32_SFLOAT,
            .arrayLayers = 6,
            .cubeCompatable = true,
        },
        equirectangular->getWidth() / 4,
        equirectangular->getWidth() / 4);

    pom::Ref<pom::gfx::TextureView> cubemapArrayViews[6];
    for (u8 i = 0; i < 6; ++i) {
        cubemapArrayViews[i] = pom::gfx::TextureView::create(cubemap,
                                                             {
                                                                 .type = pom::gfx::TextureViewType::VIEW_2D,
                                                                 .format = pom::gfx::Format::R32G32B32A32_SFLOAT,
                                                                 .subresourceRange = { .baseArrayLayer = i },
                                                             });
    }

    auto converterComputeShader = pom::gfx::ShaderModule::create(
        pom::gfx::ShaderStageFlags::COMPUTE,
        equirectangular_to_cubemap_comp_spv_size,
        reinterpret_cast<const u32*>(equirectangular_to_cubemap_comp_spv_data)); // NOLINT
    auto converterShader = pom::gfx::Shader::create({ converterComputeShader });
    auto converterPipelineLayout
        = pom::gfx::PipelineLayout::create(1,
                                           {
                                               {
                                                   .type = pom::gfx::DescriptorType::STORAGE_IMAGE,
                                                   .set = 0,
                                                   .binding = 0,
                                                   .stages = pom::gfx::ShaderStageFlags::COMPUTE,
                                               },
                                               {
                                                   .type = pom::gfx::DescriptorType::STORAGE_IMAGE,
                                                   .set = 0,
                                                   .binding = 1,
                                                   .stages = pom::gfx::ShaderStageFlags::COMPUTE,
                                                   .count = 6,
                                               },
                                           },
                                           {});

    auto converterPipeline = pom::gfx::Pipeline::createCompute(converterShader, converterPipelineLayout);
    auto converterDescriptorSet = pom::gfx::DescriptorSet::create(converterPipelineLayout, 0);
    converterDescriptorSet->setTextureView(pom::gfx::DescriptorType::STORAGE_IMAGE, 0, equirectangularView);
    converterDescriptorSet->setTextureViews(pom::gfx::DescriptorType::STORAGE_IMAGE, 1, cubemapArrayViews, 6);

    auto commandBuffer = pom::gfx::CommandBuffer::create(pom::gfx::CommandBufferSpecialization::GENERAL, 1);
    commandBuffer->begin();
    commandBuffer->bindPipeline(converterPipeline);
    commandBuffer->bindDescriptorSet(pom::gfx::PipelineBindPoint::COMPUTE,
                                     converterPipelineLayout,
                                     0,
                                     converterDescriptorSet);
    commandBuffer->dispatch(equirectangular->getWidth() / 4 / 16, equirectangular->getWidth() / 4 / 16, 6);

    commandBuffer->end();
    commandBuffer->submit();
    vkDeviceWaitIdle(dynamic_cast<pom::gfx::InstanceVk*>(pom::gfx::Instance::get())->getVkDevice());
    return cubemap;
}

pom::Ref<pom::gfx::Texture> equirectangularToIrradianceMap(const pom::Ref<pom::gfx::Texture>& equirectangular,
                                                           const pom::Ref<pom::gfx::TextureView>& equirectangularView)
{
    POM_ASSERT(equirectangular->getWidth() == 2 * equirectangular->getHeight(), "equirectangular image must be 2:1");
    pom::Ref<pom::gfx::Texture> cubemap = pom::gfx::Texture::create(
        {
            .type = pom::gfx::TextureType::IMAGE_2D,
            .usage = pom::gfx::TextureUsage::SAMPLED | pom::gfx::TextureUsage::STORAGE,
            .format = pom::gfx::Format::R32G32B32A32_SFLOAT,
            .arrayLayers = 6,
            .cubeCompatable = true,
        },
        equirectangular->getWidth() / 4,
        equirectangular->getWidth() / 4);

    pom::Ref<pom::gfx::TextureView> cubemapArrayViews[6];
    for (u8 i = 0; i < 6; ++i) {
        cubemapArrayViews[i] = pom::gfx::TextureView::create(cubemap,
                                                             {
                                                                 .type = pom::gfx::TextureViewType::VIEW_2D,
                                                                 .format = pom::gfx::Format::R32G32B32A32_SFLOAT,
                                                                 .subresourceRange = { .baseArrayLayer = i },
                                                             });
    }

    auto converterComputeShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStageFlags::COMPUTE,
                                         irradiance_comp_spv_size,
                                         reinterpret_cast<const u32*>(irradiance_comp_spv_data)); // NOLINT
    auto converterShader = pom::gfx::Shader::create({ converterComputeShader });
    auto converterPipelineLayout
        = pom::gfx::PipelineLayout::create(1,
                                           {
                                               {
                                                   .type = pom::gfx::DescriptorType::STORAGE_IMAGE,
                                                   .set = 0,
                                                   .binding = 0,
                                                   .stages = pom::gfx::ShaderStageFlags::COMPUTE,
                                               },
                                               {
                                                   .type = pom::gfx::DescriptorType::STORAGE_IMAGE,
                                                   .set = 0,
                                                   .binding = 1,
                                                   .stages = pom::gfx::ShaderStageFlags::COMPUTE,
                                                   .count = 6,
                                               },
                                           },
                                           {});

    auto converterPipeline = pom::gfx::Pipeline::createCompute(converterShader, converterPipelineLayout);
    auto converterDescriptorSet = pom::gfx::DescriptorSet::create(converterPipelineLayout, 0);
    converterDescriptorSet->setTextureView(pom::gfx::DescriptorType::STORAGE_IMAGE, 0, equirectangularView);
    converterDescriptorSet->setTextureViews(pom::gfx::DescriptorType::STORAGE_IMAGE, 1, cubemapArrayViews, 6);

    auto commandBuffer = pom::gfx::CommandBuffer::create(pom::gfx::CommandBufferSpecialization::GENERAL, 1);
    commandBuffer->begin();
    commandBuffer->bindPipeline(converterPipeline);
    commandBuffer->bindDescriptorSet(pom::gfx::PipelineBindPoint::COMPUTE,
                                     converterPipelineLayout,
                                     0,
                                     converterDescriptorSet);
    commandBuffer->dispatch(equirectangular->getWidth() / 4 / 16, equirectangular->getWidth() / 4 / 16, 6);
    commandBuffer->end();
    commandBuffer->submit();
    vkDeviceWaitIdle(dynamic_cast<pom::gfx::InstanceVk*>(pom::gfx::Instance::get())->getVkDevice());

    return cubemap;
}

void initSkybox(GameState* gamestate)
{
    // my computer isn't great so we use 2k to not crash
    i32 width, height, channels;
    const f32* pixels = stbi_loadf((pom::getAssetPath() + "kiara_1_dawn_2k.hdr").c_str(),
                                   &width,
                                   &height,
                                   &channels,
                                   4); // NOTE: my computer doesn't do 3 channel stuff annoyingly.

    const usize textureSize = (usize)width * height * 4 * sizeof(f32);

    auto equirectangularMap = pom::gfx::Texture::create(
        {
            .type = pom::gfx::TextureType::IMAGE_2D,
            .usage
            = pom::gfx::TextureUsage::SAMPLED | pom::gfx::TextureUsage::TRANSFER_DST | pom::gfx::TextureUsage::STORAGE,
            .format = pom::gfx::Format::R32G32B32A32_SFLOAT,
        },
        width,
        height,
        1,
        pixels,
        0,
        textureSize);

    auto equirectangularMapView = pom::gfx::TextureView::create(equirectangularMap,
                                                                {
                                                                    .type = pom::gfx::TextureViewType::VIEW_2D,
                                                                    .format = pom::gfx::Format::R32G32B32A32_SFLOAT,
                                                                });

    gamestate->skyboxTexture = equirectangularToCubemap(equirectangularMap, equirectangularMapView);
    gamestate->skyboxTextureView = pom::gfx::TextureView::create(gamestate->skyboxTexture,
                                                                 {
                                                                     .type = pom::gfx::TextureViewType::CUBE,
                                                                     .format = pom::gfx::Format::R32G32B32A32_SFLOAT,
                                                                     .subresourceRange = { .layerCount = 6 },
                                                                 });
    gamestate->irradianceMapTexture = equirectangularToIrradianceMap(equirectangularMap, equirectangularMapView);
    gamestate->irradianceMapTextureView
        = pom::gfx::TextureView::create(gamestate->irradianceMapTexture,
                                        {
                                            .type = pom::gfx::TextureViewType::CUBE,
                                            .format = pom::gfx::Format::R32G32B32A32_SFLOAT,
                                            .subresourceRange = { .layerCount = 6 },
                                        });

    pom::Ref<pom::gfx::ShaderModule> skyboxVertShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStageFlags::VERTEX,
                                         skybox_vert_spv_size,
                                         reinterpret_cast<const u32*>(skybox_vert_spv_data)); // NOLINT

    pom::Ref<pom::gfx::ShaderModule> skyboxFragShader
        = pom::gfx::ShaderModule::create(pom::gfx::ShaderStageFlags::FRAGMENT,
                                         skybox_frag_spv_size,
                                         reinterpret_cast<const u32*>(skybox_frag_spv_data)); // NOLINT

    pom::Ref<pom::gfx::Shader> skyboxShader = pom::gfx::Shader::create({ skyboxVertShader, skyboxFragShader });

    gamestate->skyboxPipelineLayout
        = pom::gfx::PipelineLayout::create(POM_MAX_FRAMES_IN_FLIGHT,
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
                                           {});

    for (u32 i = 0; i < POM_MAX_FRAMES_IN_FLIGHT; i++) {
        gamestate->skyboxDescriptorSets[i] = pom::gfx::DescriptorSet::create(gamestate->skyboxPipelineLayout, 0);
        gamestate->skyboxDescriptorSets[i]->setBuffer(pom::gfx::DescriptorType::UNIFORM_BUFFER,
                                                      0,
                                                      gamestate->cameraBuffers[i]);
        gamestate->skyboxDescriptorSets[i]->setTextureView(pom::gfx::DescriptorType::COMBINED_TEXTURE_SAMPLER,
                                                           1,
                                                           gamestate->skyboxTextureView);
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
    const i32 n = 10;
    for (i32 x = -n / 2; x < n / 2; x++) {
        for (i32 y = -n / 2; y < n / 2; y++) {
            auto e = gamestate->store.createEntity();
            gamestate->store.addComponent<TransformComponent>(e) = {
                pom::maths::mat4::translate({ (f32)x * 2.3f, (f32)y * 2.3f, 0.f }),
            };
            gamestate->store.addComponent<MeshComponent>(e) = {
                .meshptr = &gamestate->sphereMesh,
            };
            gamestate->store.addComponent<MaterialComponent>(e) = {
                .albedo = pom::maths::vec3(0.7, 0.7, 0.7), // pom::maths::vec3((x + 5) * 17, 1, (y + 5) * 17),
                .metalic = (f32)(x + (n / 2)) / n,
                .roughness = (f32)(y + (n / 2) - 0.05f) / n + 0.05f,
            };
            auto& r = gamestate->store.addComponent<RenderComponent>(e);

            for (u32 i = 0; i < POM_MAX_FRAMES_IN_FLIGHT; i++) {
                r.transformBuffers[i] = pom::gfx::Buffer::create(pom::gfx::BufferUsage::UNIFORM,
                                                                 pom::gfx::BufferMemoryAccess::CPU_WRITE,
                                                                 sizeof(TransformComponent));
                r.descriptorSets[i] = pom::gfx::DescriptorSet::create(gamestate->pipelineLayout, 1);
                r.descriptorSets[i]->setBuffer(pom::gfx::DescriptorType::UNIFORM_BUFFER, 0, r.transformBuffers[i]);

                r.materialBuffers[i] = pom::gfx::Buffer::create(pom::gfx::BufferUsage::UNIFORM,
                                                                pom::gfx::BufferMemoryAccess::CPU_WRITE,
                                                                sizeof(MaterialComponent));
                r.descriptorSets[i]->setBuffer(pom::gfx::DescriptorType::UNIFORM_BUFFER, 1, r.materialBuffers[i]);
            }
        }
    }
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
                                                          gamestate->irradianceMapTextureView);
    }

    gamestate->pipeline = pom::gfx::Pipeline::create(
        {},
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
                    pom::maths::vec4(cos(f / 10.f + TAU * 0.f / 3.f), sin(f / 10.f + TAU * 0.f / 3.f), 1, 0) * 10.f,
                    pom::maths::vec4(cos(f / 10.f + TAU * 1.f / 3.f), sin(f / 10.f + TAU * 1.f / 3.f), 1, 0) * 10.f,
                    pom::maths::vec4(cos(f / 10.f + TAU * 2.f / 3.f), sin(f / 10.f + TAU * 2.f / 3.f), 1, 0) * 10.f, 
                },
                .colors = { 
                    pom::maths::vec4(100, 0, 0, 0), 
                    pom::maths::vec4(0, 100, 0, 0), 
                    pom::maths::vec4(0, 0, 100, 0), 
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
                gs->commandBuffer->bindPipeline(gs->skyboxPipeline);
                gs->commandBuffer->bindDescriptorSet(pom::gfx::PipelineBindPoint::GRAPHICS,
                                                     gs->skyboxPipelineLayout,
                                                     0,
                                                     gs->skyboxDescriptorSets[frame % POM_MAX_FRAMES_IN_FLIGHT]);
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
