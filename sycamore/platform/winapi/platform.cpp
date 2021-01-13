#include "pch.hpp"

#include "../platform.hpp"
#include "SDL.h"

namespace sm { namespace platform {
    void openErrorWindow(const char* title, const char* body)
    {
        // MessageBox(0, body, title, MB_ICONERROR | MB_OK);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, body, 0);
    }
}} // namespace sm::platform