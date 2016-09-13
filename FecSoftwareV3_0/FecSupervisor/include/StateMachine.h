/*
  This file is part of Fec Software project.
  
  Fec Software is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  Fec Software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with Fec Software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  
  Copyright 2002 - 2003, P. Schieferdecker - CERN
  Changed by F. Drouhin to add more state, namespace is changed to avoid confusion
  with the other StateMachine
*/
#ifndef STRIPTRACKERFSM_STATEMACHINE_H
#define STRIPTRACKERFSM_STATEMACHINE_H


#include "xdaq/Application.h"
#include "xdaq/NamespaceURI.h"

#include "toolbox/fsm/FiniteStateMachine.h"
#include "toolbox/task/WorkLoopFactory.h"
#include "toolbox/task/Action.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/Method.h"

#include "xdata/String.h"

#include "xdaq2rc/RcmsStateNotifier.h"

#include <string>

namespace StripTrackerFSM
{

  typedef char State;
  
  class StateMachine : public toolbox::lang::Class
  {
  public:
    //
    // construction / destruction
    //
    StateMachine(xdaq::Application* app);
    virtual ~StateMachine();
    
    
    //
    // member functions 
    //
    
    // finite state machine command callback
    xoap::MessageReference commandCallback(xoap::MessageReference msg)
      throw (xoap::exception::Exception);
    
    // finite state machine callback for entering new state
    void stateChanged(toolbox::fsm::FiniteStateMachine & fsm) 
      throw (toolbox::fsm::exception::Exception);
    
    // finite state machine callback for transition into 'Failed' state
    void failed(toolbox::Event::Reference e)
      throw(toolbox::fsm::exception::Exception);
    
    // fire state transition event
    void fireEvent(const std::string& evtType,void* originator);
    
    // initiate transition to state 'Failed'
    void fireFailed(const std::string& errorMsg,void* originator);
    
    // report current state
    xdata::String* stateName() { return &stateName_; }
    
    // lookup the RCMS state listener
    void findRcmsStateListener() { rcmsStateNotifier_.findRcmsStateListener(); }

    // report if RCMS StateListener was found
    xdata::Boolean* foundRcmsStateListener()
    {
      return rcmsStateNotifier_.getFoundRcmsStateListenerParameter();
    }

