#pragma once

#include "base.hpp"

#include "../pipelineLayout.hpp"

#include <unordered_map>

namespace pom::gfx {
    class InstanceVk;

    class POM_API PipelineLayoutVk final : public PipelineLayout {
    public:
        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        PipelineLayoutVk(InstanceVk* instance,
                         std::initializer_list<Descriptor> descriptors,
                         std::initializer_list<PushConstant> pushConstants);

        ~PipelineLayoutVk() final;

        [[nodiscard]] inline VkPipelineLayout getVkPipelineLayout()
        {
            return pipelineLayout;
        }

    private:
        friend class DescriptorSetVk;

        POM_NOCOPY(PipelineLayoutVk);

        VkDescriptorPool pool = VK_NULL_HANDLE; // TODO: global pools.
        
        std::unordered_map<u32, std::vector<VkDescriptorSetLayoutBinding>> descriptorSetBindings;
        std::unordered_map<u32, VkDescriptorSetLayout> descriptorSetLayouts;

        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

        InstanceVk* instance;
    };
} // namespace pom::gfx