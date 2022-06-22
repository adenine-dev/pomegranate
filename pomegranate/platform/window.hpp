#pragma once

#include "base.hpp"

struct SDL_Window;

namespace pom {

    enum WindowEventType {
        FOCUS,
        BLUR,
        MOVE,
        RESIZE,
        MINIMIZE,
        CLOSE,
    };

    struct WindowEvent {
        WindowEventType type;

        union {
            struct FocusData {
            } focus;

            struct BlurData {
            } blur;

            struct MoveData {
                i32 x, y;
            } move;

            struct ResizeData {
                i32 width, height;
            } resize;

            struct MinimizeData {
            } minimize;

            struct CloseData {
            } close;
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
struct fmt::formatter<pom::WindowEventType> : fmt::formatter<fmt::string_view> {
    template <typename FormatContext>
    auto format(pom::WindowEventType type, FormatContext &ctx) -> decltype(ctx.out())
    {
        fmt::string_view name = "unknown WindowEventType";
        switch (type) {
        case pom::WindowEventType::FOCUS: {
            name = "WindowEventType::FOCUS";
        } break;
        case pom::WindowEventType::BLUR: {
            name = "WindowEventType::BLUR";
        } break;
        case pom::WindowEventType::MOVE: {
            name = "WindowEventType::MOVE";
        } break;
        case pom::WindowEventType::RESIZE: {
            name = "WindowEventType::RESIZE";
        } break;
        case pom::WindowEventType::MINIMIZE: {
            name = "WindowEventType::MINIMIZE";
        } break;
        case pom::WindowEventType::CLOSE: {
            name = "WindowEventType::CLOSE";
        } break;
        }
        return formatter<string_view>::format(name, ctx);
    }
};

template <>
struct fmt::formatter<pom::WindowEvent> : fmt::formatter<fmt::string_view> {
    template <typename FormatContext>
    auto format(const pom::WindowEvent &event, FormatContext &ctx) -> decltype(ctx.out())
    {
        switch (event.type) {
        case pom::WindowEventType::FOCUS: 
        case pom::WindowEventType::BLUR: 
        case pom::WindowEventType::MINIMIZE: 
        case pom::WindowEventType::CLOSE: {
            return fmt::format_to(ctx.out(), "WindowEvent ({}) {{}}", event.type);
        } break;
        case pom::WindowEventType::MOVE: {
            return fmt::format_to(ctx.out(),
                                  "WindowEvent ({}) {{ x: {}, y: {} }}",
                                  event.type,
                                  event.move.x,
                                  event.move.y);
        } break;
        case pom::WindowEventType::RESIZE: {
            return fmt::format_to(ctx.out(),
                                  "WindowEvent ({}) {{ width: {}, height: {} }}",
                                  event.type,
                                  event.resize.width,
                                  event.resize.height);
        } break;
        }
        return formatter<string_view>::format("unknown WindowEvent type", ctx);
    }
};