#include "pch.hpp"

#include "context.hpp"
#include "instance.hpp"

#include "vulkan/contextVk.hpp"

namespace pom::gfx {
    Context* Context::create(GraphicsAPI api, Window* window)
    {
        // Currently only allow one graphics context per run.
        POM_ASSERT(Instance::get()->getAPI() == api,
                   "Attempting to create a graphics context with non-matching Instance");

        switch (api) {
        case GraphicsAPI::VULKAN: {
            return new ContextVk(dynamic_cast<InstanceVk*>(Instance::get()), window);
        } break;
        default: {
            POM_FATAL("attempting to create context with unknown graphics api");
        }
        }
    }

    void Context::destroy(Context* context)
    {
        delete context;
    }

    Context::Context(Window* window) : window(window)
    {
    }

    Context::~Context() = default;

} // namespace pom::gfx