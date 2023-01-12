#pragma once

#include <concepts>
#include <type_traits>

namespace ecpp::fsm {

struct base_guard {};

template<class Child>
struct guard : base_guard {
    using child_t = Child;
    using base_t  = base_guard;

    template <class... Args>
    bool operator()(Args&&...) const {
        return true;
    }
};

template<typename T>
concept IsGuard = std::is_base_of_v<base_guard, T>;

}
