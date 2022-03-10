#pragma once

// Extremely hacky nonsense because *somehow* c++ doesn't have a good way to do bitflags so .. here we are.
// adapted mostly from various answers here:
// https://stackoverflow.com/questions/1448396/how-to-use-enums-as-flags-in-c/33971769#33971769

// What all of this means is that **ALL** weak enums will be considered bitflags and have these applied to them. I don't
// care about this because I will only be using weak enums for bitflags anyway.

/// @private
template <typename T, bool B = std::is_enum<T>::value> struct is_scoped_enum : std::false_type {
};
/// @private
template <typename T>
struct is_scoped_enum<T, true>
    : std::integral_constant<bool, !std::is_convertible<T, typename std::underlying_type<T>::type>::value> {
};
/// @private
template <typename T>
struct is_bitflagable_enum
    : std::integral_constant<bool,
                             std::is_enum<T>::value
                                 && std::is_convertible<T, typename std::underlying_type<T>::type>::value> {
};

template <typename E> constexpr E operator&(E lhs, E rhs) requires(is_bitflagable_enum<E>::value)
{
    return static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) & static_cast<std::underlying_type_t<E>>(rhs));
}

template <typename E> constexpr E operator|(E lhs, E rhs) requires(is_bitflagable_enum<E>::value)
{
    return static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) | static_cast<std::underlying_type_t<E>>(rhs));
}

template <typename E> constexpr E operator^(E lhs, E rhs) requires(is_bitflagable_enum<E>::value)
{
    return static_cast<E>(static_cast<std::underlying_type_t<E>>(lhs) ^ static_cast<std::underlying_type_t<E>>(rhs));
}

template <typename E> constexpr E& operator|=(E& a, E b) requires(is_bitflagable_enum<E>::value)
{
    a = static_cast<E>(static_cast<std::underlying_type_t<E>>(a) | static_cast<std::underlying_type_t<E>>(b));
    return a;
}
template <typename E> constexpr E& operator&=(E& a, E b) requires(is_bitflagable_enum<E>::value)
{
    a = static_cast<E>(static_cast<std::underlying_type_t<E>>(a) & static_cast<std::underlying_type_t<E>>(b));
    return a;
}
template <typename E> constexpr E& operator^=(E& a, E b) requires(is_bitflagable_enum<E>::value)
{
    a = static_cast<E>(static_cast<std::underlying_type_t<E>>(a) ^ static_cast<std::underlying_type_t<E>>(b));
    return a;
}