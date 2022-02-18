#include "pch.hpp"

#include "buffer.hpp"

#include "instance.hpp"

#include "vulkan/bufferVk.hpp"
#include "vulkan/instanceVk.hpp"

namespace pom::gfx {
    Ref<Buffer> Buffer::create(BufferUsage usage,
                               BufferMemoryAccess access,
                               usize size,
                               const void* initialData,
                               usize initialDataOffset,
                               usize initialDataSize)
    {
        switch (Instance::get()->getAPI()) {
        case GraphicsAPI::VULKAN: {
            return Ref<Buffer>(new BufferVk(dynamic_cast<InstanceVk*>(Instance::get()),
                                            usage,
                                            access,
                                            size,
                                            initialData,
                                            initialDataOffset,
                                            initialDataSize));
        }
        }
    }

    Buffer::Buffer(BufferUsage usage, BufferMemoryAccess access, usize size) :
        usage(usage), memoryAccess(access), size(size)
    {
    }

} // namespace pom::gfx