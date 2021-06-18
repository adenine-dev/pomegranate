#pragma once

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
    /// POM_EXPORT pom::AppCreateInfo clientGetAppCreateInfo(int argc, char** argv)
    /// {
    ///     return { .name = "app name" };
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
        /// The name of the application. Used in several places.
        const char* name;
        // TODO: version? engine config? application type? icon?
    };

    /// The type of the `clientGetAppCreateInfo` function which **must** be defined in the client code. It is called
    /// once during the setup of the application before anything else in the engine is initialized. It should return an
    /// `ApplicationCreateInfo` structure. `argc` and `argv` are passed directly from those passed to the executable.
    using ClientGetCreateInfoFunction = AppCreateInfo (*)(int argc, char** argv);

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

    /// The type of the `clientUnmount` function which **may** be defined in the client code. It is called each time
    /// before the client is unloaded.
    using ClientUnmountFunction = void (*)(GameState* gamestate);

    /// The type of the `clientEnd` function which **must** be defined in the client code. It is called once during
    /// the destruction of the application, before any other assets are freed. It should free the data returned in the
    /// initial `clientCreateState`.
    using ClientEndFunction = void (*)(GameState* gamestate);

    /// @}
    /// @}

} // namespace pom