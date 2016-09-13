/*

   FileName : 		GlobalErrorDispatcher.cc

   Content : 		GlobalErrorDispatcher module

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
the Free Software Foundation; either version 2 of the License, or*
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


#include "GlobalErrorDispatcher.h"


XDAQ_INSTANTIATOR_IMPL(GlobalErrorDispatcher)

	#ifdef WILL_USE_GWT_FIREITEMS_VIEWER
		GlobalErrorDispatcher::GlobalErrorDispatcher(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): xdaq::Application(s), JsInterface(s)
	#else
		GlobalErrorDispatcher::GlobalErrorDispatcher(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): xdaq::Application(s)
	#endif
        {
//std::cout << "Initializing state machine states and transitions" << std::endl;





            /* Liste des etats qu'on va trouver dans la FSM */
            fsm_.addState ('C', "Configured") ;
            fsm_.addState ('H', "Halted");

            /* Methode initialiseAction qui doit gerer les constructions, etc... qui surviennent lors de la
            transition entre l'etat Initial et l'etat Halted */
            fsm_.addStateTransition ('C','H', EXECTRANSCH, this, &GlobalErrorDispatcher::haltAction);
            fsm_.addStateTransition ('H','C', EXECTRANSHC, this, &GlobalErrorDispatcher::configureAction);

            /* Si une transition ne peut se faire, on appel la methode failedTransition */
            fsm_.setFailedStateTransitionAction( this, &GlobalErrorDispatcher::failedTransition );

//std::cout << "Forcing initial state to HALTED" << std::endl;
            /* On synchronise la FASM avec l'etat du process apres load, a savoir : etat HALTED */
            fsm_.setInitialState('H');
            fsm_.reset();

            /* Import/Export the stateName variable */
            getApplicationInfoSpace()->fireItemAvailable("stateName", &fsm_.stateName_);
//std::cout << "Work on FSM done" << std::endl;



			//By default, set filename xtension to : generic
			xmlFileNameSpecificExtension_ = GENERICXMLEXTENSION;

			//Then, check if compilation is specific...
			# ifdef WILL_COMPILE_FOR_PIXELS
				xmlFileNameSpecificExtension_ = PIXELSXMLEXTENSION;
			#endif


			# ifdef WILL_COMPILE_FOR_TRACKER
				xmlFileNameSpecificExtension_ = TRACKERXMLEXTENSION;
			#endif
/*

	#ifdef DATABASE
		tkDiagErrorAnalyser = NULL;
		tkTibDiagErrorAnalyser = NULL;
		tkTobDiagErrorAnalyser = NULL;
		tkTecpDiagErrorAnalyser = NULL;
		tkTecmDiagErrorAnalyser = NULL;
		isDbUsable = false;

		DbPartitionName_ = "";

		for (int i=0; i<DIAG_MAX_PARTITIONS_NBR; i++)
		{
			DbPartitionNameArray_[i] = "";
		}
		part_counter_ = 0;
		
		
		tibPartitionName_ = "";
		tobPartitionName_ = "";
		tecpPartitionName_ = "";
		tecmPartitionName_ = "";

		isRunningOnP5Machines_ = false;
	#endif
*/
	isFirstTimerTick_ = true;



			RCMSMutex_ = new toolbox::BSem(toolbox::BSem::FULL);

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

//P5MOD
#ifdef WILL_COMPILE_FOR_TRACKER
    	    dcuFileOutIsOpen_ = false;
			dcuFileOut_ = NULL;

    	    fedDebugFileOutIsOpen_ = false;
			fedDebugFileOut_ = NULL;

    	    fedTmonFileOutIsOpen_ = false;
			fedTmonFileOut_ = NULL;

    	    fedTmonFileOutIsOpen_ = false;
			fedTmonFileOut_ = NULL;

			dcuLogFileNameHasChanged_ = true;
			dcuOldLogFileName_ = "oldLogFileName_";
			dcuLogFileName_ = "logFileName_";

			fedDebugLogFileNameHasChanged_ = true;
			fedDebugOldLogFileName_ = "oldLogFileName_";
			fedDebugLogFileName_ = "logFileName_";

			fedSpyLogFileNameHasChanged_ = true;
			fedSpyOldLogFileName_ = "oldLogFileName_";
			fedSpyLogFileName_ = "logFileName_";
#endif
//P5MODEND



//P5MODBIS
			rnHasFlippedForStandardLogFile = false;
			#ifdef WILL_COMPILE_FOR_TRACKER
				rnHasFlippedForDcuLogFile = false;
				rnHasFlippedForFedDebugLogFile = false;
				rnHasFlippedForFedTmonLogFile = false;
				rnHasFlippedForFedSpyLogFile = false;
			#endif
//P5MODBISEND



//			GedTimerHasBeenFiredAtLeastOnce_ = false;
			
			reconfRunning__ = false;


//			isRunningForPixels_ = false;



//P5MODBIS
runNumberIsInitialized_ = false;
oldRunNumber_ = 0;
//currentRunNumber_ = 0;
//P5MODBISEND









			//ERRDBRELATED
/*
			xmlFecOutputFile_ = NULL;
			xmlFecOutputFileFirstOpen = true;
			xmlFedOutputFile_ = NULL;
			xmlFedOutputFileFirstOpen = true;
			xmlOtherOutputFile_ = NULL;
			xmlOtherOutputFileFirstOpen = true;
*/





/*

	#ifdef DATABASE
		// Which DB partition do we want to use ?
		//Extract from environment/RCMS or ask explicit ?
		readEnv=getenv ("ENV_CMS_TK_PARTITION") ;
		if ( (readEnv != NULL) && (std::string(readEnv) != "") )
		{
			//std::cout << "readEnv = " << readEnv << std::endl;
			partitionName = std::string(readEnv);
			DbPartitionName_ = partitionName;

			//Si on a trouve le DB PARTITION NAME, est ce qu'on est en mono parttiion ou en multi partition?
			std::string::size_type locA, locB;
			locA = 0;
			locB = 0;
			do
			{
				try
				{
				locB = DbPartitionName_.find( ":", locA );
//				if ( (locA != std::string::npos) && (locB != std::string::npos) ) DbPartitionNameArray_[part_counter_] = DbPartitionName_.substr(locA, locB-locA);
				DbPartitionNameArray_[part_counter_] = DbPartitionName_.substr(locA, locB-locA);
				}
				catch(...){}

//				if( locB != std::string::npos )
//				{
//					std::cout << "Found : occurence in string" << std::endl;
					//std::cout << "Substring is : " << DbPartitionName_.substr(locA, locB-locA) << std::endl;
					part_counter_++;
//				}
//				else
//				{
				
//				}
				locA = locB+1;
			} while (locB != std::string::npos);
			std::cout << "I'm working with " << part_counter_ << " partition(s)" << std::endl;
			for (int j=0; j<part_counter_; j++) std::cout << DbPartitionNameArray_[j] << std::endl;



			for (int k=0; k< part_counter_; k++)
			{
				std::string::size_type locTIB=0, locTOB=0, locTECP=0, locTECM=0;
				locTIB = DbPartitionNameArray_[k].find( tibPartitionPrefix, 0 );
				if (locTIB != std::string::npos)
				{
					tibPartitionName_ = DbPartitionNameArray_[k];
					isRunningOnP5Machines_ = true;
				}

				locTOB = DbPartitionNameArray_[k].find( tobPartitionPrefix, 0 );
				if (locTOB != std::string::npos)
				{
					tobPartitionName_ = DbPartitionNameArray_[k];
					isRunningOnP5Machines_ = true;
				}


				locTECP = DbPartitionNameArray_[k].find( tecpPartitionPrefix, 0 );
				if (locTECP != std::string::npos)
				{
					tecpPartitionName_ = DbPartitionNameArray_[k];
					isRunningOnP5Machines_ = true;
				}


				locTECM = DbPartitionNameArray_[k].find( tecmPartitionPrefix, 0 );
				if (locTECM != std::string::npos)
				{
					tecmPartitionName_ = DbPartitionNameArray_[k];
					isRunningOnP5Machines_ = true;
				}

			}
			
			if (isRunningOnP5Machines_ == true)
			{
				if (tibPartitionName_ != "") std::cout << "tibPartitionName_ = " << tibPartitionName_ << std::endl;
				if (tobPartitionName_ != "") std::cout << "tobPartitionName_ = " << tobPartitionName_ << std::endl;
				if (tecpPartitionName_ != "") std::cout << "tecpPartitionName_ = " << tecpPartitionName_ << std::endl;
				if (tecmPartitionName_ != "") std::cout << "tecmPartitionName_ = " << tecmPartitionName_ << std::endl;
			}
			else std::cout << "Running with NON-P5 partition = " << DbPartitionName_ << std::endl;



		}

		else
		{
			//std::cout << "readEnv is NULL " << std::endl;
			std::cerr << "Please set the ENV_CMS_TK_PARTITION environemental variable as ENV_CMS_TK_PARTITION=PartitionName" << std::endl ;
			DbPartitionName_ = "";
		}
	#endif

*/


            /* Give funny and useless informations at load time */
            std::stringstream mmesg;
            mmesg << "Process version " << GLOBALERRORDISPATCHER_PACKAGE_VERSION << " loaded";
            this->getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);
            LOG4CPLUS_INFO(this->getApplicationLogger(), mmesg.str());
            this->getApplicationLogger().setLogLevel(ERROR_LOG_LEVEL);

            /* Set Default() metyhod state as : initialising.... */
            internalState_ = 0;
//std::cout << "Initialising arrays" << std::endl;

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
            dumpedlogcounter = 0;
            fecLogCounter = 0;
            fedLogCounter = 0;
            otherLogCounter = 0;
            fileLogsCounter = 0;
            pixelsFileLogsCounter = 0;
            log4jFileLogsCounter = 0;
/*            dbFileLogsCounter = 0; */



						logFileNameHasFlipped = false;

//P5MOD
#ifdef WILL_COMPILE_FOR_TRACKER
            dcuErrorLogCounter = 0;
            fedDebugLogCounter = 0;
            fedTmonLogCounter = 0;
            fedSpyLogCounter = 0;


			dcuErrorFileLogCounter = 0;
			fedDebugFileLogCounter = 0;
			fedTmonFileLogCounter = 0;
			fedSpyFileLogCounter = 0;


            dcuDumpedlogcounter = 0;
			fedDebugDumpedlogcounter = 0;
			fedTmonDumpedlogcounter = 0;
			fedSpyDumpedlogcounter = 0;

			dcuLogFileNameHasFlipped = false;
			fedDebugLogFileNameHasFlipped = false;
			fedTempMonLogFileNameHasFlipped = false;
			fedSpyLogFileNameHasFlipped = false;

#endif
//P5MODEND





//std::cout << "Instanciating diagBagWizard object" << std::endl;

            /* Instanciate diagnostic object */
            diagService_ = new DiagBagWizard(
                                    getApplicationDescriptor()->getClassName() + "DiagLvlOne" ,
                                    this->getApplicationLogger(),
                                    getApplicationDescriptor()->getClassName(),
                                    getApplicationDescriptor()->getInstance(),
                                    getApplicationDescriptor()->getLocalId(),
                                    (xdaq::WebApplication *)this );


//			readSubDetectorTypeEnvSettings();
			readCsEnvSettings();
//std::cout << "Binding pages" << std::endl;

//GLAP
//std::cout << "Run number is " << diagService_->feRunFromRcms_ << std::endl;

            /* bind xgi and xoap commands specific to this application */
            xgi::bind(this,&GlobalErrorDispatcher::Default, "Default");
            xgi::bind(this,&GlobalErrorDispatcher::Default1, "Default1");
            xgi::bind(this,&GlobalErrorDispatcher::callDiagSystemPage, "callDiagSystemPage");
            xgi::bind(this,&GlobalErrorDispatcher::callFsmPage, "callFsmPage");
            xgi::bind(this,&GlobalErrorDispatcher::callVisualizeLogsPage, "callVisualizeLogsPage");
            xgi::bind(this,&GlobalErrorDispatcher::getIFrame, "getIFrame");
            xgi::bind(this,&GlobalErrorDispatcher::configureStateMachine, "configureStateMachine");
            xgi::bind(this,&GlobalErrorDispatcher::stopStateMachine, "stopStateMachine");
            xoap::bind(this, &GlobalErrorDispatcher::receivedLog, "receivedLog", XDAQ_NS_URI );
            xoap::bind(this, &GlobalErrorDispatcher::runNumberRequest, "runNumberRequest", XDAQ_NS_URI );
/*
            xgi::bind(this,&GlobalErrorDispatcher::callLogsSummaryPage, "callLogsSummaryPage");
*/
//RECONTEST
/* xoap::bind(this, &GlobalErrorDispatcher::reconfLog, "reconfLog", XDAQ_NS_URI ); */

            xoap::bind(this, &GlobalErrorDispatcher::freeReconfCmd, "freeReconfCmd", XDAQ_NS_URI );

            xgi::bind(this,&GlobalErrorDispatcher::sortLognumbers, "sortLognumbers");
            xgi::bind(this,&GlobalErrorDispatcher::sortMessages, "sortMessages");
            xgi::bind(this,&GlobalErrorDispatcher::sortOrigin, "sortOrigin");
            xgi::bind(this,&GlobalErrorDispatcher::sortTimestamp, "sortTimestamp");
            xgi::bind(this,&GlobalErrorDispatcher::sortLevel, "sortLevel");
            xgi::bind(this,&GlobalErrorDispatcher::sortErrcode, "sortErrcode");
            xgi::bind(this,&GlobalErrorDispatcher::sortFaultstate, "sortFaultstate");
            xgi::bind(this,&GlobalErrorDispatcher::sortSysid, "sortSysid");
            xgi::bind(this,&GlobalErrorDispatcher::sortSubsysid, "sortSubsysid");
            xgi::bind(this,&GlobalErrorDispatcher::sortEdtimestamp, "sortEdtimestamp");


//            xgi::bind(this,&GlobalErrorDispatcher::getJSONDataList, "getJSONDataList");


//            xgi::bind(this,&GlobalErrorDispatcher::callJsonLogsPage, "callJsonLogsPage");


            /* bind xgi commands needed for running a diagService in this application */
            xgi::bind(this,&GlobalErrorDispatcher::configureDiagSystem, "configureDiagSystem");
            xgi::bind(this,&GlobalErrorDispatcher::applyConfigureDiagSystem, "applyConfigureDiagSystem");

			//CallBack to inform LogReader process of the logFileName currently in use
            xoap::bind(this,&GlobalErrorDispatcher::getCurrentLogFileName, "getCurrentLogFileName", XDAQ_NS_URI);

//std::cout << "Calling DiagSystem declaration macros" << std::endl;

            /* Call a macro needed for running a diagService in this application */
            DIAG_DECLARE_GLB_ED_APP


            /* Bind SOAP callbacks for FSM control messages */
            xoap::bind (this, &GlobalErrorDispatcher::fireEvent, EXECTRANSCH, XDAQ_NS_URI);
            xoap::bind (this, &GlobalErrorDispatcher::fireEvent, EXECTRANSHC, XDAQ_NS_URI);

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



diagService_->logFileNameHasChanged_ = true;
//diagService_->fileLoggingUsable_ = true;
standardFileLoggingUsable_ = true;
//P5MOD
#ifdef WILL_COMPILE_FOR_TRACKER
dcuLogFileNameHasChanged_ = true;
fedDebugLogFileNameHasChanged_ = true;
fedTmonLogFileNameHasChanged_ = true;
fedSpyLogFileNameHasChanged_ = true;

dcuFileLoggingUsable_ = true;
fedDebugFileLoggingUsable_ = true;
fedTmonFileLoggingUsable_ = true;
fedSpyFileLoggingUsable_ = true;


#endif

            #ifdef AUTO_CONFIGURE_PROCESSES

                timerName_ << getApplicationDescriptor()->getContextDescriptor()->getURL() << ":";
                timerName_ << getApplicationDescriptor()->getClassName() << ":" << getApplicationDescriptor()->getLocalId() << ":" << getApplicationDescriptor()->getInstance();
                timer_ = toolbox::task::getTimerFactory()->createTimer(timerName_.str());
				toolbox::TimeInterval interval_(AUTO_GED_CONFIGURE_DELAY,0);
                start_ = toolbox::TimeVal::gettimeofday() + interval_;
//                timer_->scheduleAtFixedRate( start_, this, interval_, getApplicationDescriptor()->getContextDescriptor(), "" );


//		timer_->schedule( this, start_,  0, "" );
timer_->scheduleAtFixedRate( start_, this, interval_, this->getApplicationDescriptor()->getContextDescriptor(), "" );

//				std::cout << "Timer created and started" << std::endl;
            #endif



