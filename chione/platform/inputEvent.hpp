#pragma once

#include "maths/maths.hpp"

namespace ce {
    class Window;

    /// @addtogroup platform
    /// @{

    /// @addtogroup input_events Input Events
    /// @brief The event system used by windows to transmit user interaction.
    /// @{

    /// The type of a given InputEvent, each of these is an event that can be emitted by a window.
    enum class InputEventType {
        NONE = 0, ///< None event, this is not used.

        // TODO: differentiate between close and close requested?
        WINDOW_CLOSE, ///< Fires when a window is closed.
        WINDOW_MOVE, ///< Fires when a window is moved, only makes sense on platforms that can do that.
        WINDOW_RESIZE, ///< Fires when a window is resized, only makes sense on platforms that can do that.
        WINDOW_FOCUS, ///< Fires when a window is focused, that is clicked onto.
        WINDOW_BLUR ///< Fires when a window loses focus, that is clicked away from.
    };

    /// @private
    struct NoneEvent {
    };

    /// Data concerning the window close event. An empty type. Not intended to be used alone.
    struct WindowCloseEvent {
    };

    /// Data concerning the window move event. Not intended to be used alone.
    struct WindowMoveEvent {
        /// The new position of the window.
        maths::ivec2 position;
    };

    /// Data concerning the window resize event. Not intended to be used alone.
    struct WindowResizeEvent {
        /// The new size of the window.
        maths::ivec2 size;
    };

    /// Data concerning the window focus event. An empty type. Not intended to be used alone.
    struct WindowFocusEvent {
    };

    /// Data concerning the window blur event. An empty type. Not intended to be used alone.
    struct WindowBlurEvent {
    };

    /// @brief Contains all data for an input event. This includes the type of the event, and a pointer to the source
    /// window, and the event data.
    /// @warning This uses the tagged union based on `type`. Attempting to access the wrong event data is undefined
    /// behavior in c++. It is recommended to use the helper functions which do checking. However, if you do not want
    /// that checking, then the felids are public.
    struct InputEvent {
        /// The type of the input event. This determines the tagged union's correct value.
        const InputEventType type;

        /// The origin Window of this event.
        Window* sourceWindow;

        // FIXME: Doxyfile:942 excludes this union, this is dumb and probably will break at some point.
        union {
            WindowCloseEvent windowCloseData;
            WindowMoveEvent windowMoveData;
            WindowResizeEvent windowResizeData;
            WindowFocusEvent windowFocusData;
            WindowBlurEvent windowBlurData;
        };

        /// Prints a trace message of an event, and its associated data.
        void debugPrint() const;

        /// Returns position data, only works when the event type is `WINDOW_MOVE`. If the type is not valid
        /// this will assert.
        const maths::ivec2& getPosition() const;

        /// Returns position data, only works when the event type is `WINDOW_RESIZE`. If the type is not valid
        /// this will assert.
        const maths::ivec2& getSize() const;
    };

    /// @}

    /// @}

} // namespace ce