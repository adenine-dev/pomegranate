#pragma once

#include "base.hpp"

#include "gfx.hpp"

#include "util/util.hpp"

namespace pom::gfx {

    /// @addtogroup gfx
    /// @{

    namespace BufferUsageNamespace {
        enum BufferUsage {
            VERTEX = 1 << 0,
            INDEX = 1 << 1,
            UNIFORM = 1 << 2,
            // TODO: transfer, storage, texels, indirect
        };
    }

    using BufferUsage = BufferUsageNamespace::BufferUsage;

    POM_MAKE_FLAGS(BufferUsage);

    class POM_API Buffer {
    public:
        static Buffer* create(BufferUsage usage, size_t size);

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