#pragma once

#include <type_traits>
#include <variant>
#include <array>

namespace ecpp::fsm::meta {

template <typename T, typename... Ts>
struct unique : std::type_identity<T> {};

template <typename... Ts, typename U, typename... Us>
struct unique<std::variant<Ts...>, U, Us...>
       : std::conditional_t<(std::is_same_v<U, Ts> || ...)
       , unique<std::variant<Ts...>, Us...>
       , unique<std::variant<Ts..., U>, Us...>> {};

template <typename... Ts>
using unique_variant = typename unique<std::variant<>, Ts...>::type;


template <class... Ts>
struct type_pack {};

template<size_t N, typename T, typename... Ts>
struct get_type_by_index {
    using type = typename get_type_by_index<N - 1, Ts...>::type;
};

template<typename T, typename... Ts>
struct get_type_by_index<0, T, Ts...> {
    using type = T;
};

template<size_t N, typename... Ts>
using get_type_from_index = typename get_type_by_index<N, Ts...>::type;

template <class T>
constexpr bool contains(type_pack<>) noexcept {
    return false;
}

template <class T, class... Ts>
constexpr bool contains(type_pack<Ts...>) noexcept {
    return (... || std::is_same<T, Ts>::value);
}

template <typename T, typename... Ts>
constexpr std::size_t find() noexcept {
    constexpr bool is_true[] = {std::is_same_v<T, Ts>...};
    for (std::size_t i {0}; i < sizeof...(Ts); ++i) {
        if (is_true[i])
            return i;
    }
    return sizeof...(Ts);
}

template <class T, class... Ts>
struct count_of {
    static constexpr std::size_t value = (static_cast<std::size_t>(std::is_same<T, Ts>::value) + ...);
};

template <class... Ts>
struct no_dublicates {
    static constexpr bool value = ((count_of<Ts, Ts...>::value == 1) && ...);
};


// формирование списка типов без типа void
template <typename T, typename... Ts>
struct non_void : std::type_identity<T> {};

template <typename... Ts, typename U, typename... Us>
struct non_void<type_pack<Ts...>, U, Us...>
        : std::conditional_t<(std::is_same_v<void, U>)
                , non_void<type_pack<Ts...>, Us...>
                , non_void<type_pack<Ts..., U>, Us...>> {};

template <typename... Ts>
using non_void_type_pack = typename non_void<type_pack<>, Ts...>::type;

}

