#pragma once

#include <type_traits>
#include "fsm_meta_lib_ecpp.h"

namespace ecpp::fsm {

struct none {};

struct transition_base {};

template <IsState Source, class Event, IsState Target, class Action = none, class Guard = none>
struct tr : transition_base
{
    using source_t = Source;
    using event_t  = Event;
    using target_t = Target;
    using action_t = Action;
    using guard_t  = Guard;

    using source_tr_t = typename source_t::internal_transitions;
    using target_tr_t = typename target_t::internal_transitions;

    // тип идентификатор уникальности
    using tag_t = meta::type_pack<source_t, event_t>;

    // исходное и конечное состояния не могут быть одним и тем же
    static_assert(!std::is_same_v<Source, Target>, "source state and target state is same!");
};

template <class Event, class Action, class Guard = none>
struct in : transition_base {
    using event_t  = Event;
    using action_t = Action;
    using guard_t  = Guard;
    using source_t = void;
    using target_t = void;
    using source_tr_t = void;
    using target_tr_t = void;

    // тип идентификатор уникальности
    using tag_t = meta::type_pack<event_t, action_t>;
};

template<typename T>
concept IsTransition = std::is_base_of_v<transition_base, T>;

}
