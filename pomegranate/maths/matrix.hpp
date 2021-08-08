#pragma once

#include "base.hpp"

#include "vector.hpp"

namespace pom::maths {
    /// @addtogroup maths
    /// @{

    // FIXME: this is garbage tbh, rewrite the entire thing also make tests probably.

    /// @brief Generic matrix type with `M` columns and `N` rows.
    ///
    /// You probably want to use the @ref matrix_specializations "matrix specializations", but this
    /// can be used independently for larger matrices.
    /// The maximum value for `M` and `N` is determined by @ref POM_VECTOR_SIZE_TYPE
    template <typename T, POM_VECTOR_SIZE_TYPE M, POM_VECTOR_SIZE_TYPE N> struct Matrix {
        static_assert(N > 0 && M > 0, "Cannot have a 0 size matrix.");

        /// Column vectors for this matrix.
        Vector<T, N> data[M];

        /// Returns a reference to the nth column of the matrix.
        /// @warning No bounds checking is preformed so using this improperly can crash.
        constexpr Vector<T, N>& operator[](const POM_VECTOR_SIZE_TYPE n)
        {
            return data[n];
        }

        /// Returns a const reference to the nth column of the matrix.
        /// @warning No bounds checking is preformed so using this improperly can crash.
        constexpr const Vector<T, N>& operator[](const POM_VECTOR_SIZE_TYPE n) const
        {
            return data[n];
        }

        constexpr static Matrix<T, M, N> identity()
        {
            Matrix<T, M, N> ret { 0 };
            for (POM_VECTOR_SIZE_TYPE i = 0; i < M; i++) {
                ret[i][i] = 1;
            }
            return ret;
        }

        constexpr static Matrix<T, M, N> translate(const Vector<T, N - 1>& v)
        {
            Matrix<T, M, N> ret = identity();
            for (POM_VECTOR_SIZE_TYPE i = 0; i < M; i++) {
                ret[i][N - 1] = v[i];
            }
            return ret;
        }

        constexpr static Matrix<T, M, N> scale(const Vector<T, N - 1>& v)
        {
            Matrix<T, M, N> ret { 0 };
            for (POM_VECTOR_SIZE_TYPE i = 0; i < N - 1; i++) {
                ret[i][i] = v[i];
            }
            ret[N - 1][N - 1] = 1;
            return ret;
        }

        // FIXME: try doing a half way decent 2d/3d one...
        // NOTE: rotation non-linear (able to be translated) matrix
        constexpr static Matrix<T, M, N> rotate(const Vector<T, (((N - 1) * (N - 1 - 1)) / 2)>& v) requires(N == M)
        {
            Matrix<T, M, N> ret = identity();
            POM_VECTOR_SIZE_TYPE x = 0;
            POM_VECTOR_SIZE_TYPE y = x + 1;
            for (POM_VECTOR_SIZE_TYPE i = 0; i < ((N - 1) * (N - 1 - 1)) / 2; i++) {
                T a = cos(v[i]);
                T b = sin(v[i]);
                Matrix<T, M, N> r = identity();
                r[x][x] = a;
                r[x][y] = b * (i % 2 == 0 ? -1 : 1);
                r[y][x] = -r[x][y];
                r[y][y] = a;
                ret *= r;
                y++;
                if (y > N - 1 - 1) {
                    x++;
                    y = x + 1;
                }
            }

            return ret;
        }

        constexpr Matrix<T, M, N>& operator*=(const Matrix<T, M, N>& other)
        {
            *this = operator*(*this, other);
            return *this;
        }
    };

    // NOTE: most of these operations should prob be specialized for speed
    template <typename T, POM_VECTOR_SIZE_TYPE M, POM_VECTOR_SIZE_TYPE N>
    constexpr Vector<T, N> operator*(const Matrix<T, M, N>& lhs, const Vector<T, N>& rhs)
    {
        Vector<T, N> ret { 0 };
        for (POM_VECTOR_SIZE_TYPE i = 0; i < N; i++) {
            for (POM_VECTOR_SIZE_TYPE j = 0; j < N; j++) {
                ret[i] += lhs[i][j] * rhs[j];
            }
        }
        return ret;
    }

    template <typename T, POM_VECTOR_SIZE_TYPE M, POM_VECTOR_SIZE_TYPE N, POM_VECTOR_SIZE_TYPE P>
    constexpr Matrix<T, N, P> operator*(const Matrix<T, M, N>& lhs, const Matrix<T, N, P>& rhs)
    {
        Matrix<T, N, P> ret { 0 };
        for (POM_VECTOR_SIZE_TYPE i = 0; i < M; i++) {
            for (POM_VECTOR_SIZE_TYPE j = 0; j < P; j++) {
                for (POM_VECTOR_SIZE_TYPE k = 0; k < N; k++) {
                    ret[i][j] += lhs[i][k] * rhs[k][j];
                }
            }
        }
        return ret;
    }

    /// @anchoredname{Matrix Specializations, matrix_specializations}
    /// Matrix specialization, used mostly for convenience.
    /// @{

    using mat2 = Matrix<f32, 2, 2>; ///< 2x2 component floating point matrix.
    using mat3 = Matrix<f32, 3, 3>; ///< 3x3 component floating point matrix.
    using mat4 = Matrix<f32, 4, 4>; ///< 4x4 component floating point matrix.

    // TODO: the rest
    /// @}

    /// @}
    /// @}

    template <typename T, POM_VECTOR_SIZE_TYPE M, POM_VECTOR_SIZE_TYPE N>
    std::ostream& operator<<(std::ostream& os, const Matrix<T, M, N>& mat)
    {
        os << "mat" << +M << "x" << +N << " { \n";
        for (POM_VECTOR_SIZE_TYPE i = 0; i < M; i++) {
            os << "\t{ ";
            for (POM_VECTOR_SIZE_TYPE j = 0; j < N - 1; j++) {
                os << mat.data[i][j] << ", ";
            }
            os << mat.data[i][N - 1] << " },\n";
        }
        os << "}";

        return os;
    }

} // namespace pom::maths