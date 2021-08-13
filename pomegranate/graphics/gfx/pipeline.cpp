#include "pch.hpp"

#include "pipeline.hpp"

#include "vulkan/pipelineVk.hpp"

#include "instance.hpp"
#include "renderPass.hpp"
#include "shader.hpp"

#include "vulkan/instanceVk.hpp"
#include "vulkan/renderPassVk.hpp"
#include "vulkan/shaderVk.hpp"

namespace pom::gfx {
    Pipeline* Pipeline::create(PipelineState state,
                               Shader* shader,
                               RenderPass* renderPass,
                               std::initializer_list<VertexBinding> vertexBindings,
                               VkPipelineLayout pipelineLayout)
    {
        switch (Instance::get()->getAPI()) {
        case GraphicsAPI::VULKAN: {
            POM_ASSERT(renderPass->getAPI() == GraphicsAPI::VULKAN, "mismatched api");
            POM_ASSERT(shader->getAPI() == GraphicsAPI::VULKAN, "mismatched api");
            return new PipelineVk(dynamic_cast<InstanceVk*>(Instance::get()),
                                  dynamic_cast<RenderPassVk*>(renderPass),
                                  dynamic_cast<ShaderVk*>(shader),
                                  state,
                                  vertexBindings,
                                  pipelineLayout);
        }
        }
    }
} // namespace pom::gfx