#pragma once

#include "../renderPass.hpp"

namespace pom::gfx {
    /// @addtogroup vulkan
    /// @{

    class InstanceVk;

    /// Vulkan Context, should be created implicitly through `RenderPass::create`.
    class RenderPassVk final : public RenderPass {
    public:
        RenderPassVk(InstanceVk* instance, std::initializer_list<RenderPassAttachment> attachments);
        ~RenderPassVk() final;

        [[nodiscard]] inline VkRenderPass getHandle() const
        {
            return renderPass;
        }

    private:
        InstanceVk* instance;
        VkRenderPass renderPass;
    };

    /// @}
} // namespace pom::gfx