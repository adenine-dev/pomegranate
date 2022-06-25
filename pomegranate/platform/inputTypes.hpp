#pragma once

#include "base.hpp"

namespace pom {
    enum class MouseButton {
        // NOTE: the values makes  sure it lines up with SDL2
        B1 = 1,
        B2 = 2,
        B3 = 3,
        B4 = 4,
        B5 = 5,

        LEFT = B1,
        MIDDLE = B2,
        RIGHT = B3,
    };
} // namespace pom

template <>
struct fmt::formatter<pom::MouseButton> : formatter<string_view> {
    template <typename FormatContext>
    auto format(pom::MouseButton btn, FormatContext &ctx)
    {
        string_view name = "unknown MouseButton";
        switch (btn) {
        case pom::MouseButton::B1: name = "pom::MouseButton::LEFT"; break;
        case pom::MouseButton::B2: name = "pom::MouseButton::MIDDLE"; break;
        case pom::MouseButton::B3: name = "pom::MouseButton::RIGHT"; break;
        case pom::MouseButton::B4: name = "pom::MouseButton::B4"; break;
        case pom::MouseButton::B5: name = "pom::MouseButton::B5"; break;
        }
        return formatter<string_view>::format(name, ctx);
    }
};
