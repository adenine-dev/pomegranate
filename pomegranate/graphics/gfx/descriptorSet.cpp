#include "pch.hpp"

#include "descriptorSet.hpp"

#include "vulkan/descriptorSetVk.hpp"

#include "instance.hpp"
#include "pipelineLayout.hpp"

#include "vulkan/instanceVk.hpp"
#include "vulkan/pipelineLayoutVk.hpp"

namespace pom::gfx {
    DescriptorSet* DescriptorSet::create(PipelineLayout* layout, u32 set)
    {
        switch (Instance::get()->getAPI()) {
        case GraphicsAPI::VULKAN: {
            POM_ASSERT(layout->getAPI() == GraphicsAPI::VULKAN,
                       "attempting to create descriptor set with mismatched api");
            return new DescriptorSetVk(dynamic_cast<InstanceVk*>(Instance::get()),
                                       dynamic_cast<PipelineLayoutVk*>(layout),
                                       set);
        }
        }
    }
} // namespace pom::gfx