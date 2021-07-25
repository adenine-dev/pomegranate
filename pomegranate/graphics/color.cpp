#include "pch.hpp"

#include "color.hpp"

namespace pom {
    const Color Color::TRANSPARENT = { 0.0f, 0.0f, 0.0f, 0.0f };
    const Color Color::BLACK = { 0.0f, 0.0f, 0.0f, 1.0f };
    const Color Color::WHITE = { 1.0f, 1.0f, 1.0f, 1.0f };

    std::ostream& operator<<(std::ostream& os, const Color& color)
    {
        os << "Color { " << color.r << ", " << color.g << ", " << color.b << ", " << color.a << " }";
        return os;
    }
} // namespace pom