#pragma once

#include "base.hpp"

#include "color.hpp"
#include "gfx/buffer.hpp"
#include "gfx/commandBuffer.hpp"

namespace pom::geometry {
    struct MeshVertex {
        maths::vec3 position;
        maths::vec3 normal;
        maths::vec2 uv;
        Color color;
        // TODO: normals
    };

    template <typename Vertex> struct GPUMesh {
        Ref<gfx::Buffer> vertexBuffer;
        Ref<gfx::Buffer> indexBuffer;
        gfx::IndexType indexType;
    };

    POM_API GPUMesh<MeshVertex> cube(Color color = Color::WHITE);

    POM_API GPUMesh<MeshVertex> sphere(Color color = Color::WHITE);

} // namespace pom::geometry