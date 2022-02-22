#pragma once

#include "base.hpp"

#include <array>
#include <string_view>
#include <utility>

namespace pom {
    namespace detail {
        template <typename T> struct typeNameStorage {
            template <std::size_t... Idxs>
            static constexpr auto arrify(std::string_view sv, std::index_sequence<Idxs...> /*unused*/)
            {
                return std::array { sv[Idxs]..., '\0' };
            }

            static constexpr auto value = ([]() constexpr {
                constexpr auto pretty_function = std::string_view { __PRETTY_FUNCTION__ };
#ifdef __GNUG__
                constexpr auto first = pretty_function.find_first_not_of(' ', pretty_function.find_first_of('<') + 1);
                constexpr auto name = pretty_function.substr(first, pretty_function.find_first_of('>') - first);
#else
        constexpr auto first = pretty_function.find_first_not_of(' ', pretty_function.find_first_of('=') + 1);
        constexpr auto name = pretty_function.substr(first, pretty_function.find_last_of(']') - first);
#endif
                // TODO: msvc or just drop everything but clang.
                return arrify(name, std::make_index_sequence<name.size()> {});
            })();
        };
    } // namespace detail

    /// Returns a compile time stringified name of the underlying type.
    /// `typeName<u32>` → `"unsigned int"`.
    /// `typeName<u32&>` → `"unsigned int &"`.
    template <typename T> constexpr const char* typeName()
    {
        return detail::typeNameStorage<T>::value.data();
    }

    /// Like `std::binary_search` but returns an iterator to the value, returns `last` if the value is not found.
    /// Expects all the same things `std::binary_search` does, additionally T and the iterator value type must be
    /// equal comparable.
    template <class ForwardIt, class T> constexpr auto binaryFind(ForwardIt first, ForwardIt last, const T& value)
    {
        first = std::lower_bound(first, last, value);
        if (first == last || *first != value)
            return last;
        return first;
    }

    /// Like `std::binary_search` but returns an iterator to the value, returns `last` if the value is not found.
    /// Expects all the same things `std::binary_search` does, additionally T and the iterator value type must be
    /// equal comparable.
    template <class ForwardIt, class T, class Compare>
    constexpr auto binaryFind(ForwardIt first, ForwardIt last, const T& value, Compare comp)
    {
        first = std::lower_bound(first, last, value, comp);
        if (first == last || *first != value)
            return last;
        return first;
    }

} // namespace pom
