#pragma once

#include <type_traits>
#include "fsm_conditions_ecpp.h"

namespace ecpp::fsm {

struct transition_base {};

template <IsState Source,
          class Event,
          IsState Target,
          IsAction Action = none,
          IsGuard Guard = none>
struct tr : transition_base
{
    using source_t = Source;
    using event_t  = Event;
    using target_t = Target;
    using action_t = Action;
    using guard_t  = Guard;
    using source_tr_t = typename source_t::internal_transitions;
    using target_tr_t = typename source_t::internal_transitions;

    static_assert(!std::is_same_v<Source, Target>, "source state and target state is same!");
};

template <class Event, IsAction Action, IsGuard Guard = none>
struct in : transition_base {
    using event_t  = Event;
    using action_t = Action;
    using guard_t  = Guard;
    using source_t = void;
    using target_t = void;
    using source_tr_t = void;
    using target_tr_t = void;
};

template<typename T>
concept IsTransition = std::is_base_of_v<transition_base, T>;

template <IsTransition Ts>
constexpr bool operator==(Ts, Ts) {
    return true;
}

template <IsTransition Ts, IsTransition Us>
constexpr bool operator==(Ts, Us) {
    return false;
}

template <IsTransition Ts>
constexpr bool operator!=(Ts, Ts) {
    return false;
}

template <IsTransition Ts, IsTransition Us>
constexpr bool operator!=(Ts, Us) {
    return true;
}

}
