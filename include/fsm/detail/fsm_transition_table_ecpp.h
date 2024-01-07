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
    // в таблице переходов должно быть хотя бы один переход
    static_assert(sizeof...(T) > 0, "transition_table must be not empty!");
    static_assert(no_dublicates<typename T::tag_t...>::value, "transition table contains duplicates");

    // определяем промежуточное состояние FSM, в котором оно будет находиться во время перехода между состояниями
    struct empty_state : state<empty_state> {};
    // извлекаем список уникальных состояний из всех переходов и добавляем пустое состояние
    using states_t = unique_variant<typename T::source_t..., typename T::target_t..., empty_state>;
    // извлекаем типы всех событий
    using all_states_t = type_pack<typename T::source_t..., typename T::target_t...>;
    // извлекаем список всех событий из всех переходов
    using events_t = type_pack<typename T::event_t...>;
    // количество переходов
    static constexpr std::size_t count = sizeof...(T);
    // извлекаем все таблицы переходов определённые внутри состояний
    using internal_transitions = non_void_type_pack<typename T::source_tr_t..., typename T::target_tr_t...>;

    // возвращает номер перехода, которому соответствует входной тип состояния и события
    static inline constexpr std::size_t index_of(const auto &s, const auto &e) noexcept {
        using tag_t = type_pack<std::decay_t<decltype(s)>, std::decay_t<decltype(e)>>;
        constexpr bool bs[] = {std::is_same_v<tag_t, typename T::tag_t>...};
        for (std::size_t index {0}; index < sizeof...(T); ++index) {
            if (bs[index])
                return index;
        }
        return sizeof...(T);
    }

    static inline constexpr std::size_t index_of(const auto &e) {
        using event_t = std::decay_t<decltype(e)>;
        constexpr bool bs[] = {(std::is_same_v<event_t, typename T::event_t>)...};
        for (std::size_t index {0}; index < sizeof...(T); ++index) {
            if (bs[index])
                return index;
        }
        return sizeof...(T);
    }

    static inline constexpr std::variant<T...> make_transition(std::size_t index) {
        return make_current_variant<T...>(std::index_sequence_for<T...>(), index);
    }

    template <class U>
    static constexpr bool contains_in_table(type_pack<>&&) {
        return false;
    }

    template <class E, class... Es>
    static constexpr bool contains_in_table(type_pack<Es...> &&) {
        return (... || contains<E>(typename Es::events_t{}));
    }

    template<class E>
    struct has_event {
        static constexpr bool value = contains<E>(events_t{}) || contains_in_table<E>(internal_transitions{});
    };
};

}
