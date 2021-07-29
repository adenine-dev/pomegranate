#pragma once

#include <iostream>

#include "types.hpp"

#ifndef POM_VECTOR_SIZE_TYPE
/// The size type for vectors is the type used to index into vectors. Default is `u8` giving a
/// maximum vector size of 255, it should be kept as small as possible. If you really need larger
/// vectors, then you can change it.
/// @ingroup maths
#    define POM_VECTOR_SIZE_TYPE u8
#endif

#define IMPL_COMMON_VECTOR_FUNCS(T, N)                                                                                 \
    /** Returns a reference to the nth member of a vector. \n                                                          \
        @warning No bounds checking is preformed so using this improperly can crash.                                   \
     */                                                                                                                \
    constexpr T& operator[](const POM_VECTOR_SIZE_TYPE n)                                                              \
    {                                                                                                                  \
        return data[n];                                                                                                \
    }                                                                                                                  \
    /** Returns a const reference to the nth member of a vector. \n                                                    \
       @warning No bounds checking is preformed so using this improperly can crash.                                    \
     */                                                                                                                \
    constexpr const T& operator[](const POM_VECTOR_SIZE_TYPE n) const                                                  \
    {                                                                                                                  \
        return data[n];                                                                                                \
    }                                                                                                                  \
    constexpr Vector<T, N>& add(T other)                                                                               \
    {                                                                                                                  \
        for (POM_VECTOR_SIZE_TYPE i = 0; i < N; i++)                                                                   \
            data[i] += other;                                                                                          \
        return *this;                                                                                                  \
    }                                                                                                                  \
    constexpr Vector<T, N>& sub(T other)                                                                               \
    {                                                                                                                  \
        for (POM_VECTOR_SIZE_TYPE i = 0; i < N; i++)                                                                   \
            data[i] -= other;                                                                                          \
        return *this;                                                                                                  \
    }                                                                                                                  \
    constexpr Vector<T, N>& mul(T other)                                                                               \
    {                                                                                                                  \
        for (POM_VECTOR_SIZE_TYPE i = 0; i < N; i++)                                                                   \
            data[i] *= other;                                                                                          \
        return *this;                                                                                                  \
    }                                                                                                                  \
    constexpr Vector<T, N>& div(T other)                                                                               \
    {                                                                                                                  \
        for (POM_VECTOR_SIZE_TYPE i = 0; i < N; i++)                                                                   \
            data[i] /= other;                                                                                          \
        return *this;                                                                                                  \
    }                                                                                                                  \
    constexpr Vector<T, N>& add(const Vector<T, N>& other)                                                             \
    {                                                                                                                  \
        for (POM_VECTOR_SIZE_TYPE i = 0; i < N; i++)                                                                   \
            data[i] += other[i];                                                                                       \
        return *this;                                                                                                  \
    }                                                                                                                  \
    constexpr Vector<T, N>& sub(const Vector<T, N>& other)                                                             \
    {                                                                                                                  \
        for (POM_VECTOR_SIZE_TYPE i = 0; i < N; i++)                                                                   \
            data[i] -= other[i];                                                                                       \
        return *this;                                                                                                  \
    }                                                                                                                  \
    constexpr Vector<T, N>& mul(const Vector<T, N>& other)                                                             \
    {                                                                                                                  \
        for (POM_VECTOR_SIZE_TYPE i = 0; i < N; i++)                                                                   \
            data[i] *= other[i];                                                                                       \
        return *this;                                                                                                  \
    }                                                                                                                  \
    constexpr Vector<T, N>& div(const Vector<T, N>& other)                                                             \
    {                                                                                                                  \
        for (POM_VECTOR_SIZE_TYPE i = 0; i < N; i++)                                                                   \
            data[i] /= other[i];                                                                                       \
        return *this;                                                                                                  \
    }                                                                                                                  \
    constexpr Vector<T, N>& operator+=(const Vector<T, N>& other)                                                      \
    {                                                                                                                  \
        return add(other);                                                                                             \
    }                                                                                                                  \
    constexpr Vector<T, N>& operator-=(const Vector<T, N>& other)                                                      \
    {                                                                                                                  \
        return sub(other);                                                                                             \
    }                                                                                                                  \
    constexpr Vector<T, N>& operator*=(const Vector<T, N>& other)                                                      \
    {                                                                                                                  \
        return mul(other);                                                                                             \
    }                                                                                                                  \
    constexpr Vector<T, N>& operator/=(const Vector<T, N>& other)                                                      \
    {                                                                                                                  \
        return div(other);                                                                                             \
    }                                                                                                                  \
    constexpr Vector<T, N>& operator+=(T other)                                                                        \
    {                                                                                                                  \
        return add(other);                                                                                             \
    }                                                                                                                  \
    constexpr Vector<T, N>& operator-=(T other)                                                                        \
    {                                                                                                                  \
        return sub(other);                                                                                             \
    }                                                                                                                  \
    constexpr Vector<T, N>& operator*=(T other)                                                                        \
    {                                                                                                                  \
        return mul(other);                                                                                             \
    }                                                                                                                  \
    constexpr Vector<T, N>& operator/=(T other)                                                                        \
    {                                                                                                                  \
        return div(other);                                                                                             \
    }                                                                                                                  \
    constexpr bool operator==(const Vector<T, N>& other) const                                                         \
    {                                                                                                                  \
        bool ret = true;                                                                                               \
        for (POM_VECTOR_SIZE_TYPE i = 0; i < N; i++)                                                                   \
            ret = ret && (data[i] == other[i]);                                                                        \
        return ret;                                                                                                    \
    }                                                                                                                  \
    constexpr bool operator!=(const Vector<T, N>& other) const                                                         \
    {                                                                                                                  \
        return !(operator==(other));                                                                                   \
    }

