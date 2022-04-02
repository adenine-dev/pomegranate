#include "maths/maths.hpp"
#include "pch.hpp"

#include "mesh.hpp"

namespace pom::geometry {
    GPUMesh<MeshVertex> cube(Color color)
    {
        using namespace maths;

        // clang-format off
        MeshVertex vertices[] = {
            // front
            { vec3(-1, -1,  1), vec3(0, 0, 1), vec2(0, 0), Color::rgb(255, 0, 0) },
            { vec3( 1, -1,  1), vec3(0, 0, 1), vec2(0, 1), Color::rgb(255, 0, 0) }, 
            { vec3( 1,  1,  1), vec3(0, 0, 1), vec2(1, 1), Color::rgb(255, 0, 0) },
            { vec3(-1,  1,  1), vec3(0, 0, 1), vec2(1, 0), Color::rgb(255, 0, 0) }, 

            // back
            { vec3(-1, -1, -1), vec3(0, 0, -1), vec2(1, 0), Color::rgb(0, 255, 0) },
            { vec3( 1, -1, -1), vec3(0, 0, -1), vec2(1, 1), Color::rgb(0, 255, 0) }, 
            { vec3( 1,  1, -1), vec3(0, 0, -1), vec2(0, 1), Color::rgb(0, 255, 0) },
            { vec3(-1,  1, -1), vec3(0, 0, -1), vec2(0, 0), Color::rgb(0, 255, 0) }, 

            // left
            { vec3(-1, -1, -1), vec3(-1, 0, 0), vec2(1, 0), Color::rgb(0, 0, 255) },
            { vec3(-1, -1,  1), vec3(-1, 0, 0), vec2(1, 1), Color::rgb(0, 0, 255) }, 
            { vec3(-1,  1,  1), vec3(-1, 0, 0), vec2(0, 1), Color::rgb(0, 0, 255) },
            { vec3(-1,  1, -1), vec3(-1, 0, 0), vec2(0, 0), Color::rgb(0, 0, 255) }, 

            // right
            { vec3( 1, -1, -1), vec3(1, 0, 0), vec2(0, 0), Color::rgb(255, 255, 0) },
            { vec3( 1, -1,  1), vec3(1, 0, 0), vec2(0, 1), Color::rgb(255, 255, 0) }, 
            { vec3( 1,  1,  1), vec3(1, 0, 0), vec2(1, 1), Color::rgb(255, 255, 0) },
            { vec3( 1,  1, -1), vec3(1, 0, 0), vec2(1, 0), Color::rgb(255, 255, 0) }, 

            // top
            { vec3(-1,  1, -1), vec3(0, 1, 0), vec2(0, 0), Color::rgb(255, 0, 255) },
            { vec3(-1,  1,  1), vec3(0, 1, 0), vec2(0, 1), Color::rgb(255, 0, 255) }, 
            { vec3( 1,  1,  1), vec3(0, 1, 0), vec2(1, 1), Color::rgb(255, 0, 255) },
            { vec3( 1,  1, -1), vec3(0, 1, 0), vec2(1, 0), Color::rgb(255, 0, 255) }, 

            // bottom
            { vec3(-1, -1, -1), vec3(0, -1, 0), vec2(1, 0), Color::rgb(0, 255, 255) },
            { vec3(-1, -1,  1), vec3(0, -1, 0), vec2(1, 1), Color::rgb(0, 255, 255) }, 
            { vec3( 1, -1,  1), vec3(0, -1, 0), vec2(0, 1), Color::rgb(0, 255, 255) },
            { vec3( 1, -1, -1), vec3(0, -1, 0), vec2(0, 0), Color::rgb(0, 255, 255) }, 
        };

        static constexpr u16 indices[] = {
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
                                                sizeof(vertices),
                                                vertices),
            .indexBuffer
            = gfx::Buffer::create(gfx::BufferUsage::INDEX, gfx::BufferMemoryAccess::GPU_ONLY, sizeof(indices), indices),
            .indexType = gfx::IndexType::U16,
        };
    }

    GPUMesh<MeshVertex> sphere(Color color)
    {
        using namespace maths;

        const u32 latDetail = 32;
        const u32 lonDetail = 32;

        MeshVertex vertices[(latDetail + 1) * (lonDetail + 1)];
        u16 indices[(latDetail - 1) * lonDetail * 6];

        u32 count = 0;
        u16 k1 = 0;
        u16 k2 = 0;
        for (u32 lat = 0; lat <= latDetail; ++lat) {
            const f32 phi = (f32)PI / 2 - (f32)lat * (f32)PI / latDetail;
            const f32 xz = cosf(phi);
            const f32 y = sinf(phi);

            k1 = lat * (lonDetail + 1);
            k2 = k1 + lonDetail + 1;

            for (u32 lon = 0; lon <= lonDetail; ++lon, ++k1, ++k2) {
                const f32 theta = (f32)lon * 2 * (f32)PI / lonDetail;
                const f32 x = xz * sinf(theta);
                const f32 z = xz * cosf(theta);

                vertices[lat * (lonDetail + 1) + lon] = {
                    .position = vec3(x, y, z),
                    .normal = vec3(x, y, z),
                    .uv = vec2((f32)lon / lonDetail, (f32)lat / latDetail),
                    .color = color,
                };

                if (lat != latDetail && lon != lonDetail) {
                    if (lat != 0) {
                        indices[count++] = k1;
                        indices[count++] = k2;
                        indices[count++] = k1 + 1;
                    }

                    if (lat != (latDetail - 1)) {
                        indices[count++] = k1 + 1;
                        indices[count++] = k2;
                        indices[count++] = k2 + 1;
                    }
                }
            }
        }

        return {
            .vertexBuffer = gfx::Buffer::create(gfx::BufferUsage::VERTEX,
                                                gfx::BufferMemoryAccess::GPU_ONLY,
                                                sizeof(vertices),
                                                vertices),
            .indexBuffer
            = gfx::Buffer::create(gfx::BufferUsage::INDEX, gfx::BufferMemoryAccess::GPU_ONLY, sizeof(indices), indices),
            .indexType = gfx::IndexType::U16,
        };
    }

} // namespace pom::geometry