#pragma once

#include <type_traits>

namespace ecpp::fsm {

struct base_action {};

template<class Child>
struct action : base_action {
    using child_t = Child;
    using base_t  = base_action;
};


}
