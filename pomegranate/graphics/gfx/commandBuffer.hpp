#pragma once

#include "base.hpp"

#include "gfx.hpp"

#include "maths/vector.hpp"

namespace pom::gfx {

    class RenderPass;
    class Buffer;
    class Pipeline;
    class Context;
    class Texture;
    class PipelineLayout;
    class DescriptorSet;

    /// @addtogroup gfx
    /// @{

    /// Hint for what queue the `CommandBuffer` should be constructed from, only applies in graphics APIs where that is
    /// applicable such as Vulkan. In addition to this, some hardware won't have a dedicated queue for a certain
    /// specialization and the command buffer will be constructed from a different more general queue.
    enum class CommandBufferSpecialization {
        GRAPHICS, ///< Command buffer should be constructed by the graphics queue. This allows the buffer to use all
                  ///< commands.
        // TODO: compute
        // COMPUTE, ///< Command buffer should be constructed by the compute queue.
        TRANSFER, ///< Command buffer should be constructed by the transfer queue. Regardless of what queue this command
                  ///< buffer is constructed from this limits the commands that can be used by this command buffer to
                  ///< only transfer commands.

        GENERAL = GRAPHICS, ///< Equivelent to `CommandBufferSpecialization::GRAPHICS`.
    };

    /// The type of a member in an index Buffer. @see CommandBuffer::bindIndexBuffer
    enum class IndexType {
        // U8, // TODO: VK_EXT_index_type_uint8 ?
        U16, ///< The type is an unsigned 16bit integer.
        U32 ///< The type is an unsigned 32bit integer.
    };

    /// Command buffers are used to submit actions to the GPU.
    class POM_API CommandBuffer {
    public:
        /// Creates a command buffer tied to this context.
        /// @arg specialization: Dictates what this command buffer can do.
        /// @arg countHint: hint for how many internal command buffers should be created. If not passed will default to
        /// the maximum number of frames in flight. It is useful to set this to `1` for one time use command buffers.
        [[nodiscard]] static CommandBuffer* create(CommandBufferSpecialization specialization, u32 countHint = 0);

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

        /// Stop recording to a command buffer.
        virtual void end() = 0;

        /// Send the command buffer to the GPU for processing. Must have previously called `end`.
        // TODO: submit multiple command buffers and batch them
        virtual void submit() = 0;

        /// Begin the first subpass of a RenderPass. Should be called **after** `begin`. Requires the command buffer to
        /// have been created with `CommandBufferSpecialization::GRAPHICS`.
        // TODO: different frame buffers, currently only renders to swapchain framebuffer
        virtual void beginRenderPass(RenderPass* renderPass, Context* context) = 0;

        /// End the previously begun `RenderPass` should only be called after `beginRenderPass`. Requires the command
        /// buffer to have been created with `CommandBufferSpecialization::GRAPHICS`
        virtual void endRenderPass() = 0;

        /// Submits a set viewport command to the command buffer. Requires the command buffer to have been created with
        /// `CommandBufferSpecialization::GRAPHICS`
        virtual void setViewport(const Viewport& viewport) = 0;

        /// Submits a set scissor command to the command buffer. Requires the command buffer to have been created with
        /// `CommandBufferSpecialization::GRAPHICS`
        // TODO: AABB struct
        virtual void setScissor(const maths::ivec2& offset, const maths::uvec2& extent) = 0;

        /// Submits a draw command to the command buffer. Requires the command buffer to have been created with
        /// `CommandBufferSpecialization::GRAPHICS`
        virtual void draw(u32 vertexCount, u32 vertexOffset = 0) = 0;

        /// Submits a draw indexed command to the command buffer. Requires the command buffer to have been created with
        /// `CommandBufferSpecialization::GRAPHICS`
        virtual void drawIndexed(u32 indexCount, u32 firstIndex = 0, i32 vertexOffset = 0) = 0;

        /// Binds a vertex buffer to the given bind point, the Buffer **must** have been initialized with
        /// `BufferUsage::VERTEX`. The `offset` is the offset to the first byte that the shader will read. Requires the
        /// command buffer to have been created with `CommandBufferSpecialization::GRAPHICS`
        virtual void bindVertexBuffer(Buffer* vertexBuffer, u32 bindPoint = 0, size_t offset = 0) = 0;

        /// Binds an index buffer, the Buffer **must** have been initialize with `BufferUsage::INDEX`. `type` is how the
        /// data in the buffer will be read, this is independent from the type of whatever data was written into the
        /// buffer. `offset` is the offset to the first byte from which indicies will be calculated. Requires the
        /// command buffer to have been created with `CommandBufferSpecialization::GRAPHICS`
        virtual void bindIndexBuffer(Buffer* indexBuffer, IndexType type, size_t offset = 0) = 0;

        virtual void bindPipeline(Pipeline* pipeline) = 0;

        virtual void bindDescriptorSet(PipelineLayout* pipelineLayout, u32 set, DescriptorSet* descriptorSet) = 0;

        /// Copies the contents of `src` to `dst`.
        virtual void copyBuffer(Buffer* src, Buffer* dst, size_t size, size_t srcOffset, size_t dstOffset) = 0;

        /// Copies the contents of `src` to `dst`.
        virtual void copyBufferToTexture(Buffer* src,
                                         Texture* dst,
                                         size_t size,
                                         size_t srcOffset,
                                         maths::ivec3 dstOffset,
                                         maths::uvec3 dstExtent)
            = 0;

    protected:
        CommandBuffer(CommandBufferSpecialization specialization);

        CommandBufferSpecialization specialization;
    };

    /// @}
} // namespace pom::gfx