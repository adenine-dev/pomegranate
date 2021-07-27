#include "pch.hpp"

#include "commandBuffer.hpp"

namespace pom::gfx {
    CommandBuffer::CommandBuffer(CommandBufferSpecialization specialization) : specialization(specialization)
    {
    }
} // namespace pom::gfx