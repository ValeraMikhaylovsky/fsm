#pragma once

#include <iostream>
#include <functional>
#include <type_traits>

#include "detail/fsm_state_ecpp.h"
#include "detail/fsm_action_ecpp.h"
#include "detail/fsm_guard_ecpp.h"
#include "detail/fsm_transition_ecpp.h"
#include "detail/fsm_transition_table_ecpp.h"

namespace ecpp::fsm {

template <typename... Args>
void print_types(Args... args) {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
}

enum class result {
    refuse,
    done
};

/**
 * \class state_machine
 * \brief Реализация автомата состояний
 * \tparam T - тип, реализующий определение состояний и таблицы переходов.
 */
template<class T> requires requires { typename T::transitions; typename T::initial_state; }
struct state_machine : T {
    // извлекаем тип таблицы переходов
    using transitions_t = typename T::transitions;
    // извлекаем тип начального состояния
    using initial_state_t = typename T::initial_state;
    // извлекаем все типы событий из таблицы переходов
    using events_t = typename transitions_t::events_t;

    state_machine(const state_machine&) = delete;
    state_machine& operator=(const state_machine&) = delete;

    state_machine() noexcept : T{} {
        std::visit([&, fsm = static_cast<T&>(*this)](auto &t_current) {
            call_on_entry{}(t_current, fsm);
        }, m_current);
    }

    template <typename... Args>
    constexpr explicit state_machine(Args&&... args) noexcept : T{std::forward<Args>(args)...} {
        std::visit([&, fsm = static_cast<T&>(*this)](auto &t_current) {
            call_on_entry{}(t_current, fsm);
        }, m_current);
    }

    ~state_machine() {
        std::visit([&, fsm = static_cast<T&>(*this)](auto &t_current) {
            call_on_exit{}(t_current, fsm);
        }, m_current);
    }

    /**
     * \brief Обработка события.
     * \tparam E - перемещаемый тип события, должен присутствовать в таблице переходов.
     * \param event - перемещаемая ссылка на сам объект события.
     * \return результат выполнения.
     */
    template<class E>
    result process_event(E &&event) noexcept {
        // проверка на этапе компиляции, что тип события содержится в таблице переходов
        static_assert(transitions_t::template has_event<E>::value, "unknown event type, this type is missing from the transition table!");
        // задаём статус выполнения по умолчанию
        auto t_result {result::refuse};
        // ссылка на контекст (принудительно определяем именно ссылку, что бы избежать неявного копирования)
        T &fsm = static_cast<T&>(*this);
        // определяем текущее состояние
        std::visit([&](auto &t_source) noexcept {
            // извлекаем типы текущего состояния и события
            using source_t = std::decay_t<decltype(t_source)>;
            using event_t = std::decay_t<decltype(event)>;
            // находим порядковый номер перехода, соответствующий текущему состоянию и входному событию
            constexpr auto tr_index = transitions_t::template index_of<meta::type_pack<source_t,event_t>>();
            if constexpr (tr_index < transitions_t::count) {
                // создаём экземпляр перехода, для извлечения необходимых типов
                using transition_t = typename transitions_t::template get_transition_type<tr_index>::type;
                using guard_t  = typename transition_t::guard_t;
                using action_t = typename transition_t::action_t;
                using target_t = typename transition_t::target_t;
                // проверяем, что GUARD разрешает переход
                if (!call_guard<guard_t>{}(std::forward<E>(event), fsm, t_source))
                    return;
                // завершаем текущее состояние
                call_on_exit{}(t_source, std::forward<E>(event), fsm);
                // выход из текущего состояния и переход в пустое состояние
                m_current = typename transitions_t::empty_state{};
                // создаём экземпляр следующего состояния
                target_t t_target{};
                // выполняем действие
                call_action<action_t>{}(std::forward<E>(event), fsm, t_source, t_target);
                // выполняем переход FSM из пустого состояния в новое состояние
                m_current = std::move(t_target);
                // выполняем вход в новое состояние
                call_on_entry{}(*std::get_if<target_t>(&m_current), std::forward<E>(event), fsm);
                // помечаем результат как выполненный
                t_result = result::done;
            }
            else {
                using internal_transitions_t = typename std::decay_t<decltype(t_source)>::internal_transitions;
                if constexpr (!std::is_same_v<internal_transitions_t, void>) {
                    using event_t = std::decay_t<decltype(event)>;
                    constexpr auto in_index = internal_transitions_t::template internal_index_of<event_t>();
                    if constexpr (in_index < internal_transitions_t::count) {
                        using transition_t = typename internal_transitions_t::template get_transition_type<in_index>::type;
                        using action_t = typename transition_t::action_t;
                        using guard_t  = typename transition_t::guard_t;
                        // проверяем, что GUARD разрешает переход
                        if (call_guard<guard_t>{}(std::forward<E>(event), fsm, t_source)) {
                            // выполняем действие
                            call_action<action_t>{}(std::forward<E>(event), fsm, t_source, t_source);
                            t_result = result::done;
                        }
                    }
                }
            }
        }, m_current);
        return t_result;
    }

    /**
     * \brief Возвращает true если FSM находится в заданном состоянии, иначе вернёт false.
     * \tparam State - тип состояния.
     */
    template <IsState State>
    [[nodiscard]] constexpr bool is_in_state() const noexcept {
        static_assert(meta::contains<State>(typename transitions_t::all_states_t{}), "the state is missing from the transitions table");
        return std::holds_alternative<State>(m_current);
    }

private:
    // создаём std::variant со всеми состояниями и инициализируем начальным состоянием
    typename transitions_t::states_t m_current {initial_state_t{}};
};

}
