#pragma once

#include "base.hpp"

namespace pom {
    /// The buttons on a mouse.
    enum class MouseButton {
        // NOTE: the values line up with SDL2.
        B1 = 1, /// First mouse button (equal to LEFT).
        B2 = 2, /// Second mouse button (equal to MIDDLE).
        B3 = 3, /// Third mouse button (equal to RIGHT).
        B4 = 4, /// Fourth mouse button (May not be on all mouses).
        B5 = 5, /// Fifth mouse button (May not be on all mouses).

        LEFT = B1,   /// Left mouse button.
        MIDDLE = B2, /// Middle mouse button.
        RIGHT = B3,  /// Right mouse button.
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
