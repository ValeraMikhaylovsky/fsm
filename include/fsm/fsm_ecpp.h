#pragma once

#include <iostream>
#include <functional>
#include <type_traits>

#include "detail/fsm_state_ecpp.h"
#include "detail/fsm_action_ecpp.h"
#include "detail/fsm_guard_ecpp.h"
#include "detail/fsm_conditions_ecpp.h"
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

template<class T>
struct state_machine : T {
    using transitions_pack_t = typename T::transitions;
    using events_pack_t = typename transitions_pack_t::events_pack;
    using initial_state_t = typename T::initial_state;

    static_assert(!std::is_same_v<initial_state_t, void>, "the initial state is not defined");
    static_assert(!std::is_same_v<transitions_pack_t, void>, "the transition table is not defined");

    constexpr state_machine() : T{} {
        std::visit([&](auto &&var) {
            var.on_enter(static_cast<state_machine<T>&>(*this), nullptr);
        }, m_current);
    }

    template <typename... Args>
    constexpr state_machine(Args&&... args) : T{std::forward<Args>(args)...} {
        std::visit([&](auto &&var) {
            var.on_enter(static_cast<state_machine<T>&>(*this), nullptr);
        }, m_current);
    }

    constexpr ~state_machine() {
        std::visit([&](auto &&var) {
            var.on_exit(static_cast<state_machine<T>&>(*this), nullptr);
        }, m_current);
    }

    template<class E>
    event_result process_event(E && event) {
        event_result t_result {event_result::refuse};
        static_assert(contains<E>(events_pack_t{}) || contains_in_table<E>(typename transitions_pack_t::internal_transitions{}), "the event is missing from the transitions table");

        std::visit([&](auto &&t_source) mutable {
            if (const auto t_transition_index = transitions_pack_t::get_index(t_source, event); t_transition_index < transitions_pack_t::count) {
                auto t_transition = transitions_pack_t::make_transition(t_transition_index);
                std::visit([&](auto &&transition) mutable {
                    using transition_t = std::decay_t<decltype(transition)>;
                    using guard_t  = typename transition_t::guard_t;
                    using action_t = typename transition_t::action_t;
                    using source_t = typename std::decay_t<decltype(t_source)>;
                    using target_t = typename transition_t::target_t;
                    target_t t_target;
                    if (impl::GuardHelper<guard_t> t_guard; t_guard(event, static_cast<state_machine<T> const&>(*this), static_cast<source_t const &>(t_source), static_cast<target_t const &>(t_target))) {
                        t_source.on_exit(static_cast<state_machine<T>&>(*this), std::forward<E>(event));
                        m_current  = typename transitions_pack_t::empty_state{};
                        if constexpr (std::is_invocable_v<action_t, E, state_machine<T>&, source_t, target_t>)
                            std::invoke(action_t{}, std::forward<E>(event), static_cast<state_machine<T>&>(*this), t_source, t_target);
                        else if constexpr (std::is_invocable_v<action_t, E, state_machine<T>&, source_t>)
                            std::invoke(action_t{}, std::forward<E>(event), static_cast<state_machine<T>&>(*this), t_source);
                        else if constexpr (std::is_invocable_v<action_t, E, state_machine<T>&>)
                            std::invoke(action_t{}, std::forward<E>(event), static_cast<state_machine<T>&>(*this));
                        t_target.on_enter(static_cast<state_machine<T>&>(*this), std::forward<E>(event));
                        m_current = std::move(t_target);
                        t_result = event_result::done;
                    }
                }, t_transition);
            }
            else {
                using internal_transitions_t = typename std::decay_t<decltype(t_source)>::internal_transitions;
                if constexpr (!std::is_same_v<internal_transitions_t, void>) {
                    if (const auto t_transition_index = internal_transitions_t::get_index(event); t_transition_index < internal_transitions_t::count) {
                        auto t_transition = internal_transitions_t::make_transition(t_transition_index);
                        std::visit([&](auto &&transition) mutable {
                            using transition_t = std::decay_t<decltype(transition)>;
                            using action_t = typename transition_t::action_t;
                            using guard_t  = typename transition_t::guard_t;
                            using source_t = typename std::decay_t<decltype(t_source)>;
                            if (impl::GuardHelper<guard_t> t_guard; t_guard(event, static_cast<state_machine<T> const&>(*this), static_cast<source_t const &>(t_source))) {
                                if constexpr (std::is_invocable_v<action_t, E, state_machine<T>&, source_t>)
                                    std::invoke(action_t{}, std::forward<E>(event), static_cast<state_machine<T> &>(*this), static_cast<source_t&>(t_source));
                                else if constexpr (std::is_invocable_v<action_t, E, state_machine<T>&>)
                                    std::invoke(action_t{}, std::forward<E>(event), static_cast<state_machine<T> &>(*this));
                            }
                            t_result = event_result::done;
                        }, t_transition);
                    }
                }
            }
        }, m_current);

        return t_result;
    }

    template <IsState State>
    constexpr bool is_in_state() const noexcept {
        static_assert(contains<State>(typename transitions_pack_t::states_pack{}), "the state is missing from the transitions table");
        return std::holds_alternative<State>(m_current);
    }

private:
    typename transitions_pack_t::states_variant m_current {initial_state_t{}};
};

}
