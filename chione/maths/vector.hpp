#pragma once

#include <iostream>

#define IMPL_COMMON_VECTOR_FUNCS(N)                                                                                    \
    /** Returns a reference to the nth member of a vector. \n                                                          \
        @warning No bounds checking is preformed so using this improperly can crash.                                   \
     */                                                                                                                \
    inline T& operator[](size_t n)                                                                                     \
    {                                                                                                                  \
        return data[n];                                                                                                \
    }                                                                                                                  \
    /** Returns a const reference to the nth member of a vector. \n                                                    \
       @warning No bounds checking is preformed so using this improperly can crash.                                    \
     */                                                                                                                \
    inline const T& operator[](size_t n) const                                                                         \
    {                                                                                                                  \
        return data[n];                                                                                                \
    }

namespace ce { namespace maths {
    /// @addtogroup maths
    /// @{

    /// Generic Vector Type. You probably want to use the @ref vector_specializations "Vector Specializations", but this
    /// can be used independently for larger vectors.
    template <typename T, size_t N> struct Vector {
        static_assert(N > 0, "Cannot have a 0 size vector.");

        /// Raw data of the vector, recommended to access through `operator[]()`.
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

    /// @anchoredname{Vector specializations, vector_specializations}
    /// Vector specialization, used mostly for convenience. Has `xyzw`, `rgba`, and `stpq` members
    /// Equivelent to the same name in glsl. These do not do swizzeling.
    /// @{

    using vec2 = Vector<float, 2>;
    using vec3 = Vector<float, 3>;
    using vec4 = Vector<float, 4>;

    using dvec2 = Vector<double, 2>;
    using dvec3 = Vector<double, 3>;
    using dvec4 = Vector<double, 4>;

    using uvec2 = Vector<uint32_t, 2>;
    using uvec3 = Vector<uint32_t, 3>;
    using uvec4 = Vector<uint32_t, 4>;

    using ivec2 = Vector<int32_t, 2>;
    using ivec3 = Vector<int32_t, 3>;
    using ivec4 = Vector<int32_t, 4>;

    using bvec2 = Vector<bool, 2>;
    using bvec3 = Vector<bool, 3>;
    using bvec4 = Vector<bool, 4>;
    /// @}
    /// @}

    template <typename T, size_t N> std::ostream& operator<<(std::ostream& os, const Vector<T, N>& vec)
    {
        os << "vec" << N << " { ";
        for (size_t i = 0; i < N - 1; i++)
            os << vec.data[i] << ", ";
        os << vec.data[N - 1] << " }";

        return os;
    }

}} // namespace ce::maths

#undef IMPL_COMMON_VECTOR_FUNCS
