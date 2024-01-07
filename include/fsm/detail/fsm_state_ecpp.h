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
concept CallOnEnter = requires(S state, E event, F fsm) {
    state.on_enter(event, fsm);
};

template<class S, class F>
concept CallShortOnEnter = requires(S state, F fsm) {
    state.on_enter(fsm);
};

template<class S, class E, class F>
concept CallOnExit = requires(S state, E event, F fsm) {
    state.on_exit(event, fsm);
};

template<class S, class F>
concept CallShortOnExit = requires(S state, F fsm) {
    state.on_exit(fsm);
};

struct call_on_enter {
    template<class State, class FSM, class E> requires CallOnEnter<State, E, FSM>
    void operator()(State &state, const E &event, FSM &fsm){ state.on_enter(event, fsm); }

    template<class State, class FSM, class E>
    void operator()(State &state, const E &e, FSM &fsm){}

    template<class State, class FSM> requires CallShortOnEnter<State, FSM>
    void operator()(State &state, FSM &fsm){ state.on_enter(fsm); }

    template<class State, class FSM>
    void operator()(State &state, FSM &fsm){}
};

struct call_on_exit {
    template<class State, class FSM, class E> requires CallOnExit<State, E, FSM>
    void operator()(State &state, const E &event, FSM &fsm){ state.on_exit(event, fsm); }

    template<class State, class FSM, class E>
    void operator()(State &state, const E &e, FSM &fsm){}


    template<class State, class FSM> requires CallShortOnExit<State, FSM>
    void operator()(State &state, FSM &fsm){ state.on_exit(fsm); }

    template<class State, class FSM>
    void operator()(State &state, FSM &fsm){}
};

}
