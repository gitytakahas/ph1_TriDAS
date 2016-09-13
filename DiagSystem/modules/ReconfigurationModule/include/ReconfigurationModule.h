/*
   FileName : 		ReconfigurationModule.h

   Content : 		ReconfigurationModule module

   Used in : 		Tracker Diagnostic System

   Programmer : 	Laurent GROSS

   Version : 		DiagSystem 4.0

   Date of last modification : 19/12/2006

   Support : 		mail to : laurent.gross@ires.in2p3.fr
   
   Online help : 	https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem
*/
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

Copyright 2005, Laurent GROSS - IReS/IN2P3
*/

#ifndef _ReconfigurationModule_h_
#define _ReconfigurationModule_h_

#ifndef _SOAPStateMachine_h_
#define _SOAPStateMachine_h_


#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"
#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"
#include "xoap/domutils.h" //for XMLCh2String


#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include "xdata/String.h"
#include "xdata/UnsignedLong.h"
#include "xdata/Boolean.h"

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>

#include <diagbag/DiagBagWizard.h>

#include "DiagCompileOptions.h"
/* needed for autoconfiguration */
#ifdef AUTO_CONFIGURE_PROCESSES
    #include "toolbox/task/Timer.h"
    #include "toolbox/task/TimerFactory.h"
    #include "toolbox/TimeInterval.h"
#endif

/* needed for FSM */
#include "tools/standalone/FsmWithStateName.h"
#include "toolbox/fsm/FailedEvent.h"

#define EXECTRANSHC "Configure"
#define EXECTRANSCH "Halt"


#include "ReconfigurationModuleV.h"

#include "toolbox/BSem.h"


//#ifdef AUTO_CONFIGURE_PROCESSES
    class ReconfigurationModule: public xdaq::Application, public toolbox::task::TimerListener
