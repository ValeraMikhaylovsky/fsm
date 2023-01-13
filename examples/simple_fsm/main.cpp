#include <cassert>
#include <fsm/fsm_ecpp.h>

using namespace ecpp::fsm;

// Events
struct start {};
struct stop {};

// State machine definition
struct minimal_def {
    //@{
    /** @name States */
    struct initial      : state<initial> {};
    struct running      : state<running> {};
    struct terminated   : state<terminated> {};
    //@}

    using initial_state = initial;
    using transitions   = transition_table
      <
        /*  State       Event       Next        */
        tr< initial,    start,      running     >,
        tr< running,    stop,       terminated  >
      >;
    };

// State machine object
using minimal = ecpp::fsm::state_machine<minimal_def>;

int main(int argc, char *argv[])
{
    minimal fsm;

    assert(fsm.is_in_state<minimal_def::initial>());
    assert(fsm.process_event(start{}) == event_result::done);
    assert(fsm.is_in_state<minimal_def::running>());
    assert(fsm.process_event(stop{})  == event_result::done);
    assert(fsm.is_in_state<minimal_def::terminated>());

    return 0;
}
