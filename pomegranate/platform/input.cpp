#include "pch.hpp"

#include "input.hpp"

namespace pom {
    bool keyDown(const KeyHid& hid)
    {
        const u8* state = SDL_GetKeyboardState(nullptr);
        return state[static_cast<SDL_Scancode>(hid)] != 0;
    }

    bool mouseButtonDown(const MouseButton& button)
    {
        u32 state = SDL_GetMouseState(nullptr, nullptr);
        return (state & SDL_BUTTON(static_cast<u8>(button))) != 0;
    }

    maths::ivec2 getMousePostition()
    {
        maths::ivec2 ret;
        SDL_GetMouseState(&ret.x, &ret.y);
        return ret;
    }

    std::ostream& operator<<(std::ostream& os, const MouseButton& button)
    {
        switch (button) {
        case MouseButton::NONE: {
            os << "MouseButton::NONE";
        } break;
        case MouseButton::BUTTON_1: {
            os << "MouseButton::BUTTON_1";
        } break;
        case MouseButton::BUTTON_2: {
            os << "MouseButton::BUTTON_2";
        } break;
        case MouseButton::BUTTON_3: {
            os << "MouseButton::BUTTON_3";
        } break;
        case MouseButton::BUTTON_4: {
            os << "MouseButton::BUTTON_4";
        } break;
        case MouseButton::BUTTON_5: {
            os << "MouseButton::BUTTON_5";
        } break;
        }

        return os;
    }

    // NOTE: these are very reliant on SDL, if we ever diversify the platform these will need to
    // change...
    std::ostream& operator<<(std::ostream& os, const KeyHid& hid)
    {
        os << SDL_GetScancodeName(static_cast<SDL_Scancode>(hid));

        return os;
    }

    std::ostream& operator<<(std::ostream& os, const Keycode& code)
    {
        os << SDL_GetKeyName(static_cast<SDL_Keycode>(code));

        return os;
    }

} // namespace pom