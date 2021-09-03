#pragma once

#include "base.hpp"

#include "gfx.hpp"

#include "../color.hpp"

namespace pom::gfx {
    /// @addtogroup gfx
    /// @{

    /// Operation to be performed when a renderpass attachment is loaded.
    enum class LoadOperation {
        LOAD, ///< Previous image contents will be preserved.
        CLEAR, ///< Previous image contents will be cleared to the value specified by `RenderPassAttachment.clearColor`.
        DONTCARE ///< Previous image contents are uknown, accessing them is undefined behavior.
    };

    /// Operation to be performed when a renderpass attachment is stored.
    enum class StoreOperation {
        STORE, ///< Image contents are stored to memory.
        DONTCARE ///< Image contents may be discarded, accessing them is undefined behavior.
    };

    /// Attachment data for a `RenderPass`.
    struct RenderPassAttachment {
        Format format; ///< Image format of this attachment.
        // TODO: figure out how multisampling is going to be done.
        // u8 samples; ///< Number of samples, must be a power of 2
        LoadOperation loadOperation; ///< Load operation for this attachment.
        StoreOperation storeOperation; ///< Store operation for this attachment.
        // TODO: depth/stencil clear colors for when that's a thing.
        Color clearColor = Color::BLACK; ///< Clear color if this attachment's load operation is `LoadOperation::CLEAR`
    };

    // TODO: multiple subpasses, subpass dependencies. Make this less extremely dumb.

    /// currently a thin abstraction over Vulkan's, needs a lot of work lol
    class POM_API RenderPass {
    public:
        /// Returns the GraphicsAPI associated with this render pass.
        [[nodiscard]] constexpr virtual GraphicsAPI getAPI() const = 0;

        [[nodiscard]] static Ref<RenderPass> create(std::initializer_list<RenderPassAttachment> attachments);

        virtual ~RenderPass() = default;
    };

    /// @}
} // namespace pom::gfx