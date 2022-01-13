#include "pch.hpp"

#include "pipeline.hpp"

#include "vulkan/pipelineVk.hpp"

#include "instance.hpp"
#include "pipelineLayout.hpp"
#include "renderPass.hpp"
#include "shader.hpp"

#include "vulkan/instanceVk.hpp"
#include "vulkan/pipelineLayoutVk.hpp"
#include "vulkan/renderPassVk.hpp"
#include "vulkan/shaderVk.hpp"

namespace pom::gfx {
    Ref<Pipeline> Pipeline::create(GraphicsPipelineState state,
                                   const Ref<Shader>& shader,
                                   const Ref<RenderPass>& renderPass,
                                   std::initializer_list<VertexBinding> vertexBindings,
                                   const Ref<PipelineLayout>& pipelineLayout)
    {
        POM_PROFILE_FUNCTION();
        switch (Instance::get()->getAPI()) {
        case GraphicsAPI::VULKAN: {
            POM_ASSERT(renderPass->getAPI() == GraphicsAPI::VULKAN, "mismatched api");
            POM_ASSERT(shader->getAPI() == GraphicsAPI::VULKAN, "mismatched api");
            if (pipelineLayout)
                POM_ASSERT(pipelineLayout->getAPI() == GraphicsAPI::VULKAN, "mismatched api");

            auto* instance = dynamic_cast<InstanceVk*>(Instance::get());

            return Ref<Pipeline>(new PipelineVk(instance,
                                                renderPass.dynCast<RenderPassVk>(),
                                                shader.dynCast<ShaderVk>(),
                                                state,
                                                vertexBindings,
                                                pipelineLayout ? pipelineLayout.dynCast<PipelineLayoutVk>()
                                                               : instance->emptyPipelineLayout));
        }
        }
    }
} // namespace pom::gfx