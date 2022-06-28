#pragma once

#include "base.hpp"

#define COMMON_VECTOR_MEMBERS(T, N)                                                                                    \
    constexpr const T &operator[](u8 n) const {                                                                        \
        return data[n];                                                                                                \
    }                                                                                                                  \
    constexpr T &operator[](u8 n) {                                                                                    \
        return data[n];                                                                                                \
    }                                                                                                                  \
    constexpr Vector<T, N>() = default;

namespace pom {
    template <typename T, u8 N>
    struct Vector {
        static_assert(N > 1, "vector too small use a scalar :p");

        T data[N];

        COMMON_VECTOR_MEMBERS(T, N);
    };

    template <typename T>
    struct Vector<T, 2> {
        union {
            T data[2];

            struct {
                T x, y;
            };

            struct {
                T u, v;
            };

            struct {
                T width, height;
            };

            struct {
                T r, g;
            };

            struct {
                T s, t;
            };
        };

        COMMON_VECTOR_MEMBERS(T, 2);

        constexpr explicit Vector<T, 2>(T x) : data{x, x} {}

        constexpr Vector<T, 2>(T x, T y) : data{x, y} {}

        template <typename X, typename Y>
        constexpr Vector<T, 2>(X x, Y y) : data{(T)x, (T)y} {}

        template <typename U, u8 M>
        constexpr Vector<T, 2>(const Vector<U, M> &v) requires(M >= 2) : data{(T)v[0], (T)v[1]} {}
    };

    template <typename T>
    struct Vector<T, 3> {
        union {
            T data[3];

            struct {
                T x, y, z;
            };

            struct {
                T width, height, depth;
            };

            struct {
                T r, g, b;
            };

            struct {
                T s, t, p;
            };
        };

        COMMON_VECTOR_MEMBERS(T, 3);

        constexpr explicit Vector<T, 3>(T x) : data{x, x, x} {}

        constexpr Vector<T, 3>(T x, T y, T z) : data{x, y, z} {}

        template <typename X, typename Y, typename Z, typename W>
        constexpr Vector<T, 3>(X x, Y y, Z z) : data{(T)x, (T)y, (T)z} {}

        template <typename U, u8 M>
        constexpr Vector<T, 3>(const Vector<U, M> &v) requires(M >= 3) : data{(T)v[0], (T)v[1], (T)v[2]} {}

        template <typename V, typename Z>
        constexpr Vector<T, 3>(Vector<V, 2> v, Z z) : data{(T)v[0], (T)v[1], (T)z} {}

        template <typename X, typename V>
        constexpr Vector<T, 3>(X x, Vector<V, 2> v) : data{(T)x, (T)v[0], (T)v[1]} {}
    };

    template <typename T>
    struct Vector<T, 4> {
        union {
            T data[4];

            struct {
                T x, y, z, w;
            };

            struct {
                T r, g, b, a;
            };

            struct {
                T s, t, p, q;
            };
        };

        COMMON_VECTOR_MEMBERS(T, 4);

        constexpr explicit Vector<T, 4>(T x) : data{x, x, x, x} {}

        constexpr Vector<T, 4>(T x, T y, T z, T w) : data{x, y, z, w} {}

        template <typename X, typename Y, typename Z, typename W>
        constexpr Vector<T, 4>(X x, Y y, Z z, W w) : data{(T)x, (T)y, (T)z, (T)w} {}

        template <typename U, u8 M>
        constexpr Vector<T, 4>(const Vector<U, M> &v) requires(M >= 4) : data{(T)v[0], (T)v[1], (T)v[2], (T)v[3]} {}

        template <typename V, typename Z, typename W>
        constexpr Vector<T, 4>(Vector<V, 2> v, Z z, W w) : data{(T)v[0], (T)v[1], (T)z, (T)w} {}

        template <typename X, typename V, typename W>
        constexpr Vector<T, 4>(X x, Vector<V, 2> v, W w) : data{(T)x, (T)v[0], (T)v[1], (T)w} {}

        template <typename X, typename Y, typename V>
        constexpr Vector<T, 4>(X x, Y y, Vector<V, 2> v) : data{(T)x, (T)y, (T)v[0], (T)v[1]} {}

        template <typename V, typename W>
        constexpr Vector<T, 4>(Vector<V, 3> v, W w) : data{(T)v[0], (T)v[1], (T)v[2], (T)z} {}

        template <typename X, typename V>
        constexpr Vector<T, 4>(X x, Vector<V, 3> v) : data{(T)x, (T)v[0], (T)v[1], (T)v[2]} {}
    };

    // comparison operaors
    template <typename T, u8 N>
    constexpr bool operator==(const Vector<T, N> &lhs, const Vector<T, N> &rhs) {
        for (u8 i = 0; i < N; i++)
            if (lhs[i] != rhs[i])
                return false;
        return true;
    }

    template <typename T, u8 N>
    constexpr bool operator!=(const Vector<T, N> &lhs, const Vector<T, N> &rhs) {
        return !(rhs == lhs);
    }

    // unary operators
    template <typename T, u8 N>
    constexpr Vector<T, N> &operator++(Vector<T, N> &v) {
        for (u8 i = 0; i < N; i++)
            ++v[i];
        return v;
    }

