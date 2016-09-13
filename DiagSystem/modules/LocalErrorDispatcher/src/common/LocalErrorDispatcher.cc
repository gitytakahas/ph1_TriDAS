/*

   FileName : 		LocalErrorDispatcher.cc

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


#include "LocalErrorDispatcher.h"

XDAQ_INSTANTIATOR_IMPL(LocalErrorDispatcher)




        LocalErrorDispatcher::LocalErrorDispatcher(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): xdaq::Application(s)
        {

			logFilePathAndPrefix_ = "";

    	    fileOutIsOpen_ = false;
			fileOut_ = NULL;

			fileOutPixelsXmlIsOpen_ = false;
			fileOutPixelsXml_ = NULL;
			fileOutPixelsXmlUsable_ = false;
			pixelsXmlLogFileName_ = "";

			fileOutPixelsLog4jIsOpen_ = false;
			fileOutPixelsLog4j_ = NULL;
			fileOutPixelsXmlUsable_ = false;
			pixelsLog4jLogFileName_ = "";



			reconfRunning__ = false;


			isRunningForPixels_ = false;


			//ERRDBRELATED
/*
			xmlFecOutputFile_ = NULL;
			xmlFecOutputFileFirstOpen = true;
			xmlFedOutputFile_ = NULL;
			xmlFedOutputFileFirstOpen = true;
			xmlOtherOutputFile_ = NULL;
			xmlOtherOutputFileFirstOpen = true;
*/


            /* Give funny and useless informations at load time */
            std::stringstream mmesg;
            mmesg << "Process version " << LOCALERRORDISPATHCER_PACKAGE_VERSION << " loaded";
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
            fecLogCounter = 0;
            fedLogCounter = 0;
            otherLogCounter = 0;
            fileLogsCounter = 0;
/*            dbFileLogsCounter = 0; */

            /* Instanciate diagnostic object */
            diagService_ = new DiagBagWizard(
                                    getApplicationDescriptor()->getClassName() + "DiagLvlOne" ,
                                    this->getApplicationLogger(),
                                    getApplicationDescriptor()->getClassName(),
                                    getApplicationDescriptor()->getInstance(),
                                    getApplicationDescriptor()->getLocalId(),
                                    (xdaq::WebApplication *)this );


            /* bind xgi and xoap commands specific to this application */
            xgi::bind(this,&LocalErrorDispatcher::Default, "Default");
            xgi::bind(this,&LocalErrorDispatcher::Default1, "Default1");
            xgi::bind(this,&LocalErrorDispatcher::callDiagSystemPage, "callDiagSystemPage");
            xgi::bind(this,&LocalErrorDispatcher::callFsmPage, "callFsmPage");
            xgi::bind(this,&LocalErrorDispatcher::callVisualizeLogsPage, "callVisualizeLogsPage");
            xgi::bind(this,&LocalErrorDispatcher::getIFrame, "getIFrame");
            xgi::bind(this,&LocalErrorDispatcher::configureStateMachine, "configureStateMachine");
            xgi::bind(this,&LocalErrorDispatcher::stopStateMachine, "stopStateMachine");
            xoap::bind(this, &LocalErrorDispatcher::receivedLog, "receivedLog", XDAQ_NS_URI );
/*
            xgi::bind(this,&LocalErrorDispatcher::callLogsSummaryPage, "callLogsSummaryPage");
*/
//RECONTEST
/* xoap::bind(this, &LocalErrorDispatcher::reconfLog, "reconfLog", XDAQ_NS_URI ); */

            xoap::bind(this, &LocalErrorDispatcher::freeReconfCmd, "freeReconfCmd", XDAQ_NS_URI );

            xgi::bind(this,&LocalErrorDispatcher::sortLognumbers, "sortLognumbers");
            xgi::bind(this,&LocalErrorDispatcher::sortMessages, "sortMessages");
            xgi::bind(this,&LocalErrorDispatcher::sortOrigin, "sortOrigin");
            xgi::bind(this,&LocalErrorDispatcher::sortTimestamp, "sortTimestamp");
            xgi::bind(this,&LocalErrorDispatcher::sortLevel, "sortLevel");
            xgi::bind(this,&LocalErrorDispatcher::sortErrcode, "sortErrcode");
            xgi::bind(this,&LocalErrorDispatcher::sortFaultstate, "sortFaultstate");
            xgi::bind(this,&LocalErrorDispatcher::sortSysid, "sortSysid");
            xgi::bind(this,&LocalErrorDispatcher::sortSubsysid, "sortSubsysid");
            xgi::bind(this,&LocalErrorDispatcher::sortEdtimestamp, "sortEdtimestamp");


            /* bind xgi commands needed for running a diagService in this application */
            xgi::bind(this,&LocalErrorDispatcher::configureDiagSystem, "configureDiagSystem");
            xgi::bind(this,&LocalErrorDispatcher::applyConfigureDiagSystem, "applyConfigureDiagSystem");

			//CallBack to inform LogReader process of the logFileName currently in use
            xoap::bind(this,&LocalErrorDispatcher::getCurrentLogFileName, "getCurrentLogFileName", XDAQ_NS_URI);

            /* Call a macro needed for running a diagService in this application */
            DIAG_DECLARE_ED_APP

            /* Liste des etats qu'on va trouver dans la FSM */
            fsm_.addState ('C', "Configured") ;
            fsm_.addState ('H', "Halted");

            /* Methode initialiseAction qui doit gerer les constructions, etc... qui surviennent lors de la
            transition entre l'etat Initial et l'etat Halted */
            fsm_.addStateTransition ('C','H', EXECTRANSCH, this, &LocalErrorDispatcher::haltAction);
            fsm_.addStateTransition ('H','C', EXECTRANSHC, this, &LocalErrorDispatcher::configureAction);

            /* Si une transition ne peut se faire, on appel la methode failedTransition */
            fsm_.setFailedStateTransitionAction( this, &LocalErrorDispatcher::failedTransition );

            /* On synchronise la FASM avec l'etat du process apres load, a savoir : etat HALTED */
            fsm_.setInitialState('H');
            fsm_.reset();

            /* Import/Export the stateName variable */
            getApplicationInfoSpace()->fireItemAvailable("stateName", &fsm_.stateName_);

            /* Bind SOAP callbacks for FSM control messages */
            xoap::bind (this, &LocalErrorDispatcher::fireEvent, EXECTRANSCH, XDAQ_NS_URI);
            xoap::bind (this, &LocalErrorDispatcher::fireEvent, EXECTRANSHC, XDAQ_NS_URI);

			//ERRDBRELATED
			//Delet existing file if any
