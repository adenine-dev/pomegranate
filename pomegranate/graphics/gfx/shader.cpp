#include "pch.hpp"

#include "shader.hpp"

#include "instance.hpp"

#include "vulkan/instanceVk.hpp"
#include "vulkan/shaderVk.hpp"

namespace pom::gfx {
    Ref<ShaderModule> ShaderModule::create(ShaderStage stage, size_t size, const u32* spirvCode)
    {
        POM_PROFILE_FUNCTION();
        switch (Instance::get()->getAPI()) {
        case GraphicsAPI::VULKAN: {
            return Ref<ShaderModule>(
                new ShaderModuleVk(dynamic_cast<InstanceVk*>(Instance::get()), stage, size, spirvCode));
        }
        }
    }

    ShaderModule::ShaderModule(ShaderStage stage) : stage(stage)
    {
    }

    Ref<Shader> Shader::create(std::initializer_list<Ref<ShaderModule>> modules)
    {
        POM_PROFILE_FUNCTION();
        switch (Instance::get()->getAPI()) {
        case GraphicsAPI::VULKAN: {
            return Ref<Shader>(new ShaderVk(modules));
        }
        }
    }

} // namespace pom::gfx