#include <cassert>
#include <iostream>
#include <fsm/fsm_ecpp.h>

using namespace ecpp::fsm;

// Events
struct push {};
struct coin {};

// State machine definition
struct turnstile_def {
    //@{
    /** @name States */
    struct locked   : state<locked> {

        struct beep {
            // partial specialization by event
            void operator()(const push &, auto &/*fsm*/) const {
                std::cout << "beep!" << std::endl;
            }
        };

        using internal_transitions = transition_table<
            /*  Event     Action  */
            in< push,     beep    >
        >;
    };

    struct unlocked : state<unlocked> {

    };
    //@}

    struct on_blink {
        void operator()(const coin &, auto&) const {
            std::cout << "blink, blink, blink!" << std::endl;
        }
    };

    using initial_state = locked;
    using transitions   = transition_table
      <
        /*  State       Event       Next        Action    */
        tr< locked,     coin,       unlocked,   on_blink  >,
        tr< unlocked,   push,       locked                >
      >;
    };

// State machine object
using turnstile = ecpp::fsm::state_machine<turnstile_def>;

int main(int argc, char *argv[])
{
    turnstile fsm;

    assert(fsm.is_in_state<turnstile_def::locked>());
    assert(fsm.process_event(push{}) == result::done); // beep!
    assert(fsm.process_event(coin{}) == result::done); // blink, blink, blink!
    assert(fsm.is_in_state<turnstile_def::unlocked>());
    assert(fsm.process_event(push{})  == result::done);
    assert(fsm.is_in_state<turnstile_def::locked>());

    return 0;
}
