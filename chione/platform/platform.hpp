#pragma once

#ifdef CE_PLATFORM_WINDOWS
#    define CE_DEBUGBREAK() __debugbreak()
#endif

namespace ce { namespace platform {

    /// @addtogroup platform
    /// @{

    /// Opens a platform based error window, separate from all other state. Useful for throwing fatal errors in
    /// production, when that is needed.
    void openErrorWindow(const char* title, const char* body);

    /// @}

}} // namespace ce::platform