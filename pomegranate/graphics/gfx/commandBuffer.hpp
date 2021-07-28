#pragma once

#include "base.hpp"

#include "gfx.hpp"

#include "maths/vector.hpp"

namespace pom::gfx {

    class RenderPass;
    class Buffer;

    /// @addtogroup gfx
    /// @{

    /// Hint for what queue the `CommandBuffer` should be constructed from, only applies in graphics APIs where that is
    /// applicable such as Vulkan. In addition to this, some hardware won't have a dedicated queue for a certain
    /// specialization and the command buffer will be constructed from a different more general queue.
    enum class CommandBufferSpecialization {
        GRAPHICS,
        // COMPUTE,
        // TRANSFER //TODO: transfer queues
    };

    /// Command buffers are used to submit actions to the GPU.
    class POM_API CommandBuffer {
    public:
        CommandBuffer(CommandBufferSpecialization specialization);
        virtual ~CommandBuffer() = default;

        /// Returns the GraphicsAPI associated with this command buffer.
        [[nodiscard]] inline CommandBufferSpecialization getSpecialization() const
        {
            return specialization;
        };

        /// Returns the GraphicsAPI associated with this command buffer.
        [[nodiscard]] constexpr virtual GraphicsAPI getAPI() const = 0;

        /// Start recording to a command buffer, should be called before any other commands are submitted.
        // TODO: maybe explicitly keep track of command buffer state?
        virtual void begin() = 0;

        /// Begin the first subpass of a RenderPass. Should be called **after** `begin`.
        // TODO: different frame buffers, currently only renders to swapchain framebuffer
        virtual void beginRenderPass(RenderPass* renderPass) = 0;

        /// End the previously begun `RenderPass` should only be called after `beginRenderPass`.
        virtual void endRenderPass() = 0;

        /// Stop recording to a command buffer.
        virtual void end() = 0;

        /// Submits a set viewport command to the command buffer.
        // TODO: AABB struct
        virtual void setViewport(const maths::vec2& offset, const maths::vec2& extent, f32 mindepth, f32 maxdepth) = 0;

        /// Submits a set scissor command to the command buffer.
        // TODO: AABB struct
        virtual void setScissor(const maths::ivec2& offset, const maths::uvec2& extent) = 0;

        /// Submits a draw command to the command buffer.
        virtual void draw(u32 vertexCount, u32 firstVertex = 0) = 0;

        /// Binds a vertex buffer to the given bind point, the Buffer **must** have been initialized with
        /// `BufferUsage::VERTEX`. The `offset` is the offset to the first byte that the shader will read.
        virtual void bindVertexBuffer(Buffer* vertexBuffer, u32 bindPoint = 0, size_t offset = 0) = 0;

    protected:
        CommandBufferSpecialization specialization;
    };

    /// @}
} // namespace pom::gfx