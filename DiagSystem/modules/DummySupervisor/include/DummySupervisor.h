/*
   FileName : 		DummySupervisor.h

   Content : 		DummySupervisor module

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

#ifndef _DummySupervisor_h_
#define _DummySupervisor_h_

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


//Use L.M. gwt-interface applet
#ifdef WILL_USE_GWT_FIREITEMS_VIEWER
	#include "tools/standalone/jsinterface.h"
#endif


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

#include "DummySupervisorV.h"

#include "toolbox/BSem.h"



#ifdef AUTO_CONFIGURE_PROCESSES
	#ifdef WILL_USE_GWT_FIREITEMS_VIEWER
		class DummySupervisor: public xdaq::Application, public toolbox::task::TimerListener, public JsInterface
	#else
		class DummySupervisor: public xdaq::Application, public toolbox::task::TimerListener
	#endif
#else
	#ifdef WILL_USE_GWT_FIREITEMS_VIEWER
		class DummySupervisor: public xdaq::Application, public JsInterface
	#else
		class DummySupervisor: public xdaq::Application
	#endif
#endif

{


public:

	toolbox::BSem * executeReconfMethodMutex;

  FsmWithStateName fsm_;

	int internalState_;


	//DIAGREQUESTED
	DiagBagWizard * diagService_;



//	toolbox::BSem executeReconfMethodMutex = new toolbox::BSem::BSem(toolbox::BSem::FULL);
	
        std::string simulatedFecHardId_;
        std::string simulatedFecRing_;
        std::string simulatedCcuAddress_;
        std::string simulatedI2cChannel_;
        std::string simulatedI2cAddress_;
        std::string simulatedFedId_;
        std::string simulatedFedChannel_;
        std::string simulatedPsuId_;




		std::stringstream myPixelsConsole;


	//! define factory method for instantion of HelloWorld application
	XDAQ_INSTANTIATOR();

	DummySupervisor(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception);
	~DummySupervisor();

	void getDefault1Content(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	void getFsmStatePage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
	
	void Default1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	void callDiagSystemPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	void callFsmPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	void displayLinks(xgi::Input * in, xgi::Output * out);

	void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	//DIAGREQUESTED
	void DIAG_CONFIGURE_CALLBACK();
	void DIAG_APPLY_CALLBACK();
/*
	void DIAG_FREELCLSEM_CALLBACK();
	void DIAG_FREEGLBSEM_CALLBACK();
	void DIAG_REQUEST_ENTRYPOINT();
*/


	void simulateFecError(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);


	void simulateFedError(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	void simulatePsuError(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	void sendReconfAutoLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	void sendReconfStopLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	void sendReconfRunLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	void sendReconfLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	void sendTraceLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	void sendDebugLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	void sendInfoLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	void sendWarnLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	void sendUserInfoLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	void sendErrorLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	void sendFatalLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

	void configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);

	void haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
   
   
	xoap::MessageReference fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception);


	void failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);

	void configureStateMachine(xgi::Input * in, xgi::Output * out);

	void stopStateMachine(xgi::Input * in, xgi::Output * out);


    void timeExpired (toolbox::task::TimerEvent& e);

	xoap::MessageReference recoveryEntry(xoap::MessageReference msg) throw (xoap::exception::Exception);




};


#endif
#endif
