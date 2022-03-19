#include "pch.hpp"

#include "mesh.hpp"

namespace pom::geometry {
    GPUMesh<MeshVertex> cube(Color color)
    {
        // clang-format off
        MeshVertex VERTICIES[] = {
            // front
            { maths::vec3(-1, -1,  1), Color::rgb(255, 0, 0), maths::vec2(0, 0) },
            { maths::vec3( 1, -1,  1), Color::rgb(255, 0, 0), maths::vec2(0, 1) }, 
            { maths::vec3( 1,  1,  1), Color::rgb(255, 0, 0), maths::vec2(1, 1) },
            { maths::vec3(-1,  1,  1), Color::rgb(255, 0, 0), maths::vec2(1, 0) }, 

            // back
            { maths::vec3(-1, -1, -1), Color::rgb(0, 255, 0), maths::vec2(1, 0) },
            { maths::vec3( 1, -1, -1), Color::rgb(0, 255, 0), maths::vec2(1, 1) }, 
            { maths::vec3( 1,  1, -1), Color::rgb(0, 255, 0), maths::vec2(0, 1) },
            { maths::vec3(-1,  1, -1), Color::rgb(0, 255, 0), maths::vec2(0, 0) }, 

            // left
            { maths::vec3(-1, -1, -1), Color::rgb(0, 0, 255), maths::vec2(1, 0) },
            { maths::vec3(-1, -1,  1), Color::rgb(0, 0, 255), maths::vec2(1, 1) }, 
            { maths::vec3(-1,  1,  1), Color::rgb(0, 0, 255), maths::vec2(0, 1) },
            { maths::vec3(-1,  1, -1), Color::rgb(0, 0, 255), maths::vec2(0, 0) }, 

            // right
            { maths::vec3( 1, -1, -1), Color::rgb(255, 255, 0), maths::vec2(0, 0) },
            { maths::vec3( 1, -1,  1), Color::rgb(255, 255, 0), maths::vec2(0, 1) }, 
            { maths::vec3( 1,  1,  1), Color::rgb(255, 255, 0), maths::vec2(1, 1) },
            { maths::vec3( 1,  1, -1), Color::rgb(255, 255, 0), maths::vec2(1, 0) }, 

            // top
            { maths::vec3(-1,  1, -1), Color::rgb(255, 0, 255), maths::vec2(0, 0) },
            { maths::vec3(-1,  1,  1), Color::rgb(255, 0, 255), maths::vec2(0, 1) }, 
            { maths::vec3( 1,  1,  1), Color::rgb(255, 0, 255), maths::vec2(1, 1) },
            { maths::vec3( 1,  1, -1), Color::rgb(255, 0, 255), maths::vec2(1, 0) }, 

            // bottom
            { maths::vec3(-1, -1, -1), Color::rgb(0, 255, 255), maths::vec2(1, 0) },
            { maths::vec3(-1, -1,  1), Color::rgb(0, 255, 255), maths::vec2(1, 1) }, 
            { maths::vec3( 1, -1,  1), Color::rgb(0, 255, 255), maths::vec2(0, 1) },
            { maths::vec3( 1, -1, -1), Color::rgb(0, 255, 255), maths::vec2(0, 0) }, 
        };

        static constexpr u16 INDICIES[] = {
             0,  1,  3,  3,  1,  2, // front
             4,  5,  7,  7,  5,  6, // back
             8,  9, 11, 11,  9, 10, // left
            12, 13, 15, 15, 13, 14, // right
            16, 17, 19, 19, 17, 18, // top
            20, 21, 23, 23, 21, 22, // bottom
        };
        // clang-format on

        return {
            .vertexBuffer = gfx::Buffer::create(gfx::BufferUsage::VERTEX,
                                                gfx::BufferMemoryAccess::GPU_ONLY,
                                                sizeof(VERTICIES),
                                                VERTICIES),
            .indexBuffer = gfx::Buffer::create(gfx::BufferUsage::INDEX,
                                               gfx::BufferMemoryAccess::GPU_ONLY,
                                               sizeof(INDICIES),
                                               INDICIES),
            .indexType = gfx::IndexType::U16,
        };
    }
} // namespace pom::geometry