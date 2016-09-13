#ifndef _FsmWithStateName_h_
#define _FsmWithStateName_h_

#include "toolbox/fsm/FiniteStateMachine.h"
#include "xdata/String.h"


/**
 * Adds public attributes stateName_ and state_ to
 * toolbox::fsm::FiniteStateMachine
 */
class FsmWithStateName:
public toolbox::fsm::FiniteStateMachine
{
public:

    /**
     * Application state as an "integer" - to be used in event building loop.
     */
    toolbox::fsm::State state_;


    /**
     * Application state as a string - to be used an exported parameter for
     * run-control.
     */
    xdata::String stateName_;


    /**
     * Calls FiniteStateMachine::reset() and keeps stateName_ and state_
     * in sync.
     */
    void reset() throw (toolbox::fsm::exception::Exception)
    {
        FiniteStateMachine::reset();

        state_     = FiniteStateMachine::getCurrentState();
        stateName_ = FiniteStateMachine::getStateName(state_);
    }


    /**
     * Calls FiniteStateMachine::fireEvent() and keeps stateName_ and state_
     * in sync.
     */
    void fireEvent(toolbox::Event::Reference e) 
    throw (toolbox::fsm::exception::Exception)
    {
        FiniteStateMachine::fireEvent(e);

        state_     = FiniteStateMachine::getCurrentState();
        stateName_ = FiniteStateMachine::getStateName(state_);
    }
};


#endif
