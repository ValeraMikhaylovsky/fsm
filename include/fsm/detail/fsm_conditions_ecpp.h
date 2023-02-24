#pragma once

#include <type_traits>
#include "fsm_guard_ecpp.h"

namespace ecpp::fsm {

struct none : base_action, base_guard {

};

namespace impl {
template <IsGuard Guard>
struct GuardHelper {
    bool operator()(auto &&event, auto const &fsm, auto const &src, auto const &dst) const {
        using event_t  = std::decay_t<decltype(event)>;
        using fsm_t    = std::decay_t<decltype(fsm)>;
        using source_t = std::decay_t<decltype(src)>;
        using target_t = std::decay_t<decltype(dst)>;

        if constexpr (std::is_invocable_r_v<bool, Guard, event_t, fsm_t, source_t, target_t>)
            return std::invoke(Guard{}, event, fsm, src, dst);
        else if constexpr (std::is_invocable_r_v<bool, Guard, event_t, fsm_t, source_t>)
            return std::invoke(Guard{}, event, fsm, src);
        else if constexpr (std::is_invocable_r_v<bool, Guard, event_t, fsm_t>)
            return std::invoke(Guard{}, event, fsm);
        return true;
    }

    bool operator()(auto &&event, auto const &fsm, auto const &src) const {
        using event_t  = std::decay_t<decltype(event)>;
        using fsm_t    = std::decay_t<decltype(fsm)>;
        using source_t = std::decay_t<decltype(src)>;

        if constexpr (std::is_invocable_r_v<bool, Guard, event_t, fsm_t, source_t>)
            return std::invoke(Guard{}, event, fsm, src);
        else if constexpr (std::is_invocable_r_v<bool, Guard, event_t, fsm_t>)
            return std::invoke(Guard{}, event, fsm);
        return true;
    }
};
}

template  <IsGuard Predicate>
struct not_ : base_guard {
    template <class... Args>
    bool operator()(Args&&... args) const {
        return !impl::GuardHelper<Predicate>{}(std::forward<Args>(args)...);
    }
};

template <IsGuard... Predicate>
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

template <IsGuard... Predicate>
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
