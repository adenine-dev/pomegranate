#include <pomegranate/pomegranate.hpp>

#include "embed/basic_frag_spv.hpp"
#include "embed/basic_vert_spv.hpp"

#include <spirv_glsl.hpp>
#include <spirv_hlsl.hpp>
#include <spirv_reflect.hpp>

struct GameState {
};

POM_CLIENT_EXPORT const pom::AppCreateInfo* clientGetAppCreateInfo(int /*argc*/, char** /*argv*/)
{
    static const pom::AppCreateInfo aci = {
        .name = "SPIRV cross tests",
    };

    return &aci;
}

POM_CLIENT_EXPORT GameState* clientCreateState()
{
    auto* gc = new GameState;

    return gc;
}

POM_CLIENT_EXPORT void clientBegin(GameState* gamestate)
{
    spirv_cross::CompilerReflection compiler(reinterpret_cast<const u32*>(basic_vert_spv_data),
                                             basic_vert_spv_size / sizeof(u32));

    // The SPIR-V is now parsed, and we can perform reflection on it.
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    // Get all sampled images in the shader.
    for (auto& resource : resources.stage_inputs) {
        u32 set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
        u32 binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
        u32 location = compiler.get_decoration(resource.id, spv::DecorationLocation);
        POM_DEBUG(resource.name, ", ", set, ", ", binding, ", ", location);
    }
}

POM_CLIENT_EXPORT void clientMount(GameState* gamestate)
{
}

POM_CLIENT_EXPORT void clientUpdate(GameState* gamestate, pom::DeltaTime dt)
{
}

POM_CLIENT_EXPORT void clientOnInputEvent(GameState* gamestate, pom::InputEvent* ev)
{
}

POM_CLIENT_EXPORT void clientUnmount(GameState* gamestate)
{
}

POM_CLIENT_EXPORT void clientEnd(GameState* gamestate)
{

    delete gamestate;
}
