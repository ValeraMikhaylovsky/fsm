#pragma once

#include <type_traits>
#include <variant>
#include <array>

namespace ecpp::fsm {

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


template <class... T, std::size_t... I>
static std::variant<T...> make_current_variant(std::index_sequence<I...>, std::size_t i) {
    return std::array { +[] { return std::variant<T...>(std::in_place_index<I>); }... }[i]();
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

