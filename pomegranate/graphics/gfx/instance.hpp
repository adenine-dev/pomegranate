#pragma once

#include "base.hpp"

#include "gfx.hpp"

#include "platform/window.hpp"

namespace pom::gfx {
    /// @addtogroup gfx
    /// @{

    /// Capabilities of a given GPU, contains information on the variable abilities and attributes of the hardware.
    struct Capabilities {
        /// The number of vertex buffers that can be bound at once.
        u32 vertexInputBindings;
        // TODO: everything else...
    };

    /// Corresponds to a hardware Graphics device, may not be strictly a GPU for example integrated CPU graphics.
    struct GPU {
        /// Human readable name of the device.
        const char* name;
        /// true if the gpu is dedicated, this usually correlates with better performance.
        bool discrete;
        /// Graphics API dependent handle.
        void* handle;
        /// The capabilities of the GPU, see Capabilities.
        Capabilities capabilities;
        // TODO: other device properties.
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

        /// Returns the active GPU, only valid after calling `determineGPU`.
        [[nodiscard]] const GPU& getGPU() const
        {
            POM_ASSERT(ready(), "Attempting to get active GPU before the instance is ready.");
            return GPUs[selectedGPUIndex];
        };

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
        u32 selectedGPUIndex;

        static Instance* singleton;
    };

    /// @}

} // namespace pom::gfx