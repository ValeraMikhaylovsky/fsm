#pragma once

#include <type_traits>
#include <variant>

#include "fsm_state_ecpp.h"
#include "fsm_transition_ecpp.h"
#include "fsm_meta_lib_ecpp.h"

namespace ecpp::fsm {

template<IsTransition ...T>
struct transition_table
{
    // в таблице переходов должно быть хотя бы один переход
    static_assert(sizeof...(T) > 0, "transition_table must be not empty!");
    static_assert(meta::no_dublicates<typename T::tag_t...>::value, "transition table contains duplicates");

    // определяем промежуточное состояние FSM, в котором оно будет находиться во время перехода между состояниями
    struct empty_state : state<empty_state> {};
    // извлекаем список уникальных состояний из всех переходов и добавляем пустое состояние
    using states_t = meta::unique_variant<typename T::source_t..., typename T::target_t..., empty_state>;
    // извлекаем типы всех событий
    using all_states_t = meta::type_pack<typename T::source_t..., typename T::target_t...>;
    // извлекаем список всех событий из всех переходов
    using events_t = meta::type_pack<typename T::event_t...>;
    // количество переходов
    static constexpr std::size_t count = sizeof...(T);
    // извлекаем все таблицы переходов определённые внутри состояний
    using internal_transitions = meta::non_void_type_pack<typename T::source_tr_t..., typename T::target_tr_t...>;

    // возвращает номер перехода, которому соответствует входной тип состояния и события
    template <class Tag>
    static inline constexpr std::size_t index_of() noexcept {
        constexpr bool bs[] = {std::is_same_v<Tag, typename T::tag_t>...};
        for (std::size_t index {0}; index < sizeof...(T); ++index) {
            if (bs[index])
                return index;
        }
        return sizeof...(T);
    }

    template <class E>
    static inline constexpr std::size_t internal_index_of() noexcept {
        constexpr bool bs[] = {(std::is_same_v<E, typename T::event_t>)...};
        for (std::size_t index {0}; index < sizeof...(T); ++index) {
            if (bs[index])
                return index;
        }
        return sizeof...(T);
    }

    template<std::size_t I>
    struct get_transition_type {
        using type = meta::get_type_from_index<I, T...>;
    };

    template <class U>
    static constexpr bool contains_in_table(meta::type_pack<>&&) noexcept {
        return false;
    }

    template <class E, class... Es>
    static constexpr bool contains_in_table(meta::type_pack<Es...> &&) noexcept {
        return (... || contains<E>(typename Es::events_t{}));
    }

    template<class E>
    struct has_event {
        static constexpr bool value = meta::contains<E>(events_t{}) || contains_in_table<E>(internal_transitions{});
    };
};


}
