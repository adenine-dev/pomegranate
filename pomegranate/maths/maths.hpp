#pragma once

#include "matrix.hpp"
#include "types.hpp"
#include "vector.hpp"

constexpr f64 TAU = 6.28318530717958647692;
constexpr f64 PI = 3.14159265358979323846;

template <typename T> constexpr T sgn(T v)
{
    return v > 0 ? 1 : v < 0 ? -1 : 0;
}