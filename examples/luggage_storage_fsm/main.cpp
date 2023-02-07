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
            void operator()(auto &&, auto &&) const {
                std::cout << "status: locked" << std::endl;
            }
        };

        using internal_transitions = transition_table<
            /*  Event     Action  */
            in< status,   print_status    >
        >;
    };

    struct unlocked : state<unlocked> {
        struct print_status : action<print_status> {
            void operator()(auto &&, auto &&) const {
                std::cout << "status: unlocked" << std::endl;
            }
        };

        using internal_transitions = transition_table<
            /*  Event     Action  */
            in< status,   print_status    >
        >;
    };
    //@}

    struct on_locked : action<on_locked> {
        template <class FSM, class Event, class SourceState, class TargetState>
        void operator()(Event &&event, FSM &fsm, const SourceState &, const TargetState &) const {
            if constexpr (!std::is_same_v<Event, status>) {
                root_machine(fsm).m_pincode = event.pincode; // save pincode
                std::cout << "locked!" << std::endl;
            }
        }
    };

    struct on_unlocked : action<on_unlocked> {
        template <class FSM, class Event, class SourceState, class TargetState>
        void operator()(Event &&event, FSM &fsm, const SourceState &, const TargetState &) const {
            root_machine(fsm).m_pincode = 0; // reset pincode
            std::cout << "unlocked!" << std::endl;
        }
    };

    struct is_set_pincode : guard<is_set_pincode> {
        template < class FSM, class State, class Event >
        bool operator()(FSM const &, State const&, Event const &event) const {
            if constexpr (!std::is_same_v<Event, status>)
                return event.pincode != 0;
            else
                return true;
        }
    };

    struct is_valid_pincode : guard<is_valid_pincode> {
        template < class FSM, class State, class Event >
        bool operator()(FSM const &fsm, State const&, Event const &event) const {
            if constexpr (!std::is_same_v<Event, status>)
                return event.pincode == root_machine(fsm).m_pincode; // check input pincode
            else
                return true;
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
