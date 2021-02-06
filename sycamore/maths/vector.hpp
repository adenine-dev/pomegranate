#pragma once

#include <iostream>

#define IMPL_COMMON_VECTOR_FUNCS(N)                                                                                    \
    /** Returns a reference to the nth member of a vector. \n                                                          \
        No bounds checking is preformed so using this improperly can crash.                                            \
     */                                                                                                                \
    inline T& operator[](size_t n)                                                                                     \
    {                                                                                                                  \
        return data[n];                                                                                                \
    }                                                                                                                  \
    /** Returns a const reference to the nth member of a vector. \n                                                    \
        No bounds checking is preformed so using this improperly can crash.                                            \
     */                                                                                                                \
    inline const T& operator[](size_t n) const                                                                         \
    {                                                                                                                  \
        return data[n];                                                                                                \
    }

namespace sm { namespace maths {
    /// @addtogroup vectors Vectors
    /// @ingroup maths
    /// Vector specializations (typedefs of Vector), are mostly for convenience, and use glsl notation, ie the first
    /// element can be accessed through `x`, `r`, or `s`. Does not do swizzling.
    /// @{

    /// Generic Vector Type. Specialized by `vec2`, `vec3`, and `vec4`, but can be used independently for larger
    /// vectors.
    template <typename T, size_t N> struct Vector {
        static_assert(N > 0, "Cannot have a 0 size vector.");

        /// Raw data of the vector, recommended to access through operator[].
        T data[N];

        IMPL_COMMON_VECTOR_FUNCS(N)
    };

    /// @private
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

    /// @private
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

    /// @private
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

    typedef Vector<float, 2> vec2;
    typedef Vector<float, 3> vec3;
    typedef Vector<float, 4> vec4;
    /// @}

    template <typename T, size_t N> std::ostream& operator<<(std::ostream& os, const Vector<T, N>& vec)
    {
        os << "vec" << N << " { ";
        for (size_t i = 0; i < N - 1; i++)
            os << vec.data[i] << ", ";
        os << vec.data[N - 1] << " }";

        return os;
    }

}} // namespace sm::maths

#undef IMPL_COMMON_VECTOR_FUNCS
