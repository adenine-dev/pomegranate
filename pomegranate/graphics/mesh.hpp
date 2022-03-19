#pragma once

#include "base.hpp"

#include "color.hpp"
#include "gfx/buffer.hpp"
#include "gfx/commandBuffer.hpp"

namespace pom::geometry {
    struct MeshVertex {
        maths::vec3 position;
        Color color;
        maths::vec2 uv;
        // TODO: normals
    };

    template <typename Vertex> struct GPUMesh {
        Ref<gfx::Buffer> vertexBuffer;
        Ref<gfx::Buffer> indexBuffer;
        gfx::IndexType indexType;
    };

    POM_API GPUMesh<MeshVertex> cube(Color color = Color::WHITE);

} // namespace pom::geometry