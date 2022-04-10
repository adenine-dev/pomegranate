#pragma once

#include "base.hpp"

#include "gfx.hpp"

#include "platform/window.hpp"

namespace pom::gfx {
    /// @addtogroup gfx
    /// @{

#ifndef DOXYGEN // Hide this because technically it doesn't matter, the namespace is only really here so that the
    // members of the enum do not pollute the namespace.
    namespace impl {
#endif

        enum FrameBufferSamples : u8 {
            COUNT_1 = 1 << 0,
            COUNT_2 = 1 << 1,
            COUNT_4 = 1 << 2,
            COUNT_8 = 1 << 3,
            COUNT_16 = 1 << 4,
            COUNT_32 = 1 << 5,
            COUNT_64 = 1 << 6,
        };

#ifndef DOXYGEN
    } // namespace impl

    using FrameBufferSamples = impl::FrameBufferSamples;
#endif

    /// Corresponds to a hardware Graphics device, may not be strictly a GPU for example integrated CPU graphics.
    struct GPU {
        /// Human readable name of the device.
        std::string name;
        /// true if the gpu is dedicated, this usually correlates with better performance.
        bool discrete;
        /// Graphics API dependent handle.
        void* handle;
        /// Bitmask of available framebuffer sample counts.
        FrameBufferSamples frameBufferSampleCounts;
        // TODO: other device properties and capabilities
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

        virtual void update() = 0;

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