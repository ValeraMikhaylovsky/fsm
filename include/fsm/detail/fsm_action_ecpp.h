#pragma once

#include <concepts>
#include <type_traits>

namespace ecpp::fsm {

template<class A, class E, class F, class S, class D>
concept CallActionLong = requires(A action, E event, F fsm, S src, D dst) {
    action(event, fsm, src, dst);
};

template<class A, class E, class F>
concept CallActionShort = requires(A action, E event, F fsm) {
    action(event, fsm);
};

template <class A>
struct call_action {
    template<class E, class F, class S, class D> requires CallActionLong<A, E, F, S, D>
    void operator()(const E &event, F &fsm, S &src, D &dst) noexcept {
        A{}(event, fsm, src, dst);
    }

    template<class E, class F, class S, class D> requires CallActionShort<A, E, F>
    void operator()(const E &event, F &fsm, S&, D&) noexcept {
        A{}(event, fsm);
    }

    template<class E, class F, class S, class D>
    void operator()(const E&, F&, S&, D&) noexcept {}
};

}
