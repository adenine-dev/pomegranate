#include "pch.hpp"

#include "platform/inputEvent.hpp"
#include "window.hpp"

namespace pom {

    bool Window::initializedSDL = false;

    Window::Window(const char* title, const maths::ivec2& position, const maths::ivec2& size)
    {
        constexpr int DEFAULT_WIDTH = 720;
        constexpr int DEFAULT_HEIGHT = 480;

        if (!initializedSDL) {
            if (SDL_Init(SDL_INIT_VIDEO) != 0)
                POM_ASSERT(false, "Unable to initialize SDL. error: ", SDL_GetError());

            // maybe make this an option?
            SDL_SetHint(SDL_HINT_WINDOWS_NO_CLOSE_ON_ALT_F4, "1");

            initializedSDL = true;

            // FIXME: this code provides continuous events to both window resize and move, which
            // SDL2 does not do by default. Make sure this is safe.
            SDL_AddEventWatch(
                [](void*, SDL_Event* e) -> int {
                    if (e->type == SDL_WINDOWEVENT) {
                        Window* self = static_cast<Window*>(
                            SDL_GetWindowData(SDL_GetWindowFromID(e->window.windowID),
                                              POM_SDL_WINDOW_PTR));

                        switch (e->window.event) {
                        case SDL_WINDOWEVENT_MOVED: {
                            self->callbackFn(
                                { .type = InputEventType::WINDOW_MOVE,
                                  .sourceWindow = self,
                                  .windowMoveData = { e->window.data1, e->window.data2 } });

                        } break;
                        case SDL_WINDOWEVENT_RESIZED: {
                            self->callbackFn(
                                { .type = InputEventType::WINDOW_RESIZE,
                                  .sourceWindow = self,
                                  .windowResizeData = { e->window.data1, e->window.data2 } });

                        } break;
                        }
                    }

                    return 0;
                },
                nullptr);
        }

        // TODO: load previous size/position values from file.

        if (windowHandle = SDL_CreateWindow(title,
                                            position.x < 0 ? SDL_WINDOWPOS_UNDEFINED : position.x,
                                            position.y < 0 ? SDL_WINDOWPOS_UNDEFINED : position.y,
                                            size.x < 0 ? DEFAULT_WIDTH : size.x,
                                            size.y < 0 ? DEFAULT_HEIGHT : size.y,
                                            SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
            !windowHandle) {
            POM_ASSERT(false, "Unable to initialize Window. error: ", SDL_GetError());
        }

        SDL_SetWindowData(windowHandle, POM_SDL_WINDOW_PTR, (void*)this);
    }

    Window::~Window()
    {
        if (windowHandle)
            SDL_DestroyWindow(windowHandle);
    }

    void Window::pollEvents()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type) {
            case SDL_WINDOWEVENT: {
                Window* self = static_cast<Window*>(
                    SDL_GetWindowData(SDL_GetWindowFromID(e.window.windowID), POM_SDL_WINDOW_PTR));
                switch (e.window.event) {
                case SDL_WINDOWEVENT_CLOSE: {
                    self->closeRequested = true;
                    self->callbackFn({ .type = InputEventType::WINDOW_CLOSE,
                                       .sourceWindow = self,
                                       .windowCloseData = {} });
                } break;

                    // See above, this preserves default behavior
                    /* case SDL_WINDOWEVENT_MOVED: {
                        self->callbackFn({ .type = InputEventType::WINDOW_MOVE,
                                           .sourceWindow = self,
                                           .windowMoveData = { e.window.data1, e.window.data2 } });

                    } break;
                    case SDL_WINDOWEVENT_RESIZED: {
                        self->callbackFn({ .type = InputEventType::WINDOW_RESIZE,
                                           .sourceWindow = self,
                                           .windowResizeData = { e.window.data1, e.window.data2 }
                    });

                    } break; */

                case SDL_WINDOWEVENT_FOCUS_GAINED: {
                    self->callbackFn({ .type = InputEventType::WINDOW_FOCUS,
                                       .sourceWindow = self,
                                       .windowFocusData = {} });
                } break;
                case SDL_WINDOWEVENT_FOCUS_LOST: {
                    self->callbackFn({ .type = InputEventType::WINDOW_BLUR,
                                       .sourceWindow = self,
                                       .windowBlurData = {} });

                } break;
                }
            } break;
            case SDL_MOUSEMOTION: {
                Window* self = static_cast<Window*>(
                    SDL_GetWindowData(SDL_GetWindowFromID(e.window.windowID), POM_SDL_WINDOW_PTR));

                self->callbackFn({ .type = InputEventType::MOUSE_MOVE,
                                   .sourceWindow = self,
                                   .mouseMoveData = { e.motion.x, e.motion.y } });
            } break;
            case SDL_MOUSEBUTTONDOWN: {
                Window* self = static_cast<Window*>(
                    SDL_GetWindowData(SDL_GetWindowFromID(e.window.windowID), POM_SDL_WINDOW_PTR));

                self->callbackFn({ .type = InputEventType::MOUSE_DOWN,
                                   .sourceWindow = self,
                                   .mouseDownData = { static_cast<MouseButton>(e.button.button),
                                                      e.button.clicks } });
            } break;
            case SDL_MOUSEBUTTONUP: {
                Window* self = static_cast<Window*>(
                    SDL_GetWindowData(SDL_GetWindowFromID(e.window.windowID), POM_SDL_WINDOW_PTR));

                self->callbackFn({ .type = InputEventType::MOUSE_UP,
                                   .sourceWindow = self,
                                   .mouseUpData = { static_cast<MouseButton>(e.button.button),
                                                    e.button.clicks } });
            } break;
            case SDL_MOUSEWHEEL: {
                Window* self = static_cast<Window*>(
                    SDL_GetWindowData(SDL_GetWindowFromID(e.window.windowID), POM_SDL_WINDOW_PTR));

                self->callbackFn({ .type = InputEventType::MOUSE_SCROLL,
                                   .sourceWindow = self,
                                   .mouseScrollData = { e.wheel.x, e.wheel.y } });
            } break;
            case SDL_KEYDOWN: {
                Window* self = static_cast<Window*>(
                    SDL_GetWindowData(SDL_GetWindowFromID(e.window.windowID), POM_SDL_WINDOW_PTR));
                self->callbackFn({ .type = InputEventType::KEY_DOWN,
                                   .sourceWindow = self,
                                   .keyDownData = { static_cast<KeyHid>(e.key.keysym.scancode),
                                                    static_cast<Keycode>(e.key.keysym.sym),
                                                    e.key.repeat > 0 } });
            } break;
            case SDL_KEYUP: {
                Window* self = static_cast<Window*>(
                    SDL_GetWindowData(SDL_GetWindowFromID(e.window.windowID), POM_SDL_WINDOW_PTR));
                self->callbackFn({ .type = InputEventType::KEY_UP,
                                   .sourceWindow = self,
                                   .keyUpData = { static_cast<KeyHid>(e.key.keysym.scancode),
                                                  static_cast<Keycode>(e.key.keysym.sym),
                                                  e.key.repeat > 0 } });
            } break;
            };
        }
    }
} // namespace pom