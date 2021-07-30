#include "pch.hpp"

#include "commandBuffer.hpp"

#include "instance.hpp"

#include "vulkan/commandBufferVk.hpp"
#include "vulkan/contextVk.hpp"

#include "core/application.hpp"

namespace pom::gfx {
    CommandBuffer* CommandBuffer::create(CommandBufferSpecialization specialization, u32 count)
    {
        switch (Instance::get()->getAPI()) {
        case GraphicsAPI::VULKAN: {
            auto* ctx = dynamic_cast<ContextVk*>(Application::get()->getMainWindow().getContext());
            return new CommandBufferVk(dynamic_cast<InstanceVk*>(Instance::get()),
                                       specialization,
                                       count ? count : ctx->getSwapchainImageCount());
        }
        }
    }

    CommandBuffer::CommandBuffer(CommandBufferSpecialization specialization) : specialization(specialization)
    {
    }
} // namespace pom::gfx