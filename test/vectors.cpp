#include "doctest.h"

#include "pomegranate.hpp"

using namespace pom;

TEST_CASE_TEMPLATE("Vectors are sized correctly", T, f32, f64, u8, u16, u32, u64, i8, i16, i32, i64) {
    CHECK(sizeof(Vector<T, 2>) == sizeof(T) * 2);
    CHECK(sizeof(Vector<T, 3>) == sizeof(T) * 3);
    CHECK(sizeof(Vector<T, 4>) == sizeof(T) * 4);
    CHECK(sizeof(Vector<T, 5>) == sizeof(T) * 5);
    CHECK(sizeof(Vector<T, 6>) == sizeof(T) * 6);
    CHECK(sizeof(Vector<T, 7>) == sizeof(T) * 7);
}

TEST_CASE_TEMPLATE("Vector access is correct", T, f32, f64, u8, u16, u32, u64, i8, i16, i32, i64) {
    SUBCASE("Vector 2") {
        const Vector<T, 2> v(2, 4);

        CHECK(v[0] == 2);
        CHECK(v[0] == v.x);
        CHECK(v[0] == v.r);
        CHECK(v[0] == v.s);
        CHECK(v[0] == v.u);
        CHECK(v[0] == v.width);

        CHECK(v[1] == 4);
        CHECK(v[1] == v.y);
        CHECK(v[1] == v.g);
        CHECK(v[1] == v.t);
        CHECK(v[1] == v.v);
        CHECK(v[1] == v.height);
    }

    SUBCASE("Vector 3") {
        const Vector<T, 3> v(2, 4, 6);

        CHECK(v[0] == 2);
        CHECK(v[0] == v.x);
        CHECK(v[0] == v.r);
        CHECK(v[0] == v.s);
        CHECK(v[0] == v.width);

        CHECK(v[1] == 4);
        CHECK(v[1] == v.y);
        CHECK(v[1] == v.g);
        CHECK(v[1] == v.t);
        CHECK(v[1] == v.height);

        CHECK(v[2] == 6);
        CHECK(v[2] == v.z);
        CHECK(v[2] == v.b);
        CHECK(v[2] == v.p);
        CHECK(v[2] == v.depth);
    }

    SUBCASE("Vector 4") {
        const Vector<T, 4> v(2, 4, 6, 8);

        CHECK(v[0] == 2);
        CHECK(v[0] == v.x);
        CHECK(v[0] == v.r);
        CHECK(v[0] == v.s);

        CHECK(v[1] == 4);
        CHECK(v[1] == v.y);
        CHECK(v[1] == v.g);
        CHECK(v[1] == v.t);

        CHECK(v[2] == 6);
        CHECK(v[2] == v.z);
        CHECK(v[2] == v.b);
        CHECK(v[2] == v.p);

        CHECK(v[3] == 8);
        CHECK(v[3] == v.w);
        CHECK(v[3] == v.a);
        CHECK(v[3] == v.q);
    }
}

TEST_CASE_TEMPLATE("Vectors can be compared", T, f32, f64, u8, u16, u32, u64, i8, i16, i32, i64) {
    SUBCASE("Vector 2") {
        const Vector<T, 2> v1(1, 2);
        const Vector<T, 2> v2(1, 2);

        CHECK(v1 == v2);
        CHECK(!(v1 != v2));
    }

    SUBCASE("Vector 3") {
        const Vector<T, 3> v1(1, 2, 3);
        const Vector<T, 3> v2(1, 2, 3);

        CHECK(v1 == v2);
        CHECK(!(v1 != v2));
    }

    SUBCASE("Vector 4") {
        const Vector<T, 4> v1(1, 2, 3, 4);
        const Vector<T, 4> v2(1, 2, 3, 4);

        CHECK(v1 == v2);
        CHECK(!(v1 != v2));
    }
}

TEST_CASE_TEMPLATE("Vectors have unary operators", T, f32, f64, u8, u16, u32, u64, i8, i16, i32, i64) {
    SUBCASE("Vector 2") {
        Vector<T, 2> v(1, 2);

        CHECK(++v == Vector<T, 2>(2, 3));
        CHECK(v++ == Vector<T, 2>(2, 3));
        CHECK(v == Vector<T, 2>(3, 4));

        CHECK(--v == Vector<T, 2>(2, 3));
        CHECK(v-- == Vector<T, 2>(2, 3));
        CHECK(v == Vector<T, 2>(1, 2));
    }

    SUBCASE("Vector 3") {
        Vector<T, 3> v(1, 2, 3);

        CHECK(++v == Vector<T, 3>(2, 3, 4));
        CHECK(v++ == Vector<T, 3>(2, 3, 4));
        CHECK(v == Vector<T, 3>(3, 4, 5));

        CHECK(--v == Vector<T, 3>(2, 3, 4));
        CHECK(v-- == Vector<T, 3>(2, 3, 4));
        CHECK(v == Vector<T, 3>(1, 2, 3));
    }

    SUBCASE("Vector 4") {
        Vector<T, 4> v(1, 2, 3, 4);

        CHECK(++v == Vector<T, 4>(2, 3, 4, 5));
        CHECK(v++ == Vector<T, 4>(2, 3, 4, 5));
        CHECK(v == Vector<T, 4>(3, 4, 5, 6));

        CHECK(--v == Vector<T, 4>(2, 3, 4, 5));
        CHECK(v-- == Vector<T, 4>(2, 3, 4, 5));
        CHECK(v == Vector<T, 4>(1, 2, 3, 4));
    }
}

