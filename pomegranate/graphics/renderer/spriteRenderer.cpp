#include "pch.hpp"

#include "spriteRenderer.hpp"

#include "scene/ecs/view.hpp"

#include "graphics/gfx/buffer.hpp"
#include "graphics/gfx/descriptorSet.hpp"
#include "graphics/gfx/instance.hpp"
#include "graphics/gfx/pipeline.hpp"
#include "graphics/gfx/pipelineLayout.hpp"
#include "graphics/gfx/shader.hpp"

#include "graphics/gfx/vulkan/instanceVk.hpp"

#include "embed/shaders/spriteRenderer/spriteRenderer_frag_spv.hpp"
#include "embed/shaders/spriteRenderer/spriteRenderer_vert_spv.hpp"

namespace pom {
    static constexpr usize NUM_QUADS = 10000;

    struct PushConstantData {
        maths::mat4 viewProjection;
    };

    struct SpriteVertex {
        maths::vec3 position;
    };

    SpriteRenderer::SpriteRenderer(gfx::Context* context, Store& store) : store(store)
    {
        Ref<gfx::ShaderModule> vertShader = gfx::ShaderModule::create(
            gfx::ShaderStageFlags::VERTEX,
            spriteRenderer_vert_spv_size,
            reinterpret_cast<const u32*>(spriteRenderer_vert_spv_data)); // NOLINT, spv data is always 32 bit aligned.

        Ref<gfx::ShaderModule> fragShader = gfx::ShaderModule::create(
            gfx::ShaderStageFlags::FRAGMENT,
            spriteRenderer_frag_spv_size,
            reinterpret_cast<const u32*>(spriteRenderer_frag_spv_data)); // NOLINT, spv data is always 32 bit aligned.

        pipelineLayout = gfx::PipelineLayout::create(
            {
                {
                    .type = gfx::DescriptorType::UNIFORM_BUFFER,
                    .set = 0,
                    .binding = 0,
                    .stages = gfx::ShaderStageFlags::VERTEX,
                },
            },
            { {
                .stages = gfx::ShaderStageFlags::VERTEX,
                .size = sizeof(PushConstantData),
                .offset = 0,
            } });

        for (u8 i = 0; i < POM_MAX_FRAMES_IN_FLIGHT; i++) {
            uniformBuffers[i] = gfx::Buffer::create(gfx::BufferUsage::UNIFORM,
                                                    gfx::BufferMemoryAccess::CPU_WRITE,
                                                    NUM_QUADS * sizeof(SpriteRendererComponent));
            uniformMappings[i] = static_cast<SpriteRendererComponent*>(uniformBuffers[i]->map());

            descriptorSets[i] = gfx::DescriptorSet::create(pipelineLayout, 0);
            descriptorSets[i]->setBuffer(gfx::DescriptorType::UNIFORM_BUFFER, 0, uniformBuffers[i]);
        }

        Ref<gfx::Shader> shader = gfx::Shader::create({ vertShader, fragShader });

        pipeline = gfx::Pipeline::create(
            {
                .cullMode = gfx::CullMode::NONE,
            },
            shader,
            context->getSwapchainRenderPass(),
            {},
            pipelineLayout);
    }

    void flush()
    {
    }

    void SpriteRenderer::render(gfx::Context* context,
                                Ref<gfx::CommandBuffer> commandBuffer,
                                const maths::mat4& viewProjection)
    {
        const u64 frameIdx
            = dynamic_cast<gfx::InstanceVk*>(gfx::Instance::get())->getCurrentFrameIndex() % POM_MAX_FRAMES_IN_FLIGHT;

        commandBuffer->setViewport(context->getSwapchainViewport());
        commandBuffer->setScissor({ 0, 0 }, context->getSwapchainExtent());

        commandBuffer->bindPipeline(pipeline);
        commandBuffer->bindDescriptorSet(gfx::PipelineBindPoint::GRAPHICS, pipelineLayout, 0, descriptorSets[frameIdx]);
        PushConstantData pcData = {
            .viewProjection = viewProjection,
        };
        commandBuffer->setPushConstants(pipelineLayout,
                                        gfx::ShaderStageFlags::VERTEX,
                                        sizeof(PushConstantData),
                                        &pcData);

        u32 instances = 0;
        for (auto [e, sr] : store.view<SpriteRendererComponent>()) {
            uniformMappings[frameIdx][instances++] = sr; // TODO: memcpy buffers from archetypes

            if (instances > NUM_QUADS) {
                commandBuffer->drawInstanced(6, instances);
            }
        }

        commandBuffer->drawInstanced(6, instances);
    }
} // namespace pom