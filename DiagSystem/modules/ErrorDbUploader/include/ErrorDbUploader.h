/*
   FileName : 		ErrorDbUploader.h

   Content : 		ErrorDbUploader module

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

#ifndef _ErrorDbUploader_h_
#define _ErrorDbUploader_h_

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



//Use L.M. gwt-interface applet
#ifdef WILL_USE_GWT_FIREITEMS_VIEWER
	#include "tools/standalone/jsinterface.h"
#endif

#include "DiagCompileOptions.h"
#include <diagbag/DiagBagWizard.h>


#include "ErrorDbUploaderV.h"


// needed for autoconfiguration
#ifdef AUTO_CONFIGURE_PROCESSES
    #include "toolbox/task/Timer.h"
    #include "toolbox/task/TimerFactory.h"
    #include "toolbox/TimeInterval.h"
#endif

// needed for FSM
#include "tools/standalone/FsmWithStateName.h"
#include "toolbox/fsm/FailedEvent.h"
#define EXECTRANSHC "Configure"
#define EXECTRANSCH "Halt"



//Copied from TestDiagUploadData package (FecSoftware Thirdparty)

#ifdef WILL_USE_TRACKER_ERRDB
	#include "FecExceptionHandler.h"    // exceptions
	#include "DbTkDcuInfoAccess.h"
	#include <fstream>
#endif



//#define WILL_USE_PIXELS_ERRDB
//#define WILL_USE_TRACKER_ERRDB


#define ZIPCOMMAND "/usr/bin/zip"



#define ERRDBUPLOADER_TIMESTAMP_CORRECT_LENGTH 31



#ifdef AUTO_CONFIGURE_PROCESSES
	#ifdef WILL_USE_GWT_FIREITEMS_VIEWER
		class ErrorDbUploader: public xdaq::Application, public toolbox::task::TimerListener, public JsInterface
	#else
		class ErrorDbUploader: public xdaq::Application, public toolbox::task::TimerListener
	#endif
#else
	#ifdef WILL_USE_GWT_FIREITEMS_VIEWER
		class ErrorDbUploader: public xdaq::Application, public JsInterface
	#else
		class ErrorDbUploader: public xdaq::Application
	#endif
#endif
{

    public:

		toolbox::task::Timer * timer_;
		time_t errDbTimerHalfRate_;

		bool isFirstTimerTick_;

		long filePos_;

        //unsigned long logcounter;





		#ifdef WILL_USE_PIXELS_ERRDB
			std::string errDbPixelsFileNameNumericPart_;
			std::string errDbPixelsXmlFileName_;
			std::string errDbPixelsZipFileName_;

			std::string path_to_errdb_credentials_;
			std::string errdb_scptools_param_;
			bool pixelsErrDbIsUsable_;
		#endif

		#ifdef WILL_USE_TRACKER_ERRDB
			std::string errDbTrackerFileNameNumericPart_;
			std::string errDbTrackerXmlFileName_;

			std::string tracker_conf_db_;
			bool trackerErrDbIsUsable_;

		#endif

    	FILE * fileIn_;
		bool fileinIsOpen_;
	
		#ifdef WILL_USE_PIXELS_ERRDB
	    	FILE * pixelsFileOut_;
			bool pixelsFileoutIsOpen_;
		#endif

		#ifdef WILL_USE_TRACKER_ERRDB
	    	FILE * trackerFileOut_;
			bool trackerFileoutIsOpen_;
		#endif
	
      	// Pointer to FSM object
        FsmWithStateName fsm_;

        // used to know which HTML page must be displayed when Default() method is called
        int internalState_;

        // Declare pointer to diagnostic system object
        DiagBagWizard * diagService_;

        // define factory method for instantion of this application
        XDAQ_INSTANTIATOR();





        ErrorDbUploader(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception);

	~ErrorDbUploader();


        // Returns homepage ; choose auto-refresh rate
        void getDefault1Content(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        // Returns FSM state page
        void getFsmStatePage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        // Returns Logs list page
        void getVisualizeLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        // Returns Logs list page
        void getForceWriteLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);




        //Caller methods
        void Default1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void callDiagSystemPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void callFsmPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void callWriteLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);




        //Show links at page top
        void displayLinks(xgi::Input * in, xgi::Output * out);

        // Default call
        void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);


        //Diag related callbacks
        void DIAG_CONFIGURE_CALLBACK();
        void DIAG_APPLY_CALLBACK();


        // Turns the ErrorDbUploader into state INITIALISED (FSM point of view)
        void configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);

        // Turns the ErrorDbUploader into state HALTED (FSM point of view)
        void haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);
   
        // Allows the FSM to be controlled via a soap message carrying a "fireEvent" message
        xoap::MessageReference fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception);

        // Called when a state transition fails in the FSM
        void failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);

        // Can be called via the <configure_FSM> hyperdaq HTML page of this process
        void configureStateMachine(xgi::Input * in, xgi::Output * out);

        // Can be called via the <configure_FSM> hyperdaq HTML page of this process
        void stopStateMachine(xgi::Input * in, xgi::Output * out);


		//What do we do at initialisation timer expiration?
        #ifdef AUTO_CONFIGURE_PROCESSES
            void timeExpired (toolbox::task::TimerEvent& e);
        #endif

		//Cleanup messages in logs (dirty chars, etc...=
		std::string cleanField(std::string& rawField, std::string& tag);


		void readLogsFromFile(int command);

		std::string getTag(std::string& rawField);

		void cleanupMsgString(std::string * msg);

		#ifdef WILL_USE_PIXELS_ERRDB
			#ifdef WILL_COMPILE_FOR_TRACKER
				void writePixelsFileOutHeaderForTracker();
				void writePixelsFileOutFooterForTracker();
			#endif
			#ifdef WILL_COMPILE_FOR_PIXELS
				void writePixelsFileOutHeaderForPixels();
				void writePixelsFileOutFooterForPixels();
			#endif
		#endif

int logsReadFromFile;
bool isInProcessingStage;
int storageTickCount_;

std::string currentRunNumberAsString_;
bool runNumberIsSet_;
//bool isFirstSetOfrunNumber_;


long filePosForFirstNoRunNumberLog_;
bool filePosForFirstNoRunNumberLogIsSetted_;



std::string currentRunNumberFromGedAsString_;

#ifdef WILL_USE_TRACKER_ERRDB
	DbTkDcuInfoAccess * dbAccess_;
#endif


unsigned long successLogsUploadAttempts;
unsigned long failLogsUploadAttempts;

long long summedSuccessfulInsertionTime;

long long maxDbUploadTime, minDbUploadTime;

std::string lastKnownGoodTimestamp;

//Messages filtering part
//Defines moved to DiagCompileOptions.h
/*
#define FED_MONITORLOG_PATTERN	"*** Fed Monitor Log ***"
#define FEC_DCUREADOUTERROR_PATTERN	"Error during the upload of the DCUs"
#define FEC_WORKLOOPERROR_PATTERN	"Failed to submit, waitingWorkLoop queue ful"
*/
#include <fstream>
typedef struct dcuStructure{
std::string crate;
std::string slot;
std::string ring;
std::string ccu;
std::string i2cChannel;
std::string i2cAddress;
long occurences;
};



long storedDcuErrorStructs_;
std::vector<dcuStructure> dcuStructList_;


long fedMonitorLogAmount_;
long dcuStructErrorsLogAmount_;
long fecWorkLoopErrorsLogAmount_;

std::string fecWorkLoopLogPattern;
std::string fedMonitorLogPattern;
std::string dcuStructErrorLogPattern;
//End Messages filtering part


};

#endif
#endif
