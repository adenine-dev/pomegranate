#include "pomegranate.hpp"

#include <SDL.h>

namespace pom {
    void hello()
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "heck", "heck again", nullptr);
    }
} // namespace pom