    template <typename T, u8 N>
    constexpr Vector<T, N> operator++(Vector<T, N> &v, int) {
        Vector<T, N> old = v;
        ++v;
        return old;
    }

    template <typename T, u8 N>
    constexpr Vector<T, N> &operator--(Vector<T, N> &v) {
        for (u8 i = 0; i < N; i++)
            --v[i];
        return v;
    }

    template <typename T, u8 N>
    constexpr Vector<T, N> operator--(Vector<T, N> &v, int) {
        Vector<T, N> old = v;
        --v;
        return old;
    }

    // negation
    template <typename T, u8 N>
    constexpr Vector<T, N> operator-(const Vector<T, N> &v) {
        return -1 * v;
    }

    // vector arithmetic assignment operators
    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> &operator+=(Vector<T, N> &lhs, const Vector<U, N> &rhs) {
        for (u8 i = 0; i < N; i++)
            lhs[i] += (T)rhs[i];

        return lhs;
    }

    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> &operator-=(Vector<T, N> &lhs, const Vector<U, N> &rhs) {
        for (u8 i = 0; i < N; i++)
            lhs[i] -= (T)rhs[i];

        return lhs;
    }

    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> &operator*=(Vector<T, N> &lhs, const Vector<U, N> &rhs) {
        for (u8 i = 0; i < N; i++)
            lhs[i] *= (T)rhs[i];

        return lhs;
    }

    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> &operator/=(Vector<T, N> &lhs, const Vector<U, N> &rhs) {
        for (u8 i = 0; i < N; i++)
            lhs[i] /= (T)rhs[i];

        return lhs;
    }

    // scalar arithmetic assignment operators
    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> &operator+=(Vector<T, N> &lhs, const U rhs) {
        for (u8 i = 0; i < N; i++)
            lhs[i] += (T)rhs;

        return lhs;
    }

    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> &operator-=(Vector<T, N> &lhs, const U rhs) {
        for (u8 i = 0; i < N; i++)
            lhs[i] -= (T)rhs;

        return lhs;
    }

    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> &operator*=(Vector<T, N> &lhs, const U rhs) {
        for (u8 i = 0; i < N; i++)
            lhs[i] *= (T)rhs;

        return lhs;
    }

    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> &operator/=(Vector<T, N> &lhs, const U rhs) {
        for (u8 i = 0; i < N; i++)
            lhs[i] /= (T)rhs;

        return lhs;
    }

    // vector arithmetic operators
    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> operator+(Vector<T, N> lhs, const Vector<U, N> &rhs) {
        for (u8 i = 0; i < N; i++)
            lhs[i] += (T)rhs[i];

        return lhs;
    }

    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> operator-(Vector<T, N> lhs, const Vector<U, N> &rhs) {
        for (u8 i = 0; i < N; i++)
            lhs[i] -= (T)rhs[i];

        return lhs;
    }

    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> operator*(Vector<T, N> lhs, const Vector<U, N> &rhs) {
        for (u8 i = 0; i < N; i++)
            lhs[i] *= (T)rhs[i];

        return lhs;
    }

    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> operator/(Vector<T, N> lhs, const Vector<U, N> &rhs) {
        for (u8 i = 0; i < N; i++)
            lhs[i] /= (T)rhs[i];

        return lhs;
    }

    // scalar arithmetic assignment operators
    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> operator+(Vector<T, N> lhs, const U rhs) {
        for (u8 i = 0; i < N; i++)
            lhs[i] += (T)rhs;

        return lhs;
    }

    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> operator-(Vector<T, N> lhs, const U rhs) {
        for (u8 i = 0; i < N; i++)
            lhs[i] -= (T)rhs;

        return lhs;
    }

    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> operator*(Vector<T, N> lhs, const U rhs) {
        for (u8 i = 0; i < N; i++)
            lhs[i] *= (T)rhs;

        return lhs;
    }

    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> operator/(Vector<T, N> lhs, const U rhs) {
        for (u8 i = 0; i < N; i++)
            lhs[i] /= (T)rhs;

        return lhs;
    }

    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> operator+(const U lhs, Vector<T, N> rhs) {
        for (u8 i = 0; i < N; i++)
            rhs[i] += (T)lhs;

        return rhs;
    }

    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> operator-(const U lhs, Vector<T, N> rhs) {
        for (u8 i = 0; i < N; i++)
            rhs[i] -= (T)lhs;

        return rhs;
    }

    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> operator*(const U lhs, Vector<T, N> rhs) {
        for (u8 i = 0; i < N; i++)
            rhs[i] *= (T)lhs;

        return rhs;
    }

    template <typename T, typename U, u8 N>
    constexpr Vector<T, N> operator/(const U lhs, Vector<T, N> rhs) {
        for (u8 i = 0; i < N; i++)
            rhs[i] /= (T)lhs;

        return rhs;
    }

} // namespace pom

template <typename T, u8 N>
struct fmt::formatter<pom::Vector<T, N>> : formatter<std::string> {
    template <typename FormatContext>
    auto format(const pom::Vector<T, N> &v, FormatContext &ctx) {
        std::string str;
        str += "Vector" + std::to_string(N) + " { ";

        for (u8 i = 0; i < N - 1; i++)
            str += std::to_string(v[i]) + ", ";

        str += std::to_string(v[N - 1]) + " }";

        return formatter<std::string>::format(str, ctx);
    }
};