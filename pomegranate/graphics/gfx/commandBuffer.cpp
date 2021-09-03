#include "pch.hpp"

#include "commandBuffer.hpp"

#include "instance.hpp"

#include "vulkan/commandBufferVk.hpp"
#include "vulkan/contextVk.hpp"

#include "core/application.hpp"

namespace pom::gfx {
    Ref<CommandBuffer> CommandBuffer::create(CommandBufferSpecialization specialization, u32 count)
    {
        switch (Instance::get()->getAPI()) {
        case GraphicsAPI::VULKAN: {
            return Ref<CommandBuffer>(new CommandBufferVk(dynamic_cast<InstanceVk*>(Instance::get()),
                                                          specialization,
                                                          count ? count : POM_MAX_FRAMES_IN_FLIGHT));
        }
        }
    }

    CommandBuffer::CommandBuffer(CommandBufferSpecialization specialization) : specialization(specialization)
    {
    }
} // namespace pom::gfx