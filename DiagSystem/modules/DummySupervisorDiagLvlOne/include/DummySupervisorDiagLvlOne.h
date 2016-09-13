/*
   FileName : 		DummySupervisorDiagLvlOne.h

   Content : 		DummySupervisorDiagLvlOne module

   Used in : 		Tracker Diagnostic System

   Programmer : 	Laurent GROSS

   Version : 		DiagSystem 2.1

   Date of last modification : 20/06/2006

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

#ifndef _DummySupervisorDiagLvlOne_h_
#define _DummySupervisorDiagLvlOne_h_


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
#include "xoap/SOAPElement.h"
#include "xoap/Method.h"
#include "xoap/domutils.h"


#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include "xdata/String.h"
#include "xdata/UnsignedLong.h"
#include "xdata/Integer.h"


#include <diagbag/DiagBagWizard.h>


#include "DiagCompileOptions.h"
/* needed for autoconfiguration */
#ifdef AUTO_CONFIGURE_PROCESSES
    #include "toolbox/task/Timer.h"
    #include "toolbox/task/TimerFactory.h"
    #include "toolbox/TimeInterval.h"
#endif


//FSMADDED
#ifdef WILL_COMPILE_FOR_PIXELS
	#include "toolbox/fsm/FsmWithStateName.h"
#else
	#include "FsmWithStateName.h"
#endif
#include "toolbox/fsm/FailedEvent.h"
#define EXECTRANSHC "Configure"
#define EXECTRANSCH "Halt"

#include "DummySupervisorDiagLvlOneV.h"

#ifdef AUTO_CONFIGURE_PROCESSES
    class DummySupervisorDiagLvlOne: public xdaq::Application, public toolbox::task::TimerListener
#else
    class DummySupervisorDiagLvlOne: public xdaq::Application
