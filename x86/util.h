#pragma once

#include <type_traits>

namespace KK::x86::Util {

template <typename T, typename... Ts>
struct is_any_of :
    std::disjunction<
        std::is_same<T, Ts>...
    > {};

template <typename T, typename... Ts>
constexpr bool is_any_of_v
    = is_any_of<T, Ts...>::value;

}