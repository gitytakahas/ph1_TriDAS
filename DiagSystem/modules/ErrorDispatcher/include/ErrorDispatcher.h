/*
i   FileName : 		ErrorDispatcher.h

   Content : 		ErrorDispatcher module

   Used in : 		Tracker Diagnostic System

   Programmer : 	Laurent GROSS

   Version : 		DiagSystem 4.0.0

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

#ifndef _ErrorDispatcher_h_
#define _ErrorDispatcher_h_

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

#include "ErrorDispatcherV.h"



/* needed for autoconfiguration */
#ifdef AUTO_CONFIGURE_PROCESSES
    #include "toolbox/task/Timer.h"
    #include "toolbox/task/TimerFactory.h"
    #include "toolbox/TimeInterval.h"
#endif

/* needed for FSM */
#include "tools/standalone/FsmWithStateName.h"
#include "toolbox/fsm/FailedEvent.h"


//Use L.M. gwt-interface applet
#include "tools/standalone/jsinterface.h"

#define EXECTRANSHC "Configure"
#define EXECTRANSCH "Halt"


#include "tools/ErrorsDbRelated/XMLTemplatesForLclEd.hh"


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



//#define ED_FORCE_FILE_LOGGING

#ifdef AUTO_CONFIGURE_PROCESSES
    class ErrorDispatcher: public xdaq::Application, public toolbox::task::TimerListener
