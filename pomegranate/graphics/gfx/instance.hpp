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

    /// Singleton information associated with a graphics API.
    class POM_API Instance {
    public:
        /// Returns the GraphicsAPI associated with this instance.
        [[nodiscard]] constexpr virtual GraphicsAPI getAPI() const = 0;

        /// Returns a list of the available and valid GPUs, may not include every GPU on a given system if a GPU does
        /// not have the needed capabilities.
        [[nodiscard]] const std::vector<GPU>& listGPUs() const
        {
            return GPUs;
        };

        /// Determines the GPU and finishes initializing the Instance, only needs to be called once.
        virtual void determineGPU(Context* context) = 0;

        /// Returns true if the Instance is ready to be used. Currently this involves calling `determineGPU`.
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