TEST_CASE_TEMPLATE("Vectors can be negated", T, f32, f64, i8, i16, i32, i64, u8, u16, u32, u64) {
    SUBCASE("Vector 2") {
        const Vector<T, 2> v(1, 2);
        CHECK(-v == Vector<T, 2>(-1, -2));
    }

    SUBCASE("Vector 3") {
        const Vector<T, 3> v(1, 2, 3);
        CHECK(-v == Vector<T, 3>(-1, -2, -3));
    }

    SUBCASE("Vector 4") {
        const Vector<T, 4> v(1, 2, 3, 4);
        CHECK(-v == Vector<T, 4>(-1, -2, -3, -4));
    }
}

TEST_CASE_TEMPLATE("Vectors have arithmetic operators", T, f32, f64, i8, i16, i32, i64, u8, u16, u32, u64) {
    SUBCASE("Vector 2") {
        const Vector<T, 2> a(2, 4);
        const Vector<T, 2> b(2, 2);

        CHECK(a + b == Vector<T, 2>(4, 6));
        CHECK(a - b == Vector<T, 2>(0, 2));
        CHECK(a * b == Vector<T, 2>(4, 8));
        CHECK(a / b == Vector<T, 2>(1, 2));

        const T s = 2;
        CHECK(a + s == Vector<T, 2>(4, 6));
        CHECK(a - s == Vector<T, 2>(0, 2));
        CHECK(a * s == Vector<T, 2>(4, 8));
        CHECK(a / s == Vector<T, 2>(1, 2));
    }

    SUBCASE("Vector 3") {
        const Vector<T, 3> a(2, 4, 6);
        const Vector<T, 3> b(2, 2, 3);

        CHECK(a + b == Vector<T, 3>(4, 6, 9));
        CHECK(a - b == Vector<T, 3>(0, 2, 3));
        CHECK(a * b == Vector<T, 3>(4, 8, 18));
        CHECK(a / b == Vector<T, 3>(1, 2, 2));

        const T s = 2;
        CHECK(a + s == Vector<T, 3>(4, 6, 8));
        CHECK(a - s == Vector<T, 3>(0, 2, 4));
        CHECK(a * s == Vector<T, 3>(4, 8, 12));
        CHECK(a / s == Vector<T, 3>(1, 2, 3));
    }

    SUBCASE("Vector 4") {
        const Vector<T, 4> a(2, 4, 6, 8);
        const Vector<T, 4> b(2, 2, 3, 2);

        CHECK(a + b == Vector<T, 4>(4, 6, 9, 10));
        CHECK(a - b == Vector<T, 4>(0, 2, 3, 6));
        CHECK(a * b == Vector<T, 4>(4, 8, 18, 16));
        CHECK(a / b == Vector<T, 4>(1, 2, 2, 4));

        const T s = 2;
        CHECK(a + s == Vector<T, 4>(4, 6, 8, 10));
        CHECK(a - s == Vector<T, 4>(0, 2, 4, 6));
        CHECK(a * s == Vector<T, 4>(4, 8, 12, 16));
        CHECK(a / s == Vector<T, 4>(1, 2, 3, 4));
    }
}

TEST_CASE_TEMPLATE("Vectors have arithmetic assignment operators", T, f32, f64, i8, i16, i32, i64, u8, u16, u32, u64) {
    SUBCASE("Vector 2") {
        Vector<T, 2> a(2, 4);
        const Vector<T, 2> b(2, 2);

        a += b;
        CHECK(a == Vector<T, 2>(4, 6));
        a -= b;
        CHECK(a == Vector<T, 2>(2, 4));
        a *= b;
        CHECK(a == Vector<T, 2>(4, 8));
        a /= b;
        CHECK(a == Vector<T, 2>(2, 4));

        const T s = 2;
        a += s;
        CHECK(a == Vector<T, 2>(4, 6));
        a -= s;
        CHECK(a == Vector<T, 2>(2, 4));
        a *= s;
        CHECK(a == Vector<T, 2>(4, 8));
        a /= s;
        CHECK(a == Vector<T, 2>(2, 4));
    }

    SUBCASE("Vector 3") {
        Vector<T, 3> a(2, 4, 6);
        const Vector<T, 3> b(2, 2, 3);

        a += b;
        CHECK(a == Vector<T, 3>(4, 6, 9));
        a -= b;
        CHECK(a == Vector<T, 3>(2, 4, 6));
        a *= b;
        CHECK(a == Vector<T, 3>(4, 8, 18));
        a /= b;
        CHECK(a == Vector<T, 3>(2, 4, 6));

        const T s = 2;
        a += s;
        CHECK(a == Vector<T, 3>(4, 6, 8));
        a -= s;
        CHECK(a == Vector<T, 3>(2, 4, 6));
        a *= s;
        CHECK(a == Vector<T, 3>(4, 8, 12));
        a /= s;
        CHECK(a == Vector<T, 3>(2, 4, 6));
    }

    SUBCASE("Vector 4") {
        Vector<T, 4> a(2, 4, 6, 8);
        const Vector<T, 4> b(2, 2, 3, 2);

        a += b;
        CHECK(a == Vector<T, 4>(4, 6, 9, 10));
        a -= b;
        CHECK(a == Vector<T, 4>(2, 4, 6, 8));
        a *= b;
        CHECK(a == Vector<T, 4>(4, 8, 18, 16));
        a /= b;
        CHECK(a == Vector<T, 4>(2, 4, 6, 8));

        const T s = 2;
        a += s;
        CHECK(a == Vector<T, 4>(4, 6, 8, 10));
        a -= s;
        CHECK(a == Vector<T, 4>(2, 4, 6, 8));
        a *= s;
        CHECK(a == Vector<T, 4>(4, 8, 12, 16));
        a /= s;
        CHECK(a == Vector<T, 4>(2, 4, 6, 8));
    }
}