/*
#else
    class ReconfigurationModule: public xdaq::Application
#endif
*/
{
private:
	int internalState_;
//BSEMMOD
//	BSem reconfCmdMutex;
toolbox::BSem * reconfCmdMutex;

	std::string className__;
	std::string instance__;
	std::string errorCode__;
	std::string extraBuffer__;	
	std::string reconfAction__;

	toolbox::task::Timer * timer;
	toolbox::TimeVal start;
	bool timerInitStage;
	xoap::MessageReference cmd_msg;

	std::string classNameToUse__;
	std::string instanceToUse__;


public:
	FsmWithStateName fsm_;

	//DIAGREQUESTED
	DiagBagWizard * diagService_;

	//! define factory method for instantion of HelloWorld application
	XDAQ_INSTANTIATOR();

	~ReconfigurationModule()
	{
		delete reconfCmdMutex;
	}
	
	
	ReconfigurationModule(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): xdaq::Application(s)//BSEMMOD    , reconfCmdMutex(BSem::FULL)
	{

//BSEMMOD
		reconfCmdMutex = new toolbox::BSem(toolbox::BSem::FULL);

		//Give funny and useless informations at load time
		std::stringstream mmesg;
		mmesg << "Process version " << RECONFMODULE_PACKAGE_VERSION << " loaded";
		this->getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);
		LOG4CPLUS_INFO(this->getApplicationLogger(), mmesg.str());
		this->getApplicationLogger().setLogLevel(ERROR_LOG_LEVEL);

		internalState_ = 0;
		timerInitStage = true;
		
		//DIAGREQUESTED
		 diagService_ = new DiagBagWizard(
 			 (getApplicationDescriptor()->getClassName() + "DiagLvlOne") ,
 			 this->getApplicationLogger(),
			 getApplicationDescriptor()->getClassName(),
			 getApplicationDescriptor()->getInstance(),
			 getApplicationDescriptor()->getLocalId(),
			 (xdaq::WebApplication *)this,
			 "MYSYSTEM",
			 "MYSUBSYTSTEM"
			  );

		// A simple web control interface
		xgi::bind(this,&ReconfigurationModule::Default, "Default");
		xgi::bind(this,&ReconfigurationModule::Default1, "Default1");

		xgi::bind(this,&ReconfigurationModule::callDiagSystemPage, "callDiagSystemPage");
		xgi::bind(this,&ReconfigurationModule::callFsmPage, "callFsmPage");

		xgi::bind(this,&ReconfigurationModule::configureStateMachine, "configureStateMachine");
		xgi::bind(this,&ReconfigurationModule::stopStateMachine, "stopStateMachine");


		//DIAGREQUESTED
		xgi::bind(this,&ReconfigurationModule::configureDiagSystem, "configureDiagSystem");
		xgi::bind(this,&ReconfigurationModule::applyConfigureDiagSystem, "applyConfigureDiagSystem");
		xoap::bind(this, &ReconfigurationModule::freeLclDiagSemaphore, "freeLclDiagSemaphore", XDAQ_NS_URI );
		xoap::bind(this, &ReconfigurationModule::freeGlbDiagSemaphore, "freeGlbDiagSemaphore", XDAQ_NS_URI );

		//DIAGREQUESTED
		DIAG_DECLARE_EM_APP

		//RECONFREQUESTED
		xoap::bind(this, &ReconfigurationModule::reconfLog, "reconfLog", XDAQ_NS_URI );
  // ---------------------------------- State machine
  // Initiliase state
  /* Liste des etats qu'on va trouver dans la FSM */
  fsm_.addState ('C', "Configured") ;
  fsm_.addState ('H', "Halted");


  /* Methode initialiseAction qui doit gerer les constructions, etc... qui surviennent lors de la
  transition entre l'etat Initial et l'etat Halted */
  fsm_.addStateTransition ('C','H', EXECTRANSCH, this, &ReconfigurationModule::haltAction);
  fsm_.addStateTransition ('H','C', EXECTRANSHC, this, &ReconfigurationModule::configureAction);

  /* Si une transition merde, on appel la methode failedTransition */
  fsm_.setFailedStateTransitionAction( this, &ReconfigurationModule::failedTransition );

  /* On synchronise la FASM avec l'etat du process apres load, a savoir : etat HALTED */
  fsm_.setInitialState('H');
  fsm_.reset();

  //Export the stateName variable
  getApplicationInfoSpace()->fireItemAvailable("stateName", &fsm_.stateName_);

  // Bind SOAP callbacks for control messages
  xoap::bind (this, &ReconfigurationModule::fireEvent, EXECTRANSCH, XDAQ_NS_URI);
  xoap::bind (this, &ReconfigurationModule::fireEvent, EXECTRANSHC, XDAQ_NS_URI);

            #ifdef AUTO_CONFIGURE_PROCESSES
                std::stringstream timerName;
                timerName << getApplicationDescriptor()->getContextDescriptor()->getURL() << ":";
                timerName << getApplicationDescriptor()->getClassName() << ":" << getApplicationDescriptor()->getLocalId() << ":" << getApplicationDescriptor()->getInstance();
//                toolbox::task::Timer * timer = toolbox::task::getTimerFactory()->createTimer(timerName.str());
                timer = toolbox::task::getTimerFactory()->createTimer(timerName.str());
                toolbox::TimeInterval interval(AUTO_UP_CONFIGURE_DELAY,0);
//                toolbox::TimeVal start;
                start = toolbox::TimeVal::gettimeofday() + interval;
                timer->schedule( this, start,  0, "" );
            #endif

	}




	void getDefault1Content(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		*out << "<br>No default content for this module<br>";
	}


	void getFsmStatePage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{

		if (fsm_.getCurrentState() == 'H') *out << "Current State is : Halted" << std::endl;
		if (fsm_.getCurrentState() == 'C') *out << "Current State is : Configured" << std::endl;
		if ( (fsm_.getCurrentState() != 'C') && (fsm_.getCurrentState() != 'H') ) *out << "Current State is : DANS LES CHOUX" << std::endl;
		*out << "<br>";


		std::string urlConfig = "/";
		urlConfig += getApplicationDescriptor()->getURN();
		urlConfig += "/configureStateMachine";	
		*out << cgicc::form().set("method","post").set("action", urlConfig).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Configure State Machine");
		*out << "<br>";
		*out << cgicc::form() << std::endl;


		std::string urlStop = "/";
		urlStop += getApplicationDescriptor()->getURN();
		urlStop += "/stopStateMachine";	
		*out << cgicc::form().set("method","post").set("action", urlStop).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Stop State Machine");
		*out << "<br>";
		*out << cgicc::form() << std::endl;
	}

	
	void Default1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		internalState_ = 0;
		Default(in, out);
	}

	void callDiagSystemPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		 internalState_ = 1;
		Default(in, out);
	}


	void callFsmPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		 internalState_ = 2;
		Default(in, out);
	}



