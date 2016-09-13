/*
   FileName : 		LogReader.h

   Content : 		LogReader module

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



#ifndef _LogReader_h_
#define _LogReader_h_

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


#include "LogReaderV.h"


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


//How many lines will we have in our errors list
#define LOGARRAYLINES   200
//How many rows do we need in our errors list
#define LOGARRAYROWS    5
//Which rows ?
#define MSGOFFSET       0
#define ORGOFFSET       1
#define DIAGTSOFFSET    2
#define LVLOFFSET       3
#define ECOFFSET        4


//What applications will we detect ?
std::string appsUsingDiag[] = {	"CrateController",
								"DcuFilter",
				    	    	"FecSupervisor",
								"DataBaseCache",
								"DS",
								"Fed9USupervisor",
								"TrackerSupervisor",
				     	    	"DummySupervisor",
				     	    	"ENDOFLIST"};
//Give list length, minus the terminating ENDOFLIST
#define NUMBER_OF_APPS_LISTED	8
int appsUsingDiagDetected[NUMBER_OF_APPS_LISTED];



//If we want to use the hardware map given by FEC & DB
#ifdef DATABASE
	#include "FecExceptionHandler.h" 
	#include "TkDiagErrorAnalyser.h" 
	#include "ErrorsChecker.h"
	#include "ErrorsTreeBuilder.h"
#endif


//HTML helpers
#define HTMLTAB "&nbsp;&nbsp;&nbsp;&nbsp;"

//Use L.M. gwt-interface applet
#include "tools/standalone/jsinterface.h"


#ifdef AUTO_CONFIGURE_PROCESSES
    class LogReader: public xdaq::Application, public toolbox::task::TimerListener
#else
    class LogReader: public xdaq::Application
#endif
{

    public:
		std::string oldLogFileName;

        std::string logsMemory[LOGARRAYLINES][LOGARRAYROWS];
        std::string copyOfLogsMemory[LOGARRAYLINES][LOGARRAYROWS];
        int arrayliner;
        int copyOfArrayliner;

        bool processingSortRequest;
        bool sortMessagesGrowing;
        bool sortOriginGrowing;
        bool sortTimestampGrowing;
        bool sortLevelGrowing;
        bool sortErrcodeGrowing;

        unsigned long logcounter;

    	FILE * fileOut_;
		bool fileoutIsOpen_;
	
	
      	// Pointer to FSM object
        FsmWithStateName fsm_;

        // used to know which HTML page must be displayed when Default() method is called
        int internalState_;

        // Declare pointer to diagnostic system object
        DiagBagWizard * diagService_;

        // define factory method for instantion of this application
        XDAQ_INSTANTIATOR();


    	long lastKnownPos;
    	long lastKnownGoodPos;
    	int arrayDesc;
		long currentLogNumber;
		long numberOfLogsToBypass;


    	bool newLogsFound;

		bool autoRefreshEnabled_;
		int refreshRate_;
		std::string textRefreshRate_;

		//find APPS
		bool mustDisplayOneAppDiagSettings_;
		std::string oneAppDiagSettingsToDisplay_;

		//find APPS GROUPS
		bool mustDisplayGroupAppDiagSettings_;
		std::string groupAppDiagSettingsToDisplay_;

		//find APPS WORLDWIDE
		bool mustDisplayWorldAppDiagSettings_;
		std::string worldAppDiagSettingsToDisplay_;


        LogReader(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception);

		~LogReader();


        // Returns homepage ; choose auto-refresh rate
        void getDefault1Content(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        // Returns FSM state page
        void getFsmStatePage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        // Returns Logs list page
        void getVisualizeLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);



		// Single application log levels management callbacks
		void seeAppsDiagParamsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
		void getAppsPanelPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
		void cancelAppLevelsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
		void acceptAppLevelsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);


		// Grouped application log levels management callbacks
		void seeGroupDiagParamsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
		void getGroupsPanelPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
		void cancelGroupLevelsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
		void acceptGroupLevelsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);


		//WorldWide application log levels management callbacks
		void seeWorldDiagParamsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
		void getWorldPanelPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
		void cancelWorldLevelsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
		void acceptWorldLevelsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);




        //Caller methods
        void Default1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void callDiagSystemPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void callFsmPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void callVisualizeLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void callVisualizePreviousLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void callVisualizeNextLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void closeFile(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void callAppsSpyingPanelPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void callGroupSpyingPanelPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void callWorldSpyingPanelPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);



		//Sort logs before display on list
        void genericSort(int rowOffset, bool sortGrowing);
        void sortMessages(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void sortOrigin(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void sortTimestamp(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void sortLevel(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
        void sortErrcode(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

		//Set HTML tags in page header for auto-refresh
        void autoRefresh(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

		//Build logs list HTML block
        void getIFrame(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

        //Show links at page top
        void displayLinks(xgi::Input * in, xgi::Output * out);

        // Default call
        void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);


        //Diag related callbacks
        void DIAG_CONFIGURE_CALLBACK();
        void DIAG_APPLY_CALLBACK();


        // Turns the LogReader into state INITIALISED (FSM point of view)
        void configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception);

        // Turns the LogReader into state HALTED (FSM point of view)
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

		//Read log file and mount it in memory
        void readLogsFromFile(int whereFrom);



std::string simulatedFecHardId_;
std::string simulatedFecRing_;
std::string simulatedCcuAddress_;
std::string simulatedI2cChannel_;
std::string simulatedI2cAddress_;

std::string simulatedFedId_;
std::string simulatedFedChannel_;

std::string simulatedPsuId_;




		#ifdef DATABASE



			//Used to archieve DB parameters, in case of failure
			std::string DbLogin_, DbPasswd_, DbPath_, DbPartitionName_;
			
			//BoolState ; Is DB usable or not?
			bool isDbUsable;

			//DB conf related
			std::string partitionName;
			char* readEnv;
			
			//FEC MAP
			TkDiagErrorAnalyser *tkDiagErrorAnalyser ;

			//HELPER CLASS
			ErrorsChecker *errorsChecker ;


			//HELPER CLASS
			ErrorsTreeBuilder *errorsTreeBuilder ;

			bool displayNonFaultyItems_;

			//Expander for Detector->FEC,FEC,PSU display
			bool subDetectorShowDetails_;

			//Expander for FEC->FEC crates display
			bool allFecsShowDetails_;
			//Expander for FEC Crate->FEC slots display
			bool fecCratesShowDetails_[NUMBER_OF_FEC_CRATES];
			//Expander for FEC slot->FEC rings display
			bool fecSlotItemShowDetails_[NUMBER_OF_FEC_CRATES][NUMBER_OF_FEC_SLOTS];
			//Expander for FEC ring->FEC CCU display
			bool fecRingItemShowDetails_[NUMBER_OF_FEC_CRATES][NUMBER_OF_FEC_SLOTS][NUMBER_OF_FEC_RING];
			//Expander for FEC CCU->FEC I2C channel display
			bool fecCcuItemShowDetails_[NUMBER_OF_FEC_CRATES][NUMBER_OF_FEC_SLOTS][NUMBER_OF_FEC_RING][NUMBER_OF_FEC_CCU];
			bool fecChannelItemShowDetails_[NUMBER_OF_FEC_CRATES][NUMBER_OF_FEC_SLOTS][NUMBER_OF_FEC_RING][NUMBER_OF_FEC_CCU][NUMBER_OF_FEC_CHANNEL];

			//Internals for FEC errors detector management
			bool fecCratesList[NUMBER_OF_FEC_CRATES];
			bool fecSlotsList[NUMBER_OF_FEC_SLOTS];
			bool fecRingList[NUMBER_OF_FEC_RING];
			bool fecCcuList[NUMBER_OF_FEC_CCU];
			bool fecChannelList[NUMBER_OF_FEC_CHANNEL];
			bool fecI2cAddrList[NUMBER_OF_FEC_I2CADDR];

			//Expander for FED->FED crates display
			bool allFedsShowDetails_;
			//Expander for FED Crate->FED slots display
			bool fedCratesShowDetails_[NUMBER_OF_FED_CRATES];
			//Expander for FED slot->FED connection display	
			bool fedSlotItemShowDetails_[NUMBER_OF_FED_CRATES][NUMBER_OF_FED_SLOTS];

			//Internals for FEC errors detector management
			bool fedCratesList[NUMBER_OF_FED_CRATES];
			bool fedSlotsList[NUMBER_OF_FED_SLOTS];
			bool fedChannelsList[NUMBER_OF_FED_CHANNELS];


			//Expander for PSU->PSU crates display
			bool allPsusShowDetails_;



			// Close/Expand overall detector errors view
			void switchDetectorView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

			// Close/Expand FECs->FEC crate errors view
			void switchAllFecsView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
			// Close/Expand FEC crate->FEC slot errors view
			void switchFecCratesView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
			// Close/Expand FEC slot->FEC ring errors view
			void switchFecSlotsView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
			// Close/Expand FEC ring->FEC ccu errors view
			void switchFecRingsView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
			// Close/Expand FEC ccu->FEC i2c channel errors view
			void switchFecCcusView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
			// Close/Expand FEC i2c channel->FEC i2c address errors view
			void switchFecChannelsView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

			// Close/Expand FEDs->FED crate errors view
			void switchAllFedsView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
			// Close/Expand FED crate->FED slot errors view
			void switchFedCratesView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
			// Close/Expand FED slot->FED connection errors view
			void switchFedSlotsView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

			// Close/Expand PSUs->PSU crate errors view
			void switchAllPsusView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

			//Extract FEC,FED and PSU informations from URL when a link is clicked
			int getFecCrateNumber(xgi::Input * in);
			int getFecSlotNumber(xgi::Input * in);
			int getFecRingNumber(xgi::Input * in);
			int getFecCcuNumber(xgi::Input * in);
			int getFecChannelNumber(xgi::Input * in);
			int getFedCrateNumber(xgi::Input * in);
			int getFedSlotNumber(xgi::Input * in);

			//Show/Hide non faulty elements in structure
			void switchHiddenItems(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);



			void simulateFecError(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
			void simulateFedError(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
			void simulatePsuError(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
			void displayDetIdErrors(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
			void displayFecErrors(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

			//Count all FEC, FED & PSU errors present on the tracker
			void setTrackerCounters(unsigned int & trackerFecErrorsCounter, unsigned int & trackerFedErrorsCounter, unsigned int & trackerPsuErrorsCounter);

			//Call Logs analysis structured page
	        void getLogsSummaryPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
	        void getLogsNoDbPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
	        void callLogsSummaryPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);
	        void callLogsNoDbPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

		#endif

};

#endif
#endif
