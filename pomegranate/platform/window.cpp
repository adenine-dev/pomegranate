#include "window.hpp"

#include <SDL.h>

namespace pom {
    Window::Window()
    {
        assert(SDL_WasInit(SDL_INIT_VIDEO) || SDL_Init(SDL_INIT_VIDEO) == 0,
               "Could not initialize SDL video. SDL Error: {}",
               SDL_GetError());

        assert((handle = SDL_CreateWindow("heck",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          720,
                                          480,
                                          SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE)),
               "Could not create a window. SDL Error: {}",
               SDL_GetError());

        SDL_SetWindowData(handle, WINDOWDATA_NAME, this);
    }

    Window::~Window()
    {
        SDL_DestroyWindow(handle);
    }

    void Window::processEvent(const WindowEvent &event)
    {
        debug("{}", event);

        if (event.type == WindowEventType::WINDOW_CLOSE) {
            closeRequested = true;
        }
    }

} // namespace pom