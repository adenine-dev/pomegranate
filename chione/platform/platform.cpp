#include "pch.hpp"

#include "platform.hpp"

namespace ce { namespace platform {
    void openErrorWindow(const char* title, const char* body)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, body, 0);
    }
}} // namespace ce::platform