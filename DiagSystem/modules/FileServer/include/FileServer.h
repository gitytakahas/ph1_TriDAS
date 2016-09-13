/*
   FileName : 		FileServer.h

   Content : 		FileServer module

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

#ifndef _FileServer_h_
#define _FileServer_h_

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


//Use L.M. gwt-interface applet
#ifdef WILL_USE_GWT_FIREITEMS_VIEWER
	#include "tools/standalone/jsinterface.h"
#endif


#include <diagbag/DiagBagWizard.h>
#include "FileServerV.h"




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

#include "trackerMachinesVsPartition.h"




#define LOGARRAYLINES   100
#define LOGARRAYROWS    10

#define LNOFFSET       0
#define MSGOFFSET       1
#define ORGOFFSET       2
#define DIAG_GED_TSOFFSET        3
#define LVLOFFSET       4
#define ECOFFSET        5
#define FSOFFSET        6
#define SIDOFFSET       7
#define SSIDOFFSET      8
#define EDDIAG_GED_TSOFFSET      9

#define LOGFILENAMEPREFIX	"errorsLogFile--GED--"
#define LOGFILENAMESUFFIX	".xml"

#define PIXELSXMLEXTENSION	"pixelsXml"
#define TRACKERXMLEXTENSION	"trackerXml"
#define GENERICXMLEXTENSION	"genericXml"



#ifdef WILL_COMPILE_FOR_TRACKER
	#define STANDARDLOGFILENAMETAG	"SystemWide--"
	#define DCULOGFILENAMETAG	"DCUerrors--"
	#define FEDDEBUGLOGFILENAMETAG	"FEDdebug--"
	#define FEDTMONLOGFILENAMETAG	"FEDtmon--"
	#define FEDSPYLOGFILENAMETAG	"FEDspy--"
#else
	#define STANDARDLOGFILENAMETAG	""
#endif


#include <iostream>
#include <fstream>


#define DEFAULT_LOGGING_REPOSITORY "/tmp/"



#define IS_STANDARD_FILE	true
#define IS_TK_SPECIFIC_FILE	false


//If we want to use the hardware map given by FEC & DB
#ifdef DATABASE
	#include "FecExceptionHandler.h" 
	#include "TkDiagErrorAnalyser.h" 
#endif



/* Version definition for DiagSentinelErrorsGrabber */
//#define FILESERVER_PACKAGE_VERSION "5.0"




class FileServer: public xdaq::Application
{

/*
const std::string versions =  FileServer_PACKAGE_VERSION;
const std::string description = "FileServer: Process used to catch, manage and route logs";
const std::string link = "https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem";
const std::string authors  =  "Laurent GROSS - laurent.gross@cern.ch";
const std::string summary  =  "";
*/


    public:
 

        std::string logsMemory[LOGARRAYLINES][LOGARRAYROWS];
        std::string copyOfLogsMemory[LOGARRAYLINES][LOGARRAYROWS];
        int arrayliner;
        int copyOfArrayliner;
        bool processingSortRequest;


		std::string xmlFileNameSpecificExtension_;


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


        unsigned long dumpedlogcounter;
        unsigned long logcounter;
        unsigned long fileLogsCounter;
        unsigned long pixelsFileLogsCounter;
        unsigned long log4jFileLogsCounter;

		//ERRDBRELATED
        unsigned long fecLogCounter;
        unsigned long fedLogCounter;
        unsigned long otherLogCounter;

//P5MOD
#ifdef WILL_COMPILE_FOR_TRACKER
        unsigned long dcuErrorLogCounter;
        unsigned long fedDebugLogCounter;
        unsigned long fedTmonLogCounter;
        unsigned long fedSpyLogCounter;

        unsigned long dcuErrorFileLogCounter;
        unsigned long fedDebugFileLogCounter;
        unsigned long fedTmonFileLogCounter;
        unsigned long fedSpyFileLogCounter;

        unsigned long dcuDumpedlogcounter;
        unsigned long fedDebugDumpedlogcounter;
        unsigned long fedTmonDumpedlogcounter;
        unsigned long fedSpyDumpedlogcounter;
#endif
//P5MODEND





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

//P5MOD
#ifdef WILL_COMPILE_FOR_TRACKER
    	FILE * dcuFileOut_;
		bool dcuFileOutIsOpen_;

    	FILE * fedDebugFileOut_;
		bool fedDebugFileOutIsOpen_;

    	FILE * fedTmonFileOut_;
		bool fedTmonFileOutIsOpen_;

    	FILE * fedSpyFileOut_;
		bool fedSpyFileOutIsOpen_;

#endif
//P5MODEND



		bool reconfRunning__;

	
      /* Pointer to FSM object */
        FsmWithStateName fsm_;

        /* used to know which HTML page must be displayed when Default() method is called */
        int internalState_;

        /* Declare pointer to diagnostic system object*/
        DiagBagWizard * diagService_;



bool isRunningForPixels_;



		std::string logFilePathAndPrefix_;



        /* define factory method for instantion of this application */
        XDAQ_INSTANTIATOR();


        FileServer(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception);
		~FileServer();



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



	/* Replies to specific request by sending back the current runnumber */
        xoap::MessageReference runNumberRequest(xoap::MessageReference msg) throw (xoap::exception::Exception);



