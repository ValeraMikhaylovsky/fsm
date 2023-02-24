#include <cassert>
#include <iostream>
#include <fsm/fsm_ecpp.h>

using namespace ecpp::fsm;

// Events
struct lock   { int pincode {0}; };
struct unlock { int pincode {0}; };
struct status {};

// State machine definition
struct luggage_storage_def {
    //@{
    /** @name States */
    struct locked : state<locked> {
        struct print_status : action<print_status> {
            template <class FSM>
            void operator()(status &&/*event*/, FSM &/*fsm*/) const {
                std::cout << "status: locked" << std::endl;
            }
        };

        struct is_print : guard<print_status> {
            template <class FSM>
            bool operator()(const status &event, const FSM &fsm) const {
                return true;
            }
        };

        using internal_transitions = transition_table<
          /*  Event     Action         Guard    */
          in< status,   print_status , is_print  >
        >;
    };

    struct unlocked : state<unlocked> {
        struct print_status : action<print_status> {
            template <class FSM>
            void operator()(status &&/*event*/, FSM &/*fsm*/) const {
                std::cout << "status: unlocked" << std::endl;
            }
        };

        using internal_transitions = transition_table<
            /*  Event     Action  */
            in< status,   print_status >
        >;
    };
    //@}

    struct on_locked : action<on_locked> {
        template <class FSM>
        void operator()(lock &&event, FSM &fsm) const {
            fsm.m_pincode = event.pincode; // save pincode
            std::cout << "locked!" << std::endl;
        }
    };

    struct on_unlocked : action<on_unlocked> {
        template <class FSM>
        void operator()(unlock &&event, FSM &fsm) const {
            fsm.m_pincode = 0; // reset pincode
            std::cout << "unlocked!" << std::endl;
        }
    };

    struct is_set_pincode : guard<is_set_pincode> {
        template <class FSM>
        bool operator()(const lock &event, const FSM &) const {
            return event.pincode != 0;
        }
    };

    struct is_valid_pincode : guard<is_valid_pincode> {
        template <class FSM>
        bool operator()(const unlock &event, const FSM &fsm) const {
            return event.pincode == fsm.m_pincode; // check input pincode
        }
    };

    using initial_state = unlocked;
    using transitions   = transition_table
    <   /*  State       Event       Next         Action        Guard            */
        tr< unlocked,   lock  ,     locked  ,    on_locked,    is_set_pincode   >,
        tr< locked  ,   unlock,     unlocked,    on_unlocked,  is_valid_pincode >
    >;

private:
    int m_pincode {0};
};

// State machine object
using luggage_storage = ecpp::fsm::state_machine<luggage_storage_def>;

int main(int argc, char *argv[])
{
    luggage_storage fsm;

    assert(fsm.is_in_state<luggage_storage_def::unlocked>());
    assert(fsm.process_event(status{}) == event_result::done);          // print: "status: unlocked"
    assert(fsm.process_event(lock{0}) == event_result::refuse);         // incorrect pincode - ignore
    assert(fsm.is_in_state<luggage_storage_def::unlocked>());
    assert(fsm.process_event(lock{0x3254}) == event_result::done);      // correct pincode - switch to lock
    assert(fsm.is_in_state<luggage_storage_def::locked>());
    assert(fsm.process_event(status{}) == event_result::done);          // print: "status: locked"
    assert(fsm.process_event(unlock{0x7258}) == event_result::refuse);  // incorrect pincode - ignore
    assert(fsm.is_in_state<luggage_storage_def::locked>());
    assert(fsm.process_event(unlock{0x3254}) == event_result::done);    // correct pincode - switch to unlock
    assert(fsm.is_in_state<luggage_storage_def::unlocked>());
    assert(fsm.process_event(status{}) == event_result::done);          // print: "status: unlocked"

    return 0;
}
