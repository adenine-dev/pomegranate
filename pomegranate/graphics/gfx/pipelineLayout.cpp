#include "pch.hpp"

#include "pipelineLayout.hpp"

#include "vulkan/pipelineLayoutVk.hpp"

#include "instance.hpp"

#include "vulkan/instanceVk.hpp"

namespace pom::gfx {
    PipelineLayout* PipelineLayout::create(std::initializer_list<Descriptor> descriptors)
    {
        switch (Instance::get()->getAPI()) {
        case GraphicsAPI::VULKAN: {
            return new PipelineLayoutVk(dynamic_cast<InstanceVk*>(Instance::get()), descriptors);
        }
        }
    }
} // namespace pom::gfx