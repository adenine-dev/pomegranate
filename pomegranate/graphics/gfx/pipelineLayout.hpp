#pragma once

#include "base.hpp"

#include "gfx.hpp"

#include "shader.hpp"

namespace pom::gfx {

    /// @addtogroup gfx
    /// @{

    class Buffer;
    class Texture;

    enum class DescriptorType {
        UNIFORM_BUFFER,
        COMBINED_TEXTURE_SAMPLER,
        STORAGE_IMAGE,
        // TODO: the rest of these lol
    };

    struct Descriptor {
        DescriptorType type;
        u32 set;
        u32 binding;
        ShaderStageFlags stages;
        u32 count = 1;
    };

    struct PushConstant {
        ShaderStageFlags stages;
        u32 size;
        u32 offset;
    };

    class POM_API PipelineLayout {
    public:
        /// Returns the GraphicsAPI associated with this pipeline layout.
        [[nodiscard]] constexpr virtual GraphicsAPI getAPI() const = 0;

        [[nodiscard]] static Ref<PipelineLayout> create(u32 numDescSets,
                                                        std::initializer_list<Descriptor> descriptors,
                                                        std::initializer_list<PushConstant> pushConstants);

        // [[nodiscard]] virtual u32 getDescriptorSets() const = 0;

        virtual ~PipelineLayout() = default;

    protected:
    };

    ///@}

} // namespace pom::gfx