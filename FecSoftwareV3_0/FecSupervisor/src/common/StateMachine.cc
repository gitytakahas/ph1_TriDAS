////////////////////////////////////////////////////////////////////////////////
//
// StateMachine
// ------------
//
//            03/06/2007 Philipp Schieferdecker <philipp.schieferdecker@cern.ch>
////////////////////////////////////////////////////////////////////////////////


#include "StateMachine.h"

#include "toolbox/fsm/FailedEvent.h"

#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/domutils.h"

#include "xcept/tools.h"

#include <typeinfo>
#include <string>
#include <sstream>


using namespace std;
using namespace StripTrackerFSM;


////////////////////////////////////////////////////////////////////////////////
// construction/destruction
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
StateMachine::StateMachine(xdaq::Application* app)
  : logger_(app->getApplicationLogger())
  , workLoopInitialising_(0)
  , workLoopConfiguring_(0)
  , workLoopEnabling_(0)
  , workLoopPausing_(0)
  , workLoopFixingSoftError_(0)
  , workLoopResuming_(0)
  , workLoopDisabling_(0)
  , workLoopHalting_(0)
  , workLoopDestroying_(0)
  , workLoopDcuConfiguring_(0)
  , asInitialising_(0)
  , asConfiguring_(0)
  , asPausing_(0)
  , asFixingSoftError_(0)
  , asResuming_(0)
  , asEnabling_(0)
  , asDisabling_(0)
  , asHalting_(0)
  , asDestroying_(0)
  , asDcuConfiguring_(0)
  , rcmsStateNotifier_(app->getApplicationLogger(),
		       app->getApplicationDescriptor(),
		       app->getApplicationContext())
{
  ostringstream oss;
  oss<<app->getApplicationDescriptor()->getClassName()
     <<app->getApplicationDescriptor()->getInstance();
  appNameAndInstance_ = oss.str();
}


//______________________________________________________________________________
StateMachine::~StateMachine()
{

}


////////////////////////////////////////////////////////////////////////////////
// implementation of member functions
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
xoap::MessageReference StateMachine::commandCallback(xoap::MessageReference msg)
  throw (xoap::exception::Exception)
{
  xoap::SOAPPart     part    =msg->getSOAPPart();
  xoap::SOAPEnvelope env     =part.getEnvelope();
  xoap::SOAPBody     body    =env.getBody();
  DOMNode           *node    =body.getDOMNode();
  DOMNodeList       *bodyList=node->getChildNodes();
  DOMNode           *command =0;
  string             commandName;
  
  for (unsigned int i=0;i<bodyList->getLength();i++) {
    command = bodyList->item(i);
    if(command->getNodeType() == DOMNode::ELEMENT_NODE) {
      commandName = xoap::XMLCh2String(command->getLocalName());
      break;
    }
  }
  
  if (commandName.empty()) {
    XCEPT_RAISE(xoap::exception::Exception,"Command not found.");
  }
  
  // fire appropriate event and create according response message
  try {
    toolbox::Event::Reference e(new toolbox::Event(commandName,this));
    fsm_.fireEvent(e);
    
    // response string
    xoap::MessageReference reply = xoap::createMessage();
    xoap::SOAPEnvelope envelope  = reply->getSOAPPart().getEnvelope();
    xoap::SOAPName responseName  = envelope.createName(commandName+"Response",
						       "xdaq",XDAQ_NS_URI);
    xoap::SOAPBodyElement responseElem =
      envelope.getBody().addBodyElement(responseName);
    
    // state string
    int               iState        = fsm_.getCurrentState();
    string            state         = fsm_.getStateName(iState);
    xoap::SOAPName    stateName     = envelope.createName("state",
							  "xdaq",XDAQ_NS_URI);
    xoap::SOAPElement stateElem     = responseElem.addChildElement(stateName);
    xoap::SOAPName    attributeName = envelope.createName("stateName",
							  "xdaq",XDAQ_NS_URI);
    stateElem.addAttribute(attributeName,state);
    
    return reply;
  }
  catch (toolbox::fsm::exception::Exception & e) {
    XCEPT_RETHROW(xoap::exception::Exception,"invalid command.",e);
  }	
}


