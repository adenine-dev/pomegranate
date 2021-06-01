#include "pch.hpp"

#include "platform.hpp"

namespace pom::platform {
    void openErrorWindow(const char* title, const char* body)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, body, nullptr);
    }
} // namespace pom::platform