#include "assets/assets.hpp"
#include "pch.hpp"

#include "environment.hpp"

#include "gfx/commandBuffer.hpp"
#include "gfx/descriptorSet.hpp"
#include "gfx/pipeline.hpp"
#include "gfx/pipelineLayout.hpp"
#include "gfx/shader.hpp"

#include "embed/shaders/ibl/equirectangular_to_cubemap_comp_spv.hpp"
#include "embed/shaders/ibl/irradiance_comp_spv.hpp"
#include "embed/shaders/ibl/prefiltered_envmap_comp_spv.hpp"

#include <filesystem>

namespace pom {
    const u8 PREFILTER_MIPS = 5;

    const char* PADDING
        = "666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666"
          "666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666"
          "666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666"
          "666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666"
          "66666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666";

    Ref<gfx::Texture> cubemapFromEquirectangular(Ref<gfx::Texture> equirectangular,
                                                 Ref<gfx::TextureView> equirectangularView)
    {
        POM_PROFILE_FUNCTION();

        POM_ASSERT(equirectangular->getWidth() == 2 * equirectangular->getHeight(),
                   "equirectangular image must be 2:1");
        Ref<gfx::Texture> cubemap = gfx::Texture::create(
            {
                .type = gfx::TextureType::IMAGE_2D,
                .usage = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::STORAGE | gfx::TextureUsage::TRANSFER_DST,
                .format = equirectangularView->getFormat(),
                .arrayLayers = 6,
                .cubeCompatable = true,
            },
            equirectangular->getWidth() / 4,
            equirectangular->getWidth() / 4);

        Ref<gfx::TextureView> cubemapArrayViews[6];
        for (u8 i = 0; i < 6; ++i) {
            cubemapArrayViews[i] = gfx::TextureView::create(cubemap,
                                                            {
                                                                .type = gfx::TextureViewType::VIEW_2D,
                                                                .format = equirectangularView->getFormat(),
                                                                .subresourceRange = { .baseArrayLayer = i },
                                                            });
        }

        auto converterComputeShader = gfx::ShaderModule::create(
            gfx::ShaderStageFlags::COMPUTE,
            equirectangular_to_cubemap_comp_spv_size,
            reinterpret_cast<const u32*>(equirectangular_to_cubemap_comp_spv_data)); // NOLINT
        auto converterShader = gfx::Shader::create({ converterComputeShader });
        auto converterPipelineLayout = gfx::PipelineLayout::create(1,
                                                                   {
                                                                       {
                                                                           .type = gfx::DescriptorType::STORAGE_IMAGE,
                                                                           .set = 0,
                                                                           .binding = 0,
                                                                           .stages = gfx::ShaderStageFlags::COMPUTE,
                                                                       },
                                                                       {
                                                                           .type = gfx::DescriptorType::STORAGE_IMAGE,
                                                                           .set = 0,
                                                                           .binding = 1,
                                                                           .stages = gfx::ShaderStageFlags::COMPUTE,
                                                                           .count = 6,
                                                                       },
                                                                   },
                                                                   {});

        auto converterPipeline = gfx::Pipeline::createCompute(converterShader, converterPipelineLayout);
        auto converterDescriptorSet = gfx::DescriptorSet::create(converterPipelineLayout, 0);
        converterDescriptorSet->setTextureView(gfx::DescriptorType::STORAGE_IMAGE, 0, equirectangularView);
        converterDescriptorSet->setTextureViews(gfx::DescriptorType::STORAGE_IMAGE, 1, cubemapArrayViews, 6);

        auto commandBuffer = gfx::CommandBuffer::create(gfx::CommandBufferSpecialization::GENERAL, 1);
        commandBuffer->begin();
        commandBuffer->bindPipeline(converterPipeline);
        commandBuffer->bindDescriptorSet(gfx::PipelineBindPoint::COMPUTE,
                                         converterPipelineLayout,
                                         0,
                                         converterDescriptorSet);
        commandBuffer->dispatch(equirectangular->getWidth() / 4 / 16, equirectangular->getWidth() / 4 / 16, 6);

        commandBuffer->end();
        commandBuffer->submit();
        return cubemap;
    }

