#pragma once

#include "base.hpp"

#include "maths/vector.hpp"
#include <cmath>

namespace pom {
    /// @addtogroup graphics
    /// @{

    /// Standard shader parsable color. `r` `g` `b` and `a` are the channels, where 0.0f is the minimum value and 1.0f
    /// is the maximum value for a given channel.
    struct Color {
        /// @private
        union {
            f32 data[4];
            maths::vec4 vec = maths::vec4(0.f);
            struct {
                f32 r;
                f32 g;
                f32 b;
                f32 a;
            };
        };

        static const Color TRANSPARENT;
        static const Color BLACK;
        static const Color WHITE;

        /// @brief Creates a color in standard rgb color format, `r`, `g`, and `b` are in the range [0, 255]. `a` will
        /// be `1.0f`
        static constexpr Color rgb(u8 r, u8 g, u8 b)
        {
            return { r / 255.f, g / 255.f, b / 255.f, 1.f };
        }

        /// @brief Creates a color in standard rgba color format, `r`, `g`, and `b` are in the range [0, 255], `a` is in
        /// the range [0, 1].
        static constexpr Color rgba(u8 r, u8 g, u8 b, f32 a)
        {
            return { r / 255.f, g / 255.f, b / 255.f, a };
        }
    };
    /// @}

    std::ostream& operator<<(std::ostream& os, const Color& color);
} // namespace pom