/*

	#ifdef DATABASE
	
//	if ( (readEnv != NULL) && (std::string(readEnv) != "") )
//	{
//std::cout << "Checking availability of DB accesses" << std::endl;

	// Create the database access
	// retreive the connection of the database through an environmental variable called CONFDB
	std::string login="nil", passwd="nil", path="nil" ;
	DbAccess::getDbConfiguration (login, passwd, path) ;
	DbLogin_ = login;
	DbPasswd_ = passwd;
	DbPath_ = path;
	if (login == "nil" || passwd=="nil" || path=="nil")
	{
		std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
	}
	else
	{
	
		if (isRunningOnP5Machines_ == false)
		{
			try
			{
				tkDiagErrorAnalyser = new TkDiagErrorAnalyser ( DbPartitionName_ ) ; //, login, passwd, path ) ;
				if (tkDiagErrorAnalyser->getConnectionNotIdentified() != 0) std::cerr << "Found " << tkDiagErrorAnalyser->getConnectionNotIdentified() << " connections not identified with DCU ID" << std::endl ;
				if (tkDiagErrorAnalyser->getPsuNotIdentified() != 0) std::cerr << "Found " << tkDiagErrorAnalyser->getPsuNotIdentified() << " PSU not identified with DCU hardware ID" << std::endl ;
				//Let's suppose that once this point reached the DB is usable
				if (tkDiagErrorAnalyser != NULL) isDbUsable = true;
			}
			catch (oracle::occi::SQLException &e)
			{
				std::cerr << "Cannot create the access to the database: " << login << "/" << "******" << "@" << path << std::endl ;
				std::cerr << e.what() << std::endl ;
			}
			catch (FecExceptionHandler &e)
			{
				std::cerr << "Cannot create the access to DB: " << login << "/" << "******" << "@" << path << std::endl ;
				std::cerr << e.what() << std::endl ;
			}
			catch (...)
			{
				std::cerr << "Unattended exception during DB access creation" << std::endl ;
			}
		}



		else
		{
			if (tibPartitionName_ != "")
			{
				try
				{
					tkTibDiagErrorAnalyser = new TkDiagErrorAnalyser ( tibPartitionName_ ) ; //, login, passwd, path ) ;
					if (tkTibDiagErrorAnalyser->getConnectionNotIdentified() != 0) std::cerr << "Found " << tkTibDiagErrorAnalyser->getConnectionNotIdentified() << " connections not identified with DCU ID" << std::endl ;
					if (tkTibDiagErrorAnalyser->getPsuNotIdentified() != 0) std::cerr << "Found " << tkTibDiagErrorAnalyser->getPsuNotIdentified() << " PSU not identified with DCU hardware ID" << std::endl ;
					//Let's suppose that once this point reached the DB is usable
					if (tkTibDiagErrorAnalyser != NULL) isDbUsable = true;
				}
				catch (oracle::occi::SQLException &e)
				{
					std::cerr << "Cannot create the access to the database: " << login << "/" << "******" << "@" << path << std::endl ;
					std::cerr << e.what() << std::endl ;
				}
				catch (FecExceptionHandler &e)
				{
					std::cerr << "Cannot create the access to DB: " << login << "/" << "******" << "@" << path << std::endl ;
					std::cerr << e.what() << std::endl ;
				}
				catch (...)
				{
					std::cerr << "Unattended exception during DB access creation" << std::endl ;
				}
			}
		



			if (tobPartitionName_ != "")
			{
				try
				{
					tkTobDiagErrorAnalyser = new TkDiagErrorAnalyser ( tobPartitionName_ ) ; //, login, passwd, path ) ;
					if (tkTobDiagErrorAnalyser->getConnectionNotIdentified() != 0) std::cerr << "Found " << tkTobDiagErrorAnalyser->getConnectionNotIdentified() << " connections not identified with DCU ID" << std::endl ;
					if (tkTobDiagErrorAnalyser->getPsuNotIdentified() != 0) std::cerr << "Found " << tkTobDiagErrorAnalyser->getPsuNotIdentified() << " PSU not identified with DCU hardware ID" << std::endl ;
					//Let's suppose that once this point reached the DB is usable
					if (tkTobDiagErrorAnalyser != NULL) isDbUsable = true;
				}
				catch (oracle::occi::SQLException &e)
				{
					std::cerr << "Cannot create the access to the database: " << login << "/" << "******" << "@" << path << std::endl ;
					std::cerr << e.what() << std::endl ;
				}
				catch (FecExceptionHandler &e)
				{
					std::cerr << "Cannot create the access to DB: " << login << "/" << "******" << "@" << path << std::endl ;
					std::cerr << e.what() << std::endl ;
				}
				catch (...)
				{
					std::cerr << "Unattended exception during DB access creation" << std::endl ;
				}
			}






			if (tecpPartitionName_ != "")
			{
				try
				{
					tkTecpDiagErrorAnalyser = new TkDiagErrorAnalyser ( tecpPartitionName_ ) ; //, login, passwd, path ) ;
					if (tkTecpDiagErrorAnalyser->getConnectionNotIdentified() != 0) std::cerr << "Found " << tkTecpDiagErrorAnalyser->getConnectionNotIdentified() << " connections not identified with DCU ID" << std::endl ;
					if (tkTecpDiagErrorAnalyser->getPsuNotIdentified() != 0) std::cerr << "Found " << tkTecpDiagErrorAnalyser->getPsuNotIdentified() << " PSU not identified with DCU hardware ID" << std::endl ;
					//Let's suppose that once this point reached the DB is usable
					if (tkTecpDiagErrorAnalyser != NULL) isDbUsable = true;
				}
				catch (oracle::occi::SQLException &e)
				{
					std::cerr << "Cannot create the access to the database: " << login << "/" << "******" << "@" << path << std::endl ;
					std::cerr << e.what() << std::endl ;
				}
				catch (FecExceptionHandler &e)
				{
					std::cerr << "Cannot create the access to DB: " << login << "/" << "******" << "@" << path << std::endl ;
					std::cerr << e.what() << std::endl ;
				}
				catch (...)
				{
					std::cerr << "Unattended exception during DB access creation" << std::endl ;
				}
			}




			if (tecmPartitionName_ != "")
			{
				try
				{
					tkTecmDiagErrorAnalyser = new TkDiagErrorAnalyser ( tecmPartitionName_ ) ; //, login, passwd, path ) ;
					if (tkTecmDiagErrorAnalyser->getConnectionNotIdentified() != 0) std::cerr << "Found " << tkTecmDiagErrorAnalyser->getConnectionNotIdentified() << " connections not identified with DCU ID" << std::endl ;
					if (tkTecmDiagErrorAnalyser->getPsuNotIdentified() != 0) std::cerr << "Found " << tkTecmDiagErrorAnalyser->getPsuNotIdentified() << " PSU not identified with DCU hardware ID" << std::endl ;
					//Let's suppose that once this point reached the DB is usable
					if (tkTecmDiagErrorAnalyser != NULL) isDbUsable = true;
				}
				catch (oracle::occi::SQLException &e)
				{
					std::cerr << "Cannot create the access to the database: " << login << "/" << "******" << "@" << path << std::endl ;
					std::cerr << e.what() << std::endl ;
				}
				catch (FecExceptionHandler &e)
				{
					std::cerr << "Cannot create the access to DB: " << login << "/" << "******" << "@" << path << std::endl ;
					std::cerr << e.what() << std::endl ;
				}
				catch (...)
				{
					std::cerr << "Unattended exception during DB access creation" << std::endl ;
				}
			}




		}
                

                
//	}
//std::cout << "End of Checking availability of DB accesses" << std::endl;



//sleep((unsigned int)5);





	}
	#endif


*/


//GLAP
//std::cout << "Run number is " << diagService_->feRunFromRcms_ << std::endl;

        }


		GlobalErrorDispatcher::~GlobalErrorDispatcher()
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

//P5MOD
#ifdef WILL_COMPILE_FOR_TRACKER
    		if (dcuFileOutIsOpen_ == true)
		    {
    			//std::cout << "DEBUG : Closing log file" << std::endl;
    			if (dcuFileOut_ != NULL) fclose(dcuFileOut_);
		    }
    		if (fedDebugFileOutIsOpen_ == true)
		    {
    			//std::cout << "DEBUG : Closing log file" << std::endl;
    			if (fedDebugFileOut_ != NULL) fclose(fedDebugFileOut_);
		    }
    		if (fedTmonFileOutIsOpen_ == true)
		    {
    			//std::cout << "DEBUG : Closing log file" << std::endl;
    			if (fedTmonFileOut_ != NULL) fclose(fedTmonFileOut_);
		    }
    		if (fedSpyLogFileNameHasChanged_ == true)
		    {
    			//std::cout << "DEBUG : Closing log file" << std::endl;
    			if (fedTmonFileOut_ != NULL) fclose(fedTmonFileOut_);
		    }
#endif
//P5MODEND



			if (RCMSMutex_ != NULL) delete RCMSMutex_;
			
			
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
		#ifdef DATABASE
			// Delete the database access
			try
			{     
				if (tkDiagErrorAnalyser != NULL) delete tkDiagErrorAnalyser ;
				if (tkTibDiagErrorAnalyser != NULL) delete tkTibDiagErrorAnalyser ;
				if (tkTobDiagErrorAnalyser != NULL) delete tkTobDiagErrorAnalyser ;
				if (tkTecpDiagErrorAnalyser != NULL) delete tkTecpDiagErrorAnalyser ;
				if (tkTecmDiagErrorAnalyser != NULL) delete tkTecmDiagErrorAnalyser ;
			}  
			catch (oracle::occi::SQLException &e)
			{
				std::cerr << "Cannot close the access to the database" << std::endl ;
				std::cerr << e.what() << std::endl ;
			}
			catch (...)
			{
				std::cerr << "Unattended exception while deleting DB access object" << std::endl ;

			}

		#endif

		}



        /* Returns the HTML page displayed from the Default() method when the <default_page> link is clicked */
        void GlobalErrorDispatcher::getDefault1Content(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            *out << cgicc::p() << std::endl;
			#ifdef WILL_COMPILE_FOR_TRACKER
			    *out << "<br>Global Error Dispatcher is logging standard logs to file : " << diagService_->logFileName_.c_str() << std::endl;
	    		*out << "<br>Global Error Dispatcher has received " << logcounter << " standard logs from Supervisors" << std::endl;
			    *out << "<br>Global Error Dispatcher has dumped " << dumpedlogcounter << " standard logs out of " << logcounter << " (received) into logfile<br>" << std::endl;
//				*out << "<br>" << std::endl;

			    *out << "<br>Global Error Dispatcher is logging dcu errors related logs to file : " << dcuLogFileName_.c_str() << std::endl;
	    		*out << "<br>Global Error Dispatcher has received " << dcuErrorLogCounter << " dcu errors related logs from Supervisors" << std::endl;
			    *out << "<br>Global Error Dispatcher has dumped " << dcuDumpedlogcounter << " dcu errors related logs out of " << dcuErrorLogCounter << " (received) into logfile<br>" << std::endl;
//				*out << "<br>" << std::endl;

			    *out << "<br>Global Error Dispatcher is logging fed debug logs to file : " << fedDebugLogFileName_.c_str() << std::endl;
	    		*out << "<br>Global Error Dispatcher has received " << fedDebugLogCounter << " fed debug logs from Supervisors" << std::endl;
			    *out << "<br>Global Error Dispatcher has dumped " << fedDebugDumpedlogcounter << " fed debug logs out of " << fedDebugLogCounter << " (received) into logfile<br>" << std::endl;
//				*out << "<br>" << std::endl;


			    *out << "<br>Global Error Dispatcher is logging fed temperature monitoring logs to file : " << fedTmonLogFileName_.c_str() << std::endl;
	    		*out << "<br>Global Error Dispatcher has received " << fedTmonFileLogCounter << " fed temperature monitoring logs from Supervisors" << std::endl;
			    *out << "<br>Global Error Dispatcher has dumped " << fedTmonDumpedlogcounter << " fed temperature monitoring logs out of " << fedTmonFileLogCounter << " (received) into logfile<br>" << std::endl;
//				*out << "<br>" << std::endl;


		    #else
			    *out << "<br>Global Error Dispatcher is logging logs to file : " << diagService_->logFileName_.c_str() << "<br>" << std::endl;
	    		*out << "<br>Global Error Dispatcher has received " << logcounter << " logs from Supervisors<br>" << std::endl;
			    *out << "<br>Global Error Dispatcher has dumped " << dumpedlogcounter << " logs out of " << logcounter << " (received) into logfile<br>" << std::endl;
				*out << "<br>" << std::endl;

			#endif
            *out << cgicc::p() << std::endl;
        }



        /* Returns the HTML page displayed from the Default() method when the <access_fsm> link is clicked */
        void GlobalErrorDispatcher::getFsmStatePage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
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
        void GlobalErrorDispatcher::getVisualizeLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
			getIFrame(in, out);

        }

/*
        void GlobalErrorDispatcher::getJsonLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
			*out << "you are on the JSON applet homepage" << std::endl;

*out << "<script type=\"text/javascript\" language=\"javascript\" src=\"com.google.gwt.sample.logreader.LogReader.nocache.js\"></script>" << std::endl;

        }
*/




        /* xgi method called when the link <default_page> is clicked */
        void GlobalErrorDispatcher::Default1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            internalState_ = 0;
            Default(in, out);
        }

        /* xgi method called when the link <display_diagsystem> is clicked */
        void GlobalErrorDispatcher::callDiagSystemPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            internalState_ = 1;
            Default(in, out);
        }

        /* xgi method called when the link <display_fsm> is clicked */
        void GlobalErrorDispatcher::callFsmPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            internalState_ = 2;
            Default(in, out);
        }

        /* xgi method called when the link <display_logs> is clicked */
        void GlobalErrorDispatcher::callVisualizeLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
	    internalState_ = 3;
            Default(in, out);
        }

