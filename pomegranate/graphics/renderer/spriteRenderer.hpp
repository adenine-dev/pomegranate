#pragma once

#include "base.hpp"

#include "graphics/color.hpp"
#include "graphics/gfx/context.hpp"
#include "maths/matrix.hpp"
#include "scene/ecs/store.hpp"

namespace pom {
    struct SpriteRendererComponent {
        maths::mat4 transform;
        Color color;

        ECS_COMPONENT();
    };

    class POM_API SpriteRenderer {
    public:
        SpriteRenderer(gfx::Context* context, Store& store);

        void render(gfx::Context* context, Ref<gfx::CommandBuffer> commandBuffer, const maths::mat4& viewProjection);

    private:
        Store& store;

        Ref<gfx::Buffer> uniformBuffers[POM_MAX_FRAMES_IN_FLIGHT];
        SpriteRendererComponent* uniformMappings[POM_MAX_FRAMES_IN_FLIGHT];

        Ref<gfx::PipelineLayout> pipelineLayout;
        Ref<gfx::DescriptorSet> descriptorSets[POM_MAX_FRAMES_IN_FLIGHT];
        Ref<gfx::Pipeline> pipeline;
    };
} // namespace pom