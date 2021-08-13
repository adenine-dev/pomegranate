#pragma once

#include "base.hpp"

#include "gfx.hpp"

namespace pom::gfx {

    class RenderPass;
    class Shader;

    enum class PrimitiveTopology {
        POINTS,
        LINES,
        LINE_STRIP,
        TRIANGLES,
        TRIANGLE_STRIP,
        TRIANGLE_FAN,
    };

    enum class CullMode {
        NONE,
        FRONT,
        BACK,
        BOTH,
    };

    struct PipelineState {
        PrimitiveTopology topology = PrimitiveTopology::TRIANGLES;
        bool primativeRestart = false;
        CullMode cullMode = CullMode::BACK;
        bool frontIsClockwise = false;
        // TODO: blend modes, prob a dozen other things
    };

    struct VertexAttribute {
        u32 location;
        Format format;
        i32 offset = -1;
    };

    struct VertexBinding {
        u32 binding;
        i32 stride = -1;
        std::vector<VertexAttribute> attribs;
    };

    // TODO: compute pipelines

    class POM_API Pipeline {
    public:
        /// Returns the GraphicsAPI associated with this pipeline.
        [[nodiscard]] constexpr virtual GraphicsAPI getAPI() const = 0;

        [[nodiscard]] static Pipeline* create(PipelineState state,
                                              Shader* shader,
                                              RenderPass* renderPass,
                                              std::initializer_list<VertexBinding> vertexBindings,
                                              VkPipelineLayout pipelineLayout); // FIXME: api independence
        virtual ~Pipeline() = default;

    protected:
    };
} // namespace pom::gfx