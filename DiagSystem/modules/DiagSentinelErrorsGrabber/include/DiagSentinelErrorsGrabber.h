/*
   FileName : 		DiagSentinelErrorsGrabber.h

   Content : 		DiagSentinelErrorsGrabber module

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


#ifndef _DiagSentinelErrorsGrabber_h_
#define _DiagSentinelErrorsGrabber_h_

#ifndef _SOAPStateMachine_h_
#define _SOAPStateMachine_h_


#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"
#include "xdaq/NamespaceURI.h"
#include "xdaq/WebApplication.h"
#include "xdaq/exception/ApplicationNotFound.h"


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
#include "cgicc/HTTPResponseHeader.h"

#include "xdata/String.h"
#include "xdata/UnsignedLong.h"
#include "xdata/Integer.h"

#include <sys/time.h>

#include "xcept/tools.h"
#include "toolbox/utils.h"


#include "sentinel/Interface.h"
#include "sentinel/Listener.h"


#include <diagbag/DiagBagWizard.h>


#include "DiagCompileOptions.h"
/* needed for autoconfiguration */
#ifdef AUTO_CONFIGURE_PROCESSES
    #include "toolbox/task/Timer.h"
    #include "toolbox/task/TimerFactory.h"
    #include "toolbox/TimeInterval.h"
#endif



/* needed for FSM */
#ifdef WILL_COMPILE_FOR_PIXELS
	#include "toolbox/fsm/FsmWithStateName.h"
#else
	#ifdef WILL_COMPILE_AS_STANDALONE
		#include "tools/standalone/FsmWithStateName.h"
	#else //Compile in Tracker Framework
			#include "FsmWithStateName.h"
	#endif
#endif


#include "toolbox/fsm/FailedEvent.h"
#define EXECTRANSHC "Configure"
#define EXECTRANSCH "Halt"

#include "DiagSentinelErrorsGrabberV.h"


#ifdef AUTO_CONFIGURE_PROCESSES
    class DiagSentinelErrorsGrabber: public xdaq::WebApplication, sentinel::Listener, public toolbox::task::TimerListener
#else
    class DiagSentinelErrorsGrabber: public xdaq::WebApplication, sentinel::Listener
