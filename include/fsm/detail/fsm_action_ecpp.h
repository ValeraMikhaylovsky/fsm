#pragma once

#include <concepts>
#include <type_traits>

namespace ecpp::fsm {

struct base_action {};

template<class Child>
struct action : base_action {
    using child_t = Child;
    using base_t  = base_action;
};

template<typename T>
concept IsAction = std::is_base_of_v<base_action, T>;

}
