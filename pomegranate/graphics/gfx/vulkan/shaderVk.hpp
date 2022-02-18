#pragma once

#include "../shader.hpp"

namespace pom::gfx {
    /// @addtogroup vulkan
    /// @{

    class InstanceVk;

    /// Vulkan Shader Module, should be created implicitly through `ShaderModule::create`. Contains the vulkan shader
    /// module.
    class ShaderModuleVk final : public ShaderModule {
    public:
        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        ShaderModuleVk(InstanceVk* instance, ShaderStage stage, usize size, const u32* spirvCode);
        ~ShaderModuleVk() final;

        [[nodiscard]] inline VkShaderModule getShaderModuleVk()
        {
            return module;
        }

        [[nodiscard]] inline const std::vector<u32>& getSpirv() const
        {
            return spirv;
        }

    private:
        POM_NOCOPY(ShaderModuleVk);

        InstanceVk* instance;
        VkShaderModule module = VK_NULL_HANDLE;

        std::vector<u32> spirv;
    };

    /// Vulkan Shader, should be created implicitly through `Shader::create`. Contains a collection of vulkan shader
    /// modules and information on how to construct them into a pipeline.
    class ShaderVk final : public Shader {
    public:
        [[nodiscard]] constexpr GraphicsAPI getAPI() const final
        {
            return GraphicsAPI::VULKAN;
        }

        ShaderVk(std::initializer_list<Ref<ShaderModule>> modules);

        [[nodiscard]] inline usize getNumModules() const final
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
        std::vector<Ref<ShaderModuleVk>> modules;
        std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
    };

    /// @}
} // namespace pom::gfx