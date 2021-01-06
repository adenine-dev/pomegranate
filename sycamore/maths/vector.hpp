#pragma once

#include <cstdint>
#include <iostream>

#define IMPL_COMMON_VECTOR_FUNCS(N)                                                                                    \
    inline T& operator[](size_t i)                                                                                     \
    {                                                                                                                  \
        return data[i];                                                                                                \
    }                                                                                                                  \
    inline const T& operator[](size_t i) const                                                                         \
    {                                                                                                                  \
        return data[i];                                                                                                \
    }

namespace sm { namespace maths {
    template <typename T, size_t N> struct Vector {
        static_assert(N > 0, "Cannot have a 0 size vector.");

        T data[N];
        IMPL_COMMON_VECTOR_FUNCS(N)
    };

    template <typename T> struct Vector<T, 2> {
        union {
            T data[2];
            struct {
                union {
                    T x, r, s;
                };
                union {
                    T y, g, t;
                };
            };
        };

        IMPL_COMMON_VECTOR_FUNCS(2)
    };

    template <typename T> struct Vector<T, 3> {
        union {
            T data[3];
            struct {
                union {
                    T x, r, s;
                };
                union {
                    T y, g, t;
                };
                union {
                    T z, b, p;
                };
            };
        };

        IMPL_COMMON_VECTOR_FUNCS(3)
    };

    template <typename T> struct Vector<T, 4> {
        union {
            T data[4];
            struct {
                union {
                    T x, r, s;
                };
                union {
                    T y, g, t;
                };
                union {
                    T z, b, p;
                };
                union {
                    T w, a, q;
                };
            };
        };

        IMPL_COMMON_VECTOR_FUNCS(4)
    };

    template <typename T, size_t N> std::ostream& operator<<(std::ostream& os, const Vector<T, N>& vec)
    {
        os << "vec" << +N << " { ";
        for (size_t i = 0; i < N - 1; i++)
            os << vec.data[i] << ", ";
        os << vec.data[N - 1] << " }";

        return os;
    }

    using vec2 = Vector<float, 2>;
    using vec3 = Vector<float, 3>;
    using vec4 = Vector<float, 4>;

}} // namespace sm::maths

#undef IMPL_COMMON_VECTOR_FUNCS
