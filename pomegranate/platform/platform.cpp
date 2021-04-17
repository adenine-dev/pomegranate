#include "pch.hpp"

#include "platform.hpp"

namespace pom { namespace platform {
    void openErrorWindow(const char* title, const char* body)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, body, 0);
    }
}} // namespace pom::platform