    Ref<gfx::Texture> irradianceMapFromEquirectangular(Ref<gfx::Texture> equirectangular,
                                                       Ref<gfx::TextureView> equirectangularView)
    {
        POM_PROFILE_FUNCTION();

        POM_ASSERT(equirectangular->getWidth() == 2 * equirectangular->getHeight(),
                   "equirectangular image must be 2:1");
        Ref<gfx::Texture> cubemap = gfx::Texture::create(
            {
                .type = gfx::TextureType::IMAGE_2D,
                .usage = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::STORAGE | gfx::TextureUsage::TRANSFER_DST,
                .format = equirectangularView->getFormat(),
                .arrayLayers = 6,
                .cubeCompatable = true,
            },
            equirectangular->getWidth() / 4,
            equirectangular->getWidth() / 4);

        Ref<gfx::TextureView> cubemapArrayViews[6];
        for (u8 i = 0; i < 6; ++i) {
            cubemapArrayViews[i] = gfx::TextureView::create(cubemap,
                                                            {
                                                                .type = gfx::TextureViewType::VIEW_2D,
                                                                .format = equirectangularView->getFormat(),
                                                                .subresourceRange = { .baseArrayLayer = i },
                                                            });
        }

        auto converterComputeShader
            = gfx::ShaderModule::create(gfx::ShaderStageFlags::COMPUTE,
                                        irradiance_comp_spv_size,
                                        reinterpret_cast<const u32*>(irradiance_comp_spv_data)); // NOLINT
        auto converterShader = gfx::Shader::create({ converterComputeShader });
        auto converterPipelineLayout
            = gfx::PipelineLayout::create(1,
                                          {
                                              {
                                                  .type = gfx::DescriptorType::STORAGE_IMAGE,
                                                  .set = 0,
                                                  .binding = 0,
                                                  .stages = gfx::ShaderStageFlags::COMPUTE,
                                              },
                                              {
                                                  .type = gfx::DescriptorType::STORAGE_IMAGE,
                                                  .set = 0,
                                                  .binding = 1,
                                                  .stages = gfx::ShaderStageFlags::COMPUTE,
                                                  .count = 6,
                                              },
                                          },
                                          { { .stages = gfx::ShaderStageFlags::COMPUTE, .size = sizeof(f32) * 2 } });

        auto converterPipeline = gfx::Pipeline::createCompute(converterShader, converterPipelineLayout);
        auto converterDescriptorSet = gfx::DescriptorSet::create(converterPipelineLayout, 0);
        converterDescriptorSet->setTextureView(gfx::DescriptorType::STORAGE_IMAGE, 0, equirectangularView);
        converterDescriptorSet->setTextureViews(gfx::DescriptorType::STORAGE_IMAGE, 1, cubemapArrayViews, 6);

        auto commandBuffer = gfx::CommandBuffer::create(gfx::CommandBufferSpecialization::GENERAL, 1);
        commandBuffer->begin();
        commandBuffer->bindPipeline(converterPipeline);
        const f32 pc[2] = { 0.05f, 0.05f };
        commandBuffer->setPushConstants(converterPipelineLayout, gfx::ShaderStageFlags::COMPUTE, sizeof(f32) * 2, pc);
        commandBuffer->bindDescriptorSet(gfx::PipelineBindPoint::COMPUTE,
                                         converterPipelineLayout,
                                         0,
                                         converterDescriptorSet);
        commandBuffer->dispatch(equirectangular->getWidth() / 4 / 16, equirectangular->getWidth() / 4 / 16, 6);
        commandBuffer->end();
        commandBuffer->submit();

        return cubemap;
    }

