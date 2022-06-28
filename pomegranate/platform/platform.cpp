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
                    window->processEvent({.type = WindowEventType::WINDOW_CLOSE});
                } break;
                case SDL_WINDOWEVENT_FOCUS_GAINED: {
                    window->processEvent({.type = WindowEventType::WINDOW_FOCUS});
                } break;
                case SDL_WINDOWEVENT_FOCUS_LOST: {
                    window->processEvent({.type = WindowEventType::WINDOW_BLUR});
                } break;

                case SDL_WINDOWEVENT_RESIZED: {
                    window->processEvent({.type = WindowEventType::WINDOW_RESIZE,
                                          .windowResize = {.size = {event.window.data1, event.window.data2}}});
                } break;
                case SDL_WINDOWEVENT_MOVED: {
                    window->processEvent({.type = WindowEventType::WINDOW_MOVE,
                                          .windowMove = {.position = {event.window.data1, event.window.data2}}});
                } break;
                case SDL_WINDOW_MINIMIZED: {
                    window->processEvent({.type = WindowEventType::WINDOW_MINIMIZE});
                }
                case SDL_WINDOWEVENT_ENTER: {
                    window->processEvent({.type = WindowEventType::MOUSE_ENTER});
                } break;
                case SDL_WINDOWEVENT_LEAVE: {
                    window->processEvent({.type = WindowEventType::MOUSE_LEAVE});
                } break;
                }
            } break;
            case SDL_MOUSEMOTION: {
                // NOTE: this disables touch as mouse, maybe do this when touch is a thing?
                //  if (event.motion.which != SDL_TOUCH_MOUSEID) {
                SDL_Window *handle = SDL_GetWindowFromID(event.motion.windowID);
                assert(handle, "Ill formed window event. SDL error: {}", SDL_GetError());
                Window *window = (Window *)SDL_GetWindowData(handle, Window::WINDOWDATA_NAME);

                window->processEvent(
                    {.type = WindowEventType::MOUSE_MOVE, .mouseMove = {.position = {event.motion.x, event.motion.y}}});
            } break;
            case SDL_MOUSEWHEEL: {
                SDL_Window *handle = SDL_GetWindowFromID(event.wheel.windowID);
                assert(handle, "Ill formed window event. SDL error: {}", SDL_GetError());
                Window *window = (Window *)SDL_GetWindowData(handle, Window::WINDOWDATA_NAME);
                window->processEvent({
                    .type = WindowEventType::MOUSE_SCROLL,
                    .mouseScroll = {.delta = {event.wheel.x, event.wheel.y},
                                    .flipped = event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED}
                });
            } break;
            case SDL_MOUSEBUTTONDOWN: {
                SDL_Window *handle = SDL_GetWindowFromID(event.button.windowID);
                assert(handle, "Ill formed window event. SDL error: {}", SDL_GetError());
                Window *window = (Window *)SDL_GetWindowData(handle, Window::WINDOWDATA_NAME);

                window->processEvent({
                    .type = WindowEventType::MOUSE_DOWN,
                    .mouseDown = {.button = (MouseButton)event.button.button, .clicks = event.button.clicks}
                });
            } break;
            case SDL_MOUSEBUTTONUP: {
                SDL_Window *handle = SDL_GetWindowFromID(event.button.windowID);
                assert(handle, "Ill formed window event. SDL error: {}", SDL_GetError());
                Window *window = (Window *)SDL_GetWindowData(handle, Window::WINDOWDATA_NAME);

                window->processEvent({
                    .type = WindowEventType::MOUSE_UP,
                    .mouseUp = {.button = (MouseButton)event.button.button, .clicks = event.button.clicks}
                });
            } break;
            }
        }
    }
} // namespace pom