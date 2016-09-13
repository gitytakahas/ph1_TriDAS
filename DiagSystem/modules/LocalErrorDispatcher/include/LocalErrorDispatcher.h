/*
   FileName : 		LocalErrorDispatcher.h

   Content : 		LocalErrorDispatcher module

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

#ifndef _LocalErrorDispatcher_h_
#define _LocalErrorDispatcher_h_

#ifndef _SOAPStateMachine_h_
#define _SOAPStateMachine_h_

#include <sys/time.h>

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


#include "DiagCompileOptions.h"
#include <diagbag/DiagBagWizard.h>

#include "LocalErrorDispatcherV.h"



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


#include "tools/ErrorsDbRelated/XMLTemplatesForGlbEd.hh"

//Use L.M. gwt-interface applet
#include "tools/standalone/jsinterface.h"


#define LOGARRAYLINES   100
#define LOGARRAYROWS    10

#define LNOFFSET       0
#define MSGOFFSET       1
#define ORGOFFSET       2
#define TSOFFSET        3
#define LVLOFFSET       4
#define ECOFFSET        5
#define FSOFFSET        6
#define SIDOFFSET       7
#define SSIDOFFSET      8
#define EDTSOFFSET      9

#define LOGFILENAMEPREFIX	"errorsLogFile--LED--"
#define LOGFILENAMESUFFIX	".xml"

#define PIXELSXMLEXTENSION	"--pixelsXml"

#define PIXELSLOG4JEXTENSION	"--log4jXml"

//#define ED_FORCE_FILE_LOGGING

#ifdef AUTO_CONFIGURE_PROCESSES
    class LocalErrorDispatcher: public xdaq::Application, public toolbox::task::TimerListener
#else
    class LocalErrorDispatcher: public xdaq::Application
#endif
{

    public:
        std::string logsMemory[LOGARRAYLINES][LOGARRAYROWS];
        std::string copyOfLogsMemory[LOGARRAYLINES][LOGARRAYROWS];
        int arrayliner;
        int copyOfArrayliner;
        bool processingSortRequest;

        bool sortLognumbersGrowing;
        bool sortMessagesGrowing;
        bool sortOriginGrowing;
        bool sortTimestampGrowing;
        bool sortLevelGrowing;
        bool sortErrcodeGrowing;
        bool sortFaultstateGrowing;
        bool sortSysidGrowing;
        bool sortSubsysidGrowing;
        bool sortEdtimestampGrowing;

        unsigned long logcounter;
        unsigned long fileLogsCounter;
/*        unsigned long dbFileLogsCounter; */
		//ERRDBRELATED
        unsigned long fecLogCounter;
        unsigned long fedLogCounter;
        unsigned long otherLogCounter;

        #ifdef ED_FORCE_FILE_LOGGING
            FILE * forcedLogsFile;
            bool logsCanBeForced;
            std::string forcedLogsFileName;
        #endif

    	FILE * fileOut_;
		bool fileOutIsOpen_;


    	FILE * fileOutPixelsXml_;
		bool fileOutPixelsXmlIsOpen_;
		bool fileOutPixelsXmlUsable_;
		std::string pixelsXmlLogFileName_;

    	FILE * fileOutPixelsLog4j_;
		bool fileOutPixelsLog4jIsOpen_;
		bool fileOutPixelsLog4jUsable_;
		std::string pixelsLog4jLogFileName_;



/*
    	FILE * dbFileOut_;
		bool dbFileOutIsOpen_;
*/
		bool reconfRunning__;

	
      /* Pointer to FSM object */
        FsmWithStateName fsm_;

        /* used to know which HTML page must be displayed when Default() method is called */
        int internalState_;

        /* Declare pointer to diagnostic system object*/
        DiagBagWizard * diagService_;


