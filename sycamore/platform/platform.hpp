#pragma once

#ifdef SM_PLATFORM_WINDOWS
#    define SM_DEBUGBREAK() __debugbreak()
#endif

namespace sm { namespace platform {
    void openErrorWindow(const char* title, const char* body);
}} // namespace sm::platform