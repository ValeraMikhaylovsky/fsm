#pragma once

#include <type_traits>

namespace ecpp::fsm {

struct base_state {};

template<class Child>
struct state : base_state {
    using child_t = Child;
    using base_t  = base_state;
    using internal_transitions = void;
    using event_t = void;

    template <typename FSM, typename EVENT>
    void on_enter([[maybe_unused]] FSM &&fsm, [[maybe_unused]] EVENT &&event){}

    template <typename FSM, typename EVENT>
    void on_exit([[maybe_unused]] FSM &&fsm, [[maybe_unused]] EVENT &&event){}
};

}
