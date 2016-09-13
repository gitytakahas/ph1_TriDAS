/*
   FileName : 		AjaxLogReaderWithFileServer.h

   Content : 		AjaxLogReaderWithFileServer module

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

#ifndef _AjaxLogReaderWithFileServer_h_
#define _AjaxLogReaderWithFileServer_h_

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


#include "AjaxLogReaderWithFileServerV.h"
#include <diagbag/JSONItem.h>
#include <diagbag/JSONFileItem.h>
#include <diagbag/JSONUtils.h>


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


#include "AppletsUidManagement.h"




#ifdef AUTO_CONFIGURE_PROCESSES
	#ifdef WILL_USE_GWT_FIREITEMS_VIEWER
		class AjaxLogReaderWithFileServer: public xdaq::Application, public toolbox::task::TimerListener, public JsInterface
	#else
		class AjaxLogReaderWithFileServer: public xdaq::Application, public toolbox::task::TimerListener
	#endif
#else
	#ifdef WILL_USE_GWT_FIREITEMS_VIEWER
		class AjaxLogReaderWithFileServer: public xdaq::Application, public JsInterface
	#else
		class AjaxLogReaderWithFileServer: public xdaq::Application
	#endif
#endif
{

    public:

		std::vector<JSONItem> jsonItemsList_;

		std::vector<JSONFileItem> jsonFilesItemsList_;


		//long filePos_;

        //unsigned long logcounter;

		//unsigned long lastLogReadInFile_;


		bool allowSendTraceLevels_;
		bool allowSendDebugLevels_;
		bool allowSendInfoLevels_;
		bool allowSendWarnLevels_;
		bool allowSendUserinfoLevels_;
		bool allowSendErrorLevels_;
		bool allowSendFatalLevels_;


//int logsBurstLength_;



    	//FILE * fileOut_;
	//	bool fileoutIsOpen_;
	
	
      	// Pointer to FSM object
        FsmWithStateName fsm_;

        // used to know which HTML page must be displayed when Default() method is called
        int internalState_;

        // Declare pointer to diagnostic system object
        DiagBagWizard * diagService_;

        // define factory method for instantion of this application
        XDAQ_INSTANTIATOR();





        AjaxLogReaderWithFileServer(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception);

	~AjaxLogReaderWithFileServer();


        // Returns homepage ; choose auto-refresh rate
        void getDefault1Content(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        // Returns FSM state page
        void getFsmStatePage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        // Returns Logs list page
        void getVisualizeLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);





        //Caller methods
        void Default1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void callDiagSystemPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void callFsmPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);




        //Show links at page top
        void displayLinks(xgi::Input * in, xgi::Output * out);

        // Default call
        void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);


        //Diag related callbacks
        void DIAG_CONFIGURE_CALLBACK();
        void DIAG_APPLY_CALLBACK();


        // Turns the AjaxLogReaderWithFileServer into state INITIALISED (FSM point of view)
        void configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);

        // Turns the AjaxLogReaderWithFileServer into state HALTED (FSM point of view)
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


		void readLogsFromFile(int command, AppletsUidManagement * appletUidPointer);

		std::string getTag(std::string& rawField);


		void getJSONDataList(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

		void getJsJSONDataList(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

		void getJSONConnectionParam(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

		void getJsJSONConnectionParam(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

		std::string extractParameterValueFromURL(xgi::Input * in, std::string parameter);

        int processRequestParameters(xgi::Input * in, std::string & appletUidPtr, int & appletlogsBurstLengthPtr, int & limitReadToLastNLogsPtr);
        int processRequestParameters(xgi::Input * in, std::string & appletUidPtr, int & appletlogsBurstLengthPtr, int & limitReadToLastNLogsPtr,
					std::string & logFileNameToUSeForApplet, std::string & repositoryNameToUseForApplet, bool & appletIsWorkingWithOnlineFile,
					bool & setAppletRepository, bool & setAppletLogFile, bool & toggleFileChange);

/*
std::string logFileNameToUSeForApplet_;
std::string repositoryNameToUseForApplet_;
//bool appletIsWorkingWithOnlineFile_;
*/
void cleanupMsgString(std::string * msg);

std::vector<AppletsUidManagement *> AppletsUidManagementList_;

toolbox::BSem * handleIncomingJsonConnection_;




//FILTERING-BEGIN
bool allowFedDebugDisplay_;
bool allowDcuAccessErrorsDisplay_;
bool allowDcuWorkloopFullWarnDisplay_;




void viewFedDebugPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
void viewDcuWorkLoopFullWarnPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
void viewDcuAccessErrorPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);


//FILTERING-GWT
bool validateLogForDisplay_;



//Messages filtering part
#define FED_MONITORLOG_PATTERN	"*** Fed Monitor Log ***"
#define FEC_DCUREADOUTERROR_PATTERN	"Error during the upload of the DCUs"
#define FEC_WORKLOOPERROR_PATTERN	"Failed to submit, waitingWorkLoop queue ful"

#include <fstream>

//FILTERING-END



/* ADDONS FOR FILES PARSING/READING */

std::string previousRepository_;
std::string currentRepository_;

std::vector<std::string> repositoriesList_;
std::vector<std::string> filesList_;


void getDiskContent(std::string activeRepository="/tmp", std::string callingAppletUid="000000");
void displayDiskContent(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
//std::string extractParamValueFromURL(xgi::Input * in, std::string parameter);

/* Returns the HTML page displayed from the Default() method when the <default_page> link is clicked */
void getRepositoryContentPage(xgi::Input * in, xgi::Output * out, std::string repositoryToParse ) throw (xgi::exception::Exception);


/* xgi method called when the link <default_page> is clicked */
void callParseDiskPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);



//std::string logFileNameToUSeForApplet_;
//std::string repositoryNameToUseForApplet_;
//bool appletIsWorkingWithOnlineFile_;


//bool fileSelectionOngoing_;
//std::string appletUIDSelectingFile_;
//AppletsUidManagement * pointerToAppSelectingFile_;


//std::string currentAppletUID_;


void getJSONDiskTree(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
void getJsJSONDiskTree(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);


void castDiskContentToJsonArray(void);
//void castDiskContentToJsonArray(std::string & previousRepository, std::string & currentRepository, std::vector<std::string> & repositoriesList, std::vector<std::string> & filesList);



//bool oneLogfileHasBeenSelected_;

};

#endif
#endif























