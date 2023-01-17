#pragma once

#include <type_traits>
#include "fsm_conditions_ecpp.h"

namespace ecpp::fsm {

struct transition_base {};

template <class Source,
          class Event,
          class Target,
          class Action = none,
          class Guard = none>
struct tr : transition_base
{
    static_assert(std::is_base_of_v<base_state, Source>, "source state is not a state type!");
    static_assert(std::is_base_of_v<base_state, Target>, "target state is not a state type!");
    static_assert(std::is_base_of_v<base_action, Action>, "action is not a action type!");
    static_assert(std::is_base_of_v<base_guard, Guard>, "guard is not a guard type!");

    using source_t = Source;
    using event_t  = Event;
    using target_t = Target;
    using action_t = Action;
    using guard_t  = Guard;
    using source_tr_t = typename source_t::internal_transitions;
    using target_tr_t = typename source_t::internal_transitions;

    static_assert(!std::is_same_v<Source, Target>, "source state and target state is same!");
};

template <class Event, class Action, class Guard = none>
struct in : transition_base {
    static_assert(std::is_base_of_v<base_action, Action>, "action is not a action type!");
    static_assert(std::is_base_of_v<base_guard, Guard>, "guard is not a guard type!");

    using event_t  = Event;
    using action_t = Action;
    using guard_t  = Guard;
    using source_t = void;
    using target_t = void;
    using source_tr_t = void;
    using target_tr_t = void;
};


template <class Ts>
constexpr bool operator==(Ts, Ts) {
    return true;
}

template <class Ts, class Us>
constexpr bool operator==(Ts, Us) {
    return false;
}

template <class Ts>
constexpr bool operator!=(Ts, Ts) {
    return false;
}

template <class Ts, class Us>
constexpr bool operator!=(Ts, Us) {
    return true;
}

}