    // initialize state machine and bind callbacks to driving application
    template<class T> void initialize(T *app) 
    {
      // action signatures
      asInitialising_   = toolbox::task::bind(app,&T::initialising,  "initialising");
      asConfiguring_    = toolbox::task::bind(app,&T::configuring,   "configuring");
      asEnabling_       = toolbox::task::bind(app,&T::enabling,      "enabling");
      asDisabling_      = toolbox::task::bind(app,&T::disabling,     "disabling");
      asPausing_       = toolbox::task::bind(app,&T::pausing,      "pausing");

      asFixingSoftError_ = toolbox::task::bind(app,&T::fixingSoftError, "fixingSoftError");
      asResuming_       = toolbox::task::bind(app,&T::resuming,      "resuming");
      asHalting_        = toolbox::task::bind(app,&T::halting,       "halting");
      asDestroying_     = toolbox::task::bind(app,&T::destroying,    "destroying");
      asDcuConfiguring_ = toolbox::task::bind(app,&T::dcuconfiguring,"dcuconfiguring");
      
      // work loops
      workLoopInitialising_ =
	toolbox::task::getWorkLoopFactory()->getWorkLoop(appNameAndInstance_+
							 "_Initialising",
							 "waiting");
      workLoopConfiguring_ =
	toolbox::task::getWorkLoopFactory()->getWorkLoop(appNameAndInstance_+
							 "_Configuring",
							 "waiting");
      workLoopEnabling_ =
	toolbox::task::getWorkLoopFactory()->getWorkLoop(appNameAndInstance_+
							 "_Enabling",
							 "waiting");
      workLoopPausing_ =
	toolbox::task::getWorkLoopFactory()->getWorkLoop(appNameAndInstance_+
							 "_Pausing",
							 "waiting");

      workLoopFixingSoftError_ =
	toolbox::task::getWorkLoopFactory()->getWorkLoop(appNameAndInstance_+
							 "_FixingSoftError",
							 "waiting");
      workLoopResuming_ =
	toolbox::task::getWorkLoopFactory()->getWorkLoop(appNameAndInstance_+
							 "_Resuming",
							 "waiting");

      workLoopDisabling_ =
	toolbox::task::getWorkLoopFactory()->getWorkLoop(appNameAndInstance_+
							 "_Disabling",
							 "waiting");
      workLoopHalting_ =
	toolbox::task::getWorkLoopFactory()->getWorkLoop(appNameAndInstance_+
							 "_Halting",
							 "waiting");
      workLoopDestroying_ =
	toolbox::task::getWorkLoopFactory()->getWorkLoop(appNameAndInstance_+
							 "_Destroying",
							 "waiting");
      workLoopDcuConfiguring_ =
	toolbox::task::getWorkLoopFactory()->getWorkLoop(appNameAndInstance_+
							 "_DcuConfiguing",
							 "waiting");

      // bind SOAP callbacks
      xoap::bind(app,&T::fsmCallback,"Initialise",   XDAQ_NS_URI);
      xoap::bind(app,&T::fsmCallback,"Configure",    XDAQ_NS_URI);
      xoap::bind(app,&T::fsmCallback,"ConfigureDcu", XDAQ_NS_URI);
      xoap::bind(app,&T::fsmCallback,"Enable",       XDAQ_NS_URI);
      xoap::bind(app,&T::fsmCallback,"Pause",       XDAQ_NS_URI);
      xoap::bind(app,&T::fsmCallback,"FixSoftError", XDAQ_NS_URI);
      xoap::bind(app,&T::fsmCallback,"Resume",       XDAQ_NS_URI);
      xoap::bind(app,&T::fsmCallback,"Disable",      XDAQ_NS_URI);
      xoap::bind(app,&T::fsmCallback,"Halt",         XDAQ_NS_URI);
      xoap::bind(app,&T::fsmCallback,"Destroy",      XDAQ_NS_URI);

      // define finite state machine
      fsm_.addState ('I', "Initial"       ,this,&StripTrackerFSM::StateMachine::stateChanged);
      fsm_.addState ('i', "initialising"  ,this,&StripTrackerFSM::StateMachine::stateChanged) ;
      fsm_.addState ('C', "Configured"    ,this,&StripTrackerFSM::StateMachine::stateChanged);
      fsm_.addState ('c', "configuring"   ,this,&StripTrackerFSM::StateMachine::stateChanged);
      fsm_.addState ('E', "Enabled"       ,this,&StripTrackerFSM::StateMachine::stateChanged);
      fsm_.addState ('e', "enabling"      ,this,&StripTrackerFSM::StateMachine::stateChanged);
      fsm_.addState ('D', "Disabled"      ,this,&StripTrackerFSM::StateMachine::stateChanged);
      fsm_.addState ('d', "disabling"     ,this,&StripTrackerFSM::StateMachine::stateChanged);
      fsm_.addState ('H', "Halted"        ,this,&StripTrackerFSM::StateMachine::stateChanged);
      fsm_.addState ('h', "halting"       ,this,&StripTrackerFSM::StateMachine::stateChanged);
      fsm_.addState ('P', "Paused"        ,this,&StripTrackerFSM::StateMachine::stateChanged);
      fsm_.addState ('p', "pausing"        ,this,&StripTrackerFSM::StateMachine::stateChanged);
      fsm_.addState ('f', "fixingSoftError" ,this,&StripTrackerFSM::StateMachine::stateChanged);
      fsm_.addState ('r', "resuming"        ,this,&StripTrackerFSM::StateMachine::stateChanged);
      fsm_.addState ('x', "destroying"    ,this,&StripTrackerFSM::StateMachine::stateChanged);
      fsm_.addState ('U', "DCUConfigured" ,this,&StripTrackerFSM::StateMachine::stateChanged);
      fsm_.addState ('u', "dcuconfiguring",this,&StripTrackerFSM::StateMachine::stateChanged);


      // define the transition
      fsm_.addStateTransition('I','i',"Initialise");
      fsm_.addStateTransition('i','H',"InitialiseDone");

      fsm_.addStateTransition('H','c',"Configure");
      fsm_.addStateTransition('c','C',"ConfigureDone");
      fsm_.addStateTransition('C','h',"Halt");

      fsm_.addStateTransition('C','e',"Enable");
      fsm_.addStateTransition('e','E',"EnableDone");
      fsm_.addStateTransition('E','h',"Halt");

      fsm_.addStateTransition('E','d',"Disable");


      //fsm_.addStateTransition('d','D',"DisableDone"); // remove to go to Configured when Stop is applied
      fsm_.addStateTransition('d','C',"DisableDone"); 
      //fsm_.addStateTransition('D','h',"Halt"); // remove to go to Configured when Stop is applied
      fsm_.addStateTransition('E','f',"FixSoftError");
      fsm_.addStateTransition('f','E',"FixSoftErrorDone");

      fsm_.addStateTransition('E','p',"Pause");
      fsm_.addStateTransition('p','P',"PauseDone");

      fsm_.addStateTransition('P','r',"Resume");
      fsm_.addStateTransition('r','E',"ResumeDone");
      fsm_.addStateTransition('P','d',"Disable");
      fsm_.addStateTransition('P','h',"Halt");

      fsm_.addStateTransition('H','u',"ConfigureDcu");
      fsm_.addStateTransition('u','U',"ConfigureDcuDone");
      fsm_.addStateTransition('U','h',"Halt");

      fsm_.addStateTransition('h','H',"HaltDone");

      fsm_.addStateTransition('H','x',"Destroy");
      fsm_.addStateTransition('x','I',"DestroyDone");
      
      fsm_.addStateTransition('i','F',"Fail",this,&StripTrackerFSM::StateMachine::failed);
      fsm_.addStateTransition('f','F',"Fail",this,&StripTrackerFSM::StateMachine::failed);
      fsm_.addStateTransition('c','F',"Fail",this,&StripTrackerFSM::StateMachine::failed);
      fsm_.addStateTransition('e','F',"Fail",this,&StripTrackerFSM::StateMachine::failed);
      fsm_.addStateTransition('d','F',"Fail",this,&StripTrackerFSM::StateMachine::failed);
      fsm_.addStateTransition('h','F',"Fail",this,&StripTrackerFSM::StateMachine::failed);
      fsm_.addStateTransition('F','F',"Fail",this,&StripTrackerFSM::StateMachine::failed);

      fsm_.setFailedStateTransitionAction(this,&StripTrackerFSM::StateMachine::failed);
      fsm_.setFailedStateTransitionChanged(this,&StripTrackerFSM::StateMachine::stateChanged);
      fsm_.setStateName('F',"Failed");

      fsm_.setInitialState('I');      
      fsm_.reset();
      stateName_ = fsm_.getStateName(fsm_.getCurrentState());
      
      if (!workLoopInitialising_->isActive()) workLoopInitialising_->activate(); 
      if (!workLoopConfiguring_->isActive()) workLoopConfiguring_->activate();
      if (!workLoopPausing_->isActive()) workLoopPausing_->activate();
      if (!workLoopFixingSoftError_->isActive()) workLoopFixingSoftError_->activate();
      if (!workLoopResuming_->isActive()) workLoopResuming_->activate();   
      if (!workLoopEnabling_->isActive()) workLoopEnabling_->activate(); 
      if (!workLoopDisabling_->isActive()) workLoopDisabling_->activate(); 
      if (!workLoopHalting_->isActive()) workLoopHalting_->activate(); 
      if (!workLoopDestroying_->isActive()) workLoopDestroying_->activate(); 
      if (!workLoopDcuConfiguring_->isActive()) workLoopDcuConfiguring_->activate(); 
      
      findRcmsStateListener();

      //Export the stateName variable
      app->getApplicationInfoSpace()->fireItemAvailable("stateName", &stateName_);
    }

