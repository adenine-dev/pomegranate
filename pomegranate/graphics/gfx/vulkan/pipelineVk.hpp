#pragma once

#include "base.hpp"

#include "../pipeline.hpp"

namespace pom::gfx {
    class InstanceVk;
    class RenderPassVk;
    class ShaderVk;

    class POM_API PipelineVk final : public Pipeline {
    public:
        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        PipelineVk(InstanceVk* instance,
                   RenderPassVk* renderPass,
                   ShaderVk* shader,
                   PipelineState state,
                   std::initializer_list<VertexBinding> vertexBindings,
                   VkPipelineLayout pipelineLayout);

        ~PipelineVk() final;

        VkPipeline getHandle()
        {
            return pipeline;
        }

    private:
        POM_NOCOPY(PipelineVk);

        InstanceVk* instance;

        VkPipeline pipeline;
    };
} // namespace pom::gfx