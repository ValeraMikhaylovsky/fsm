#include "pimpl.h"
#include <iostream>
#include <fsm/fsm_ecpp.h>

using namespace ecpp::fsm;

// Events
namespace events {
    struct start {};
    struct stop {};
}

// State machine definition
struct minimal_def {
    //@{
    /** @name States */
    struct initial      : state<initial> {};
    struct running      : state<running> {};
    struct terminated   : state<terminated> {};
    //@}

    //@{
    /** @name Actions */
    struct on_start : action<on_start> {
        void operator()(auto &&/*event*/, auto &/*fsm*/, const auto &/*source_state*/, const auto &/*target_state*/) const {
            std::cout << "started" << std::endl;
        }
    };

    struct on_stop : action<on_start> {
        void operator()(auto &&/*event*/, auto &/*fsm*/, const auto &/*source_state*/, const auto &/*target_state*/) const {
            std::cout << "stoped" << std::endl;
        }
    };
    //@}

    using initial_state = initial;
    using transitions   = transition_table
    <
        /*  State       Event            Next        */
        tr< initial,    events::start,   running   ,   on_start  >,
        tr< running,    events::stop,    terminated,   on_stop   >
    >;

    minimal_def(pimpl &context) : m_context{context}{}

private:
    pimpl &m_context;
};

// State machine object
struct pimpl::impl : ecpp::fsm::state_machine<minimal_def>
{
    impl(pimpl &context) : ecpp::fsm::state_machine<minimal_def>{context} {

    }
};



pimpl::pimpl() : pImpl{std::make_unique<impl>(*this)}
{

}

pimpl::~pimpl()
{

}

void pimpl::start()
{
    pImpl->process_event(events::start{});
}

void pimpl::stop()
{
    pImpl->process_event(events::stop{});
}

bool pimpl::is_started() const
{
    return pImpl->is_in_state<minimal_def::running>();
}
