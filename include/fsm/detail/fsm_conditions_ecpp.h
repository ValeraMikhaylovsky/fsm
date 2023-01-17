#pragma once

#include <type_traits>
#include <utility>
#include "fsm_action_ecpp.h"
#include "fsm_guard_ecpp.h"
#include "fsm_state_ecpp.h"

namespace ecpp::fsm {

struct none : base_action, base_guard {
    template <class... Args>
    void operator()(Args&&...) const {}

    template <class FSM, class State, class Event>
    bool operator()(FSM &, State &, Event &&) const {
        static_assert(std::is_base_of_v<base_state, State>);
        return true;
    }
};

template  <class Predicate>
struct not_ : base_guard {
    static_assert(std::is_base_of_v<base_guard, Predicate>);

    template <typename... Args>
    bool operator()(Args&&... args) const {
        return !Predicate{}(std::forward<Args>(args)...);
    }
};

template <class... Predicate>
struct and_ : base_guard {
    static_assert(std::is_base_of_v<base_guard, Predicate...>);

    template <class... Args>
    bool operator()(Args&&... args) const {
        return (Predicate{}(std::forward<Args>(args)...) && ...);
    }
};

template <>
struct and_<> : base_guard {
    template <class... Args>
    bool operator()(Args&&...) const {
        return false;
    }
};

template <class... Predicate>
struct or_ : base_guard {
    static_assert(std::is_base_of_v<base_guard, Predicate...>);

    template <class... Args>
    bool operator()(Args && ... args) const {
        return (Predicate{}(std::forward<Args>(args)...) || ...);
    }
};

template <>
struct or_<> : base_guard {
    template <class... Args>
    bool operator()(Args&&...) const {
        return true;
    }
};

}
