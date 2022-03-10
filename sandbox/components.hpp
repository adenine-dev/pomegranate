#pragma once

#include <pomegranate.hpp>

constexpr usize PARTICLES = 15000;
constexpr usize PARTICLE_VERTICIES = 3;

struct Position {
    pom::maths::vec3 data;

    ECS_COMPONENT();
};

struct Velocity {
    static Velocity random()
    {
        return Velocity {
            pom::maths::vec3(((f32)rand() / RAND_MAX - 0.5f) * 0.3f,
                             ((f32)rand() / RAND_MAX - 0.5f) * 0.3f,
                             ((f32)rand() / RAND_MAX - 0.5f) * 0.3f)
                .norm(),
        };
    }

    pom::maths::vec3 data;

    ECS_COMPONENT();
};

struct Color {
    pom::Color data;

    static Color random()
    {
        return Color { pom::Color { (f32)rand() / RAND_MAX, (f32)rand() / RAND_MAX, (f32)rand() / RAND_MAX, 1.f } };
    }

    ECS_COMPONENT();
};