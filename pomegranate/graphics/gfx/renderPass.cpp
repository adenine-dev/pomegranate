#include "pch.hpp"

#include "renderPass.hpp"

#include "instance.hpp"

#include "vulkan/instanceVk.hpp"
#include "vulkan/renderPassVk.hpp"

namespace pom::gfx {
    Ref<RenderPass> RenderPass::create(std::initializer_list<RenderPassAttachment> colorAttachments,
                                       RenderPassAttachment depthStencilAttachment)
    {
        POM_PROFILE_FUNCTION();
        switch (Instance::get()->getAPI()) {
        case GraphicsAPI::VULKAN: {
            return Ref<RenderPass>(
                new RenderPassVk(dynamic_cast<InstanceVk*>(Instance::get()), colorAttachments, depthStencilAttachment));
        }
        }
    }
} // namespace pom::gfx