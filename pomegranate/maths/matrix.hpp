#pragma once

#include "base.hpp"

#include "vector.hpp"

namespace pom::maths {
    /// @addtogroup maths
    /// @{

    // FIXME: this is garbage tbh, rewrite the entire thing also make tests probably.

    /// @brief Generic column major matrix type with `N` columns and `M` rows.
    ///
    /// You probably want to use the @ref matrix_specializations "matrix specializations", but this
    /// can be used independently for larger matrices.
    /// The maximum value for `M` and `N` is determined by @ref POM_VECTOR_SIZE_TYPE
    template <typename T, POM_VECTOR_SIZE_TYPE N, POM_VECTOR_SIZE_TYPE M> struct Matrix {
        static_assert(N > 0 && M > 0, "Cannot have a 0 size matrix.");

        /// Column vectors for this matrix.
        Vector<T, M> data[N];

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

        /// Returns an new identity matrix.
        constexpr static Matrix<T, M, N> identity()
        {
            Matrix<T, M, N> ret { 0 };
            for (POM_VECTOR_SIZE_TYPE i = 0; i < M; i++) {
                ret[i][i] = 1;
            }
            return ret;
        }

        /// Returns a new translation matrix, that translates by the members of `v`.
        constexpr static Matrix<T, M, N> translate(const Vector<T, N - 1>& v)
        {
            Matrix<T, M, N> ret = identity();
            for (POM_VECTOR_SIZE_TYPE i = 0; i < M; i++) {
                ret[i][N - 1] = v[i];
            }
            return ret;
        }

        /// Returns a new scale matrix, that scales by the members of `v`
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
        /// Returns a new rotation matrix.
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

        /// Returns a new perspective matrix.
        constexpr static Matrix<T, 4, 4> perspective(f32 fov, f32 aspectRatio, f32 near, f32 far) requires(N == M
                                                                                                           && N == 4)
        {
            Matrix<T, 4, 4> ret;

            f32 q = tan(fov / 2);

            ret[0][0] = 1 / (aspectRatio * q);
            ret[1][1] = -1 / q; // TODO: some way to specify if this should be flipped or not
            ret[2][2] = (near + far) / (near - far);
            ret[2][3] = -1.f;
            ret[3][2] = (2.f * near * far) / (near - far);

            return ret;
        }

        /// Returns a new view matrix looking at `at`.
        constexpr static Matrix<T, 4, 4> lookAt(const vec3& eye, const vec3& at, const vec3& up) requires(N == M
                                                                                                          && N == 4)
        {
            Matrix<T, 4, 4> ret;
            vec3 f = (at - eye).norm();
            vec3 s = f.cross(up).norm();
            vec3 u = s.cross(f);

            ret[0][0] = s[0];
            ret[1][0] = s[1];
            ret[2][0] = s[2];
            ret[0][1] = u[0];
            ret[1][1] = u[1];
            ret[2][1] = u[2];
            ret[0][2] = -f[0];
            ret[1][2] = -f[1];
            ret[2][2] = -f[2];
            ret[3][0] = -s.dot(eye);
            ret[3][1] = -u.dot(eye);
            ret[3][2] = f.dot(eye);

            ret[3][3] = 1.f;

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
    constexpr Matrix<T, P, M> operator*(const Matrix<T, N, M>& lhs, const Matrix<T, P, N>& rhs)
    {
        Matrix<T, P, M> ret;
        for (POM_VECTOR_SIZE_TYPE i = 0; i < P; i++) {
            for (POM_VECTOR_SIZE_TYPE j = 0; j < M; j++) {
                for (POM_VECTOR_SIZE_TYPE k = 0; k < N; k++) {
                    ret[i][j] += lhs[k][j] * rhs[i][k];
                }
            }
        }
        return ret;
    }

    /// @addtogroup types
    /// @{

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