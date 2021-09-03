#pragma once

#include "base.hpp"

#include "gfx.hpp"

namespace pom::gfx {

    /// @addtogroup gfx
    /// @{

    class Buffer;
    class Texture;

#ifndef DOXYGEN // Hide this because technically it doesn't matter, the namespace is only really here so that the
                // members of the enum do not pollute the namespace.
    namespace ShaderStageFlagsNamespace {
#endif
        enum ShaderStageFlags {
            VERTEX = 1 << 0,
            // TESSELLATION_CONTROL,
            // TESSELLATION_EVALUATION,
            // GEOMETRY,
            FRAGMENT = 1 << 1,
            // COMPUTE,
        };

#ifndef DOXYGEN
    } // namespace ShaderStageFlagsNamespace

    using ShaderStageFlags = ShaderStageFlagsNamespace::ShaderStageFlags;
#endif

    enum class DescriptorType {
        UNIFORM_BUFFER,
        COMBINED_TEXTURE_SAMPLER,
        // TODO: the rest of these lol
    };

    struct Descriptor {
        DescriptorType type;
        u32 set;
        u32 binding;
        ShaderStageFlags stages;
        u32 count = 1;
    };

    class POM_API PipelineLayout {
    public:
        /// Returns the GraphicsAPI associated with this pipeline layout.
        [[nodiscard]] constexpr virtual GraphicsAPI getAPI() const = 0;

        [[nodiscard]] static Ref<PipelineLayout> create(std::initializer_list<Descriptor> descriptors);

        // [[nodiscard]] virtual u32 getDescriptorSets() const = 0;

        virtual ~PipelineLayout() = default;

    protected:
    };

    ///@}

} // namespace pom::gfx