/*
        void GlobalErrorDispatcher::callJsonLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
	    internalState_ = 4;
            Default(in, out);
        }
*/
/*
        void callLogsSummaryPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
	    internalState_ = 4;
            Default(in, out);
        }
*/



        void GlobalErrorDispatcher::genericSort(int rowOffset, bool sortGrowing)
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
                            Timestamp = copyOfLogsMemory[j][DIAG_GED_TSOFFSET];
                            Level = copyOfLogsMemory[j][LVLOFFSET];
                            Errcode = copyOfLogsMemory[j][ECOFFSET];
                            Faultstate = copyOfLogsMemory[j][FSOFFSET];
                            Sysid = copyOfLogsMemory[j][SIDOFFSET];
                            Subsysid = copyOfLogsMemory[j][SSIDOFFSET];
                            Edtimestamp = copyOfLogsMemory[j][EDDIAG_GED_TSOFFSET];

                            /* permute values */
                            copyOfLogsMemory[j][LNOFFSET] = copyOfLogsMemory[j-1][LNOFFSET];
                            copyOfLogsMemory[j][MSGOFFSET] = copyOfLogsMemory[j-1][MSGOFFSET];
                            copyOfLogsMemory[j][ORGOFFSET] = copyOfLogsMemory[j-1][ORGOFFSET];
                            copyOfLogsMemory[j][DIAG_GED_TSOFFSET] = copyOfLogsMemory[j-1][DIAG_GED_TSOFFSET];
                            copyOfLogsMemory[j][LVLOFFSET] = copyOfLogsMemory[j-1][LVLOFFSET];
                            copyOfLogsMemory[j][ECOFFSET] = copyOfLogsMemory[j-1][ECOFFSET];
                            copyOfLogsMemory[j][FSOFFSET] = copyOfLogsMemory[j-1][FSOFFSET];
                            copyOfLogsMemory[j][SIDOFFSET] = copyOfLogsMemory[j-1][SIDOFFSET];
                            copyOfLogsMemory[j][SSIDOFFSET] = copyOfLogsMemory[j-1][SSIDOFFSET];
                            copyOfLogsMemory[j][EDDIAG_GED_TSOFFSET] = copyOfLogsMemory[j-1][EDDIAG_GED_TSOFFSET];

                            /* Restore lowest value at right place */
                            copyOfLogsMemory[j-1][LNOFFSET] = LogNumbers;
                            copyOfLogsMemory[j-1][MSGOFFSET] = Message;
                            copyOfLogsMemory[j-1][ORGOFFSET] = Origine;
                            copyOfLogsMemory[j-1][DIAG_GED_TSOFFSET] = Timestamp;
                            copyOfLogsMemory[j-1][LVLOFFSET] = Level;
                            copyOfLogsMemory[j-1][ECOFFSET] = Errcode;
                            copyOfLogsMemory[j-1][FSOFFSET] = Faultstate;
                            copyOfLogsMemory[j-1][SIDOFFSET] = Sysid;
                            copyOfLogsMemory[j-1][SSIDOFFSET] = Subsysid;
                            copyOfLogsMemory[j-1][EDDIAG_GED_TSOFFSET] = Edtimestamp;
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
                            Timestamp = copyOfLogsMemory[j][DIAG_GED_TSOFFSET];
                            Level = copyOfLogsMemory[j][LVLOFFSET];
                            Errcode = copyOfLogsMemory[j][ECOFFSET];
                            Faultstate = copyOfLogsMemory[j][FSOFFSET];
                            Sysid = copyOfLogsMemory[j][SIDOFFSET];
                            Subsysid = copyOfLogsMemory[j][SSIDOFFSET];
                            Edtimestamp = copyOfLogsMemory[j][EDDIAG_GED_TSOFFSET];

                            /* permute values */
                            copyOfLogsMemory[j][LNOFFSET] = copyOfLogsMemory[j-1][LNOFFSET];
                            copyOfLogsMemory[j][MSGOFFSET] = copyOfLogsMemory[j-1][MSGOFFSET];
                            copyOfLogsMemory[j][ORGOFFSET] = copyOfLogsMemory[j-1][ORGOFFSET];
                            copyOfLogsMemory[j][DIAG_GED_TSOFFSET] = copyOfLogsMemory[j-1][DIAG_GED_TSOFFSET];
                            copyOfLogsMemory[j][LVLOFFSET] = copyOfLogsMemory[j-1][LVLOFFSET];
                            copyOfLogsMemory[j][ECOFFSET] = copyOfLogsMemory[j-1][ECOFFSET];
                            copyOfLogsMemory[j][FSOFFSET] = copyOfLogsMemory[j-1][FSOFFSET];
                            copyOfLogsMemory[j][SIDOFFSET] = copyOfLogsMemory[j-1][SIDOFFSET];
                            copyOfLogsMemory[j][SSIDOFFSET] = copyOfLogsMemory[j-1][SSIDOFFSET];
                            copyOfLogsMemory[j][EDDIAG_GED_TSOFFSET] = copyOfLogsMemory[j-1][EDDIAG_GED_TSOFFSET];

                            /* Restore lowest value at right place */
                            copyOfLogsMemory[j-1][LNOFFSET] = LogNumbers;
                            copyOfLogsMemory[j-1][MSGOFFSET] = Message;
                            copyOfLogsMemory[j-1][ORGOFFSET] = Origine;
                            copyOfLogsMemory[j-1][DIAG_GED_TSOFFSET] = Timestamp;
                            copyOfLogsMemory[j-1][LVLOFFSET] = Level;
                            copyOfLogsMemory[j-1][ECOFFSET] = Errcode;
                            copyOfLogsMemory[j-1][FSOFFSET] = Faultstate;
                            copyOfLogsMemory[j-1][SIDOFFSET] = Sysid;
                            copyOfLogsMemory[j-1][SSIDOFFSET] = Subsysid;
                            copyOfLogsMemory[j-1][EDDIAG_GED_TSOFFSET] = Edtimestamp;
                        }
                    }
                }

            }
        
        }



        void GlobalErrorDispatcher::sortLognumbers(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(LNOFFSET, sortLognumbersGrowing);
            if (sortLognumbersGrowing == true) {sortLognumbersGrowing = false;} else {sortLognumbersGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        /* xgi method called when the link <default_page> is clicked */
        void GlobalErrorDispatcher::sortMessages(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(MSGOFFSET, sortMessagesGrowing);
            if (sortMessagesGrowing == true) {sortMessagesGrowing = false;} else {sortMessagesGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }


        void GlobalErrorDispatcher::sortOrigin(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(ORGOFFSET, sortOriginGrowing);
            if (sortOriginGrowing == true) {sortOriginGrowing = false;} else {sortOriginGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void GlobalErrorDispatcher::sortTimestamp(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(DIAG_GED_TSOFFSET, sortTimestampGrowing);
            if (sortTimestampGrowing == true) {sortTimestampGrowing = false;} else {sortTimestampGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void GlobalErrorDispatcher::sortLevel(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(LVLOFFSET, sortLevelGrowing);
            if (sortLevelGrowing == true) {sortLevelGrowing = false;} else {sortLevelGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void GlobalErrorDispatcher::sortErrcode(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(ECOFFSET, sortErrcodeGrowing);
            if (sortErrcodeGrowing == true) {sortErrcodeGrowing = false;} else {sortErrcodeGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void GlobalErrorDispatcher::sortFaultstate(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(FSOFFSET, sortFaultstateGrowing);
            if (sortFaultstateGrowing == true) {sortFaultstateGrowing = false;} else {sortFaultstateGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void GlobalErrorDispatcher::sortSysid(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(SIDOFFSET, sortSysidGrowing);
            if (sortSysidGrowing == true) {sortSysidGrowing = false;} else {sortSysidGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void GlobalErrorDispatcher::sortSubsysid(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(SSIDOFFSET, sortSubsysidGrowing);
            if (sortSubsysidGrowing == true) {sortSubsysidGrowing = false;} else {sortSubsysidGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }

        void GlobalErrorDispatcher::sortEdtimestamp(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
            processingSortRequest = true;
            genericSort(EDDIAG_GED_TSOFFSET, sortEdtimestampGrowing);
            if (sortEdtimestampGrowing == true) {sortEdtimestampGrowing = false;} else {sortEdtimestampGrowing = true;}
            //getIFrame(in, out);
            Default(in, out);
        }



        void GlobalErrorDispatcher::getIFrame(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
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
                        *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << copyOfLogsMemory[i][DIAG_GED_TSOFFSET] << "</td>";
                        // *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << formattedTimeStamp << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << copyOfLogsMemory[i][LVLOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << copyOfLogsMemory[i][ECOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << copyOfLogsMemory[i][FSOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << copyOfLogsMemory[i][SIDOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << copyOfLogsMemory[i][SSIDOFFSET] << "</td>";
                        // *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << copyOfLogsMemory[i][EDDIAG_GED_TSOFFSET] << "</td>";
                        // *out << "<td style=\"vertical-align: top; background-color: rgb(255, 255, 255);\">" << edformattedTimeStamp << "</td>";
                        *out << "</tr>";
                    }
                    else
                    {
                        *out << "<tr>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][LNOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][MSGOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][ORGOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][DIAG_GED_TSOFFSET] << "</td>";
                        // *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << formattedTimeStamp << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][LVLOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][ECOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][FSOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][SIDOFFSET] << "</td>";
                        *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][SSIDOFFSET] << "</td>";
                        // *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << copyOfLogsMemory[i][EDDIAG_GED_TSOFFSET] << "</td>";
                        // *out << "<td style=\"vertical-align: top; background-color: rgb(204, 204, 204);\">" << edformattedTimeStamp << "</td>";
                        *out << "</tr>";
                    }
                }
            }

            *out << "</tbody>";
            *out << "</table>";
        }



        /* Displays the available HyperDaq links for this process */
        void GlobalErrorDispatcher::displayLinks(xgi::Input * in, xgi::Output * out)
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
            *out << "<a href=" << urlDiag_ << ">Configure Global Error Dispatcher</a>";
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
            *out << "&nbsp;&nbsp;&nbsp;&nbsp;";

/*
            std::string toto_;
            toto_ += getApplicationDescriptor()->getContextDescriptor()->getURL();
			std::cout << "toto = " << toto_ << std::endl;
*/

            /* Display JSON visu page */
/*
            std::string urlJson_;
            urlJson_ += getApplicationDescriptor()->getContextDescriptor()->getURL();
            urlJson_ += "/testgwt/LogReader/LogReader.html";
            *out << "<a target=\"_blank\" href=" << urlJson_ << ">Ajax logs display panel</a>";
*/


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
        void GlobalErrorDispatcher::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {

            /* Create HTML header */
            *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
            *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
            *out << cgicc::title("Global Error Dispatcher") << std::endl;
            xgi::Utils::getPageHeader(  out, 
                                        "Global Error Dispatcher", 
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
//            if (internalState_ == 4) getJsonLogsPage(in, out);

            /* Create HTML footer */
            xgi::Utils::getPageFooter(*out);
        }






        /* converts DiagSystem loglevel format into OWN loglevel format */
        long GlobalErrorDispatcher::getLogLevelToLong(std::string logLevel)
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






/*
        void GlobalErrorDispatcher::getJSONDataList(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {

		JSONUtils jsonUtility;
		*out << jsonUtility.craftJsonArray(&jsonItemsList_);
	}

*/










#ifdef DATABASE


        void GlobalErrorDispatcher::findCrateSlotVsHardwareId(CLog *l)
        {
if (isDbUsable == true)
{
/*
std::cout << "Entering findCrateSlotVsHardwareId" << std::endl;
std::cout << "Log record partition name is " << l->getPartitionName() << std::endl;
*/

TkDiagErrorAnalyser * tkDiagErrorAnalyserInstancePointer = NULL;

if (isRunningOnP5Machines_ == false)
{
//std::cout << "isRunningOnP5Machines_ is false" << std::endl;
	tkDiagErrorAnalyserInstancePointer = tkDiagErrorAnalyser;
}
else
{
//	std::cout << "isRunningOnP5Machines_ is true" << std::endl;
	if (l->getPartitionName() == tibPartitionName_)
	{
		tkDiagErrorAnalyserInstancePointer = tkTibDiagErrorAnalyser;
//		std::cout << "Detected partition name : " << tibPartitionName_ << " so assigning pointer to tkTibDiagErrorAnalyser" << std::endl;
	}
	else if (l->getPartitionName() == tobPartitionName_)
	{
		tkDiagErrorAnalyserInstancePointer = tkTobDiagErrorAnalyser;
//		std::cout << "Detected partition name : " << tobPartitionName_ << " so assigning pointer to tkTobDiagErrorAnalyser" << std::endl;
	}
	else if (l->getPartitionName() == tecpPartitionName_)
	{
		tkDiagErrorAnalyserInstancePointer = tkTecpDiagErrorAnalyser;
//		std::cout << "Detected partition name : " << tecpPartitionName_ << " so assigning pointer to tkTecpDiagErrorAnalyser" << std::endl;
	}
	else if (l->getPartitionName() == tecmPartitionName_)
	{
		tkDiagErrorAnalyserInstancePointer = tkTecmDiagErrorAnalyser;
//		std::cout << "Detected partition name : " << tecmPartitionName_ << " so assigning pointer to tkTecmDiagErrorAnalyser" << std::endl;
	}
//	else std::cout << "No match found for partition name : " << l->getPartitionName() << " ; no pointer assignation" << std::endl;
}

if (tkDiagErrorAnalyserInstancePointer == NULL)
{
//	std::cout << "tkDiagErrorAnalyserInstancePointer is NULL for a partition name setted in log record to : " <<  l->getPartitionName() << std::endl;
	return;
}

											unsigned int crate = 9999;
											unsigned int slot = 9999;
											std::string eBuff = l->getExtraBuffer();

											std::string fecHardId = "NA";
											fecHardId = l->findInExtraBuffer((std::string)"<FECHARDID>", (std::string)"</FECHARDID>",eBuff);
											if (fecHardId != (std::string)"NA")
											{
												//std::cout << "GOT A FECHARDID from logmessage = " << fecHardId << std::endl;
												try
												{
													ConnectionVector fecConnections = tkDiagErrorAnalyserInstancePointer->getListOfModulesAsConnection();
												    for (ConnectionVector::iterator it = fecConnections.begin() ; it != fecConnections.end() ; it ++)
													{
														ConnectionDescription *connection = *it;
														//std::cout << "GOT A DB ISSUED FECHARID EQUAL TO : " << connection->getFecHardwareId() << std::endl;
														if (connection->getFecHardwareId() == fecHardId)
														{
															crate = connection->getFecCrateId();
                                                                                                                        //std::cout << "Crate number found in DB : " << crate << std::endl;
															slot = connection->getFecSlot();
                                                                                                                        //std::cout << "Slot number found in DB : " << slot << std::endl;
															break;
														}
													}
												}
												catch (std::string &e) {}
												catch (oracle::occi::SQLException &e) {}
												catch (FecExceptionHandler &e) {}
												catch (...) {}

												//std::cout << "Fec Crate Id = " << crate << std::endl;
												//std::cout << "Fec Slot Id = " << slot << std::endl;

											}

											std::string fedId = "NA";
											fedId = l->findInExtraBuffer((std::string)"<FEDID>", (std::string)"</FEDID>",eBuff);
											if (fedId != (std::string)"NA")
											{
												unsigned int uiFedId = (unsigned int)atoi(fedId.c_str());
												//std::cout << "GOT A FEDID = " << uiFedId << std::endl;
												try
												{
													ConnectionVector fedConnections = tkDiagErrorAnalyserInstancePointer->getListOfModulesAsConnection();
												    for (ConnectionVector::iterator it = fedConnections.begin() ; it != fedConnections.end() ; it ++)
													{
														ConnectionDescription *connection = *it;
														//std::cout << "GOT A DB ISSUED FEDID EQUAL TO : " << connection->getFedId() << std::endl;
														if (connection->getFedId() == uiFedId)
														{
															crate = connection->getFedCrateId();
															slot = connection->getFedSlot();
															break;
														}
													}
												}
												catch (std::string &e) {}
												catch (oracle::occi::SQLException &e) {}
												catch (FecExceptionHandler &e) {}
												catch (...) {}

												//std::cout << "Fed Crate Id = " << crate << std::endl;
												//std::cout << "Fed Slot Id = " << slot << std::endl;
											}
											
											// If the crate/slot pair detection has been successful
											if ( (crate != 9999) && (slot != 9999) )
											{
												std::ostringstream txtCrateId;
												std::ostringstream txtSlotId;
												txtCrateId << crate;
												txtSlotId << slot;
												l->setCrateId(txtCrateId.str());
												l->setSlotId(txtSlotId.str());
											}
}
}
#endif //End DB dependant section



/*

	void GlobalErrorDispatcher::writeMessageToFile (FILE *f, CLog* l, unsigned long fileLogsCounter)
	{
		#ifdef WILL_COMPILE_FOR_TRACKER
			#ifdef DATABASE
				findCrateSlotVsHardwareId(l);
			#endif
			//USE DIRECTLY FIREITEM VALUE FOR FILE LOGGING, COPY TO PROTECTED VAR IS NOT MADE DUE TO NO INIT/CONF COMMAND DENT BY RCMS
			fprintf(f,"%s",l->buildStringForTrackerLogFileOutput(fileLogsCounter, diagService_->feRunFromRcms_).c_str());
		#else
			#ifdef WILL_COMPILE_FOR_PIXELS
				fprintf(f,"%s",l->buildStringForPixelsLogFileOutput(fileLogsCounter).c_str());
			#else
				//Generic case
				fprintf(f,"%s",l->buildStringForLogFileOutput(fileLogsCounter).c_str());
			#endif
		#endif
		fflush(f);
	}

*/














        void GlobalErrorDispatcher::processReceivedLog (CLog l)
		{

/*
std::cout << "Entering processReceivedLog" << std::endl;
std::cout << "logcounter = " << logcounter << std::endl;
std::cout << "diagService_->ccRoute_ = " << diagService_->ccRoute_ << std::endl;
std::cout << "diagService_->ccRouteValidated_ = " << diagService_->ccRouteValidated_ << std::endl;
std::cout << "diagService_->fileLoggingUsable_ = " << diagService_->fileLoggingUsable_ << std::endl;
*/

            //logcounter++;


    	    /* Set History "LogNumber" Field */
            char ln_str[20];
            snprintf(ln_str, 19, "%lu", logcounter);
            logsMemory[arrayliner][LNOFFSET] = (std::string)ln_str;
    	    /* Set History "Message" Field */
            logsMemory[arrayliner][MSGOFFSET] = l.getMessage();
    	    /* Set History "Logger" Field */
            logsMemory[arrayliner][ORGOFFSET] = l.getLogger();
    	    /* Set History "Timestamp" Field */
    	    logsMemory[arrayliner][DIAG_GED_TSOFFSET] = l.getTimestampAsString();
    	    /* Set History "Level" Field */
            logsMemory[arrayliner][LVLOFFSET] = l.getLevel();






            arrayliner++;
            if (arrayliner >= LOGARRAYLINES) arrayliner=0;


			/* Verify first that no reconfiguration action is needed, accorded to log content.*/
			checkIfReconfigurationIsNeeded(&l);



//std::cout << "Processing REPOST condition" << std::endl;

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

















//std::cout << "Processing FILE condition" << std::endl;
                /* IF we have to route errors to FILE */

/*
                if (diagService_->fileLoggingUsable_ = true)
                //DO NOT CORRECT THIS BUG, IT IS A VOLONTARY BAD CONDITION TESTING
                {
*/
                    /* Log to file */
                    if ( getLogLevelToLong(l.getLevel()) >= getLogLevelToLong(diagService_->fileLogLevel_) )
                    {
					
/*

std::cout << "AAAAA" << std::endl;
						if (diagService_->logFileNameHasChanged_ == true)
						{
std::cout << "BBBBBBB" << std::endl;
							//if (fileOut_ != NULL) fclose(fileOut_);
							if (outfile.is_open()) outfile.close();
							fileOutIsOpen_ = false;
							fileOut_ = NULL;
						}
						bool logFileNameHasFlipped = false;
std::cout << "CCCCC" << std::endl;

//P5MOD
#ifdef WILL_COMPILE_FOR_TRACKER
						if (dcuLogFileNameHasChanged_ == true)
						{
							if (dcuOutfile.is_open()) dcuOutfile.close();
							dcuFileOutIsOpen_ = false;
							dcuFileOut_ = NULL;
						}
						bool dcuLogFileNameHasFlipped = false;


						if (fedDebugLogFileNameHasChanged_ == true)
						{
							if (fedDebugOutfile.is_open()) fedDebugOutfile.close();
							fedDebugFileOutIsOpen_ = false;
							fedDebugFileOut_ = NULL;
						}
						bool fedDebugLogFileNameHasFlipped = false;


						if (fedTmonLogFileNameHasChanged_ == true)
						{
							if (fedTmonfile.is_open()) fedTmonfile.close();
							fedTmonFileOutIsOpen_ = false;
							fedTmonFileOut_ = NULL;
						}
						bool fedTempMonLogFileNameHasFlipped = false;

						if (fedSpyLogFileNameHasChanged_ == true)
						{
							if (fedSpyFile.is_open()) fedSpyFile.close();
							fedSpyFileOutIsOpen_ = false;
							fedSpyFileOut_ = NULL;
						}
						bool fedSpyLogFileNameHasFlipped = false;
#endif
*/



//P5MODEND


/*
						#define GLBED_PROCUID_LENGTH 1000
						char testStr[GLBED_PROCUID_LENGTH];
						strncpy(testStr, this->getApplicationLogger().getName().c_str(), (GLBED_PROCUID_LENGTH-1) );
						for (int ii=0; ii<GLBED_PROCUID_LENGTH; ii++)
						{
							if (testStr[ii] == '.') testStr[ii] = '-';
							if (testStr[ii] == ')') testStr[ii] = '-';
							if (testStr[ii] == '(') testStr[ii] = '-';
							if (testStr[ii] == ':') testStr[ii] = '-';
						}


*/






if ((fileOutIsOpen_ == false) && (standardFileLoggingUsable_ == true)) openStandardLoggingFile();







/*


std::cout << "DDDDDDd" << std::endl;
    	    	    	//Reset File if first log
				    	if (fileOutIsOpen_ == false)
						{
std::cout << "EEEEEe" << std::endl;
							std::ostringstream thisMachineAsText;
							thisMachineAsText << this->getApplicationLogger().getName();
							std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + STANDARDLOGFILENAMETAG + getDateTagForFile() + (std::string)"--" + (std::string)testStr + (std::string)"-" + (std::string)xmlFileNameSpecificExtension_ + (std::string)LOGFILENAMESUFFIX;
							diagService_->setFilePathAndName(lclLogFileName);
							if (diagService_->logFileNameHasChanged_ == true)
							{
std::cout << "FFFFFFf" << std::endl;
								outfile.open(diagService_->logFileName_.c_str(),std::ios_base::out);
								diagService_->logFileNameHasChanged_ = false;
								logFileNameHasFlipped = true;
								fileOutIsOpen_ = true;
							}
							else
							{
std::cout << "GGGGGg" << std::endl;
								outfile.open(diagService_->logFileName_.c_str(),std::ios_base::app);
								fileOutIsOpen_ = true;
							}
							
						    if (!(outfile.is_open()))
    	    	        	{
std::cout << "HHHHHHH" << std::endl;
	            	        	std::stringstream errMsg;
                                        //FILEACCESSERROR			
	                	    	//std::cout << "Error : Unable to write to destination log file \"" << diagService_->logFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
		                	    diagService_->fileLoggingUsable_ = false;
								fileOutIsOpen_ = false;
								fileOut_ = NULL;
    	    		        }
        			        else
    	        		    {
std::cout << "IIIIIII" << std::endl;
							    fileOutIsOpen_ = true;
								if (logFileNameHasFlipped == true) fileLogsCounter=0;
								std::cout << "LogFile \"" << diagService_->logFileName_.c_str() << "\" opened in write mode" << std::endl;
		    	            }

						}
						
*/
//P5MOD
#ifdef WILL_COMPILE_FOR_TRACKER


if (dcuFileOutIsOpen_ == false) openDcuErrorsLoggingFile();
/*
    	    	    	//Reset DCU errors File if first log
				    	if (dcuFileOutIsOpen_ == false)
						{
							std::ostringstream thisMachineAsText;
							thisMachineAsText << this->getApplicationLogger().getName();
							std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + DCULOGFILENAMETAG + getDateTagForFile() + (std::string)"--" + (std::string)testStr + (std::string)"-" + (std::string)xmlFileNameSpecificExtension_ + (std::string)LOGFILENAMESUFFIX;
							//diagService_->setFilePathAndName(lclLogFileName);//logFileName_ = fileName;
							dcuLogFileName_ = lclLogFileName;
							
							if (dcuLogFileNameHasChanged_ == true)
							{
								dcuOutfile.open(dcuLogFileName_.c_str(),std::ios_base::out);
								dcuLogFileNameHasChanged_ = false;
								dcuLogFileNameHasFlipped = true;
								dcuFileOutIsOpen_ = true;
							}
							else
							{
								dcuOutfile.open(dcuLogFileName_.c_str(),std::ios_base::app);
								dcuFileOutIsOpen_ = true;
							}
							
						    if (!(dcuOutfile.is_open()))
    	    	        	{
	            	        	std::stringstream errMsg;
                                        //FILEACCESSERROR			
	                	    	//std::cout << "Error : Unable to write to dcu errors logging dedicated destination log file \"" << dcuLogFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
		                	    //diagService_->fileLoggingUsable_ = false;
								dcuFileOutIsOpen_ = false;
								dcuFileOut_ = NULL;
    	    		        }
        			        else
    	        		    {
							    dcuFileOutIsOpen_ = true;
								if (dcuLogFileNameHasFlipped == true) dcuErrorFileLogCounter=0;
								std::cout << "DCU errors dedicated logFile \"" << dcuLogFileName_.c_str() << "\" opened in write mode" << std::endl;
		    	            }

						}

*/
if (fedDebugFileOutIsOpen_ == false) openFedDebugLoggingFile();


/*
    	    	    	//Reset FED DEBUG errors File if first log
				    	if (fedDebugFileOutIsOpen_ == false)
						{
							std::ostringstream thisMachineAsText;
							thisMachineAsText << this->getApplicationLogger().getName();
							std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + FEDDEBUGLOGFILENAMETAG + getDateTagForFile() + (std::string)"--" + (std::string)testStr + (std::string)"-" + (std::string)xmlFileNameSpecificExtension_ + (std::string)LOGFILENAMESUFFIX;
							//diagService_->setFilePathAndName(lclLogFileName);//logFileName_ = fileName;
							fedDebugLogFileName_ = lclLogFileName;
							
							if (fedDebugLogFileNameHasChanged_ == true)
							{
								fedDebugOutfile.open(fedDebugLogFileName_.c_str(),std::ios_base::out);
								fedDebugLogFileNameHasChanged_ = false;
								fedDebugLogFileNameHasFlipped = true;
								fedDebugFileOutIsOpen_ = true;
							}
							else
							{
								fedDebugOutfile.open(fedDebugLogFileName_.c_str(),std::ios_base::app);
								fedDebugFileOutIsOpen_ = true;
							}
							
						    if (!(fedDebugOutfile.is_open()))
    	    	        	{
	            	        	std::stringstream errMsg;
                                        //FILEACCESSERROR
	                	    	//std::cout << "Error : Unable to write to FED debug messages dedicated destination log file \"" << fedDebugLogFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
		                	    //diagService_->fileLoggingUsable_ = false;
								fedDebugFileOutIsOpen_ = false;
								fedDebugFileOut_ = NULL;
    	    		        }
        			        else
    	        		    {
							    fedDebugFileOutIsOpen_ = true;
								if (fedDebugLogFileNameHasFlipped == true) fedDebugFileLogCounter=0;
								std::cout << "FED debug messages dedicated logFile \"" << fedDebugLogFileName_.c_str() << "\" opened in write mode" << std::endl;
		    	            }

						}
*/

if (fedTmonFileOutIsOpen_ == false) openFedTmonLoggingFile();

/*

    	    	    	//Reset FED TEMPERATURE MONITORING File if first log
				    	if (fedTmonFileOutIsOpen_ == false)
						{
							std::ostringstream thisMachineAsText;
							thisMachineAsText << this->getApplicationLogger().getName();
							std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + FEDTMONLOGFILENAMETAG + getDateTagForFile() + (std::string)"--" + (std::string)testStr + (std::string)"-" + (std::string)xmlFileNameSpecificExtension_ + (std::string)LOGFILENAMESUFFIX;
							//diagService_->setFilePathAndName(lclLogFileName);//logFileName_ = fileName;
							fedTmonLogFileName_ = lclLogFileName;
							
							if (fedTmonLogFileNameHasChanged_ == true)
							{
								fedTmonfile.open(fedTmonLogFileName_.c_str(),std::ios_base::out);
								fedTmonLogFileNameHasChanged_ = false;
								fedTempMonLogFileNameHasFlipped = true;
								fedTmonFileOutIsOpen_ = true;
							}
							else
							{
								fedTmonfile.open(fedTmonLogFileName_.c_str(),std::ios_base::app);
								fedTmonFileOutIsOpen_ = true;
							}
							
						    if (!(fedTmonfile.is_open()))
    	    	        	{
	            	        	std::stringstream errMsg;
                                        //FILEACCESSERROR
	                	    	//std::cout << "Error : Unable to write to FED temperature monitoring messages dedicated destination log file \"" << fedTmonLogFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
		                	    //diagService_->fileLoggingUsable_ = false;
								fedTmonFileOutIsOpen_ = false;
								fedTmonFileOut_ = NULL;
    	    		        }
        			        else
    	        		    {
							    fedTmonFileOutIsOpen_ = true;
								if (fedTempMonLogFileNameHasFlipped == true) fedTmonFileLogCounter=0;
								std::cout << "FED temperature monitoring messages dedicated logFile \"" << fedTmonLogFileName_.c_str() << "\" opened in write mode" << std::endl;
		    	            }

						}

*/
if (fedSpyFileOutIsOpen_ == false) openFedSpyLoggingFile();

/*
    	    	    	//Reset FED SPY CHANNEL File if first log
				    	if (fedSpyFileOutIsOpen_ == false)
						{
							std::ostringstream thisMachineAsText;
							thisMachineAsText << this->getApplicationLogger().getName();
							std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + FEDSPYLOGFILENAMETAG + getDateTagForFile() + (std::string)"--" + (std::string)testStr + (std::string)"-" + (std::string)xmlFileNameSpecificExtension_ + (std::string)LOGFILENAMESUFFIX;
							//diagService_->setFilePathAndName(lclLogFileName);//logFileName_ = fileName;
							fedSpyLogFileName_ = lclLogFileName;
							
							if (fedSpyLogFileNameHasChanged_ == true)
							{
								fedSpyFile.open(fedSpyLogFileName_.c_str(),std::ios_base::out);
								fedSpyLogFileNameHasChanged_ = false;
								fedSpyLogFileNameHasFlipped = true;
								fedSpyFileOutIsOpen_ = true;
							}
							else
							{
								fedSpyFile.open(fedSpyLogFileName_.c_str(),std::ios_base::app);
								fedSpyFileOutIsOpen_ = true;
							}
							
						    if (!(fedSpyFile.is_open()))
    	    	        	{
	            	        	std::stringstream errMsg;
                                        //FILEACCESSERROR
	                	    	//std::cout << "Error : Unable to write to FED Spy channel messages dedicated destination log file \"" << fedSpyLogFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
		                	    //diagService_->fileLoggingUsable_ = false;
								fedSpyFileOutIsOpen_ = false;
								fedSpyFileOut_ = NULL;
    	    		        }
        			        else
    	        		    {
							    fedSpyFileOutIsOpen_ = true;
								if (fedSpyLogFileNameHasFlipped == true) fedSpyFileLogCounter=0;
								std::cout << "FED Spy channel messages dedicated logFile \"" << fedSpyLogFileName_.c_str() << "\" opened in write mode" << std::endl;
		    	            }

						}
*/
#endif

//P5MODEND

//P5MOD
#ifdef WILL_COMPILE_FOR_TRACKER

						//Handle messages logging into STANDARD file
						//Write message in logfile only if it's not a FEDDEBUG message
						std::string dcuErrorLogPatternOne = FEC_DCUREADOUTERROR_PATTERN;
						std::string dcuErrorLogPatternTwo = FEC_WORKLOOPERROR_PATTERN;
						std::string fedDebugLogPattern = FED_DEBUG_PATTERN;
						std::string fedTempMonitorLogPattern = FED_TEMP_MON_PATTERN;
						std::string fedSpyLogPattern = FED_SPY_PATTERN;


						size_t dcuStringFoundOne;
						size_t dcuStringFoundTwo;
						size_t fedDebugStringFound;
						size_t fedTmonStringFound;
						size_t fedSpyStringFound;
						
						std::string errMsg = l.getMessage();
						
						dcuStringFoundOne=errMsg.find(dcuErrorLogPatternOne);
						dcuStringFoundTwo=errMsg.find(dcuErrorLogPatternTwo);
						fedDebugStringFound=errMsg.find(fedDebugLogPattern);
						fedTmonStringFound=errMsg.find(fedTempMonitorLogPattern);
						fedSpyStringFound=errMsg.find(fedSpyLogPattern);
#endif
//P5MODEND





#ifdef WILL_COMPILE_FOR_TRACKER
						//If the message does NOT have to be filtered out
						if ( (dcuStringFoundOne == string::npos) && (dcuStringFoundTwo == string::npos) && (fedDebugStringFound == string::npos) && (fedTmonStringFound == string::npos) && (fedSpyStringFound == string::npos) )
						{
#endif


if (standardFileLoggingUsable_ == true)
{



/*
					if (fileOutIsOpen_ == false)
					{
						std::ostringstream thisMachineAsText;
						thisMachineAsText << this->getApplicationLogger().getName();
						std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + STANDARDLOGFILENAMETAG + getDateTagForFile() + (std::string)"--" + (std::string)testStr + (std::string)"-" + (std::string)xmlFileNameSpecificExtension_ + (std::string)LOGFILENAMESUFFIX;
						diagService_->setFilePathAndName(lclLogFileName);
						if (diagService_->logFileNameHasChanged_ == true)
						{
							outfile.open(diagService_->logFileName_.c_str(),std::ios_base::out);
							diagService_->logFileNameHasChanged_ = false;
							// next line useful only in case of "default" config keyword
							diagService_->oldLogFileName_ = diagService_->logFileName_;
							fileOutIsOpen_ = true;
						}
						else
						{
							outfile.open(diagService_->logFileName_.c_str(),std::ios_base::app);
							fileOutIsOpen_ = true;
						}

    		            if (!(outfile.is_open()))
    	    	        {
	            	        std::stringstream errMsg;
                                //FILEACCESSERROR				
                	    	//std::cout << "Error : Unable to open destination file \"" << diagService_->logFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
	                	    diagService_->fileLoggingUsable_ = false;
							fileOutIsOpen_ = false;
							fileOut_ = NULL;
    	    	        }
        		        else
    	        	    {
							std::cout << "LogFile \"" << diagService_->logFileName_.c_str() << "\" opened in write mode" << std::endl;
	    	            }

					}

*/



	//Handle files swap management
	//Rem : fileOutIsOpen_ is supposed to be true ; if not then fileloggingusable is false
	logcounter++;
	fileLogsCounter++;
	if (fileLogsCounter >= MAX_NBR_OF_GENERAL_FILE_LOGS)
//	if (fileLogsCounter >= 2)
	{
		//Close file
		std::cout << "closing general file " << diagService_->logFileName_ << std::endl;
		if (outfile.is_open()) outfile.close();
		outfile.clear();
		fileOutIsOpen_ = false;
		fileOut_ = NULL;
		fileLogsCounter = 0;
								
		//Reopen file
		openStandardLoggingFile();
	}

	//Now take care of writing data into logfile
	//Add info to XTRA buffer in case of RN flip
	if (rnHasFlippedForStandardLogFile == true)
	{
		l.setExtraBuffer("--SYSTEM : Run number changed (possibly on STOP->START transition) ; using new logfile. " + l.getExtraBuffer());
		rnHasFlippedForStandardLogFile = false;
	}

	std::string stringToBuild="";
	#ifdef WILL_COMPILE_FOR_TRACKER
		#ifdef DATABASE
			findCrateSlotVsHardwareId(&l);
		#endif
//		l.buildStringForTrackerLogFileOutput(&stringToBuild, fileLogsCounter, diagService_->feRunFromRcms_);
		l.buildStringForTrackerLogFileOutput(&stringToBuild, logcounter, diagService_->feRunFromRcms_);
	#else
		#ifdef WILL_COMPILE_FOR_PIXELS
			l.buildStringForPixelsLogFileOutput(&stringToBuild, logcounter);
		#else
			l.buildStringForTrackerLogFileOutput(&stringToBuild, logcounter, diagService_->feRunFromRcms_);
		#endif
	#endif
	
	outfile << stringToBuild;
	outfile.flush();
	dumpedlogcounter++;


/*

						//P5MOD
						logcounter++;
						//P5MODEND
//						if (fileOutIsOpen_ == true)
//						{
							fileLogsCounter++;

//Manage file handling ; if we are at MAX then close/reopen logging files

							if (fileLogsCounter >= MAX_NBR_OF_GENERAL_FILE_LOGS)
							{
								std::cout << "closing general file " << diagService_->logFileName_ << std::endl;
								if (outfile.is_open()) outfile.close();
								std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + STANDARDLOGFILENAMETAG + getDateTagForFile() + (std::string)"--" + (std::string)testStr + (std::string)"-" + (std::string)xmlFileNameSpecificExtension_ + (std::string)LOGFILENAMESUFFIX;
								diagService_->setFilePathAndName(lclLogFileName);
								std::cout << "reopening general file as " << diagService_->logFileName_ << std::endl;
								//REOPEN WITH "w"
								outfile.open(diagService_->logFileName_.c_str(),std::ios_base::out);
	    		        	    if (!(outfile.is_open()))
    		    	        	{
	    	        	        	std::stringstream errMsg;
                                                //FILEACCESSERROR
	        	        	    	//std::cout << "Error : Unable to write to destination log file \"" << diagService_->logFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
		        	        	    diagService_->fileLoggingUsable_ = false;
									fileOutIsOpen_ = false;
									fileOut_ = NULL;
	    	    		        }
								else
								{
									fileOutIsOpen_ = true;
									std::cout << "LogFile \"" << diagService_->logFileName_.c_str() << "\" opened in write mode" << std::endl;
									std::string stringToBuild = "";
									
									//Add info to XTRA buffer in case of RN flip
									if (rnHasFlippedForStandardLogFile == true)
									{
										l.setExtraBuffer("--SYSTEM : Run number changed (possibly on STOP->START transition) ; using new logfile. " + l.getExtraBuffer());
										rnHasFlippedForStandardLogFile = false;
									}


									#ifdef WILL_COMPILE_FOR_TRACKER
										#ifdef DATABASE
											findCrateSlotVsHardwareId(&l);
										#endif
										//USE DIRECTLY FIREITEM VALUE FOR FILE LOGGING, COPY TO PROTECTED VAR IS NOT MADE DUE TO NO INIT/CONF COMMAND DENT BY RCMS
										l.buildStringForTrackerLogFileOutput(&stringToBuild, fileLogsCounter, diagService_->feRunFromRcms_);
									#else
										#ifdef WILL_COMPILE_FOR_PIXELS
											l.buildStringForPixelsLogFileOutput(&stringToBuild, fileLogsCounter);
										#else
											l.buildStringForTrackerLogFileOutput(&stringToBuild, fileLogsCounter, diagService_->feRunFromRcms_);
										#endif
									#endif
									//Write message in logfile only if it's not a FEDDEBUG message
                                    //std::string fedMonitorLogPattern = FED_MONITORLOG_PATTERN;
									//size_t fedStringFound;
                                    //std::string errMsg = l.getMessage();
									//fedStringFound=errMsg.find(fedMonitorLogPattern);
									//if (fedStringFound == string::npos)
									//{
										outfile << stringToBuild;
										outfile.flush();
										dumpedlogcounter++;
									//}
								}
								fileLogsCounter=0;
							}
							else
							{
std::cout << "NNNNNNNNNNNNN" << std::endl;
								std::string stringToBuild = "";

								//Add info to XTRA buffer in case of RN flip
								if (rnHasFlippedForStandardLogFile == true)
								{
std::cout << "OOOOOOOOOOO" << std::endl;
									l.setExtraBuffer("--SYSTEM : Run number changed (possibly on STOP->START transition) ; using new logfile. " + l.getExtraBuffer());
									rnHasFlippedForStandardLogFile = false;
								}
std::cout << "PPPPPPPPPPP" << std::endl;
								#ifdef WILL_COMPILE_FOR_TRACKER
									#ifdef DATABASE
										findCrateSlotVsHardwareId(&l);
									#endif
std::cout << "QQQQQQQQQQQ" << std::endl;
									//USE DIRECTLY FIREITEM VALUE FOR FILE LOGGING, COPY TO PROTECTED VAR IS NOT MADE DUE TO NO INIT/CONF COMMAND DENT BY RCMS
									l.buildStringForTrackerLogFileOutput(&stringToBuild, fileLogsCounter, diagService_->feRunFromRcms_);
								#else
									#ifdef WILL_COMPILE_FOR_PIXELS
										l.buildStringForPixelsLogFileOutput(&stringToBuild, fileLogsCounter);
									#else
										l.buildStringForTrackerLogFileOutput(&stringToBuild, fileLogsCounter, diagService_->feRunFromRcms_);
									#endif
								#endif
								//Write message in logfile only if it's not a FEDDEBUG message
								//std::string fedMonitorLogPattern = FED_MONITORLOG_PATTERN;
								//size_t fedStringFound;
								//std::string errMsg = l.getMessage();
								//fedStringFound=errMsg.find(fedMonitorLogPattern);
								//if (fedStringFound == string::npos)
								//{
std::cout << "WRITING : " << stringToBuild << std::endl;
std::cout << "WRITING TO : " << diagService_->logFileName_ << std::endl;
std::cout << "Outfile as pointer is " << &outfile << std::endl;
									outfile << stringToBuild;
									outfile.flush();
									dumpedlogcounter++;
                                //}
							}
//						} (was : if fileout is opened)


*/

}
else
{
//	std::cout << "standardFileLoggingUsable_ is FALSE ; logging disabled" << std::endl;
}


#ifdef WILL_COMPILE_FOR_TRACKER
                    }//No special messages pattern found condition
#endif









//P5MOD

#ifdef WILL_COMPILE_FOR_TRACKER
						//Handle DCU ERRORS messages logging into file
						//Write message in logfile only if it's a DCU ERROR message
/*
						std::string dcuErrorLogPatternOne = FEC_DCUREADOUTERROR_PATTERN;
						std::string dcuErrorLogPatternTwo = FEC_WORKLOOPERROR_PATTERN;
						size_t dcuStringFoundOne;
						size_t dcuStringFoundTwo;
						std::string errMsg = l.getMessage();
						dcuStringFoundOne=errMsg.find(dcuErrorLogPatternOne);
						dcuStringFoundTwo=errMsg.find(dcuErrorLogPatternTwo);
*/
						if ( (dcuStringFoundOne != string::npos) || (dcuStringFoundTwo != string::npos) )
						{
if (dcuFileLoggingUsable_ == true)
{

	//Handle files swap management
	//Rem : fileOutIsOpen_ is supposed to be true ; if not then fileloggingusable is false
	dcuErrorLogCounter++;
	dcuErrorFileLogCounter++;
	if (dcuErrorFileLogCounter >= MAX_NBR_OF_GENERAL_FILE_LOGS)
	{
		//Close file
		std::cout << "closing DCU errors related file " << dcuLogFileName_ << std::endl;
		if (dcuOutfile.is_open()) dcuOutfile.close();
		dcuOutfile.clear();
		dcuFileOutIsOpen_ = false;
		dcuFileOut_ = NULL;
		dcuErrorFileLogCounter = 0;
								
		//Reopen file
		openDcuErrorsLoggingFile();
	}

	//Now take care of writing data into logfile
	//Add info to XTRA buffer in case of RN flip
	if (rnHasFlippedForDcuLogFile == true)
	{
		l.setExtraBuffer("--SYSTEM : Run number changed (possibly on STOP->START transition) ; using new logfile. " + l.getExtraBuffer());
		rnHasFlippedForDcuLogFile = false;
	}

	std::string stringToBuild="";
	#ifdef DATABASE
	findCrateSlotVsHardwareId(&l);
	#endif
	l.buildStringForTrackerLogFileOutput(&stringToBuild, dcuErrorLogCounter, diagService_->feRunFromRcms_);
	dcuOutfile << stringToBuild;
	dcuOutfile.flush();
	dcuDumpedlogcounter++;



/*
						dcuErrorLogCounter++;

						if (dcuFileOutIsOpen_ == true)
						{
							dcuErrorFileLogCounter++;
							if (dcuErrorFileLogCounter >= MAX_NBR_OF_GENERAL_DCUERRORS_LOGS)
							{
								std::cout << "closing DCU errors dedicated file " << dcuLogFileName_ << std::endl;
								if (dcuOutfile.is_open()) dcuOutfile.close();
								std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + DCULOGFILENAMETAG + getDateTagForFile() + (std::string)"--" + (std::string)testStr + (std::string)"-" + (std::string)xmlFileNameSpecificExtension_ + (std::string)LOGFILENAMESUFFIX;
								//diagService_->setFilePathAndName(lclLogFileName);
								dcuLogFileName_ = lclLogFileName;
								std::cout << "reopening DCU errors dedicated file as " << dcuLogFileName_ << std::endl;
								//REOPEN WITH "w"
								dcuOutfile.open(dcuLogFileName_.c_str(),std::ios_base::out);
	    		        	    if (!(dcuOutfile.is_open()))
    		    	        	{
	    	        	        	std::stringstream errMsg;
                                                //FILEACCESSERROR
	        	        	    	//std::cout << "Error : Unable to write to DCU errors dedicated destination log file \"" << dcuLogFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
		        	        	    //diagService_->fileLoggingUsable_ = false;
									dcuFileOutIsOpen_ = false;
									dcuFileOut_ = NULL;
	    	    		        }
								else
								{
									dcuFileOutIsOpen_ = true;
									std::cout << "DCU errors dedicated logFile \"" << dcuLogFileName_.c_str() << "\" opened in write mode" << std::endl;
									std::string stringToBuild = "";

									//Add info to XTRA buffer in case of RN flip
									if (rnHasFlippedForDcuLogFile == true)
									{
										l.setExtraBuffer("--SYSTEM : Run number changed (possibly on STOP->START transition) ; using new logfile. " + l.getExtraBuffer());
										rnHasFlippedForDcuLogFile = false;
									}

									#ifdef DATABASE
										findCrateSlotVsHardwareId(&l);
									#endif
									//USE DIRECTLY FIREITEM VALUE FOR FILE LOGGING, COPY TO PROTECTED VAR IS NOT MADE DUE TO NO INIT/CONF COMMAND DENT BY RCMS
									l.buildStringForTrackerLogFileOutput(&stringToBuild, dcuErrorFileLogCounter, diagService_->feRunFromRcms_);

									//Write message in logfile only if it's not a FEDDEBUG message
                                    //std::string fedMonitorLogPattern = FED_MONITORLOG_PATTERN;
									//size_t fedStringFound;
                                    //std::string errMsg = l.getMessage();
									//fedStringFound=errMsg.find(fedMonitorLogPattern);
									//if (fedStringFound == string::npos)
									//{
										dcuOutfile << stringToBuild;
										dcuOutfile.flush();
										dcuDumpedlogcounter++;
									//}
								}
								dcuErrorFileLogCounter=0;
							}
							else
							{
								std::string stringToBuild = "";

								//Add info to XTRA buffer in case of RN flip
								if (rnHasFlippedForDcuLogFile == true)
								{
									l.setExtraBuffer("--SYSTEM : Run number changed (possibly on STOP->START transition) ; using new logfile. " + l.getExtraBuffer());
									rnHasFlippedForDcuLogFile = false;
								}

								#ifdef DATABASE
									findCrateSlotVsHardwareId(&l);
								#endif
								//USE DIRECTLY FIREITEM VALUE FOR FILE LOGGING, COPY TO PROTECTED VAR IS NOT MADE DUE TO NO INIT/CONF COMMAND DENT BY RCMS
								l.buildStringForTrackerLogFileOutput(&stringToBuild, dcuErrorFileLogCounter, diagService_->feRunFromRcms_);

								//Write message in logfile only if it's not a FEDDEBUG message
								//std::string fedMonitorLogPattern = FED_MONITORLOG_PATTERN;
								//size_t fedStringFound;
								//std::string errMsg = l.getMessage();
								//fedStringFound=errMsg.find(fedMonitorLogPattern);
								//if (fedStringFound == string::npos)
								//{
									dcuOutfile << stringToBuild;
									dcuOutfile.flush();
									dcuDumpedlogcounter++;
                                //}
							}
						}
*/
}
else
{
//	std::cout << "dcuFileLoggingUsable_ is FALSE ; logging disabled" << std::endl;
}

                    }//DCU pattern found condition








						//Handle FED DEBUG messages logging into file
						//Write message in logfile only if it's a DCU ERROR message
						if (fedDebugStringFound != string::npos)
						{
if (fedDebugFileLoggingUsable_ == true)
{



	//Handle files swap management
	//Rem : fileOutIsOpen_ is supposed to be true ; if not then fileloggingusable is false
	fedDebugLogCounter++;
	fedDebugFileLogCounter++;
	if (fedDebugFileLogCounter >= MAX_NBR_OF_GENERAL_FILE_LOGS)
	{
		//Close file
		std::cout << "closing FED Debug related file " << fedDebugLogFileName_ << std::endl;
		if (fedDebugOutfile.is_open()) fedDebugOutfile.close();
		fedDebugOutfile.clear();
		fedDebugFileOutIsOpen_ = false;
		fedDebugFileOut_ = NULL;
		fedDebugFileLogCounter = 0;
								
		//Reopen file
		openFedDebugLoggingFile();
	}

	//Now take care of writing data into logfile
	//Add info to XTRA buffer in case of RN flip
	if (fedDebugLogFileNameHasFlipped == true)
	{
		l.setExtraBuffer("--SYSTEM : Run number changed (possibly on STOP->START transition) ; using new logfile. " + l.getExtraBuffer());
		fedDebugLogFileNameHasFlipped = false;
	}

	std::string stringToBuild="";
	#ifdef DATABASE
	findCrateSlotVsHardwareId(&l);
	#endif
	l.buildStringForTrackerLogFileOutput(&stringToBuild, fedDebugLogCounter, diagService_->feRunFromRcms_);
	fedDebugOutfile << stringToBuild;
	fedDebugOutfile.flush();
	fedDebugFileLogCounter++;

/*
						fedDebugLogCounter++;

						if (fedDebugFileOutIsOpen_ == true)
						{
							fedDebugFileLogCounter++;
							if (fedDebugFileLogCounter >= MAX_NBR_OF_GENERAL_FEDDEBUG_LOGS)
							{
								std::cout << "closing Fed debug dedicated file " << fedDebugLogFileName_ << std::endl;
								if (fedDebugOutfile.is_open()) fedDebugOutfile.close();
								std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + FEDDEBUGLOGFILENAMETAG + getDateTagForFile() + (std::string)"--" + (std::string)testStr + (std::string)"-" + (std::string)xmlFileNameSpecificExtension_ + (std::string)LOGFILENAMESUFFIX;
								//diagService_->setFilePathAndName(lclLogFileName);
								fedDebugLogFileName_ = lclLogFileName;
								std::cout << "reopening FED debug dedicated file as " << dcuLogFileName_ << std::endl;
								//REOPEN WITH "w"
								fedDebugOutfile.open(fedDebugLogFileName_.c_str(),std::ios_base::out);
	    		        	    if (!(fedDebugOutfile.is_open()))
    		    	        	{
	    	        	        	std::stringstream errMsg;
                                                //FILEACCESSERROR
	        	        	    	//std::cout << "Error : Unable to write to FED debug dedicated destination log file \"" << fedDebugLogFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
		        	        	    //diagService_->fileLoggingUsable_ = false;
									fedDebugFileOutIsOpen_ = false;
									fedDebugFileOut_ = NULL;
	    	    		        }
								else
								{
									fedDebugFileOutIsOpen_ = true;
									std::cout << "FED debug dedicated logFile \"" << fedDebugLogFileName_.c_str() << "\" opened in write mode" << std::endl;
									std::string stringToBuild = "";

									//Add info to XTRA buffer in case of RN flip
									if (rnHasFlippedForFedDebugLogFile == true)
									{
										l.setExtraBuffer("--SYSTEM : Run number changed (possibly on STOP->START transition) ; using new logfile. " + l.getExtraBuffer());
										rnHasFlippedForFedDebugLogFile = false;
									}

									#ifdef DATABASE
										findCrateSlotVsHardwareId(&l);
									#endif
									//USE DIRECTLY FIREITEM VALUE FOR FILE LOGGING, COPY TO PROTECTED VAR IS NOT MADE DUE TO NO INIT/CONF COMMAND DENT BY RCMS
									l.buildStringForTrackerLogFileOutput(&stringToBuild, fedDebugFileLogCounter, diagService_->feRunFromRcms_);

									//Write message in logfile only if it's not a FEDDEBUG message
                                    //std::string fedMonitorLogPattern = FED_MONITORLOG_PATTERN;
									//size_t fedStringFound;
                                    //std::string errMsg = l.getMessage();
									//fedStringFound=errMsg.find(fedMonitorLogPattern);
									//if (fedStringFound == string::npos)
									//{
										fedDebugOutfile << stringToBuild;
										fedDebugOutfile.flush();
										fedDebugDumpedlogcounter++;
									//}
								}
								fedDebugFileLogCounter=0;
							}
							else
							{
								std::string stringToBuild = "";

								//Add info to XTRA buffer in case of RN flip
								if (rnHasFlippedForFedDebugLogFile == true)
								{
									l.setExtraBuffer("--SYSTEM : Run number changed (possibly on STOP->START transition) ; using new logfile. " + l.getExtraBuffer());
									rnHasFlippedForFedDebugLogFile = false;
								}

								#ifdef DATABASE
									findCrateSlotVsHardwareId(&l);
								#endif
								//USE DIRECTLY FIREITEM VALUE FOR FILE LOGGING, COPY TO PROTECTED VAR IS NOT MADE DUE TO NO INIT/CONF COMMAND DENT BY RCMS
								l.buildStringForTrackerLogFileOutput(&stringToBuild, fedDebugFileLogCounter, diagService_->feRunFromRcms_);

								//Write message in logfile only if it's not a FEDDEBUG message
								//std::string fedMonitorLogPattern = FED_MONITORLOG_PATTERN;
								//size_t fedStringFound;
								//std::string errMsg = l.getMessage();
								//fedStringFound=errMsg.find(fedMonitorLogPattern);
								//if (fedStringFound == string::npos)
								//{
									fedDebugOutfile << stringToBuild;
									fedDebugOutfile.flush();
									fedDebugDumpedlogcounter++;
                                //}
							}
						}

*/
}
else
{
//	std::cout << "fedDebugFileLoggingUsable_ is FALSE ; logging disabled" << std::endl;
}

                    }//FED DEBUG pattern found condition










						//Handle FED TEMPERATURE MONITORING messages logging into file
						//Write message in logfile only if it's a DCU ERROR message
						if (fedTmonStringFound != string::npos)
						{
if (fedTmonFileLoggingUsable_ == true)
{

	//Handle files swap management
	//Rem : fileOutIsOpen_ is supposed to be true ; if not then fileloggingusable is false
	fedTmonLogCounter++;
	fedTmonFileLogCounter++;
	if (fedTmonFileLogCounter >= MAX_NBR_OF_GENERAL_FILE_LOGS)
	{
		//Close file
		std::cout << "closing FED Temperature monitoring file " << fedTmonLogFileName_ << std::endl;
		if (fedTmonfile.is_open()) fedTmonfile.close();
		fedTmonfile.clear();
		fedTmonFileOutIsOpen_ = false;
		fedTmonFileOut_ = NULL;
		fedTmonFileLogCounter = 0;
								
		//Reopen file
		openFedTmonLoggingFile();
	}

	//Now take care of writing data into logfile
	//Add info to XTRA buffer in case of RN flip
	if (fedTempMonLogFileNameHasFlipped == true)
	{
		l.setExtraBuffer("--SYSTEM : Run number changed (possibly on STOP->START transition) ; using new logfile. " + l.getExtraBuffer());
		fedTempMonLogFileNameHasFlipped = false;
	}

	std::string stringToBuild="";
	#ifdef DATABASE
	findCrateSlotVsHardwareId(&l);
	#endif
	l.buildStringForTrackerLogFileOutput(&stringToBuild, fedTmonLogCounter, diagService_->feRunFromRcms_);
	fedTmonfile << stringToBuild;
	fedTmonfile.flush();
	fedTmonFileLogCounter++;

/*
						fedTmonLogCounter++;

						if (fedTmonFileOutIsOpen_ == true)
						{
							fedTmonFileLogCounter++;
							if (fedTmonFileLogCounter >= MAX_NBR_OF_GENERAL_FEDTEMPMONITORING_LOGS)
							{
								std::cout << "closing Fed Tmon dedicated file " << fedTmonLogFileName_ << std::endl;
								if (fedTmonfile.is_open()) fedTmonfile.close();
								std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + FEDTMONLOGFILENAMETAG + getDateTagForFile() + (std::string)"--" + (std::string)testStr + (std::string)"-" + (std::string)xmlFileNameSpecificExtension_ + (std::string)LOGFILENAMESUFFIX;
								//diagService_->setFilePathAndName(lclLogFileName);
								fedTmonLogFileName_ = lclLogFileName;
								std::cout << "reopening FED Tmon dedicated file as " << fedTmonLogFileName_ << std::endl;
								//REOPEN WITH "w"
								fedTmonfile.open(fedTmonLogFileName_.c_str(),std::ios_base::out);
	    		        	    if (!(fedTmonfile.is_open()))
    		    	        	{
	    	        	        	std::stringstream errMsg;
                                                //FILEACCESSERROR
	        	        	    	//std::cout << "Error : Unable to write to FED Tmon dedicated destination log file \"" << fedTmonLogFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
		        	        	    //diagService_->fileLoggingUsable_ = false;
									fedTmonFileOutIsOpen_ = false;
									fedTmonFileOut_ = NULL;
	    	    		        }
								else
								{
									fedTmonFileOutIsOpen_ = true;
									std::cout << "FED Tmon dedicated logFile \"" << fedTmonLogFileName_.c_str() << "\" opened in write mode" << std::endl;
									std::string stringToBuild = "";

									//Add info to XTRA buffer in case of RN flip
									if (rnHasFlippedForFedTmonLogFile == true)
									{
										l.setExtraBuffer("--SYSTEM : Run number changed (possibly on STOP->START transition) ; using new logfile. " + l.getExtraBuffer());
										rnHasFlippedForFedTmonLogFile = false;
									}

									#ifdef DATABASE
										findCrateSlotVsHardwareId(&l);
									#endif
									//USE DIRECTLY FIREITEM VALUE FOR FILE LOGGING, COPY TO PROTECTED VAR IS NOT MADE DUE TO NO INIT/CONF COMMAND SENT BY RCMS
									l.buildStringForTrackerLogFileOutput(&stringToBuild, fedTmonFileLogCounter, diagService_->feRunFromRcms_);

									//Write message in logfile only if it's not a FEDDEBUG message
                                    //std::string fedMonitorLogPattern = FED_MONITORLOG_PATTERN;
									//size_t fedStringFound;
                                    //std::string errMsg = l.getMessage();
									//fedStringFound=errMsg.find(fedMonitorLogPattern);
									//if (fedStringFound == string::npos)
									//{
										fedTmonfile << stringToBuild;
										fedTmonfile.flush();
										fedTmonDumpedlogcounter++;
									//}
								}
								fedTmonFileLogCounter=0;
							}
							else
							{
								std::string stringToBuild = "";

								//Add info to XTRA buffer in case of RN flip
								if (rnHasFlippedForFedTmonLogFile == true)
								{
									l.setExtraBuffer("--SYSTEM : Run number changed (possibly on STOP->START transition) ; using new logfile. " + l.getExtraBuffer());
									rnHasFlippedForFedTmonLogFile = false;
								}

								#ifdef DATABASE
									findCrateSlotVsHardwareId(&l);
								#endif
								//USE DIRECTLY FIREITEM VALUE FOR FILE LOGGING, COPY TO PROTECTED VAR IS NOT MADE DUE TO NO INIT/CONF COMMAND DENT BY RCMS
								l.buildStringForTrackerLogFileOutput(&stringToBuild, fedTmonFileLogCounter, diagService_->feRunFromRcms_);
								//Write message in logfile only if it's not a FEDDEBUG message
								//std::string fedMonitorLogPattern = FED_MONITORLOG_PATTERN;
								//size_t fedStringFound;
								//std::string errMsg = l.getMessage();
								//fedStringFound=errMsg.find(fedMonitorLogPattern);
								//if (fedStringFound == string::npos)
								//{
									fedTmonfile << stringToBuild;
									fedTmonfile.flush();
									fedTmonDumpedlogcounter++;
                                //}
							}
						}
*/
}
else
{
//	std::cout << "fedTmonFileLoggingUsable_ is FALSE ; logging disabled" << std::endl;
}


                    }//FED TEMPERATURE MONITORING  pattern found condition






						//Handle FED SPY CHANNEL messages logging into file
						//Write message in logfile only if it's a DCU ERROR message
						if (fedSpyStringFound != string::npos)
						{

if (fedSpyFileLoggingUsable_ == true)
{



	//Handle files swap management
	//Rem : fileOutIsOpen_ is supposed to be true ; if not then fileloggingusable is false
	fedSpyLogCounter++;
	fedSpyFileLogCounter++;
	if (fedSpyFileLogCounter >= MAX_NBR_OF_GENERAL_FILE_LOGS)
	{
		//Close file
		std::cout << "closing FED Spy channel dedicated file " << fedSpyLogFileName_ << std::endl;
		if (fedSpyFile.is_open()) fedSpyFile.close();
		fedSpyFile.clear();
		fedSpyFileOutIsOpen_ = false;
		fedSpyFileOut_ = NULL;
		fedSpyFileLogCounter = 0;
								
		//Reopen file
		openFedSpyLoggingFile();
	}

	//Now take care of writing data into logfile
	//Add info to XTRA buffer in case of RN flip
	if (fedSpyLogFileNameHasFlipped == true)
	{
		l.setExtraBuffer("--SYSTEM : Run number changed (possibly on STOP->START transition) ; using new logfile. " + l.getExtraBuffer());
		fedSpyLogFileNameHasFlipped = false;
	}

	std::string stringToBuild="";
	#ifdef DATABASE
	findCrateSlotVsHardwareId(&l);
	#endif
	l.buildStringForTrackerLogFileOutput(&stringToBuild, fedSpyLogCounter, diagService_->feRunFromRcms_);
	fedSpyFile << stringToBuild;
	fedSpyFile.flush();
	fedSpyFileLogCounter++;


/*
						fedSpyLogCounter++;

						if (fedSpyFileOutIsOpen_ == true)
						{
							fedSpyFileLogCounter++;
							if (fedSpyFileLogCounter >= MAX_NBR_OF_GENERAL_FEDTEMPMONITORING_LOGS)
							{
								std::cout << "closing Fed Spy dedicated file " << fedSpyLogFileName_ << std::endl;
								if (fedSpyFile.is_open()) fedSpyFile.close();
								std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + FEDSPYLOGFILENAMETAG + getDateTagForFile() + (std::string)"--" + (std::string)testStr + (std::string)"-" + (std::string)xmlFileNameSpecificExtension_ + (std::string)LOGFILENAMESUFFIX;
								//diagService_->setFilePathAndName(lclLogFileName);
								fedSpyLogFileName_ = lclLogFileName;
								std::cout << "reopening FED Spy dedicated file as " << fedSpyLogFileName_ << std::endl;
								//REOPEN WITH "w"
								fedSpyFile.open(fedSpyLogFileName_.c_str(),std::ios_base::out);
	    		        	    if (!(fedSpyFile.is_open()))
    		    	        	{
	    	        	        	std::stringstream errMsg;
                                                //FILEACCESSERROR
	        	        	    	//std::cout << "Error : Unable to write to FED Spy messages dedicated destination log file \"" << fedSpyLogFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
		        	        	    //diagService_->fileLoggingUsable_ = false;
									fedSpyFileOutIsOpen_ = false;
									fedSpyFileOut_ = NULL;
	    	    		        }
								else
								{
									fedSpyFileOutIsOpen_ = true;
									std::cout << "FED Spy dedicated logFile \"" << fedSpyLogFileName_.c_str() << "\" opened in write mode" << std::endl;
									std::string stringToBuild = "";

									//Add info to XTRA buffer in case of RN flip
									if (rnHasFlippedForFedSpyLogFile == true)
									{
										l.setExtraBuffer("--SYSTEM : Run number changed (possibly on STOP->START transition) ; using new logfile. " + l.getExtraBuffer());
										rnHasFlippedForFedSpyLogFile = false;
									}

									#ifdef DATABASE
										findCrateSlotVsHardwareId(&l);
									#endif
									//USE DIRECTLY FIREITEM VALUE FOR FILE LOGGING, COPY TO PROTECTED VAR IS NOT MADE DUE TO NO INIT/CONF COMMAND SENT BY RCMS
									l.buildStringForTrackerLogFileOutput(&stringToBuild, fedSpyFileLogCounter, diagService_->feRunFromRcms_);

									//Write message in logfile only if it's not a FEDDEBUG message
                                    //std::string fedMonitorLogPattern = FED_MONITORLOG_PATTERN;
									//size_t fedStringFound;
                                    //std::string errMsg = l.getMessage();
									//fedStringFound=errMsg.find(fedMonitorLogPattern);
									//if (fedStringFound == string::npos)
									//{
										fedSpyFile << stringToBuild;
										fedSpyFile.flush();
										fedSpyDumpedlogcounter++;
									//}
								}
								fedSpyFileLogCounter=0;
							}
							else
							{
								std::string stringToBuild = "";

								//Add info to XTRA buffer in case of RN flip
								if (rnHasFlippedForFedSpyLogFile == true)
								{
									l.setExtraBuffer("--SYSTEM : Run number changed (possibly on STOP->START transition) ; using new logfile. " + l.getExtraBuffer());
									rnHasFlippedForFedSpyLogFile = false;
								}

								#ifdef DATABASE
									findCrateSlotVsHardwareId(&l);
								#endif
								//USE DIRECTLY FIREITEM VALUE FOR FILE LOGGING, COPY TO PROTECTED VAR IS NOT MADE DUE TO NO INIT/CONF COMMAND DENT BY RCMS
								l.buildStringForTrackerLogFileOutput(&stringToBuild, fedSpyFileLogCounter, diagService_->feRunFromRcms_);
								//Write message in logfile only if it's not a FEDDEBUG message
								//std::string fedMonitorLogPattern = FED_MONITORLOG_PATTERN;
								//size_t fedStringFound;
								//std::string errMsg = l.getMessage();
								//fedStringFound=errMsg.find(fedMonitorLogPattern);
								//if (fedStringFound == string::npos)
								//{
									fedSpyFile << stringToBuild;
									fedSpyFile.flush();
									fedSpyDumpedlogcounter++;
                                //}
							}
						}
*/

}
else
{
//	std::cout << "fedSpyFileLoggingUsable_ is FALSE ; logging disabled" << std::endl;
}

                    }//FED TEMPERATURE MONITORING  pattern found condition




#endif




				}//End of tests on loglevel to decide if we have to enter the FILE LOGGING process or not

//                }








//std::cout << "Processing CHAINSAW condition" << std::endl;
                /* IF we have to route errors to CHAINSAW */

/*
if (diagService_->fiAreInitialized_ == true)
{
*/

//DEBUGME
//std::cout << "diagService_->nbrOfChainsawStreams_ = " << diagService_->nbrOfChainsawStreams_ << std::endl;

for (int csStreamNum = 0; csStreamNum < diagService_->nbrOfChainsawStreams_ ; csStreamNum++)
{
                /* IF we have to route errors to CHAINSAW */
                if ( (diagService_->csRoute_[csStreamNum] == true) && (diagService_->csRouteValidated_[csStreamNum] == true) )
                {
//std::cout << "AAAAAAAAAA" << std::endl;

                    /* Si le lvl general du message est superieur ou egal au level lcl DB,
                    OU si on est en forced log et que le lvl general du message est superieur ou egal au lvl de FORCE_DB_LOG */
                    if ( getLogLevelToLong(l.getLevel()) >= getLogLevelToLong(diagService_->csLogLevel_[csStreamNum]) )
                    {
//std::cout << "BBBBBBBBBB" << std::endl;

                        #ifdef DEBUG_ED_ONRECEIVEDLOG
                            std::cout << "CS logging enabled for current levels on stream " << diagService_->csReceiver_[csStreamNum] << std::endl;
                        #endif
						//std::cout << "Begin to append to socket" << std::endl;
                        //diagService_->chainSawManager_->pushClogToStream(diagService_->csReceiver_[csStreamNum], l);

/*
					std::vector<CLog>::iterator clogIterator = RcmsClogList_.begin();
					RcmsClogList_.insert( clogIterator, l );

					std::vector<int>::iterator snumIterator = RcmsStreamNumList_.begin();
					RcmsStreamNumList_.insert( snumIterator, csStreamNum );
*/
//std::cout << "RCMS messages piled up" << std::endl;


                        diagService_->pushClogToFilteredStreams(l, csStreamNum);
						//diagService_->chainsawAppender_->appendToSocket(l);
						//std::cout << "End to append to socket" << std::endl;




                    }
                }
}

/*
}
else
{
for (int csStreamNum = 0; csStreamNum < diagService_->nbrOfChainsawStreams_ ; csStreamNum++)
{

					std::vector<CLog>::iterator clogIterator = RcmsClogList_.begin();
					RcmsClogList_.insert( clogIterator, l );

					std::vector<int>::iterator snumIterator = RcmsStreamNumList_.begin();
					RcmsStreamNumList_.insert( snumIterator, csStreamNum );
}


}

*/
//std::cout << "Processing CONSOLE condition" << std::endl;

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

		
		}
 










        /* Processes incoming LOG messages.
        Routes the messages to default appender and/or console and/or chainsaw and/or file.
        Routes the messages to DiagLevelOne.
        DO NOT USE LOG4C CALLS IN THIS METHOD, IT IS NOT REENTRANT */
        xoap::MessageReference GlobalErrorDispatcher::runNumberRequest (xoap::MessageReference msg) throw (xoap::exception::Exception)
        {



            /* post reply to caller */
            xoap::MessageReference reply = xoap::createMessage();
            xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
            xoap::SOAPName responseName = envelope.createName( "runNumberRequestResponse", "xdaq", XDAQ_NS_URI);
            envelope.getBody().addBodyElement ( responseName );

	    /* Add requested attributes to SOAP part */
	    xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
	    std::vector<xoap::SOAPElement> logElement = rb.getChildElements ();

	    xoap::SOAPName currentRunNumber ("currentRunNumber", "", "");

            std::ostringstream rnFromRcms;
            rnFromRcms << diagService_->feRunFromRcms_;
		logElement[0].addAttribute(currentRunNumber,rnFromRcms.str());

            return reply;
        }










        /* Processes incoming LOG messages.
        Routes the messages to default appender and/or console and/or chainsaw and/or file.
        Routes the messages to DiagLevelOne.
        DO NOT USE LOG4C CALLS IN THIS METHOD, IT IS NOT REENTRANT */
        xoap::MessageReference GlobalErrorDispatcher::receivedLog (xoap::MessageReference msg) throw (xoap::exception::Exception)
        {
RCMSMutex_->take();

//P5MODBIS
			//Take care of run number management
			if (runNumberIsInitialized_ == false)
			{
				if (diagService_->feRunFromRcms_ <= 0)
				{
					//std::cout << "Run number is uninitialised yet" << std::endl;
				}
				else
				{
					//std::cout << "Run number received for first initilisation ; run number setted to " << diagService_->feRunFromRcms_ << std::endl;
					oldRunNumber_ = diagService_->feRunFromRcms_;
					//currentRunNumber_ = diagService_->feRunFromRcms_;
					runNumberIsInitialized_ = true;
				}
			}
			else //if (runNumberIsInitialized_ == true)
			{
				if (diagService_->feRunFromRcms_ == oldRunNumber_)
				{
					//std::cout << "Still running with the same run number : " << diagService_->feRunFromRcms_ << std::endl;
				}
				else
				{
					//std::cout << "Run number change detected, from " << oldRunNumber_ << " to " << diagService_->feRunFromRcms_ << std::endl;
					
					//Backup previous value for futur message
					xdata::Integer lclOldRunNumber = oldRunNumber_;
					
					//Set oldRunNumber_ to new value
					oldRunNumber_ = diagService_->feRunFromRcms_;
					
					//Close all old files and reset indicators
					outfile.close();
					#ifdef WILL_COMPILE_FOR_TRACKER
						dcuOutfile.close();
						fedDebugOutfile.close();
						fedTmonfile.close();
					#endif

					//Set filenames changes
					diagService_->setFilePathAndName("");
					diagService_->logFileNameHasChanged_ = true;
					fileOutIsOpen_ = false;
					fileOut_ = NULL;
					
					#ifdef WILL_COMPILE_FOR_TRACKER
						dcuLogFileNameHasChanged_ = true;
						dcuFileOutIsOpen_ = false;
						dcuFileOut_ = NULL;
					
						fedDebugLogFileNameHasChanged_ = true;
						fedDebugFileOutIsOpen_ = false;
						fedDebugFileOut_ = NULL;
					
						fedTmonLogFileNameHasChanged_ = true;
						fedTmonFileOutIsOpen_ = false;
						fedTmonFileOut_ = NULL;
					#endif
					

					//Set indicators for tagging xtra buffer after RN change
					rnHasFlippedForStandardLogFile = true;
					#ifdef WILL_COMPILE_FOR_TRACKER
						rnHasFlippedForDcuLogFile = true;
						rnHasFlippedForFedDebugLogFile = true;
						rnHasFlippedForFedTmonLogFile = true;
						rnHasFlippedForFedSpyLogFile = true;
					#endif



				}
			
			}		
			





            /* post reply to caller */
            xoap::MessageReference reply = xoap::createMessage();
            xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
            xoap::SOAPName responseName = envelope.createName( ANSWER_TO_COMMAND_TRIGGED_WHEN_ED_RECEIVED_A_LOG, "xdaq", XDAQ_NS_URI);
            envelope.getBody().addBodyElement ( responseName );

            /* Convert SOAP message into CLog message and get process timestamp */
            CLog l;
			getMessage(msg, &l);

						
			

			if (diagService_->fiAreInitialized_ == true)
			{
//std::cout << "FireItems are initialized, I will process the incoming log" << std::endl;
				processReceivedLog(l);
			}
			else
			{
//std::cout << "FireItems are not initialized, I will store the incoming log for future processing" << std::endl;
				std::vector<CLog>::iterator clogIterator = RcmsClogList_.begin();
				RcmsClogList_.insert( clogIterator, l );
			}



RCMSMutex_->give();

            return reply;
        }





        /* Turns the GlobalErrorDispatcher into state INITIALISED (FSM point of view) */
        void GlobalErrorDispatcher::configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
        {
            /* Apply fireitems value to internal variables and check validity */
			//std::cout << "Calling DIAG_EXEC_FSM_INIT_TRANS from configureAction" << std::endl;
//            DIAG_EXEC_FSM_INIT_TRANS
	    diagService_->feRunFromRcms_ = 111;

        }



        /* Turns the GlobalErrorDispatcher into state HALTED (FSM point of view) */
        void GlobalErrorDispatcher::haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
        {
            /* Apply fireitems value to internal variables and check validity */
            DIAG_EXEC_FSM_STOP_TRANS
        }
   

   
        /* Allows the FSM to be controlled via a soap message carrying a "fireEvent" message */
        xoap::MessageReference GlobalErrorDispatcher::fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception)
        {
            //#define DEBUG_FIREEVENT
            #ifdef DEBUG_FIREEVENT
                std::cout << "In GlobalErrorDispatcher : fireEvent received" << std::endl ;
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
                    if ( ((fsm_.getCurrentState() == 'H') && (commandName == EXECTRANSHC)) || ((fsm_.getCurrentState() == 'C') && (commandName == EXECTRANSCH)) )
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
        void GlobalErrorDispatcher::failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
        {
            toolbox::fsm::FailedEvent & fe = dynamic_cast<toolbox::fsm::FailedEvent&>(*e);
            std::ostringstream msgError;
            msgError << "Failure occurred when performing transition from: " << fe.getFromState() << " to: " << fe.getToState() << " exception: " << fe.getException().what();
            diagService_->reportError (msgError.str(), DIAGERROR);
        }



        /* Can be called via the <configure_FSM> hyperdaq HTML page of this process */
        void GlobalErrorDispatcher::configureStateMachine(xgi::Input * in, xgi::Output * out)
        {

            if (fsm_.getCurrentState() == 'H')
            {
                toolbox::Event::Reference e(new toolbox::Event(EXECTRANSHC, this));
                fsm_.fireEvent(e);
            }

            Default(in, out);
        }




















        /* Can be called via the <configure_FSM> hyperdaq HTML page of this process */
        void GlobalErrorDispatcher::stopStateMachine(xgi::Input * in, xgi::Output * out)
        {
            if (fsm_.getCurrentState() == 'C')
            {
                toolbox::Event::Reference e(new toolbox::Event(EXECTRANSCH, this));
                fsm_.fireEvent(e);
            }
                Default(in, out);
        }







bool GlobalErrorDispatcher::openLogFile (	std::string typeOfFile,
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
											FILE * filePointer) 
{

	if ((*fileIsopen) == false)
	{
		std::ostringstream thisMachineAsText;
		thisMachineAsText << this->getApplicationLogger().getName();
		std::string lclLogFileName = path + prefix + tag + getDateTagForFile() + (std::string)"--" + procinfo + (std::string)"-" + extension + suffix;

		if (typeOfFile == "STANDARD") {diagService_->setFilePathAndName(lclLogFileName);}

#ifdef WILL_COMPILE_FOR_TRACKER
		else if (typeOfFile == "DCU") {dcuLogFileName_ = lclLogFileName;}
		else if (typeOfFile == "FEDDEBUG") {fedDebugLogFileName_ = lclLogFileName;}
		else if (typeOfFile == "FEDTMON") {fedTmonLogFileName_ = lclLogFileName;}
		else if (typeOfFile == "FEDSPY") {fedSpyLogFileName_ = lclLogFileName;}
#endif
		if ((*fileChangedNotifier) == true)
		{
			try
			{
				outfile.open(lclLogFileName.c_str(),std::ios_base::out);
				(*fileChangedNotifier) = false;
				// next line useful only in case of "default" config keyword
				if (typeOfFile == "STANDARD") diagService_->oldLogFileName_ = diagService_->logFileName_;
				(*fileIsopen) = true;
				//(*fileisUsable) = true;

			}
			catch(...)
			{
				(*fileIsopen) = false;
				//(*fileisUsable) = false;
			}
		}
		else
		{
			try
			{
				outfile.open(lclLogFileName.c_str(),std::ios_base::app);
				(*fileIsopen) = true;
				//(*fileisUsable) = true;
			}
			catch(...)
			{
				(*fileIsopen) = false;
				//(*fileisUsable) = false;
			}
		}

		if (!(outfile.is_open()))
		{
			std::stringstream errMsg;
			//FILEACCESSERROR				
			//std::cout << "Error : Unable to open destination file \"" << diagService_->logFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
			//(*fileisUsable) = false;
			(*fileIsopen) = false;
			filePointer = NULL;
			outfile.clear();
			return false;
		}
		else return true;
	}
	else return true;
return false;
}






void GlobalErrorDispatcher::openStandardLoggingFile ()
{
standardFileLoggingUsable_ = true;
	int filesTryCounter = 0;
	do
	{
		bool fileIsOpenedWithParameterDefinedPAth = openLogFile (	(std::string)"STANDARD",
																	&(diagService_->logFileNameHasChanged_),
																	//&(diagService_->fileLoggingUsable_),
																	&standardFileLoggingUsable_,
																	&fileOutIsOpen_,
																	(std::string)diagService_->logFilePath_,
																	(std::string)LOGFILENAMEPREFIX,
																	STANDARDLOGFILENAMETAG,
																	getCleanUidName(),
																	(std::string)xmlFileNameSpecificExtension_,
																	(std::string)LOGFILENAMESUFFIX,
																	outfile,
																	fileOut_);
		if (fileIsOpenedWithParameterDefinedPAth == true)
		{
			std::cout <<"File " << diagService_->logFileName_ << " successfully opened" << std::endl;
			return;
		}
		else
		{
			if (filesTryCounter == 0)
			{ 
				std::cout <<"Failed opening file " << diagService_->logFileName_ << " ; attempt logging into default repository " << DEFAULT_LOGGING_REPOSITORY << std::endl;
				diagService_->logFilePath_ = DEFAULT_LOGGING_REPOSITORY;
				diagService_->logFileNameHasChanged_ = true;
			}
			else
			{
				std::cout <<"Failed opening file " << diagService_->logFileName_ << " ; aborting logging process" << std::endl;
				standardFileLoggingUsable_ = false;
			}
		}
		filesTryCounter++;
	} while (filesTryCounter < 2);
}
			


#ifdef WILL_COMPILE_FOR_TRACKER

void GlobalErrorDispatcher::openDcuErrorsLoggingFile ()
{
	dcuFileLoggingUsable_ = true;
	int filesTryCounter = 0;
	do
	{
		bool fileIsOpenedWithParameterDefinedPAth = openLogFile (	(std::string)"DCU",
																	&dcuLogFileNameHasChanged_,
																	&dcuFileLoggingUsable_,
																	&dcuFileOutIsOpen_,
																	(std::string)diagService_->logFilePath_,
																	(std::string)LOGFILENAMEPREFIX,
																	DCULOGFILENAMETAG,
																	getCleanUidName(),
																	(std::string)xmlFileNameSpecificExtension_,
																	(std::string)LOGFILENAMESUFFIX,
																	dcuOutfile,
																	dcuFileOut_);
		if (fileIsOpenedWithParameterDefinedPAth == true)
		{
			std::cout <<"File " << dcuLogFileName_ << " successfully opened" << std::endl;
			return;
		}
		else
		{
			if (filesTryCounter == 0)
			{ 
				std::cout <<"Failed opening file " << dcuLogFileName_ << " ; attempt logging into default repository " << DEFAULT_LOGGING_REPOSITORY << std::endl;
				diagService_->logFilePath_ = DEFAULT_LOGGING_REPOSITORY;
				dcuLogFileNameHasChanged_ = true;
			}
			else
			{
				std::cout <<"Failed opening file " << dcuLogFileName_ << " ; aborting logging process" << std::endl;
				dcuFileLoggingUsable_ = false;
			}
		}
		filesTryCounter++;
	} while (filesTryCounter < 2);
}



void GlobalErrorDispatcher::openFedDebugLoggingFile ()
{
	fedDebugFileLoggingUsable_ = true;
	int filesTryCounter = 0;
	do
	{
		bool fileIsOpenedWithParameterDefinedPAth = openLogFile (	(std::string)"FEDDEBUG",
																	&fedDebugLogFileNameHasChanged_,
																	&fedDebugFileLoggingUsable_,
																	&fedDebugFileOutIsOpen_,
																	(std::string)diagService_->logFilePath_,
																	(std::string)LOGFILENAMEPREFIX,
																	FEDDEBUGLOGFILENAMETAG,
																	getCleanUidName(),
																	(std::string)xmlFileNameSpecificExtension_,
																	(std::string)LOGFILENAMESUFFIX,
																	fedDebugOutfile,
																	fedDebugFileOut_);
		if (fileIsOpenedWithParameterDefinedPAth == true)
		{
			std::cout <<"File " << fedDebugLogFileName_ << " successfully opened" << std::endl;
			return;
		}
		else
		{
			if (filesTryCounter == 0)
			{ 
				std::cout <<"Failed opening file " << fedDebugLogFileName_ << " ; attempt logging into default repository " << DEFAULT_LOGGING_REPOSITORY << std::endl;
				diagService_->logFilePath_ = DEFAULT_LOGGING_REPOSITORY;
				fedDebugLogFileNameHasChanged_ = true;
			}
			else
			{
				std::cout <<"Failed opening file " << fedDebugLogFileName_ << " ; aborting logging process" << std::endl;
				fedDebugFileLoggingUsable_ = false;
			}
		}
		filesTryCounter++;
	} while (filesTryCounter < 2);
}









void GlobalErrorDispatcher::openFedTmonLoggingFile ()
{
	fedTmonFileLoggingUsable_ = true;
	int filesTryCounter = 0;
	do
	{
		bool fileIsOpenedWithParameterDefinedPAth = openLogFile (	(std::string)"FEDTMON",
																	&fedTmonLogFileNameHasChanged_,
																	&fedTmonFileLoggingUsable_,
																	&fedTmonFileOutIsOpen_,
																	(std::string)diagService_->logFilePath_,
																	(std::string)LOGFILENAMEPREFIX,
																	FEDTMONLOGFILENAMETAG,
																	getCleanUidName(),
																	(std::string)xmlFileNameSpecificExtension_,
																	(std::string)LOGFILENAMESUFFIX,
																	fedTmonfile,
																	fedTmonFileOut_);
		if (fileIsOpenedWithParameterDefinedPAth == true)
		{
			std::cout <<"File " << fedTmonLogFileName_ << " successfully opened" << std::endl;
			return;
		}
		else
		{
			if (filesTryCounter == 0)
			{ 
				std::cout <<"Failed opening file " << fedTmonLogFileName_ << " ; attempt logging into default repository " << DEFAULT_LOGGING_REPOSITORY << std::endl;
				diagService_->logFilePath_ = DEFAULT_LOGGING_REPOSITORY;
				fedTmonLogFileNameHasChanged_ = true;
			}
			else
			{
				std::cout <<"Failed opening file " << fedTmonLogFileName_ << " ; aborting logging process" << std::endl;
				fedTmonFileLoggingUsable_ = false;
			}
		}
		filesTryCounter++;
	} while (filesTryCounter < 2);
}








void GlobalErrorDispatcher::openFedSpyLoggingFile ()
{
	fedSpyFileLoggingUsable_ = true;
	int filesTryCounter = 0;
	do
	{
		bool fileIsOpenedWithParameterDefinedPAth = openLogFile (	(std::string)"FEDSPY",
																	&fedSpyLogFileNameHasChanged_,
																	&fedSpyFileLoggingUsable_,
																	&fedSpyFileOutIsOpen_,
																	(std::string)diagService_->logFilePath_,
																	(std::string)LOGFILENAMEPREFIX,
																	FEDSPYLOGFILENAMETAG,
																	getCleanUidName(),
																	(std::string)xmlFileNameSpecificExtension_,
																	(std::string)LOGFILENAMESUFFIX,
																	fedSpyFile,
																	fedSpyFileOut_);
		if (fileIsOpenedWithParameterDefinedPAth == true)
		{
			std::cout <<"File " << fedSpyLogFileName_ << " successfully opened" << std::endl;
			return;
		}
		else
		{
			if (filesTryCounter == 0)
			{ 
				std::cout <<"Failed opening file " << fedSpyLogFileName_ << " ; attempt logging into default repository " << DEFAULT_LOGGING_REPOSITORY << std::endl;
				diagService_->logFilePath_ = DEFAULT_LOGGING_REPOSITORY;
				fedSpyLogFileNameHasChanged_ = true;
			}
			else
			{
				std::cout <<"Failed opening file " << fedSpyLogFileName_ << " ; aborting logging process" << std::endl;
				fedSpyFileLoggingUsable_ = false;
			}
		}
		filesTryCounter++;
	} while (filesTryCounter < 2);
}

#endif





std::string GlobalErrorDispatcher::getCleanUidName ()
{
					int glbed_procuid_length = 1000;
					char testStr[glbed_procuid_length];
					strncpy(testStr, this->getApplicationLogger().getName().c_str(), (glbed_procuid_length-1) );
					for (int ii=0; ii<glbed_procuid_length; ii++)
					{
						if (testStr[ii] == '.') testStr[ii] = '-';
						if (testStr[ii] == ')') testStr[ii] = '-';
						if (testStr[ii] == '(') testStr[ii] = '-';
						if (testStr[ii] == ':') testStr[ii] = '-';
					}
return (std::string)testStr;
}


#ifdef AUTO_CONFIGURE_PROCESSES
void GlobalErrorDispatcher::timeExpired (toolbox::task::TimerEvent& e) 
{
	if (isFirstTimerTick_ == true)
	{
		DIAG_EXEC_FSM_INIT_TRANS



//		openStandardLoggingFile();




/*
					//Processing STANDARD log file creation
					//std::cout << "In first timer tick block ; processing STANDARD log file generation" << std::endl;
					if (fileOutIsOpen_ == false)
					{
						std::ostringstream thisMachineAsText;
						thisMachineAsText << this->getApplicationLogger().getName();
						std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + STANDARDLOGFILENAMETAG + getDateTagForFile() + (std::string)"--" + (std::string)testStr + (std::string)"-" + (std::string)xmlFileNameSpecificExtension_ + (std::string)LOGFILENAMESUFFIX;
						diagService_->setFilePathAndName(lclLogFileName);
						if (diagService_->logFileNameHasChanged_ == true)
						{
							outfile.open(diagService_->logFileName_.c_str(),std::ios_base::out);
							diagService_->logFileNameHasChanged_ = false;
							// next line useful only in case of "default" config keyword
							diagService_->oldLogFileName_ = diagService_->logFileName_;
							fileOutIsOpen_ = true;
						}
						else
						{
							outfile.open(diagService_->logFileName_.c_str(),std::ios_base::app);
							fileOutIsOpen_ = true;
						}

    		            if (!(outfile.is_open()))
    	    	        {
	            	        std::stringstream errMsg;
                                //FILEACCESSERROR				
                	    	//std::cout << "Error : Unable to open destination file \"" << diagService_->logFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
	                	    diagService_->fileLoggingUsable_ = false;
							fileOutIsOpen_ = false;
							fileOut_ = NULL;
    	    	        }
        		        else
    	        	    {
							std::cout << "LogFile \"" << diagService_->logFileName_.c_str() << "\" opened in write mode" << std::endl;
	    	            }

					}
*/
//P5MOD
#ifdef WILL_COMPILE_FOR_TRACKER
/*
		openDcuErrorsLoggingFile();
		openFedDebugLoggingFile();
		openFedTmonLoggingFile();
		openFedSpyLoggingFile();
*/

/*

openLogFile (	(std::string)"DCU",
				&dcuLogFileNameHasChanged_,
				&dcuFileLoggingUsable_,
				&dcuFileOutIsOpen_,
				(std::string)diagService_->logFilePath_,
				(std::string)LOGFILENAMEPREFIX,
				DCULOGFILENAMETAG,
				getCleanUidName(),
				(std::string)xmlFileNameSpecificExtension_,
				(std::string)LOGFILENAMESUFFIX,
				dcuOutfile,
				dcuFileOut_);
*/


/*
std::cout << "Summary from standard file opening in FTT" << std::endl;
if (fileOutIsOpen_ == true){std::cout << "fileOutIsOpen_ = TRUE" << std::endl;} else std::cout << "fileOutIsOpen_ = FALSE" << std::endl;
if (diagService_->logFileNameHasChanged_ == true){std::cout << "diagService_->logFileNameHasChanged_ = TRUE" << std::endl;} else std::cout << "diagService_->logFileNameHasChanged_ = FALSE" << std::endl;
if (diagService_->fileLoggingUsable_ == true){std::cout << "diagService_->fileLoggingUsable_ = TRUE" << std::endl;} else std::cout << "diagService_->fileLoggingUsable_ = FALSE" << std::endl;
std::cout << std::endl;
*/

/*

					//Processing DCU ERRORS log file creation
					//std::cout << "In first timer tick block ; processing DCU ERRORS log file generation" << std::endl;
					if (dcuFileOutIsOpen_ == false)
					{
						std::ostringstream thisMachineAsText;
						thisMachineAsText << this->getApplicationLogger().getName();
						std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + DCULOGFILENAMETAG + getDateTagForFile() + (std::string)"--" + (std::string)testStr + (std::string)"-" + (std::string)xmlFileNameSpecificExtension_ + (std::string)LOGFILENAMESUFFIX;
						//diagService_->setFilePathAndName(lclLogFileName);
						dcuLogFileName_ = lclLogFileName;
						if (dcuLogFileNameHasChanged_ == true)
						{
							dcuOutfile.open(dcuLogFileName_.c_str(),std::ios_base::out);
							dcuLogFileNameHasChanged_ = false;
							// next line useful only in case of "default" config keyword
							//diagService_->oldLogFileName_ = diagService_->logFileName_;
							dcuFileOutIsOpen_ = true;
						}
						else
						{
							dcuOutfile.open(dcuLogFileName_.c_str(),std::ios_base::app);
							dcuFileOutIsOpen_ = true;
						}

    		            if (!(dcuOutfile.is_open()))
    	    	        {
	            	        std::stringstream errMsg;				
                                //FILEACCESSERROR
                	    	//std::cout << "Error : Unable to open DCU errors destination file \"" << dcuLogFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
							dcuFileOutIsOpen_ = false;
							dcuFileOut_ = NULL;
    	    	        }
        		        else
    	        	    {
							std::cout << "DCU errors LogFile \"" << dcuLogFileName_.c_str() << "\" opened in write mode" << std::endl;
	    	            }

					}
*/





					//Processing FED DEBUG log file creation
/*
openLogFile (	(std::string)"FEDDEBUG",
				&fedDebugLogFileNameHasChanged_,
				&fedDebugFileLoggingUsable_,
				&fedDebugFileOutIsOpen_,
				(std::string)diagService_->logFilePath_,
				(std::string)LOGFILENAMEPREFIX,
				FEDDEBUGLOGFILENAMETAG,
				getCleanUidName(),
				(std::string)xmlFileNameSpecificExtension_,
				(std::string)LOGFILENAMESUFFIX,
				fedDebugOutfile,
				fedDebugFileOut_);
*/
/*
					//std::cout << "In first timer tick block ; processing FED DEBUG log file generation" << std::endl;
					if (fedDebugFileOutIsOpen_ == false)
					{
						std::ostringstream thisMachineAsText;
						thisMachineAsText << this->getApplicationLogger().getName();
						std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + FEDDEBUGLOGFILENAMETAG + getDateTagForFile() + (std::string)"--" + (std::string)testStr + (std::string)"-" + (std::string)xmlFileNameSpecificExtension_ + (std::string)LOGFILENAMESUFFIX;
						//diagService_->setFilePathAndName(lclLogFileName);
						fedDebugLogFileName_ = lclLogFileName;
						if (fedDebugLogFileNameHasChanged_ == true)
						{
							fedDebugOutfile.open(fedDebugLogFileName_.c_str(),std::ios_base::out);
							fedDebugLogFileNameHasChanged_ = false;
							// next line useful only in case of "default" config keyword
							//diagService_->oldLogFileName_ = diagService_->logFileName_;
							fedDebugFileOutIsOpen_ = true;
						}
						else
						{
							fedDebugOutfile.open(fedDebugLogFileName_.c_str(),std::ios_base::app);
							fedDebugFileOutIsOpen_ = true;
						}

    		            if (!(fedDebugOutfile.is_open()))
    	    	        {
	            	        std::stringstream errMsg;				
                                //FILEACCESSERROR
                	    	//std::cout << "Error : Unable to open FED DEBUG destination file \"" << fedDebugLogFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
							fedDebugFileOutIsOpen_ = false;
							fedDebugFileOut_ = NULL;
    	    	        }
        		        else
    	        	    {
							std::cout << "FED DEBUG LogFile \"" << fedDebugLogFileName_.c_str() << "\" opened in write mode" << std::endl;
	    	            }

					}
*/



					//Processing FED TEMPERATURE MONITORING log file creation
/*
openLogFile (	(std::string)"FEDTMON",
				&fedTmonLogFileNameHasChanged_,
				&fedTmonFileLoggingUsable_,
				&fedTmonFileOutIsOpen_,
				(std::string)diagService_->logFilePath_,
				(std::string)LOGFILENAMEPREFIX,
				FEDTMONLOGFILENAMETAG,
				getCleanUidName(),
				(std::string)xmlFileNameSpecificExtension_,
				(std::string)LOGFILENAMESUFFIX,
				fedTmonfile,
				fedTmonFileOut_);
*/
/*
					//std::cout << "In first timer tick block ; processing FED TEMPERATURE MONITORING log file generation" << std::endl;
					if (fedTmonFileOutIsOpen_ == false)
					{
						std::ostringstream thisMachineAsText;
						thisMachineAsText << this->getApplicationLogger().getName();
						std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + FEDTMONLOGFILENAMETAG + getDateTagForFile() + (std::string)"--" + (std::string)testStr + (std::string)"-" + (std::string)xmlFileNameSpecificExtension_ + (std::string)LOGFILENAMESUFFIX;
						//diagService_->setFilePathAndName(lclLogFileName);
						fedTmonLogFileName_ = lclLogFileName;
						if (fedTmonLogFileNameHasChanged_ == true)
						{
							fedTmonfile.open(fedTmonLogFileName_.c_str(),std::ios_base::out);
							fedTmonLogFileNameHasChanged_ = false;
							// next line useful only in case of "default" config keyword
							//diagService_->oldLogFileName_ = diagService_->logFileName_;
							fedTmonFileOutIsOpen_ = true;
						}
						else
						{
							fedTmonfile.open(fedTmonLogFileName_.c_str(),std::ios_base::app);
							fedTmonFileOutIsOpen_ = true;
						}

    		            if (!(fedTmonfile.is_open()))
    	    	        {
	            	        std::stringstream errMsg;				
                                //FILEACCESSERROR
                	    	//std::cout << "Error : Unable to open FED Tmon destination file \"" << fedTmonLogFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
							fedTmonFileOutIsOpen_ = false;
							fedTmonFileOut_ = NULL;
    	    	        }
        		        else
    	        	    {
							std::cout << "FED TEMPERATURE MONITORING LogFile \"" << fedTmonLogFileName_.c_str() << "\" opened in write mode" << std::endl;
	    	            }

					}
*/




					//Processing FED SPY CHANNEL messages log file creation
					//std::cout << "In first timer tick block ; processing FED SPY CHANNEL messages log file generation" << std::endl;
/*
openLogFile (	(std::string)"FEDSPY",
				&fedSpyLogFileNameHasChanged_,
				&fedSpyFileLoggingUsable_,
				&fedSpyFileOutIsOpen_,
				(std::string)diagService_->logFilePath_,
				(std::string)LOGFILENAMEPREFIX,
				FEDSPYLOGFILENAMETAG,
				getCleanUidName(),
				(std::string)xmlFileNameSpecificExtension_,
				(std::string)LOGFILENAMESUFFIX,
				fedSpyFile,
				fedSpyFileOut_);
*/
/*

				    	if (fedSpyFileOutIsOpen_ == false)
						{
							std::ostringstream thisMachineAsText;
							thisMachineAsText << this->getApplicationLogger().getName();
							std::string lclLogFileName = (std::string)diagService_->logFilePath_ + (std::string)LOGFILENAMEPREFIX + FEDSPYLOGFILENAMETAG + getDateTagForFile() + (std::string)"--" + (std::string)testStr + (std::string)"-" + (std::string)xmlFileNameSpecificExtension_ + (std::string)LOGFILENAMESUFFIX;
							//diagService_->setFilePathAndName(lclLogFileName);//logFileName_ = fileName;
							fedSpyLogFileName_ = lclLogFileName;
							
							if (fedSpyLogFileNameHasChanged_ == true)
							{
								fedSpyFile.open(fedSpyLogFileName_.c_str(),std::ios_base::out);
								fedSpyLogFileNameHasChanged_ = false;
								fedSpyFileOutIsOpen_ = true;
							}
							else
							{
								fedSpyFile.open(fedSpyLogFileName_.c_str(),std::ios_base::app);
								fedSpyFileOutIsOpen_ = true;
							}
							
						    if (!(fedSpyFile.is_open()))
    	    	        	{
	            	        	std::stringstream errMsg;				
                                        //FILEACCESSERROR
	                	    	//std::cout << "Error : Unable to write to FED Spy channel messages dedicated destination log file \"" << fedSpyLogFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
		                	    //diagService_->fileLoggingUsable_ = false;
								fedSpyFileOutIsOpen_ = false;
								fedSpyFileOut_ = NULL;
    	    		        }
        			        else
    	        		    {
								std::cout << "FED Spy channel messages dedicated logFile \"" << fedSpyLogFileName_.c_str() << "\" opened in write mode" << std::endl;
		    	            }

						}
						
*/

#endif

//P5MODEND

/*
				}
*/
				
//std::cout << "Hahaha, faking initialization entry" << std::endl;
doInstanciationExtraBehaviour();
//std::cout << "Fake initialization done" << std::endl;

				
				
                std::cout << "GlobalErrorDispatcher(LID=" << getApplicationDescriptor()->getLocalId() << ",Instance=" << getApplicationDescriptor()->getInstance() << ") ready" << std::endl;
				//std::cout << "EXITING TIMER:csReceiver_[0]=" << diagService_->csReceiver_[0] << std::endl;

//				GedTimerHasBeenFiredAtLeastOnce_ = true;


		isFirstTimerTick_ = false;

		//After first tick, modify tickrate to approx. 1 minute between each ErrorDb connaction attempt
		isFirstTimerTick_ = false;

		toolbox::TimeInterval interval(2,0);
		toolbox::TimeVal start;
		start = toolbox::TimeVal::gettimeofday() + interval;
		timer_->scheduleAtFixedRate( start, this, interval, this->getApplicationDescriptor()->getContextDescriptor(), "" );
//std::cout << "Rescheduling timer after fireitems initialisation" << std::endl;

	}
	else
	{


//DEBUGME
//std::cout << "I WILL FLUSH " << RcmsClogList_.size() << " MESSAGES" << std::endl;

//GLAP
//std::cout << "Run number is at NON FIRST timer tick " << diagService_->feRunFromRcms_ << std::endl;


//std::cout << "Falling in second timer tick" << std::endl;

				if (RcmsClogList_.size() > 0)
				{

					//std::cout << "Flushing stored logs buffer..." << std::endl;
					while (RcmsClogList_.empty() == false)
					{

						CLog tmpClog = *(RcmsClogList_.end() - 1);

						//std::cout << "Clog message = : " << tmpClog.getMessage() << std::endl;

						processReceivedLog(tmpClog);			
						RcmsClogList_.pop_back();

						//DEBUGME
						//std::cout << "RcmsClogList_.size() size is now : " << RcmsClogList_.size() << std::endl;

					}
				}

//std::cout << "Disabling timer" << std::endl;
timer_->stop();
//std::cout << "Timer Disabled" << std::endl;
	}
//std::cout << "Exiting Autoconfiguration timer method" << std::endl;

          }
        #endif




        /* Read the SOAP message and create a CLog structure
        DO NOT USE LOG4C CALLS IN THIS METHOD, IT IS NOT REENTRANT */
        void GlobalErrorDispatcher::getMessage(xoap::MessageReference m, CLog * l) throw (xoap::exception::Exception)
        {
            xoap::SOAPBody rb = m->getSOAPPart().getEnvelope().getBody();
            if (rb.hasFault() )
            {
                xoap::SOAPFault fault = rb.getFault();
                std::string msg = "GlobalErrorDispatcher: ";
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
						//std::cout << "In GlobalErrorDispatcher : ExtraBuffer = " << l->getExtraBuffer() << std::endl;



    	    	    	xoap::SOAPName nestedSubClassPath ("nestedSubClassPath", "", "");
    	    	    	l->setSubClassPath(logElement[0].getAttributeValue(nestedSubClassPath));
						//std::cout << "In GlobalErrorDispatcher : nestedSubClassPath = " << l->getSubClassPath() << std::endl;

    	    	    	xoap::SOAPName nestedFileName ("nestedFileName", "", "");
    	    	    	l->setFileName(logElement[0].getAttributeValue(nestedFileName));
						//std::cout << "In GlobalErrorDispatcher : nestedFileName = " << l->getFileName() << std::endl;

    	    	    	xoap::SOAPName nestedLineNumber ("nestedLineNumber", "", "");
			    		l->setLineNumberAsString(logElement[0].getAttributeValue(nestedLineNumber));
			    		l->setLineNumber(atoi(logElement[0].getAttributeValue(nestedLineNumber).c_str()));
						//std::cout << "In GlobalErrorDispatcher : nestedLineNumber = " << l->getLineNumber() << std::endl;

			#ifdef DATABASE

			if (isRunningOnP5Machines_ == true)
			{


				std::string::size_type locA, locB;
				std::string loggerName = l->getLogger();
	//			std::cout << "Working on string " << loggerName << std::endl;
				locA = loggerName.find( "vmepc", 0 );
	//			std::cout << "vmepc pos found at " << locA << std::endl;
				locB = loggerName.find( ".p", 0 );
	//			std::cout << ".p pos found at " << locB << std::endl;
				std::string machineName = "";
				if ( (locA != std::string::npos) && (locB != std::string::npos) )
				{
					machineName = loggerName.substr(locA, locB-locA);
				}
				//std::cout << "Got my machine name : " << machineName << std::endl;
				if (machineName != "")
				{
					//Find partition according to machine name
					bool partitionFound = false;


					//Check if message belongs to TIB partition
					for (int m=0; m<nbOfMachinesInTib_; m++)
					{
						if (machineName == tibMachinesNamesList_[m])
						{
							l->setPartitionName(tibPartitionName_);
							partitionFound = true;
							m = nbOfMachinesInTib_;
						}
					}
					
					//Check if message belongs to TOB partition
					if (partitionFound == false)
					{
						for (int m=0; m<nbOfMachinesInTob_; m++)
						{
							if (machineName == tobMachinesNamesList_[m])
							{
								l->setPartitionName(tobPartitionName_);
								partitionFound = true;
								m = nbOfMachinesInTob_;
							}
						}
					}
					
	
	
					//Check if message belongs to TECM partition
					if (partitionFound == false)
					{
						for (int m=0; m<nbOfMachinesInTecm_; m++)
						{
							if (machineName == tecmMachinesNamesList_[m])
							{
								l->setPartitionName(tecmPartitionName_);
								partitionFound = true;
								m = nbOfMachinesInTecm_;
							}
						}
					}
	

					//Check if message belongs to TECM partition
					if (partitionFound == false)
					{
						for (int m=0; m<nbOfMachinesInTecp_; m++)
						{
							if (machineName == tecpMachinesNamesList_[m])
							{
								l->setPartitionName(tecpPartitionName_);
								partitionFound = true;
								m = nbOfMachinesInTecp_;
							}
						}
					}

					//Just in case, fallthru trap
					if (partitionFound == false) l->setPartitionName("");
				}
				else l->setPartitionName("");
			}
			else l->setPartitionName(DbPartitionName_);
			#else
				l->setPartitionName("");
			#endif

/*
		    		}
*/


                } /* End of if (logsize==1) */
            } /* End of No BODY FAULT block */
            /* MUST manage exceptions here */
        }




		void GlobalErrorDispatcher::checkIfReconfigurationIsNeeded(CLog * l) throw (xoap::exception::Exception)
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


        xoap::MessageReference GlobalErrorDispatcher::freeReconfCmd (xoap::MessageReference msg) throw (xoap::exception::Exception)
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
        std::string GlobalErrorDispatcher::getDateTagForFile(void)
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




        std::string GlobalErrorDispatcher::getDateTagForFile(void)
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
		
		
		
		
		

xoap::MessageReference GlobalErrorDispatcher::getCurrentLogFileName(xoap::MessageReference msg) throw (xoap::exception::Exception)
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






void GlobalErrorDispatcher::readCsEnvSettings (void)
{

diagService_->useCsEnvSettings_ = true;

char* readCsEnvMachine = NULL;
readCsEnvMachine=getenv ("TK_CS_MACHINE_NAME_FOR_GED") ;
if (readCsEnvMachine != NULL)
{
		diagService_->csConfMachine_=(std::string)readCsEnvMachine;
//		std::cout << "GED will log error messages to CS. CS machine name is : " << diagService_->csConfMachine_ << std::endl;		
}
else
{
//	std::cout << "GED CSENV error. Machine name undefined in environment" << std::endl;		
	diagService_->useCsEnvSettings_ = false;
}


char* readCsEnvPort = NULL;
readCsEnvPort=getenv ("TK_CS_PORT_NUMBER_FOR_GED") ;
if (readCsEnvPort != NULL)
{
		diagService_->csConfPort_=(std::string)readCsEnvPort;
//		std::cout << "GED will log error messages to CS. CS port number is : " << diagService_->csConfPort_ << std::endl;		
}
else
{
//	std::cout << "GED CSENV error. Port number undefined in environment" << std::endl;		
	diagService_->useCsEnvSettings_ = false;
}


char* readCsEnvReceiverName = NULL;
readCsEnvReceiverName=getenv ("TK_CS_RECEIVER_NAME_FOR_GED") ;
if (readCsEnvReceiverName != NULL)
{
		diagService_->csConfReceiverName_=(std::string)readCsEnvReceiverName;
//		std::cout << "GED will log error messages to CS. CS receiver name is : " << diagService_->csConfReceiverName_ << std::endl;		
}
else
{
//	std::cout << "GED CSENV error. Receiver Name undefined in environment" << std::endl;		
	diagService_->useCsEnvSettings_ = false;
}

char* readCsEnvLogLevel = NULL;
readCsEnvLogLevel=getenv ("TK_CS_LOG_LEVEL_FOR_GED") ;
if (readCsEnvLogLevel != NULL)
{
		diagService_->csConfLogLevel_=(std::string)readCsEnvLogLevel;
//		std::cout << "GED will log error messages to CS. CS log level is : " << diagService_->readCsEnvLogLevel << std::endl;		
}
else
{
//	std::cout << "GED CSENV error. LogLevel undefined in environment" << std::endl;
	diagService_->useCsEnvSettings_ = false;
}

if (diagService_->useCsEnvSettings_ == true)
{
	std::cout << "GED will use Chainsaw environment definitions :" <<
	" MachineName="	<< diagService_->csConfMachine_ <<
	" PortNumber=" << diagService_->csConfPort_ <<
	" ReceiverName=" << diagService_->csConfReceiverName_ <<
	" LogLevel=" << diagService_->csConfLogLevel_ <<
	std::endl;
}



}




void GlobalErrorDispatcher::doInstanciationExtraBehaviour(void)
{
	#ifdef DATABASE
		tkDiagErrorAnalyser = NULL;
		tkTibDiagErrorAnalyser = NULL;
		tkTobDiagErrorAnalyser = NULL;
		tkTecpDiagErrorAnalyser = NULL;
		tkTecmDiagErrorAnalyser = NULL;
		isDbUsable = false;

		DbPartitionName_ = "";

		for (int i=0; i<DIAG_MAX_PARTITIONS_NBR; i++)
		{
			DbPartitionNameArray_[i] = "";
		}
		part_counter_ = 0;
		
		
		tibPartitionName_ = "";
		tobPartitionName_ = "";
		tecpPartitionName_ = "";
		tecmPartitionName_ = "";

		isRunningOnP5Machines_ = false;
	#endif
	#ifdef DATABASE
		// Which DB partition do we want to use ?
		//Extract from environment/RCMS or ask explicit ?
		readEnv=getenv ("ENV_CMS_TK_PARTITION") ;
		if ( (readEnv != NULL) && (std::string(readEnv) != "") )
		{
			//std::cout << "readEnv = " << readEnv << std::endl;
			partitionName = std::string(readEnv);
			DbPartitionName_ = partitionName;

			//Si on a trouve le DB PARTITION NAME, est ce qu'on est en mono parttiion ou en multi partition?
			std::string::size_type locA, locB;
			locA = 0;
			locB = 0;
			do
			{
				try
				{
				locB = DbPartitionName_.find( ":", locA );
//				if ( (locA != std::string::npos) && (locB != std::string::npos) ) DbPartitionNameArray_[part_counter_] = DbPartitionName_.substr(locA, locB-locA);
				DbPartitionNameArray_[part_counter_] = DbPartitionName_.substr(locA, locB-locA);
				}
				catch(...){}

//				if( locB != std::string::npos )
//				{
//					std::cout << "Found : occurence in string" << std::endl;
					//std::cout << "Substring is : " << DbPartitionName_.substr(locA, locB-locA) << std::endl;
					part_counter_++;
//				}
//				else
//				{
				
//				}
				locA = locB+1;
			} while (locB != std::string::npos);
			std::cout << "I'm working with " << part_counter_ << " partition(s)" << std::endl;
			for (int j=0; j<part_counter_; j++) std::cout << DbPartitionNameArray_[j] << std::endl;



			for (int k=0; k< part_counter_; k++)
			{
				std::string::size_type locTIB=0, locTOB=0, locTECP=0, locTECM=0;
				locTIB = DbPartitionNameArray_[k].find( tibPartitionPrefix, 0 );
				if (locTIB != std::string::npos)
				{
					tibPartitionName_ = DbPartitionNameArray_[k];
					isRunningOnP5Machines_ = true;
				}

				locTOB = DbPartitionNameArray_[k].find( tobPartitionPrefix, 0 );
				if (locTOB != std::string::npos)
				{
					tobPartitionName_ = DbPartitionNameArray_[k];
					isRunningOnP5Machines_ = true;
				}


				locTECP = DbPartitionNameArray_[k].find( tecpPartitionPrefix, 0 );
				if (locTECP != std::string::npos)
				{
					tecpPartitionName_ = DbPartitionNameArray_[k];
					isRunningOnP5Machines_ = true;
				}


				locTECM = DbPartitionNameArray_[k].find( tecmPartitionPrefix, 0 );
				if (locTECM != std::string::npos)
				{
					tecmPartitionName_ = DbPartitionNameArray_[k];
					isRunningOnP5Machines_ = true;
				}

			}
			
			if (isRunningOnP5Machines_ == true)
			{
				if (tibPartitionName_ != "") std::cout << "tibPartitionName_ = " << tibPartitionName_ << std::endl;
				if (tobPartitionName_ != "") std::cout << "tobPartitionName_ = " << tobPartitionName_ << std::endl;
				if (tecpPartitionName_ != "") std::cout << "tecpPartitionName_ = " << tecpPartitionName_ << std::endl;
				if (tecmPartitionName_ != "") std::cout << "tecmPartitionName_ = " << tecmPartitionName_ << std::endl;
			}
			else std::cout << "Running with NON-P5 partition = " << DbPartitionName_ << std::endl;



		}

		else
		{
			//std::cout << "readEnv is NULL " << std::endl;
			std::cerr << "Please set the ENV_CMS_TK_PARTITION environemental variable as ENV_CMS_TK_PARTITION=PartitionName" << std::endl ;
			DbPartitionName_ = "";
		}
	#endif



	#ifdef DATABASE
	
//	if ( (readEnv != NULL) && (std::string(readEnv) != "") )
//	{
//std::cout << "Checking availability of DB accesses" << std::endl;

	// Create the database access
	// retreive the connection of the database through an environmental variable called CONFDB
	std::string login="nil", passwd="nil", path="nil" ;
	DbAccess::getDbConfiguration (login, passwd, path) ;
	DbLogin_ = login;
	DbPasswd_ = passwd;
	DbPath_ = path;
	if (login == "nil" || passwd=="nil" || path=="nil")
	{
		std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
	}
	else
	{
	
		if (isRunningOnP5Machines_ == false)
		{
			try
			{
				tkDiagErrorAnalyser = new TkDiagErrorAnalyser ( DbPartitionName_ ) ; //, login, passwd, path ) ;
				if (tkDiagErrorAnalyser->getConnectionNotIdentified() != 0) std::cerr << "Found " << tkDiagErrorAnalyser->getConnectionNotIdentified() << " connections not identified with DCU ID" << std::endl ;
				if (tkDiagErrorAnalyser->getPsuNotIdentified() != 0) std::cerr << "Found " << tkDiagErrorAnalyser->getPsuNotIdentified() << " PSU not identified with DCU hardware ID" << std::endl ;
				//Let's suppose that once this point reached the DB is usable
				if (tkDiagErrorAnalyser != NULL) isDbUsable = true;
			}
			catch (oracle::occi::SQLException &e)
			{
				std::cerr << "Cannot create the access to the database: " << login << "/" << "******" << "@" << path << std::endl ;
				std::cerr << e.what() << std::endl ;
			}
			catch (FecExceptionHandler &e)
			{
				std::cerr << "Cannot create the access to DB: " << login << "/" << "******" << "@" << path << std::endl ;
				std::cerr << e.what() << std::endl ;
			}
			catch (...)
			{
				std::cerr << "Unattended exception during DB access creation" << std::endl ;
			}
		}



		else
		{
			if (tibPartitionName_ != "")
			{
				try
				{
					tkTibDiagErrorAnalyser = new TkDiagErrorAnalyser ( tibPartitionName_ ) ; //, login, passwd, path ) ;
					if (tkTibDiagErrorAnalyser->getConnectionNotIdentified() != 0) std::cerr << "Found " << tkTibDiagErrorAnalyser->getConnectionNotIdentified() << " connections not identified with DCU ID" << std::endl ;
					if (tkTibDiagErrorAnalyser->getPsuNotIdentified() != 0) std::cerr << "Found " << tkTibDiagErrorAnalyser->getPsuNotIdentified() << " PSU not identified with DCU hardware ID" << std::endl ;
					//Let's suppose that once this point reached the DB is usable
					if (tkTibDiagErrorAnalyser != NULL) isDbUsable = true;
				}
				catch (oracle::occi::SQLException &e)
				{
					std::cerr << "Cannot create the access to the database: " << login << "/" << "******" << "@" << path << std::endl ;
					std::cerr << e.what() << std::endl ;
				}
				catch (FecExceptionHandler &e)
				{
					std::cerr << "Cannot create the access to DB: " << login << "/" << "******" << "@" << path << std::endl ;
					std::cerr << e.what() << std::endl ;
				}
				catch (...)
				{
					std::cerr << "Unattended exception during DB access creation" << std::endl ;
				}
			}
		



			if (tobPartitionName_ != "")
			{
				try
				{
					tkTobDiagErrorAnalyser = new TkDiagErrorAnalyser ( tobPartitionName_ ) ; //, login, passwd, path ) ;
					if (tkTobDiagErrorAnalyser->getConnectionNotIdentified() != 0) std::cerr << "Found " << tkTobDiagErrorAnalyser->getConnectionNotIdentified() << " connections not identified with DCU ID" << std::endl ;
					if (tkTobDiagErrorAnalyser->getPsuNotIdentified() != 0) std::cerr << "Found " << tkTobDiagErrorAnalyser->getPsuNotIdentified() << " PSU not identified with DCU hardware ID" << std::endl ;
					//Let's suppose that once this point reached the DB is usable
					if (tkTobDiagErrorAnalyser != NULL) isDbUsable = true;
				}
				catch (oracle::occi::SQLException &e)
				{
					std::cerr << "Cannot create the access to the database: " << login << "/" << "******" << "@" << path << std::endl ;
					std::cerr << e.what() << std::endl ;
				}
				catch (FecExceptionHandler &e)
				{
					std::cerr << "Cannot create the access to DB: " << login << "/" << "******" << "@" << path << std::endl ;
					std::cerr << e.what() << std::endl ;
				}
				catch (...)
				{
					std::cerr << "Unattended exception during DB access creation" << std::endl ;
				}
			}






			if (tecpPartitionName_ != "")
			{
				try
				{
					tkTecpDiagErrorAnalyser = new TkDiagErrorAnalyser ( tecpPartitionName_ ) ; //, login, passwd, path ) ;
					if (tkTecpDiagErrorAnalyser->getConnectionNotIdentified() != 0) std::cerr << "Found " << tkTecpDiagErrorAnalyser->getConnectionNotIdentified() << " connections not identified with DCU ID" << std::endl ;
					if (tkTecpDiagErrorAnalyser->getPsuNotIdentified() != 0) std::cerr << "Found " << tkTecpDiagErrorAnalyser->getPsuNotIdentified() << " PSU not identified with DCU hardware ID" << std::endl ;
					//Let's suppose that once this point reached the DB is usable
					if (tkTecpDiagErrorAnalyser != NULL) isDbUsable = true;
				}
				catch (oracle::occi::SQLException &e)
				{
					std::cerr << "Cannot create the access to the database: " << login << "/" << "******" << "@" << path << std::endl ;
					std::cerr << e.what() << std::endl ;
				}
				catch (FecExceptionHandler &e)
				{
					std::cerr << "Cannot create the access to DB: " << login << "/" << "******" << "@" << path << std::endl ;
					std::cerr << e.what() << std::endl ;
				}
				catch (...)
				{
					std::cerr << "Unattended exception during DB access creation" << std::endl ;
				}
			}




			if (tecmPartitionName_ != "")
			{
				try
				{
					tkTecmDiagErrorAnalyser = new TkDiagErrorAnalyser ( tecmPartitionName_ ) ; //, login, passwd, path ) ;
					if (tkTecmDiagErrorAnalyser->getConnectionNotIdentified() != 0) std::cerr << "Found " << tkTecmDiagErrorAnalyser->getConnectionNotIdentified() << " connections not identified with DCU ID" << std::endl ;
					if (tkTecmDiagErrorAnalyser->getPsuNotIdentified() != 0) std::cerr << "Found " << tkTecmDiagErrorAnalyser->getPsuNotIdentified() << " PSU not identified with DCU hardware ID" << std::endl ;
					//Let's suppose that once this point reached the DB is usable
					if (tkTecmDiagErrorAnalyser != NULL) isDbUsable = true;
				}
				catch (oracle::occi::SQLException &e)
				{
					std::cerr << "Cannot create the access to the database: " << login << "/" << "******" << "@" << path << std::endl ;
					std::cerr << e.what() << std::endl ;
				}
				catch (FecExceptionHandler &e)
				{
					std::cerr << "Cannot create the access to DB: " << login << "/" << "******" << "@" << path << std::endl ;
					std::cerr << e.what() << std::endl ;
				}
				catch (...)
				{
					std::cerr << "Unattended exception during DB access creation" << std::endl ;
				}
			}




		}
                

                
//	}
//std::cout << "End of Checking availability of DB accesses" << std::endl;



//sleep((unsigned int)5);





	}
	#endif

}


/*

void GlobalErrorDispatcher::readSubDetectorTypeEnvSettings (void)
{

char* readEnv = NULL;
readEnv=getenv ("USE_DIAGSYSTEM_FOR_PIXELS") ;
if (readEnv != NULL)
{
	if ( std::string(readEnv) == "TRUE" )
	{
		isRunningForPixels_ = true;
		std::cout << "GED will run in PIXELS mode" << std::endl;
	}
}
if ( isRunningForPixels_ == false ) std::cout << "GED will run in TRACKER mode" << std::endl;

}
*/





