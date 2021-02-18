#include "debug/logging.hpp"
#include "pch.hpp"

#include "inputEvent.hpp"

namespace ce {
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
        default: {
            os << "Uknown InputEventType";
        }
        }

        return os;
    }

    const maths::ivec2& InputEvent::getPosition() const
    {
        if (type == InputEventType::WINDOW_MOVE) {
            return windowMoveData.position;
        } else {
            CE_ASSERT(false, "Attempting to get position for an event of type: ", type);

            // prevent warning, it is undefined behavior but it will never run so its only mostly dumb :p
            return windowMoveData.position;
        }
    }
    const maths::ivec2& InputEvent::getSize() const
    {
        CE_ASSERT(type == InputEventType::WINDOW_RESIZE, "Attempting to get size for an event of type: ", type);
        return windowResizeData.size;
    }

    void InputEvent::debugPrint() const
    {
        switch (type) {
        case InputEventType::NONE: {
            CE_LOG_WARN("None Event (Something has gone terribly wrong)");
        } break;
        case InputEventType::WINDOW_CLOSE: {
            CE_LOG_TRACE("Window Close Event");
        } break;
        case InputEventType::WINDOW_MOVE: {
            CE_LOG_TRACE("Window Move Event { position: ", windowMoveData.position, " }");
        } break;
        case InputEventType::WINDOW_RESIZE: {
            CE_LOG_TRACE("Window Resize Event { size: ", windowResizeData.size, " }");
        } break;
        case InputEventType::WINDOW_FOCUS: {
            CE_LOG_TRACE("Window Focus Event");
        } break;
        case InputEventType::WINDOW_BLUR: {
            CE_LOG_TRACE("Window Blur Event");
        } break;
        }
    }
} // namespace ce