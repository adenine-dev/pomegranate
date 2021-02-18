#pragma once

#include <SDL.h>

#include "debug/logging.hpp"
#include "inputEvent.hpp"
#include "maths/maths.hpp"

namespace ce {

    /// @ingroup platform
    /// @brief Represents either a window (on desktop platforms) or surface (on mobile platforms)
    class Window {
    public:
        /// Event handler function pointer, provided for convenience.
        using EventHandler = void (*)(const InputEvent&);

    public:
        ///
        /// @param title: Initial title (the text that goes in the titlebar, if present) of the window.
        /// @param position: Initial position of the window, if it is negative then it will be left to the operating
        /// system to decide where it is located.
        /// @param size: Initial size of the window, if it is negative then it will default to 720x480, this behavior is
        /// subject to change.
        Window(const char* title, const maths::ivec2& position = { -1, -1 }, const maths::ivec2& size = { -1, -1 });
        ~Window();

        /// Returns true if the operating system or user says that this window should close, this should be checked each
        /// update and obeyed. The exception being if a window is tied to some state that the user may not want to lose.
        inline bool shouldClose() const
        {
            return closeRequested;
        }

        /// Sets the function that will be called whenever this window receives an event. Will assert if invalid.
        inline void setEventHandler(EventHandler fn)
        {
            CE_ASSERT(fn, "Attempting to pass an invalid function pointer to `Window::setEventHandler`");
            callbackFn = fn;
        }

    public:
        /// Polls the platform for events that have happened in the past update, should be called every update.
        static void pollEvents();

    private:
        SDL_Window* windowHandle = nullptr;
        bool closeRequested = false;

        EventHandler callbackFn = [](const InputEvent&) {};

    private:
        static constexpr const char* CHIONE_SDL_WINDOW_PTR = "CE_WIN";
        static bool initializedSDL;
    };
} // namespace ce