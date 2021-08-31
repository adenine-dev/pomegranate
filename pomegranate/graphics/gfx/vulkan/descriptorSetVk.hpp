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

        DescriptorSetVk(InstanceVk* instance, PipelineLayoutVk* layout, u32 set);

        ~DescriptorSetVk() final;

        void setBuffer(u32 binding, Buffer* buffer, u32 offset = 0, u32 size = 0) final;
        void setTexture(u32 binding, Texture* texture) final;

        inline VkDescriptorSet getVkDescriptorSet()
        {
            return descriptorSet;
        }

    private:
        POM_NOCOPY(DescriptorSetVk);

        InstanceVk* instance;

        PipelineLayoutVk* layout;

        // VkDescriptorSet descriptorSets[POM_MAX_FRAMES_IN_FLIGHT];
        VkDescriptorSet descriptorSet;
    };
} // namespace pom::gfx