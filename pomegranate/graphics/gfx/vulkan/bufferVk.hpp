#pragma once

#include "../buffer.hpp"

#include "instanceVk.hpp"

namespace pom::gfx {

    /// @addtogroup vulkan
    /// @{

    /// Vulkan Buffer, should be created implicitly through `Buffer::create`. Contains the buffer and corresponding
    /// memory.
    class POM_API BufferVk final : public Buffer {
    public:
        BufferVk(InstanceVk* instance,
                 BufferUsage usage,
                 BufferMemoryAccess access,
                 size_t size,
                 const void* initialData,
                 size_t initialDataOffset,
                 size_t initialDataSize);
        ~BufferVk() final;

        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        [[nodiscard]] VkBuffer getBuffer()
        {
            return buffer;
        }

        [[nodiscard]] void* map(size_t offset = 0, size_t size = 0) final;
        void unmap() final;

    protected:
        POM_NOCOPY(BufferVk);

        InstanceVk* instance;
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
    };
} // namespace pom::gfx