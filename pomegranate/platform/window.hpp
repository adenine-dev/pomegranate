#pragma once

#include "base.hpp"

#include "inputTypes.hpp"

#include "maths/vector.hpp"

struct SDL_Window;

namespace pom {

    class Window;

    /// The types of events that can be processed by a window.
    enum WindowEventType {
        WINDOW_FOCUS,    ///< The window has gained focus
        WINDOW_BLUR,     ///< The window has lost focus.
        WINDOW_MOVE,     ///< The window has been moved. (only relevant on platforms where this makes sense)
        WINDOW_RESIZE,   ///< The window has been resized. (only relevant on platforms where this makes sense)
        WINDOW_MINIMIZE, ///< The window has been minimized. (only relevant on platforms where this makes sense)
        WINDOW_CLOSE,    ///< A close has been requested.

        MOUSE_ENTER,  ///< The mouse has entered the window.
        MOUSE_LEAVE,  ///< The mouse has left the window.
        MOUSE_MOVE,   ///< The mouse has moved over the window.
        MOUSE_SCROLL, ///< The mouse has scrolled over the window.
        MOUSE_DOWN,   ///< A mouse button has been pressed over the window.
        MOUSE_UP,     ///< A mouse button has been released over the window.
    };

    /// A strong union of all possible window events. To process this, a conditional should be made with the `type`
    /// field, and then events processed accordingly based on its value.
    struct WindowEvent {
        /// The type of the event, attempting to access a data field that does not correspond to the type of event
        /// specified is undefined behavior.
        WindowEventType type;

        /// A pointer to the window that the event fired on.
        Window *sourceWindow;

        union {
            struct WindowFocusData {
            } windowFocus;

            struct WindowBlurData {
            } windowBlur;

            struct WindowMoveData {
                Vector<i32, 2> position;
            } windowMove;

            struct WindowResizeData {
                Vector<i32, 2> size;
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
                Vector<i32, 2> position;
            } mouseMove;

            struct MouseScroll {
                Vector<i32, 2> delta;
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

    struct WindowCreateInfo {
        // NOTE: these are SDL's don't care values.
        static constexpr Vector<u32, 2> POSITION_DONT_CARE = {0x1FFF0000u, 0x1FFF0000u};

        const char *title = "Pomegranate Window";
        Vector<u32, 2> size = Vector<u32, 2>(720, 480);
        Vector<i32, 2> position = POSITION_DONT_CARE;
    };

    /// Creates and manages a window on the platform.
    class Window {
    public:
        Window(const WindowCreateInfo &createInfo);
        ~Window();

        /// Returns true if the user has requested the window close by for example pressing the close button. Client
        /// should close the window if this is true.
        [[nodiscard]] bool shouldClose() { return closeRequested; }

    private:
        SDL_Window *handle = nullptr;
        bool closeRequested = false;

        friend void pollEvents();
        void processEvent(const WindowEvent &event);
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
            str = fmt::format("WindowEvent ({}) {{ position: {} }}", event.type, event.windowMove.position);
        } break;
        case pom::WindowEventType::WINDOW_RESIZE: {
            str = fmt::format("WindowEvent ({}) {{ size: {} }}", event.type, event.windowResize.size);
        } break;
        case pom::WindowEventType::MOUSE_MOVE: {
            str = fmt::format("WindowEvent ({}) {{ position: {} }}", event.type, event.mouseMove.position);
        } break;
        case pom::WindowEventType::MOUSE_SCROLL: {
            str = fmt::format("WindowEvent ({}) {{ delta: {}, flipped: {} }}",
                              event.type,
                              event.mouseScroll.delta,
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