#include "pch.hpp"

#include "buffer.hpp"

#include "instance.hpp"

#include "vulkan/bufferVk.hpp"
#include "vulkan/instanceVk.hpp"

namespace pom::gfx {
    Buffer* Buffer::create(BufferUsage usage, size_t size)
    {
        switch (Instance::get()->getAPI()) {
        case GraphicsAPI::VULKAN: {
            return new BufferVk(dynamic_cast<InstanceVk*>(Instance::get()), usage, size);
        }
        }
    }

    Buffer::Buffer(BufferUsage usage, size_t size) : usage(usage), size(size)
    {
    }

} // namespace pom::gfx