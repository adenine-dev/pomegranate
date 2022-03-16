#pragma once

#include "graphics/gfx/gfx.hpp"
#include "platform/inputEvent.hpp"
#include "platform/platform.hpp"
#include "timing.hpp"

namespace pom {
    using GameState = void;

    /// @addtogroup core
    /// @{

    /// @addtogroup client_interface Client Interface
    /// Pomegranate is structured into a runtime and a client. The client is per-game native code. In theory it can
    /// be written in any language that can compile to a shared library and export the following symbols. However
    /// the client is expected to be a non-script (ie faster) interface with Pomegranate. So it is recommended to
    /// use either C or C++. Each of the following types is the same as a function that can be included in this native
    /// code. An (incomplete) example of this is:
    /// ```cpp
    /// #include <pomegranate/pomegranate.hpp>
    ///
    /// POM_EXPORT const pom::AppCreateInfo* clientGetAppCreateInfo(int argc, char** argv)
    /// {
    ///     static const pom::AppCreateInfo aci = {
    ///         .name = "App Name",
    ///     };
    ///
    ///     return &aci;
    /// }
    ///
    /// POM_EXPORT void clientUpdate(GameState* gameState, f32 dt)
    /// {
    ///     // do game update things.
    /// }
    /// ```
    /// `POM_EXPORT` is used to define a function as `extern "C"` (to prevent name mangling) and depending on the
    /// platform explicitly exports it from the SO file.
    /// @{

    /// Contains client specific information about an application, returned from `ClientGetCreateInfoFunction`. Used in
    /// the initialization of the application.
    struct AppCreateInfo {
        /// The name of the application. Used in several places including the title of the main window.
        const char* name;

        /// `true` if the application should limit the number of updates per second (UPS). It is a good idea to set this
        /// to true because it will improve system wide performance. This *roughly* corresponds with FPS (frames per
        /// second). Common UPS include 24, 30, 60, 120, and 240.
        bool limitUpdateRate = true;

        /// The target number of UPS (updates per second) the actual UPS may be very slightly faster, or in the case of
        /// a poorly optimized program the actual UPS may be less than the target. Only applies if `limitUpdateRate` is
        /// set to `true`.
        u16 targetUPS = 60;

        /// Desired graphics API for the application's rendering systems.
        gfx::GraphicsAPI graphicsAPI = gfx::GraphicsAPI::VULKAN;

        /// `true` if VSync should be used, this does not override `targetUPS`.
        bool enableVsync = false;

        // TODO: version? engine config? application type? icon?
    };

    /// The type of the `clientGetAppCreateInfo` function which **must** be defined in the client code. It is called
    /// once during the setup of the application before most anything else. It should return an `ApplicationCreateInfo`
    /// structure. `argc` and `argv` are passed directly from those passed to the executable.
    using ClientGetCreateInfoFunction = const AppCreateInfo* (*)(int argc, char** argv);

    /// The type of the `clientCreateState` function which **must** be defined in the client code. It is called once
    /// during the setup of the application before `clientBegin`, and should return a pointer to a datastructure
    /// defining your game's state. This data should be freed in `clientEnd`.
    using ClientCreateStateFunction = void* (*)();

    /// The type of the `clientBegin` function which **must** be defined in the client code. It is called once
    /// during the setup of the application.
    using ClientBeginFunction = void (*)(GameState* gamestate);

    /// The type of the `clientMount` function which **may** be defined in the client code. It is called each time after
    /// the client is loaded, including the first time.
    using ClientMountFunction = void (*)(GameState* gamestate);

    /// The type of the `clientUpdate` function which **must** be defined in the client code. It is called each update.
    using ClientUpdateFunction = void (*)(GameState* gamestate, DeltaTime deltatime);

    /// The type of the `clientOnInputEvent` function which **must** be defined in the client code. It is called each
    /// time an input event from the main window is received.
    using ClientOnInputEventFunction = void (*)(GameState* gamestate, const InputEvent* ev);

    /// The type of the `clientUnmount` function which **may** be defined in the client code. It is called each time
    /// before the client is unloaded.
    using ClientUnmountFunction = void (*)(GameState* gamestate);

    /// The type of the `clientEnd` function which **must** be defined in the client code. It is called once during
    /// the destruction of the application, before any other assets are freed. It should free the data returned in the
    /// initial `clientCreateState`.
    using ClientEndFunction = void (*)(GameState* gamestate);

    /// Wrapper around the client for use in the Application, there is no reason for the client to use this.
    /// @private
    class POM_API Client {
    public:
        explicit Client(std::string clientFilename);
        ~Client();

        void begin();
        void update(DeltaTime dt);
        void onEvent(const pom::InputEvent* ev);
        void reload();

        void setSOFile(std::string filename);

    private:
        platform::SharedObject clientSO;

        GameState* gamestate = nullptr;

        POM_NOCOPY(Client);

        friend class Application;

        platform::SharedObject::SOFunction<ClientGetCreateInfoFunction> clientGetAppCreateInfo
            = clientSO.getFunction<ClientGetCreateInfoFunction>("clientGetAppCreateInfo");
        platform::SharedObject::SOFunction<ClientCreateStateFunction> clientCreateState
            = clientSO.getFunction<ClientCreateStateFunction>("clientCreateState");
        platform::SharedObject::SOFunction<ClientBeginFunction> clientBegin
            = clientSO.getFunction<ClientBeginFunction>("clientBegin");
        platform::SharedObject::SOFunction<ClientEndFunction> clientEnd
            = clientSO.getFunction<ClientEndFunction>("clientEnd");
        platform::SharedObject::SOFunction<ClientUpdateFunction> clientUpdate
            = clientSO.getFunction<ClientUpdateFunction>("clientUpdate");
        platform::SharedObject::SOFunction<ClientOnInputEventFunction> clientOnInputEvent
            = clientSO.getFunction<ClientOnInputEventFunction>("clientOnInputEvent");
        platform::SharedObject::SOFunction<ClientMountFunction> clientMount
            = clientSO.getFunction<ClientMountFunction>("clientMount");
        platform::SharedObject::SOFunction<ClientUnmountFunction> clientUnmount
            = clientSO.getFunction<ClientUnmountFunction>("clientUnmount");
    };

    /// @}
    /// @}

} // namespace pom