#endif
{

protected:
    sentinel::Interface * sentinel_;

public:
    FsmWithStateName fsm_;
    int internalState_;

    /* DIAGREQUESTED */
    DiagBagWizard * diagService_;

    /* define factory method for instantion of application */
    XDAQ_INSTANTIATOR();

    DiagSentinelErrorsGrabber(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception) : xdaq::WebApplication(s)
    {
        /* Give funny and useless informations at load time */
        std::stringstream mmesg;
        mmesg << "Process version " << DIAGSENTINELERRORSGRABBER_PACKAGE_VERSION << " loaded";
        this->getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);
        LOG4CPLUS_INFO(this->getApplicationLogger(), mmesg.str());
        this->getApplicationLogger().setLogLevel(ERROR_LOG_LEVEL);

        internalState_ = 0;

        /* DIAGREQUESTED */
/*
        diagService_ = new DiagBagWizard::DiagBagWizard(
                                                        (getApplicationDescriptor()->getClassName() + "DiagLvlOne") ,
                                                        this->getApplicationLogger(),
                                                        getApplicationDescriptor()->getClassName(),
                                                        getApplicationDescriptor()->getInstance(),
                                                        getApplicationDescriptor()->getLocalId(),
                                                        (xdaq::WebApplication *)this
                                                        );
*/

		 diagService_ = new DiagBagWizard(
 			 ("ReconfigurationModule") ,
 			 this->getApplicationLogger(),
			 getApplicationDescriptor()->getClassName(),
			 getApplicationDescriptor()->getInstance(),
			 getApplicationDescriptor()->getLocalId(),
			 (xdaq::WebApplication *)this,
			 "MYSYSTEM",
			 "MYSUBSYTSTEM"
			  );

        /* export Web interfaces */
        xgi::bind(this,&DiagSentinelErrorsGrabber::Default, "Default");
        xgi::bind(this,&DiagSentinelErrorsGrabber::Default1, "Default1");
        xgi::bind(this,&DiagSentinelErrorsGrabber::callDiagSystemPage, "callDiagSystemPage");
        xgi::bind(this,&DiagSentinelErrorsGrabber::callFsmPage, "callFsmPage");

        xgi::bind(this,&DiagSentinelErrorsGrabber::configureStateMachine, "configureStateMachine");
        xgi::bind(this,&DiagSentinelErrorsGrabber::stopStateMachine, "stopStateMachine");

        /* DIAGREQUESTED */
        xgi::bind(this,&DiagSentinelErrorsGrabber::configureDiagSystem, "configureDiagSystem");
        xgi::bind(this,&DiagSentinelErrorsGrabber::applyConfigureDiagSystem, "applyConfigureDiagSystem");

        /* DIAGREQUESTED */
        DIAG_DECLARE_SENTINEL_GRABBER_APP

        /* Liste des etats qu'on va trouver dans la FSM */
        fsm_.addState ('C', "Configured") ;
        fsm_.addState ('H', "Halted");

        /* Methode initialiseAction qui doit gerer les constructions, etc... qui surviennent lors de la
        transition entre l'etat Initial et l'etat Halted */
        fsm_.addStateTransition ('C','H', EXECTRANSCH, this, &DiagSentinelErrorsGrabber::haltAction);
        fsm_.addStateTransition ('H','C', EXECTRANSHC, this, &DiagSentinelErrorsGrabber::configureAction);

        /* Si une transition merde, on appel la methode failedTransition */
        fsm_.setFailedStateTransitionAction( this, &DiagSentinelErrorsGrabber::failedTransition );

        /* On synchronise la FASM avec l'etat du process apres load, a savoir : etat HALTED */
        fsm_.setInitialState('H');
        fsm_.reset();

        /* Export the stateName variable */
        getApplicationInfoSpace()->fireItemAvailable("stateName", &fsm_.stateName_);

        /* Bind SOAP callbacks for control messages */
        xoap::bind (this, &DiagSentinelErrorsGrabber::fireEvent, EXECTRANSCH, XDAQ_NS_URI);
        xoap::bind (this, &DiagSentinelErrorsGrabber::fireEvent, EXECTRANSHC, XDAQ_NS_URI);


            #ifdef AUTO_CONFIGURE_PROCESSES
                std::stringstream timerName;
                timerName << getApplicationDescriptor()->getContextDescriptor()->getURL() << ":";
                timerName << getApplicationDescriptor()->getClassName() << ":" << getApplicationDescriptor()->getLocalId() << ":" << getApplicationDescriptor()->getInstance();
                toolbox::task::Timer * timer = toolbox::task::getTimerFactory()->createTimer(timerName.str());
                toolbox::TimeInterval interval(AUTO_SG_CONFIGURE_DELAY,0);
                toolbox::TimeVal start;
                start = toolbox::TimeVal::gettimeofday() + interval;
                timer->schedule( this, start,  0, "" );
            #endif


	}



    void getDefault1Content(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
    {
        *out << cgicc::p() << std::endl;
        *out << "Sentinel Listener Default page is empty at the moment";
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
        *out << cgicc::title("DiagSentinel Errors Grabber") << std::endl;

        xgi::Utils::getPageHeader(out, 
                                "Sentinel Listener", 
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



    void onException(xcept::Exception& e)
    {
        #define DEBUG_XCEPT

        #ifdef DEBUG_XCEPT
            std::cout << "got an exception" << std::endl;
        #endif

/*
        xcept::ExceptionHistory history0(e);
        std::string mycontext;
        std::string message;
        std::string severity;
        std::string sessionID;

        while ( history0.hasMore() )
        {
            xcept::ExceptionInformation & info = history0.getPrevious();
            std::map<std::string, std::string, std::less<std::string> > & properties = info.getProperties();
            for  (std::map<std::string, std::string, std::less<std::string> >::iterator j = properties.begin(); j != properties.end(); j++)
            {
                // std::cout << (*j).first << " = " << (*j).second << std::endl;
                if ( (*j).first == "module" ) mycontext = (*j).second;
                if ( (*j).first == "message" ) message = (*j).second;
                if ( (*j).first == "severity" ) severity = (*j).second;
                if ( (*j).first == "sessionID" ) sessionID = (*j).second;			
            }
        }

        #ifdef DEBUG_XCEPT
            if (sessionID == "diagPropagatedMax")
            {
                std::cout <<"Process error string as LARGE SOAP buffer" << std::endl;
                std::cout <<"Error severity is : " << severity << std::endl;
                std::cout <<"Error string is : " << std::endl;		
                std::cout << message << std::endl << std::endl;
            }
            else
            {
                if (sessionID == "diagPropagatedMin")
                {
                    std::cout <<"Process error string as SMALL SOAP buffer" << std::endl;
                    std::cout <<"Error severity is : " << severity << std::endl;
                    std::cout <<"Error string is : " << std::endl;
                    std::cout << message << std::endl << std::endl;
                }
                else
                {
                    std::cout <<"Process error string as regular buffer" << std::endl;
                    std::cout <<"Error severity is : " << severity << std::endl;
                    std::cout <<"Error string is : " << std::endl;
                    std::cout << message << std::endl << std::endl;
                }
            }
        #endif


        // Decode message embedded into exception "Message" field 
        std::string ebMsg;
        std::string ebText;
        std::string ebErrorCode;
        std::string ebFaultState;
        std::string ebSystemID;
        std::string ebSubSystemID;
        std::string ebClassname;
        std::string ebInstance;
        std::string ebLID;
        std::string eblvl1name;
        std::string ebinitialLogger;
        std::string ebinitialTimeStamp;
        std::string ebRelayed;

        if ( (sessionID == "diagPropagatedMin") || (sessionID == "diagPropagatedMax") )
        {
            unsigned int i=0;
            while ( (message[i] != '\"') && i<strlen(message.c_str()) )
            {
                ebMsg = ebMsg + message[i];
                i++;
            }
            i++;
            #ifdef DEBUG_XCEPT
                std::cout << "Embedded message is : " << ebMsg << std::endl;
            #endif

            while ( (message[i] != '\"') && i<strlen(message.c_str()) ) i++;
            i++;
            while ( (message[i] != '\"') && i<strlen(message.c_str()) )
            {
                ebText = ebText + message[i];
                i++;
            }
            i++;
            #ifdef DEBUG_XCEPT
                std::cout << "Embedded text is : " << ebText << std::endl;
            #endif

            while ( (message[i] != '\"') && i<strlen(message.c_str()) ) i++;
            i++;
            while ( (message[i] != '\"') && i<strlen(message.c_str()) )
            {
                ebErrorCode = ebErrorCode + message[i];
                i++;
            }
            i++;
            #ifdef DEBUG_XCEPT
                std::cout << "Embedded ErrorCode is : " << ebErrorCode << std::endl;
            #endif

            while ( (message[i] != '\"') && i<strlen(message.c_str()) ) i++;
            i++;
            while ( (message[i] != '\"') && i<strlen(message.c_str()) )
            {
                ebFaultState = ebFaultState + message[i];
                i++;
            }
            i++;
            #ifdef DEBUG_XCEPT
                std::cout << "Embedded FaultState is : " << ebFaultState << std::endl;
            #endif

            while ( (message[i] != '\"') && i<strlen(message.c_str()) ) i++;
            i++;
            while ( (message[i] != '\"') && i<strlen(message.c_str()) )
            {
                ebSystemID = ebSystemID + message[i];
                i++;
            }
            i++;
            #ifdef DEBUG_XCEPT
            std::cout << "Embedded SystemID is : " << ebSystemID << std::endl;
            #endif

            while ( (message[i] != '\"') && i<strlen(message.c_str()) ) i++;
            i++;
            while ( (message[i] != '\"') && i<strlen(message.c_str()) )
            {
                ebSubSystemID = ebSubSystemID + message[i];
                i++;
            }
            i++;
            #ifdef DEBUG_XCEPT
                std::cout << "Embedded SubSystemID is : " << ebSubSystemID << std::endl;
            #endif

            while ( (message[i] != '\"') && i<strlen(message.c_str()) ) i++;
            i++;
            while ( (message[i] != '\"') && i<strlen(message.c_str()) )
            {
                ebClassname = ebClassname + message[i];
                i++;
            }
            i++;
            #ifdef DEBUG_XCEPT
                std::cout << "Embedded Classname is : " << ebClassname << std::endl;
            #endif

            while ( (message[i] != '\"') && i<strlen(message.c_str()) ) i++;
            i++;
            while ( (message[i] != '\"') && i<strlen(message.c_str()) )
            {
                ebInstance = ebInstance + message[i];
                i++;
            }
            i++;
            #ifdef DEBUG_XCEPT
                std::cout << "Embedded Instance is : " << ebInstance << std::endl;
            #endif

            while ( (message[i] != '\"') && i<strlen(message.c_str()) ) i++;
            i++;
            while ( (message[i] != '\"') && i<strlen(message.c_str()) )
            {
                ebLID = ebLID + message[i];
                i++;
            }
            i++;
            #ifdef DEBUG_XCEPT
                std::cout << "Embedded LID is : " << ebLID << std::endl;
            #endif

            while ( (message[i] != '\"') && i<strlen(message.c_str()) ) i++;
            i++;
            while ( (message[i] != '\"') && i<strlen(message.c_str()) )
            {
                eblvl1name = eblvl1name + message[i];
                i++;
            }
            i++;
            #ifdef DEBUG_XCEPT
                std::cout << "Embedded lvl1name is : " << eblvl1name << std::endl;
            #endif

            for (int k=0; k<11; k++)
            {
                while ( (message[i] != '\"') && i<strlen(message.c_str()) ) i++;
                i++;
                while ( (message[i] != '\"') && i<strlen(message.c_str()) )
                {
                    i++;
                }
                i++;
            }

            // Jump to next interesting fields sequence ; Bypass 11 fields
            while ( (message[i] != '\"') && i<strlen(message.c_str()) ) i++;
            i++;
            while ( (message[i] != '\"') && i<strlen(message.c_str()) )
            {
                ebinitialLogger = ebinitialLogger + message[i];
                i++;
            }
            i++;
            #ifdef DEBUG_XCEPT
                std::cout << "Embedded initialLogger is : " << ebinitialLogger << std::endl;
            #endif

            while ( (message[i] != '\"') && i<strlen(message.c_str()) ) i++;
            i++;
            while ( (message[i] != '\"') && i<strlen(message.c_str()) )
            {
                ebinitialTimeStamp = ebinitialTimeStamp + message[i];
                i++;
            }
            i++;
            #ifdef DEBUG_XCEPT
                std::cout << "Embedded initialTimeStamp is : " << ebinitialTimeStamp << std::endl;
            #endif

            while ( (message[i] != '\"') && i<strlen(message.c_str()) ) i++;
            i++;
            while ( (message[i] != '\"') && i<strlen(message.c_str()) )
            {
                ebRelayed = ebRelayed + message[i];
                i++;
            }
            i++;
            #ifdef DEBUG_XCEPT
                std::cout << "Embedded Relayed is : " << ebRelayed << std::endl;
            #endif
        }
*/
/*
        if (sessionID == "diagPropagatedMin")
        {
            #ifdef DEBUG_XCEPT
                std::cout << "Now pushing message to defined shortcall logger(s)" << std::endl << std::endl;
            #endif
            if (severity == DIAGTRACE) diagService_->reportError(ebMsg, DIAGTRACE, ebClassname, (unsigned long)atol(ebInstance.c_str()), (unsigned long)atol(ebLID.c_str()), ebinitialLogger, ebinitialTimeStamp, ebRelayed );
            if (severity == DIAGDEBUG) diagService_->reportError(ebMsg, DIAGDEBUG, ebClassname, (unsigned long)atol(ebInstance.c_str()), (unsigned long)atol(ebLID.c_str()), ebinitialLogger, ebinitialTimeStamp, ebRelayed );
            if (severity == DIAGINFO) diagService_->reportError(ebMsg, DIAGINFO, ebClassname, (unsigned long)atol(ebInstance.c_str()), (unsigned long)atol(ebLID.c_str()), ebinitialLogger, ebinitialTimeStamp, ebRelayed );
            if (severity == DIAGWARN) diagService_->reportError(ebMsg, DIAGWARN, ebClassname, (unsigned long)atol(ebInstance.c_str()), (unsigned long)atol(ebLID.c_str()), ebinitialLogger, ebinitialTimeStamp, ebRelayed );
            if (severity == DIAGUSERINFO) diagService_->reportError(ebMsg, DIAGUSERINFO, ebClassname, (unsigned long)atol(ebInstance.c_str()), (unsigned long)atol(ebLID.c_str()), ebinitialLogger, ebinitialTimeStamp, ebRelayed );
            if (severity == DIAGERROR) diagService_->reportError(ebMsg, DIAGERROR, ebClassname, (unsigned long)atol(ebInstance.c_str()), (unsigned long)atol(ebLID.c_str()), ebinitialLogger, ebinitialTimeStamp, ebRelayed );
            if (severity == DIAGFATAL) diagService_->reportError(ebMsg, DIAGFATAL, ebClassname, (unsigned long)atol(ebInstance.c_str()), (unsigned long)atol(ebLID.c_str()), ebinitialLogger, ebinitialTimeStamp, ebRelayed );
        }

        if (sessionID == "diagPropagatedMax")
        {
            #ifdef DEBUG_XCEPT
                std::cout << "Now pushing message to defined longcall logger(s)" << std::endl << std::endl;
            #endif
            if (severity == DIAGTRACE) diagService_->reportError(ebMsg, DIAGTRACE, ebText, (int)atol(ebErrorCode.c_str()), ebFaultState, ebSystemID, ebSubSystemID, ebClassname, (unsigned long)atol(ebInstance.c_str()), (unsigned long)atol(ebLID.c_str()), ebinitialLogger, ebinitialTimeStamp, ebRelayed );
            if (severity == DIAGDEBUG) diagService_->reportError(ebMsg, DIAGDEBUG, ebText, (int)atol(ebErrorCode.c_str()), ebFaultState, ebSystemID, ebSubSystemID, ebClassname, (unsigned long)atol(ebInstance.c_str()), (unsigned long)atol(ebLID.c_str()), ebinitialLogger, ebinitialTimeStamp, ebRelayed );
            if (severity == DIAGINFO) diagService_->reportError(ebMsg, DIAGINFO, ebText, (int)atol(ebErrorCode.c_str()), ebFaultState, ebSystemID, ebSubSystemID, ebClassname, (unsigned long)atol(ebInstance.c_str()), (unsigned long)atol(ebLID.c_str()), ebinitialLogger, ebinitialTimeStamp, ebRelayed );
            if (severity == DIAGWARN) diagService_->reportError(ebMsg, DIAGWARN, ebText, (int)atol(ebErrorCode.c_str()), ebFaultState, ebSystemID, ebSubSystemID, ebClassname, (unsigned long)atol(ebInstance.c_str()), (unsigned long)atol(ebLID.c_str()), ebinitialLogger, ebinitialTimeStamp, ebRelayed );
            if (severity == DIAGUSERINFO) diagService_->reportError(ebMsg, DIAGUSERINFO, ebText, (int)atol(ebErrorCode.c_str()), ebFaultState, ebSystemID, ebSubSystemID, ebClassname, (unsigned long)atol(ebInstance.c_str()), (unsigned long)atol(ebLID.c_str()), ebinitialLogger, ebinitialTimeStamp, ebRelayed );
            if (severity == DIAGERROR) diagService_->reportError(ebMsg, DIAGERROR, ebText, (int)atol(ebErrorCode.c_str()), ebFaultState, ebSystemID, ebSubSystemID, ebClassname, (unsigned long)atol(ebInstance.c_str()), (unsigned long)atol(ebLID.c_str()), ebinitialLogger, ebinitialTimeStamp, ebRelayed );
            if (severity == DIAGFATAL) diagService_->reportError(ebMsg, DIAGFATAL, ebText, (int)atol(ebErrorCode.c_str()), ebFaultState, ebSystemID, ebSubSystemID, ebClassname, (unsigned long)atol(ebInstance.c_str()), (unsigned long)atol(ebLID.c_str()), ebinitialLogger, ebinitialTimeStamp, ebRelayed );
        }


        if ( (sessionID != "diagPropagatedMin") && (sessionID != "diagPropagatedMax") )
        {
            #ifdef DEBUG_XCEPT
                std::cout << "Now pushing message to default XceptCalls logger" << std::endl << std::endl;
            #endif
            if (severity == DIAGTRACE) diagService_->reportError(message, DIAGTRACE);
            if (severity == DIAGDEBUG) diagService_->reportError(message, DIAGDEBUG);
            if (severity == DIAGINFO) diagService_->reportError(message, DIAGINFO);
            if (severity == DIAGWARN) diagService_->reportError(message, DIAGWARN);
            if (severity == DIAGUSERINFO) diagService_->reportError(message, DIAGUSERINFO);
            if (severity == DIAGERROR) diagService_->reportError(message, DIAGERROR);
            if (severity == DIAGFATAL) diagService_->reportError(message, DIAGFATAL);
        }
*/
    }	


    /* DIAGREQUESTED */
    void DiagSentinelErrorsGrabber::DIAG_CONFIGURE_CALLBACK();
    void DiagSentinelErrorsGrabber::DIAG_CONFIGURE_FIRE_CALLBACK();

    /* DIAGREQUESTED */
    void DiagSentinelErrorsGrabber::DIAG_APPLY_CALLBACK();

    /* Initialise all the paremters for the DiagSentinelErrorsGrabber module */
    void configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
    {
        /* Apply fireitems value to internals and check validity */
        DIAG_EXEC_FSM_INIT_TRANS
    }

    /* Unset all logging options for the DiagSentinelErrorsGrabber module */
    void haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
    {
        /* Apply fireitems value to internals and check validity */
        DIAG_EXEC_FSM_STOP_TRANS
    }
   
   
   
    xoap::MessageReference fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception)
    {
        /* std::cout << "fireEvent" << std::endl ; */
        xoap::SOAPPart part = msg->getSOAPPart();
        xoap::SOAPEnvelope env = part.getEnvelope();
        xoap::SOAPBody body = env.getBody();
        DOMNode* node = body.getDOMNode();
        DOMNodeList* bodyList = node->getChildNodes();
        for (unsigned int i = 0; i < bodyList->getLength(); i++)
        {
            DOMNode* command = bodyList->item(i);
            if (command->getNodeType() == DOMNode::ELEMENT_NODE)
            {
                std::string commandName = xoap::XMLCh2String (command->getLocalName());

                if ( (fsm_.getCurrentState() == 'H') && (commandName == EXECTRANSHC) || (fsm_.getCurrentState() == 'C') && (commandName == EXECTRANSCH) )
                {
                    try
                    {
                        toolbox::Event::Reference e(new toolbox::Event(commandName, this));
                        fsm_.fireEvent(e);
                        /* Synchronize Web state machine */
                        /* wsm_.setInitialState(fsm_.getCurrentState()); */
                    }
                    catch (toolbox::fsm::exception::Exception & e)
                    {
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



