void displayLinks(xgi::Input * in, xgi::Output * out)
{

	std::string urlBase_ = "/"; \
	urlBase_ += getApplicationDescriptor()->getURN(); \
	urlBase_ += "/Default1"; \
	*out << "<a href=" << urlBase_ << ">Default Page</a>";

	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";

	std::string urlDiag_ = "/"; \
	urlDiag_ += getApplicationDescriptor()->getURN(); \
	urlDiag_ += "/callDiagSystemPage"; \
	*out << "<a href=" << urlDiag_ << ">Configure DiagSystem</a>";

	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";

	std::string urlFsm_ = "/"; \
	urlFsm_ += getApplicationDescriptor()->getURN(); \
	urlFsm_ += "/callFsmPage"; \
	*out << "<a href=" << urlFsm_ << ">FSM Access</a>";

	*out << "<br>";
}


	void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{

		*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
		*out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
		*out << cgicc::title("Reconfiguration Module") << std::endl;

		xgi::Utils::getPageHeader
			(out, 
			"Reconfiguration Module", 
			getApplicationDescriptor()->getContextDescriptor()->getURL(),
			getApplicationDescriptor()->getURN(),
			"/daq/xgi/images/Application.jpg"
			);

		displayLinks(in, out);

		if (internalState_ == 0) getDefault1Content(in, out);
		if (internalState_ == 1) diagService_->getDiagSystemHtmlPage(in, out,getApplicationDescriptor()->getURN());
		if (internalState_ == 2) getFsmStatePage(in, out);
	
		xgi::Utils::getPageFooter(*out);
	}




	//DIAGREQUESTED
	void DIAG_CONFIGURE_CALLBACK();
	void DIAG_APPLY_CALLBACK();
  	DIAG_FREELCLSEM_CALLBACK();
    DIAG_FREEGLBSEM_CALLBACK();
	DIAG_REQUEST_ENTRYPOINT();


	xoap::MessageReference reconfLog(xoap::MessageReference msg) throw (xoap::exception::Exception)
	{

//BSEMMOD
		reconfCmdMutex->take();

		// prepare reply to caller
		xoap::MessageReference reply = xoap::createMessage();
		xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
		xoap::SOAPName responseName = envelope.createName( "reconfLogResponse", "xdaq", XDAQ_NS_URI);
		envelope.getBody().addBodyElement ( responseName );


		//Get incoming message elements
		xoap::SOAPBody rb = msg->getSOAPPart().getEnvelope().getBody();
		if (rb.hasFault() )
		{
			// MUST raise an exception here
		}
		else
		{
			std::vector<xoap::SOAPElement> reconfElement = rb.getChildElements ();
			if(reconfElement.size()==1)
			{
				xoap::SOAPName className ("className", "", "");
				className__ = reconfElement[0].getAttributeValue(className);

				xoap::SOAPName instance ("instance", "", "");
				instance__ = reconfElement[0].getAttributeValue(instance);

				xoap::SOAPName errorCode ("errorCode", "", "");
				errorCode__ = reconfElement[0].getAttributeValue(errorCode);

				xoap::SOAPName extraBuffer ("extraBuffer", "", "");
				extraBuffer__ = reconfElement[0].getAttributeValue(extraBuffer);
			}
		}
		std::string execMsg = (std::string)"Received a reconfiguration request from class " + className__ + (std::string)" instance " + instance__ + (std::string)" based on error code " + errorCode__;
		if (extraBuffer__ != DIAG_DEFAULT_EBUFF) execMsg = execMsg + (std::string)"  <>  Attached ExtraBuffer content is : " + extraBuffer__;
		diagService_->reportError(execMsg, DIAGUSERINFO);


		/* Conditions testing here ; more or less the IA part in order to know
		which answer/action is the most appropriate to the situation.
		Once this defined, the action to take encoded as a string into reconfAction__
		is sent to the process which needs to drive the system recovery task. */
		reconfAction__ = defineRecoveryActionToTake();



		/* If necessary, Send appropriate command to destination */
		if (reconfAction__ != "DO_NOTHING")
		{
		    /* prepare SOAP for sending action */
//	    	xoap::MessageReference cmd_msg = xoap::createMessage();
			cmd_msg = xoap::createMessage();

		    xoap::SOAPPart cmd_soap = cmd_msg->getSOAPPart();
	    	xoap::SOAPEnvelope cmd_envelope = cmd_soap.getEnvelope();

		    xoap::SOAPName command = cmd_envelope.createName("recoveryEntry", "xdaq", XDAQ_NS_URI);
    		xoap::SOAPBody cmd_body = cmd_envelope.getBody();
		    cmd_body.addBodyElement(command);

		    /* Add requested attributes to SOAP part */
	    	xoap::SOAPBody cmdb = cmd_msg->getSOAPPart().getEnvelope().getBody();
	    	std::vector<xoap::SOAPElement> cmdElement = cmdb.getChildElements ();

			xoap::SOAPName reconfAction ("reconfAction", "", "");
			cmdElement[0].addAttribute(reconfAction,reconfAction__);

			if ( (className__ != "ErrorDispatcher") && (className__ != "GlobalErrorDispatcher") )
			{
				std::string comdMsg = (std::string)"Asking to process " + className__ + (std::string)" instance " + (std::string)instance__ + (std::string)" to fire recovery action : " + reconfAction__;
				diagService_->reportError(comdMsg, DIAGUSERINFO);
				try
				{	
					#ifdef XDAQ373
						xdaq::ApplicationDescriptor * err_d = getApplicationContext()->getDefaultZone()->getApplicationDescriptor(className__, atoi(instance__.c_str()));
					#else
						xdaq::ApplicationDescriptor * err_d = getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(className__, atoi(instance__.c_str()));
					#endif
					getApplicationContext()->postSOAP(cmd_msg, *getApplicationDescriptor(), *err_d);
				}
				catch (xdaq::exception::Exception& e) {}
//BSEMMOD
				reconfCmdMutex->give();
			}
			else
			{
				timerInitStage = false;
                toolbox::TimeInterval interval(0,100);
                start = toolbox::TimeVal::gettimeofday() + interval;
                timer->schedule( this, start,  0, "" );
			}
		}
		else
		{
			diagService_->reportError("No recovery action found for the analysed set of parameters ; exiting recovery loop.", DIAGUSERINFO);
//BSEMMOD
			reconfCmdMutex->give();
		}
	    return reply;
	}





  /** \brief initiliase all the paremters for the FecSupervisor
   */
void configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{

	//Apply fireitems value to internals and check validity
	DIAG_EXEC_FSM_INIT_TRANS

}

void haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
	//Apply fireitems value to internals and check validity
	DIAG_EXEC_FSM_STOP_TRANS

}
   
   
   
xoap::MessageReference fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception)
{
std::cout << "fireEvent" << std::endl ;

  xoap::SOAPPart part = msg->getSOAPPart();
  xoap::SOAPEnvelope env = part.getEnvelope();
  xoap::SOAPBody body = env.getBody();
  DOMNode* node = body.getDOMNode();
  DOMNodeList* bodyList = node->getChildNodes();
  for (unsigned int i = 0; i < bodyList->getLength(); i++)  {

    DOMNode* command = bodyList->item(i);

    if (command->getNodeType() == DOMNode::ELEMENT_NODE) {

      std::string commandName = xoap::XMLCh2String (command->getLocalName());
                    if ( (fsm_.getCurrentState() == 'H') && (commandName == EXECTRANSHC) || (fsm_.getCurrentState() == 'C') && (commandName == EXECTRANSCH) )
                    {

      try {
	toolbox::Event::Reference e(new toolbox::Event(commandName, this));
	fsm_.fireEvent(e);
	// Synchronize Web state machine
	//	      wsm_.setInitialState(fsm_.getCurrentState());
      }
      catch (toolbox::fsm::exception::Exception & e) {
	XCEPT_RETHROW(xcept::Exception, "invalid command", e);
	diagService_->reportError (e.what(), DIAGERROR) ;
      }

      xoap::MessageReference reply = xoap::createMessage();
      xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
      xoap::SOAPName responseName = envelope.createName( commandName +"Response", "xdaq", XDAQ_NS_URI);
      envelope.getBody().addBodyElement ( responseName );
      return reply;
                }
    }
  }

  XCEPT_RAISE(xcept::Exception,"command not found");	
  diagService_->reportError ("Command not found", DIAGERROR) ;

}



void failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
  toolbox::fsm::FailedEvent & fe = dynamic_cast<toolbox::fsm::FailedEvent&>(*e);
  std::ostringstream msgError ;
  msgError << "Failure occurred when performing transition from: " << fe.getFromState() << " to: " << fe.getToState() << " exception: " << fe.getException().what() ;
  diagService_->reportError (msgError.str(), DIAGERROR) ;
}

void configureStateMachine(xgi::Input * in, xgi::Output * out)
{
            if (fsm_.getCurrentState() == 'H')
            {

	toolbox::Event::Reference e(new toolbox::Event(EXECTRANSHC, this));
	fsm_.fireEvent(e);
            }
	Default(in, out);
}

void stopStateMachine(xgi::Input * in, xgi::Output * out)
{
            if (fsm_.getCurrentState() == 'C')
            {

	toolbox::Event::Reference e(new toolbox::Event(EXECTRANSCH, this));
	fsm_.fireEvent(e);
            }
	Default(in, out);
}


//        #ifdef AUTO_CONFIGURE_PROCESSES
            void timeExpired (toolbox::task::TimerEvent& e) 
            {
				if (timerInitStage == true)
				{
	                DIAG_EXEC_FSM_INIT_TRANS
					std::cout << "RECONFIGURATION_MODULE ready" << std::endl;
				}
				else
				{
					std::string comdMsg = (std::string)"Asking to process " + classNameToUse__ + (std::string)" instance " + instanceToUse__ + (std::string)" to fire recovery action : " + reconfAction__;
					diagService_->reportError(comdMsg, DIAGUSERINFO);
					try
					{	
						#ifdef XDAQ373
							xdaq::ApplicationDescriptor * err_d = getApplicationContext()->getDefaultZone()->getApplicationDescriptor(classNameToUse__, atoi(instanceToUse__.c_str()));
						#else
							xdaq::ApplicationDescriptor * err_d = getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(classNameToUse__, atoi(instanceToUse__.c_str()));
						#endif
						getApplicationContext()->postSOAP(cmd_msg, *getApplicationDescriptor(), *err_d);
					}
					catch (xdaq::exception::Exception& e) {}
					/* rendre le mutex */
//BSEMMOD
					reconfCmdMutex->give();
					
					/* de-freeze l'analyseur origine du log */
			    	xoap::MessageReference unfreeze_msg = xoap::createMessage();

				    xoap::SOAPPart cmd_soap = unfreeze_msg->getSOAPPart();
			    	xoap::SOAPEnvelope cmd_envelope = cmd_soap.getEnvelope();

				    xoap::SOAPName command = cmd_envelope.createName("freeReconfCmd", "xdaq", XDAQ_NS_URI);
    				xoap::SOAPBody cmd_body = cmd_envelope.getBody();
				    cmd_body.addBodyElement(command);

					try
					{	
						#ifdef XDAQ373
							xdaq::ApplicationDescriptor * err_d = getApplicationContext()->getDefaultZone()->getApplicationDescriptor(className__, atoi(instance__.c_str()));
						#else
							xdaq::ApplicationDescriptor * err_d = getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(className__, atoi(instance__.c_str()));
						#endif
						getApplicationContext()->postSOAP(unfreeze_msg, *getApplicationDescriptor(), *err_d);
					}
					catch (xdaq::exception::Exception& e) {}
				}
            }