#else
    class ErrorDispatcher: public xdaq::Application
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
        /*unsigned long dbFileLogsCounter;*/

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

        /* define factory method for instantion of this application */
        XDAQ_INSTANTIATOR();


        ErrorDispatcher(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): xdaq::Application(s)
        {
	
    	    fileOutIsOpen_ = false;
    	    /*dbFileOutIsOpen_ = false;*/
			reconfRunning__ = false;


			//ERRDBRELATED
			xmlFecOutputFile_ = NULL;
			xmlFecOutputFileFirstOpen = true;
			xmlFedOutputFile_ = NULL;
			xmlFedOutputFileFirstOpen = true;
			xmlOtherOutputFile_ = NULL;
			xmlOtherOutputFileFirstOpen = true;

            /* Give funny and useless informations at load time */
            std::stringstream mmesg;
            mmesg << "Process version " << ERRORDISPATCHER_PACKAGE_VERSION << " loaded";
            this->getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);
            LOG4CPLUS_INFO(this->getApplicationLogger(), mmesg.str());
            this->getApplicationLogger().setLogLevel(ERROR_LOG_LEVEL);

            /* Set Default() metyhod state as : initialising.... */
            internalState_ = 0;

            for (int logarrayinitialiser=0; logarrayinitialiser < LOGARRAYLINES ; logarrayinitialiser++)
            {
                logsMemory[logarrayinitialiser][MSGOFFSET] = "UNINITIALIZED_BT_ED";
                copyOfLogsMemory[logarrayinitialiser][MSGOFFSET] = "UNINITIALIZED_BT_ED";
            }
            arrayliner = 0;
            copyOfArrayliner = 0;
            processingSortRequest = false;
            sortLognumbersGrowing = true;
            sortMessagesGrowing = true;
            sortOriginGrowing = true;
            sortTimestampGrowing = true;
            sortLevelGrowing = true;
            sortErrcodeGrowing = true;
            sortFaultstateGrowing = true;
            sortSysidGrowing = true;
            sortSubsysidGrowing = true;
            sortEdtimestampGrowing = true;

            logcounter = 0;
            fileLogsCounter = 0;
            //dbFileLogsCounter = 0;

            /* Instanciate diagnostic object */
            diagService_ = new DiagBagWizard(
                                    getApplicationDescriptor()->getClassName() + "DiagLvlOne" ,
                                    this->getApplicationLogger(),
                                    getApplicationDescriptor()->getClassName(),
                                    getApplicationDescriptor()->getInstance(),
                                    getApplicationDescriptor()->getLocalId(),
                                    (xdaq::WebApplication *)this );


            /* bind xgi and xoap commands specific to this application */
            xgi::bind(this,&ErrorDispatcher::Default, "Default");
            xgi::bind(this,&ErrorDispatcher::Default1, "Default1");
            xgi::bind(this,&ErrorDispatcher::callDiagSystemPage, "callDiagSystemPage");
            xgi::bind(this,&ErrorDispatcher::callFsmPage, "callFsmPage");
            xgi::bind(this,&ErrorDispatcher::callVisualizeLogsPage, "callVisualizeLogsPage");
            xgi::bind(this,&ErrorDispatcher::getIFrame, "getIFrame");
            xgi::bind(this,&ErrorDispatcher::configureStateMachine, "configureStateMachine");
            xgi::bind(this,&ErrorDispatcher::stopStateMachine, "stopStateMachine");
            xoap::bind(this, &ErrorDispatcher::receivedLog, "receivedLog", XDAQ_NS_URI );

            xoap::bind(this, &ErrorDispatcher::freeReconfCmd, "freeReconfCmd", XDAQ_NS_URI );

            xgi::bind(this,&ErrorDispatcher::sortLognumbers, "sortLognumbers");
            xgi::bind(this,&ErrorDispatcher::sortMessages, "sortMessages");
            xgi::bind(this,&ErrorDispatcher::sortOrigin, "sortOrigin");
            xgi::bind(this,&ErrorDispatcher::sortTimestamp, "sortTimestamp");
            xgi::bind(this,&ErrorDispatcher::sortLevel, "sortLevel");
            xgi::bind(this,&ErrorDispatcher::sortErrcode, "sortErrcode");
            xgi::bind(this,&ErrorDispatcher::sortFaultstate, "sortFaultstate");
            xgi::bind(this,&ErrorDispatcher::sortSysid, "sortSysid");
            xgi::bind(this,&ErrorDispatcher::sortSubsysid, "sortSubsysid");
            xgi::bind(this,&ErrorDispatcher::sortEdtimestamp, "sortEdtimestamp");


            /* bind xgi commands needed for running a diagService in this application */
            xgi::bind(this,&ErrorDispatcher::configureDiagSystem, "configureDiagSystem");
            xgi::bind(this,&ErrorDispatcher::applyConfigureDiagSystem, "applyConfigureDiagSystem");

            /* Call a macro needed for running a diagService in this application */
            DIAG_DECLARE_ED_APP

            /* Liste des etats qu'on va trouver dans la FSM */
            fsm_.addState ('C', "Configured") ;
            fsm_.addState ('H', "Halted");

            /* Methode initialiseAction qui doit gerer les constructions, etc... qui surviennent lors de la
            transition entre l'etat Initial et l'etat Halted */
            fsm_.addStateTransition ('C','H', EXECTRANSCH, this, &ErrorDispatcher::haltAction);
            fsm_.addStateTransition ('H','C', EXECTRANSHC, this, &ErrorDispatcher::configureAction);

            /* Si une transition ne peut se faire, on appel la methode failedTransition */
            fsm_.setFailedStateTransitionAction( this, &ErrorDispatcher::failedTransition );

            /* On synchronise la FASM avec l'etat du process apres load, a savoir : etat HALTED */
            fsm_.setInitialState('H');
            fsm_.reset();

            /* Import/Export the stateName variable */
            getApplicationInfoSpace()->fireItemAvailable("stateName", &fsm_.stateName_);

            /* Bind SOAP callbacks for FSM control messages */
            xoap::bind (this, &ErrorDispatcher::fireEvent, EXECTRANSCH, XDAQ_NS_URI);
            xoap::bind (this, &ErrorDispatcher::fireEvent, EXECTRANSHC, XDAQ_NS_URI);

			//ERRDBRELATED
			//Delet existing file if any
			std::string systemCommand;
			systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
	 		system(systemCommand.c_str());
			systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
			system(systemCommand.c_str());
			systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
			system(systemCommand.c_str());

			//set run start timestamp
			gettimeofday(&runTimeStamptv,NULL);
			runTimeStampSplittedPtr = localtime(&runTimeStamptv.tv_sec);
			snprintf(runTimeStamp, 22, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d",
						(runTimeStampSplittedPtr->tm_year + 1900),
						runTimeStampSplittedPtr->tm_mon,
						runTimeStampSplittedPtr->tm_mday,
						runTimeStampSplittedPtr->tm_hour,
						runTimeStampSplittedPtr->tm_min,
						runTimeStampSplittedPtr->tm_sec);


            #ifdef AUTO_CONFIGURE_PROCESSES
                std::stringstream timerName;
                timerName << getApplicationDescriptor()->getContextDescriptor()->getURL() << ":";
                timerName << getApplicationDescriptor()->getClassName() << ":" << getApplicationDescriptor()->getLocalId() << ":" << getApplicationDescriptor()->getInstance();
                toolbox::task::Timer * timer = toolbox::task::getTimerFactory()->createTimer(timerName.str());
                toolbox::TimeInterval interval(AUTO_ED_CONFIGURE_DELAY,0);
                toolbox::TimeVal start;
                start = toolbox::TimeVal::gettimeofday() + interval;
                timer->schedule( this, start,  0, "" );
            #endif

        }


		~ErrorDispatcher()
		{
    		if (fileOutIsOpen_ == true)
		    {
    			//std::cout << "DEBUG : Closing log file" << std::endl;
    			if (fileOut_ != NULL) fclose(fileOut_);
		    }
/*
    		if (dbFileOutIsOpen_ == true)
		    {
    			//std::cout << "DEBUG : Closing log file" << std::endl;
    			if (dbFileOut_ != NULL) fclose(dbFileOut_);
		    }
*/

			//ERRDBRELATED
			if (xmlFecOutputFileFirstOpen == false)
			{
				CLog l;
				std::string systemCommand;
				std::string fileName;

				/* std::cout << "Opening XML output file for FECs" << std::endl; */
				fileName = (std::string)LOG_FILES_ROOT + (std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
				xmlFecOutputFile_ = l.OpenXmlOutputFile(	fileName.c_str(),
															XML_APPEND_ACESS);
				/* std::cout << "Writing XML output file END_HEADER for FECs" << std::endl; */
				l.WriteXmlOutputFileEndHeader( xmlFecOutputFile_);

				/* std::cout << "Closing XML output file for FECs" << std::endl; */
				l.CloseXmlOutputFile(xmlFecOutputFile_);
/*
				std::cout << "Deleting previously completed Fec xml logfile" << std::endl;
				system(DELETE_COMPLETED_XML_FEC_LOGFILE_NAME);
*/

				/* std::cout << "Moving current Fec xml logfile to dated version" << std::endl; */
/*
				system(MOVE_COMPLETED_XML_FEC_LOGFILE_NAME);
*/
				systemCommand = (std::string)MOVE_COMMAND +
								(std::string)LOG_FILES_ROOT + (std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION +
								(std::string)" " +
								(std::string)LOG_FILES_ROOT + (std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION + (std::string)"." + getDateTagForFile();
				system(systemCommand.c_str());



				/* std::cout << "Deleting Fec xml logfile" << std::endl; */
/*
				system(DELETE_XML_FEC_LOGFILE_NAME);
*/
				systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
				system(systemCommand.c_str());

				xmlFecOutputFileFirstOpen = true;
			}
			if (xmlFedOutputFileFirstOpen == false)
			{
				CLog l;
				std::string systemCommand;
				std::string fileName;

				/* std::cout << "Opening XML output file for FEDs" << std::endl; */
				fileName = (std::string)LOG_FILES_ROOT + (std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
				xmlFedOutputFile_ = l.OpenXmlOutputFile(	fileName.c_str(),
															XML_APPEND_ACESS);

				/* std::cout << "Writing XML output file END_HEADER for FEDs" << std::endl; */
				l.WriteXmlOutputFileEndHeader( xmlFedOutputFile_);

				/* std::cout << "Closing XML output file for FEDs" << std::endl; */
				l.CloseXmlOutputFile(xmlFedOutputFile_);

/*
				std::cout << "Deleting previously completed Fed xml logfile" << std::endl;
				system(DELETE_COMPLETED_XML_FED_LOGFILE_NAME);
*/

				/* std::cout << "Moving current Fed xml logfile to completed version" << std::endl; */
/*
				system(MOVE_COMPLETED_XML_FED_LOGFILE_NAME);
*/
				systemCommand = (std::string)MOVE_COMMAND +
								(std::string)LOG_FILES_ROOT + (std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION +
								(std::string)" " +
								(std::string)LOG_FILES_ROOT + (std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION + (std::string)"." + getDateTagForFile();
				system(systemCommand.c_str());


				/* std::cout << "Deleting Fed xml logfile" << std::endl; */
/*
				system(DELETE_XML_FED_LOGFILE_NAME);
*/
				systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
				system(systemCommand.c_str());


				xmlFedOutputFileFirstOpen = true;
			}
			if (xmlOtherOutputFileFirstOpen == false)
			{
				CLog l;
				std::string systemCommand;
				std::string fileName;

				/* std::cout << "Opening XML output file for OTHERs" << std::endl; */
				fileName = (std::string)LOG_FILES_ROOT + (std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;

				xmlOtherOutputFile_ = l.OpenXmlOutputFile(	fileName.c_str(),
															XML_APPEND_ACESS);

				/* std::cout << "Writing XML output file END_HEADER for OTHERs" << std::endl; */
				l.WriteXmlOutputFileEndHeader( xmlOtherOutputFile_);

				/* std::cout << "Closing XML output file for OTHERs" << std::endl; */
				l.CloseXmlOutputFile(xmlOtherOutputFile_);
/*
				std::cout << "Deleting previously completed Other xml logfile" << std::endl;
				system(DELETE_COMPLETED_XML_OTHER_LOGFILE_NAME);
*/

				/* std::cout << "Moving current Other xml logfile to completed version" << std::endl; */
/*
				system(MOVE_COMPLETED_XML_OTHER_LOGFILE_NAME);
*/
				systemCommand = (std::string)MOVE_COMMAND +
								(std::string)LOG_FILES_ROOT + (std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION +
								(std::string)" " +
								(std::string)LOG_FILES_ROOT + (std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION + (std::string)"." + getDateTagForFile();
				system(systemCommand.c_str());

				/* std::cout << "Deleting Other xml logfile" << std::endl; */
/*
				system(DELETE_XML_OTHER_LOGFILE_NAME);
*/
				systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
				system(systemCommand.c_str());


				xmlOtherOutputFileFirstOpen = true;
			}

		}



        /* Returns the HTML page displayed from the Default() method when the <default_page> link is clicked */
        void getDefault1Content(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            *out << cgicc::p() << std::endl;
            *out << "Error Dispatcher Default page is empty at the moment";
            *out << cgicc::p() << std::endl;
        }


        /* Returns the HTML page displayed from the Default() method when the <access_fsm> link is clicked */
        void getFsmStatePage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            /* Display current state */
            *out << cgicc::p() << std::endl;
            if (fsm_.getCurrentState() == 'H') *out << "Current State is : Halted" << std::endl;
            if (fsm_.getCurrentState() == 'C') *out << "Current State is : Configured" << std::endl;
            if ( (fsm_.getCurrentState() != 'C') && (fsm_.getCurrentState() != 'H') ) *out << "Current State is : DANS LES CHOUX" << std::endl;
            *out << cgicc::p() << std::endl;

            /* Fire the action CONFIGURE */
            std::string urlConfig = "/";
            urlConfig += getApplicationDescriptor()->getURN();
            urlConfig += "/configureStateMachine";	
            *out << cgicc::form().set("method","post").set("action", urlConfig).set("enctype","multipart/form-data") << std::endl;
            *out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Configure State Machine");
            *out << cgicc::p() << std::endl;
            *out << cgicc::form() << std::endl;
            *out << cgicc::p() << std::endl;

            /* Fire the action STOP */
            std::string urlStop = "/";
            urlStop += getApplicationDescriptor()->getURN();
            urlStop += "/stopStateMachine";	
            *out << cgicc::form().set("method","post").set("action", urlStop).set("enctype","multipart/form-data") << std::endl;
            *out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Stop State Machine");
            *out << cgicc::p() << std::endl;
            *out << cgicc::form() << std::endl;
            *out << cgicc::p() << std::endl;
        }

        /* Returns the HTML page displayed from the Default() method when the <default_page> link is clicked */
        void getVisualizeLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
			getIFrame(in, out);

        }



        /* xgi method called when the link <default_page> is clicked */
        void Default1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            internalState_ = 0;
            Default(in, out);
        }

        /* xgi method called when the link <display_diagsystem> is clicked */
        void callDiagSystemPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            internalState_ = 1;
            Default(in, out);
        }

        /* xgi method called when the link <display_fsm> is clicked */
        void callFsmPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            internalState_ = 2;
            Default(in, out);
        }

        /* xgi method called when the link <display_logs> is clicked */
        void callVisualizeLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
	    internalState_ = 3;
            Default(in, out);
        }





        void genericSort(int rowOffset, bool sortGrowing)
        {
            std::string LogNumbers;
            std::string Message;
            std::string Origine;
            std::string Timestamp;
            std::string Level;
            std::string Errcode;
            std::string Faultstate;
            std::string Sysid;
            std::string Subsysid;
            std::string Edtimestamp;

            if (sortGrowing == true)
            /* Tri croissant */
            {
                //sortGrowing = false;
                for (int i=0; i<LOGARRAYLINES; i++)
                {
                    for (int j=1; j<LOGARRAYLINES; j++)
                    {
                        if (copyOfLogsMemory[j-1][rowOffset] > copyOfLogsMemory[j][rowOffset])
                        {
                            /* backup lowest value */

                            LogNumbers = copyOfLogsMemory[j][LNOFFSET];
                            Message = copyOfLogsMemory[j][MSGOFFSET];
                            Origine = copyOfLogsMemory[j][ORGOFFSET];
                            Timestamp = copyOfLogsMemory[j][TSOFFSET];
                            Level = copyOfLogsMemory[j][LVLOFFSET];
                            Errcode = copyOfLogsMemory[j][ECOFFSET];
                            Faultstate = copyOfLogsMemory[j][FSOFFSET];
                            Sysid = copyOfLogsMemory[j][SIDOFFSET];
                            Subsysid = copyOfLogsMemory[j][SSIDOFFSET];
                            Edtimestamp = copyOfLogsMemory[j][EDTSOFFSET];

                            /* permute values */
                            copyOfLogsMemory[j][LNOFFSET] = copyOfLogsMemory[j-1][LNOFFSET];
                            copyOfLogsMemory[j][MSGOFFSET] = copyOfLogsMemory[j-1][MSGOFFSET];
                            copyOfLogsMemory[j][ORGOFFSET] = copyOfLogsMemory[j-1][ORGOFFSET];
                            copyOfLogsMemory[j][TSOFFSET] = copyOfLogsMemory[j-1][TSOFFSET];
                            copyOfLogsMemory[j][LVLOFFSET] = copyOfLogsMemory[j-1][LVLOFFSET];
                            copyOfLogsMemory[j][ECOFFSET] = copyOfLogsMemory[j-1][ECOFFSET];
                            copyOfLogsMemory[j][FSOFFSET] = copyOfLogsMemory[j-1][FSOFFSET];
                            copyOfLogsMemory[j][SIDOFFSET] = copyOfLogsMemory[j-1][SIDOFFSET];
                            copyOfLogsMemory[j][SSIDOFFSET] = copyOfLogsMemory[j-1][SSIDOFFSET];
                            copyOfLogsMemory[j][EDTSOFFSET] = copyOfLogsMemory[j-1][EDTSOFFSET];

                            /* Restore lowest value at right place */
                            copyOfLogsMemory[j-1][LNOFFSET] = LogNumbers;
                            copyOfLogsMemory[j-1][MSGOFFSET] = Message;
                            copyOfLogsMemory[j-1][ORGOFFSET] = Origine;
                            copyOfLogsMemory[j-1][TSOFFSET] = Timestamp;
                            copyOfLogsMemory[j-1][LVLOFFSET] = Level;
                            copyOfLogsMemory[j-1][ECOFFSET] = Errcode;
                            copyOfLogsMemory[j-1][FSOFFSET] = Faultstate;
                            copyOfLogsMemory[j-1][SIDOFFSET] = Sysid;
                            copyOfLogsMemory[j-1][SSIDOFFSET] = Subsysid;
                            copyOfLogsMemory[j-1][EDTSOFFSET] = Edtimestamp;
                        }
                    }
                }
            }
            else
            {
                //sortGrowing = true;
                for (int i=0; i<LOGARRAYLINES; i++)
                {
                    for (int j=1; j<LOGARRAYLINES; j++)
                    {
                        if (copyOfLogsMemory[j-1][rowOffset] < copyOfLogsMemory[j][rowOffset])
                        {
                            /* backup lowest value */
                            LogNumbers = copyOfLogsMemory[j][LNOFFSET];
                            Message = copyOfLogsMemory[j][MSGOFFSET];
                            Origine = copyOfLogsMemory[j][ORGOFFSET];
                            Timestamp = copyOfLogsMemory[j][TSOFFSET];
                            Level = copyOfLogsMemory[j][LVLOFFSET];
                            Errcode = copyOfLogsMemory[j][ECOFFSET];
                            Faultstate = copyOfLogsMemory[j][FSOFFSET];
                            Sysid = copyOfLogsMemory[j][SIDOFFSET];
                            Subsysid = copyOfLogsMemory[j][SSIDOFFSET];
                            Edtimestamp = copyOfLogsMemory[j][EDTSOFFSET];

                            /* permute values */
                            copyOfLogsMemory[j][LNOFFSET] = copyOfLogsMemory[j-1][LNOFFSET];
                            copyOfLogsMemory[j][MSGOFFSET] = copyOfLogsMemory[j-1][MSGOFFSET];
                            copyOfLogsMemory[j][ORGOFFSET] = copyOfLogsMemory[j-1][ORGOFFSET];
                            copyOfLogsMemory[j][TSOFFSET] = copyOfLogsMemory[j-1][TSOFFSET];
                            copyOfLogsMemory[j][LVLOFFSET] = copyOfLogsMemory[j-1][LVLOFFSET];
                            copyOfLogsMemory[j][ECOFFSET] = copyOfLogsMemory[j-1][ECOFFSET];
                            copyOfLogsMemory[j][FSOFFSET] = copyOfLogsMemory[j-1][FSOFFSET];
                            copyOfLogsMemory[j][SIDOFFSET] = copyOfLogsMemory[j-1][SIDOFFSET];
                            copyOfLogsMemory[j][SSIDOFFSET] = copyOfLogsMemory[j-1][SSIDOFFSET];
                            copyOfLogsMemory[j][EDTSOFFSET] = copyOfLogsMemory[j-1][EDTSOFFSET];

                            /* Restore lowest value at right place */
                            copyOfLogsMemory[j-1][LNOFFSET] = LogNumbers;
                            copyOfLogsMemory[j-1][MSGOFFSET] = Message;
                            copyOfLogsMemory[j-1][ORGOFFSET] = Origine;
                            copyOfLogsMemory[j-1][TSOFFSET] = Timestamp;
                            copyOfLogsMemory[j-1][LVLOFFSET] = Level;
                            copyOfLogsMemory[j-1][ECOFFSET] = Errcode;
                            copyOfLogsMemory[j-1][FSOFFSET] = Faultstate;
                            copyOfLogsMemory[j-1][SIDOFFSET] = Sysid;
                            copyOfLogsMemory[j-1][SSIDOFFSET] = Subsysid;
                            copyOfLogsMemory[j-1][EDTSOFFSET] = Edtimestamp;
                        }
                    }
                }

            }
        
        }



        void sortLognumbers(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(LNOFFSET, sortLognumbersGrowing);
            if (sortLognumbersGrowing == true) {sortLognumbersGrowing = false;} else {sortLognumbersGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        /* xgi method called when the link <default_page> is clicked */
        void sortMessages(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(MSGOFFSET, sortMessagesGrowing);
            if (sortMessagesGrowing == true) {sortMessagesGrowing = false;} else {sortMessagesGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }


        void sortOrigin(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(ORGOFFSET, sortOriginGrowing);
            if (sortOriginGrowing == true) {sortOriginGrowing = false;} else {sortOriginGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void sortTimestamp(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(TSOFFSET, sortTimestampGrowing);
            if (sortTimestampGrowing == true) {sortTimestampGrowing = false;} else {sortTimestampGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void sortLevel(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(LVLOFFSET, sortLevelGrowing);
            if (sortLevelGrowing == true) {sortLevelGrowing = false;} else {sortLevelGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void sortErrcode(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(ECOFFSET, sortErrcodeGrowing);
            if (sortErrcodeGrowing == true) {sortErrcodeGrowing = false;} else {sortErrcodeGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void sortFaultstate(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(FSOFFSET, sortFaultstateGrowing);
            if (sortFaultstateGrowing == true) {sortFaultstateGrowing = false;} else {sortFaultstateGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void sortSysid(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(SIDOFFSET, sortSysidGrowing);
            if (sortSysidGrowing == true) {sortSysidGrowing = false;} else {sortSysidGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void sortSubsysid(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(SSIDOFFSET, sortSubsysidGrowing);
            if (sortSubsysidGrowing == true) {sortSubsysidGrowing = false;} else {sortSubsysidGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void sortEdtimestamp(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(EDTSOFFSET, sortEdtimestampGrowing);
            if (sortEdtimestampGrowing == true) {sortEdtimestampGrowing = false;} else {sortEdtimestampGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }



        void getIFrame(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {

            /* Display Logs in a table */
            *out << "<table style=\"width: 100%; text-align: left;\" border=\"1\" cellpadding=\"2\" cellspacing=\"2\">";
            *out << "<tbody> ";

            /* Table Line 0 : name columns */
            *out << "<tr>";


            /* Column 0 */
            *out << "<td style=\"text-align: center; vertical-align: top;\">";
            std::string urlSortLn = "/";
            urlSortLn += getApplicationDescriptor()->getURN();
            urlSortLn += "/sortLognumbers";	
            *out << cgicc::form().set("method","post")
            .set("action", urlSortLn)
            .set("enctype","multipart/form-data");
            *out << cgicc::input().set("type", "submit")
            .set("name", "submit")
            .set("value", "N");
            *out << cgicc::form();
             *out << "</td>";

            /* Column 1 */
            *out << "<td style=\"text-align: center; vertical-align: top;\">";
            std::string urlSortMessages = "/";
            urlSortMessages += getApplicationDescriptor()->getURN();
            urlSortMessages += "/sortMessages";	
            *out << cgicc::form().set("method","post")
            .set("action", urlSortMessages)
            .set("enctype","multipart/form-data");
            *out << cgicc::input().set("type", "submit")
            .set("name", "submit")
            .set("value", "MESSAGE");
            *out << cgicc::form();
             *out << "</td>";
             
            /* Column 2 */
            *out << "<td style=\"text-align: center; vertical-align: top;\">";
            std::string urlSortOrigin = "/";
            urlSortOrigin += getApplicationDescriptor()->getURN();
            urlSortOrigin += "/sortOrigin";	
            *out << cgicc::form().set("method","post")
            .set("action", urlSortOrigin)
            .set("enctype","multipart/form-data");
            *out << cgicc::input().set("type", "submit")
            .set("name", "submit")
            .set("value", "ORIGIN");
            *out << cgicc::form();
             *out << "</td>";

            /* Column 3 */
            *out << "<td style=\"text-align: center; vertical-align: top;\">";
            std::string urlSortTimestamp = "/";
            urlSortTimestamp += getApplicationDescriptor()->getURN();
            urlSortTimestamp += "/sortTimestamp";	
            *out << cgicc::form().set("method","post")
            .set("action", urlSortTimestamp)
            .set("enctype","multipart/form-data");
            *out << cgicc::input().set("type", "submit")
            .set("name", "submit")
            .set("value", "TIMESTAMP");
            *out << cgicc::form();
             *out << "</td>";

            /* Column 4 */
            *out << "<td style=\"text-align: center; vertical-align: top;\">";
            std::string urlSortLevel = "/";
            urlSortLevel += getApplicationDescriptor()->getURN();
            urlSortLevel += "/sortLevel";	
            *out << cgicc::form().set("method","post")
            .set("action", urlSortLevel)
            .set("enctype","multipart/form-data");
            *out << cgicc::input().set("type", "submit")
            .set("name", "submit")
            .set("value", "LEVEL");
            *out << cgicc::form();
             *out << "</td>";

            /* Column 5 */
            *out << "<td style=\"text-align: center; vertical-align: top;\">";
            std::string urlSortErrcode = "/";
            urlSortErrcode += getApplicationDescriptor()->getURN();
            urlSortErrcode += "/sortErrcode";	
            *out << cgicc::form().set("method","post")
            .set("action", urlSortErrcode)
            .set("enctype","multipart/form-data");
            *out << cgicc::input().set("type", "submit")
            .set("name", "submit")
            .set("value", "ERRCODE");
            *out << cgicc::form();
             *out << "</td>";


            /* Column 6 */
            *out << "<td style=\"text-align: center; vertical-align: top;\">";
            std::string urlSortFaultstate = "/";
            urlSortFaultstate += getApplicationDescriptor()->getURN();
            urlSortFaultstate += "/sortFaultstate";	
            *out << cgicc::form().set("method","post")
            .set("action", urlSortFaultstate)
            .set("enctype","multipart/form-data");
            *out << cgicc::input().set("type", "submit")
            .set("name", "submit")
            .set("value", "FAULTSTATE");
            *out << cgicc::form();
             *out << "</td>";


            /* Column 7 */
            *out << "<td style=\"text-align: center; vertical-align: top;\">";
            std::string urlSortSysid = "/";
            urlSortSysid += getApplicationDescriptor()->getURN();
            urlSortSysid += "/sortSysid";	
            *out << cgicc::form().set("method","post")
            .set("action", urlSortSysid)
            .set("enctype","multipart/form-data");
            *out << cgicc::input().set("type", "submit")
            .set("name", "submit")
            .set("value", "SYSID");
            *out << cgicc::form();
             *out << "</td>";


            /* Column 8 */
            *out << "<td style=\"text-align: center; vertical-align: top;\">";
            std::string urlSortSubsysid = "/";
            urlSortSubsysid += getApplicationDescriptor()->getURN();
            urlSortSubsysid += "/sortSubsysid";	
            *out << cgicc::form().set("method","post")
            .set("action", urlSortSubsysid)
            .set("enctype","multipart/form-data");
            *out << cgicc::input().set("type", "submit")
            .set("name", "submit")
            .set("value", "SUBSYSID");
            *out << cgicc::form();
             *out << "</td>";

            /* Column 9 */
            /*
            *out << "<td style=\"text-align: center; vertical-align: top;\">";
            std::string urlSortEdtimestamp = "/";
            urlSortEdtimestamp += getApplicationDescriptor()->getURN();
            urlSortEdtimestamp += "/sortEdtimestamp";	
            *out << cgicc::form().set("method","post")
            .set("action", urlSortEdtimestamp)
            .set("enctype","multipart/form-data");
            *out << cgicc::input().set("type", "submit")
            .set("name", "submit")
            .set("value", "EDTIMESTAMP");
            *out << cgicc::form();
             *out << "</td>";
             */

            /* End Table tag */
            *out << "</tr>";

            if (processingSortRequest == false)
            {
                for (int k=0; k<LOGARRAYLINES; k++)
                {
                    for (int l=0; l<LOGARRAYROWS; l++)
                    {
                        copyOfLogsMemory[k][l] = logsMemory[k][l];
                    }
                }
            }
            else processingSortRequest = false;




            int colorSwitcher = 0;
            for (int i=0; i<LOGARRAYLINES; i++)
            {
                if (copyOfLogsMemory[i][MSGOFFSET] != "UNINITIALIZED_BT_ED")
                {
                    colorSwitcher++;
                    if (colorSwitcher%2 == 0)
                    {
                        *out << "<tr>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << copyOfLogsMemory[i][LNOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << copyOfLogsMemory[i][MSGOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << copyOfLogsMemory[i][ORGOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << copyOfLogsMemory[i][TSOFFSET] << "</td>";
                        // *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << formattedTimeStamp << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << copyOfLogsMemory[i][LVLOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << copyOfLogsMemory[i][ECOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << copyOfLogsMemory[i][FSOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << copyOfLogsMemory[i][SIDOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << copyOfLogsMemory[i][SSIDOFFSET] << "</td>";
                        // *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << copyOfLogsMemory[i][EDTSOFFSET] << "</td>";
                        // *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << edformattedTimeStamp << "</td>";
                        *out << "</tr>";
                    }
                    else
                    {
                        *out << "<tr>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][LNOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][MSGOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][ORGOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][TSOFFSET] << "</td>";
                        // *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << formattedTimeStamp << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][LVLOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][ECOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][FSOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][SIDOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][SSIDOFFSET] << "</td>";
                        // *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][EDTSOFFSET] << "</td>";
                        // *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << edformattedTimeStamp << "</td>";
                        *out << "</tr>";
                    }
                }
            }

            *out << "</tbody>";
            *out << "</table>";
        }



        /* Displays the available HyperDaq links for this process */
        void displayLinks(xgi::Input * in, xgi::Output * out)
        {
            /* Display default page */
            std::string urlBase_ = "/";
            urlBase_ += getApplicationDescriptor()->getURN();
            urlBase_ += "/Default1";
            *out << "<a href=" << urlBase_ << ">Default Page</a>";
            *out << "&nbsp;&nbsp;&nbsp;&nbsp;";

            /* Display diagnostic page */
            std::string urlDiag_ = "/";
            urlDiag_ += getApplicationDescriptor()->getURN();
            urlDiag_ += "/callDiagSystemPage";
            *out << "<a href=" << urlDiag_ << ">Configure Error Dispatcher</a>";
            *out << "&nbsp;&nbsp;&nbsp;&nbsp;";

            /* Display FSM page */
            std::string urlFsm_ = "/";
            urlFsm_ += getApplicationDescriptor()->getURN();
            urlFsm_ += "/callFsmPage";
            *out << "<a href=" << urlFsm_ << ">FSM Access</a>";
            *out << "&nbsp;&nbsp;&nbsp;&nbsp;";

            /* Display diagnostic page */
            std::string urlVisu_ = "/";
            urlVisu_ += getApplicationDescriptor()->getURN();
            urlVisu_ += "/callVisualizeLogsPage";
            *out << "<a href=" << urlVisu_ << ">See received Logs</a>";

            *out << "<br>";
            *out << cgicc::p() << std::endl;
        }




        /* Default() hyperDaq method */
        void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            /* Create HTML header */
            *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
            *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
            *out << cgicc::title("Error Dispatcher") << std::endl;
            xgi::Utils::getPageHeader(  out, 
                                        "Error Dispatcher", 
                                        getApplicationDescriptor()->getContextDescriptor()->getURL(),
                                        getApplicationDescriptor()->getURN(),
                                        "/daq/xgi/images/Application.jpg" );

            /* Display available links on top of the HTML page */
            displayLinks(in, out);

            /* Get page HTML content according to the selected link */
            if (internalState_ == 0) getDefault1Content(in, out);
            if (internalState_ == 1) diagService_->getDiagSystemHtmlPage(in, out,getApplicationDescriptor()->getURN());
            if (internalState_ == 2) getFsmStatePage(in, out);
            if (internalState_ == 3) getVisualizeLogsPage(in, out);

            /* Create HTML footer */
            xgi::Utils::getPageFooter(*out);
        }




        /* xgi methods, defined as macros, needed for running a diagService in this application */
        void ErrorDispatcher::DIAG_CONFIGURE_CALLBACK();
        void ErrorDispatcher::DIAG_APPLY_CALLBACK();




        /* converts DiagSystem loglevel format into OWN loglevel format */
        long getLogLevelToLong(std::string logLevel)
        {
            if (logLevel == DIAGTRACE) return TRACELEVEL;
            else if (logLevel == DIAGDEBUG) return DEBUGLEVEL;
            else if (logLevel == DIAGINFO) return INFOLEVEL;
            else if (logLevel == DIAGWARN) return WARNLEVEL;
            else if (logLevel == DIAGUSERINFO) return USERINFOLEVEL;
            else if (logLevel == DIAGERROR) return ERRORLEVEL;
            else if (logLevel == DIAGFATAL) return FATALLEVEL;
            else if (logLevel == DIAGOFF) return OFFLEVEL;
            return 0;
        }





        /* Processes incoming LOG messages.
        Routes the messages to default appender and/or console and/or chainsaw and/or file.
        Routes the messages to DiagLevelOne.
        DO NOT USE LOG4C CALLS IN THIS METHOD, IT IS NOT REENTRANT */
        xoap::MessageReference receivedLog (xoap::MessageReference msg) throw (xoap::exception::Exception)
        {

            logcounter++;

            /* post reply to caller */
            xoap::MessageReference reply = xoap::createMessage();
            xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
            xoap::SOAPName responseName = envelope.createName( ANSWER_TO_COMMAND_TRIGGED_WHEN_ED_RECEIVED_A_LOG, "xdaq", XDAQ_NS_URI);
            envelope.getBody().addBodyElement ( responseName );

            /* Convert SOAP message into CLog message and get process timestamp */
            CLog l;
			getMessage(msg, &l);

    	    /* Set History "LogNumber" Field */
            char ln_str[20];
            snprintf(ln_str, 19, "%lu", logcounter);
            logsMemory[arrayliner][LNOFFSET] = (std::string)ln_str;
    	    /* Set History "Message" Field */
            logsMemory[arrayliner][MSGOFFSET] = l.getMessage();
    	    /* Set History "Logger" Field */
            logsMemory[arrayliner][ORGOFFSET] = l.getLogger();
    	    /* Set History "Timestamp" Field */
    	    logsMemory[arrayliner][TSOFFSET] = l.getTimestampAsString();
    	    /* Set History "Level" Field */
            logsMemory[arrayliner][LVLOFFSET] = l.getLevel();








			//ERRDBRELATED


			//Expand Logger
			std::string processName;
			std::string processInstance;
			std::string machine;
			std::string port;
			l.expandLoggerInfos(&processName, &processInstance, &machine, &port);
			//Expand Timestamp
			std::string logTimeStamp = l.expandTimeStamp();

/* Fake behaviour of system running FEC+FED+Other */
/*
if (l.getLevel() == "FATAL") processName="FecSupervisor";
if (l.getLevel() == "ERROR") processName="Fed9USupervisor";
*/


			if (processName == (std::string)"FecSupervisor")
			{
				fecLogCounter++;
				/* std::cout << "Opening XML output file for FECs" << std::endl; */
				std::string fileName = (std::string)LOG_FILES_ROOT + (std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
				xmlFecOutputFile_ = l.OpenXmlOutputFile(	fileName.c_str(),
														XML_APPEND_ACESS);
				if (xmlFecOutputFileFirstOpen == true)
				{
					/* std::cout << "Writing XML output file Pre-header for FECs" << std::endl; */
					l.WriteXmlOutputFilePreHeader( 	xmlFecOutputFile_,
													XML_VERSION,
													XML_ENCODING,
													XML_DOCTYPE_ROOT);

					/* std::cout << "Writing XML output file header for FECs" << std::endl; */
					l.WriteXmlOutputFileHeader(xmlFecOutputFile_,
													HEADER_TYPE_EXTENSION_TABLE_NAME_FOR_FEC,
													HEADER_TYPE_NAME_FOR_FEC,
													HEADER_RUN_RUN_TYPE,
													HEADER_RUN_RUN_NUMBER,
													(std::string)runTimeStamp,
													HEADER_RUN_INITIATED_BY_USER,
													HEADER_RUN_LOCATION,
													HEADER_RUN_COMMENT_DESCRIPTION);

					xmlFecOutputFileFirstOpen = false;
				}

				/* std::cout << "Writing XML output file data set for FECs" << std::endl; */
				l.WriteXmlOutputFileDataSet( xmlFecOutputFile_,
											COMMENT_DESCRIPTION,
											DATA_FEC_FILE_NAME,
											ERRXML_VERSION,
											PART_FEC_SERIAL_NUMBER,
											l.getLevel(),
											logsMemory[arrayliner][LNOFFSET],
											TRK_FEC_NAME,
											//ERR_CODE,
											//ERR_MSG,
											//lclErrCode,
											//lclErrMsg,
											l.getErrorCode(),
											l.getMessage(),
											logTimeStamp,
											processName,
											processInstance,
											machine,
											port);


				if ( (fecLogCounter%MAX_LOG_NUMBER_IN_FEC_XML_FILE) == 0 )
				{
					/* std::cout << "Writing XML output file END_HEADER for FECs" << std::endl; */
					l.WriteXmlOutputFileEndHeader( xmlFecOutputFile_);
					/* std::cout << "Closing XML output file for FECs" << std::endl; */
					l.CloseXmlOutputFile(xmlFecOutputFile_);
/*
					std::cout << "Deleting previously completed Fec xml logfile" << std::endl;
					system(DELETE_COMPLETED_XML_FEC_LOGFILE_NAME);
*/
					/* std::cout << "Moving current Fec xml logfile to completed version" << std::endl; */
/*
					system(MOVE_COMPLETED_XML_FEC_LOGFILE_NAME);
*/



					std::string systemCommand = (std::string)MOVE_COMMAND +
								(std::string)LOG_FILES_ROOT + (std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION +
								(std::string)" " +
								(std::string)LOG_FILES_ROOT + (std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION + (std::string)"." + getDateTagForFile();
					system(systemCommand.c_str());



					/* std::cout << "Deleting Fec xml logfile" << std::endl; */
/*
					system(DELETE_XML_FEC_LOGFILE_NAME);
*/
					systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
					system(systemCommand.c_str());


					xmlFecOutputFileFirstOpen = true;
				}
				else
				{
					/* std::cout << "Closing XML output file for FECs" << std::endl; */
					l.CloseXmlOutputFile(xmlFecOutputFile_);
				}

			}//End FEC case processing



			if (processName == (std::string)"Fed9USupervisor")
			{
				fedLogCounter++;

				/* std::cout << "Opening XML output file for FEDs" << std::endl; */
				std::string fileName = (std::string)LOG_FILES_ROOT + (std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
				xmlFedOutputFile_ = l.OpenXmlOutputFile(fileName.c_str(),
														XML_APPEND_ACESS);
				if (xmlFedOutputFileFirstOpen == true)
				{
					/* std::cout << "Writing XML output file Pre-header for FEDs" << std::endl; */
					l.WriteXmlOutputFilePreHeader( 	xmlFedOutputFile_,
													XML_VERSION,
													XML_ENCODING,
													XML_DOCTYPE_ROOT);

					/* std::cout << "Writing XML output file header for FEDs" << std::endl; */
					l.WriteXmlOutputFileHeader(xmlFedOutputFile_,
													HEADER_TYPE_EXTENSION_TABLE_NAME_FOR_FED,
													HEADER_TYPE_NAME_FOR_FED,
													HEADER_RUN_RUN_TYPE,
													HEADER_RUN_RUN_NUMBER,
													(std::string)runTimeStamp,
													HEADER_RUN_INITIATED_BY_USER,
													HEADER_RUN_LOCATION,
													HEADER_RUN_COMMENT_DESCRIPTION);

					xmlFedOutputFileFirstOpen = false;
				}

				/* std::cout << "Writing XML output file data set for FEDs" << std::endl; */
				l.WriteXmlOutputFileDataSet( xmlFedOutputFile_,
											COMMENT_DESCRIPTION,
											DATA_FED_FILE_NAME,
											ERRXML_VERSION,
											PART_FED_SERIAL_NUMBER,
											l.getLevel(),
											logsMemory[arrayliner][LNOFFSET],
											TRK_FED_NAME,
											l.getErrorCode(),
											l.getMessage(),
											logTimeStamp,
											processName,
											processInstance,
											machine,
											port);


				if ( (fedLogCounter%MAX_LOG_NUMBER_IN_FED_XML_FILE) == 0 )
				{
					/* std::cout << "Writing XML output file END_HEADER for FEDs" << std::endl; */
					l.WriteXmlOutputFileEndHeader( xmlFedOutputFile_);
					/* std::cout << "Closing XML output file for FEDs" << std::endl; */
					l.CloseXmlOutputFile(xmlFedOutputFile_);
/*
					std::cout << "Deleting previously completed Fed xml logfile" << std::endl;
					system(DELETE_COMPLETED_XML_FED_LOGFILE_NAME);
*/
					/* std::cout << "Moving current Fed xml logfile to completed version" << std::endl; */
/*
					system(MOVE_COMPLETED_XML_FED_LOGFILE_NAME);
*/
					std::string systemCommand = (std::string)MOVE_COMMAND +
								(std::string)LOG_FILES_ROOT + (std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION +
								(std::string)" " +
								(std::string)LOG_FILES_ROOT + (std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION + (std::string)"." + getDateTagForFile();
					system(systemCommand.c_str());

					/* std::cout << "Deleting Fed xml logfile" << std::endl; */
/*
					system(DELETE_XML_FED_LOGFILE_NAME);
*/
					systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
					system(systemCommand.c_str());

					xmlFedOutputFileFirstOpen = true;
				}
				else
				{
					/* std::cout << "Closing XML output file for FEDs" << std::endl; */
					l.CloseXmlOutputFile(xmlFedOutputFile_);
				}

			}//End FED case processing



			if ( (processName != (std::string)"FecSupervisor") && (processName != (std::string)"Fed9USupervisor") )
			{
				otherLogCounter++;

				/* std::cout << "Opening XML output file for OTHERs" << std::endl; */
				std::string fileName = (std::string)LOG_FILES_ROOT + (std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
				xmlOtherOutputFile_ = l.OpenXmlOutputFile(	fileName.c_str(),
															XML_APPEND_ACESS);
				if (xmlOtherOutputFileFirstOpen == true)
				{
					/* std::cout << "Writing XML output file Pre-header for OTHERs" << std::endl; */
					l.WriteXmlOutputFilePreHeader( 	xmlOtherOutputFile_,
													XML_VERSION,
													XML_ENCODING,
													XML_DOCTYPE_ROOT);

					/* std::cout << "Writing XML output file header for OTHERs" << std::endl; */
					l.WriteXmlOutputFileHeader(xmlOtherOutputFile_,
													HEADER_TYPE_EXTENSION_TABLE_NAME_FOR_OTHER,
													HEADER_TYPE_NAME_FOR_OTHER,
													HEADER_RUN_RUN_TYPE,
													HEADER_RUN_RUN_NUMBER,
													(std::string)runTimeStamp,
													HEADER_RUN_INITIATED_BY_USER,
													HEADER_RUN_LOCATION,
													HEADER_RUN_COMMENT_DESCRIPTION);

					xmlOtherOutputFileFirstOpen = false;
				}

				/* std::cout << "Writing XML output file data set for OTHERs" << std::endl; */
				l.WriteXmlOutputFileDataSet( xmlOtherOutputFile_,
											COMMENT_DESCRIPTION,
											DATA_OTHER_FILE_NAME,
											ERRXML_VERSION,
											PART_OTHER_SERIAL_NUMBER,
											l.getLevel(),
											logsMemory[arrayliner][LNOFFSET],
											TRK_OTHER_NAME,
											l.getErrorCode(),
											l.getMessage(),
											logTimeStamp,
											processName,
											processInstance,
											machine,
											port);


				if ( (otherLogCounter%MAX_LOG_NUMBER_IN_FEC_XML_FILE) == 0 )
				{
					/* std::cout << "Writing XML output file END_HEADER for OTHERs" << std::endl; */
					l.WriteXmlOutputFileEndHeader( xmlOtherOutputFile_);
					/* std::cout << "Closing XML output file for OTHERs" << std::endl; */
					l.CloseXmlOutputFile(xmlOtherOutputFile_);
/*
					std::cout << "Deleting previously completed Other xml logfile" << std::endl;
					system(DELETE_COMPLETED_XML_OTHER_LOGFILE_NAME);
*/
					/* std::cout << "Moving current Other xml logfile to completed version" << std::endl; */
/*
					system(MOVE_COMPLETED_XML_OTHER_LOGFILE_NAME);
*/
					std::string systemCommand = (std::string)MOVE_COMMAND +
								(std::string)LOG_FILES_ROOT + (std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION +
								(std::string)" " +
								(std::string)LOG_FILES_ROOT + (std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION + (std::string)"." + getDateTagForFile();
					system(systemCommand.c_str());

					/*std::cout << "Deleting Other xml logfile" << std::endl;*/
/*
					system(DELETE_XML_OTHER_LOGFILE_NAME);
*/
					systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
					system(systemCommand.c_str());

					xmlOtherOutputFileFirstOpen = true;
				}
				else
				{
					/* std::cout << "Closing XML output file for OTHERs" << std::endl; */
					l.CloseXmlOutputFile(xmlOtherOutputFile_);
				}

			}//End OTHER case processing

			//ERRDBRELATED - END

























/*
    	    if (l.getTypeOfMessage() == MESSAGE_TYPE_IS_LONG)
	    {
*/
    	    	/* Set History "ErrorCode" Field */
                logsMemory[arrayliner][ECOFFSET] = l.getErrorCode();
    	    	/* Set History "FaultState" Field */
                logsMemory[arrayliner][FSOFFSET] = l.getFaultState();
    	    	/* Set History "SystemID" Field */
                logsMemory[arrayliner][SIDOFFSET] = l.getSystemID();
    	    	/* Set History "SubSystemID" Field */
                logsMemory[arrayliner][SSIDOFFSET] = l.getSubSystemID();
/*
    	    }
	    else
	    {
                logsMemory[arrayliner][ECOFFSET] = "";
                logsMemory[arrayliner][FSOFFSET] = "";
                logsMemory[arrayliner][SIDOFFSET] = "";
                logsMemory[arrayliner][SSIDOFFSET] = "";
	    }
*/

            arrayliner++;
            if (arrayliner >= LOGARRAYLINES) arrayliner=0;


/* Verify first that no reconfiguration action is needed,
accorded to log content.*/
checkIfReconfigurationIsNeeded(&l);


            /* If we use the DEFAULT appender */
/*
            if (diagService_->useDefaultAppender_ == true)
            {
                if (l.getLevel() ==  DIAGTRACE) diagService_->reportError(l.getMessage(), DIAGTRACE);
                else if (l.getLevel() ==  DIAGDEBUG) diagService_->reportError(l.getMessage(), DIAGDEBUG);
                else if (l.getLevel() ==  DIAGINFO) diagService_->reportError(l.getMessage(), DIAGINFO);
                else if (l.getLevel() ==  DIAGWARN) diagService_->reportError(l.getMessage(), DIAGWARN);
                else if (l.getLevel() ==  DIAGUSERINFO) diagService_->reportError(l.getMessage(), DIAGUSERINFO);
                else if (l.getLevel() ==  DIAGERROR) diagService_->reportError(l.getMessage(), DIAGERROR);
                else if (l.getLevel() ==  DIAGFATAL) diagService_->reportError(l.getMessage(), DIAGFATAL);

            }
*/
/*
            else
            // If we are on a home-made routing strategy
            {
*/

                /* IF we have to forward errors towards another Local/Global Error Dispatcher or in Sentinel relay */
			    if ( (diagService_->edUsable_ == true) || (diagService_->edGlbUsable_ == true) || (diagService_->sentinelUsable_ == true) )
                {
					diagService_->repostError(	l.getTypeOfMessage(),
												l.getLevel(),
												l.getMessage(),
												l.getTimeStamp(),
												l.getTimestampAsString(),
												l.getLogger(),
												l.getErrorCode(),
												l.getFaultState(),
												l.getSystemID(),
												l.getSubSystemID(),
												l.getExtraBuffer()
												);
                }


                /* IF we have to route errors to FILE */
                if (diagService_->fileLoggingUsable_ == true)
                {
                    /* Log to file */
                    if ( getLogLevelToLong(l.getLevel()) >= getLogLevelToLong(diagService_->fileLogLevel_) )
                    {

						if (diagService_->logFileNameHasChanged_ == true)
						{
							if (fileOut_ != NULL) fclose(fileOut_);
							fileOutIsOpen_ = false;
						}
						bool logFileNameHasFlipped = false;
    	    	    	//Reset File if first log
				    	if (fileOutIsOpen_ == false)
						{
							if (diagService_->logFileNameHasChanged_ == true)
							{
								fileOut_ = fopen(diagService_->logFileName_.c_str(), "w");
								diagService_->logFileNameHasChanged_ = false;
								logFileNameHasFlipped = true;
							}
							else fileOut_ = fopen(diagService_->logFileName_.c_str(), "a");

    		        	    if (fileOut_ == NULL)
    	    	        	{
	            	        	std::stringstream errMsg;				
	                	    	std::cout << "Error : Unable to write to destination log file \"" << diagService_->logFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
		                	    diagService_->fileLoggingUsable_ = false;
								fileOutIsOpen_ = false;
    	    		        }
        			        else
    	        		    {
							    fileOutIsOpen_ = true;
								if (logFileNameHasFlipped == true) fileLogsCounter=0;
								std::cout << "LogFile \"" << diagService_->logFileName_.c_str() << "\" opened in write mode" << std::endl;
		    	            }

						}
						if (fileOutIsOpen_ == true)
						{
							fileLogsCounter++;
									#ifdef WILL_COMPILE_FOR_TRACKER
										fprintf(fileOut_,"%s",l.buildStringForTrackerLogFileOutput(fileLogsCounter).c_str());
									#else
										#ifdef WILL_COMPILE_FOR_PIXELS
											fprintf(fileOut_,"%s",l.buildStringForPixelsLogFileOutput(fileLogsCounter).c_str());
										#else
											fprintf(fileOut_,"%s",l.buildStringForLogFileOutput(fileLogsCounter).c_str());
										#endif
									#endif

							fflush(fileOut_);
							if (fileLogsCounter >= MAX_NBR_OF_GENERAL_FILE_LOGS)
							{
								std::cout << "closing local file" << std::endl;
								fclose(fileOut_);
								
								//MOVE
								
					std::string systemCommand = (std::string)MOVE_COMMAND +
								diagService_->logFileName_ +
								(std::string)" " +
								diagService_->logFileName_ + (std::string)"." + getDateTagForFile();
					system(systemCommand.c_str());
								std::cout << "moving local file as : " << systemCommand << std::endl;

								std::cout << "reopening local file" << std::endl;
								//REOPEN WITH "w"
								fileOut_ = fopen(diagService_->logFileName_.c_str(), "w");
	    		        	    if (fileOut_ == NULL)
    		    	        	{
	    	        	        	std::stringstream errMsg;				
	        	        	    	std::cout << "Error : Unable to write to destination log file \"" << diagService_->logFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
		        	        	    diagService_->fileLoggingUsable_ = false;
									fileOutIsOpen_ = false;
	    	    		        }
								else std::cout << "LogFile \"" << diagService_->logFileName_.c_str() << "\" opened in write mode" << std::endl;
								
								//Reset thing
								fileLogsCounter=0;
								


							}

						}
                   }
                }

                /* IF we have to route errors to CHAINSAW */
for (int csStreamNum = 0; csStreamNum < diagService_->nbrOfChainsawStreams_ ; csStreamNum++)
{
                /* IF we have to route errors to CHAINSAW */
                if ( (diagService_->csRoute_[csStreamNum] == true) && (diagService_->csRouteValidated_[csStreamNum] == true) )
                {
                    /* Si le lvl general du message est superieur ou egal au level lcl DB,
                    OU si on est en forced log et que le lvl general du message est superieur ou egal au lvl de FORCE_DB_LOG */
                    if ( getLogLevelToLong(l.getLevel()) >= getLogLevelToLong(diagService_->csLogLevel_[csStreamNum]) )
                    {
                        #ifdef DEBUG_ED_ONRECEIVEDLOG
                            std::cout << "CS logging enabled for current levels on stream " << diagService_->csReceiver_[csStreamNum] << std::endl;
                        #endif
						//std::cout << "Begin to append to socket" << std::endl;
                        diagService_->chainSawManager_->pushClogToStream(diagService_->csReceiver_[csStreamNum], l);
						//diagService_->chainsawAppender_->appendToSocket(l);
						//std::cout << "End to append to socket" << std::endl;
                    }
                }
}
    
                /* IF we have to route errors to CONSOLE */
                if ( (diagService_->ccRoute_ == true) && (diagService_->ccRouteValidated_ == true) )
                {
                    /* Si le lvl general du message est superieur ou egal au level lcl console,
                    OU si on est en forced log et que le lvl general du message est superieur ou egal au lvl de FORCE_CC_LOG */
                    if ( getLogLevelToLong(l.getLevel()) >= getLogLevelToLong(diagService_->ccLogLevel_) )
                    {
                        #ifdef DEBUG_ED_ONRECEIVEDLOG
                            std::cout << "Console logging enabled for current levels" << std::endl;
                        #endif
                        std::cout << l.buildStringForConsoleOutput() << std::endl ;
                    }
                }
/*
                // IF we have to route errors to DB
                if ( (diagService_->dbRoute_ == true) && (diagService_->dbRouteValidated_ == true) )
                {
                    // Si le lvl general du message est superieur ou egal au level lcl DB,
                    // OU si on est en forced log et que le lvl general du message est superieur ou egal au lvl de FORCE_DB_LOG
                    if ( getLogLevelToLong(l.getLevel()) >= getLogLevelToLong(diagService_->dbLogLevel_) )
                    {
                        //#ifdef DEBUG_ED_ONRECEIVEDLOG
                            std::cout << "ErrorDispatcher : DB logging enabled for current levels" << std::endl;
                        //#endif
                        // Log Message to DB
                        //See with Damien for implementation
                    }
                }
*/

/*
                if (diagService_->dbRoute_ == true)
                {
                    // Log to file
                    if ( getLogLevelToLong(l.getLevel()) >= getLogLevelToLong(diagService_->dbLogLevel_) )
                    {
						if (diagService_->dbFileNameHasChanged_ == true)
						{
							if (dbFileOut_ != NULL) fclose(dbFileOut_);
							dbFileOutIsOpen_ = false;
						}

    	    	    	//Reset File if first log
				    	if (dbFileOutIsOpen_ == false)
						{
							if (diagService_->dbFileNameHasChanged_ == true)
							{
								dbFileOut_ = fopen(diagService_->dbFileName_.c_str(), "w");
								diagService_->dbFileNameHasChanged_ = false;
							}
							else dbFileOut_ = fopen(diagService_->dbFileName_.c_str(), "a");

    		        	    if (dbFileOut_ == NULL)
    	    	        	{
	            	        	std::stringstream errMsg;				
	                	    	std::cout << "Error : Unable to write to destination DB file \"" << diagService_->dbFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
		                	    diagService_->dbRoute_ = false;
								dbFileOutIsOpen_ = false;
    	    		        }
        			        else
    	        		    {
							    dbFileOutIsOpen_ = true;
								std::cout << "DB File \"" << diagService_->dbFileName_.c_str() << "\" opened in write mode" << std::endl;
		    	            }

						}
						if (dbFileOutIsOpen_ == true)
						{
							dbFileLogsCounter++;
							fprintf(dbFileOut_,"%s",l.buildStringForDbFileOutput(dbFileLogsCounter).c_str());
							fflush(dbFileOut_);
						}
                    }
                }
*/
/*
            } // end of home made routing strategy 
*/


    	    #ifdef USE_XRELAY
    	    /* Ack message reception for asynchronous shakehand with originator process */
    	    /* Build SOAP for sending action */
    	    xoap::MessageReference err_msg = xoap::createMessage();
    	    xoap::SOAPPart err_soap = err_msg->getSOAPPart();
    	    xoap::SOAPEnvelope err_envelope = err_soap.getEnvelope();

    	    std::string err_topNode = XR_TOPNODE;
            std::string err_prefix = XR_PREFIX;
            std::string err_httpAdd = XR_HTTPADD;

            xoap::SOAPName err_envelopeName = err_envelope.getElementName();
    	    xoap::SOAPHeader err_header = err_envelope.addHeader();

            xoap::SOAPName err_relayName = err_envelope.createName(err_topNode, err_prefix,  err_httpAdd);
            xoap::SOAPHeaderElement err_relayElement = err_header.addHeaderElement(err_relayName);

    	    /* Add the actor attribute */
    	    xoap::SOAPName err_actorName = err_envelope.createName(XR_ACTORNAME, err_envelope.getElementName().getPrefix(), err_envelope.getElementName().getURI());
    	    err_relayElement.addAttribute(err_actorName,err_httpAdd);

            std::string tmpCommandName;
            if (l.getTalkToLocalEd() == true)
            {
                tmpCommandName = FREE_DISTANT_SEMAPHORE_FOR_LCL_ED_COMMAND;
            }
            else tmpCommandName = FREE_DISTANT_SEMAPHORE_FOR_GLB_ED_COMMAND;
            xoap::SOAPName command = err_envelope.createName(tmpCommandName, "xdaq", XDAQ_NS_URI);
            xoap::SOAPBody err_body = err_envelope.getBody();
            err_body.addBodyElement(command);

    	    // Add the "to" node
    	    std::string err_childNode = XR_TONODENAME;
    	    xoap::SOAPName err_toName = err_envelope.createName(err_childNode, err_prefix, " ");
    	    xoap::SOAPElement err_childElement = err_relayElement.addChildElement(err_toName);


    	    xoap::SOAPName err_urlName = err_envelope.createName(XR_URLNAME);
    	    xoap::SOAPName err_urnName = err_envelope.createName(XR_URNNAME);
    	    err_childElement.addAttribute(err_urlName,l.getSenderURL());
    	    err_childElement.addAttribute(err_urnName,l.getSenderURN());

    	    //Send message through XRelaY
    	    try 
    	    {	
	    		// Get the Xrelay application descriptor and post the message:
				xdaq::ApplicationDescriptor * err_xrelay = getApplicationContext()->getFirstApplication ("XRelay")->getApplicationDescriptor();
				xoap::MessageReference err_reply = this->getApplicationContext()->postSOAP(err_msg, *getApplicationDescriptor(), *err_xrelay);
    	    } 
    	    catch (xdaq::exception::Exception& e) 
    	    {
	    	/*
            	std::cout << "Can't find the XRelaY SOURCE ApplicationDescriptor" << std::endl;
                std::cout << "I wanted to post the SOURCE message :";
                err_msg->writeTo(std::cout);
                std::cout << std::endl;
	    	*/
    	    }
    	    #endif

            return reply;
        }





        /* Turns the ErrorDispatcher into state INITIALISED (FSM point of view) */
        void configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
        {
            /* Apply fireitems value to internal variables and check validity */
            DIAG_EXEC_FSM_INIT_TRANS
        }



        /* Turns the ErrorDispatcher into state HALTED (FSM point of view) */
        void haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
        {
            /* Apply fireitems value to internal variables and check validity */
            DIAG_EXEC_FSM_STOP_TRANS
        }
   

   
        /* Allows the FSM to be controlled via a soap message carrying a "fireEvent" message */
        xoap::MessageReference fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception)
        {
            //#define DEBUG_FIREEVENT
            #ifdef DEBUG_FIREEVENT
                std::cout << "In ErrorDispatcher : fireEvent received" << std::endl ;
            #endif

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
                        }
                        catch (toolbox::fsm::exception::Exception & e)
                        {
                            XCEPT_RETHROW(xcept::Exception, "invalid command", e);
                            diagService_->reportError (e.what(), DIAGERROR);
                        }
                        xoap::MessageReference reply = xoap::createMessage();
                        xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
                        xoap::SOAPName responseName = envelope.createName( commandName +"Response", "xdaq", XDAQ_NS_URI);
                        envelope.getBody().addBodyElement ( responseName );
                        return reply;
                    }
    	            #ifdef DEBUG_FIREEVENT
    	    	    	else std::cout << "Duplicate fire command sent" << std::endl;
                    #endif
                }
            }
            diagService_->reportError ("Command not found", DIAGERROR);
            XCEPT_RAISE(xcept::Exception,"command not found");	
        }


        /* Called when a state transition fails in the FSM */
        void failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
        {
            toolbox::fsm::FailedEvent & fe = dynamic_cast<toolbox::fsm::FailedEvent&>(*e);
            std::ostringstream msgError;
            msgError << "Failure occurred when performing transition from: " << fe.getFromState() << " to: " << fe.getToState() << " exception: " << fe.getException().what();
            diagService_->reportError (msgError.str(), DIAGERROR);
        }



        /* Can be called via the <configure_FSM> hyperdaq HTML page of this process */
        void configureStateMachine(xgi::Input * in, xgi::Output * out)
        {
            if (fsm_.getCurrentState() == 'H')
            {
                toolbox::Event::Reference e(new toolbox::Event(EXECTRANSHC, this));
                fsm_.fireEvent(e);
            }
            Default(in, out);
        }


        /* Can be called via the <configure_FSM> hyperdaq HTML page of this process */
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




				if (diagService_->fileLoggingUsable_ = true)
				{
				
					if (diagService_->logFileNameHasChanged_ == true)
					{
						if (fileOut_ != NULL) fclose(fileOut_);
						fileOutIsOpen_ = false;
					}

				
					if (fileOutIsOpen_ == false)
					{
						if (diagService_->logFileNameHasChanged_ == true)
						{
							fileOut_ = fopen(diagService_->logFileName_.c_str(), "w");
							diagService_->logFileNameHasChanged_ = false;
							/* next line useful only in case of "default" config keyword */
							diagService_->oldLogFileName_ = diagService_->logFileName_;
						}
						else fileOut_ = fopen(diagService_->logFileName_.c_str(), "a");

    		            if (fileOut_ == NULL)
    	    	        {
	            	        std::stringstream errMsg;				
                	    	std::cout << "Error : Unable to open destination file \"" << diagService_->logFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
	                	    diagService_->fileLoggingUsable_ = false;
							fileOutIsOpen_ = false;
    	    	        }
        		        else
    	        	    {
						    fileOutIsOpen_ = true;
							std::cout << "LogFile \"" << diagService_->logFileName_.c_str() << "\" opened in write mode" << std::endl;
	    	            }

					}
				}



/*
                if (diagService_->dbRoute_ == true)
                {
						if (diagService_->dbFileNameHasChanged_ == true)
						{
							if (dbFileOut_ != NULL) fclose(dbFileOut_);
							dbFileOutIsOpen_ = false;
						}

    	    	    	//Reset File if first log
				    	if (dbFileOutIsOpen_ == false)
						{
							if (diagService_->dbFileNameHasChanged_ == true)
							{
								dbFileOut_ = fopen(diagService_->dbFileName_.c_str(), "w");
								diagService_->dbFileNameHasChanged_ = false;
								// next line useful only in case of "default" config keyword
		 						diagService_->oldDbFileName_ = diagService_->dbFileName_;
							}
							else dbFileOut_ = fopen(diagService_->dbFileName_.c_str(), "a");

    		        	    if (dbFileOut_ == NULL)
    	    	        	{
	            	        	std::stringstream errMsg;				
	                	    	std::cout << "Error : Unable to write to destination DB file \"" << diagService_->dbFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
		                	    diagService_->dbRoute_ = false;
								dbFileOutIsOpen_ = false;
    	    		        }
        			        else
    	        		    {
							    dbFileOutIsOpen_ = true;
								std::cout << "DB File \"" << diagService_->dbFileName_.c_str() << "\" opened in write mode" << std::endl;
		    	            }

						}
                }
*/


                std::cout << "ErrorDispatcher(LID=" << getApplicationDescriptor()->getLocalId() << ",Instance=" << getApplicationDescriptor()->getInstance() << ") ready" << std::endl;
            }
        #endif






        /* Read the SOAP message and create a CLog structure
        DO NOT USE LOG4C CALLS IN THIS METHOD, IT IS NOT REENTRANT */
        void getMessage(xoap::MessageReference m, CLog * l) throw (xoap::exception::Exception)
        {
            xoap::SOAPBody rb = m->getSOAPPart().getEnvelope().getBody();
            if (rb.hasFault() )
            {
                xoap::SOAPFault fault = rb.getFault();
                std::string msg = "ErrorDispatcher: ";
                msg += fault.getFaultString();
                std::cout << msg << std::endl;
                /* MUST raise an exception here */
            }
            else
            {
                std::vector<xoap::SOAPElement> logElement = rb.getChildElements ();
                if(logElement.size()==1)
                {
                    xoap::SOAPName messageType ("messageType", "", "");
		    		l->setTypeOfMessage(logElement[0].getAttributeValue(messageType));
		    
		    		/* Fields to get in all cases of messages, SHORT or LONG */
                    xoap::SOAPName timestamp ("timestamp", "", "");
		    		l->setTimestampAsString(logElement[0].getAttributeValue(timestamp));

                    xoap::SOAPName numtimestamp ("numtimestamp", "", "");
		    		l->setTimeStamp(atoll(logElement[0].getAttributeValue(numtimestamp).c_str()));

                    xoap::SOAPName level ("level", "", "");
		    		l->setLevel(logElement[0].getAttributeValue(level));

                    xoap::SOAPName logger ("logger", "", "");
		    		l->setLogger(logElement[0].getAttributeValue(logger));

                    xoap::SOAPName message ("message", "", "");
		    		l->setMessage(logElement[0].getAttributeValue(message));
/*
    	    	    if (l->getTypeOfMessage() == MESSAGE_TYPE_IS_LONG)
		    		{
*/
    	    	    	xoap::SOAPName errorCode ("errorCode", "", "");
    	    	    	l->setErrorCode(logElement[0].getAttributeValue(errorCode));

    	    	    	xoap::SOAPName faultstate ("faultstate", "", "");
    	    	    	l->setFaultState(logElement[0].getAttributeValue(faultstate));

    	    	    	xoap::SOAPName systemid ("systemid", "", "");
    	    	    	l->setSystemID(logElement[0].getAttributeValue(systemid));

    	    	    	xoap::SOAPName subsystemid ("subsystemid", "", "");
    	    	    	l->setSubSystemID(logElement[0].getAttributeValue(subsystemid));

    	    	    	xoap::SOAPName extraBuffer ("extraBuffer", "", "");
    	    	    	l->setExtraBuffer(logElement[0].getAttributeValue(extraBuffer));
						// std::cout << "In ErrorDispatcher : ExtraBuffer = " << l->getExtraBuffer() << std::endl;
/*
		    		}
*/

    	    	    #ifdef USE_XRELAY
                        xoap::SOAPName senderURL ("senderURL", "", "");
		    			l->setSenderURL(logElement[0].getAttributeValue(senderURL));

                        xoap::SOAPName senderURN ("senderURN", "", "");
		    			l->setSenderURN(logElement[0].getAttributeValue(senderURN));
			
                        xoap::SOAPName talkToLocalEd ("talkToLocalEd", "", "");
                        if (logElement[0].getAttributeValue(talkToLocalEd) == "true")
                        {
                            l->setTalkToLocalEd(true);
                        } else l->setTalkToLocalEd(false);

    	    	    	xoap::SOAPName missedMessages ("missedMessages", "", "");
		    			l->setMissedMessagesAsString(logElement[0].getAttributeValue(missedMessages));

		    		#endif

                } /* End of if (logsize==1) */
            } /* End of No BODY FAULT block */
            /* MUST manage exceptions here */
        }



void checkIfReconfigurationIsNeeded(CLog * l) throw (xoap::exception::Exception)
{

	bool reconfIsNeeded = false;

	/* Compute if reconfiguration is needed ... */

	if (reconfIsNeeded == true)
	{
		/* if we already have a reconfiguration action running from this ErrorDispatcher AND
		we need to perform a new one AND the previous one is not over then simply skip the operation.
		A mutex cannot be used here, would lead to infinite wait loop. */
		if (reconfRunning__ == false)
		{
			reconfRunning__ = true;
			diagService_->reportError("Send a reconfiguration request Log with ErrorCode 201 from ED",DIAGUSERINFO,
			"RECONFCMD",
			201,
			"STEADY",
			"TRACKER",
			"TEC");
			//reconfRunning__ = false;
		}
		else
		{
			std::cout << "Impossible to execute the requested reconfiguration command, previous one is still running." << std::endl;
		}

	}
}


        xoap::MessageReference freeReconfCmd (xoap::MessageReference msg) throw (xoap::exception::Exception)
        {
            /* post reply to caller */
            xoap::MessageReference reply = xoap::createMessage();
            xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
            xoap::SOAPName responseName = envelope.createName("ReconfigurationModuleResponse", "xdaq", XDAQ_NS_URI);
            envelope.getBody().addBodyElement ( responseName );

			reconfRunning__ = false;
//			std::cout << "Complex. reconf. freed." << std::endl;
			return reply;
		}


//ERRDBRELATED
        std::string getDateTagForFile(void)
		{

	struct timeval tv;
	gettimeofday(&tv,NULL);
CLog myClog;
                    unsigned long resu1, resu2;
                    resu1 = tv.tv_sec;
                    resu2 = tv.tv_usec/1000;
                    char s1[100];
                    sprintf(s1,"%lu",resu1);
                    char s2[100];
                    sprintf(s2,"%.3lu",resu2);
                    char s3[100];
                    strcpy(s3,s1);
                    strcat(s3,s2);
					long long lcl_timestamp = atoll(s3);
                    myClog.setTimeStamp(lcl_timestamp);
char secs[50];
std::stringstream tstamp;
		tstamp << myClog.getTimeStamp();

strcpy(secs, tstamp.str().c_str());
secs[strlen(secs) - 3] = '\0';
		struct tm * timeSplittedPtr;
		time_t modTS = (time_t)atoll(secs);
		timeSplittedPtr = localtime(&modTS);

		char logTimeStamp1[100];
		snprintf(logTimeStamp1, 22, "%.4d-%.2d-%.2d--%.2d:%.2d:%.2d",
				(timeSplittedPtr->tm_year + 1900),
				timeSplittedPtr->tm_mon+1,
				timeSplittedPtr->tm_mday,
				timeSplittedPtr->tm_hour,
				timeSplittedPtr->tm_min,
				timeSplittedPtr->tm_sec);

/* std::cout << "MY TIMESTAMP IS : " << logTimeStamp1 << std::endl; */
return (std::string)logTimeStamp1;
	}


};

#endif
#endif
