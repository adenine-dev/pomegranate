#pragma once

#include "base.hpp"

struct SDL_Window;

namespace pom {

    struct WindowEvent {
        bool windowClosed;
    };

    class Window {
    public:
        Window();
        ~Window();

        [[nodiscard]] bool shouldClose() { return closeRequested; }

        void processEvent(const WindowEvent &event);

    private:
        SDL_Window *handle = nullptr;
        bool closeRequested = false;

        friend void pollEvents();
        static inline const char *WINDOWDATA_NAME = "pomegranate_window_data_ptr";
    };
} // namespace pom