    /** Get state name
     */
    std::string getStateName(State s) throw (toolbox::fsm::exception::Exception) {
      return fsm_.getStateName(s) ;
    }

    /** Set a state name
     */
    void setStateName(State s, const std::string & name) throw (toolbox::fsm::exception::Exception) {

      fsm_.setStateName(s,name) ;
    }
	
    /** Get all states
     */
    std::vector<State> getStates() {
      return fsm_.getStates() ;
    }
		
    /** Return input for a given state
     */
    std::set<std::string> getInputs(State s) {

      return fsm_.getInputs(s) ;
    }
	
    /** Retun all inputs
     */
    std::set<std::string> getInputs() {
      return fsm_.getInputs() ;
    }
	
    /** return the possible transitions for all inputs from a given state
     */
    std::map<std::string, State, std::less<std::string> > getTransitions(State s) throw (toolbox::fsm::exception::Exception) {

      return fsm_.getTransitions(s) ;
    }
	
    /** Get the current state
     */
    State getCurrentState() {

      return fsm_.getCurrentState() ;
    }
     
    /** Get the finite state machine
     */
    toolbox::fsm::FiniteStateMachine getXDAQFSM ( ) {
      return fsm_ ;
    }

  private:
    //
    // member data
    //
    
    // application name&instance
    log4cplus::Logger                logger_;
    std::string                      appNameAndInstance_;
    xdata::String                    stateName_;
    
    // finite state machine
    toolbox::fsm::FiniteStateMachine fsm_;
    
    // work loops for transitional states
    toolbox::task::WorkLoop         *workLoopInitialising_;
    toolbox::task::WorkLoop         *workLoopConfiguring_;
    toolbox::task::WorkLoop         *workLoopEnabling_;
    toolbox::task::WorkLoop         *workLoopPausing_;
    toolbox::task::WorkLoop         *workLoopFixingSoftError_;
    toolbox::task::WorkLoop         *workLoopResuming_;
    toolbox::task::WorkLoop         *workLoopDisabling_;
    toolbox::task::WorkLoop         *workLoopHalting_;
    toolbox::task::WorkLoop         *workLoopDestroying_;
    toolbox::task::WorkLoop         *workLoopDcuConfiguring_;

    // action signatures for transitional states
    toolbox::task::ActionSignature  *asInitialising_ ;
    toolbox::task::ActionSignature  *asConfiguring_;
    toolbox::task::ActionSignature  *asEnabling_;
    toolbox::task::ActionSignature  *asPausing_;
    toolbox::task::ActionSignature  *asFixingSoftError_;
    toolbox::task::ActionSignature  *asResuming_;
    toolbox::task::ActionSignature  *asDisabling_;
    toolbox::task::ActionSignature  *asHalting_;
    toolbox::task::ActionSignature  *asDestroying_;
    toolbox::task::ActionSignature  *asDcuConfiguring_;
    
    // rcms state notifier
    xdaq2rc::RcmsStateNotifier       rcmsStateNotifier_;
    
  };
  
}


#endif