/*
		//ERRDBRELATED
		//I/O XML file management
		FILE * xmlFecOutputFile_;
		FILE * xmlFedOutputFile_;
		FILE * xmlOtherOutputFile_;
		bool xmlFecOutputFileFirstOpen;
		bool xmlFedOutputFileFirstOpen;
		bool xmlOtherOutputFileFirstOpen;
		//Run start timestamp management
		struct timeval runTimeStamptv;
		struct tm * runTimeStampSplittedPtr;
		char runTimeStamp[100];
*/

bool isRunningForPixels_;



		std::string logFilePathAndPrefix_;



        /* define factory method for instantion of this application */
        XDAQ_INSTANTIATOR();


        LocalErrorDispatcher(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception);
		~LocalErrorDispatcher();



        /* Returns the HTML page displayed from the Default() method when the <default_page> link is clicked */
        void getDefault1Content(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);


        /* Returns the HTML page displayed from the Default() method when the <access_fsm> link is clicked */
        void getFsmStatePage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);


        /* Returns the HTML page displayed from the Default() method when the <default_page> link is clicked */
        void getVisualizeLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);


        /* xgi method called when the link <default_page> is clicked */
        void Default1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        /* xgi method called when the link <display_diagsystem> is clicked */
        void callDiagSystemPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        /* xgi method called when the link <display_fsm> is clicked */
        void callFsmPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        /* xgi method called when the link <display_logs> is clicked */
        void callVisualizeLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);


        void genericSort(int rowOffset, bool sortGrowing);


        void sortLognumbers(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        /* xgi method called when the link <default_page> is clicked */
        void sortMessages(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        void sortOrigin(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        void sortTimestamp(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        void sortLevel(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        void sortErrcode(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        void sortFaultstate(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        void sortSysid(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        void sortSubsysid(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        void sortEdtimestamp(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);


        void getIFrame(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);


        /* Displays the available HyperDaq links for this process */
        void displayLinks(xgi::Input * in, xgi::Output * out);


        /* Default() hyperDaq method */
        void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);




        /* xgi methods, defined as macros, needed for running a diagService in this application */
        void DIAG_CONFIGURE_CALLBACK();
        void DIAG_APPLY_CALLBACK();




        /* converts DiagSystem loglevel format into OWN loglevel format */
        long getLogLevelToLong(std::string logLevel);





        /* Processes incoming LOG messages.
        Routes the messages to default appender and/or console and/or chainsaw and/or file.
        Routes the messages to DiagLevelOne.
        DO NOT USE LOG4C CALLS IN THIS METHOD, IT IS NOT REENTRANT */
        xoap::MessageReference receivedLog (xoap::MessageReference msg) throw (xoap::exception::Exception);



        /* Turns the LocalErrorDispatcher into state INITIALISED (FSM point of view) */
        void configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);



        /* Turns the LocalErrorDispatcher into state HALTED (FSM point of view) */
        void haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
   

   
        /* Allows the FSM to be controlled via a soap message carrying a "fireEvent" message */
        xoap::MessageReference fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception);


        /* Called when a state transition fails in the FSM */
        void failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);


        /* Can be called via the <configure_FSM> hyperdaq HTML page of this process */
        void configureStateMachine(xgi::Input * in, xgi::Output * out);


        /* Can be called via the <configure_FSM> hyperdaq HTML page of this process */
        void stopStateMachine(xgi::Input * in, xgi::Output * out);

        #ifdef AUTO_CONFIGURE_PROCESSES
            void timeExpired (toolbox::task::TimerEvent& e);
        #endif






        /* Read the SOAP message and create a CLog structure
        DO NOT USE LOG4C CALLS IN THIS METHOD, IT IS NOT REENTRANT */
        void getMessage(xoap::MessageReference m, CLog * l) throw (xoap::exception::Exception);

void checkIfReconfigurationIsNeeded(CLog * l) throw (xoap::exception::Exception);


        xoap::MessageReference freeReconfCmd (xoap::MessageReference msg) throw (xoap::exception::Exception);


        std::string getDateTagForFile(void);
	xoap::MessageReference getCurrentLogFileName(xoap::MessageReference msg) throw (xoap::exception::Exception);

};

#endif
#endif
