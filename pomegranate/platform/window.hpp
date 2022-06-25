#pragma once

#include "base.hpp"

#include "inputTypes.hpp"

struct SDL_Window;

namespace pom {

    enum WindowEventType {
        WINDOW_FOCUS,
        WINDOW_BLUR,
        WINDOW_MOVE,
        WINDOW_RESIZE,
        WINDOW_MINIMIZE,
        WINDOW_CLOSE,

        MOUSE_ENTER,
        MOUSE_LEAVE,
        MOUSE_MOVE,
        MOUSE_SCROLL,
        MOUSE_DOWN,
        MOUSE_UP,
    };

    struct WindowEvent {
        WindowEventType type;

        union {
            struct WindowFocusData {
            } windowFocus;

            struct WindowBlurData {
            } windowBlur;

            struct WindowMoveData {
                i32 x, y;
            } windowMove;

            struct WindowResizeData {
                i32 width, height;
            } windowResize;

            struct WindowMinimizeData {
            } windowMinimize;

            struct WindowCloseData {
            } windowClose;

            struct MouseEnter {
            } mouseEnter;

            struct MouseLeave {
            } mouseLeave;

            struct MouseMove {
                i32 x, y;
            } mouseMove;

            struct MouseScroll {
                i32 deltaX, deltaY;
                bool flipped;
            } mouseScroll;

            struct MouseDown {
                MouseButton button;
                u8 clicks;
            } mouseDown;

            struct MouseUp {
                MouseButton button;
                u8 clicks;
            } mouseUp;
        };
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

template <>
struct fmt::formatter<pom::WindowEventType> : formatter<string_view> {
    template <typename FormatContext>
    auto format(pom::WindowEventType type, FormatContext &ctx)
    {
        string_view name = "unknown WindowEventType";
        switch (type) {
        case pom::WindowEventType::WINDOW_FOCUS: {
            name = "WindowEventType::WINDOW_FOCUS";
        } break;
        case pom::WindowEventType::WINDOW_BLUR: {
            name = "WindowEventType::WINDOW_BLUR";
        } break;
        case pom::WindowEventType::WINDOW_MOVE: {
            name = "WindowEventType::WINDOW_MOVE";
        } break;
        case pom::WindowEventType::WINDOW_RESIZE: {
            name = "WindowEventType::WINDOW_RESIZE";
        } break;
        case pom::WindowEventType::WINDOW_MINIMIZE: {
            name = "WindowEventType::WINDOW_MINIMIZE";
        } break;
        case pom::WindowEventType::WINDOW_CLOSE: {
            name = "WindowEventType::CLOSE";
        } break;

        case pom::WindowEventType::MOUSE_ENTER: {
            name = "WindowEventType::MOUSE_ENTER";
        } break;
        case pom::WindowEventType::MOUSE_LEAVE: {
            name = "WindowEventType::MOUSE_LEAVE";
        } break;
        case pom::WindowEventType::MOUSE_MOVE: {
            name = "WindowEventType::MOUSE_MOVE";
        } break;
        case pom::WindowEventType::MOUSE_SCROLL: {
            name = "WindowEventType::MOUSE_SCROLL";
        } break;
        case pom::WindowEventType::MOUSE_DOWN: {
            name = "WindowEventType::MOUSE_DOWN";
        } break;
        case pom::WindowEventType::MOUSE_UP: {
            name = "WindowEventType::MOUSE_UP";
        } break;
        }
        return formatter<string_view>::format(name, ctx);
    }
};

template <>
struct fmt::formatter<pom::WindowEvent> : formatter<std::string> {
    template <typename FormatContext>
    auto format(const pom::WindowEvent &event, FormatContext &ctx)
    {
        std::string str = "unknown WindowEvent type";
        switch (event.type) {
        case pom::WindowEventType::WINDOW_FOCUS:
        case pom::WindowEventType::WINDOW_BLUR:
        case pom::WindowEventType::WINDOW_MINIMIZE:
        case pom::WindowEventType::WINDOW_CLOSE:
        case pom::WindowEventType::MOUSE_ENTER:
        case pom::WindowEventType::MOUSE_LEAVE: {
            str = fmt::format("WindowEvent ({}) {{}}", event.type);
        } break;
        case pom::WindowEventType::WINDOW_MOVE: {
            str =
                fmt::format("WindowEvent ({}) {{ x: {}, y: {} }}", event.type, event.windowMove.x, event.windowMove.y);
        } break;
        case pom::WindowEventType::WINDOW_RESIZE: {
            str = fmt::format("WindowEvent ({}) {{ width: {}, height: {} }}",
                              event.type,
                              event.windowResize.width,
                              event.windowResize.height);
        } break;
        case pom::WindowEventType::MOUSE_MOVE: {
            str = fmt::format("WindowEvent ({}) {{ x: {}, y: {} }}", event.type, event.mouseMove.x, event.mouseMove.y);
        } break;
        case pom::WindowEventType::MOUSE_SCROLL: {
            str = fmt::format("WindowEvent ({}) {{ deltaX: {}, deltaY: {}, flipped: {} }}",
                              event.type,
                              event.mouseScroll.deltaX,
                              event.mouseScroll.deltaY,
                              event.mouseScroll.flipped);
        } break;
        case pom::WindowEventType::MOUSE_DOWN: {
            str = fmt::format("WindowEvent ({}) {{ button: {}, clicks: {} }}",
                              event.type,
                              event.mouseDown.button,
                              event.mouseDown.clicks);
        } break;
        case pom::WindowEventType::MOUSE_UP: {
            str = fmt::format("WindowEvent ({}) {{ button: {}, clicks: {} }}",
                              event.type,
                              event.mouseUp.button,
                              event.mouseUp.clicks);
        } break;
        }
        return formatter<std::string>::format(str, ctx);
    }
};