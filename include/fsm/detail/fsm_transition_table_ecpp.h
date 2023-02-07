#pragma once

#include <type_traits>
#include <variant>

#include "fsm_state_ecpp.h"
#include "fsm_transition_ecpp.h"
#include "fsm_meta_lib_ecpp.h"

namespace ecpp::fsm {

struct transition_table_base {};


template<IsTransition ...T>
struct transition_table : transition_table_base
{
    static_assert(no_dublicates<T...>::value, "transition table contains duplicates");

    struct empty_state : state<empty_state> {};

    static constexpr std::size_t count = sizeof...(T);
    using transition_pack = type_pack<T...>;
    using events_pack = type_pack<typename T::event_t...>;
    using states_pack = unique_type_pack<typename T::source_t..., typename T::target_t...>;
    using internal_transitions = non_void_type_pack<typename T::source_tr_t..., typename T::target_tr_t...>;
    using states_variant = unique_variant<typename T::source_t..., typename T::target_t..., empty_state>;

    static inline constexpr std::variant<T...> make_transition(std::size_t index) {
        return make_impl(std::index_sequence_for<T...>(), index);
    }

    static inline constexpr std::size_t get_index(const auto &s, const auto &e) {
        using state_t = std::decay_t<decltype(s)>;
        using event_t = std::decay_t<decltype(e)>;
        constexpr bool bs[] = {(std::is_same_v<state_t, typename T::source_t> && std::is_same_v<event_t, typename T::event_t>)...};
        for (std::size_t index {0}; index < sizeof...(T); ++index) {
            if (bs[index])
                return index;
        }
        return sizeof...(T);
    }

    static inline constexpr std::size_t get_index(const auto &e) {
        using event_t = std::decay_t<decltype(e)>;
        constexpr bool bs[] = {(std::is_same_v<event_t, typename T::event_t>)...};
        for (std::size_t index {0}; index < sizeof...(T); ++index) {
            if (bs[index])
                return index;
        }
        return sizeof...(T);
    }

    template <std::size_t... I>
    static std::variant<T...> make_impl(std::index_sequence<I...>, std::size_t i) {
        return std::array { +[] { return std::variant<T...>(std::in_place_index<I>); }... }[i]();
    }
};

template <class T>
constexpr bool contains_in_table(type_pack<>&&) {
    return false;
}

template <class T, class... Ts>
constexpr bool contains_in_table(type_pack<Ts...> &&) {
    return (... || contains<T>(typename Ts::events_pack{}));
}


}
