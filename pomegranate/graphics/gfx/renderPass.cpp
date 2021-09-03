#include "pch.hpp"

#include "renderPass.hpp"

#include "instance.hpp"

#include "vulkan/instanceVk.hpp"
#include "vulkan/renderPassVk.hpp"

namespace pom::gfx {
    Ref<RenderPass> RenderPass::create(std::initializer_list<RenderPassAttachment> attachments)
    {
        switch (Instance::get()->getAPI()) {
        case GraphicsAPI::VULKAN: {
            return Ref<RenderPass>(new RenderPassVk(dynamic_cast<InstanceVk*>(Instance::get()), attachments));
        }
        }
    }
} // namespace pom::gfx