    Ref<gfx::Texture> prefilteredEnvMapFromEquirectangular(Ref<gfx::Texture> equirectangular,
                                                           Ref<gfx::TextureView> equirectangularView,
                                                           u8 mips)
    {
        POM_PROFILE_FUNCTION();

        POM_ASSERT(equirectangular->getWidth() == 2 * equirectangular->getHeight(),
                   "equirectangular image must be 2:1");
        POM_ASSERT(mips > 1, "prefiltered env map must have more than 1 mip");

        Ref<gfx::Texture> cubemap = gfx::Texture::create(
            {
                .type = gfx::TextureType::IMAGE_2D,
                .usage = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::STORAGE | gfx::TextureUsage::TRANSFER_DST,
                .format = gfx::Format::R32G32B32A32_SFLOAT,
                .mipLevels = mips,
                .arrayLayers = 6,
                .cubeCompatable = true,
            },
            equirectangular->getWidth() / 4,
            equirectangular->getWidth() / 4);

        Ref<gfx::TextureView> cubemapArrayViews[6 * mips];
        for (u8 m = 0; m < mips; ++m) {
            for (u8 i = 0; i < 6; ++i) {
                cubemapArrayViews[i + (m * 6)]
                = gfx::TextureView::create(cubemap,
                                                {
                                                    .type = gfx::TextureViewType::VIEW_2D,
                                                    .format = gfx::Format::R32G32B32A32_SFLOAT,
                                                    .subresourceRange = { .baseArrayLayer = i, .baseMip = m, },
                                                });
            }
        }

        auto converterComputeShader
            = gfx::ShaderModule::create(gfx::ShaderStageFlags::COMPUTE,
                                        prefiltered_envmap_comp_spv_size,
                                        reinterpret_cast<const u32*>(prefiltered_envmap_comp_spv_data)); // NOLINT
        auto converterShader = gfx::Shader::create({ converterComputeShader });
        auto converterPipelineLayout = gfx::PipelineLayout::create(1,
                                                                   {
                                                                       {
                                                                           .type = gfx::DescriptorType::STORAGE_IMAGE,
                                                                           .set = 0,
                                                                           .binding = 0,
                                                                           .stages = gfx::ShaderStageFlags::COMPUTE,
                                                                       },
                                                                       {
                                                                           .type = gfx::DescriptorType::STORAGE_IMAGE,
                                                                           .set = 0,
                                                                           .binding = 1,
                                                                           .stages = gfx::ShaderStageFlags::COMPUTE,
                                                                           .count = (u32)6 * mips,
                                                                       },
                                                                   },
                                                                   { {
                                                                       .stages = gfx::ShaderStageFlags::COMPUTE,
                                                                       .size = sizeof(f32) + sizeof(u32) + sizeof(u32),
                                                                       .offset = 0,
                                                                   } });

        auto converterPipeline = gfx::Pipeline::createCompute(converterShader, converterPipelineLayout);
        auto converterDescriptorSet = gfx::DescriptorSet::create(converterPipelineLayout, 0);
        converterDescriptorSet->setTextureView(gfx::DescriptorType::STORAGE_IMAGE, 0, equirectangularView);
        converterDescriptorSet->setTextureViews(gfx::DescriptorType::STORAGE_IMAGE, 1, cubemapArrayViews, 6 * mips);

        auto commandBuffer = gfx::CommandBuffer::create(gfx::CommandBufferSpecialization::GENERAL, 1);
        commandBuffer->begin();
        commandBuffer->bindPipeline(converterPipeline);
        commandBuffer->bindDescriptorSet(gfx::PipelineBindPoint::COMPUTE,
                                         converterPipelineLayout,
                                         0,
                                         converterDescriptorSet);

        for (u32 m = 0; m < mips; ++m) {
            struct PC {
                f32 roughness;
                u32 mip;
                u32 sampleCount;
            };
            PC pc = { (f32)m / (f32)(mips - 1), m, 1024 };
            commandBuffer->setPushConstants(converterPipelineLayout, gfx::ShaderStageFlags::COMPUTE, sizeof(PC), &pc);
            commandBuffer->dispatch((equirectangular->getWidth() / 4 / pow(2, m)) / 16,
                                    (equirectangular->getWidth() / 4 / pow(2, m)) / 16,
                                    6);
        }
        commandBuffer->end();
        commandBuffer->submit();

        return cubemap;
    }

