#pragma once

#include "base.hpp"

#include "gfx.hpp"

#include "util/util.hpp"

namespace pom::gfx {

    /// @addtogroup gfx
    /// @{

#ifndef DOXYGEN // Hide this because technically it doesn't matter, the namespace is only really here so that the
                // members of the enum do not pollute the namespace.
    namespace BufferUsageNamespace {
#endif
        /// The usage for a given `Buffer`, used to optimize buffers, members may be treated as a flags and have
        /// bitwise operators performed on them.
        enum BufferUsage {
            VERTEX = 1 << 0, ///< This Buffer will be used as a vertex buffer. @see CommandBuffer::bindVertexBuffer
            INDEX = 1 << 1, ///< This Buffer will be used as an index buffer. @see CommandBuffer::bindIndexBuffer
            UNIFORM = 1 << 2, ///< This Buffer will be used as an uniform buffer.
            TRANSFER_SRC = 1 << 3, ///< This Buffer will be used as a source for a buffer transfer.
            TRANSFER_DST = 1 << 4 ///< This Buffer will be used as a destination for a buffer transfer.
            // TODO: storage, texels, indirect
        };

#ifndef DOXYGEN
    } // namespace BufferUsageNamespace

    using BufferUsage = BufferUsageNamespace::BufferUsage;

    POM_MAKE_FLAGS(BufferUsage);
#endif

    enum class BufferMemoryAccess { GPU_ONLY, CPU_VISIBLE };

    /// Static sized GPU buffer.
    class POM_API Buffer {
    public:
        // TODO: CPU access flags, staging buffer for data that is write only probs.

        /// Creates a Buffer.
        /// @arg usage: Some combination of `BufferUsage`s that this buffer can be used with.
        /// @arg size: The size of the buffer.
        /// @arg initialData: If passed the buffer will be initialized with this data.
        /// @arg initialDataOffset: If passed the buffer will be created with the `initialData` at the specified offset.
        /// @arg initialDataSize: If passed the size of `initialData`. If not passed `initialData` is assumed to be of
        /// size `size`, and `initialDataOffset` should be 0.
        static Buffer* create(BufferUsage usage,
                              size_t size,
                              const void* initialData = nullptr,
                              size_t initialDataOffset = 0,
                              size_t initialDataSize = 0);

        virtual ~Buffer() = default;

        /// Returns the GraphicsAPI associated with this buffer.
        [[nodiscard]] constexpr virtual GraphicsAPI getAPI() const = 0;

        /// Returns the size of this buffer.
        [[nodiscard]] inline size_t getSize() const
        {
            return size;
        }

        /// Returns the usage flags of this buffer.
        [[nodiscard]] inline BufferUsage getUsage() const
        {
            return usage;
        }

        [[nodiscard]] virtual void* map(size_t offset, size_t size) = 0;
        virtual void unmap() = 0;

    protected:
        Buffer(BufferUsage usage, size_t size);

        BufferUsage usage;
        size_t size;
    };

    /// @}

} // namespace pom::gfx