//        #endif


std::string defineRecoveryActionToTake()
{
	/* If the error is not issued from an ErrorDispatcher or a GlobalErrorDispatcher but directly emitted by a specific user
	process, we will by default reply to the process who made this direct reconfiguration call - process(className__, instance__).
	Next two blocks gives an example for a direct reconfiguration command issued from DummySupervisor(1) which will fire a
	reconfiguration action in this same module, i.e. DummySupervisor(1) */
	if ( (className__ == "DummySupervisor") && (atol(instance__.c_str()) == 1 ) && (atoi(errorCode__.c_str()) == 100) )
	{
	    return "RECONFIGURE DUMMY SUPERVISOR(1) FOR ERROR 100 RAISED BY DUMMY SUPERVISOR(1)";
	}

	if ( (className__ == "DummySupervisor") && (atol(instance__.c_str()) == 1 ) && (atoi(errorCode__.c_str()) == 101) )
	{
	    return "RECONFIGURE DUMMY SUPERVISOR(1) FOR ERROR 101 RAISED BY DUMMY SUPERVISOR(1)";
	}


	/* If the error is not issued from an ErrorDispatcher or a GlobalErrorDispatcher but directly emitted by a specific user
	process, AND if we know that for THIS reconfiguration request emitted by THIS process we have to target another user process
	as the reconfiguration command destination, then specify explicitely the destination target.

	Next block gives an example for a direct reconfiguration command issued from DummySupervisor(0) which must fire a
	reconfiguration action in another user process. The rule we apply here is: 
	If we have an Error number 102 emitted by the process DummySupervisor(0), then request the process DummySupervisor(1) 
	to make a recovery action*/
	if ( (className__ == "DummySupervisor") && (atol(instance__.c_str()) == 0 ) && (atoi(errorCode__.c_str()) == 102) )
	{
    	    classNameToUse__ = "DummySupervisor";
    	    instanceToUse__ = "1";
	    return "RECONFIGURE DUMMY SUPERVISOR(1) FOR ERROR 102 RAISED BY DUMMY SUPERVISOR(0)";
	    
	}


    	/* If the reconfiguration command is sent by the GlobalErrorDispatcher, after a first level of analysis
	done in this same GlobalErrorDispatcher, we must explicitely specify which module will be the
	destination for the reconfiguration command. */
	if ( (className__ == "GlobalErrorDispatcher") && (atoi(errorCode__.c_str()) == 200) )
	{
		/* We know that for this composite error, we will have to act on process X instance Y */
		classNameToUse__ = "DummySupervisor";
		instanceToUse__ = "0";
		return "RECONFIGURE DUMMY SUPERVISOR(0) FOR ERROR 200 RAISED BY GLOBAL ERROR DISPATCHER";
	}


    	/* If the reconfiguration command is sent by one local ErrorDispatcher, after a first level of analysis
	done in this same local ErrorDispatcher, we must explicitely specify which module will be the
	destination for the reconfiguration command. */
	if ( (className__ == "ErrorDispatcher") && (atol(instance__.c_str()) == 0 ) && (atoi(errorCode__.c_str()) == 201) )
	{
		/* We know that for this composite error, we will have to act on process X instance Y */
		classNameToUse__ = "DummySupervisor";
		instanceToUse__ = "0";
		return "RECONFIGURE DUMMY SUPERVISOR(0) FOR ERROR 201 RAISED BY LOCAL ERROR DISPATCHER(0)";
	}
	/* let's test if it works ; no recovery action should be taken with an error code 104 emitted from a DummySupeervisor process */
	/*
	if ( (className__ == "DummySupervisor") && (atol(instance__.c_str()) >= 0 ) && (atoi(errorCode__.c_str()) == 103) ) return "RECONFIGURE DUMMY SUPERVISOR FOR ERROR 103";
	*/
return "DO_NOTHING";	
}


};


#endif
#endif