//______________________________________________________________________________
void StateMachine::stateChanged(toolbox::fsm::FiniteStateMachine & fsm) 
  throw (toolbox::fsm::exception::Exception)
{

  // ?????????????????????????????????????????????????????????????????????????????????
  std::cout << "-------------------------> " << __PRETTY_FUNCTION__ << " BEGIN" << std::endl ;

  stateName_   = fsm_.getStateName(fsm_.getCurrentState());
  string state = stateName_.toString();
  
  LOG4CPLUS_INFO(logger_,"New state is: "<<state);

  // ?????????????????????????????????????????????????????????????????????????????????
  std::cout << "-------------------------> " << __PRETTY_FUNCTION__ << " state = " << state << std::endl ;
  
  if (state=="initialising") {
    try {

      // ?????????????????????????????????????????????????????????????????????????????????
      std::cout << "-------------------------> " << __PRETTY_FUNCTION__ << " " << state << " method called by workloop" << std::endl ;

      workLoopConfiguring_->submit(asInitialising_);
    }
    catch (xdaq::exception::Exception& e) {
      LOG4CPLUS_ERROR(logger_,xcept::stdformat_exception_history(e));
    }
  }
  else if (state=="configuring") {
    try {
      workLoopConfiguring_->submit(asConfiguring_);
    }
    catch (xdaq::exception::Exception& e) {
      LOG4CPLUS_ERROR(logger_,xcept::stdformat_exception_history(e));
    }
  }
  else if (state=="resuming") {
    try {
      workLoopResuming_->submit(asResuming_);
    }
    catch (xdaq::exception::Exception& e) {
      LOG4CPLUS_ERROR(logger_,xcept::stdformat_exception_history(e));
    }
  }
  else if (state=="fixingSoftError") {
    try {
      workLoopFixingSoftError_->submit(asFixingSoftError_);
    }
    catch (xdaq::exception::Exception& e) {
      LOG4CPLUS_ERROR(logger_,xcept::stdformat_exception_history(e));
    }
  }
  else if (state=="pausing") {
    try {
      workLoopPausing_->submit(asPausing_);
    }
    catch (xdaq::exception::Exception& e) {
      LOG4CPLUS_ERROR(logger_,xcept::stdformat_exception_history(e));
    }
  }
  else if (state=="enabling") {
    try {
      workLoopEnabling_->submit(asEnabling_);
    }
    catch (xdaq::exception::Exception& e) {
      LOG4CPLUS_ERROR(logger_,xcept::stdformat_exception_history(e));
    }
  }
  else if (state=="disabling") {
    try {
      workLoopDisabling_->submit(asDisabling_);
    }
    catch (xdaq::exception::Exception& e) {
      LOG4CPLUS_ERROR(logger_,xcept::stdformat_exception_history(e));
    }
  }
  else if (state=="halting") {
    try {
      workLoopHalting_->submit(asHalting_);
    }
    catch (xdaq::exception::Exception& e) {
      LOG4CPLUS_ERROR(logger_,xcept::stdformat_exception_history(e));
    }
  }
  else if (state=="destroying") {
    try {
      workLoopConfiguring_->submit(asDestroying_);
    }
    catch (xdaq::exception::Exception& e) {
      LOG4CPLUS_ERROR(logger_,xcept::stdformat_exception_history(e));
    }
  }
  else if (state=="dcuconfiguring") {
    try {
      workLoopDcuConfiguring_->submit(asDcuConfiguring_);
    }
    catch (xdaq::exception::Exception& e) {
      LOG4CPLUS_ERROR(logger_,xcept::stdformat_exception_history(e));
    }
  }
  else if (state=="Initial"||state=="Configured"||state=="Enabled"||state=="Disabled"||state=="Halted"||state=="DcuConfigured"||state=="Paused"||state=="Failed") {
    try {

      // ?????????????????????????????????????????????????????????????????????????????????
      std::cout << "-------------------------> " << __PRETTY_FUNCTION__ << " " << " Notify RCMS" << std::endl ;

      rcmsStateNotifier_.stateChanged(state,appNameAndInstance_+
				      " has reached target state " +
				      state);
    }
    catch (xcept::Exception& e) {

      // ?????????????????????????????????????????????????????????????????????????????????
      std::cout << "-------------------------> " << __PRETTY_FUNCTION__ << " " << " Notify RCMS failed: " << xcept::stdformat_exception_history(e) << std::endl; 

      LOG4CPLUS_ERROR(logger_,"Failed to notify state change: "
		      <<xcept::stdformat_exception_history(e));
    }
  }
}


//______________________________________________________________________________
void StateMachine::failed(toolbox::Event::Reference e)
  throw (toolbox::fsm::exception::Exception)
{
  if (typeid(*e) == typeid(toolbox::fsm::FailedEvent)) {
    toolbox::fsm::FailedEvent &fe=dynamic_cast<toolbox::fsm::FailedEvent&>(*e);
    LOG4CPLUS_FATAL(logger_,"Failure occurred in transition from '"
		    <<fe.getFromState()<<"' to '"<<fe.getToState()
		    <<"', exception history: "
		    <<xcept::stdformat_exception_history(fe.getException()));
  }
  else {
    toolbox::Event &fe=dynamic_cast<toolbox::Event&>(*e);
    LOG4CPLUS_FATAL(logger_,"fsm failure occured: "<<fe.type());
  }
}


//______________________________________________________________________________
void StateMachine::fireEvent(const string& evtType,void* originator)
{
  toolbox::Event::Reference e(new toolbox::Event(evtType,originator));
  fsm_.fireEvent(e);
}


//______________________________________________________________________________
void StateMachine::fireFailed(const string& errorMsg,void* originator)
{
  toolbox::Event::Reference e(new toolbox::Event(errorMsg,originator));
  fsm_.fireEvent(e);
}