namespace pom::maths {

    /// @addtogroup maths
    /// @{

    /// @brief Generic vector type.
    ///
    /// You probably want to use the @ref vector_specializations "Vector Specializations", but this
    /// can be used independently for larger vectors.
    /// The maximum value for `N` is determined by @ref POM_VECTOR_SIZE_TYPE
    template <typename T, POM_VECTOR_SIZE_TYPE N> struct Vector {
        static_assert(N > 0, "Cannot have a 0 size vector.");

        /// Raw data of the vector, recommended to access through `operator[]()`.
        T data[N];

        IMPL_COMMON_VECTOR_FUNCS(T, N)
    };

    /// @private
    template <typename T> struct Vector<T, 2> {
        /// @private
        union {
            T data[2];
            /// @private
            struct {
                /// @private
                union {
                    T x, r, s;
                };
                /// @private
                union {
                    T y, g, t;
                };
            };
        };

        IMPL_COMMON_VECTOR_FUNCS(T, 2)
    };

    /// @private
    template <typename T> struct Vector<T, 3> {
        /// @private
        union {
            T data[3];
            /// @private
            struct {
                /// @private
                union {
                    T x, r, s;
                };
                /// @private
                union {
                    T y, g, t;
                };
                /// @private
                union {
                    T z, b, p;
                };
            };
        };

        IMPL_COMMON_VECTOR_FUNCS(T, 3)
    };

    /// @private
    template <typename T> struct Vector<T, 4> {
        /// @private
        union {
            T data[4];
            /// @private
            struct {
                /// @private
                union {
                    T x, r, s;
                };
                /// @private
                union {
                    T y, g, t;
                };
                /// @private
                union {
                    T z, b, p;
                };
                /// @private
                union {
                    T w, a, q;
                };
            };
        };

        IMPL_COMMON_VECTOR_FUNCS(T, 4)
    };

    template <typename T, POM_VECTOR_SIZE_TYPE N>
    constexpr Vector<T, N>& operator+(Vector<T, N> lhs, const Vector<T, N>& rhs)
    {
        return lhs.add(rhs);
    }

    template <typename T, POM_VECTOR_SIZE_TYPE N>
    constexpr Vector<T, N>& operator-=(Vector<T, N> lhs, const Vector<T, N>& rhs)
    {
        return lhs.sub(rhs);
    }

    template <typename T, POM_VECTOR_SIZE_TYPE N>
    constexpr Vector<T, N>& operator*=(Vector<T, N> lhs, const Vector<T, N>& rhs)
    {
        return lhs.mul(rhs);
    }

    template <typename T, POM_VECTOR_SIZE_TYPE N>
    constexpr Vector<T, N>& operator/=(Vector<T, N> lhs, const Vector<T, N>& rhs)
    {
        return lhs.div(rhs);
    }

    template <typename T, POM_VECTOR_SIZE_TYPE N> constexpr Vector<T, N>& operator+(Vector<T, N> lhs, T rhs)
    {
        return lhs.add(rhs);
    }

    template <typename T, POM_VECTOR_SIZE_TYPE N> constexpr Vector<T, N>& operator-=(Vector<T, N> lhs, T rhs)
    {
        return lhs.sub(rhs);
    }

    template <typename T, POM_VECTOR_SIZE_TYPE N> constexpr Vector<T, N>& operator*=(Vector<T, N> lhs, T rhs)
    {
        return lhs.mul(rhs);
    }

    template <typename T, POM_VECTOR_SIZE_TYPE N> constexpr Vector<T, N>& operator/=(Vector<T, N> lhs, T rhs)
    {
        return lhs.div(rhs);
    }

    /// @addtogroup types
    /// @{

    /// @anchoredname{Vector specializations, vector_specializations}
    /// Vector specialization, used mostly for convenience. Has `xyzw`, `rgba`, and `stpq` members
    /// Equivelent to the same name in glsl. These do not do swizzling.
    /// @{

    using vec2 = Vector<f32, 2>; ///< 2 component floating point vector.
    using vec3 = Vector<f32, 3>; ///< 3 component floating point vector.
    using vec4 = Vector<f32, 4>; ///< 4 component floating point vector.

    using dvec2 = Vector<f64, 2>; ///< 2 component double precision vector.
    using dvec3 = Vector<f64, 3>; ///< 3 component double precision vector.
    using dvec4 = Vector<f64, 4>; ///< 4 component double precision vector.

    using uvec2 = Vector<u32, 2>; ///< 2 component unsigned int vector.
    using uvec3 = Vector<u32, 3>; ///< 3 component unsigned int vector.
    using uvec4 = Vector<u32, 4>; ///< 4 component unsigned int vector.

    using ivec2 = Vector<i32, 2>; ///< 2 component int vector.
    using ivec3 = Vector<i32, 3>; ///< 3 component int vector.
    using ivec4 = Vector<i32, 4>; ///< 4 component int vector.

    using bvec2 = Vector<bool, 2>; ///< 2 component boolean vector.
    using bvec3 = Vector<bool, 3>; ///< 3 component boolean vector.
    using bvec4 = Vector<bool, 4>; ///< 4 component boolean vector.

    /// @}
    /// @}

    /// @}

    template <typename T, POM_VECTOR_SIZE_TYPE N> std::ostream& operator<<(std::ostream& os, const Vector<T, N>& vec)
    {
        os << "vec" << +N << " { ";
        for (POM_VECTOR_SIZE_TYPE i = 0; i < N - 1; i++) {
            os << vec.data[i] << ", ";
        }
        os << vec.data[N - 1] << " }";

        return os;
    }

} // namespace pom::maths

#undef IMPL_COMMON_VECTOR_FUNCS
