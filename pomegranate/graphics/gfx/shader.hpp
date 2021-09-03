#pragma once

#include "base.hpp"

#include "gfx.hpp"

namespace pom::gfx {
    /// @addtogroup gfx
    /// @{

    enum class ShaderStage {
        VERTEX,
        FRAGMENT,
        // TODO: everything else
    };

    /// A single shader module for a given shader stage.
    class POM_API ShaderModule {
    public:
        /// Returns the GraphicsAPI associated with this shader module.
        [[nodiscard]] constexpr virtual GraphicsAPI getAPI() const = 0;

        /// Creates a shader module from SPIR-V code.
        /// @param stage: The shader stage this module will be used for.
        /// @param size: The size of `spirvCode` **in bytes**
        /// @param spirvCode: Valid SPIR-V code this shader module will be used with.
        [[nodiscard]] static Ref<ShaderModule> create(ShaderStage stage, size_t size, const u32* spirvCode);

        [[nodiscard]] inline ShaderStage getStage() const
        {
            return stage;
        };

        virtual ~ShaderModule() = default;

    protected:
        ShaderModule(ShaderStage stage);

        ShaderStage stage;
    };

    /// A collection of shader modules
    class POM_API Shader {
    public:
        /// Returns the GraphicsAPI associated with this shader program.
        [[nodiscard]] constexpr virtual GraphicsAPI getAPI() const = 0;

        [[nodiscard]] static Ref<Shader> create(std::initializer_list<Ref<ShaderModule>> modules);

        [[nodiscard]] inline virtual size_t getNumModules() const = 0;

        virtual ~Shader() = default;
    };

    /// @}
} // namespace pom::gfx