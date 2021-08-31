#pragma once

#include "base.hpp"

#include "../pipeline.hpp"

namespace pom::gfx {
    class InstanceVk;
    class RenderPassVk;
    class ShaderVk;
    class PipelineLayoutVk;

    /// Vulkan pipeline, should be created implicitly through `Pipeline::create`. Contains the vulkan pipeline handle.
    class POM_API PipelineVk final : public Pipeline {
    public:
        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        PipelineVk(InstanceVk* instance,
                   RenderPassVk* renderPass,
                   ShaderVk* shader,
                   GraphicsPipelineState state,
                   std::initializer_list<VertexBinding> vertexBindings,
                   PipelineLayoutVk* pipelineLayout);

        ~PipelineVk() final;

        VkPipeline getHandle()
        {
            return pipeline;
        }

    private:
        friend class DescriptorSetVk;

        POM_NOCOPY(PipelineVk);

        InstanceVk* instance;

        VkPipeline pipeline = VK_NULL_HANDLE;
    };
} // namespace pom::gfx