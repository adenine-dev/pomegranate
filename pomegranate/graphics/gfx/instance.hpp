#pragma once

#include "base.hpp"

#include "gfx.hpp"

#include "platform/window.hpp"

namespace pom::gfx {
    /// @addtogroup gfx
    /// @{

    /// Corresponds to a hardware Graphics device, may not be strictly a GPU for example integrated CPU graphics.
    struct GPU {
        /// Human readable name of the device.
        const char* name;
        /// true if the gpu is dedicated, this usually correlates with better performance.
        bool discrete;
        /// Graphics API dependent handle.
        void* handle;
        // TODO: other device properties, esp capabilities.
    };

    class POM_API Instance {
    public:
        [[nodiscard]] constexpr virtual GraphicsAPI getAPI() const = 0;

        [[nodiscard]] const std::vector<GPU>& listGPUs() const
        {
            return GPUs;
        };

        virtual void determineGPU(Context* context) = 0;

        /// Returns true if the Instance is ready to be used. Currently this involves calling `selectGPU`.
        [[nodiscard]] virtual bool ready() const = 0;

        static Instance* create(const char* name, GraphicsAPI api);

        static void destroy();

        static Instance* get()
        {
            return singleton;
        };

    public:
        virtual ~Instance() = default;

        friend class InstanceVk;

        std::vector<GPU> GPUs;

        static Instance* singleton;
    };

    /// @}

} // namespace pom::gfx