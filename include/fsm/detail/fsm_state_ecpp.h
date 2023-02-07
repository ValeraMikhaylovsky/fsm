#pragma once

#include <concepts>
#include <type_traits>

namespace ecpp::fsm {

struct base_state {};

template<class Child>
struct state : base_state {
    using child_t = Child;
    using base_t  = base_state;
    using internal_transitions = void;
    using event_t = void;

    void on_enter([[maybe_unused]] auto &&fsm, [[maybe_unused]] auto &&event){}
    void on_exit([[maybe_unused]] auto &&fsm, [[maybe_unused]] auto &&event){}
};

template<typename T>
concept IsState = std::is_base_of_v<base_state, T>;

}
