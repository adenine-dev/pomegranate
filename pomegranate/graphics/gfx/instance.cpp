#include "pch.hpp"

#include "instance.hpp"

#include "vulkan/instanceVk.hpp"

namespace pom::gfx {
    Instance* Instance::singleton = nullptr;

    Instance* Instance::create(const char* name, GraphicsAPI api)
    {
        POM_ASSERT(!singleton, "Attempting to create multiple instances.");

        switch (api) {
        case GraphicsAPI::VULKAN: {
            singleton = new InstanceVk(name);
        } break;
        default: {
            POM_FATAL("Unknown Graphics api");
        }
        }
        return singleton;
    }

    void Instance::destroy()
    {
        POM_ASSERT(singleton, "Attempting to destroy non-existent instance.");
        delete singleton;

        singleton = nullptr;
    }
} // namespace pom::gfx