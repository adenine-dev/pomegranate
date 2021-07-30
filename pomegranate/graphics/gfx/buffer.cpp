#include "pch.hpp"

#include "buffer.hpp"

#include "instance.hpp"

#include "vulkan/bufferVk.hpp"
#include "vulkan/instanceVk.hpp"

namespace pom::gfx {
    Buffer* Buffer::create(BufferUsage usage,
                           BufferMemoryAccess access,
                           size_t size,
                           const void* initialData,
                           size_t initialDataOffset,
                           size_t initialDataSize)
    {
        switch (Instance::get()->getAPI()) {
        case GraphicsAPI::VULKAN: {
            return new BufferVk(dynamic_cast<InstanceVk*>(Instance::get()),
                                usage,
                                access,
                                size,
                                initialData,
                                initialDataOffset,
                                initialDataSize);
        }
        }
    }

    Buffer::Buffer(BufferUsage usage, BufferMemoryAccess access, size_t size) :
        usage(usage), memoryAccess(access), size(size)
    {
    }

} // namespace pom::gfx