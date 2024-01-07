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

enum class event_result {
    refuse,
    done
};

template <class T>
concept IsFsmDefinition = requires(T def) {
    typename T::transitions;
    typename T::initial_state;
};

template<IsFsmDefinition T>
struct state_machine : T {
    // извлекаем тип таблицы переходов
    using transitions_t = typename T::transitions;
    // извлекаем тип начального состояния
    using initial_state_t = typename T::initial_state;
    // извлекаем все типы событий из таблицы переходов
    using events_t = typename transitions_t::events_t;

    state_machine(const state_machine&) = delete;
    state_machine& operator=(const state_machine&) = delete;

    state_machine() : T{} {
        std::visit([&, fsm = static_cast<T&>(*this)](auto &t_current) mutable {
            call_on_enter{}(t_current, fsm);
        }, m_current);
    }

    template <typename... Args>
    constexpr state_machine(Args&&... args) : T{std::forward<Args>(args)...} {
        std::visit([&, fsm = static_cast<T&>(*this)](auto &t_current) mutable {
            call_on_enter{}(t_current, fsm);
        }, m_current);
    }

    ~state_machine() {
        std::visit([&, fsm = static_cast<T&>(*this)](auto &t_current) mutable {
            call_on_exit{}(t_current, fsm);
        }, m_current);
    }

    /**
     * \brief Обработка события.
     * \tparam E - перемещаемый тип события, должен присутствовать в таблице переходов.
     * \param event - перемещаемая ссылка на сам объект события.
     * \return результат выполнения.
     */
    template<std::movable E>
    event_result process_event(E &&event) {
        // проверка на этапе компиляции, что тип события содержится в таблице переходов
        static_assert(transitions_t::template has_event<E>::value, "unknown event type, this type is missing from the transition table!");
        // задаём статус выполнения по умолчанию
        event_result t_result {event_result::refuse};
        T& m_fsm = static_cast<T&>(*this);
        // определяем текущее состояние
        std::visit([&](auto &t_source) mutable {
            // находим порядковый номер перехода, соответствующий текущему состоянию и входному событию
            if (const auto t_transition_index = transitions_t::index_of(t_source, event); t_transition_index < transitions_t::count) {
                // создаём экземпляр перехода, для извлечения необходимых типов
                auto t_transition = transitions_t::make_transition(t_transition_index);
                std::visit([&](auto &&transition) mutable {
                    // извлекаем типы
                    using transition_t = std::decay_t<decltype(transition)>;
                    using guard_t  = typename transition_t::guard_t;
                    using action_t = typename transition_t::action_t;
                    using source_t = typename transition_t::source_t;
                    using target_t = typename transition_t::target_t;
                    using event_t  = typename transition_t::event_t;
                    // проверяем, что генерируется реализация для текущего типа события и типа исходного состояния
                    if constexpr (std::is_same_v<source_t, std::decay_t<decltype(t_source)>> &&
                                  std::is_same_v<event_t, std::decay_t<decltype(event)>>)
                    {
                        // проверяем, что GUARD разрешает переход
                        if (!call_guard<guard_t>{}(std::forward<E>(event), m_fsm, t_source))
                            return;
                        // завершаем текущее состояние
                        call_on_exit{}(t_source, std::forward<E>(event), m_fsm);
                        // выход из текущего состояния и переход в пустое состояние
                        m_current = typename transitions_t::empty_state{};
                        // создаём экземпляр следующего состояния
                        target_t t_target{};
                        // выполняем действие
                        call_action<action_t>{}(std::forward<E>(event), m_fsm, t_source, t_target);
                        // выполняем переход FSM из пустого состояния в новое состояние
                        m_current = std::move(t_target);
                        // выполняем вход в новое состояние
                        std::visit([&](auto &t_current) mutable {
                            // проверяем, что генерируется реализация для текущего типа события и типа исходного состояния
                            if constexpr (std::is_same_v<source_t, std::decay_t<decltype(t_source)>> &&
                                          std::is_same_v<target_t, std::decay_t<decltype(t_current)>> &&
                                          std::is_same_v<event_t, std::decay_t<decltype(event)>>)
                            {
                                call_on_enter{}(t_current, std::forward<E>(event), m_fsm);
                            }
                        }, m_current);
                        // помечаем результат как выполненный
                        t_result = event_result::done;
                    }
                }, t_transition);
            }
            else {
                using internal_transitions_t = typename std::decay_t<decltype(t_source)>::internal_transitions;
                if constexpr (!std::is_same_v<internal_transitions_t, void>) {
                    if (const auto t_transition_index = internal_transitions_t::index_of(event); t_transition_index < internal_transitions_t::count) {
                        auto t_transition = internal_transitions_t::make_transition(t_transition_index);
                        std::visit([&](auto &&transition) mutable {
                            using transition_t = std::decay_t<decltype(transition)>;
                            using action_t = typename transition_t::action_t;
                            using guard_t  = typename transition_t::guard_t;
                            // проверяем, что GUARD разрешает переход
                            if (!call_guard<guard_t>{}(std::forward<E>(event), m_fsm, t_source))
                                return;
                            // выполняем действие
                            call_action<action_t>{}(std::forward<E>(event), m_fsm, t_source, t_source);
                            t_result = event_result::done;
                        }, t_transition);
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
    constexpr bool is_in_state() const noexcept {
        static_assert(contains<State>(typename transitions_t::all_states_t{}), "the state is missing from the transitions table");
        return std::holds_alternative<State>(m_current);
    }

private:
    // создаём std::variant со всеми состояниями и инициализируем начальным состоянием
    typename transitions_t::states_t m_current {initial_state_t{}};
};

}
