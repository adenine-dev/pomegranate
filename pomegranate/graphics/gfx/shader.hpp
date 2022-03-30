#pragma once

#include "base.hpp"

#include "gfx.hpp"

namespace pom::gfx {
    /// @addtogroup gfx
    /// @{

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
            COMPUTE = 1 << 2,
        };

#ifndef DOXYGEN
    } // namespace ShaderStageFlagsNamespace

    using ShaderStageFlags = ShaderStageFlagsNamespace::ShaderStageFlags;
#endif

    /// A single shader module for a given shader stage.
    class POM_API ShaderModule {
    public:
        /// Returns the GraphicsAPI associated with this shader module.
        [[nodiscard]] constexpr virtual GraphicsAPI getAPI() const = 0;

        /// Creates a shader module from SPIR-V code.
        /// @param stage: The shader stage this module will be used for.
        /// @param size: The size of `spirvCode` **in bytes**
        /// @param spirvCode: Valid SPIR-V code this shader module will be used with.
        [[nodiscard]] static Ref<ShaderModule> create(ShaderStageFlags stage, usize size, const u32* spirvCode);

        [[nodiscard]] inline ShaderStageFlags getStage() const
        {
            return stage;
        };

        virtual ~ShaderModule() = default;

    protected:
        ShaderModule(ShaderStageFlags stage);

        ShaderStageFlags stage;
    };

    /// A collection of shader modules
    class POM_API Shader {
    public:
        /// Returns the GraphicsAPI associated with this shader program.
        [[nodiscard]] constexpr virtual GraphicsAPI getAPI() const = 0;

        [[nodiscard]] static Ref<Shader> create(std::initializer_list<Ref<ShaderModule>> modules);

        [[nodiscard]] inline virtual usize getNumModules() const = 0;

        virtual ~Shader() = default;
    };

    /// @}
} // namespace pom::gfx