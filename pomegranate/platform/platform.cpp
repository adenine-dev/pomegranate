#include "platform.hpp"

#include "SDL.h"

#include "window.hpp"

namespace pom {
    void showErrorMessage(const char *title, const char *message)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, NULL);
    }

    void pollEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_WINDOWEVENT: {
                SDL_Window *handle = SDL_GetWindowFromID(event.window.windowID);
                assert(handle, "Ill formed window event. SDL error: {}", SDL_GetError());
                Window *window = (Window *)SDL_GetWindowData(handle, Window::WINDOWDATA_NAME);

                switch (event.window.event) {
                case SDL_WINDOWEVENT_CLOSE: {
                    window->processEvent({.type = WindowEventType::CLOSE});
                } break;
                case SDL_WINDOWEVENT_FOCUS_GAINED: {
                    window->processEvent({.type = WindowEventType::FOCUS});
                } break;
                case SDL_WINDOWEVENT_FOCUS_LOST: {
                    window->processEvent({.type = WindowEventType::BLUR});
                } break;

                case SDL_WINDOWEVENT_RESIZED: {
                    window->processEvent({
                        .type = WindowEventType::RESIZE,
                        .resize = {.width = event.window.data1, .height = event.window.data2}
                    });
                } break;
                case SDL_WINDOWEVENT_MOVED: {
                    window->processEvent({
                        .type = WindowEventType::MOVE,
                        .move = {.x = event.window.data1, .y = event.window.data2}
                    });
                } break;
                case SDL_WINDOW_MINIMIZED: {
                    window->processEvent({.type = WindowEventType::MINIMIZE});
                }
                }
            } break;
            }
        }
    }
} // namespace pom