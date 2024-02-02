#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

namespace ecpp::fsm {

struct base_state {};

template<class Child>
struct state : base_state {
    using internal_transitions = void;
};

template<class T>
concept IsState = std::is_base_of_v<base_state, T>;

template<class S, class E, class F>
concept CallOnEntry = requires(S state, E event, F fsm) {
    state.on_entry(event, fsm);
};

template<class S, class F>
concept CallShortOnEntry = requires(S state, F fsm) {
    state.on_entry(fsm);
};

template<class S, class E, class F>
concept CallOnExit = requires(S state, E event, F fsm) {
    state.on_exit(event, fsm);
};

template<class S, class F>
concept CallShortOnExit = requires(S state, F fsm) {
    state.on_exit(fsm);
};

struct call_on_entry {
    template<class State, class FSM, class E> requires CallOnEntry<State, E, FSM>
    void operator()(State &state, const E &event, FSM &fsm) const noexcept { state.on_entry(event, fsm); }

    template<class State, class FSM, class E>
    void operator()(State &state, const E &e, FSM &fsm) const noexcept {}

    template<class State, class FSM> requires CallShortOnEntry<State, FSM>
    void operator()(State &state, FSM &fsm) const noexcept { state.on_entry(fsm); }

    template<class State, class FSM>
    void operator()(State &state, FSM &fsm) const noexcept {}
};

struct call_on_exit {
    template<class State, class FSM, class E> requires CallOnExit<State, E, FSM>
    void operator()(State &state, const E &event, FSM &fsm) const noexcept { state.on_exit(event, fsm); }

    template<class State, class FSM, class E>
    void operator()(State &state, const E &e, FSM &fsm) const noexcept {}

    template<class State, class FSM> requires CallShortOnExit<State, FSM>
    void operator()(State &state, FSM &fsm) const noexcept { state.on_exit(fsm); }

    template<class State, class FSM>
    void operator()(State &state, FSM &fsm) const noexcept {}
};

}
