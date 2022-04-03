#include "pch.hpp"

#include "pipelineLayout.hpp"

#include "vulkan/pipelineLayoutVk.hpp"

#include "instance.hpp"

#include "vulkan/instanceVk.hpp"

namespace pom::gfx {
    Ref<PipelineLayout> PipelineLayout::create(u32 numDescSets,
                                               std::initializer_list<Descriptor> descriptors,
                                               std::initializer_list<PushConstant> pushConstants)
    {
        POM_PROFILE_FUNCTION();
        switch (Instance::get()->getAPI()) {
        case GraphicsAPI::VULKAN: {
            return Ref<PipelineLayout>(new PipelineLayoutVk(dynamic_cast<InstanceVk*>(Instance::get()),
                                                            numDescSets,
                                                            descriptors,
                                                            pushConstants));
        }
        }
    }
} // namespace pom::gfx