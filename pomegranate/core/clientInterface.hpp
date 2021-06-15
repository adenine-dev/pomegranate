#pragma once

namespace pom {
    /// @addtogroup core
    /// @{

    /// @addtogroup client_interface Client Interface
    /// Pomegranate is structured into a runtime and a client. The client is per-game native code. In theory it can be
    /// written in any language that can compile to a shared library and export the following symbols. However the
    /// client is expected to be a non-script (ie faster) interface with Pomegranate. So it is recommended to use either
    /// C or C++.
    /// @{

    /// The type of the `clientBegin` function which **must** be defined in the client code. It is called once during
    /// the setup of the application.
    using ClientBeginFunction = void (*)();

    /// The type of the `clientMount` function which **may** be defined in the client code. It is called each time after
    /// the client is loaded, including the first time.
    using ClientMountFunction = void (*)();

    /// The type of the `clientUpdate` function which **must** be defined in the client code. It is called each update.
    using ClientUpdateFunction = void (*)(f32 deltatime);

    /// The type of the `clientUnmount` function which **may** be defined in the client code. It is called each time
    /// before the client is unloaded.
    using ClientUnmountFunction = void (*)();

    /// The type of the `clientEnd` function which **must** be defined in the client code. It is called once during
    /// the destruction of the application, before any other assets are freed.
    using ClientEndFunction = void (*)();

    /// @}
    /// @}

} // namespace pom