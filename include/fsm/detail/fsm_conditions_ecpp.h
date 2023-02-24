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

    template <class Event, class FSM, class State>
    bool operator()(Event &&, FSM &, State &) const {
        static_assert(std::is_base_of_v<base_state, State>);
        return true;
    }
};

namespace impl {
    template <typename Guard>
    struct GuardHelper {
        static_assert(std::is_base_of_v<base_guard, Guard>, "Guard is not a guard type!");

        template <typename E, typename  FSM, typename SOURCE, typename TARGET>
        bool operator()(E &&event, FSM const &fsm, SOURCE const &src, TARGET const &dst) const {
            if constexpr (std::is_invocable_r_v<bool, Guard, E, FSM, SOURCE, TARGET>)
            return std::invoke(Guard{}, event, fsm, src, dst);
            else if constexpr (std::is_invocable_r_v<bool, Guard, E, FSM, SOURCE>)
            return std::invoke(Guard{}, event, fsm, src);
            else if constexpr (std::is_invocable_r_v<bool, Guard, E, FSM>)
            return std::invoke(Guard{}, event, fsm);
            return true;
        }

        template <typename E, typename  FSM, typename SOURCE>
        bool operator()(E &&event, FSM const &fsm, SOURCE const &src) const {
            if constexpr (std::is_invocable_r_v<bool, Guard, E, FSM, SOURCE>)
            return std::invoke(Guard{}, event, fsm, src);
            else if constexpr (std::is_invocable_r_v<bool, Guard, E, FSM>)
            return std::invoke(Guard{}, event, fsm);
            return true;
        }
    };
}

template  <class Predicate>
struct not_ : base_guard {
    template <typename... Args>
    bool operator()(Args&&... args) const {
        return !impl::GuardHelper<Predicate>{}(std::forward<Args>(args)...);
    }
};

template <class... Predicate>
struct and_ : base_guard {
    template <class... Args>
    bool operator()(Args&&... args) const {
        return (impl::GuardHelper<Predicate>{}(std::forward<Args>(args)...) && ...);
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
    template <class... Args>
    bool operator()(Args && ... args) const {
        return (impl::GuardHelper<Predicate>{}(std::forward<Args>(args)...) || ...);
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