    IBLEnvironment createEnviornmentFromEquirectangular(Ref<gfx::Texture> equirectangular,
                                                        Ref<gfx::TextureView> equirectangularView)
    {
        POM_PROFILE_FUNCTION();

        auto skybox = cubemapFromEquirectangular(equirectangular, equirectangularView);
        auto skyboxView = gfx::TextureView::create(skybox,
                                                   {
                                                       .type = gfx::TextureViewType::CUBE,
                                                       .format = equirectangularView->getFormat(),
                                                       .subresourceRange = { .layerCount = 6 },
                                                   });

        auto irradianceMap = irradianceMapFromEquirectangular(equirectangular, equirectangularView);
        auto irradianceMapView = gfx::TextureView::create(irradianceMap,
                                                          {
                                                              .type = gfx::TextureViewType::CUBE,
                                                              .format = equirectangularView->getFormat(),
                                                              .subresourceRange = { .layerCount = 6 },
                                                          });

        auto prefilteredEnvMap
            = prefilteredEnvMapFromEquirectangular(equirectangular, equirectangularView, PREFILTER_MIPS);
        auto prefilteredEnvMapView
            = gfx::TextureView::create(prefilteredEnvMap,
                                       { .type = gfx::TextureViewType::CUBE,
                                         .format = equirectangularView->getFormat(),
                                         .subresourceRange = { .layerCount = 6, .mipCount = PREFILTER_MIPS } });

        return {
            .skybox = skybox,
            .skyboxView = skyboxView,

            .irradianceMap = irradianceMap,
            .irradianceMapView = irradianceMapView,

            .prefilteredEnvMap = prefilteredEnvMap,
            .prefilteredEnvMapView = prefilteredEnvMapView,
        };
    }

