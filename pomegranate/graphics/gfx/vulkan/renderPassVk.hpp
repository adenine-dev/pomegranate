#pragma once

#include "../renderPass.hpp"

namespace pom::gfx {
    /// @addtogroup vulkan
    /// @{

    class InstanceVk;

    /// Vulkan RenderPass, should be created implicitly through `RenderPass::create`. Contains a RenderPass object and
    /// all its associated data.
    class RenderPassVk final : public RenderPass {
    public:
        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        RenderPassVk(InstanceVk* instance,
                     std::initializer_list<RenderPassAttachment> colorAttachments,
                     RenderPassAttachment depthStencilAttachment);
        ~RenderPassVk();

        [[nodiscard]] inline VkRenderPass getHandle() const
        {
            return renderPass;
        }

        [[nodiscard]] inline u32 getClearColorCountVk() const
        {
            return clearColors.size();
        }
        [[nodiscard]] inline const VkClearValue* getClearColorsVk() const
        {
            return clearColors.data();
        }

    private:
        POM_NOCOPY(RenderPassVk);

        InstanceVk* instance;
        VkRenderPass renderPass;
        std::vector<VkClearValue> clearColors;
    };

    /// @}
} // namespace pom::gfx