        /* Turns the FileServer into state INITIALISED (FSM point of view) */
        void configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);



        /* Turns the FileServer into state HALTED (FSM point of view) */
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


        void processReceivedLog (CLog l);


		toolbox::task::Timer * timer_;
		toolbox::TimeInterval interval_;
		toolbox::TimeVal start_;
		std::stringstream timerName_;

		toolbox::BSem * RCMSMutex_;

		std::vector<CLog> RcmsClogList_;
		std::vector<int> RcmsStreamNumList_;


		void readCsEnvSettings (void);
//		void readSubDetectorTypeEnvSettings (void);




			//DB conf related
			std::string partitionName;
			char* readEnv;
			std::string DbPartitionName_;

		#ifdef DATABASE
			//Used to archieve DB parameters, in case of failure
			std::string DbLogin_, DbPasswd_, DbPath_;
			
			//BoolState ; Is DB usable or not?
			bool isDbUsable;

			
			//FEC MAP
			TkDiagErrorAnalyser *tkDiagErrorAnalyser;
			TkDiagErrorAnalyser *tkTibDiagErrorAnalyser;
			TkDiagErrorAnalyser *tkTobDiagErrorAnalyser;
			TkDiagErrorAnalyser *tkTecpDiagErrorAnalyser;
			TkDiagErrorAnalyser *tkTecmDiagErrorAnalyser;

	        void findCrateSlotVsHardwareId(CLog *l);
		#endif

		void writeMessageToFile (FILE *f, CLog* l, unsigned long fileLogsCounter);


		bool isFirstTimerTick_;




std::ofstream outfile;

//P5MOD
#ifdef WILL_COMPILE_FOR_TRACKER
std::ofstream dcuOutfile;
std::ofstream fedDebugOutfile;
std::ofstream fedTmonfile;
std::ofstream fedSpyFile;
#endif
//P5MODEND


//std::string stringToBuild_;


#define DIAG_MAX_PARTITIONS_NBR	4
std::string DbPartitionNameArray_[DIAG_MAX_PARTITIONS_NBR];
int part_counter_;


std::string tibPartitionName_;
std::string tobPartitionName_;
std::string tecpPartitionName_;
std::string tecmPartitionName_;


bool isRunningOnP5Machines_;


void doInstanciationExtraBehaviour(void);

//P5MOD
#ifdef WILL_COMPILE_FOR_TRACKER
	bool dcuLogFileNameHasChanged_;
	std::string dcuOldLogFileName_;
    std::string dcuLogFileName_;

	bool fedDebugLogFileNameHasChanged_;
	std::string fedDebugOldLogFileName_;
    std::string fedDebugLogFileName_;

	bool fedTmonLogFileNameHasChanged_;
	std::string fedTmonOldLogFileName_;
    std::string fedTmonLogFileName_;

	bool fedSpyLogFileNameHasChanged_;
	std::string fedSpyOldLogFileName_;
    std::string fedSpyLogFileName_;
#endif
//P5MODEND

//P5MODBIS
bool runNumberIsInitialized_;
 xdata::Integer oldRunNumber_;
// xdata::Integer currentRunNumber_;

bool rnHasFlippedForStandardLogFile;
#ifdef WILL_COMPILE_FOR_TRACKER
	bool rnHasFlippedForDcuLogFile;
	bool rnHasFlippedForFedDebugLogFile;
	bool rnHasFlippedForFedTmonLogFile;
	bool rnHasFlippedForFedSpyLogFile;
#endif
//P5MODBISEND 


//P5MOD
	bool standardFileLoggingUsable_;

#ifdef WILL_COMPILE_FOR_TRACKER
	bool dcuFileLoggingUsable_;
	bool fedDebugFileLoggingUsable_;
	bool fedTmonFileLoggingUsable_;
	bool fedSpyFileLoggingUsable_;
#endif
//P5MODEND





bool openLogFile (	std::string typeOfFile,
					bool * fileChangedNotifier,
					bool * fileisUsable,
					bool * fileIsopen,
					std::string path,
					std::string prefix,
					std::string tag,
					std::string procinfo,
					std::string extension,
					std::string suffix,
					std::ofstream & outfile,
					FILE * filePointer);


std::string getCleanUidName (void);

void openStandardLoggingFile (void);
#ifdef WILL_COMPILE_FOR_TRACKER

void openDcuErrorsLoggingFile (void);
void openFedDebugLoggingFile (void);
void openFedTmonLoggingFile (void);
void openFedSpyLoggingFile (void);
#endif


bool logFileNameHasFlipped;
#ifdef WILL_COMPILE_FOR_TRACKER

bool dcuLogFileNameHasFlipped;
bool fedDebugLogFileNameHasFlipped;
bool fedTempMonLogFileNameHasFlipped;
bool fedSpyLogFileNameHasFlipped;
#endif




std::string previousRepository_;
std::string currentRepository_;

std::vector<std::string> repositoriesList_;
std::vector<std::string> filesList_;


void getDiskContent(std::string activeRepository="");
void displayDiskContent(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
std::string extractParameterValueFromURL(xgi::Input * in, std::string parameter);


        /* Returns the HTML page displayed from the Default() method when the <default_page> link is clicked */
        void getRepositoryContentPage(xgi::Input * in, xgi::Output * out, std::string repositoryToParse ) throw (xgi::exception::Exception);


        /* xgi method called when the link <default_page> is clicked */
        void callParseDiskPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

};

#endif
#endif

									/*
					#ifdef WILL_COMPILE_FOR_TRACKER
						rnHasFlippedForDcuLogFile = true;
						rnHasFlippedForFedDebugLogFile = true;
						rnHasFlippedForFedTmonLogFile = true;
*/
















































