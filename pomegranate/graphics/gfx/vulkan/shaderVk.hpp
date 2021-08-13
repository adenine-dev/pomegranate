#pragma once

#include "../shader.hpp"

namespace pom::gfx {
    /// @addtogroup vulkan
    /// @{

    class InstanceVk;

    /// Vulkan Shader, should be created implicitly through `ShaderModule::create`.
    class ShaderModuleVk final : public ShaderModule {
    public:
        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        ShaderModuleVk(InstanceVk* instance, ShaderStage stage, size_t size, const u32* spirvCode);
        ~ShaderModuleVk() final;

        [[nodiscard]] inline VkShaderModule getShaderModuleVk()
        {
            return module;
        }

    private:
        POM_NOCOPY(ShaderModuleVk);

        InstanceVk* instance;
        VkShaderModule module;
    };

    class ShaderVk final : public Shader {
    public:
        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        ShaderVk(bool takeOwnership, std::initializer_list<ShaderModule*> modules);
        ~ShaderVk();

        [[nodiscard]] inline size_t getNumModules() const final
        {
            return modules.size();
        }

        [[nodiscard]] inline const VkPipelineShaderStageCreateInfo* getShaderStagesVk() const
        {
            return shaderStageCreateInfos.data();
        }

    private:
        POM_NOCOPY(ShaderVk);

        // InstanceVk* instance;
        bool ownsModules;
        std::vector<ShaderModuleVk*> modules;
        std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
    };

    /// @}
} // namespace pom::gfx