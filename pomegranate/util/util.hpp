#pragma once

/// Makes flags out of an enum by implementing the bitwise operators for that enum.
#define POM_MAKE_FLAGS(E)                                                                                              \
    constexpr E operator~(E a)                                                                                         \
    {                                                                                                                  \
        return static_cast<E>(~static_cast<std::underlying_type_t<E>>(a));                                             \
    }                                                                                                                  \
    constexpr E operator|(E a, E b)                                                                                    \
    {                                                                                                                  \
        return static_cast<E>(static_cast<std::underlying_type_t<E>>(a) | static_cast<std::underlying_type_t<E>>(b));  \
    }                                                                                                                  \
    constexpr E operator&(E a, E b)                                                                                    \
    {                                                                                                                  \
        return static_cast<E>(static_cast<std::underlying_type_t<E>>(a) & static_cast<std::underlying_type_t<E>>(b));  \
    }                                                                                                                  \
    constexpr E operator^(E a, E b)                                                                                    \
    {                                                                                                                  \
        return static_cast<E>(static_cast<std::underlying_type_t<E>>(a) ^ static_cast<std::underlying_type_t<E>>(b));  \
    }                                                                                                                  \
    constexpr E& operator|=(E& a, E b)                                                                                 \
    {                                                                                                                  \
        a = static_cast<E>(static_cast<std::underlying_type_t<E>>(a) | static_cast<std::underlying_type_t<E>>(b));     \
        return a;                                                                                                      \
    }                                                                                                                  \
    constexpr E& operator&=(E& a, E b)                                                                                 \
    {                                                                                                                  \
        a = static_cast<E>(static_cast<std::underlying_type_t<E>>(a) & static_cast<std::underlying_type_t<E>>(b));     \
        return a;                                                                                                      \
    }                                                                                                                  \
    constexpr E& operator^=(E& a, E b)                                                                                 \
    {                                                                                                                  \
        a = static_cast<E>(static_cast<std::underlying_type_t<E>>(a) ^ static_cast<std::underlying_type_t<E>>(b));     \
        return a;                                                                                                      \
    }