    IBLEnvironment loadIBLEnvironment(std::string equirectangularMapPath, bool cache)
    {
        POM_PROFILE_FUNCTION();
        const std::filesystem::path cacheFile = getCachePath() + "iblenv_" + equirectangularMapPath + ".cache";

        IBLEnvironment environment;

        if (cache && std::filesystem::exists(cacheFile)) {
            std::ifstream file(cacheFile, std::ios::binary);
            gfx::Format format;
            file.read(reinterpret_cast<char*>(&format), sizeof(format));
            maths::uvec3 extent;
            file.read(reinterpret_cast<char*>(&extent), sizeof(extent));
            u32 mips;
            file.read(reinterpret_cast<char*>(&mips), sizeof(mips));

            auto f = [](gfx::Format format,
                        maths::uvec3 extent,
                        std::ifstream& file,
                        u8 mips,
                        Ref<gfx::Texture>& img,
                        Ref<gfx::TextureView>& imgView) {
                usize imgSize;
                file.read(reinterpret_cast<char*>(&imgSize), sizeof(imgSize));
                void* data = malloc(imgSize);

                file.read(reinterpret_cast<char*>(data), (long long)imgSize);
                img = mips == 1 ? gfx::Texture::create(
                          {
                              .type = gfx::TextureType::IMAGE_2D,
                              .usage = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::STORAGE
                                       | gfx::TextureUsage::TRANSFER_DST,
                              .format = format,
                              .mipLevels = mips,
                              .arrayLayers = 6,
                              .cubeCompatable = true,
                          },
                          extent.x / 4,
                          extent.x / 4,
                          1,
                          data,
                          0,
                          imgSize)
                                : gfx::Texture::createDirect(
                                    {
                                        .type = gfx::TextureType::IMAGE_2D,
                                        .usage = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::STORAGE
                                                 | gfx::TextureUsage::TRANSFER_DST,
                                        .format = format,
                                        .mipLevels = mips,
                                        .arrayLayers = 6,
                                        .cubeCompatable = true,
                                    },
                                    extent.x / 4,
                                    extent.x / 4,
                                    1,
                                    imgSize,
                                    data);
                imgView = gfx::TextureView::create(img,
                                                   {
                                                       .type = gfx::TextureViewType::CUBE,
                                                       .format = format,
                                                       .subresourceRange = { .layerCount = 6, .mipCount = mips },
                                                   });

                free(data);
            };
            f(format, extent, file, 1, environment.skybox, environment.skyboxView);
            f(format, extent, file, 1, environment.irradianceMap, environment.irradianceMapView);
            f(format, extent, file, mips, environment.prefilteredEnvMap, environment.prefilteredEnvMapView);

            file.close();

            return environment;
        }

        i32 width, height, channels;
        const f32* pixels = stbi_loadf((getAssetPath() + equirectangularMapPath).c_str(),
                                       &width,
                                       &height,
                                       &channels,
                                       4); // NOTE: my computer doesn't do 3 channel stuff annoyingly.

        const usize textureSize = (usize)width * height * 4 * sizeof(f32);

        auto equirectangularMap = gfx::Texture::create(
            {
                .type = gfx::TextureType::IMAGE_2D,
                .usage = gfx::TextureUsage::SAMPLED | gfx::TextureUsage::TRANSFER_DST | gfx::TextureUsage::STORAGE,
                .format = gfx::Format::R32G32B32A32_SFLOAT,
            },
            width,
            height,
            1,
            pixels,
            0,
            textureSize);

        auto equirectangularMapView = gfx::TextureView::create(equirectangularMap,
                                                               {
                                                                   .type = gfx::TextureViewType::VIEW_2D,
                                                                   .format = gfx::Format::R32G32B32A32_SFLOAT,
                                                               });

        environment = createEnviornmentFromEquirectangular(equirectangularMap, equirectangularMapView);

        if (cache) {
            std::filesystem::create_directory(cacheFile.parent_path());
            std::ofstream file(cacheFile, std::ios::out | std::ios::binary);
            gfx::Format format = equirectangularMapView->getFormat();
            maths::uvec3 extent = equirectangularMap->getExtent();
            u32 mips = environment.prefilteredEnvMap->getMipCount();
            file.write(reinterpret_cast<char*>(&format), sizeof(format));
            file.write(reinterpret_cast<char*>(&extent), sizeof(extent));
            file.write(reinterpret_cast<char*>(&mips), sizeof(mips));

            {
                usize imgSize = environment.skybox->getSize();
                file.write(reinterpret_cast<char*>(&imgSize), sizeof(imgSize));
                char* data = (char*)environment.skybox->map();
                file.write(data, (long long)environment.skybox->getSize());
                environment.skybox->unmap();
            }
            {
                usize imgSize = environment.irradianceMap->getSize();
                file.write(reinterpret_cast<char*>(&imgSize), sizeof(imgSize));
                char* data = (char*)environment.irradianceMap->map();
                file.write(data, (long long)environment.irradianceMap->getSize());
                environment.irradianceMap->unmap();
            }
            {
                usize imgSize = environment.prefilteredEnvMap->getSize();
                file.write(reinterpret_cast<char*>(&imgSize), sizeof(imgSize));
                char* data = (char*)environment.prefilteredEnvMap->map();
                file.write(data, (long long)environment.prefilteredEnvMap->getSize());
                environment.prefilteredEnvMap->unmap();
            }
        }

        return environment;
    }

} // namespace pom