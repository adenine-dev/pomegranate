#pragma once

#include "maths/maths.hpp"

namespace ce {
    class Window;

    /// @addtogroup platform
    /// @{

    /// Window Event Type, each of these is an event that can be emitted by a window.
    enum class InputEventType {
        NONE = 0, ///< none event, this is not used.

        // TODO: differentiate between close and close requested?
        WINDOW_CLOSE, ///< Fires when a window is closed.
        WINDOW_MOVE, ///< Fires when a window is moved, only makes sense on platforms that can do that. Only fires once
                     ///< the window is done being moved.
        WINDOW_RESIZE, ///< Fires when a window is resized, only makes sense on platforms that can do that.
        WINDOW_FOCUS, ///< Fires when a window is focused, that is clicked onto.
        WINDOW_BLUR ///< Fires when a window loses focus, that is clicked away from.
    };

    /// Data concerning the window close event. An empty type. Not intended to be used alone.
    struct WindowCloseEvent {
        /// @private
        static constexpr InputEventType TYPE = InputEventType::WINDOW_CLOSE;
    };

    /// Data concerning the window move event. Not intended to be used alone.
    struct WindowMoveEvent {
        /// @private
        static constexpr InputEventType TYPE = InputEventType::WINDOW_MOVE;

        /// The new position of the window.
        maths::ivec2 position;
    };

    /// Data concerning the window resize event. Not intended to be used alone.
    struct WindowResizeEvent {
        /// @private
        static constexpr InputEventType TYPE = InputEventType::WINDOW_RESIZE;

        /// The new size of the window.
        maths::ivec2 size;
    };

    /// Data concerning the window focus event. An empty type. Not intended to be used alone.
    struct WindowFocusEvent {
        /// @private
        static constexpr InputEventType TYPE = InputEventType::WINDOW_FOCUS;
    };

    /// Data concerning the window blur event. An empty type. Not intended to be used alone.
    struct WindowBlurEvent {
        /// @private
        static constexpr InputEventType TYPE = InputEventType::WINDOW_BLUR;
    };

    /// Contains all data for an input event. This includes the type of the event, and a pointer to the source window,
    /// and the event data.
    /// @warning This uses the tagged union based on `type`. Attempting to access the wrong event data is undefined
    /// behavior in c++. It is recommended to use the helper functions which do checking. However, if you do not want
    /// that checking, then the felids are public.
    struct InputEvent {
        const InputEventType type;
        Window* sourceWindow;

        const union {
            WindowCloseEvent windowCloseData;
            WindowMoveEvent windowMoveData;
            WindowResizeEvent windowResizeData;
            WindowFocusEvent windowFocusData;
            WindowBlurEvent windowBlurData;
        };

        void debugPrint() const;

        /// Returns position data, only works when the event type is `WINDOW_MOVE`. If the type is not valid this
        /// will assert.
        const maths::ivec2& getPosition() const;

        /// Returns position data, only works when the event type is `WINDOW_RESIZE`. If the type is not valid this
        /// will assert.
        const maths::ivec2& getSize() const;
    };

    /// @}

} // namespace ce