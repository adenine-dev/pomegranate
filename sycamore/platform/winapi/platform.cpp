#include "pch.hpp"

#include "../platform.hpp"

namespace sm { namespace platform {
    void openErrorWindow(const char* title, const char* body)
    {
        MessageBox(0, body, title, MB_ICONERROR | MB_OK);
    }
}} // namespace sm::platform