/*
			std::string systemCommand;
			systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
	 		system(systemCommand.c_str());
			systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
			system(systemCommand.c_str());
			systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
			system(systemCommand.c_str());
*/
/*
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
*/


char* readEnv;
readEnv=getenv ("USE_DIAGSYSTEM_FOR_PIXELS") ;
if (readEnv != NULL)
{
	if ( std::string(readEnv) == "TRUE" )
	{
		isRunningForPixels_ = true;
		std::cout << "Local Error Dispatcher will run in PIXELS mode" << std::endl;
	}
}
if ( isRunningForPixels_ == false ) std::cout << "Local Error Dispatcher will run in TRACKER mode" << std::endl;



            #ifdef AUTO_CONFIGURE_PROCESSES
                std::stringstream timerName;
                timerName << getApplicationDescriptor()->getContextDescriptor()->getURL() << ":";
                timerName << getApplicationDescriptor()->getClassName() << ":" << getApplicationDescriptor()->getLocalId() << ":" << getApplicationDescriptor()->getInstance();
                toolbox::task::Timer * timer = toolbox::task::getTimerFactory()->createTimer(timerName.str());
                toolbox::TimeInterval interval(AUTO_LED_CONFIGURE_DELAY,0);
                toolbox::TimeVal start;
                start = toolbox::TimeVal::gettimeofday() + interval;
                timer->schedule( this, start,  0, "" );
            #endif

        }


		LocalErrorDispatcher::~LocalErrorDispatcher()
		{
    		if (fileOutIsOpen_ == true)
		    {
    			//std::cout << "DEBUG : Closing log file" << std::endl;
    			if (fileOut_ != NULL) fclose(fileOut_);
		    }


    		if (fileOutPixelsXmlIsOpen_ == true)
		    {
    			//std::cout << "DEBUG : Closing log file" << std::endl;
    			if (fileOutPixelsXml_ != NULL) fclose(fileOutPixelsXml_);
		    }

    		if (fileOutPixelsLog4jIsOpen_ == true)
		    {
    			//std::cout << "DEBUG : Closing log file" << std::endl;
    			if (fileOutPixelsLog4j_ != NULL) fclose(fileOutPixelsLog4j_);
		    }


			//ERRDBRELATED
/*
			if (xmlFecOutputFileFirstOpen == false)
			{
				CLog l;
				std::string systemCommand;
				std::string fileName;

				// std::cout << "Opening XML output file for FECs" << std::endl;
				fileName = (std::string)LOG_FILES_ROOT + (std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
				xmlFecOutputFile_ = l.OpenXmlOutputFile(	fileName.c_str(),
															XML_APPEND_ACESS);
				// std::cout << "Writing XML output file END_HEADER for FECs" << std::endl;
				l.WriteXmlOutputFileEndHeader( xmlFecOutputFile_);

				// std::cout << "Closing XML output file for FECs" << std::endl;
				l.CloseXmlOutputFile(xmlFecOutputFile_);
				systemCommand = (std::string)MOVE_COMMAND +
								(std::string)LOG_FILES_ROOT + (std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION +
								(std::string)" " +
								(std::string)LOG_FILES_ROOT + (std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION + (std::string)"." + getDateTagForFile();
				system(systemCommand.c_str());


				systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
				system(systemCommand.c_str());

				xmlFecOutputFileFirstOpen = true;
			}
			if (xmlFedOutputFileFirstOpen == false)
			{
				CLog l;
				std::string systemCommand;
				std::string fileName;

				// std::cout << "Opening XML output file for FEDs" << std::endl;
				fileName = (std::string)LOG_FILES_ROOT + (std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
				xmlFedOutputFile_ = l.OpenXmlOutputFile(	fileName.c_str(),
															XML_APPEND_ACESS);

				// std::cout << "Writing XML output file END_HEADER for FEDs" << std::endl;
				l.WriteXmlOutputFileEndHeader( xmlFedOutputFile_);

				// std::cout << "Closing XML output file for FEDs" << std::endl;
				l.CloseXmlOutputFile(xmlFedOutputFile_);

				systemCommand = (std::string)MOVE_COMMAND +
								(std::string)LOG_FILES_ROOT + (std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION +
								(std::string)" " +
								(std::string)LOG_FILES_ROOT + (std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION + (std::string)"." + getDateTagForFile();
				system(systemCommand.c_str());


				systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
				system(systemCommand.c_str());


				xmlFedOutputFileFirstOpen = true;
			}
			if (xmlOtherOutputFileFirstOpen == false)
			{
				CLog l;
				std::string systemCommand;
				std::string fileName;

				// std::cout << "Opening XML output file for OTHERs" << std::endl;
				fileName = (std::string)LOG_FILES_ROOT + (std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;

				xmlOtherOutputFile_ = l.OpenXmlOutputFile(	fileName.c_str(),
															XML_APPEND_ACESS);

				// std::cout << "Writing XML output file END_HEADER for OTHERs" << std::endl;
				l.WriteXmlOutputFileEndHeader( xmlOtherOutputFile_);

				// std::cout << "Closing XML output file for OTHERs" << std::endl;
				l.CloseXmlOutputFile(xmlOtherOutputFile_);
				systemCommand = (std::string)MOVE_COMMAND +
								(std::string)LOG_FILES_ROOT + (std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION +
								(std::string)" " +
								(std::string)LOG_FILES_ROOT + (std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION + (std::string)"." + getDateTagForFile();
				system(systemCommand.c_str());

				systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
				system(systemCommand.c_str());


				xmlOtherOutputFileFirstOpen = true;
			}
*/

		}



        /* Returns the HTML page displayed from the Default() method when the <default_page> link is clicked */
        void LocalErrorDispatcher::getDefault1Content(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            *out << cgicc::p() << std::endl;
            *out << "Local Error Dispatcher Default page is empty at the moment";
            *out << cgicc::p() << std::endl;
        }


        /* Returns the HTML page displayed from the Default() method when the <access_fsm> link is clicked */
        void LocalErrorDispatcher::getFsmStatePage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
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
/*
        void getLogsSummaryPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            *out << cgicc::p() << std::endl;
            *out << "SUMMARY PAGE - UNDER CONSTRUCTION" << std::endl;
            *out << cgicc::p() << std::endl;

        }
*/

        /* Returns the HTML page displayed from the Default() method when the <default_page> link is clicked */
        void LocalErrorDispatcher::getVisualizeLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
			getIFrame(in, out);

        }



        /* xgi method called when the link <default_page> is clicked */
        void LocalErrorDispatcher::Default1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            internalState_ = 0;
            Default(in, out);
        }

        /* xgi method called when the link <display_diagsystem> is clicked */
        void LocalErrorDispatcher::callDiagSystemPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            internalState_ = 1;
            Default(in, out);
        }

        /* xgi method called when the link <display_fsm> is clicked */
        void LocalErrorDispatcher::callFsmPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            internalState_ = 2;
            Default(in, out);
        }

        /* xgi method called when the link <display_logs> is clicked */
        void LocalErrorDispatcher::callVisualizeLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
	    internalState_ = 3;
            Default(in, out);
        }
