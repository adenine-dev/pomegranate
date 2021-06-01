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
            POM_ASSERT(false, "Attempting to get position for an event of type: ", type);

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
            POM_ASSERT(false, "Attempting to get mouse button for an event of type: ", type);

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
            POM_ASSERT(false, "Attempting to get mouse repeat count for an event of type: ", type);

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
            POM_ASSERT(false, "Attempting to get keyboard hardware id for an event of type: ", type);

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
            POM_ASSERT(false, "Attempting to get keycode for an event of type: ", type);

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
            POM_ASSERT(false, "Attempting to check repeated key press state for an event of type: ", type);

            // see line 66.
            return keyDownData.repeated;
        }
        }
    }

    void InputEvent::debugPrint() const
    {
        switch (type) {
        case InputEventType::NONE: {
            POM_LOG_WARN("None Event (Something has gone terribly wrong)");
        } break;
        case InputEventType::WINDOW_CLOSE: {
            POM_LOG_TRACE("Window Close Event");
        } break;
        case InputEventType::WINDOW_MOVE: {
            POM_LOG_TRACE("Window Move Event { position: ", windowMoveData.position, " }");
        } break;
        case InputEventType::WINDOW_RESIZE: {
            POM_LOG_TRACE("Window Resize Event { size: ", windowResizeData.size, " }");
        } break;
        case InputEventType::WINDOW_FOCUS: {
            POM_LOG_TRACE("Window Focus Event");
        } break;
        case InputEventType::WINDOW_BLUR: {
            POM_LOG_TRACE("Window Blur Event");
        } break;
        case InputEventType::MOUSE_MOVE: {
            POM_LOG_TRACE("Mouse Move Event { position: ", mouseMoveData.position, " }");
        } break;
        case InputEventType::MOUSE_DOWN: {
            POM_LOG_TRACE("Mouse Down Event { button: ",
                          mouseDownData.button,
                          ", repeatCount: ",
                          +mouseDownData.repeatCount,
                          " }");
        } break;
        case InputEventType::MOUSE_UP: {
            POM_LOG_TRACE("Mouse Up Event { button: ",
                          mouseUpData.button,
                          ", repeatCount: ",
                          +mouseUpData.repeatCount,
                          " }");
        } break;
        case InputEventType::MOUSE_SCROLL: {
            POM_LOG_TRACE("Mouse Scroll Event { delta: ", mouseScrollData.delta, " }");
        } break;
        case InputEventType::KEY_DOWN: {
            POM_LOG_TRACE("Key Down Event { hid: ",
                          keyDownData.hid,
                          ", code: ",
                          keyDownData.code,
                          ", repeated: ",
                          keyDownData.repeated,
                          " }");
        } break;
        case InputEventType::KEY_UP: {
            POM_LOG_TRACE("Key Up Event { hid: ",
                          keyUpData.hid,
                          ", code: ",
                          keyUpData.code,
                          ", repeated: ",
                          keyUpData.repeated,
                          " }");
        } break;
        }
    }
} // namespace pom