#endif
{
	
public:
	//DIAGREQUESTED
  FsmWithStateName fsm_;
  int internalState_;

	//DIAGREQUESTED
	DiagBagWizard * diagService_;

	//! define factory method for instantion of application
	XDAQ_INSTANTIATOR();
	
	DummySupervisorDiagLvlOne(xdaq::ApplicationStub * s): xdaq::Application(s) 
	{	

		//Give funny and useless informations at load time
		std::stringstream mmesg;
		mmesg << "Process version " << DUMMYSUPERVISORDIAGLVLONE_PACKAGE_VERSION << " loaded";
		this->getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);
		LOG4CPLUS_INFO(this->getApplicationLogger(), mmesg.str());
		this->getApplicationLogger().setLogLevel(ERROR_LOG_LEVEL);


		internalState_ = 0;

		//DIAGREQUESTED
		 diagService_ = new DiagBagWizard(
		 	(getApplicationDescriptor()->getClassName() + "NONE") ,
 			 this->getApplicationLogger(),
			 getApplicationDescriptor()->getClassName(),
			 getApplicationDescriptor()->getInstance(),
			 getApplicationDescriptor()->getLocalId(),
			 (xdaq::WebApplication *)this
			  );

		// Bind SOAP callback
		xoap::bind(this, &DummySupervisorDiagLvlOne::onIncomingErr, "onIncomingErr", XDAQ_NS_URI );
		
		//Bind to default Web ITF
		xgi::bind(this,&DummySupervisorDiagLvlOne::Default, "Default");
		xgi::bind(this,&DummySupervisorDiagLvlOne::Default1, "Default1");
		xgi::bind(this,&DummySupervisorDiagLvlOne::callDiagSystemPage, "callDiagSystemPage");
		xgi::bind(this,&DummySupervisorDiagLvlOne::callFsmPage, "callFsmPage");



		xgi::bind(this,&DummySupervisorDiagLvlOne::configureStateMachine, "configureStateMachine");
		xgi::bind(this,&DummySupervisorDiagLvlOne::stopStateMachine, "stopStateMachine");

		//DIAGREQUESTED
		xgi::bind(this,&DummySupervisorDiagLvlOne::configureDiagSystem, "configureDiagSystem");
		xgi::bind(this,&DummySupervisorDiagLvlOne::applyConfigureDiagSystem, "applyConfigureDiagSystem");

		//DIAGREQUESTED
		DIAG_DECLARE_EM_APP

  // ---------------------------------- State machine
  // Initiliase state
  /* Liste des etats qu'on va trouver dans la FSM */
  fsm_.addState ('I', "Initial") ;
  fsm_.addState ('H', "Halted");


  /* Methode configureAction qui doit gerer les constructions, etc... qui surviennent lors de la
  transition entre l'etat Initial et l'etat Halted */
  fsm_.addStateTransition ('I','H', EXECTRANSCH, this, &DummySupervisorDiagLvlOne::haltAction);
  fsm_.addStateTransition ('H','I', EXECTRANSHC, this, &DummySupervisorDiagLvlOne::configureAction);

  /* Si une transition merde, on appel la methode failedTransition */
  fsm_.setFailedStateTransitionAction( this, &DummySupervisorDiagLvlOne::failedTransition );

  /* On synchronise la FSM avec l'etat du process apres load, a savoir : etat HALTED */
  fsm_.setInitialState('H');
  fsm_.reset();

  //Export the stateName variable
  getApplicationInfoSpace()->fireItemAvailable("stateName", &fsm_.stateName_);

  // Bind SOAP callbacks for control messages
  xoap::bind (this, &DummySupervisorDiagLvlOne::fireEvent, EXECTRANSCH, XDAQ_NS_URI);
  xoap::bind (this, &DummySupervisorDiagLvlOne::fireEvent, EXECTRANSHC, XDAQ_NS_URI);

            #ifdef AUTO_CONFIGURE_PROCESSES
                std::stringstream timerName;
                timerName << getApplicationDescriptor()->getContextDescriptor()->getURL() << ":";
                timerName << getApplicationDescriptor()->getClassName() << ":" << getApplicationDescriptor()->getLocalId() << ":" << getApplicationDescriptor()->getInstance();
                toolbox::task::Timer * timer = toolbox::task::getTimerFactory()->createTimer(timerName.str());

                toolbox::TimeInterval interval(AUTO_LVL1_CONFIGURE_DELAY,0);
                toolbox::TimeVal start;
                start = toolbox::TimeVal::gettimeofday() + interval;
                timer->schedule( this, start,  0, "" );
            #endif


	}
	



	void getDefault1Content(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{

		*out << cgicc::p() << std::endl;
		*out << "DummySupervisor Diag Level One Default page is empty at the moment";
		*out << cgicc::p() << std::endl;

	}


	void getFsmStatePage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{

		*out << cgicc::p() << std::endl;
		if (fsm_.getCurrentState() == 'H') *out << "Current State is : Halted" << std::endl;
		if (fsm_.getCurrentState() == 'C') *out << "Current State is : Configured" << std::endl;
		if ( (fsm_.getCurrentState() != 'C') && (fsm_.getCurrentState() != 'H') ) *out << "Current State is : DANS LES CHOUX" << std::endl;
		*out << cgicc::p() << std::endl;


		std::string urlConfig = "/";
		urlConfig += getApplicationDescriptor()->getURN();
		urlConfig += "/configureStateMachine";	
		*out << cgicc::form().set("method","post").set("action", urlConfig).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Configure State Machine");
		*out << cgicc::p() << std::endl;
		*out << cgicc::form() << std::endl;

		*out << cgicc::p() << std::endl;

		std::string urlStop = "/";
		urlStop += getApplicationDescriptor()->getURN();
		urlStop += "/stopStateMachine";	
		*out << cgicc::form().set("method","post").set("action", urlStop).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Stop State Machine");
		*out << cgicc::p() << std::endl;
		*out << cgicc::form() << std::endl;

		*out << cgicc::p() << std::endl;
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
	*out << "<a href=" << urlDiag_ << ">Configure Sentinel Listener</a>";

	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";

	std::string urlFsm_ = "/"; \
	urlFsm_ += getApplicationDescriptor()->getURN(); \
	urlFsm_ += "/callFsmPage"; \
	*out << "<a href=" << urlFsm_ << ">FSM Access</a>";

	*out << "<br>";
	*out << cgicc::p() << std::endl;

}


	void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{


		*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
		*out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
		*out << cgicc::title("DummySupervisorDiagLvlOne Applet") << std::endl;

		xgi::Utils::getPageHeader
			(out, 
			getApplicationDescriptor()->getClassName()+" configuration page", 
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
	void DummySupervisorDiagLvlOne::DIAG_CONFIGURE_CALLBACK();
	void DummySupervisorDiagLvlOne::DIAG_CONFIGURE_FIRE_CALLBACK();

	//DIAGREQUESTED
	void DummySupervisorDiagLvlOne::DIAG_APPLY_CALLBACK();


	void fillClog(xoap::MessageReference msg, CLog * c)
	{
		//Fill CLog with received data
		xoap::SOAPBody rb = msg->getSOAPPart().getEnvelope().getBody();
		vector<xoap::SOAPElement> logElement = rb.getChildElements ();
		xoap::SOAPName timestamp ("timestamp", "", "");
		c->setTimeStamp(atoll(logElement[0].getAttributeValue(timestamp).c_str()));
		xoap::SOAPName logger ("logger", "", "");
		c->setLogger(logElement[0].getAttributeValue(logger));
		xoap::SOAPName level ("level", "", "");
		c->setLevel(logElement[0].getAttributeValue(level));
		xoap::SOAPName thread ("thread", "", "");
		c->setThread(logElement[0].getAttributeValue(thread));
		xoap::SOAPName message ("message", "", "");
		c->setMessage(logElement[0].getAttributeValue(message));
		xoap::SOAPName ndc ("ndc", "", "");
		c->setNDC(logElement[0].getAttributeValue(ndc));
		xoap::SOAPName file ("file", "", "");
		c->setFile(logElement[0].getAttributeValue(file));
		xoap::SOAPName line ("line", "", "");
		c->setLine(atoi(logElement[0].getAttributeValue(line).c_str()));
		xoap::SOAPName text ("text", "", "");
		c->setText(logElement[0].getAttributeValue(text));
		xoap::SOAPName errorcode ("errorcode", "", "");
		c->setErrorCode(logElement[0].getAttributeValue(errorcode));
		xoap::SOAPName faultstate ("faultstate", "", "");
		c->setFaultState(logElement[0].getAttributeValue(faultstate));
		xoap::SOAPName systemid ("systemid", "", "");
		c->setSystemID(logElement[0].getAttributeValue(systemid));
		xoap::SOAPName subsystemid ("subsystemid", "", "");
		c->setSubSystemID(logElement[0].getAttributeValue(subsystemid));
		xoap::SOAPName classname ("classname", "", "");
		c->setClassName(logElement[0].getAttributeValue(classname));
		xoap::SOAPName instance ("instance", "", "");
		c->setInstance(atoi(logElement[0].getAttributeValue(instance).c_str()));
		xoap::SOAPName lid ("lid", "", "");
		c->setLid(atoi(logElement[0].getAttributeValue(lid).c_str()));
		xoap::SOAPName edtimestamp ("edtimestamp", "", "");
		c->setEdTimeStamp(atoll(logElement[0].getAttributeValue(edtimestamp).c_str()));
		xoap::SOAPName lvl1name ("lvl1name", "", "");
		c->setLvl1Name(logElement[0].getAttributeValue(lvl1name));
		xoap::SOAPName lvl1instance ("lvl1instance", "", "");
		c->setLvl1Instance(logElement[0].getAttributeValue(lvl1instance));
		xoap::SOAPName routetoem ("routetoem", "", "");
		c->setRouteToEm(logElement[0].getAttributeValue(routetoem));
		xoap::SOAPName dbloglevel ("dbloglevel", "", "");
		c->setDbLogLevel(logElement[0].getAttributeValue(dbloglevel));
		xoap::SOAPName csloglevel ("csloglevel", "", "");
		c->setCsLogLevel(logElement[0].getAttributeValue(csloglevel));
		xoap::SOAPName ccloglevel ("ccloglevel", "", "");
		c->setCcLogLevel(logElement[0].getAttributeValue(ccloglevel));
		xoap::SOAPName forceddblogs ("forceddblogs", "", "");
		c->setForcedDbLog(logElement[0].getAttributeValue(forceddblogs));
		xoap::SOAPName forcedcslogs ("forcedcslogs", "", "");
		c->setForcedCsLog(logElement[0].getAttributeValue(forcedcslogs));
		xoap::SOAPName forcedcclogs ("forcedcclogs", "", "");
		c->setForcedCcLog(logElement[0].getAttributeValue(forcedcclogs));
		xoap::SOAPName gloglevel ("gloglevel", "", "");
		c->setGLogLevel(logElement[0].getAttributeValue(gloglevel));
	}


	void displayClog(CLog * c)
	{
		if (diagService_->reconfUsable_ == false)
		{
			std::cout << "RECONF message received, but I will not use it." << std::endl;
		}
		else
		{
			std::cout << std::endl << std::endl << std::endl;
			std::cout << "in DummySupervisorDiagLvlOne, CLog filled" << std::endl;
			std::cout << "TimeStamp is : " << c->getTimeStamp() << std::endl;
			std::cout << "Logger is : " << c->getLogger() << std::endl;
			std::cout << "Level is : " << c->getLevel() << std::endl;
			std::cout << "Threads is : " << c->getThread() << std::endl;
			std::cout << "Message is : " << c->getMessage() << std::endl;
			std::cout << "NDC is : " << c->getNDC() << std::endl;
			std::cout << "File is : " << c->getFile() << std::endl;
			std::cout << "Line is : " << c->getLine() << std::endl;
			std::cout << "Text is : " << c->getText() << std::endl;
			std::cout << "ErrorCode is : " << c->getErrorCode() << std::endl;
			std::cout << "FaultState is : " << c->getFaultState() << std::endl;
			std::cout << "SystemID is : " << c->getSystemID() << std::endl;
			std::cout << "SubSystemID is : " << c->getSubSystemID() << std::endl;
			std::cout << "ClassName is : " << c->getClassName() << std::endl;
			std::cout << "Instance is : " << c->getInstance() << std::endl;
			std::cout << "LocalID is : " << c->getLid() << std::endl;
			std::cout << "ErrorDispatcherTimeStamp is : " << c->getEdTimeStamp() << std::endl;
			std::cout << "Lvl1Name is : " << c->getLvl1Name() << std::endl;
			std::cout << "Lvl1Instance is : " << c->getLvl1Instance() << std::endl;
			std::cout << "RouteToErrorManager is : " << c->getRouteToEm() << std::endl;
			std::cout << "DbLogLevel is : " << c->getDbLogLevel() << std::endl;
			std::cout << "CsLogLevel is : " << c->getCsLogLevel() << std::endl;
			std::cout << "CcLogLevel is : " << c->getCcLogLevel() << std::endl;
			std::cout << "ForcedDbLogs is : " << c->getForcedDbLog() << std::endl;
			std::cout << "ForcedCsLogs is : " << c->getForcedCsLog() << std::endl;
			std::cout << "ForcedCcLogs is : " << c->getForcedCcLog() << std::endl;
			std::cout << "GlobalLogLevel is : " << c->getGLogLevel() << std::endl;
		}
	}



	// SOAP Callback triggered on ReceivedError
	xoap::MessageReference onIncomingErr (xoap::MessageReference msg) throw (xoap::exception::Exception)
	{
		#define DEBUG_DUMMYLVL1_ON_INCOMING_ERR
		CLog c;
		// reply to caller
		xoap::MessageReference reply = xoap::createMessage();
		xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
		xoap::SOAPName responseName = envelope.createName( "onIncomingErrResponse", "xdaq", XDAQ_NS_URI);
		envelope.getBody().addBodyElement ( responseName );

		//Fill CLOG with SOAP message content
		fillClog(msg,&c);
		
		//If requested, display CLOG
		//displayClog(&c);

		//If we are not in reconfiguration stage, just ACK the reconf command to ED
		if ( (diagService_->reconfUsable_ == false) && (c.getText() == "RECONFCMD") )
		{
			#ifdef DEBUG_DUMMYLVL1_ON_INCOMING_ERR
				std::cout << "Reconf cmd on open loop received ; auto-ack sent" << std::endl;
			#endif

			diagService_->reportError("Reconfiguration auto ACK Log sent by Lvl1 to Error Dispatcher",
			 DIAGINFO,
			 "RECONFACK",
			 atoi(c.getErrorCode().c_str()),
			 "STEADY",
			 "TRACKER",
			 "TEC");
		}


		/*
		USERS_NOTE
		
		The following block must be modified to set up a home-made system recovery loop.
		
		One example is harcoded here.
		- When a reconfiguration request message arrives, sended by a process to the Error Dispatcher
		and routed from the Error Dispatcher to here, and if the error code associated
		to this message is equal to 100, then the recovery function "recovery100", located
		in the file DummySupervisor.h, is fired.
		- If a reconfiguration request message arrives with an associated error code different from 100,
		as we do not have any specific reconfiguration procedure to fire in this example for error codes
		different from 100, nothing is done. We simply ACK the reconfiguration request to the Error
		Dispatcher, like if we were in an opened loop case.
		- If you have more than one error code to manage (i.e. more than one error code which will
		fire a reconfiguration function, you have two solutions :
		  --Call a general reconfiguration function and take appropriate actions in it, according to the
		    error code
		  --Set up in this method as many callbacks to fire as you have error codes to process (1 error code
		  links to 1 reconfiguration function).
		*/
		//If we are in reconfiguration stage, then ...
		if (diagService_->reconfUsable_ == true)
		{
			if (c.getText() == "RECONFCMD")
			{
				switch ( atoi(c.getErrorCode().c_str()) )
				{
					case 100 :
						#ifdef DEBUG_DUMMYLVL1_ON_INCOMING_ERR
							std::cout << "Reconf cmd on closed loop received for ErrNo:" << atoi(c.getErrorCode().c_str()) << " ; Automated System Recovery launched." << std::endl;
							std::cout << "Calling System Recovery Callback" << std::endl;
						#endif
						try
						{
						  	xoap::MessageReference msg = xoap::createMessage();
					                xoap::SOAPPart soap = msg->getSOAPPart();
					                xoap::SOAPEnvelope envelope = soap.getEnvelope();
					                xoap::SOAPBody body = envelope.getBody();
			        		        xoap::SOAPName command = envelope.createName("recovery100","xdaq", "urn:xdaq-soap:3.0");
			                		body.addBodyElement(command);
							xdaq::ApplicationDescriptor * d = getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(diagService_->reconfName_, atol(diagService_->reconfInstance_.c_str()));
							xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, d);
						} 
						catch (xdaq::exception::Exception& e)
						{
							#ifdef DEBUG_DUMMYLVL1_ON_INCOMING_ERR
								std::cout << "Cannot reach callback for recovery100 !" << std::endl;
								std::cout << "I will at least try to ACK the ReconfigurationRunning status from ErrorDispatcher." << std::endl;
							#endif
							diagService_->reportError("This is a Reconfiguration Call Failure ACK Log",
							DIAGINFO,
							"RECONFACK",
							atoi(c.getErrorCode().c_str()),
							"STEADY",
							"TRACKER",
							"TEC");
						}
					break;
	
					default :
						#ifdef DEBUG_DUMMYLVL1_ON_INCOMING_ERR
							std::cout << "Reconf cmd on closed loop received for ErrNo " << atoi(c.getErrorCode().c_str()) << " ; ErrNo not linked to specific reconfiguration code ; doing nothing" << std::endl;
						#endif
						diagService_->reportError("This is a Reconfiguration auto ACK Log",
						DIAGINFO,
						"RECONFACK",
						atoi(c.getErrorCode().c_str()),
						"STEADY",
						"TRACKER",
						"TEC");
					 break;
				}

			}
			
			if (c.getText() == "RECONFRUN")
			{
				/*
				USERS_NOTE
				
				In this section, you can extract the content of the fields of an error message sent with a
				"RECONFRUN" tag in the "text" field of this error message.
				
				For example, when running the recovery method "recovery100" in file DummySupervisor.h, if an
				error message of kind "RECONFRUN" is sent by this method, the content of the fields
				FaultState, SystemID and SubSystemID can be retrieved here via c.getFaultState(), c.getSystemID()
				and c.getSubSystemID() methods.
				This allows you to send informations about the recovery actions/status from your recovery method
				to this process.
				*/

				#ifdef DEBUG_DUMMYLVL1_ON_INCOMING_ERR
					std::cout << "Received reconfiguration info : message is : " << c.getMessage() << std::endl;
				#endif
			}

			if (c.getText() == "RECONFSTOP")
			{
				#ifdef DEBUG_DUMMYLVL1_ON_INCOMING_ERR
					std::cout << "Reconfiguration stop request on closed loop received ; ack sent" << std::endl;
					std::cout << "Attached message was : " << c.getMessage() << std::endl;
				#endif

				diagService_->reportError("This is a Reconfiguration requested ACK Log",
				DIAGINFO,
				"RECONFACK",
				atoi(c.getErrorCode().c_str()),
				"STEADY",
				"TRACKER",
				"TEC");
			}
		}
	return reply;

	}	



  /** \brief initiliase all the paremters for the DiagSentinelErrorsGrabber
   */
void configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
	//Apply fireitems value to internals and check validity
	DIAG_EXEC_FSM_INIT_TRANS
	
}


void haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
/*
	diagService_->useDefaultAppender_ = false;
	diagService_->useConsoleDump_ = false;
	diagService_->useSentinel_ = false;
	diagService_->useDiag_ = false;
	diagService_->useReconf_ = false;
*/

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

    #ifdef AUTO_CONFIGURE_PROCESSES
        void timeExpired (toolbox::task::TimerEvent& e) 
        {
            DIAG_EXEC_FSM_INIT_TRANS
        }
    #endif


};

#endif
#endif
