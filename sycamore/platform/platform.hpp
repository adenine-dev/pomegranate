#pragma once

#ifdef SM_PLATFORM_WINDOWS
/// \def SM_DEBUGBREAK
/// debugbreak for debugging breaks.
#    define SM_DEBUGBREAK() __debugbreak()
#endif

namespace sm { namespace platform {

    /// Opens a platform based error window, separate from all other info. Useful for throwing fatal errors in
    /// production, when that is needed.
    void openErrorWindow(const char* title, const char* body);

}} // namespace sm::platform