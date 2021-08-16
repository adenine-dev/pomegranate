#include "pch.hpp"

#include "shaderVk.hpp"

#include "gfxVk.hpp"
#include "instanceVk.hpp"
#include "typesVk.hpp"

namespace pom::gfx {
    ShaderModuleVk::ShaderModuleVk(InstanceVk* instance, ShaderStage stage, size_t size, const u32* spirvCode) :
        ShaderModule(stage), instance(instance), spirv(spirvCode, spirvCode + size)
    {
        VkShaderModuleCreateInfo shaderModuleCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .codeSize = size,
            .pCode = spirvCode,
        };

        POM_CHECK_VK(vkCreateShaderModule(instance->getDevice(), &shaderModuleCreateInfo, nullptr, &module),
                     "Failed to create shader module");
    }

    ShaderModuleVk::~ShaderModuleVk()
    {
        vkDestroyShaderModule(instance->getDevice(), module, nullptr);
    }

    ShaderVk::ShaderVk(bool takeOwnership, std::initializer_list<ShaderModule*> shaderModules) :
        ownsModules(takeOwnership), modules(shaderModules.size()), shaderStageCreateInfos(shaderModules.size())
    {
        u32 i = 0;
        for (const auto& shaderModule : shaderModules) {
            POM_ASSERT(shaderModule->getAPI() == GraphicsAPI::VULKAN,
                       "Attempting to use ShaderModule with non-matching api.");
            modules[i] = dynamic_cast<ShaderModuleVk*>(shaderModule);
            shaderStageCreateInfos[i] = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stage = toVkShaderStageFlagBits(modules[i]->getStage()),
                .module = modules[i]->getShaderModuleVk(),
                .pName = "main", // TODO: make this configurable
                .pSpecializationInfo = nullptr,
            };

            i++;
        }
    }

    ShaderVk::~ShaderVk()
    {
        if (ownsModules) {
            for (auto& module : modules) {
                delete module;
            }
        }
    }

} // namespace pom::gfx