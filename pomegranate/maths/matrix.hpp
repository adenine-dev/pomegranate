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
            Matrix<T, M, N> ret {};
            for (POM_VECTOR_SIZE_TYPE i = 0; i < M; i++) {
                ret[i][i] = 1;
            }
            return ret;
        }

        /// Returns a new translation matrix, that translates by the members of `v`.
        constexpr static Matrix<T, M, N> translate(const Vector<T, N - 1>& v) requires(N == M)
        {
            Matrix<T, M, N> ret = identity();
            for (POM_VECTOR_SIZE_TYPE i = 0; i < N - 1; i++)
                ret[N - 1][i] = v[i];

            return ret;
        }

        /// Returns a new scale matrix, that scales by the members of `v`
        constexpr static Matrix<T, M, N> scale(const Vector<T, N - 1>& v)
        {
            Matrix<T, M, N> ret {};
            for (POM_VECTOR_SIZE_TYPE i = 0; i < N - 1; i++) {
                ret[i][i] = v[i];
            }
            ret[N - 1][N - 1] = 1;
            return ret;
        }

        /// Returns a new scale matrix, that scales by the members of `v`
        constexpr static Matrix<T, M, N> scale(T v)
        {
            Matrix<T, M, N> ret {};
            for (POM_VECTOR_SIZE_TYPE i = 0; i < N - 1; i++)
                ret[i][i] = v;
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

        constexpr static Matrix<T, M, N> rotate(T angle, const Vector<T, 3>& axis) requires(N == M && N == 4)
        {
            Vector<T, 3> u = axis.norm();
            T a = cos(angle);
            T b = sin(angle);

            // clang-format off
            return {{
                { a + u.x * u.x * (1 - a),         u.x * u.y * (1 - a) - u.z * b,   u.x * u.z * (1 - a) + u.y * b, 0 },
                { u.y * u.x * (1 - a) + u.z * b,   a + u.y * u.y * (1 - a),         u.y * u.z * (1 - a) - u.y * b, 0 },
                { u.z * u.x * (1 - a) - u.y * b,   u.z * u.y * (1 - a) + u.x * b,   a + u.z * u.z * (1 - a),       0 },
                { 0,                               0,                               0,                             1 },
            }};
            // clang-format on
        }

        constexpr static Matrix<T, M, N> rotate(T angle, const Vector<T, 3>& axis) requires(N == M && N == 3)
        {
            Vector<T, 3> u = axis.norm();
            T a = cos(angle);
            T b = sin(angle);

            // clang-format off
            return {{
                { a + u.x * u.x * (1 - a),         u.x * u.y * (1 - a) - u.z * b,   u.x * u.z * (1 - a) + u.y * b },
                { u.y * u.x * (1 - a) + u.z * b,   a + u.y * u.y * (1 - a),         u.y * u.z * (1 - a) - u.y * b },
                { u.z * u.x * (1 - a) - u.y * b,   u.z * u.y * (1 - a) + u.x * b,   a + u.z * u.z * (1 - a)       },
            }};
            // clang-format on
        }

        /// Returns a new perspective projection matrix.
        constexpr static Matrix<T, 4, 4> perspective(f32 fov, f32 aspectRatio, f32 near, f32 far) requires(N == M
                                                                                                           && N == 4)
        {
            Matrix<T, 4, 4> ret = {};

            f32 q = tan(fov / 2);

            ret[0][0] = 1 / (aspectRatio * q);
            ret[1][1] = -1 / q; // TODO: some way to specify if this should be flipped or not
            ret[2][2] = (near + far) / (near - far);
            ret[2][3] = -1.f;
            ret[3][2] = (2.f * near * far) / (near - far);

            return ret;
        }

        /// Returns a new orthographic projection matrix.
        constexpr static Matrix<T, 4, 4>
        ortho(T left, T right, T top, T bottom, T near = -1, T far = 1) requires(N == M && N == 4)
        {
            Matrix<T, 4, 4> ret = identity();

            ret[0][0] = 2 / (right - left);
            ret[1][1] = 2 / (top - bottom);
            ret[2][2] = -2 / (far - near);
            ret[3][0] = -(right + left) / (right - left);
            ret[3][1] = -(top + bottom) / (top - bottom);
            ret[3][2] = -(far + near) / (far - near);

            return ret;
        }

        /// Returns a new view matrix looking at `at`.
        constexpr static Matrix<T, 4, 4> lookAt(const vec3& eye, const vec3& at, const vec3& up) requires(N == M
                                                                                                          && N == 4)
        {
            Matrix<T, 4, 4> ret = {};
            const vec3 f = (at - eye).norm();
            const vec3 s = f.cross(up).norm();
            const vec3 u = s.cross(f);

            ret[0][0] = s[0];
            ret[1][0] = s[1];
            ret[2][0] = s[2];
            ret[0][1] = u[0];
            ret[1][1] = u[1];
            ret[2][1] = u[2];
            ret[0][2] = -f[0];
            ret[1][2] = -f[1];
            ret[2][2] = -f[2];
            ret[3][0] = -dot(s, eye);
            ret[3][1] = -dot(u, eye);
            ret[3][2] = dot(f, eye);

            ret[3][3] = 1.f;

            return ret;
        }

        constexpr Matrix<T, M, N>& operator*=(const Matrix<T, M, N>& other)
        {
            *this = operator*(*this, other);
            return *this;
        }

        constexpr T determinate() const requires(N == M && N > 1)
        {
            static_assert(std::is_floating_point<T>(),
                          "Inverse matrices should only be calculated with float precision.");

            Matrix<T, N, N> m = *this;
            i8 sign = 1;
            for (POM_VECTOR_SIZE_TYPE k = 0; k < N - 1; k++) {
                if (m[k][k] == 0) {
                    POM_VECTOR_SIZE_TYPE i = 0;
                    for (i = k + 1; i < N; i++) {
                        if (m[i][k] != 0) {
                            std::swap(m[i], m[k]);
                            sign *= -1;
                            break;
                        }
                    }

                    if (i == N)
                        return 0;
                }

                for (POM_VECTOR_SIZE_TYPE i = k + 1; i < N; i++)
                    for (POM_VECTOR_SIZE_TYPE j = k + 1; j < N; j++) {
                        m[i][j] = (m[i][j] * m[k][k]) - (m[i][k] * m[k][j]);
                        if (k > 0)
                            m[i][j] /= m[k - 1][k - 1];
                    }
            }
            return m[N - 1][N - 1] * sign;
        }

        constexpr Matrix<T, M, N> inverse() requires(N == M)
        {
            static_assert(std::is_floating_point<T>(),
                          "Inverse matrices should only be calculated with float precision.");
            Matrix<T, N, N * 2> m;

            for (POM_VECTOR_SIZE_TYPE i = 0; i < N; i++) {
                for (POM_VECTOR_SIZE_TYPE j = 0; j < N; j++) {
                    m.data[i][j] = data[i][j];
                }
                m.data[i][i + N] = 1;
            }

            for (POM_VECTOR_SIZE_TYPE i = 0; i < N; i++) {
                if (m.data[i][i] == 0) {
                    POM_VECTOR_SIZE_TYPE j = 0;
                    for (j = i + 1; j < N; j++) {
                        if (m.data[j][i] != 0) {
                            m.data[j] /= m.data[j][i];
                            std::swap(m.data[j], m.data[i]);
                            break;
                        }
                    }

                    if (j == N)
                        POM_ERROR("Attempting to invert noninvertable matrix.");
                }

                for (POM_VECTOR_SIZE_TYPE j = 0; j < N; j++) {
                    if (i != j) {
                        auto x = m.data[j][i] / m.data[i][i];
                        for (POM_VECTOR_SIZE_TYPE k = 0; k < N * 2; k++) {
                            m.data[j][k] -= x * m.data[i][k];
                        }
                    }
                }
            }

            for (POM_VECTOR_SIZE_TYPE i = 0; i < N; i++) {
                m.data[i] /= m.data[i][i];
            }

            Matrix<T, N, N> inv;
            for (POM_VECTOR_SIZE_TYPE i = 0; i < N; i++) {
                for (POM_VECTOR_SIZE_TYPE j = 0; j < N; j++) {
                    inv[i][j] = m.data[i][j + N];
                }
            }

            return inv;
        }

        constexpr Vector<T, N - 1> forward() requires(N == M && N > 1)
        {
            Vector<T, N - 1> ret;
            for (POM_VECTOR_SIZE_TYPE i = 0; i < N - 1; i++)
                ret[i] = -data[i][2];

            return ret;
        }

        constexpr Vector<T, N - 1> up() requires(N == M && N > 1)
        {
            Vector<T, N - 1> ret;
            for (POM_VECTOR_SIZE_TYPE i = 0; i < N - 1; i++)
                ret[i] = data[i][1];

            return ret;
        }

        constexpr Vector<T, N - 1> right() requires(N == M && N > 1)
        {
            Vector<T, N - 1> ret;
            for (POM_VECTOR_SIZE_TYPE i = 0; i < N - 1; i++)
                ret[i] = data[i][0];

            return ret;
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
        Matrix<T, P, M> ret = {};
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