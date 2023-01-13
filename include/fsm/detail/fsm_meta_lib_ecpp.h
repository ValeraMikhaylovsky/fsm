#pragma once

#include <array>
#include <cstdint>
#include <type_traits>
#include <variant>

namespace ecpp::fsm {

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

template <class T, class... Ts>
struct count_of {
    static constexpr std::size_t value = (static_cast<std::size_t>(std::is_same<T, Ts>::value) + ...);
};

template <class... Ts>
struct no_dublicates {
    static constexpr bool value = ((count_of<Ts, Ts...>::value == 1) && ...);
};

// формирование типа без дубликатов
template <typename T, typename... Ts>
struct unique : std::type_identity<T> {};

template <typename... Ts, typename U, typename... Us>
struct unique<std::variant<Ts...>, U, Us...>
        : std::conditional_t<(std::is_same_v<U, Ts> || ...)
        , unique<std::variant<Ts...>, Us...>
        , unique<std::variant<Ts..., U>, Us...>> {};

template <typename... Ts>
using unique_variant = typename unique<std::variant<>, Ts...>::type;

template <typename T, typename... Ts>
struct unique_pack : std::type_identity<T> {};


template <typename... Ts, typename U, typename... Us>
struct unique_pack<type_pack<Ts...>, U, Us...>
        : std::conditional_t<(std::is_same_v<U, Ts> || ...)
        , unique_pack<type_pack<Ts...>, Us...>
        , unique_pack<type_pack<Ts..., U>, Us...>> {};

template <typename... Ts>
using unique_type_pack = typename unique_pack<type_pack<>, Ts...>::type;


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

