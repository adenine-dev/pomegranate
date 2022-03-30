#pragma once

#include "base.hpp"

#include "../descriptorSet.hpp"

namespace pom::gfx {
    class InstanceVk;
    class PipelineLayoutVk;

    class POM_API DescriptorSetVk final : public DescriptorSet {
    public:
        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        DescriptorSetVk(InstanceVk* instance, Ref<PipelineLayoutVk> pipelineLayout, u32 set);

        ~DescriptorSetVk() final;

        void setBuffer(DescriptorType descriptorType,
                       u32 binding,
                       const Ref<Buffer>& buffer,
                       u32 offset = 0,
                       u32 size = 0) final;
        void setTextureView(DescriptorType descriptorType, u32 binding, const Ref<TextureView>& textureViews) final;

        inline VkDescriptorSet getVkDescriptorSet()
        {
            return descriptorSet;
        }

    private:
        POM_NOCOPY(DescriptorSetVk);

        InstanceVk* instance;

        Ref<PipelineLayoutVk> layout;

        // VkDescriptorSet descriptorSets[POM_MAX_FRAMES_IN_FLIGHT];
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    };
} // namespace pom::gfx