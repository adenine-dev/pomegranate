#include "pch.hpp"

#include "descriptorSet.hpp"

#include "vulkan/descriptorSetVk.hpp"

#include "instance.hpp"
#include "pipelineLayout.hpp"

#include "vulkan/instanceVk.hpp"
#include "vulkan/pipelineLayoutVk.hpp"

namespace pom::gfx {
    Ref<DescriptorSet> DescriptorSet::create(const Ref<PipelineLayout>& layout, u32 set)
    {
        POM_PROFILE_FUNCTION();
        
        switch (Instance::get()->getAPI()) {
        case GraphicsAPI::VULKAN: {
            POM_ASSERT(layout->getAPI() == GraphicsAPI::VULKAN,
                       "attempting to create descriptor set with mismatched api");
            return Ref<DescriptorSet>(new DescriptorSetVk(dynamic_cast<InstanceVk*>(Instance::get()),
                                                          layout.dynCast<PipelineLayoutVk>(),
                                                          set));
        }
        }
    }
} // namespace pom::gfx