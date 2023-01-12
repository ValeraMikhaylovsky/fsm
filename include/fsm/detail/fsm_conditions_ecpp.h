#pragma once

#include <type_traits>

namespace ecpp::fsm {

struct none : base_action, base_guard {
    template <class... Args>
    void operator()(Args&&...) const {

    }

    template <class FSM, IsState State, class Event>
    bool operator()(FSM &, State &, Event &&) const {
        return true;
    }
};

template  <IsGuard Predicate>
struct not_ : base_guard {
    template <typename... Args>
    bool operator()(Args&&... args) const {
        return !Predicate{}(::std::forward<Args>(args)...);
    }
};

template <IsGuard... Predicate>
struct and_ : base_guard {
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

template <IsGuard... Predicate>
struct or_ : base_guard {
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
