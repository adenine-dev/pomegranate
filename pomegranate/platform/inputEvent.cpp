#include "pch.hpp"

#include "debug/logging.hpp"

#include "inputEvent.hpp"

namespace pom {
    std::ostream& operator<<(std::ostream& os, const InputEventType& type)
    {
        switch (type) {
        case InputEventType::NONE: {
            os << "InputEventType::NONE";
        } break;
        case InputEventType::WINDOW_CLOSE: {
            os << "InputEventType::WINDOW_CLOSE";
        } break;
        case InputEventType::WINDOW_MOVE: {
            os << "InputEventType::WINDOW_MOVE";
        } break;
        case InputEventType::WINDOW_RESIZE: {
            os << "InputEventType::WINDOW_RESIZE";
        } break;
        case InputEventType::WINDOW_FOCUS: {
            os << "InputEventType::WINDOW_FOCUS";
        } break;
        case InputEventType::WINDOW_BLUR: {
            os << "InputEventType::WINDOW_BLUR";
        } break;
        case InputEventType::WINDOW_MINIMIZE: {
            os << "InputEventType::WINDOW_MINIMIZE";
        } break;
        case InputEventType::WINDOW_MAXIMIZE: {
            os << "InputEventType::WINDOW_MAXIMIZE";
        } break;
        case InputEventType::MOUSE_MOVE: {
            os << "InputEventType::MOUSE_MOVE";
        } break;
        case InputEventType::MOUSE_DOWN: {
            os << "InputEventType::MOUSE_DOWN";
        } break;
        case InputEventType::MOUSE_UP: {
            os << "InputEventType::MOUSE_UP";
        } break;
        case InputEventType::MOUSE_SCROLL: {
            os << "InputEventType::MOUSE_SCROLL";
        } break;
        case InputEventType::KEY_DOWN: {
            os << "InputEventType::KEY_DOWN";
        } break;
        case InputEventType::KEY_UP: {
            os << "InputEventType::KEY_UP";
        } break;
        default: {
            os << "Uknown InputEventType";
        }
        }

        return os;
    }

    const maths::ivec2& InputEvent::getPosition() const
    {
        switch (type) {
        case InputEventType::WINDOW_MOVE: {
            return windowMoveData.position;
        } break;
        case InputEventType::MOUSE_MOVE: {
            return mouseMoveData.position;
        } break;
        default: {
            POM_FATAL("Attempting to get position for an event of type: ", type);

            // prevent warning, it is undefined behavior but it will never run so its only mostly
            // dumb :p
            return windowMoveData.position;
        }
        }
    }
    const maths::ivec2& InputEvent::getSize() const
    {
        POM_ASSERT(type == InputEventType::WINDOW_RESIZE, "Attempting to get size for an event of type: ", type);
        return windowResizeData.size;
    }

    const maths::ivec2& InputEvent::getDelta() const
    {
        POM_ASSERT(type == InputEventType::MOUSE_SCROLL, "Attempting to get deltas for an event of type: ", type);
        return mouseScrollData.delta;
    }

    const MouseButton& InputEvent::getMouseButton() const
    {
        switch (type) {
        case InputEventType::MOUSE_DOWN: {
            return mouseDownData.button;
        } break;
        case InputEventType::MOUSE_UP: {
            return mouseUpData.button;
        } break;
        default: {
            POM_FATAL("Attempting to get mouse button for an event of type: ", type);

            // see line 66.
            return mouseDownData.button;
        }
        }
    }

    const u8& InputEvent::getRepeatCount() const
    {
        switch (type) {
        case InputEventType::MOUSE_DOWN: {
            return mouseDownData.repeatCount;
        } break;
        case InputEventType::MOUSE_UP: {
            return mouseUpData.repeatCount;
        } break;
        default: {
            POM_FATAL("Attempting to get mouse repeat count for an event of type: ", type);

            // see line 66.
            return mouseDownData.repeatCount;
        }
        }
    }

    const KeyHid& InputEvent::getHid() const
    {
        switch (type) {
        case InputEventType::KEY_DOWN: {
            return keyDownData.hid;
        } break;
        case InputEventType::KEY_UP: {
            return keyUpData.hid;
        } break;
        default: {
            POM_FATAL("Attempting to get keyboard hardware id for an event of type: ", type);

            // see line 66.
            return keyDownData.hid;
        }
        }
    }

    const Keycode& InputEvent::getKeycode() const
    {
        switch (type) {
        case InputEventType::KEY_DOWN: {
            return keyDownData.code;
        } break;
        case InputEventType::KEY_UP: {
            return keyUpData.code;
        } break;
        default: {
            POM_FATAL("Attempting to get keycode for an event of type: ", type);

            // see line 66.
            return keyDownData.code;
        }
        }
    }

    const bool& InputEvent::isRepeated() const
    {
        switch (type) {
        case InputEventType::KEY_DOWN: {
            return keyDownData.repeated;
        } break;
        case InputEventType::KEY_UP: {
            return keyUpData.repeated;
        } break;
        default: {
            POM_FATAL("Attempting to check repeated key press state for an event of type: ", type);

            // see line 66.
            return keyDownData.repeated;
        }
        }
    }

    POM_API std::ostream& operator<<(std::ostream& os, const InputEvent& event)
    {
        switch (event.type) {
        case InputEventType::NONE: {
            os << "None Event (Something has gone terribly wrong)";
        } break;
        case InputEventType::WINDOW_CLOSE: {
            os << "Window Close Event";
        } break;
        case InputEventType::WINDOW_MOVE: {
            os << "Window Move Event { position: " << event.windowMoveData.position << " }";
        } break;
        case InputEventType::WINDOW_RESIZE: {
            os << "Window Resize Event { size: " << event.windowResizeData.size << " }";
        } break;
        case InputEventType::WINDOW_FOCUS: {
            os << "Window Focus Event";
        } break;
        case InputEventType::WINDOW_BLUR: {
            os << "Window Blur Event";
        } break;
        case InputEventType::WINDOW_MINIMIZE: {
            os << "Window Minimize Event";
        } break;
        case InputEventType::WINDOW_MAXIMIZE: {
            os << "Window Maximize Event";
        } break;
        case InputEventType::MOUSE_MOVE: {
            os << "Mouse Move Event { position: " << event.mouseMoveData.position << " }";
        } break;
        case InputEventType::MOUSE_DOWN: {
            os << "Mouse Down Event { button: " << event.mouseDownData.button
               << ", repeatCount: " << +event.mouseDownData.repeatCount << " }";
        } break;
        case InputEventType::MOUSE_UP: {
            os << "Mouse Up Event { button: " << event.mouseUpData.button
               << ", repeatCount: " << +event.mouseUpData.repeatCount << " }";
        } break;
        case InputEventType::MOUSE_SCROLL: {
            os << "Mouse Scroll Event { delta: " << event.mouseScrollData.delta << " }";
        } break;
        case InputEventType::KEY_DOWN: {
            os << "Key Down Event { hid: " << event.keyDownData.hid << ", code: " << event.keyDownData.code
               << ", repeated: " << event.keyDownData.repeated << " }";
        } break;
        case InputEventType::KEY_UP: {
            os << "Key Up Event { hid: " << event.keyUpData.hid << ", code: " << event.keyUpData.code
               << ", repeated: " << event.keyUpData.repeated << " }";
        } break;
        }

        return os;
    }

} // namespace pom