/*
        void callLogsSummaryPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
	    internalState_ = 4;
            Default(in, out);
        }
*/



        void LocalErrorDispatcher::genericSort(int rowOffset, bool sortGrowing)
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



        void LocalErrorDispatcher::sortLognumbers(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(LNOFFSET, sortLognumbersGrowing);
            if (sortLognumbersGrowing == true) {sortLognumbersGrowing = false;} else {sortLognumbersGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        /* xgi method called when the link <default_page> is clicked */
        void LocalErrorDispatcher::sortMessages(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(MSGOFFSET, sortMessagesGrowing);
            if (sortMessagesGrowing == true) {sortMessagesGrowing = false;} else {sortMessagesGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }


        void LocalErrorDispatcher::sortOrigin(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(ORGOFFSET, sortOriginGrowing);
            if (sortOriginGrowing == true) {sortOriginGrowing = false;} else {sortOriginGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void LocalErrorDispatcher::sortTimestamp(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(TSOFFSET, sortTimestampGrowing);
            if (sortTimestampGrowing == true) {sortTimestampGrowing = false;} else {sortTimestampGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void LocalErrorDispatcher::sortLevel(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(LVLOFFSET, sortLevelGrowing);
            if (sortLevelGrowing == true) {sortLevelGrowing = false;} else {sortLevelGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void LocalErrorDispatcher::sortErrcode(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(ECOFFSET, sortErrcodeGrowing);
            if (sortErrcodeGrowing == true) {sortErrcodeGrowing = false;} else {sortErrcodeGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void LocalErrorDispatcher::sortFaultstate(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(FSOFFSET, sortFaultstateGrowing);
            if (sortFaultstateGrowing == true) {sortFaultstateGrowing = false;} else {sortFaultstateGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void LocalErrorDispatcher::sortSysid(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(SIDOFFSET, sortSysidGrowing);
            if (sortSysidGrowing == true) {sortSysidGrowing = false;} else {sortSysidGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void LocalErrorDispatcher::sortSubsysid(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(SSIDOFFSET, sortSubsysidGrowing);
            if (sortSubsysidGrowing == true) {sortSubsysidGrowing = false;} else {sortSubsysidGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void LocalErrorDispatcher::sortEdtimestamp(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(EDTSOFFSET, sortEdtimestampGrowing);
            if (sortEdtimestampGrowing == true) {sortEdtimestampGrowing = false;} else {sortEdtimestampGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }



        void LocalErrorDispatcher::getIFrame(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
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
        void LocalErrorDispatcher::displayLinks(xgi::Input * in, xgi::Output * out)
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
            *out << "<a href=" << urlDiag_ << ">Configure Local Error Dispatcher</a>";
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

/*
            *out << "&nbsp;&nbsp;&nbsp;&nbsp;";

            // Display diagnostic page
            std::string urlSummary_ = "/";
            urlSummary_ += getApplicationDescriptor()->getURN();
            urlSummary_ += "/callLogsSummaryPage";
            *out << "<a href=" << urlSummary_ << ">Summarize received Logs</a>";
*/

            *out << "<br>";
            *out << cgicc::p() << std::endl;
        }




        /* Default() hyperDaq method */
        void LocalErrorDispatcher::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {

            /* Create HTML header */
            *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
            *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
            *out << cgicc::title("Local Error Dispatcher") << std::endl;
            xgi::Utils::getPageHeader(  out, 
                                        "Local Error Dispatcher", 
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
/*
            if (internalState_ == 4) getLogsSummaryPage(in, out);
*/
            /* Create HTML footer */
            xgi::Utils::getPageFooter(*out);
        }






        /* converts DiagSystem loglevel format into OWN loglevel format */
        long LocalErrorDispatcher::getLogLevelToLong(std::string logLevel)
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
        xoap::MessageReference LocalErrorDispatcher::receivedLog (xoap::MessageReference msg) throw (xoap::exception::Exception)
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
/*

			//Expand Logger
			std::string processName;
			std::string processInstance;
			std::string machine;
			std::string port;
			l.expandLoggerInfos(&processName, &processInstance, &machine, &port);
			//Expand Timestamp
			std::string logTimeStamp = l.expandTimeStamp();



			if (processName == (std::string)"FecSupervisor")
			{
				fecLogCounter++;
				// std::cout << "Opening XML output file for FECs" << std::endl;
				std::string fileName = (std::string)LOG_FILES_ROOT + (std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
				xmlFecOutputFile_ = l.OpenXmlOutputFile(	fileName.c_str(),
														XML_APPEND_ACESS);
				if (xmlFecOutputFileFirstOpen == true)
				{
					// std::cout << "Writing XML output file Pre-header for FECs" << std::endl;
					l.WriteXmlOutputFilePreHeader( 	xmlFecOutputFile_,
													XML_VERSION,
													XML_ENCODING,
													XML_DOCTYPE_ROOT);

					// std::cout << "Writing XML output file header for FECs" << std::endl;
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

				// std::cout << "Writing XML output file data set for FECs" << std::endl;
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
					// std::cout << "Writing XML output file END_HEADER for FECs" << std::endl;
					l.WriteXmlOutputFileEndHeader( xmlFecOutputFile_);
					// std::cout << "Closing XML output file for FECs" << std::endl;
					l.CloseXmlOutputFile(xmlFecOutputFile_);

					std::string systemCommand = (std::string)MOVE_COMMAND +
								(std::string)LOG_FILES_ROOT + (std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION +
								(std::string)" " +
								(std::string)LOG_FILES_ROOT + (std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION + (std::string)"." + getDateTagForFile();
					system(systemCommand.c_str());

					systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_FEC_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
					system(systemCommand.c_str());


					xmlFecOutputFileFirstOpen = true;
				}
				else
				{
					// std::cout << "Closing XML output file for FECs" << std::endl;
					l.CloseXmlOutputFile(xmlFecOutputFile_);
				}

			}//End FEC case processing



			if (processName == (std::string)"Fed9USupervisor")
			{
				fedLogCounter++;

				// std::cout << "Opening XML output file for FEDs" << std::endl;
				std::string fileName = (std::string)LOG_FILES_ROOT + (std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
				xmlFedOutputFile_ = l.OpenXmlOutputFile(fileName.c_str(),
														XML_APPEND_ACESS);
				if (xmlFedOutputFileFirstOpen == true)
				{
					// std::cout << "Writing XML output file Pre-header for FEDs" << std::endl;
					l.WriteXmlOutputFilePreHeader( 	xmlFedOutputFile_,
													XML_VERSION,
													XML_ENCODING,
													XML_DOCTYPE_ROOT);

					// std::cout << "Writing XML output file header for FEDs" << std::endl;
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

				// std::cout << "Writing XML output file data set for FEDs" << std::endl;
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
					// std::cout << "Writing XML output file END_HEADER for FEDs" << std::endl;
					l.WriteXmlOutputFileEndHeader( xmlFedOutputFile_);
					// std::cout << "Closing XML output file for FEDs" << std::endl;
					l.CloseXmlOutputFile(xmlFedOutputFile_);
					std::string systemCommand = (std::string)MOVE_COMMAND +
								(std::string)LOG_FILES_ROOT + (std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION +
								(std::string)" " +
								(std::string)LOG_FILES_ROOT + (std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION + (std::string)"." + getDateTagForFile();
					system(systemCommand.c_str());

					systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_FED_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
					system(systemCommand.c_str());

					xmlFedOutputFileFirstOpen = true;
				}
				else
				{
					// std::cout << "Closing XML output file for FEDs" << std::endl;
					l.CloseXmlOutputFile(xmlFedOutputFile_);
				}

			}//End FED case processing



			if ( (processName != (std::string)"FecSupervisor") && (processName != (std::string)"Fed9USupervisor") )
			{
				otherLogCounter++;

				// std::cout << "Opening XML output file for OTHERs" << std::endl;
				std::string fileName = (std::string)LOG_FILES_ROOT + (std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
				xmlOtherOutputFile_ = l.OpenXmlOutputFile(	fileName.c_str(),
															XML_APPEND_ACESS);
				if (xmlOtherOutputFileFirstOpen == true)
				{
					// std::cout << "Writing XML output file Pre-header for OTHERs" << std::endl;
					l.WriteXmlOutputFilePreHeader( 	xmlOtherOutputFile_,
													XML_VERSION,
													XML_ENCODING,
													XML_DOCTYPE_ROOT);

					// std::cout << "Writing XML output file header for OTHERs" << std::endl;
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

				// std::cout << "Writing XML output file data set for OTHERs" << std::endl;
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
					// std::cout << "Writing XML output file END_HEADER for OTHERs" << std::endl;
					l.WriteXmlOutputFileEndHeader( xmlOtherOutputFile_);
					// std::cout << "Closing XML output file for OTHERs" << std::endl;
					l.CloseXmlOutputFile(xmlOtherOutputFile_);

					std::string systemCommand = (std::string)MOVE_COMMAND +
								(std::string)LOG_FILES_ROOT + (std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION +
								(std::string)" " +
								(std::string)LOG_FILES_ROOT + (std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION + (std::string)"." + getDateTagForFile();
					system(systemCommand.c_str());

					systemCommand = (std::string)REMOVE_COMMAND + (std::string)LOG_FILES_ROOT +(std::string)XML_OTHER_LOGFILE_NAME + (std::string)XML_LOG_FILES_EXTENSION;
					system(systemCommand.c_str());

					xmlOtherOutputFileFirstOpen = true;
				}
				else
				{
					// std::cout << "Closing XML output file for OTHERs" << std::endl;
					l.CloseXmlOutputFile(xmlOtherOutputFile_);
				}

			}//End OTHER case processing
*/
			//ERRDBRELATED - END






            arrayliner++;
            if (arrayliner >= LOGARRAYLINES) arrayliner=0;


			/* Verify first that no reconfiguration action is needed, accorded to log content.*/
			checkIfReconfigurationIsNeeded(&l);




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

							if (fileOutPixelsXml_ != NULL) fclose(fileOutPixelsXml_);
							fileOutPixelsXmlIsOpen_ = false;

							if (fileOutPixelsLog4j_ != NULL) fclose(fileOutPixelsLog4j_);
							fileOutPixelsLog4jIsOpen_ = false;

						}
						bool logFileNameHasFlipped = false;
    	    	    	//Reset File if first log
				    	if (fileOutIsOpen_ == false)
						{
							std::ostringstream thisInstanceAsText;
							thisInstanceAsText << getApplicationDescriptor()->getInstance();
							std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + getDateTagForFile() + (std::string)"--Instance" + thisInstanceAsText.str() +(std::string)LOGFILENAMESUFFIX;

							diagService_->setFilePathAndName(lclLogFileName);
							if (diagService_->logFileNameHasChanged_ == true)
							{

								//diagService_->logFileName_ = LOGFILENAMEPREFIX + getDateTagForFile + LOGFILENAMESUFFIX;
								fileOut_ = fopen(diagService_->logFileName_.c_str(), "w");
								diagService_->logFileNameHasChanged_ = false;
								logFileNameHasFlipped = true;
							}
							else
							{
								//diagService_->logFileName_ = LOGFILENAMEPREFIX + getDateTagForFile + LOGFILENAMESUFFIX;
								fileOut_ = fopen(diagService_->logFileName_.c_str(), "a");
							}

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


/*
if ( isRunningForPixels_ == true )
{
*/
				    	if (fileOutPixelsXmlIsOpen_ == false)
						{
							std::ostringstream thisInstanceAsText;
							thisInstanceAsText << getApplicationDescriptor()->getInstance();
							std::string lclPixelsXmlLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + getDateTagForFile() + (std::string)PIXELSXMLEXTENSION + (std::string)"--Instance" + thisInstanceAsText.str() + (std::string)LOGFILENAMESUFFIX;
							pixelsXmlLogFileName_ = lclPixelsXmlLogFileName;
							fileOutPixelsXml_ = fopen(lclPixelsXmlLogFileName.c_str(), "w");

    		        	    if (fileOutPixelsXml_ == NULL)
    	    	        	{
	            	        	std::stringstream errMsg;				
	                	    	std::cout << "Error : Unable to open destination log file \"" << lclPixelsXmlLogFileName << "\". Maybe a permissions problem?" << std::endl;
		                	    fileOutPixelsXmlUsable_ = false;
								fileOutPixelsXmlIsOpen_ = false;
    	    		        }
        			        else
    	        		    {
							    fileOutPixelsXmlIsOpen_ = true;
		                	    fileOutPixelsXmlUsable_ = true;
								std::cout << "LogFile \"" << lclPixelsXmlLogFileName << "\" opened in write mode" << std::endl;
		    	            }
						}

				    	if (fileOutPixelsLog4jIsOpen_ == false)
						{
							std::ostringstream thisInstanceAsText;
							thisInstanceAsText << getApplicationDescriptor()->getInstance();
							std::string lclPixelsLog4jLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + getDateTagForFile() + (std::string)PIXELSLOG4JEXTENSION + (std::string)"--Instance" + thisInstanceAsText.str() + (std::string)LOGFILENAMESUFFIX;
							pixelsLog4jLogFileName_ = lclPixelsLog4jLogFileName;
							fileOutPixelsLog4j_ = fopen(lclPixelsLog4jLogFileName.c_str(), "w");

    		        	    if (fileOutPixelsLog4j_ == NULL)
    	    	        	{
	            	        	std::stringstream errMsg;				
	                	    	std::cout << "Error : Unable to open destination log file \"" << lclPixelsLog4jLogFileName << "\". Maybe a permissions problem?" << std::endl;
		                	    fileOutPixelsLog4jUsable_ = false;
								fileOutPixelsLog4jIsOpen_ = false;
    	    		        }
        			        else
    	        		    {
							    fileOutPixelsLog4jIsOpen_ = true;
		                	    fileOutPixelsLog4jUsable_ = true;
								std::cout << "LogFile \"" << lclPixelsLog4jLogFileName << "\" opened in write mode" << std::endl;
		    	            }
						}








						if (fileOutPixelsXmlIsOpen_ == true)
						{
							if ( (fileLogsCounter+1) >= MAX_NBR_OF_GENERAL_FILE_LOGS)
							{
							
								std::cout << "closing pixels xml file " << pixelsXmlLogFileName_ << std::endl;
								if (fileOutPixelsXml_ != NULL) fclose(fileOutPixelsXml_);

								std::ostringstream thisInstanceAsText;
								thisInstanceAsText << getApplicationDescriptor()->getInstance();
								std::string lclPixelsXmlLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + getDateTagForFile() + (std::string)PIXELSXMLEXTENSION + (std::string)"--Instance" + thisInstanceAsText.str() + (std::string)LOGFILENAMESUFFIX;
								pixelsXmlLogFileName_ = lclPixelsXmlLogFileName;



								std::cout << "reopening pixels xml file as " << lclPixelsXmlLogFileName << std::endl;
								fileOutPixelsXml_ = fopen(pixelsXmlLogFileName_.c_str(), "w");
	    		        	    if (fileOutPixelsXml_ == NULL)
    		    	        	{
		            	        	std::stringstream errMsg;				
		                	    	std::cout << "Error : Unable to open destination log file \"" << lclPixelsXmlLogFileName << "\". Maybe a permissions problem?" << std::endl;
			                	    fileOutPixelsXmlUsable_ = false;
									fileOutPixelsXmlIsOpen_ = false;
	    	    		        }
								else
								{
								    fileOutPixelsXmlIsOpen_ = true;
			                	    fileOutPixelsXmlUsable_ = true;
									std::cout << "LogFile \"" << lclPixelsXmlLogFileName << "\" opened in write mode" << std::endl;

									//int pixType = -1;
									//l.removeFromExtraBuffer((std::string)"<PIXELS_ERROR_TYPE>", (std::string)"</PIXELS_ERROR_TYPE>", &pixType);
									fprintf(fileOutPixelsXml_,"%s",l.buildStringForPixelsLogFileOutput(fileLogsCounter).c_str());
									fflush(fileOutPixelsXml_);
								}
							}
							else
							{
								//int pixType = -1;
								//l.removeFromExtraBuffer((std::string)"<PIXELS_ERROR_TYPE>", (std::string)"</PIXELS_ERROR_TYPE>", &pixType);
								fprintf(fileOutPixelsXml_,"%s",l.buildStringForPixelsLogFileOutput(fileLogsCounter).c_str());
								fflush(fileOutPixelsXml_);
							}
						}



						if (fileOutPixelsLog4jIsOpen_ == true)
						{
							if ( (fileLogsCounter+1) >= MAX_NBR_OF_GENERAL_FILE_LOGS)
							{
							
								std::cout << "closing pixels log4j file " << pixelsLog4jLogFileName_ << std::endl;
								if (fileOutPixelsLog4j_ != NULL) fclose(fileOutPixelsLog4j_);

								std::ostringstream thisInstanceAsText;
								thisInstanceAsText << getApplicationDescriptor()->getInstance();
								std::string lclPixelsLog4jLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + getDateTagForFile() + (std::string)PIXELSLOG4JEXTENSION + (std::string)"--Instance" + thisInstanceAsText.str() + (std::string)LOGFILENAMESUFFIX;
								pixelsLog4jLogFileName_ = lclPixelsLog4jLogFileName;



								std::cout << "reopening pixels log4j file as " << lclPixelsLog4jLogFileName << std::endl;
								fileOutPixelsLog4j_ = fopen(pixelsLog4jLogFileName_.c_str(), "w");
	    		        	    if (fileOutPixelsLog4j_ == NULL)
    		    	        	{
		            	        	std::stringstream errMsg;				
		                	    	std::cout << "Error : Unable to open destination log file \"" << lclPixelsLog4jLogFileName << "\". Maybe a permissions problem?" << std::endl;
									fileOutPixelsLog4jIsOpen_ = false;
			                	    fileOutPixelsLog4jUsable_ = false;
 	    	    		        }
								else
								{
								    fileOutPixelsLog4jIsOpen_ = true;
			                	    fileOutPixelsLog4jUsable_ = true;
									std::cout << "LogFile \"" << lclPixelsLog4jLogFileName << "\" opened in write mode" << std::endl;

									fprintf(fileOutPixelsLog4j_,"%s",l.buildStringForLog4jLogFileOutput(fileLogsCounter).c_str());
									fflush(fileOutPixelsLog4j_);
								}
							}
							else
							{
								fprintf(fileOutPixelsLog4j_,"%s",l.buildStringForLog4jLogFileOutput(fileLogsCounter).c_str());
								fflush(fileOutPixelsLog4j_);
							}
						}

						
/*
}
*/
						if (fileOutIsOpen_ == true)
						{
							fileLogsCounter++;
							if (fileLogsCounter >= MAX_NBR_OF_GENERAL_FILE_LOGS)
							{
								std::cout << "closing general file " << diagService_->logFileName_ << std::endl;
								if (fileOut_ != NULL) fclose(fileOut_);

								std::ostringstream thisInstanceAsText;
								thisInstanceAsText << getApplicationDescriptor()->getInstance();
								std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + getDateTagForFile() + (std::string)"--Instance" + thisInstanceAsText.str() +(std::string)LOGFILENAMESUFFIX;

								diagService_->setFilePathAndName(lclLogFileName);
								
								std::cout << "reopening general file as " << diagService_->logFileName_ << std::endl;
								//REOPEN WITH "w"
								fileOut_ = fopen(diagService_->logFileName_.c_str(), "w");
	    		        	    if (fileOut_ == NULL)
    		    	        	{
	    	        	        	std::stringstream errMsg;				
	        	        	    	std::cout << "Error : Unable to write to destination log file \"" << diagService_->logFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
		        	        	    diagService_->fileLoggingUsable_ = false;
									fileOutIsOpen_ = false;
	    	    		        }
								else
								{
									std::cout << "LogFile \"" << diagService_->logFileName_.c_str() << "\" opened in write mode" << std::endl;
									#ifdef WILL_COMPILE_FOR_TRACKER
										fprintf(fileOut_,"%s",l.buildStringForTrackerLogFileOutput(fileLogsCounter, diagService_->runFromRcms_).c_str());
									#else
										#ifdef WILL_COMPILE_FOR_PIXELS
											fprintf(fileOut_,"%s",l.buildStringForPixelsLogFileOutput(fileLogsCounter).c_str());
										#else
											fprintf(fileOut_,"%s",l.buildStringForLogFileOutput(fileLogsCounter).c_str());
										#endif
									#endif
									fflush(fileOut_);
								}
								//Reset thing
								fileLogsCounter=0;
							}
							else
							{
									#ifdef WILL_COMPILE_FOR_TRACKER
										fprintf(fileOut_,"%s",l.buildStringForTrackerLogFileOutput(fileLogsCounter, diagService_->runFromRcms_).c_str());
									#else
										#ifdef WILL_COMPILE_FOR_PIXELS
											fprintf(fileOut_,"%s",l.buildStringForPixelsLogFileOutput(fileLogsCounter).c_str());
										#else
											fprintf(fileOut_,"%s",l.buildStringForLogFileOutput(fileLogsCounter).c_str());
										#endif
									#endif
								fflush(fileOut_);
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
                        //diagService_->chainSawManager_->pushClogToStream(diagService_->csReceiver_[csStreamNum], l);
                        diagService_->pushClogToFilteredStreams(l, csStreamNum);
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


            return reply;
        }





        /* Turns the LocalErrorDispatcher into state INITIALISED (FSM point of view) */
        void LocalErrorDispatcher::configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
        {
            /* Apply fireitems value to internal variables and check validity */
            DIAG_EXEC_FSM_INIT_TRANS
        }



        /* Turns the LocalErrorDispatcher into state HALTED (FSM point of view) */
        void LocalErrorDispatcher::haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
        {
            /* Apply fireitems value to internal variables and check validity */
            DIAG_EXEC_FSM_STOP_TRANS
        }
   

   
        /* Allows the FSM to be controlled via a soap message carrying a "fireEvent" message */
        xoap::MessageReference LocalErrorDispatcher::fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception)
        {
            //#define DEBUG_FIREEVENT
            #ifdef DEBUG_FIREEVENT
                std::cout << "In LocalErrorDispatcher : fireEvent received" << std::endl ;
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
        void LocalErrorDispatcher::failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
        {
            toolbox::fsm::FailedEvent & fe = dynamic_cast<toolbox::fsm::FailedEvent&>(*e);
            std::ostringstream msgError;
            msgError << "Failure occurred when performing transition from: " << fe.getFromState() << " to: " << fe.getToState() << " exception: " << fe.getException().what();
            diagService_->reportError (msgError.str(), DIAGERROR);
        }



        /* Can be called via the <configure_FSM> hyperdaq HTML page of this process */
        void LocalErrorDispatcher::configureStateMachine(xgi::Input * in, xgi::Output * out)
        {
            if (fsm_.getCurrentState() == 'H')
            {
                toolbox::Event::Reference e(new toolbox::Event(EXECTRANSHC, this));
                fsm_.fireEvent(e);
            }
            Default(in, out);
        }


        /* Can be called via the <configure_FSM> hyperdaq HTML page of this process */
        void LocalErrorDispatcher::stopStateMachine(xgi::Input * in, xgi::Output * out)
        {
            if (fsm_.getCurrentState() == 'C')
            {
                toolbox::Event::Reference e(new toolbox::Event(EXECTRANSCH, this));
                fsm_.fireEvent(e);
            }
                Default(in, out);
        }


        #ifdef AUTO_CONFIGURE_PROCESSES
            void LocalErrorDispatcher::timeExpired (toolbox::task::TimerEvent& e) 
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

						std::ostringstream thisInstanceAsText;
						thisInstanceAsText << getApplicationDescriptor()->getInstance();
						std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + getDateTagForFile() + (std::string)"--Instance" + thisInstanceAsText.str() +(std::string)LOGFILENAMESUFFIX;
//						std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + getDateTagForFile() + (std::string)LOGFILENAMESUFFIX;

						diagService_->setFilePathAndName(lclLogFileName);

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
//							diagService_->oldLogFileName_ = 
	    	            }

					}
				}
                std::cout << "LocalErrorDispatcher(LID=" << getApplicationDescriptor()->getLocalId() << ",Instance=" << getApplicationDescriptor()->getInstance() << ") ready" << std::endl;
			
            }
        #endif






        /* Read the SOAP message and create a CLog structure
        DO NOT USE LOG4C CALLS IN THIS METHOD, IT IS NOT REENTRANT */
        void LocalErrorDispatcher::getMessage(xoap::MessageReference m, CLog * l) throw (xoap::exception::Exception)
        {
            xoap::SOAPBody rb = m->getSOAPPart().getEnvelope().getBody();
            if (rb.hasFault() )
            {
                xoap::SOAPFault fault = rb.getFault();
                std::string msg = "LocalErrorDispatcher: ";
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
						//std::cout << "In LocalErrorDispatcher : ExtraBuffer = " << l->getExtraBuffer() << std::endl;

/*
		    		}
*/


                } /* End of if (logsize==1) */
            } /* End of No BODY FAULT block */
            /* MUST manage exceptions here */
        }




		void LocalErrorDispatcher::checkIfReconfigurationIsNeeded(CLog * l) throw (xoap::exception::Exception)
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
					diagService_->reportError("Send a reconfiguration request Log with ErrorCode 200 from GED",DIAGUSERINFO,
					"RECONFCMD",
					200,
					"STEADY",
					"TRACKER",
					"TEC");
				}
				else
				{
					std::cout << "Impossible to execute the requested reconfiguration command, previous one is still running." << std::endl;
				}

			}
		}


        xoap::MessageReference LocalErrorDispatcher::freeReconfCmd (xoap::MessageReference msg) throw (xoap::exception::Exception)
        {
            /* post reply to caller */
            xoap::MessageReference reply = xoap::createMessage();
            xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
            xoap::SOAPName responseName = envelope.createName("ReconfigurationModuleResponse", "xdaq", XDAQ_NS_URI);
            envelope.getBody().addBodyElement ( responseName );

			reconfRunning__ = false;
			return reply;
		}



/*
        std::string LocalErrorDispatcher::getDateTagForFile(void)
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


			return (std::string)logTimeStamp1;
		}
*/




        std::string LocalErrorDispatcher::getDateTagForFile(void)
		{

			struct timeval runTimeStamptv;
			struct tm * runTimeStampSplittedPtr;
			char runTimeStamp[100];

			//set run start timestamp
			gettimeofday(&runTimeStamptv,NULL);
			runTimeStampSplittedPtr = localtime(&runTimeStamptv.tv_sec);
			snprintf(runTimeStamp, 22, "%.4d-%.2d-%.2d--%.2d-%.2d-%.2d",
						(runTimeStampSplittedPtr->tm_year + 1900),
						runTimeStampSplittedPtr->tm_mon+1,
						runTimeStampSplittedPtr->tm_mday,
						runTimeStampSplittedPtr->tm_hour,
						runTimeStampSplittedPtr->tm_min,
						runTimeStampSplittedPtr->tm_sec);
			return (std::string)runTimeStamp;
		}
		
		
		
		
		

xoap::MessageReference LocalErrorDispatcher::getCurrentLogFileName(xoap::MessageReference msg) throw (xoap::exception::Exception)
{
	// Prepare reply body	
    xoap::MessageReference reply = xoap::createMessage();
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPName responseName = envelope.createName( "getLogFileNameReply", "xdaq", XDAQ_NS_URI);
	envelope.getBody().addBodyElement ( responseName );


	/* Add requested attributes to SOAP part */
	xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
	std::vector<xoap::SOAPElement> logElement = rb.getChildElements ();

	xoap::SOAPName logFileName ("logFileName", "", "");
	logElement[0].addAttribute(logFileName,diagService_->logFileName_);
	return reply;

}

