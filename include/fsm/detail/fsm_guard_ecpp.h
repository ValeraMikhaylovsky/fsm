#pragma once

#include <concepts>
#include <functional>
#include <type_traits>

namespace ecpp::fsm {

template<class G, class E, class F, class S>
concept CallGuard = requires(G guard, E event, F fsm, S src) {
    { guard(event, fsm, src) } -> std::convertible_to<bool>;
};

template<class G, class E, class F>
concept CallGuardShort = requires(G guard, E event, F fsm) {
    { guard(event, fsm) } -> std::convertible_to<bool>;
};

template  <class G>
struct call_guard {
    template<class E, class F, class S> requires CallGuard<G, E, F, S>
    bool operator()(const E &event, const F &fsm, const S &src) const noexcept {
        return G{}(event, fsm, src);
    }

    template<class E, class F, class S> requires CallGuardShort<G, E, F>
    bool operator()(const E &event, const F &fsm, const S &) const noexcept {
        return G{}(event, fsm);
    }

    template<class E, class F, class S>
    bool operator()(const E &, const F &, const S &) const noexcept {
        return true;
    }
};

template  <class G>
struct not_ {
    template<class E, class F, class S> requires CallGuard<G, E, F, S>
    bool operator()(const E &event, const F &fsm, const S &src) const noexcept {
        return !G{}(event, fsm, src);
    }

    template<class E, class F, class S> requires CallGuardShort<G, E, F>
    bool operator()(const E &event, const F &fsm, const S &) const noexcept {
        return !G{}(event, fsm);
    }

    template<class E, class F, class S>
    bool operator()(const E &, const F &, const S &) const noexcept {
        return false;
    }
};

template <class... G>
struct and_ {
    template<class E, class F, class S> requires ((CallGuard<call_guard<G>, E, F, S>) && ...)
    bool operator()(const E &event, const F &fsm, const S &src) const noexcept {
        return (call_guard<G>{}(event, fsm, src) && ...);
    }

    template<class E, class F, class S>
    bool operator()(const E &, const F &, const S &) const noexcept {
        return true;
    }
};

template <>
struct and_<> {
    template <class... Args>
    bool operator()(Args&&...) const noexcept {
        return false;
    }
};

template <class... G>
struct or_ {
    template<class E, class F, class S> requires ((CallGuard<call_guard<G>, E, F, S>) && ...)
    bool operator()(const E &event, const F &fsm, const S &src) const noexcept {
        return (call_guard<G>{}(event, fsm, src) || ...);
    }

    template<class E, class F, class S>
    bool operator()(const E &, const F &, const S &) const noexcept {
        return true;
    }
};

template <>
struct or_<> {
    template <class... Args>
    bool operator()(Args&&...) const noexcept {
        return true;
    }
};

}
