/*

   FileName : 		LogReader.cc

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


#include "LogReader.h"

XDAQ_INSTANTIATOR_IMPL(LogReader);








LogReader::LogReader(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): xdaq::Application(s)
{
	//Initialise DB access validation

	#ifdef DATABASE
		tkDiagErrorAnalyser = NULL;
		isDbUsable = false;
		
	#endif

	//Initialise input file pointer
	fileOut_ = NULL;

	#ifdef DATABASE
		// Which DB partition do we want to use ?
		//Extract from environment/RCMS or ask explicit ?
		readEnv=getenv ("ENV_CMS_TK_PARTITION") ;
		if (readEnv != NULL)
		{
			partitionName = std::string(readEnv);
			DbPartitionName_ = partitionName;
		}
		else
		{
			std::cerr << "Please set the ENV_CMS_TK_PARTITION environemental variable as ENV_CMS_TK_PARTITION=PartitionName" << std::endl ;
			DbPartitionName_ = "";
		}

		//Initialize display to HIDE tree
		displayNonFaultyItems_ = false;
		subDetectorShowDetails_ = true;
		allFecsShowDetails_ = false;
		allFedsShowDetails_ = false;
		allPsusShowDetails_ = false;

		for (int i=0; i<NUMBER_OF_FEC_CRATES; i++)
		{
			fecCratesShowDetails_[i] = false;
			for (int j=0; j<NUMBER_OF_FEC_SLOTS; j++)
			{
				fecSlotItemShowDetails_[i][j]=false;
				for (int k=0; k<NUMBER_OF_FEC_RING; k++)
				{
					fecRingItemShowDetails_[i][j][k]=false;
					for (int l=0; l<NUMBER_OF_FEC_CCU; l++)
					{
						fecCcuItemShowDetails_[i][j][k][l]=false;
						for (int m=0; m<NUMBER_OF_FEC_CHANNEL; m++)
						{
							fecChannelItemShowDetails_[i][j][k][l][m]=false;
						}
					}
				}
			}
		}

		for (int i=0; i<NUMBER_OF_FED_CRATES; i++)
		{
			fedCratesShowDetails_[i] = false;
			for (int j=0; j<NUMBER_OF_FED_SLOTS; j++)
			{
				fedSlotItemShowDetails_[i][j]=false;
			}
		}

	#endif

	fileoutIsOpen_ = false;
	arrayDesc = 0;
	lastKnownPos = 0;
	lastKnownGoodPos = lastKnownPos;
	newLogsFound = false;
	    
	mustDisplayOneAppDiagSettings_ = false;
	mustDisplayGroupAppDiagSettings_ = false;
	mustDisplayWorldAppDiagSettings_ = false;

	    
	// Give funny and useless informations at load time
	std::stringstream mmesg;
	mmesg << "Process version " << LOGREADER_PACKAGE_VERSION << " loaded";
	this->getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);
	LOG4CPLUS_INFO(this->getApplicationLogger(), mmesg.str());
	this->getApplicationLogger().setLogLevel(ERROR_LOG_LEVEL);

	// Set Default() metyhod state as : initialising....
	internalState_ = 0;

	for (int logarrayinitialiser=0; logarrayinitialiser < LOGARRAYLINES ; logarrayinitialiser++)
	{
		logsMemory[logarrayinitialiser][MSGOFFSET] = "UNINITIALIZED_BT_ED";
		copyOfLogsMemory[logarrayinitialiser][MSGOFFSET] = "UNINITIALIZED_BT_ED";
	}
	arrayliner = 0;
	copyOfArrayliner = 0;
	processingSortRequest = false;

	sortMessagesGrowing = true;
	sortOriginGrowing = true;
	sortTimestampGrowing = true;
	sortLevelGrowing = true;
	sortErrcodeGrowing = true;

	logcounter = 0;

	autoRefreshEnabled_ = false;
	refreshRate_ = 60;
	textRefreshRate_ = "60";
			
			
	// Instanciate diagnostic object
	diagService_ = new DiagBagWizard(
                                    getApplicationDescriptor()->getClassName() + "DiagLvlOne" ,
                                    this->getApplicationLogger(),
                                    getApplicationDescriptor()->getClassName(),
                                    getApplicationDescriptor()->getInstance(),
                                    getApplicationDescriptor()->getLocalId(),
                                    (xdaq::WebApplication *)this );


	// bind xgi and xoap commands specific to this application
	xgi::bind(this,&LogReader::Default, "Default");
	xgi::bind(this,&LogReader::Default1, "Default1");
	xgi::bind(this,&LogReader::callDiagSystemPage, "callDiagSystemPage");
	xgi::bind(this,&LogReader::callFsmPage, "callFsmPage");
	xgi::bind(this,&LogReader::callVisualizeLogsPage, "callVisualizeLogsPage");
	xgi::bind(this,&LogReader::callVisualizePreviousLogsPage, "callVisualizePreviousLogsPage");
	xgi::bind(this,&LogReader::callVisualizeNextLogsPage, "callVisualizeNextLogsPage");
	xgi::bind(this,&LogReader::closeFile, "closeFile");

	xgi::bind(this,&LogReader::getIFrame, "getIFrame");
	xgi::bind(this,&LogReader::configureStateMachine, "configureStateMachine");
	xgi::bind(this,&LogReader::stopStateMachine, "stopStateMachine");

	xgi::bind(this,&LogReader::sortMessages, "sortMessages");
	xgi::bind(this,&LogReader::sortOrigin, "sortOrigin");
	xgi::bind(this,&LogReader::sortTimestamp, "sortTimestamp");
	xgi::bind(this,&LogReader::sortLevel, "sortLevel");
	xgi::bind(this,&LogReader::sortErrcode, "sortErrcode");

	xgi::bind(this,&LogReader::autoRefresh, "autoRefresh");

	xgi::bind(this,&LogReader::callAppsSpyingPanelPage, "callAppsSpyingPanelPage");
	xgi::bind(this,&LogReader::seeAppsDiagParamsButton, "seeAppsDiagParamsButton");
	xgi::bind(this,&LogReader::acceptAppLevelsButton, "acceptAppLevelsButton");
	xgi::bind(this,&LogReader::cancelAppLevelsButton, "cancelAppLevelsButton");


	xgi::bind(this,&LogReader::callGroupSpyingPanelPage, "callGroupSpyingPanelPage");
	xgi::bind(this,&LogReader::seeGroupDiagParamsButton, "seeGroupDiagParamsButton");
	xgi::bind(this,&LogReader::acceptGroupLevelsButton, "acceptGroupLevelsButton");
	xgi::bind(this,&LogReader::cancelGroupLevelsButton, "cancelGroupLevelsButton");

	xgi::bind(this,&LogReader::callWorldSpyingPanelPage, "callWorldSpyingPanelPage");
	xgi::bind(this,&LogReader::seeWorldDiagParamsButton, "seeWorldDiagParamsButton");
	xgi::bind(this,&LogReader::acceptWorldLevelsButton, "acceptWorldLevelsButton");
	xgi::bind(this,&LogReader::cancelWorldLevelsButton, "cancelWorldLevelsButton");


	#ifdef DATABASE
		xgi::bind(this,&LogReader::callLogsSummaryPage, "callLogsSummaryPage");
		xgi::bind(this,&LogReader::callLogsNoDbPage, "callLogsNoDbPage");

		xgi::bind(this,&LogReader::simulateFecError, "simulateFecError");
		xgi::bind(this,&LogReader::simulateFedError, "simulateFedError");
		xgi::bind(this,&LogReader::simulatePsuError, "simulatePsuError");


		xgi::bind(this,&LogReader::switchDetectorView, "switchDetectorView");

		xgi::bind(this,&LogReader::displayDetIdErrors, "displayDetIdErrors");
		xgi::bind(this,&LogReader::displayFecErrors, "displayFecErrors");

		xgi::bind(this,&LogReader::switchAllFecsView, "switchAllFecsView");
		xgi::bind(this,&LogReader::switchAllFedsView, "switchAllFedsView");
		xgi::bind(this,&LogReader::switchAllPsusView, "switchAllPsusView");

		xgi::bind(this,&LogReader::switchFecCratesView, "switchFecCratesView");
		xgi::bind(this,&LogReader::switchFecSlotsView, "switchFecSlotsView");
		xgi::bind(this,&LogReader::switchFecRingsView, "switchFecRingsView");
		xgi::bind(this,&LogReader::switchFecCcusView, "switchFecCcusView");
		xgi::bind(this,&LogReader::switchFecChannelsView, "switchFecChannelsView");

		xgi::bind(this,&LogReader::switchFedCratesView, "switchFedCratesView");
		xgi::bind(this,&LogReader::switchFedSlotsView, "switchFedSlotsView");

		xgi::bind(this,&LogReader::switchHiddenItems, "switchHiddenItems");
	#endif

	//Bind diagnostic applications
	xgi::bind(this,&LogReader::configureDiagSystem, "configureDiagSystem");
	xgi::bind(this,&LogReader::applyConfigureDiagSystem, "applyConfigureDiagSystem");

	// Call a macro needed for running a diagService in this application
	DIAG_DECLARE_LR_APP

	//Define FSM states
	fsm_.addState ('C', "Configured") ;
	fsm_.addState ('H', "Halted");
	fsm_.addStateTransition ('C','H', EXECTRANSCH, this, &LogReader::haltAction);
	fsm_.addStateTransition ('H','C', EXECTRANSHC, this, &LogReader::configureAction);
	fsm_.setFailedStateTransitionAction( this, &LogReader::failedTransition );

	//Initialize FSM state at load
	fsm_.setInitialState('H');
	fsm_.reset();

	// Import/Export the stateName variable
	getApplicationInfoSpace()->fireItemAvailable("stateName", &fsm_.stateName_);

	// Bind SOAP callbacks for FSM control messages 
	xoap::bind (this, &LogReader::fireEvent, EXECTRANSCH, XDAQ_NS_URI);
	xoap::bind (this, &LogReader::fireEvent, EXECTRANSHC, XDAQ_NS_URI);

	#ifdef AUTO_CONFIGURE_PROCESSES
		std::stringstream timerName;
		timerName << getApplicationDescriptor()->getContextDescriptor()->getURL() << ":";
		timerName << getApplicationDescriptor()->getClassName() << ":" << getApplicationDescriptor()->getLocalId() << ":" << getApplicationDescriptor()->getInstance();
		toolbox::task::Timer * timer = toolbox::task::getTimerFactory()->createTimer(timerName.str());
		toolbox::TimeInterval interval(AUTO_LR_CONFIGURE_DELAY,0);
		toolbox::TimeVal start;
		start = toolbox::TimeVal::gettimeofday() + interval;
		timer->schedule( this, start,  0, "" );
	#endif


	#ifdef DATABASE
	// Create the database access
	// retreive the connection of the database through an environmental variable called CONFDB
	std::string login="nil", passwd="nil", path="nil" ;
	DbAccess::getDbConfiguration (login, passwd, path) ;
	DbLogin_ = login;
	DbPasswd_ = passwd;
	DbPath_ = path;
	if (login == "nil" || passwd=="nil" || path=="nil")
	{
		std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path for LogReader" << std::endl ;
	}
	try
	{
	    tkDiagErrorAnalyser = new TkDiagErrorAnalyser ( partitionName ) ; //, login, passwd, path ) ;
		if (tkDiagErrorAnalyser->getConnectionNotIdentified() != 0) std::cerr << "Found " << tkDiagErrorAnalyser->getConnectionNotIdentified() << " connections not identified with DCU ID" << std::endl ;
		if (tkDiagErrorAnalyser->getPsuNotIdentified() != 0) std::cerr << "Found " << tkDiagErrorAnalyser->getPsuNotIdentified() << " PSU not identified with DCU hardware ID" << std::endl ;

		//Let's suppose that once this point reached the DB is usable
		if (tkDiagErrorAnalyser != NULL) isDbUsable = true;
	}
	catch (oracle::occi::SQLException &e)
	{
		std::cerr << "Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
		std::cerr << e.what() << std::endl ;
	}
	catch (FecExceptionHandler &e)
	{
		std::cerr << "Cannot create the access to DB: " << login << "/" << passwd << "@" << path << std::endl ;
		std::cerr << e.what() << std::endl ;
	}

	//Create the object that will handle the errors map
	errorsChecker = new ErrorsChecker ();

	//Create the object that will handle the errors tree
	errorsTreeBuilder = new ErrorsTreeBuilder ();

	//ErrorsTreeBuilder errorsTreeBuilder();
	errorsTreeBuilder->initArrays();
	#endif

}



LogReader::~LogReader()
{
	#ifdef DATABASE

		if (errorsChecker != NULL) delete errorsChecker;
		if (errorsTreeBuilder != NULL) delete errorsTreeBuilder;
	#endif
	//If logfile is still open, close it
	if (fileoutIsOpen_ == true)
	{
		if (fileOut_ != NULL) fclose(fileOut_);
	}

	#ifdef DATABASE
		// Delete the database access
		try
		{     
			if (tkDiagErrorAnalyser != NULL) delete tkDiagErrorAnalyser ;
		}  
		catch (oracle::occi::SQLException &e)
		{
			std::cerr << "Cannot close the access to the database" << std::endl ;
			std::cerr << e.what() << std::endl ;
		}
	#endif
}


// Returns the HTML page displayed from the Default1() method
void LogReader::getDefault1Content(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	*out << "<br>";
	*out << "Logs Reader Default page ; ENABLE/DISABLE logs auto-refresh here";
	*out << "<br>";
	*out << "<br>";

	*out << "Log Reader is reading from file : " << diagService_->logFileName_.c_str() << std::endl;
	*out << "<br>";

	// auto-refresh management section 
	std::string urlAutoRefresh = "/";
	urlAutoRefresh += getApplicationDescriptor()->getURN();
	urlAutoRefresh += "/autoRefresh";	
	*out << cgicc::form().set("method","post")
	.set("action", urlAutoRefresh)
	.set("enctype","multipart/form-data");

	*out << cgicc::label("Enter refresh rate in seconds : ") << std::endl;
	*out << "&nbsp;&nbsp;";
	*out << cgicc::input().set("type","text")
	.set("name","textRefreshRate")
	.set("size","10")
	.set("value",textRefreshRate_) << std::endl;
	*out << "<br><br>";
			
	if (autoRefreshEnabled_ == false)
	{
		*out << cgicc::input().set("type", "submit")
		.set("name", "Turn Auto-Refresh ON")
		.set("value", "Turn Auto-Refresh ON");
	}
	else
	{
		*out << cgicc::input().set("type", "submit")
		.set("name", "Turn Auto-Refresh OFF")
		.set("value", "Turn Auto-Refresh OFF");			
	}
	*out << "<br><br>";
	*out << cgicc::form();
}


// Returns the HTML page for FSM status
void LogReader::getFsmStatePage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	// Display current state
	if (fsm_.getCurrentState() == 'H') *out << "Current State is : Halted" << std::endl;
	if (fsm_.getCurrentState() == 'C') *out << "Current State is : Configured" << std::endl;
	if ( (fsm_.getCurrentState() != 'C') && (fsm_.getCurrentState() != 'H') ) *out << "Current State is : DANS LES CHOUX" << std::endl;
	*out << "<br>";
	*out << "<br>";

	// Fire the action CONFIGURE
	std::string urlConfig = "/";
	urlConfig += getApplicationDescriptor()->getURN();
	urlConfig += "/configureStateMachine";	
	*out << cgicc::form().set("method","post").set("action", urlConfig).set("enctype","multipart/form-data") << std::endl;
	*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Configure State Machine");
	*out << "<br>";
	*out << cgicc::form() << std::endl;
	*out << "<br>";

	// Fire the action STOP 
	std::string urlStop = "/";
	urlStop += getApplicationDescriptor()->getURN();
	urlStop += "/stopStateMachine";	
	*out << cgicc::form().set("method","post").set("action", urlStop).set("enctype","multipart/form-data") << std::endl;
	*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Stop State Machine");
	*out << "<br>";
	*out << cgicc::form() << std::endl;
	*out << "<br>";
}



// Returns the HTML page when loglist is requested
void LogReader::getVisualizeLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	if (internalState_ == 3) { readLogsFromFile(0);}
	else if (internalState_ == 4) { readLogsFromFile(-1);}
	else if (internalState_ == 5) { readLogsFromFile(1);}
	getIFrame(in, out);

}




//SINGLE APPLICATION PANNEL SECTION BEGIN
void LogReader::cancelAppLevelsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 7;
	mustDisplayOneAppDiagSettings_ = false;
	Default(in, out);
}



void LogReader::acceptAppLevelsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	//std::cout << "Entering acceptAppLevelsButton" << std::endl;
	char appName[255];
	char appInstance[100];
	char appUrl[512];
	sscanf(oneAppDiagSettingsToDisplay_.c_str(), "%s %s %s", appName, appInstance, appUrl);
	//std::cout << "For app=" << appName << " inst=" << appInstance << " url=" << appUrl << std::endl;

	cgicc::Cgicc cgi(in);
	std::string consoleLogLevel = cgi["consoleLogLevel"]->getValue() ;
	std::string glbEdLogLevel = cgi["glbEdLogLevel"]->getValue() ;
	std::string lclEdLogLevel = cgi["lclEdLogLevel"]->getValue() ;
	//std::cout << "SOAPSend cc=" << consoleLogLevel << " glb=" << glbEdLogLevel << " lcl=" << lclEdLogLevel << std::endl << std::endl;

	//std::cout << "IN LOGREADER : preparing for triggering callback" << std::endl;
	// Build SOAP for sending action
	xoap::MessageReference err_msg = xoap::createMessage();
	xoap::SOAPPart err_soap = err_msg->getSOAPPart();
	xoap::SOAPEnvelope err_envelope = err_soap.getEnvelope();
	xoap::SOAPName command = err_envelope.createName("processOnlineDiagRequest", "xdaq", XDAQ_NS_URI);
	xoap::SOAPBody err_body = err_envelope.getBody();
	err_body.addBodyElement(command);

	// Add requested attributes to SOAP part
	xoap::SOAPBody rb = err_msg->getSOAPPart().getEnvelope().getBody();
	std::vector<xoap::SOAPElement> logElement = rb.getChildElements ();
	xoap::SOAPName remoteCommand ("remoteCommand", "", "");
	logElement[0].addAttribute(remoteCommand,"2"); //cmd=2 means force the logging values given for all outputs

	xoap::SOAPName ccLogLevel ("ccLogLevel", "", "");
	logElement[0].addAttribute(ccLogLevel,consoleLogLevel);

	xoap::SOAPName lclLogLevel ("lclLogLevel", "", "");
	logElement[0].addAttribute(lclLogLevel,lclEdLogLevel);

	xoap::SOAPName glbLogLevel ("glbLogLevel", "", "");
	logElement[0].addAttribute(glbLogLevel,glbEdLogLevel);
	
	//std::cout << "IN LOGREADER : triggering callback now" << std::endl;
	xoap::MessageReference reply;
	try
	{
		xdaq::ApplicationDescriptor * err_d = getApplicationContext()->getDefaultZone()->getApplicationDescriptor(appName, atoi(appInstance));
		reply = getApplicationContext()->postSOAP(err_msg, *getApplicationDescriptor(), *err_d);
	}
	catch (xdaq::exception::Exception& e)
	{
		//std::cout << "IN LOGREADER : triggering callback FAILED" << std::endl;
	}
	//std::cout << "IN LOGREADER : triggering callback ended" << std::endl;

	internalState_ = 7;
	mustDisplayOneAppDiagSettings_ = false;
	Default(in, out);
}




void LogReader::seeAppsDiagParamsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	//std::cout << "Entering seeAppsDiagParamsButton" << std::endl;
	// Create a new Cgicc object containing all the CGI data
	//std::cout << "Reading CGI" << std::endl;
	cgicc::Cgicc cgi(in);

	std::string whichProcess_ = cgi["whichProcess"]->getValue() ;
	//std::cout << "CGI read gives back value : " << whichProcess_ << std::endl;

	//std::cout << "Calling again getAppsPanelPage through Default()" << std::endl;
	internalState_ = 7;

	oneAppDiagSettingsToDisplay_ = whichProcess_;
	mustDisplayOneAppDiagSettings_ = true;
	Default(in, out);
}




void LogReader::getAppsPanelPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	*out << "<hr style=\"width: 100%; height: 2px;\">" << std::endl;
	*out << "<span style=\"font-weight: bold;\">";
	*out << "Diagnostic System Single Application Centralized Control Panel" << std::endl;
	*out << "</span><br>" << std::endl;
	*out << "<hr style=\"width: 100%; height: 2px;\">" << std::endl;

	//Display the list of all candidate applications found
	if (mustDisplayOneAppDiagSettings_ == false)
	{
		std::set<xdaq::ApplicationDescriptor*> appsV;
		std::set<xdaq::ApplicationDescriptor*>::iterator appsVIterator;
		std::set<xdaq::ApplicationDescriptor*> tmpAppsV;
		std::set<xdaq::ApplicationDescriptor*>::iterator tmpAppsVIterator;

		std::string testUrl = "/";
		testUrl += getApplicationDescriptor()->getURN();
		testUrl += "/seeAppsDiagParamsButton";	

		*out << cgicc::form().set("method","post")
		.set("action", testUrl)
		.set("enctype","multipart/form-data") << std::endl;

		*out << cgicc::p() << cgicc::input().set("type", "submit")
		.set("name", "submit")
		.set("value", "See Process Parameters");
		*out << cgicc::p() << std::endl ;

		// Detect applications using DiagSystem 
		for (int ii = 0; appsUsingDiag[ii] != "ENDOFLIST"; ii++)
		{
			tmpAppsV = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(appsUsingDiag[ii]);
			for (tmpAppsVIterator=tmpAppsV.begin();tmpAppsVIterator!=tmpAppsV.end();tmpAppsVIterator++)
			{
				appsV.insert(dynamic_cast<xdaq::ApplicationDescriptor*> (*tmpAppsVIterator));
			}
		}

		bool gotDefault = false;
		// Display detected applications Informations
		for (appsVIterator=appsV.begin();appsVIterator!=appsV.end();appsVIterator++)
		{
			xdaq::ApplicationDescriptor* a = dynamic_cast<xdaq::ApplicationDescriptor*> (*appsVIterator);
			*out << "<br>Application <span style=\"font-weight: bold;\">" <<
					a->getClassName() << "</span>" <<
					"&nbsp;&nbsp;&nbsp;Instance <span style=\"font-weight: bold;\">" <<
					a->getInstance() << "</span>" <<
					"&nbsp;&nbsp;&nbsp;URL <span style=\"font-weight: bold;\">" <<
					a->getContextDescriptor()->getURL() << "</span>" <<
					"&nbsp;&nbsp;&nbsp;has been detected.<br>" << std::endl;

			std::ostringstream radioButtonValue;
			radioButtonValue << a->getClassName() << " " << a->getInstance() << " " << a->getContextDescriptor()->getURL();
			if (gotDefault == false)
			{
				*out << cgicc::input().set("type","radio")
				.set("name","whichProcess")
				.set("size","30")
				.set("value",radioButtonValue.str())
				.set("selected") 
				.set("checked","true") << std::endl;
				gotDefault = true;
			}
			else
			{
				*out << cgicc::input().set("type","radio")
				.set("name","whichProcess")
				.set("size","30")
				.set("value",radioButtonValue.str())
				.set("selected") << std::endl;
			}
			*out << "&nbsp;&nbsp;&nbsp;Select this process as diagnostic parameters modification target.<br>" << std::endl;
		}
		*out << "<br><br>";

		*out << cgicc::p() << cgicc::input().set("type", "submit")
		.set("name", "submit")
		.set("value", "See Process Parameters");
		*out << cgicc::p() << std::endl ;
		*out << cgicc::form() << std::endl;
	}
	else //Display specific informations about one application
	{
		// liste loglevels OU mode expert
		char appName[255];
		char appInstance[100];
		char appUrl[512];
		sscanf(oneAppDiagSettingsToDisplay_.c_str(), "%s %s %s", appName, appInstance, appUrl);
		*out << "Modifying log levels for application <span style=\"font-weight: bold;\">" <<
				appName << "</span>" <<
				"&nbsp;&nbsp;&nbsp;Instance <span style=\"font-weight: bold;\">" <<
				appInstance << "</span>" <<
				"&nbsp;&nbsp;&nbsp;URL <span style=\"font-weight: bold;\">" <<
				appUrl << "</span>" <<
				"<br>" << std::endl;

		std::string cancelLvlsUrl = "/";
		cancelLvlsUrl += getApplicationDescriptor()->getURN();
		cancelLvlsUrl += "/cancelAppLevelsButton";	
		*out << cgicc::form().set("method","post")
		.set("action", cancelLvlsUrl)
		.set("enctype","multipart/form-data") << std::endl;

		*out << cgicc::p() << cgicc::input().set("type", "submit")
		.set("name", "cancel")
		.set("value", "Cancel");
		*out << cgicc::form() << std::endl;

		std::string setLvlsUrl = "/";
		setLvlsUrl += getApplicationDescriptor()->getURN();
		setLvlsUrl += "/acceptAppLevelsButton";	
		*out << cgicc::form().set("method","post")
		.set("action", setLvlsUrl)
		.set("enctype","multipart/form-data") << std::endl;

		*out << cgicc::p() << cgicc::input().set("type", "submit")
		.set("name", "submit")
		.set("value", "Apply Modifications");
		*out << cgicc::p() << std::endl ;

		std::string consoleLogLevel_;
		std::string glbEdLogLevel_;
		std::string lclEdLogLevel_;

		//std::cout << "IN LOGREADER : preparing for triggering callback" << std::endl;
		// Build SOAP for sending action 
		xoap::MessageReference err_msg = xoap::createMessage();
		xoap::SOAPPart err_soap = err_msg->getSOAPPart();
		xoap::SOAPEnvelope err_envelope = err_soap.getEnvelope();
		xoap::SOAPName command = err_envelope.createName("processOnlineDiagRequest", "xdaq", XDAQ_NS_URI);
		xoap::SOAPBody err_body = err_envelope.getBody();
		err_body.addBodyElement(command);

		// Add requested attributes to SOAP part
		xoap::SOAPBody rb = err_msg->getSOAPPart().getEnvelope().getBody();
		std::vector<xoap::SOAPElement> logElement = rb.getChildElements ();
		xoap::SOAPName remoteCommand ("remoteCommand", "", "");
		logElement[0].addAttribute(remoteCommand,"1"); //cmd=1 means send me back the logging values for all outputs
		//std::cout << "IN LOGREADER : triggering callback now" << std::endl;

		xoap::MessageReference reply;
		try
		{
			xdaq::ApplicationDescriptor * err_d = getApplicationContext()->getDefaultZone()->getApplicationDescriptor(appName, atoi(appInstance));
            reply = getApplicationContext()->postSOAP(err_msg, *getApplicationDescriptor(), *err_d);
		}
		catch (xdaq::exception::Exception& e)
		{
			//std::cout << "IN LOGREADER : triggering callback FAILED" << std::endl;
		}
		//std::cout << "IN LOGREADER : triggering callback ended" << std::endl;

		// Analyse callback reply */
		xoap::SOAPBody mb = reply->getSOAPPart().getEnvelope().getBody();
		std::vector<xoap::SOAPElement> paramsList = mb.getChildElements ();
		if(paramsList.size()==1)
		{
			xoap::SOAPName ccLogLevel ("ccLogLevel", "", "");
			consoleLogLevel_ = paramsList[0].getAttributeValue(ccLogLevel);

			xoap::SOAPName lclLogLevel ("lclLogLevel", "", "");
			lclEdLogLevel_ = paramsList[0].getAttributeValue(lclLogLevel);

			xoap::SOAPName glbLogLevel ("glbLogLevel", "", "");
			glbEdLogLevel_ = paramsList[0].getAttributeValue(glbLogLevel);
		}


		// Display current loglevels
		*out << cgicc::label("Set the log level for CONSOLE output :") << std::endl;
		*out << "&nbsp;<select name=\"consoleLogLevel\">";
		if (consoleLogLevel_ != DIAGTRACE) *out << "<option>" << DIAGTRACE << "</option>"; else *out << "<option selected>" << DIAGTRACE << "</option>";
		if (consoleLogLevel_ != DIAGDEBUG) *out << "<option>" << DIAGDEBUG << "</option>"; else *out << "<option selected>" << DIAGDEBUG << "</option>";
	    if (consoleLogLevel_ != DIAGINFO) *out << "<option>" << DIAGINFO << "</option>"; else *out << "<option selected>" << DIAGINFO << "</option>";
	    if (consoleLogLevel_ != DIAGWARN) *out << "<option>" << DIAGWARN << "</option>"; else *out << "<option selected>" << DIAGWARN << "</option>";
	    if (consoleLogLevel_ != DIAGUSERINFO) *out << "<option>" << DIAGUSERINFO << "</option>"; else *out << "<option selected>" << DIAGUSERINFO << "</option>";
	    if (consoleLogLevel_ != DIAGERROR) *out << "<option>" << DIAGERROR << "</option>"; else *out << "<option selected>" << DIAGERROR << "</option>";
	    if (consoleLogLevel_ != DIAGFATAL) *out << "<option>" << DIAGFATAL << "</option>"; else *out << "<option selected>" << DIAGFATAL << "</option>";
	    if (consoleLogLevel_ != DIAGOFF) *out << "<option>" << DIAGOFF << "</option>"; else *out << "<option selected>" << DIAGOFF << "</option>";
	    *out << "</select>"<< std::endl;
		*out << "<br><br>";

	    *out << cgicc::label("Set the log level for LOCAL ERROR DISPATCHER output :") << std::endl;
	    *out << "&nbsp;<select name=\"lclEdLogLevel\">";
	    if (lclEdLogLevel_ != DIAGTRACE) *out << "<option>" << DIAGTRACE << "</option>"; else *out << "<option selected>" << DIAGTRACE << "</option>";
	    if (lclEdLogLevel_ != DIAGDEBUG) *out << "<option>" << DIAGDEBUG << "</option>"; else *out << "<option selected>" << DIAGDEBUG << "</option>";
	    if (lclEdLogLevel_ != DIAGINFO) *out << "<option>" << DIAGINFO << "</option>"; else *out << "<option selected>" << DIAGINFO << "</option>";
	    if (lclEdLogLevel_ != DIAGWARN) *out << "<option>" << DIAGWARN << "</option>"; else *out << "<option selected>" << DIAGWARN << "</option>";
	    if (lclEdLogLevel_ != DIAGUSERINFO) *out << "<option>" << DIAGUSERINFO << "</option>"; else *out << "<option selected>" << DIAGUSERINFO << "</option>";
	    if (lclEdLogLevel_ != DIAGERROR) *out << "<option>" << DIAGERROR << "</option>"; else *out << "<option selected>" << DIAGERROR << "</option>";
	    if (lclEdLogLevel_ != DIAGFATAL) *out << "<option>" << DIAGFATAL << "</option>"; else *out << "<option selected>" << DIAGFATAL << "</option>";
	    if (lclEdLogLevel_ != DIAGOFF) *out << "<option>" << DIAGOFF << "</option>"; else *out << "<option selected>" << DIAGOFF << "</option>";
	    *out << "</select>"<< std::endl;
		*out << "<br><br>";

	    *out << cgicc::label("Set the log level for GLOBAL ERROR DISPATCHER output :") << std::endl;
	    *out << "&nbsp;<select name=\"glbEdLogLevel\">";
   		if (glbEdLogLevel_ != DIAGTRACE) *out << "<option>" << DIAGTRACE << "</option>"; else *out << "<option selected>" << DIAGTRACE << "</option>";
	    if (glbEdLogLevel_ != DIAGDEBUG) *out << "<option>" << DIAGDEBUG << "</option>"; else *out << "<option selected>" << DIAGDEBUG << "</option>";
	    if (glbEdLogLevel_ != DIAGINFO) *out << "<option>" << DIAGINFO << "</option>"; else *out << "<option selected>" << DIAGINFO << "</option>";
	    if (glbEdLogLevel_ != DIAGWARN) *out << "<option>" << DIAGWARN << "</option>"; else *out << "<option selected>" << DIAGWARN << "</option>";
	    if (glbEdLogLevel_ != DIAGUSERINFO) *out << "<option>" << DIAGUSERINFO << "</option>"; else *out << "<option selected>" << DIAGUSERINFO << "</option>";
	    if (glbEdLogLevel_ != DIAGERROR) *out << "<option>" << DIAGERROR << "</option>"; else *out << "<option selected>" << DIAGERROR << "</option>";
	    if (glbEdLogLevel_ != DIAGFATAL) *out << "<option>" << DIAGFATAL << "</option>"; else *out << "<option selected>" << DIAGFATAL << "</option>";
	    if (glbEdLogLevel_ != DIAGOFF) *out << "<option>" << DIAGOFF << "</option>"; else *out << "<option selected>" << DIAGOFF << "</option>";
	    *out << "</select>"<< std::endl;
		*out << "<br>";

	    *out << cgicc::p() << cgicc::input().set("type", "submit")
		    .set("name", "submit")
		    .set("value", "Apply Modifications");
	    *out << cgicc::p() << std::endl ;
	    *out << cgicc::form() << std::endl;

	    *out << cgicc::form().set("method","post")
		    .set("action", cancelLvlsUrl)
		    .set("enctype","multipart/form-data") << std::endl;
	    *out << cgicc::p() << cgicc::input().set("type", "submit")
	    	.set("name", "cancel")
		    .set("value", "Cancel");
		*out << cgicc::form() << std::endl;
	}
}
//SINGLE APPLICATION PANNEL SECTION END




//GROUP APPLICATIONS PANNEL SECTION BEGIN
void LogReader::cancelGroupLevelsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 8;
	mustDisplayGroupAppDiagSettings_ = false;
	Default(in, out);
}



void LogReader::acceptGroupLevelsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	//std::cout << "Entering acceptGroupLevelsButton" << std::endl;
	cgicc::Cgicc cgi(in);
	std::string consoleLogLevel = cgi["consoleLogLevel"]->getValue() ;
	std::string glbEdLogLevel = cgi["glbEdLogLevel"]->getValue() ;
	std::string lclEdLogLevel = cgi["lclEdLogLevel"]->getValue() ;
	//std::cout << "SOAPSend cc=" << consoleLogLevel << " lcl=" << lclEdLogLevel << " glb=" << glbEdLogLevel << std::endl << std::endl;

	//Finding list of applications to target
	std::set<xdaq::ApplicationDescriptor*> appsV;
	std::set<xdaq::ApplicationDescriptor*> tmpAppsV;
	std::set<xdaq::ApplicationDescriptor*>::iterator appsVIterator;

	// Detect applications using DiagSystem and aggregate applications per group
	tmpAppsV = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(groupAppDiagSettingsToDisplay_);
	for (appsVIterator=tmpAppsV.begin();appsVIterator!=tmpAppsV.end();appsVIterator++)
	{
		appsV.insert(dynamic_cast<xdaq::ApplicationDescriptor*> (*appsVIterator));
	}

	//Make a configuration loop over the detected applications
	for (appsVIterator=appsV.begin();appsVIterator!=appsV.end();appsVIterator++)
	{
		xdaq::ApplicationDescriptor* a = dynamic_cast<xdaq::ApplicationDescriptor*> (*appsVIterator);

		//std::cout << "IN LOGREADER : preparing for triggering callback" << std::endl;
		// Build SOAP for sending action
		xoap::MessageReference err_msg = xoap::createMessage();
		xoap::SOAPPart err_soap = err_msg->getSOAPPart();
		xoap::SOAPEnvelope err_envelope = err_soap.getEnvelope();
		xoap::SOAPName command = err_envelope.createName("processOnlineDiagRequest", "xdaq", XDAQ_NS_URI);
		xoap::SOAPBody err_body = err_envelope.getBody();
		err_body.addBodyElement(command);

		// Add requested attributes to SOAP part
		xoap::SOAPBody rb = err_msg->getSOAPPart().getEnvelope().getBody();
		std::vector<xoap::SOAPElement> logElement = rb.getChildElements ();
		xoap::SOAPName remoteCommand ("remoteCommand", "", "");
		logElement[0].addAttribute(remoteCommand,"2"); //cmd=2 means force the logging values given for all outputs

		xoap::SOAPName ccLogLevel ("ccLogLevel", "", "");
		logElement[0].addAttribute(ccLogLevel,consoleLogLevel);

		xoap::SOAPName lclLogLevel ("lclLogLevel", "", "");
		logElement[0].addAttribute(lclLogLevel,lclEdLogLevel);

		xoap::SOAPName glbLogLevel ("glbLogLevel", "", "");
		logElement[0].addAttribute(glbLogLevel,glbEdLogLevel);

		//std::cout << "IN LOGREADER : triggering callback now" << std::endl;
		xoap::MessageReference reply;
		try
		{
			reply = getApplicationContext()->postSOAP(err_msg, *getApplicationDescriptor(), *a);
		}
		catch (xdaq::exception::Exception& e)
		{
			//std::cout << "IN LOGREADER : triggering callback FAILED" << std::endl;
		}
		//std::cout << "IN LOGREADER : triggering callback ended" << std::endl;
		//std::cout << "Exiting acceptGroupLevelsButton" << std::endl;
	}
	internalState_ = 8;
	mustDisplayGroupAppDiagSettings_ = false;
	Default(in, out);
}



void LogReader::seeGroupDiagParamsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	//std::cout << "Entering seeGroupDiagParamsButton" << std::endl;
	internalState_ = 8;

	// Create a new Cgicc object containing all the CGI data
	//std::cout << "Reading CGI" << std::endl;
	cgicc::Cgicc cgi(in);

	std::string whichGroup_ = cgi["whichGroup"]->getValue() ;
	//std::cout << "CGI read gives back value : " << whichGroup_ << std::endl;

	//std::cout << "Calling again getGroupsPanelPage through Default()" << std::endl;
	internalState_ = 8;

	groupAppDiagSettingsToDisplay_ = whichGroup_;
	mustDisplayGroupAppDiagSettings_ = true;
	Default(in, out);
}




void LogReader::getGroupsPanelPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	*out << "<hr style=\"width: 100%; height: 2px;\">" << std::endl;
	*out << "<span style=\"font-weight: bold;\">";
	*out << "Diagnostic System Grouped Applications Centralized Control Panel" << std::endl;
	*out << "</span><br>" << std::endl;
	*out << "<hr style=\"width: 100%; height: 2px;\">" << std::endl;

	if (mustDisplayGroupAppDiagSettings_ == false)
	{
		std::set<xdaq::ApplicationDescriptor*> appsV;
		std::set<xdaq::ApplicationDescriptor*> tmpAppsV;
		std::set<xdaq::ApplicationDescriptor*>::iterator tmpAppsVIterator;

		std::string testUrl = "/";
		testUrl += getApplicationDescriptor()->getURN();
		testUrl += "/seeGroupDiagParamsButton";	

		*out << cgicc::form().set("method","post")
			.set("action", testUrl)
			.set("enctype","multipart/form-data") << std::endl;

		*out << cgicc::p() << cgicc::input().set("type", "submit")
			.set("name", "submit")
			.set("value", "Set Group Parameters");
		*out << cgicc::p() << std::endl ;

		for (int jj=0; jj<NUMBER_OF_APPS_LISTED; jj++)
		{
			appsUsingDiagDetected[jj] = 0;
		}

		// Detect applications using DiagSystem package and aggregate applications per group
		for (int ii = 0; appsUsingDiag[ii] != "ENDOFLIST"; ii++)
		{
			tmpAppsV = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(appsUsingDiag[ii]);
			for (tmpAppsVIterator=tmpAppsV.begin();tmpAppsVIterator!=tmpAppsV.end();tmpAppsVIterator++)
			{
				appsV.insert(dynamic_cast<xdaq::ApplicationDescriptor*> (*tmpAppsVIterator));
				(appsUsingDiagDetected[ii])++;
			}
		}

		bool gotDefault = false;
		// Display detected applications groups Informations
		for (int kk=0 ; kk<NUMBER_OF_APPS_LISTED; kk++)
		{
			if (appsUsingDiagDetected[kk] != 0)
			{
				*out << "<br>A group of " << appsUsingDiagDetected[kk] << " applications <span style=\"font-weight: bold;\">" <<
						appsUsingDiag[kk] << "</span>" <<
						"&nbsp;&nbsp;&nbsp;has been detected.<br>" << std::endl;

				if (gotDefault == false)
				{
					*out << cgicc::input().set("type","radio")
						.set("name","whichGroup")
						.set("size","30")
						.set("value",appsUsingDiag[kk])
						.set("selected") 
						.set("checked","true") << std::endl;
					gotDefault = true;
				}
				else
				{
					*out << cgicc::input().set("type","radio")
						.set("name","whichGroup")
						.set("size","30")
						.set("value",appsUsingDiag[kk])
						.set("selected") << std::endl;
				}
				*out << "&nbsp;&nbsp;&nbsp;Select this group of processes as a general diagnostic parameters modification target.<br>" << std::endl;
			}
		}
		*out << "<br><br>";

		*out << cgicc::p() << cgicc::input().set("type", "submit")
			.set("name", "submit")
			.set("value", "Set Group Parameters");
		*out << cgicc::p() << std::endl ;
		*out << cgicc::form() << std::endl;
	}
	else
	{
		// liste loglevels OU mode expert
		*out << "Modifying log levels for applications group : <span style=\"font-weight: bold;\">" <<
				groupAppDiagSettingsToDisplay_ << "</span><br>" << std::endl;

		std::string cancelLvlsUrl = "/";
		cancelLvlsUrl += getApplicationDescriptor()->getURN();
		cancelLvlsUrl += "/cancelGroupLevelsButton";	
		*out << cgicc::form().set("method","post")
			.set("action", cancelLvlsUrl)
			.set("enctype","multipart/form-data") << std::endl;

		*out << cgicc::p() << cgicc::input().set("type", "submit")
			.set("name", "cancel")
			.set("value", "Cancel");
		*out << cgicc::form() << std::endl;

		std::string setLvlsUrl = "/";
		setLvlsUrl += getApplicationDescriptor()->getURN();
		setLvlsUrl += "/acceptGroupLevelsButton";	
		*out << cgicc::form().set("method","post")
			.set("action", setLvlsUrl)
			.set("enctype","multipart/form-data") << std::endl;

		*out << cgicc::p() << cgicc::input().set("type", "submit")
			.set("name", "submit")
			.set("value", "Apply Modifications");
		*out << cgicc::p() << std::endl ;

		std::string consoleLogLevel_;
		std::string glbEdLogLevel_;
		std::string lclEdLogLevel_;

		// Display current loglevels
		*out << cgicc::label("Set the log level for CONSOLE output :") << std::endl;
		*out << "&nbsp;<select name=\"consoleLogLevel\">";
		*out << "<option>" << DIAGTRACE << "</option>";
		*out << "<option>" << DIAGDEBUG << "</option>";
	    *out << "<option>" << DIAGINFO << "</option>";
	    *out << "<option>" << DIAGWARN << "</option>";
	    *out << "<option>" << DIAGUSERINFO << "</option>";
	    *out << "<option>" << DIAGERROR << "</option>";
	    *out << "<option>" << DIAGFATAL << "</option>";
	    *out << "<option>" << DIAGOFF << "</option>";
		*out << "<option selected>" << DIAGUNCHANGED << "</option>";
	    *out << "</select>"<< std::endl;
		*out << "<br><br>";

	    *out << cgicc::label("Set the log level for LOCAL ERROR DISPATCHER output :") << std::endl;
	    *out << "&nbsp;<select name=\"lclEdLogLevel\">";
	    *out << "<option>" << DIAGTRACE << "</option>";
	    *out << "<option>" << DIAGDEBUG << "</option>";
	    *out << "<option>" << DIAGINFO << "</option>";
	    *out << "<option>" << DIAGWARN << "</option>";
	    *out << "<option>" << DIAGUSERINFO << "</option>";
	    *out << "<option>" << DIAGERROR << "</option>";
	    *out << "<option>" << DIAGFATAL << "</option>";
	    *out << "<option>" << DIAGOFF << "</option>";
		*out << "<option selected>" << DIAGUNCHANGED << "</option>";
	    *out << "</select>"<< std::endl;
		*out << "<br><br>";

	    *out << cgicc::label("Set the log level for GLOBAL ERROR DISPATCHER output :") << std::endl;
	    *out << "&nbsp;<select name=\"glbEdLogLevel\">";
   		*out << "<option>" << DIAGTRACE << "</option>";
	    *out << "<option>" << DIAGDEBUG << "</option>";
	    *out << "<option>" << DIAGINFO << "</option>";
	    *out << "<option>" << DIAGWARN << "</option>";
	    *out << "<option>" << DIAGUSERINFO << "</option>";
	    *out << "<option>" << DIAGERROR << "</option>";
	    *out << "<option>" << DIAGFATAL << "</option>";
	    *out << "<option>" << DIAGOFF << "</option>";
		*out << "<option selected>" << DIAGUNCHANGED << "</option>";
	    *out << "</select>"<< std::endl;
		*out << "<br>";


	    *out << cgicc::p() << cgicc::input().set("type", "submit")
		    .set("name", "submit")
		    .set("value", "Apply Modifications");
	    *out << cgicc::p() << std::endl ;
	    *out << cgicc::form() << std::endl;

	    *out << cgicc::form().set("method","post")
		    .set("action", cancelLvlsUrl)
		    .set("enctype","multipart/form-data") << std::endl;
	    *out << cgicc::p() << cgicc::input().set("type", "submit")
		    .set("name", "cancel")
		    .set("value", "Cancel");
	    *out << cgicc::form() << std::endl;
	}
}
//GROUP APPLICATIONS PANNEL SECTION END






//WORLD APPLICATIONS PANNEL SECTION BEGIN
void LogReader::cancelWorldLevelsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 9;
	mustDisplayWorldAppDiagSettings_ = false;
	Default(in, out);
}


void LogReader::acceptWorldLevelsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	//std::cout << "Entering acceptWorldLevelsButton" << std::endl;
	cgicc::Cgicc cgi(in);
	std::string consoleLogLevel = cgi["consoleLogLevel"]->getValue() ;
	std::string glbEdLogLevel = cgi["glbEdLogLevel"]->getValue() ;
	std::string lclEdLogLevel = cgi["lclEdLogLevel"]->getValue() ;
	//std::cout << "SOAPSend cc=" << consoleLogLevel << " lcl=" << lclEdLogLevel << " glb=" << glbEdLogLevel << std::endl << std::endl;

	//Finding list of applications to target
	std::set<xdaq::ApplicationDescriptor*> appsV;
	std::set<xdaq::ApplicationDescriptor*> tmpAppsV;
	std::set<xdaq::ApplicationDescriptor*>::iterator appsVIterator;
	//int numberOfTargetFound=0;

	// Detect applications using DiagSystem and aggregate applications per group
	for (int ii = 0; appsUsingDiag[ii] != "ENDOFLIST"; ii++)
	{
		tmpAppsV = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(appsUsingDiag[ii]);
		for (appsVIterator=tmpAppsV.begin();appsVIterator!=tmpAppsV.end();appsVIterator++)
		{
			appsV.insert(dynamic_cast<xdaq::ApplicationDescriptor*> (*appsVIterator));
		}
	}

	//Make a configuration loop over the detected applications
	for (appsVIterator=appsV.begin();appsVIterator!=appsV.end();appsVIterator++)
	{
		xdaq::ApplicationDescriptor* a = dynamic_cast<xdaq::ApplicationDescriptor*> (*appsVIterator);

		//std::cout << "IN LOGREADER : preparing for triggering callback" << std::endl;
		// Build SOAP for sending action
		xoap::MessageReference err_msg = xoap::createMessage();
		xoap::SOAPPart err_soap = err_msg->getSOAPPart();
		xoap::SOAPEnvelope err_envelope = err_soap.getEnvelope();
		xoap::SOAPName command = err_envelope.createName("processOnlineDiagRequest", "xdaq", XDAQ_NS_URI);
		xoap::SOAPBody err_body = err_envelope.getBody();
		err_body.addBodyElement(command);

		// Add requested attributes to SOAP part
		xoap::SOAPBody rb = err_msg->getSOAPPart().getEnvelope().getBody();
		std::vector<xoap::SOAPElement> logElement = rb.getChildElements ();
		xoap::SOAPName remoteCommand ("remoteCommand", "", "");
		logElement[0].addAttribute(remoteCommand,"2"); //cmd=2 means force the logging values given for all outputs

		xoap::SOAPName ccLogLevel ("ccLogLevel", "", "");
		logElement[0].addAttribute(ccLogLevel,consoleLogLevel);

		xoap::SOAPName lclLogLevel ("lclLogLevel", "", "");
		logElement[0].addAttribute(lclLogLevel,lclEdLogLevel);

		xoap::SOAPName glbLogLevel ("glbLogLevel", "", "");
		logElement[0].addAttribute(glbLogLevel,glbEdLogLevel);

		//std::cout << "IN LOGREADER : triggering callback now" << std::endl;
		xoap::MessageReference reply;
		try
		{
			reply = getApplicationContext()->postSOAP(err_msg, *getApplicationDescriptor(), *a);
		}
		catch (xdaq::exception::Exception& e)
		{
			//std::cout << "IN LOGREADER : triggering callback FAILED" << std::endl;
		}
		//std::cout << "IN LOGREADER : triggering callback ended" << std::endl;
	}
	//std::cout << "Exiting acceptWorldLevelsButton" << std::endl;
	internalState_ = 9;
	mustDisplayWorldAppDiagSettings_ = false;
	Default(in, out);
}




void LogReader::seeWorldDiagParamsButton(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	//std::cout << "Entering seeWorldDiagParamsButton" << std::endl;
	internalState_ = 9;

	// Create a new Cgicc object containing all the CGI data
	//std::cout << "Reading CGI" << std::endl;
	cgicc::Cgicc cgi(in);

	std::string whichWorld_ = cgi["whichWorld"]->getValue() ;
	//std::cout << "CGI read gives back value : " << whichWorld_ << std::endl;

	//std::cout << "Calling again getWorldPanelPage through Default()" << std::endl;
	internalState_ = 9;

	worldAppDiagSettingsToDisplay_ = whichWorld_;
	mustDisplayWorldAppDiagSettings_ = true;
	Default(in, out);
}




void LogReader::getWorldPanelPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	*out << "<hr style=\"width: 100%; height: 2px;\">" << std::endl;
	*out << "<span style=\"font-weight: bold;\">";
	*out << "Diagnostic System WorldWide Applications Centralized Control Panel" << std::endl;
	*out << "</span><br>" << std::endl;
	*out << "<hr style=\"width: 100%; height: 2px;\">" << std::endl;

	if (mustDisplayWorldAppDiagSettings_ == false)
	{
		std::set<xdaq::ApplicationDescriptor*> appsV;
		std::set<xdaq::ApplicationDescriptor*> tmpAppsV;
		std::set<xdaq::ApplicationDescriptor*>::iterator tmpAppsVIterator;

		std::string testUrl = "/";
		testUrl += getApplicationDescriptor()->getURN();
		testUrl += "/seeWorldDiagParamsButton";	

		*out << cgicc::form().set("method","post")
			.set("action", testUrl)
			.set("enctype","multipart/form-data") << std::endl;

		*out << cgicc::p() << cgicc::input().set("type", "submit")
			.set("name", "submit")
			.set("value", "Set WorldWide applications Parameters");
		*out << cgicc::p() << std::endl ;

		// Detect applications using DiagSystem package and aggregate applications per group
		int appsDetected = 0;
		for (int ii = 0; appsUsingDiag[ii] != "ENDOFLIST"; ii++)
		{
			tmpAppsV = getApplicationContext()->getDefaultZone()->getApplicationDescriptors(appsUsingDiag[ii]);
			for (tmpAppsVIterator=tmpAppsV.begin();tmpAppsVIterator!=tmpAppsV.end();tmpAppsVIterator++)
			{
				appsV.insert(dynamic_cast<xdaq::ApplicationDescriptor*> (*tmpAppsVIterator));
				appsDetected++;
			}
		}


		bool gotDefault = false;
		// Display detected applications groups Informations
		if (appsDetected != 0)
		{
			*out << "<br>I have detected " << appsDetected << " applications <br>" << std::endl;
			if (gotDefault == false)
			{
				*out << cgicc::input().set("type","radio")
					.set("name","whichWorld")
					.set("size","30")
					.set("value","WorldWide")
					.set("selected") 
					.set("checked","true") << std::endl;
				gotDefault = true;
			}
			else
			{
				*out << cgicc::input().set("type","radio")
					.set("name","whichGroup")
					.set("size","30")
					.set("value","WorldWide")
					.set("selected") << std::endl;
			}
			*out << "&nbsp;&nbsp;&nbsp;Select this button for a worldwide diagnostic parameters modification.<br>" << std::endl;
		}
		*out << "<br><br>";

		*out << cgicc::p() << cgicc::input().set("type", "submit")
			.set("name", "submit")
			.set("value", "Set WorldWide Parameters");
		*out << cgicc::p() << std::endl ;
		*out << cgicc::form() << std::endl;
	}
	else
	{
		// liste loglevels OU mode expert
		*out << "Modifying log levels for worldwide applications : <span style=\"font-weight: bold;\">" <<
				groupAppDiagSettingsToDisplay_ << "</span><br>" << std::endl;

		std::string cancelLvlsUrl = "/";
		cancelLvlsUrl += getApplicationDescriptor()->getURN();
		cancelLvlsUrl += "/cancelWorldLevelsButton";	
		*out << cgicc::form().set("method","post")
			.set("action", cancelLvlsUrl)
			.set("enctype","multipart/form-data") << std::endl;

		*out << cgicc::p() << cgicc::input().set("type", "submit")
			.set("name", "cancel")
			.set("value", "Cancel");
		*out << cgicc::form() << std::endl;


		std::string setLvlsUrl = "/";
		setLvlsUrl += getApplicationDescriptor()->getURN();
		setLvlsUrl += "/acceptWorldLevelsButton";	
		*out << cgicc::form().set("method","post")
			.set("action", setLvlsUrl)
			.set("enctype","multipart/form-data") << std::endl;

		*out << cgicc::p() << cgicc::input().set("type", "submit")
			.set("name", "submit")
			.set("value", "Apply Modifications");
		*out << cgicc::p() << std::endl ;

		std::string consoleLogLevel_;
		std::string glbEdLogLevel_;
		std::string lclEdLogLevel_;

		*out << cgicc::label("Set the log level for CONSOLE output :") << std::endl;
		*out << "&nbsp;<select name=\"consoleLogLevel\">";
		*out << "<option>" << DIAGTRACE << "</option>";
		*out << "<option>" << DIAGDEBUG << "</option>";
	    *out << "<option>" << DIAGINFO << "</option>";
	    *out << "<option>" << DIAGWARN << "</option>";
	    *out << "<option>" << DIAGUSERINFO << "</option>";
	    *out << "<option>" << DIAGERROR << "</option>";
	    *out << "<option>" << DIAGFATAL << "</option>";
	    *out << "<option>" << DIAGOFF << "</option>";
		*out << "<option selected>" << DIAGUNCHANGED << "</option>";
	    *out << "</select>"<< std::endl;
		*out << "<br><br>";

	    *out << cgicc::label("Set the log level for LOCAL ERROR DISPATCHER output :") << std::endl;
	    *out << "&nbsp;<select name=\"lclEdLogLevel\">";
	    *out << "<option>" << DIAGTRACE << "</option>";
	    *out << "<option>" << DIAGDEBUG << "</option>";
	    *out << "<option>" << DIAGINFO << "</option>";
	    *out << "<option>" << DIAGWARN << "</option>";
	    *out << "<option>" << DIAGUSERINFO << "</option>";
	    *out << "<option>" << DIAGERROR << "</option>";
	    *out << "<option>" << DIAGFATAL << "</option>";
	    *out << "<option>" << DIAGOFF << "</option>";
		*out << "<option selected>" << DIAGUNCHANGED << "</option>";
	    *out << "</select>"<< std::endl;
		*out << "<br><br>";

	    *out << cgicc::label("Set the log level for GLOBAL ERROR DISPATCHER output :") << std::endl;
	    *out << "&nbsp;<select name=\"glbEdLogLevel\">";
   		*out << "<option>" << DIAGTRACE << "</option>";
	    *out << "<option>" << DIAGDEBUG << "</option>";
	    *out << "<option>" << DIAGINFO << "</option>";
	    *out << "<option>" << DIAGWARN << "</option>";
	    *out << "<option>" << DIAGUSERINFO << "</option>";
	    *out << "<option>" << DIAGERROR << "</option>";
	    *out << "<option>" << DIAGFATAL << "</option>";
	    *out << "<option>" << DIAGOFF << "</option>";
		*out << "<option selected>" << DIAGUNCHANGED << "</option>";
	    *out << "</select>"<< std::endl;
		*out << "<br>";

	    *out << cgicc::p() << cgicc::input().set("type", "submit")
		    .set("name", "submit")
		    .set("value", "Apply Modifications");
	    *out << cgicc::p() << std::endl ;
	    *out << cgicc::form() << std::endl;

	    *out << cgicc::form().set("method","post")
		    .set("action", cancelLvlsUrl)
		    .set("enctype","multipart/form-data") << std::endl;
	    *out << cgicc::p() << cgicc::input().set("type", "submit")
		    .set("name", "cancel")
		    .set("value", "Cancel");
	    *out << cgicc::form() << std::endl;
	}
}
//WORLD APPLICATIONS PANNEL SECTION END






#ifdef DATABASE

	//Show/Hide non faulty elements in structure
	void LogReader::switchHiddenItems(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		if (displayNonFaultyItems_ == false)
		{
			displayNonFaultyItems_ = true;
		}
		else displayNonFaultyItems_ = false;
		internalState_ = 10;
		Default(in, out);
	}


	// Close/Expand overall detector errors view
	void LogReader::switchDetectorView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		if (subDetectorShowDetails_ == false)
		{
			subDetectorShowDetails_ = true;
		}
		else subDetectorShowDetails_ = false;
		internalState_ = 10;
		Default(in, out);
	}


	// Close/Expand FECs->FEC crate errors view
	void LogReader::switchAllFecsView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		if (allFecsShowDetails_ == false)
		{
			allFecsShowDetails_ = true;
		}
		else allFecsShowDetails_ = false;
		internalState_ = 10;
		Default(in, out);
	}


	// Close/Expand FEC crate->FEC slot errors view
	void LogReader::switchFecCratesView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		int feccrateNum = getFecCrateNumber(in);
		if (fecCratesShowDetails_[feccrateNum] == false)
		{
			fecCratesShowDetails_[feccrateNum] = true;
		}
		else
		{
			fecCratesShowDetails_[feccrateNum] = false;
		}
		internalState_ = 10;
		Default(in, out);
	}


	// Close/Expand FEC slot->FEC ring errors view
	void LogReader::switchFecSlotsView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		int feccrateNum = getFecCrateNumber(in);
		int fecslotNum = getFecSlotNumber(in);
		
		if (fecSlotItemShowDetails_[feccrateNum][fecslotNum] == false)
		{
			fecSlotItemShowDetails_[feccrateNum][fecslotNum] = true;
		}
		else
		{
			fecSlotItemShowDetails_[feccrateNum][fecslotNum] = false;
		}
		internalState_ = 10;
		Default(in, out);
	}


	// Close/Expand FEC ring->FEC ccu errors view
	void LogReader::switchFecRingsView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		int feccrateNum = getFecCrateNumber(in);
		int fecslotNum = getFecSlotNumber(in);
		int fecringNum = getFecRingNumber(in);
		
		if (fecRingItemShowDetails_[feccrateNum][fecslotNum][fecringNum] == false)
		{
			fecRingItemShowDetails_[feccrateNum][fecslotNum][fecringNum] = true;
		}
		else
		{
			fecRingItemShowDetails_[feccrateNum][fecslotNum][fecringNum] = false;
		}
		internalState_ = 10;
		Default(in, out);
	}


	// Close/Expand FEC ccu->FEC i2c channel errors view
	void LogReader::switchFecCcusView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		int feccrateNum = getFecCrateNumber(in);
		int fecslotNum = getFecSlotNumber(in);
		int fecringNum = getFecRingNumber(in);
		int fecCcuNum = getFecCcuNumber(in);
		
		if (fecCcuItemShowDetails_[feccrateNum][fecslotNum][fecringNum][fecCcuNum] == false)
		{
			fecCcuItemShowDetails_[feccrateNum][fecslotNum][fecringNum][fecCcuNum] = true;
		}
		else
		{
			fecCcuItemShowDetails_[feccrateNum][fecslotNum][fecringNum][fecCcuNum] = false;
		}
		internalState_ = 10;
		Default(in, out);
	}

	// Close/Expand FEC i2c channel->FEC i2c address errors view
	void LogReader::switchFecChannelsView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		int feccrateNum = getFecCrateNumber(in);
		int fecslotNum = getFecSlotNumber(in);
		int fecringNum = getFecRingNumber(in);
		int fecCcuNum = getFecCcuNumber(in);
		int fecChannelNum = getFecChannelNumber(in);
		
		if (fecChannelItemShowDetails_[feccrateNum][fecslotNum][fecringNum][fecCcuNum][fecChannelNum] == false)
		{
			fecChannelItemShowDetails_[feccrateNum][fecslotNum][fecringNum][fecCcuNum][fecChannelNum] = true;
		}
		else
		{
			fecChannelItemShowDetails_[feccrateNum][fecslotNum][fecringNum][fecCcuNum][fecChannelNum] = false;
		}
		internalState_ = 10;
		Default(in, out);
	}






	// Close/Expand FEDs->FED crate errors view
	void LogReader::switchAllFedsView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		if (allFedsShowDetails_ == false)
		{
			allFedsShowDetails_ = true;
		}
		else allFedsShowDetails_ = false;
		internalState_ = 10;
		Default(in, out);
	}


	// Close/Expand FED crate->FED slot errors view
	void LogReader::switchFedCratesView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		int fedcrateNum = getFedCrateNumber(in);
		if (fedCratesShowDetails_[fedcrateNum] == false)
		{
			fedCratesShowDetails_[fedcrateNum] = true;
		}
		else
		{
			fedCratesShowDetails_[fedcrateNum] = false;
		}
		internalState_ = 10;
		Default(in, out);
	}


	// Close/Expand FED slot->FED connection errors view
	void LogReader::switchFedSlotsView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		int fedcrateNum = getFedCrateNumber(in);
		int fedslotNum = getFedSlotNumber(in);
		
		if (fedSlotItemShowDetails_[fedcrateNum][fedslotNum] == false)
		{
			fedSlotItemShowDetails_[fedcrateNum][fedslotNum] = true;
		}
		else
		{
			fedSlotItemShowDetails_[fedcrateNum][fedslotNum] = false;
		}
		internalState_ = 10;
		Default(in, out);
	}




	// Close/Expand PSUs->PSU crate errors view
	void LogReader::switchAllPsusView(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		if (allPsusShowDetails_ == false)
		{
			allPsusShowDetails_ = true;
		}
		else allPsusShowDetails_ = false;
		internalState_ = 10;
		Default(in, out);
	}



	//Extract FEC,FED and PSU informations from URL when a link is clicked
	int LogReader::getFecCrateNumber(xgi::Input * in)
	{
        cgicc::CgiEnvironment cgi(in);
		std::string queryString = cgi.getQueryString();
//		std::cout << "Query String for CRATE view = " << queryString << std::endl;
		std::string::size_type loc_begin = queryString.find( "FECCRATENUM=", 0 );
		char feccrate_value[10];
		int counter=0;
		int i=(loc_begin+strlen("FECCRATENUM="));
		while ( ((queryString.c_str())[i] != '&') && ((unsigned int)i<(queryString.length())) && (counter<9) )
		{
			feccrate_value[counter] = (queryString.c_str())[i];
			counter++;
			i++;
		}
		feccrate_value[counter] = '\0';
//		std::cout << "Found substring for FEC crate: " << feccrate_value << std::endl;
		return atoi(feccrate_value);
	}


	//Extract FEC,FED and PSU informations from URL when a link is clicked
	int LogReader::getFecSlotNumber(xgi::Input * in)
	{
        cgicc::CgiEnvironment cgi(in);
		std::string queryString = cgi.getQueryString();
//		std::cout << "Query String for SLOT view = " << queryString << std::endl;
		std::string::size_type loc_begin = queryString.find( "FECSLOTNUM=", 0 );
		char fecslot_value[10];
		int counter=0;
		int i=(loc_begin+strlen("FECSLOTNUM="));
		while ( ((queryString.c_str())[i] != '&') && ((unsigned int)i<(queryString.length())) && (counter<9) )
		{
			fecslot_value[counter] = (queryString.c_str())[i];
			counter++;
			i++;
		}
		fecslot_value[counter] = '\0';
//		std::cout << "Found substring for FEC slot: " << fecslot_value << std::endl;
		return atoi(fecslot_value);
	}





	//Extract FEC,FED and PSU informations from URL when a link is clicked
	int LogReader::getFecRingNumber(xgi::Input * in)
	{
        cgicc::CgiEnvironment cgi(in);
		std::string queryString = cgi.getQueryString();
//		std::cout << "Query String for RING view = " << queryString << std::endl;
		std::string::size_type loc_begin = queryString.find( "FECRINGNUM=", 0 );
		char fecring_value[10];
		int counter=0;
		int i=(loc_begin+strlen("FECRINGNUM="));
		while ( ((queryString.c_str())[i] != '&') && ((unsigned int)i<(queryString.length())) && (counter<9) )
		{
			fecring_value[counter] = (queryString.c_str())[i];
			counter++;
			i++;
		}
		fecring_value[counter] = '\0';
//		std::cout << "Found substring for FEC ring: " << fecring_value << std::endl;
		return atoi(fecring_value);
	}




	//Extract FEC,FED and PSU informations from URL when a link is clicked
	int LogReader::getFecCcuNumber(xgi::Input * in)
	{
        cgicc::CgiEnvironment cgi(in);
		std::string queryString = cgi.getQueryString();
//		std::cout << "Query String for CCU view = " << queryString << std::endl;
		std::string::size_type loc_begin = queryString.find( "FECCCUNUM=", 0 );
		char fecccu_value[10];
		int counter=0;
		int i=(loc_begin+strlen("FECCCUNUM="));
		while ( ((queryString.c_str())[i] != '&') && ((unsigned int)i<(queryString.length())) && (counter<9) )
		{
			fecccu_value[counter] = (queryString.c_str())[i];
			counter++;
			i++;
		}
		fecccu_value[counter] = '\0';
//		std::cout << "Found substring for FEC ccu: " << fecccu_value << std::endl;
		return atoi(fecccu_value);
	}




	//Extract FEC,FED and PSU informations from URL when a link is clicked
	int LogReader::getFecChannelNumber(xgi::Input * in)
	{
        cgicc::CgiEnvironment cgi(in);
		std::string queryString = cgi.getQueryString();
//		std::cout << "Query String for CHANNEL view = " << queryString << std::endl;
		std::string::size_type loc_begin = queryString.find( "FECCHANNELNUM=", 0 );
		char fecchannel_value[10];
		int counter=0;
		int i=(loc_begin+strlen("FECCHANNELNUM="));
		while ( ((queryString.c_str())[i] != '&') && ((unsigned int)i<(queryString.length())) && (counter<9) )
		{
			fecchannel_value[counter] = (queryString.c_str())[i];
			counter++;
			i++;
		}
		fecchannel_value[counter] = '\0';
//		std::cout << "Found substring for FEC channel: " << fecchannel_value << std::endl;
		return atoi(fecchannel_value);
	}




	//Extract FEC,FED and PSU informations from URL when a link is clicked
	int LogReader::getFedCrateNumber(xgi::Input * in)
	{
        cgicc::CgiEnvironment cgi(in);
		std::string queryString = cgi.getQueryString();
//		std::cout << "Query String for FED CRATE view = " << queryString << std::endl;
		std::string::size_type loc_begin = queryString.find( "FEDCRATENUM=", 0 );
		char fedcrate_value[10];
		int counter=0;
		int i=(loc_begin+strlen("FEDCRATENUM="));
		while ( ((queryString.c_str())[i] != '&') && ((unsigned int)i<(queryString.length())) && (counter<9) )
		{
			fedcrate_value[counter] = (queryString.c_str())[i];
			counter++;
			i++;
		}
		fedcrate_value[counter] = '\0';
//		std::cout << "Found substring for FED crate: " << fedcrate_value << std::endl;
		return atoi(fedcrate_value);
	}


	//Extract FEC,FED and PSU informations from URL when a link is clicked
	int LogReader::getFedSlotNumber(xgi::Input * in)
	{
        cgicc::CgiEnvironment cgi(in);
		std::string queryString = cgi.getQueryString();
//		std::cout << "Query String for FED SLOT view = " << queryString << std::endl;
		std::string::size_type loc_begin = queryString.find( "FEDSLOTNUM=", 0 );
		char fedslot_value[10];
		int counter=0;
		int i=(loc_begin+strlen("FEDSLOTNUM="));
		while ( ((queryString.c_str())[i] != '&') && ((unsigned int)i<(queryString.length())) && (counter<9) )
		{
			fedslot_value[counter] = (queryString.c_str())[i];
			counter++;
			i++;
		}
		fedslot_value[counter] = '\0';
//		std::cout << "Found substring for FED slot: " << fedslot_value << std::endl;
		return atoi(fedslot_value);
	}





	//Temporary method
	void LogReader::simulateFecError(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		cgicc::Cgicc cgi(in);

        simulatedFecHardId_ = cgi["simulatedFecHardId"]->getValue();
        simulatedFecRing_ = cgi["simulatedFecRing"]->getValue();
        simulatedCcuAddress_ = cgi["simulatedCcuAddress"]->getValue();
        simulatedI2cChannel_ = cgi["simulatedI2cChannel"]->getValue();
        simulatedI2cAddress_ = cgi["simulatedI2cAddress"]->getValue();

/*
std::cout << "Setting top node value" << std::endl;
errorsTreeBuilder->setFecNodeError();
std::cout << "End of setting top node value" << std::endl;
*/
		if (simulatedFecHardId_ != "")
		{
			unsigned int crateSlot;
			unsigned int fecSlot;
			
			try
			{
				//Convert FecHardId into CrateSlot/FecSlot information
				errorsChecker->getCrateAndFecSlotsForFechardid(&crateSlot, &fecSlot, tkDiagErrorAnalyser, simulatedFecHardId_);
				if (simulatedFecRing_ == "")
				{
					tkDiagErrorAnalyser->setFecError(	simulatedFecHardId_);
					errorsTreeBuilder->setFecNodeError( crateSlot,
														fecSlot);
				}
				else
				{
					if (simulatedCcuAddress_ == "")
					{
						tkDiagErrorAnalyser->setFecError(	simulatedFecHardId_,
															(unsigned int)atoi(simulatedFecRing_.c_str()) );
						errorsTreeBuilder->setFecNodeError( crateSlot,
															fecSlot,
															(unsigned int)atoi(simulatedFecRing_.c_str()) );
					}
					else
					{
						if (simulatedI2cChannel_ == "")
						{
							tkDiagErrorAnalyser->setFecError(	simulatedFecHardId_,
																(unsigned int)atoi(simulatedFecRing_.c_str()),
																(unsigned int)atoi(simulatedCcuAddress_.c_str()) );
							errorsTreeBuilder->setFecNodeError( crateSlot,
																fecSlot,
																(unsigned int)atoi(simulatedFecRing_.c_str()),
																(unsigned int)atoi(simulatedCcuAddress_.c_str()) );
						}
						else
						{
							if (simulatedI2cAddress_ == "")
							{
								tkDiagErrorAnalyser->setFecError(	simulatedFecHardId_,
																	(unsigned int)atoi(simulatedFecRing_.c_str()),
																	(unsigned int)atoi(simulatedCcuAddress_.c_str()),
																	(unsigned int)atoi(simulatedI2cChannel_.c_str()) );
								errorsTreeBuilder->setFecNodeError( crateSlot,
																	fecSlot,
																	(unsigned int)atoi(simulatedFecRing_.c_str()),
																	(unsigned int)atoi(simulatedCcuAddress_.c_str()),
																	(unsigned int)atoi(simulatedI2cChannel_.c_str()) );

							}
							else
							{
								tkDiagErrorAnalyser->setFecError(	simulatedFecHardId_,
																	(unsigned int)atoi(simulatedFecRing_.c_str()),
																	(unsigned int)atoi(simulatedCcuAddress_.c_str()),
																	(unsigned int)atoi(simulatedI2cChannel_.c_str()),
																	(unsigned int)atoi(simulatedI2cAddress_.c_str()) );
								errorsTreeBuilder->setFecNodeError( crateSlot,
																	fecSlot,
																	(unsigned int)atoi(simulatedFecRing_.c_str()),
																	(unsigned int)atoi(simulatedCcuAddress_.c_str()),
																	(unsigned int)atoi(simulatedI2cChannel_.c_str()),
																	(unsigned int)atoi(simulatedI2cAddress_.c_str()) );

							}
						}
					}
				}

			}
			catch (std::string &e) {}
			catch (oracle::occi::SQLException &e) {}
			catch (FecExceptionHandler &e) {}
		}
		internalState_ = 10;
		Default(in, out);
	}




	void LogReader::simulateFedError(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
        cgicc::Cgicc cgi(in);
		unsigned int crateSlot;
		unsigned int fedSlot;
		
        simulatedFedId_ = cgi["simulatedFedId"]->getValue();
        simulatedFedChannel_ = cgi["simulatedFedChannel"]->getValue();
/*
std::cout << "Setting top node value" << std::endl;
errorsTreeBuilder->setFedNodeError();
std::cout << "End of setting top node value" << std::endl;
*/


		//tkDiagErrorAnalyser->setFedSoftIdError(126, 91);
		if (simulatedFedId_ != "")
		{

			try
			{
				//Convert FedId into CrateSlot/FedSlot information
				errorsChecker->getCrateAndFedSlotsForFedid(&crateSlot, &fedSlot, tkDiagErrorAnalyser, (unsigned int)atoi(simulatedFedId_.c_str()));

				if (simulatedFedChannel_ == "")
				{
					tkDiagErrorAnalyser->setFedSoftIdError(	(unsigned int)atoi(simulatedFedId_.c_str()) );
					errorsTreeBuilder->setFedNodeError( crateSlot,
														fedSlot);
				}
				else
				{
					tkDiagErrorAnalyser->setFedSoftIdError(	(unsigned int)atoi(simulatedFedId_.c_str()),
														(unsigned int)atoi(simulatedFedChannel_.c_str()) );
					errorsTreeBuilder->setFedNodeError( crateSlot,
														fedSlot,
														(unsigned int)atoi(simulatedFedChannel_.c_str()));

				}
			}
			catch (std::string &e) {}
			catch (oracle::occi::SQLException &e) {}
			catch (FecExceptionHandler &e) {}
		}


		internalState_ = 10;
		Default(in, out);
	}




	void LogReader::simulatePsuError(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
        cgicc::Cgicc cgi(in);

        simulatedPsuId_ = cgi["simulatedPsuId"]->getValue();

/*
std::cout << "Setting top node value" << std::endl;
errorsTreeBuilder->setPsuNodeError();
std::cout << "End of setting top node value" << std::endl;
*/
		if (simulatedPsuId_ != "")
		{
			try
			{
				tkDiagErrorAnalyser->setDetIdError( (unsigned int)atoi(simulatedPsuId_.c_str()) );
				unsigned int psuRank = errorsChecker->getPsuIdRange((unsigned int)atoi(simulatedPsuId_.c_str()),
												tkDiagErrorAnalyser);
				errorsTreeBuilder->setPsuNodeError( psuRank );
			}
			catch (std::string &e) {}
			catch (oracle::occi::SQLException &e) {}
			catch (FecExceptionHandler &e) {}
		}


		internalState_ = 10;
		Default(in, out);
	}





	//Temporary method
	void LogReader::displayDetIdErrors(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		try
		{
	    	unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
		    std::vector<unsigned int> listDetId = tkDiagErrorAnalyser->getListOfDetId() ;
	    	std::cout << "Number of det id " << listDetId.size() << " found in DB" << std::endl ;
		    //unsigned int i = 0 ;
		    for (std::vector<unsigned int>::iterator it = listDetId.begin() ; it != listDetId.end() ; it ++)
			{
	    	 	unsigned int fecErrorCounter, fedErrorCounter, psuErrorCounter = 8 ;
    	  		tkDiagErrorAnalyser->getDetIdErrorCounter(*it,fecErrorCounter,fedErrorCounter,psuErrorCounter) ;
				if (fecErrorCounter || fedErrorCounter || psuErrorCounter)
				{
		    	  	std::cout << (*it) << " has " << fecErrorCounter << ":" << fedErrorCounter << ":" << psuErrorCounter << std::endl ;
				}

				//Add errors / populate map
    	  		//if (i < 10) { tkDiagErrorAnalyser->setDetIdError((*it)) ; i ++; }
	    	}
	    	unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
		    std::cout << "Call took " << (endMillis-startMillis) << " ms" << std::endl ;
		}
		catch (std::string &e) {}
		catch (oracle::occi::SQLException &e) {}
		catch (FecExceptionHandler &e) {}
		internalState_ = 10;
		Default(in, out);
	}


	//Temporary method
	void LogReader::displayFecErrors(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		// Display all errors by FEC
		try
		{
			unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
			ConnectionVector connections = tkDiagErrorAnalyser->getConnectionByFec() ;
			std::cout << "Number of connections " << connections.size() << " found in DB" << std::endl ;
			for (ConnectionVector::iterator it = connections.begin() ; it != connections.end() ; it ++)
			{
				unsigned int fecErrorCounter, fedErrorCounter, psuErrorCounter  = 8 ;
				ConnectionDescription *connection = *it ;


      		tkDiagErrorAnalyser->getConnectionErrorCounter(	connection->getFecHardwareId(),
															connection->getRingSlot(),
															connection->getCcuAddress(),
															connection->getI2cChannel(),
															connection->getApvAddress(),
															fecErrorCounter,
															fedErrorCounter,
															psuErrorCounter);

/*
				tkDiagErrorAnalyser->getFecModuleErrorCounter(connection->getFecHardwareId(),connection->getRingSlot(),connection->getCcuAddress(),
						    connection->getI2cChannel(),fecErrorCounter,fedErrorCounter,psuErrorCounter) ;
*/
//std::cout << "FedSlot:" << connection->getFedId() << "  FedChannel:" << connection->getFedChannel() << std::endl;
//std::cout << "DetID:" << connection->getDetId() << std::endl;





				if (fecErrorCounter || fedErrorCounter || psuErrorCounter)
				{
//					std::cout << connection->getFecCrateSlot() << ":" << connection->getFecSlot() << ":" 
					std::cout << connection->getFecHardwareId() << ":0x" 
							<< connection->getRingSlot() << std::hex << ":0x" << connection->getCcuAddress() << ":0x"
							<< connection->getI2cChannel() << ":0x"<< connection->getApvAddress() << std::dec 
							<< " has " << fecErrorCounter << ":" << fedErrorCounter << ":" << psuErrorCounter << std::endl ;

							unsigned int fecErrorCounter1=0, fedErrorCounter1=0, psuErrorCounter1=0 ;
					tkDiagErrorAnalyser->getDetIdErrorCounter(connection->getDetId(),fecErrorCounter1,fedErrorCounter1,psuErrorCounter1) ;
			      }
		    }
			unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
			std::cout << "Call took " << (endMillis-startMillis) << " ms" << std::endl ;
		}
		catch (std::string &e) {}
		catch (oracle::occi::SQLException &e) {}
		catch (FecExceptionHandler &e) {}
		internalState_ = 10;
		Default(in, out);
	}




	//Count all FEC, FED & PSU errors present on the tracker
	void LogReader::setTrackerCounters(unsigned int & trackerFecErrorsCounter, unsigned int & trackerFedErrorsCounter, unsigned int & trackerPsuErrorsCounter)
	{
		unsigned int fec = 0, fed = 0, psu = 0;
	    std::vector<unsigned int> listDetId = tkDiagErrorAnalyser->getListOfDetId() ;
	    for (std::vector<unsigned int>::iterator it = listDetId.begin() ; it != listDetId.end() ; it ++)
		{
			tkDiagErrorAnalyser->getDetIdErrorCounter(*it, fec, fed, psu) ;
			trackerFecErrorsCounter += fec;
			trackerFedErrorsCounter += fed;
			trackerPsuErrorsCounter += psu;
		}
	}



	void LogReader::getLogsNoDbPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		*out << "Feature unavailable due to DB misconfiguration. Your current DB parameters are : <br>" << std::endl;
		*out << "PartitionName = " << DbPartitionName_ << "<br>" << std::endl;
		*out << "Login = " << DbLogin_ << "<br>" << std::endl;
		//Commented following Fred's request
		//*out << "Passwd = " << DbPasswd_ << "<br>" << std::endl;
		*out << "Path = " << DbPath_ << "<br>" << std::endl;
	}



	void LogReader::getLogsSummaryPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		ConnectionVector fecConnections = tkDiagErrorAnalyser->getConnectionByFec();

		//Get FEC top level node
		unsigned long fecNodeErrors=0;
		unsigned long fedNodeErrors=0;
		unsigned long psuNodeErrors=0;
		std::string directHitsString;
		char convertToAscii[10];

		*out << "Using partition : " << DbPartitionName_ << "<br>";


/*

		*out << "SUMMARY PAGE - UNDER CONSTRUCTION" << std::endl;

		std::string SimulateFecError_ = "/";
		SimulateFecError_ += getApplicationDescriptor()->getURN();
		SimulateFecError_ += "/simulateFecError";	
		*out << cgicc::form().set("method","post")
			.set("action", SimulateFecError_)
			.set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::p() << cgicc::input().set("type", "submit")
			.set("name", "submit")
			.set("value", "Simulate FEC Error");

		
		*out << "<br>" << std::endl;
       	// Create an input : FEC hard ID
        *out << cgicc::label("FEC HardID") << std::endl;
       	*out << cgicc::input().set("type","text")
        .set("name","simulatedFecHardId")
   	    .set("size","20")
       	.set("value",simulatedFecHardId_) << std::endl;

       	// Create an input : FEC Ring
        *out << cgicc::label("FEC Ring") << std::endl;
       	*out << cgicc::input().set("type","text")
        .set("name","simulatedFecRing")
   	    .set("size","5")
       	.set("value",simulatedFecRing_) << std::endl;

       	// Create an input : CCU Address
        *out << cgicc::label("CCU Address") << std::endl;
       	*out << cgicc::input().set("type","text")
        .set("name","simulatedCcuAddress")
   	    .set("size","5")
       	.set("value",simulatedCcuAddress_) << std::endl;

       	// Create an input : I2C Channel
        *out << cgicc::label("I2C Channel") << std::endl;
       	*out << cgicc::input().set("type","text")
        .set("name","simulatedI2cChannel")
   	    .set("size","5")
       	.set("value",simulatedI2cChannel_) << std::endl;

       	// Create an input : I2C Address
        *out << cgicc::label("I2C Address") << std::endl;
       	*out << cgicc::input().set("type","text")
        .set("name","simulatedI2cAddress")
   	    .set("size","5")
       	.set("value",simulatedI2cAddress_) << std::endl;

		*out << cgicc::form() << std::endl;












		std::string SimulateFedError_ = "/";
		SimulateFedError_ += getApplicationDescriptor()->getURN();
		SimulateFedError_ += "/simulateFedError";	
		*out << cgicc::form().set("method","post")
			.set("action", SimulateFedError_)
			.set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::p() << cgicc::input().set("type", "submit")
			.set("name", "submit")
			.set("value", "Simulate FED Error");

		*out << "<br>" << std::endl;
       	// Create an input : FED ID
        *out << cgicc::label("FED ID") << std::endl;
       	*out << cgicc::input().set("type","text")
        .set("name","simulatedFedId")
   	    .set("size","12")
       	.set("value",simulatedFedId_) << std::endl;

       	// Create an input : FED Channel
        *out << cgicc::label("FED Channel") << std::endl;
       	*out << cgicc::input().set("type","text")
        .set("name","simulatedFedChannel")
   	    .set("size","5")
       	.set("value",simulatedFedChannel_) << std::endl;

		*out << cgicc::form() << std::endl;



		std::string SimulatePsuError_ = "/";
		SimulatePsuError_ += getApplicationDescriptor()->getURN();
		SimulatePsuError_ += "/simulatePsuError";	
		*out << cgicc::form().set("method","post")
			.set("action", SimulatePsuError_)
			.set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::p() << cgicc::input().set("type", "submit")
			.set("name", "submit")
			.set("value", "Simulate PSU Error");

		*out << "<br>" << std::endl;
       	// Create an input : PSU ID
        *out << cgicc::label("PSU ID") << std::endl;
       	*out << cgicc::input().set("type","text")
        .set("name","simulatedPsuId")
   	    .set("size","20")
       	.set("value",simulatedPsuId_) << std::endl;

		*out << cgicc::form() << std::endl;

*/






/*

		std::string displayDetIdErrors_ = "/";
		displayDetIdErrors_ += getApplicationDescriptor()->getURN();
		displayDetIdErrors_ += "/displayDetIdErrors";	
		*out << cgicc::form().set("method","post")
			.set("action", displayDetIdErrors_)
			.set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::p() << cgicc::input().set("type", "submit")
			.set("name", "submit")
			.set("value", "Display errors by DETIDs");
		*out << cgicc::form() << std::endl;


		std::string displayFecErrors_ = "/";
		displayFecErrors_ += getApplicationDescriptor()->getURN();
		displayFecErrors_ += "/displayFecErrors";	
		*out << cgicc::form().set("method","post")
			.set("action", displayFecErrors_)
			.set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::p() << cgicc::input().set("type", "submit")
			.set("name", "submit")
			.set("value", "Display errors by FECs");
		*out << cgicc::form() << std::endl;

*/

		////////////////////////////////////////////////////////////////////////
		//Global Display management
		////////////////////////////////////////////////////////////////////////
		*out << "<br>";
		std::string switchHiddenItems_ = "/";
		switchHiddenItems_ += getApplicationDescriptor()->getURN();
		switchHiddenItems_ += "/switchHiddenItems";
		if (displayNonFaultyItems_ == false)//IFBLOCK
		{
			*out << "<br><a href=" << switchHiddenItems_ << ">Show non faulty elements in tree structure</a>";
		}
		else *out << "<br><a href=" << switchHiddenItems_ << ">Hide non faulty elements in tree structure</a>";



		////////////////////////////////////////////////////////////////////////
		//Manage Detector block
		////////////////////////////////////////////////////////////////////////
		unsigned int FEC_fec=0, FEC_fed=0, FEC_psu=0;
		errorsChecker->getFecViewErrorsForDetector(&FEC_fec, &FEC_fed, &FEC_psu, tkDiagErrorAnalyser);

		unsigned int FED_fec=0, FED_fed=0, FED_psu=0;
		errorsChecker->getFedViewErrorsForDetector(&FED_fec, &FED_fed, &FED_psu, tkDiagErrorAnalyser);

		unsigned int PSU_fec=0, PSU_fed=0, PSU_psu=0;
		errorsChecker->getPsuViewErrorsForDetector(&PSU_fec, &PSU_fed, &PSU_psu, tkDiagErrorAnalyser);


/*
std::cout << "FEC_fec:" << FEC_fec << "  FEC_fed:" << FEC_fed << "  FEC_psu:" << FEC_psu << std::endl;
std::cout << "FED_fec:" << FED_fec << "  FED_fed:" << FED_fed << "  FED_psu:" << FED_psu << std::endl;
std::cout << "PSU_fec:" << PSU_fec << "  PSU_fed:" << PSU_fed << "  PSU_psu:" << PSU_psu << std::endl;
*/


		std::string switchDetectorView_ = "/";
		switchDetectorView_ += getApplicationDescriptor()->getURN();
		switchDetectorView_ += "/switchDetectorView";
		if (subDetectorShowDetails_ == false)
		{
			*out << "<br>";
			*out << "<a href=" << switchDetectorView_ << ">+</a>";
			*out << "&nbsp;Display views of errors in detector SiStrTracker" << 
					" (" << FEC_fec << " FEC errors)" <<
					" (" << FEC_fed << " FED errors)" <<
					" (" << FEC_psu << " PSU errors)" << std::endl;
		}
		else
		{
			*out << "<br>";
			*out << "<a href=" << switchDetectorView_ << ">-</a>";
			*out << "&nbsp;Hide views of errors in detector SiStrTracker" << 
					" (" << FEC_fec << " FEC errors)" <<
					" (" << FEC_fed << " FED errors)" <<
					" (" << FEC_psu << " PSU errors)" << std::endl;



			////////////////////////////////////////////////////////////////////////
			//Manage FEC View Block
			////////////////////////////////////////////////////////////////////////
			//Get FEC top level node
			fecNodeErrors = errorsTreeBuilder->getFecNodeError();
			snprintf(convertToAscii, 9 , "%lu", fecNodeErrors);
			directHitsString = "";
			if (fecNodeErrors == 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hit</span>)";
			else if (fecNodeErrors > 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hits</span>)";

			
            std::string switchAllFecsView_ = "/";
			switchAllFecsView_ += getApplicationDescriptor()->getURN();
			switchAllFecsView_ += "/switchAllFecsView";
			*out << "<br>";
			if (allFecsShowDetails_ == false)//IFBLOCK
			{
				*out << "<br>";
        	    *out << "&nbsp;&nbsp;&nbsp;&nbsp;<a href=" << switchAllFecsView_ << ">+</a>";
				*out << "&nbsp;Display FEC views of errors" << 
					directHitsString <<
					" (" << FEC_fec << " FEC errors)" <<
					" (" << FEC_fed << " FED errors)" <<
					" (" << FEC_psu << " PSU errors)" << std::endl;
			}
			else//ELSEBLOCK
			{
				*out << "<br>";
        	    *out << "&nbsp;&nbsp;&nbsp;&nbsp;<a href=" << switchAllFecsView_ << ">-</a>";
				*out << "&nbsp;Hide FEC views of errors" <<
					directHitsString <<
					" (" << FEC_fec << " FEC errors)" <<
					" (" << FEC_fed << " FED errors)" <<
					" (" << FEC_psu << " PSU errors)" << std::endl;


				////////////////////////////////////////////////////////////////////////
				//Manage FEC CRATE View Block
				////////////////////////////////////////////////////////////////////////
				//Find which fec crates exists in the current configuration
				errorsChecker->getFecCratesList(fecCratesList, tkDiagErrorAnalyser);
				bool oneFecCrateFound = false;
				//For Each of these crates holding FECs
				for (unsigned int fecCrateNumber=0; fecCrateNumber<NUMBER_OF_FEC_CRATES; fecCrateNumber++) //FORCRATE
				{
					if (fecCratesList[fecCrateNumber]==true)
					{
						//Count how many errors we have on one specific crate
						unsigned int FECCRATE_fec=0, FECCRATE_fed=0, FECCRATE_psu=0;
						errorsChecker->getFecViewErrorsForCrate( &FECCRATE_fec,
																	&FECCRATE_fed,
																	&FECCRATE_psu,
																	tkDiagErrorAnalyser,
																	fecCrateNumber);

/*
						//count how many errors we can sum up on the FEDs attached to this FEC crate
						unsigned int FEDCRATES_fec=0, FEDCRATES_fed=0, FEDCRATES_psu=0;
						errorsChecker->getFedErrorsForOneFecCrate( &FEDCRATES_fec,
																	&FEDCRATES_fed,
																	&FEDCRATES_psu,
																	tkDiagErrorAnalyser,
																	fecCrateNumber);
						
						//count how many errors we can sum up on the PSUs attached to this FEC crate
						unsigned int PSUCRATES_fec=0, PSUCRATES_fed=0, PSUCRATES_psu=0;
						errorsChecker->getPsuErrorsForOneFecCrate( &PSUCRATES_fec,
																	&PSUCRATES_fed,
																	&PSUCRATES_psu,
																	tkDiagErrorAnalyser,
																	fecCrateNumber);
*/
						////////////////////////////////////////////////////////////////////////
						//Build Output
						////////////////////////////////////////////////////////////////////////
						//Get FEC crate level node
						fecNodeErrors = errorsTreeBuilder->getFecNodeError(fecCrateNumber);
						snprintf(convertToAscii, 9 , "%lu", fecNodeErrors);
						directHitsString = "";
						if (fecNodeErrors == 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hit</span>)";
						else if (fecNodeErrors > 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hits</span>)";

			            std::string switchFecCratesView_ = "/";
						switchFecCratesView_ += getApplicationDescriptor()->getURN();
						switchFecCratesView_ += "/switchFecCratesView";
							
						std::ostringstream switchFecCrateView_FullUrl;
						switchFecCrateView_FullUrl << switchFecCratesView_ << "?FECCRATENUM=" << fecCrateNumber << "&";
						if (((FECCRATE_fec || FECCRATE_fed || FECCRATE_psu) && (!displayNonFaultyItems_)) || (displayNonFaultyItems_))
						{
							oneFecCrateFound = true;
							
							if (fecCratesShowDetails_[fecCrateNumber] == false)
							{
								*out << "<br>";
		    	    		    *out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" <<
								"<a href=" << switchFecCrateView_FullUrl.str() << ">+</a>";
								*out << "&nbsp;(Expand) : FEC Crate number " << fecCrateNumber << directHitsString << " (" << FECCRATE_fec << " FEC errors) " << 
								" ("<< FECCRATE_fed << " FED errors) (" << FECCRATE_psu << " PSU errors)" << std::endl;
							}
							else
							{
								*out << "<br>";
		    	    		    *out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" <<
								"<a href=" << switchFecCrateView_FullUrl.str() << ">-</a>";
								*out << "&nbsp;(Hide) : FEC Crate number " << fecCrateNumber << directHitsString << " (" << FECCRATE_fec << " FEC errors) " << 
								" ("<< FECCRATE_fed << " FED errors) (" << FECCRATE_psu << " PSU errors)" << std::endl;

								////////////////////////////////////////////////////////////////////////
								//Manage FEC SLOT View Block
								////////////////////////////////////////////////////////////////////////
								//Find which fec slots are in use for a given crate
								errorsChecker->getFecSlotsListForOneCrate(fecSlotsList, tkDiagErrorAnalyser, fecCrateNumber);

								bool oneFecSlotFound = false;
								//For Each of these Slots holding one FEC
								for (unsigned int fecSlotNumber=0; fecSlotNumber<NUMBER_OF_FEC_SLOTS; fecSlotNumber++)
								{
									if (fecSlotsList[fecSlotNumber]==true)
									{

										//Count how many errors we have on one specific slot
										unsigned int FECSLOT_fec=0, FECSLOT_fed=0, FECSLOT_psu=0;
										std::string lclFecHardId;
										errorsChecker->getFecViewErrorsForSlot( &FECSLOT_fec,
																				&FECSLOT_fed,
																				&FECSLOT_psu,
																				tkDiagErrorAnalyser,
																				fecCrateNumber,
																				fecSlotNumber,
																				lclFecHardId);
/*
										//count how many errors we can sum up on the FEDs attached to this FEC crate
										unsigned int FEDSLOTS_fec=0, FEDSLOTS_fed=0, FEDSLOTS_psu=0;
										errorsChecker->getFedErrorsForOneFecSlot( &FEDSLOTS_fec,
																					&FEDSLOTS_fed,
																					&FEDSLOTS_psu,
																					tkDiagErrorAnalyser,
																					fecCrateNumber,
																					fecSlotNumber);
						

										//count how many errors we can sum up on the PSUs attached to this FEC crate
										unsigned int PSUSLOTS_fec=0, PSUSLOTS_fed=0, PSUSLOTS_psu=0;
										errorsChecker->getPsuErrorsForOneFecSlot( &PSUSLOTS_fec,
																					&PSUSLOTS_fed,
																					&PSUSLOTS_psu,
																					tkDiagErrorAnalyser,
																					fecCrateNumber,
																					fecSlotNumber);
*/

										////////////////////////////////////////////////////////////////////////
										//Build Output
										////////////////////////////////////////////////////////////////////////
										//Get FEC slot level node
										fecNodeErrors = errorsTreeBuilder->getFecNodeError(fecCrateNumber, fecSlotNumber);
										snprintf(convertToAscii, 9 , "%lu", fecNodeErrors);
										directHitsString = "";
										if (fecNodeErrors == 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hit</span>)";
										else if (fecNodeErrors > 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hits</span>)";

							            std::string switchFecSlotsView_ = "/";
										switchFecSlotsView_ += getApplicationDescriptor()->getURN();
										switchFecSlotsView_ += "/switchFecSlotsView";
							
										std::ostringstream switchFecSlotsView_FullUrl;
										switchFecSlotsView_FullUrl << switchFecSlotsView_ << "?FECCRATENUM=" << fecCrateNumber << "&" << "FECSLOTNUM=" << fecSlotNumber;
										if (((FECSLOT_fec || FECSLOT_fed || FECSLOT_psu) && (!displayNonFaultyItems_)) || (displayNonFaultyItems_))
										{
											oneFecSlotFound = true;

											if (fecSlotItemShowDetails_[fecCrateNumber][fecSlotNumber] == false)
											{
												*out << "<br>";
		    	    		    				*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << 
														"<a href=" << switchFecSlotsView_FullUrl.str() << ">+</a>";
												*out << "&nbsp;(Expand) : FEC (hardId:" << lclFecHardId << ") in slot number " << fecSlotNumber << directHitsString << " (" << FECSLOT_fec << " FEC errors) " << 
														" ("<< FECSLOT_fed << " FED errors) (" << FECSLOT_psu << " PSU errors)" << std::endl;

											}
											else
											{
												*out << "<br>";
		    	    		    				*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << 
														"<a href=" << switchFecSlotsView_FullUrl.str() << ">-</a>";
												*out << "&nbsp;(Hide) : FEC (hardId:" << lclFecHardId << ") in slot number " << fecSlotNumber << directHitsString << " (" << FECSLOT_fec << " FEC errors) " << 
														" ("<< FECSLOT_fed << " FED errors) (" << FECSLOT_psu << " PSU errors)" << std::endl;



												////////////////////////////////////////////////////////////////////////
												//Manage FEC PMC View Block
												////////////////////////////////////////////////////////////////////////
												//Find which fec slots are in use for a given crate
												errorsChecker->getFecRingListForOneSlot(fecRingList, tkDiagErrorAnalyser, fecCrateNumber, fecSlotNumber);
												bool oneFecPmcFound = false;

												//For Each of these Slots holding one FEC, loop on pmc's
												for (unsigned int fecPmcNumber=0; fecPmcNumber<NUMBER_OF_FEC_RING; fecPmcNumber++)
												{

													//Display unused PMC probed by FecSoftware
													if (fecRingList[fecPmcNumber]==false)
													{
														fecNodeErrors = errorsTreeBuilder->getFecNodeError(fecCrateNumber, fecSlotNumber, fecPmcNumber);
														if (fecNodeErrors != 0)
														{
															*out << "<br>" << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" <<
																	"<span style=\"color: rgb(0, 0, 210);\">" <<
																	". Detection of temptative to probe missing hardware part : FEC PMC number " << fecPmcNumber << 
																	"</span>" << std::endl;
														}
													}




													if (fecRingList[fecPmcNumber]==true)
													{
		
														//Count how many errors we have on one specific slot
														unsigned int FECPMC_fec=0, FECPMC_fed=0, FECPMC_psu=0;
														errorsChecker->getFecViewErrorsForPmc(	&FECPMC_fec,
																							&FECPMC_fed,
																							&FECPMC_psu,
																							tkDiagErrorAnalyser,
																							fecCrateNumber,
																							fecSlotNumber,
																							fecPmcNumber);
/*
														//count how many errors we can sum up on the FEDs attached to this FEC crate
														unsigned int FEDPMC_fec=0, FEDPMC_fed=0, FEDPMC_psu=0;
														errorsChecker->getFedErrorsForOneFecPmc(	&FEDPMC_fec,
																								&FEDPMC_fed,
																								&FEDPMC_psu,
																								tkDiagErrorAnalyser,
																								fecCrateNumber,
																								fecSlotNumber,
																								fecPmcNumber);
								
														//count how many errors we can sum up on the PSUs attached to this FEC crate
														unsigned int PSUPMC_fec=0, PSUPMC_fed=0, PSUPMC_psu=0;
														errorsChecker->getPsuErrorsForOneFecPmc(	&PSUPMC_fec,
																								&PSUPMC_fed,
																								&PSUPMC_psu,
																								tkDiagErrorAnalyser,
																								fecCrateNumber,
																								fecSlotNumber,
																								fecPmcNumber);

*/
														//Build Output
														//Get FEC ring level node
														fecNodeErrors = errorsTreeBuilder->getFecNodeError(fecCrateNumber, fecSlotNumber, fecPmcNumber);
														snprintf(convertToAscii, 9 , "%lu", fecNodeErrors);
														directHitsString = "";
														if (fecNodeErrors == 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hit</span>)";
														else if (fecNodeErrors > 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hits</span>)";

											            std::string switchFecRingsView_ = "/";
														switchFecRingsView_ += getApplicationDescriptor()->getURN();
														switchFecRingsView_ += "/switchFecRingsView";
							
														std::ostringstream switchFecRingsView_FullUrl;
														switchFecRingsView_FullUrl << switchFecRingsView_ << "?FECCRATENUM=" << fecCrateNumber <<
																								 "&" << "FECSLOTNUM=" << fecSlotNumber <<
																								 "&" << "FECRINGNUM=" << fecPmcNumber;
														if (((FECPMC_fec || FECPMC_fed || FECPMC_psu) && (!displayNonFaultyItems_)) || (displayNonFaultyItems_))
														{
														oneFecPmcFound = true;

															if (fecRingItemShowDetails_[fecCrateNumber][fecSlotNumber][fecPmcNumber] == false)
															{
		
																*out << "<br>";
	    	    							    				*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << 
																		"<a href=" << switchFecRingsView_FullUrl.str() << ">+</a>";
																*out << "&nbsp;(Expand) : FEC PMC number " << fecPmcNumber << directHitsString << " (" << FECPMC_fec << " FEC errors) " << 
																		" ("<< FECPMC_fed << " FED errors) (" << FECPMC_psu << " PSU errors)" << std::endl;

															}
															else
															{
																*out << "<br>";
				    	    				    				*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << 
																		"<a href=" << switchFecRingsView_FullUrl.str() << ">-</a>";
																*out << "&nbsp;(Hide) : FEC PMC number " << fecPmcNumber << directHitsString << " (" << FECPMC_fec << " FEC errors) " << 
																		" ("<< FECPMC_fed << " FED errors) (" << FECPMC_psu << " PSU errors)" << std::endl;

	
																////////////////////////////////////////////////////////////////////////
																//Manage FEC CCU View Block
																////////////////////////////////////////////////////////////////////////
																//Find which CCUs are in use for a given PMC
																errorsChecker->getFecCcuListForOnePmc(fecCcuList, tkDiagErrorAnalyser, fecCrateNumber, fecSlotNumber, fecPmcNumber);
																bool oneFecCcuFound = false;
																//For Each of these PMCs holding a ring, loop on CCU's
																for (unsigned int fecCcuNumber=0; fecCcuNumber<NUMBER_OF_FEC_CCU; fecCcuNumber++)
																{

																	//Display unused CCU probed by FecSoftware
																	if (fecCcuList[fecCcuNumber]==false)
																	{
																		fecNodeErrors = errorsTreeBuilder->getFecNodeError(fecCrateNumber, fecSlotNumber, fecPmcNumber, fecCcuNumber);
																		if (fecNodeErrors != 0)
																		{
																			*out << "<br>" << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << 
																					"<span style=\"color: rgb(0, 0, 210);\">" <<
																					". Detection of temptative to probe missing hardware part : FEC CCU address " << fecCcuNumber << 
																					"</span>" << std::endl;
																		}
																	}



																	if (fecCcuList[fecCcuNumber]==true)
																	{
				
																		//Count how many errors we have on one specific slot
																		unsigned int FECCCU_fec=0, FECCCU_fed=0, FECCCU_psu=0;
																		errorsChecker->getFecViewErrorsForCcu(	&FECCCU_fec,
																										&FECCCU_fed,
																										&FECCCU_psu,
																										tkDiagErrorAnalyser,
																										fecCrateNumber,
																										fecSlotNumber,
																										fecPmcNumber,
																										fecCcuNumber);
	/*
																		//count how many errors we can sum up on the FEDs attached to this CCU
																		unsigned int FEDCCU_fec=0, FEDCCU_fed=0, FEDCCU_psu=0;
																		errorsChecker->getFedErrorsForOneFecCcu(	&FEDCCU_fec,
																											&FEDCCU_fed,
																											&FEDCCU_psu,
																											tkDiagErrorAnalyser,
																											fecCrateNumber,
																											fecSlotNumber,
																											fecPmcNumber,
																											fecCcuNumber);

																		//count how many errors we can sum up on the PSUs attached to this CCU
																		unsigned int PSUCCU_fec=0, PSUCCU_fed=0, PSUCCU_psu=0;
																		errorsChecker->getPsuErrorsForOneFecCcu(	&PSUCCU_fec,
																											&PSUCCU_fed,
																											&PSUCCU_psu,
																											tkDiagErrorAnalyser,
																											fecCrateNumber,
																											fecSlotNumber,
																											fecPmcNumber,
																											fecCcuNumber);
*/
																		//Build Output
																		//Get FEC CCU level node
																		fecNodeErrors = errorsTreeBuilder->getFecNodeError(fecCrateNumber, fecSlotNumber, fecPmcNumber, fecCcuNumber);
																		snprintf(convertToAscii, 9 , "%lu", fecNodeErrors);
																		directHitsString = "";
																		if (fecNodeErrors == 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hit</span>)";
																		else if (fecNodeErrors > 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hits</span>)";

															            std::string switchFecCcusView_ = "/";
																		switchFecCcusView_ += getApplicationDescriptor()->getURN();
																		switchFecCcusView_ += "/switchFecCcusView";
							
																		std::ostringstream switchFecCcusView_FullUrl;
																		switchFecCcusView_FullUrl << switchFecCcusView_ << "?FECCRATENUM=" << fecCrateNumber <<
																											 				"&" << "FECSLOTNUM=" << fecSlotNumber <<
																											 				"&" << "FECRINGNUM=" << fecPmcNumber <<
																															"&" << "FECCCUNUM=" << fecCcuNumber;
																		if (((FECCCU_fec || FECCCU_fed || FECCCU_psu) && (!displayNonFaultyItems_)) || (displayNonFaultyItems_))
																		{
																			oneFecCcuFound = true;

																			if (fecCcuItemShowDetails_[fecCrateNumber][fecSlotNumber][fecPmcNumber][fecCcuNumber] == false)
																			{
																				*out << "<br>";
	    				    								    				*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << 
																						"<a href=" << switchFecCcusView_FullUrl.str() << ">+</a>";
																				*out << "&nbsp;(Expand) : FEC CCU number " << fecCcuNumber << directHitsString << " (" << FECCCU_fec << " FEC errors) " << 
																						" ("<< FECCCU_fed << " FED errors) (" << FECCCU_psu << " PSU errors)" << std::endl;
																			}
																			else
																			{
																				*out << "<br>";
				    				    					    				*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << 
																						"<a href=" << switchFecCcusView_FullUrl.str() << ">-</a>";
																				*out << "&nbsp;(Hide) : FEC CCU number " << fecCcuNumber << directHitsString << " (" << FECCCU_fec << " FEC errors) " << 
																						" ("<< FECCCU_fed << " FED errors) (" << FECCCU_psu << " PSU errors)" << std::endl;


																				////////////////////////////////////////////////////////////////////////
																				//Manage FEC CHANNELS View Block
																				////////////////////////////////////////////////////////////////////////
																				bool oneFecChannelFound = false;

																				//Find which CCUs are in use for a given PMC
																				errorsChecker->getFecChannelListForOneCcu(fecChannelList, tkDiagErrorAnalyser, fecCrateNumber, fecSlotNumber, fecPmcNumber, fecCcuNumber);
																				//For Each of these PMCs holding a ring, loop on CCU's
																				for (unsigned int fecChannelNumber=0; fecChannelNumber<NUMBER_OF_FEC_CHANNEL; fecChannelNumber++)
																				{

																					//Display unused I2C channels probed by FecSoftware
																					if (fecChannelList[fecChannelNumber]==false)
																					{
																						fecNodeErrors = errorsTreeBuilder->getFecNodeError(fecCrateNumber, fecSlotNumber, fecPmcNumber, fecCcuNumber, fecChannelNumber);
																						if (fecNodeErrors != 0)
																						{
																							*out << "<br>" << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" <<
																									"<span style=\"color: rgb(0, 0, 210);\">" <<
																									". Detection of temptative to probe missing hardware part : I2C channel address " << fecChannelNumber << 
																									"</span>" << std::endl;
																						}
																					}


																					if (fecChannelList[fecChannelNumber]==true)
																					{
				
																						//Count how many errors we have on one specific slot
																						unsigned int FECCHANNEL_fec=0, FECCHANNEL_fed=0, FECCHANNEL_psu=0;
																						errorsChecker->getFecViewErrorsForChannel(	&FECCHANNEL_fec,
																														&FECCHANNEL_fed,
																														&FECCHANNEL_psu,
																														tkDiagErrorAnalyser,
																														fecCrateNumber,
																														fecSlotNumber,
																														fecPmcNumber,
																														fecCcuNumber,
																														fecChannelNumber);
/*
																						//count how many errors we can sum up on the FEDs attached to this CCU
																						unsigned int FEDCHANNEL_fec=0, FEDCHANNEL_fed=0, FEDCHANNEL_psu=0;
																						errorsChecker->getFedErrorsForOneFecChannel(	&FEDCHANNEL_fec,
																															&FEDCHANNEL_fed,
																															&FEDCHANNEL_psu,
																															tkDiagErrorAnalyser,
																															fecCrateNumber,
																															fecSlotNumber,
																															fecPmcNumber,
																															fecCcuNumber,
																															fecChannelNumber);
	
																						//count how many errors we can sum up on the PSUs attached to this CCU
																						unsigned int PSUCHANNEL_fec=0, PSUCHANNEL_fed=0, PSUCHANNEL_psu=0;
																						errorsChecker->getPsuErrorsForOneFecChannel(	&PSUCHANNEL_fec,
																															&PSUCHANNEL_fed,
																															&PSUCHANNEL_psu,
																															tkDiagErrorAnalyser,
																															fecCrateNumber,
																															fecSlotNumber,
																															fecPmcNumber,
																															fecCcuNumber,
																															fecChannelNumber);
*/
																						//Build Output
																						//Build Output
																						//Get FEC I2C channel level node
																						fecNodeErrors = errorsTreeBuilder->getFecNodeError(fecCrateNumber, fecSlotNumber, fecPmcNumber, fecCcuNumber, fecChannelNumber);
																						snprintf(convertToAscii, 9 , "%lu", fecNodeErrors);
																						directHitsString = "";
																						if (fecNodeErrors == 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hit</span>)";
																						else if (fecNodeErrors > 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hits</span>)";

																			            std::string switchFecChannelsView_ = "/";
																						switchFecChannelsView_ += getApplicationDescriptor()->getURN();
																						switchFecChannelsView_ += "/switchFecChannelsView";
							
																						std::ostringstream switchFecChannelsView_FullUrl;
																						switchFecChannelsView_FullUrl << switchFecChannelsView_ << "?FECCRATENUM=" << fecCrateNumber <<
																											 				"&" << "FECSLOTNUM=" << fecSlotNumber <<
																											 				"&" << "FECRINGNUM=" << fecPmcNumber <<
																															"&" << "FECCCUNUM=" << fecCcuNumber <<
																															"&" << "FECCHANNELNUM=" << fecChannelNumber;

																						if (((FECCHANNEL_fec || FECCHANNEL_fed || FECCHANNEL_psu) && (!displayNonFaultyItems_)) || (displayNonFaultyItems_))
																						{
																							oneFecChannelFound = true;

																							if (fecChannelItemShowDetails_[fecCrateNumber][fecSlotNumber][fecPmcNumber][fecCcuNumber][fecChannelNumber] == false)
																							{
																								*out << "<br>";
									    				    					    			*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" <<
																										"<a href=" << switchFecChannelsView_FullUrl.str() << ">+</a>";
																								*out << "&nbsp;(Expand) : FEC Channel number " << fecChannelNumber << directHitsString << " (" << FECCHANNEL_fec << " FEC errors) " << 
																										" ("<< FECCHANNEL_fed << " FED errors) (" << FECCHANNEL_psu << " PSU errors)" << std::endl;
																							}
																							else
																							{
																								*out << "<br>";
									    				    					    			*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" <<
																										"<a href=" << switchFecChannelsView_FullUrl.str() << ">+</a>";
																								*out << "&nbsp;(Hide) : FEC Channel number " << fecChannelNumber << directHitsString << " (" << FECCHANNEL_fec << " FEC errors) " << 
																										" ("<< FECCHANNEL_fed << " FED errors) (" << FECCHANNEL_psu << " PSU errors)" << std::endl;



																								////////////////////////////////////////////////////////////////////////
																								//Manage FEC CHANNELS View Block
																								////////////////////////////////////////////////////////////////////////
																								bool oneFecI2cAddressFound = false;

																								//Find which I2C addresses are in use for a given I2C channel
																								errorsChecker->getFecI2cAddressListForOneChannel(fecI2cAddrList, tkDiagErrorAnalyser, fecCrateNumber, fecSlotNumber, fecPmcNumber, fecCcuNumber, fecChannelNumber);

																								for (unsigned int fecI2cAddressNumber=0; fecI2cAddressNumber<NUMBER_OF_FEC_I2CADDR; fecI2cAddressNumber++)
																								{


																									//Display unused I2C channels probed by FecSoftware
																									if (fecI2cAddrList[fecI2cAddressNumber]==false)
																									{
																										try {
																										fecNodeErrors = errorsTreeBuilder->getFecNodeError(fecCrateNumber, fecSlotNumber, fecPmcNumber, fecCcuNumber, fecChannelNumber, fecI2cAddressNumber);
																										}
																										catch (std::string){}
																										if (fecNodeErrors != 0)
																										{
																											*out << "<br>" << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" <<
																													"<span style=\"color: rgb(0, 0, 210);\">" <<
																													". Detection of temptative to probe missing hardware part : I2C device at address " << fecI2cAddressNumber << 
																													"</span>" << std::endl;
																										}
																									}


																									if (fecI2cAddrList[fecI2cAddressNumber]==true)
																									{
						
																										//Count how many errors we have on one specific slot
																										unsigned int FECI2CADDRESS_fec=0, FECI2CADDRESS_fed=0, FECI2CADDRESS_psu=0;
																										errorsChecker->getFecViewErrorsForI2cAddress(	&FECI2CADDRESS_fec,
																														&FECI2CADDRESS_fed,
																														&FECI2CADDRESS_psu,
																														tkDiagErrorAnalyser,
																														fecCrateNumber,
																														fecSlotNumber,
																														fecPmcNumber,
																														fecCcuNumber,
																														fecChannelNumber,
																														fecI2cAddressNumber);


																										//Get FEC I2C address level node
																										fecNodeErrors = errorsTreeBuilder->getFecNodeError(fecCrateNumber, fecSlotNumber, fecPmcNumber, fecCcuNumber, fecChannelNumber, fecI2cAddressNumber);
																										snprintf(convertToAscii, 9 , "%lu", fecNodeErrors);
																										directHitsString = "";
																										if (fecNodeErrors == 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hit</span>)";
																										else if (fecNodeErrors > 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hits</span>)";

																										if (((FECI2CADDRESS_fec || FECI2CADDRESS_fed || FECI2CADDRESS_psu) && (!displayNonFaultyItems_)) || (displayNonFaultyItems_))
																										{
																											oneFecI2cAddressFound = true;
																											int i2cAddr = 0;
																											if (fecI2cAddressNumber == 0) i2cAddr=0x20;
																											else if (fecI2cAddressNumber == 1) i2cAddr=0x22;
																											else if (fecI2cAddressNumber == 2) i2cAddr=0x24;

																											*out << "<br>";
														    				    					   		*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
																											*out << "&nbsp;.I2C address " << i2cAddr << directHitsString << " (" << FECI2CADDRESS_fec << " FEC errors) " << 
																												" ("<< FECI2CADDRESS_fed << " FED errors) (" << FECI2CADDRESS_psu << " PSU errors)" << std::endl;
																										}
																									}
																								}
								
																							}
																						}
																					}//End of if Channel number X exists
																				}//End of loop on Channel numbers
																				if ((oneFecChannelFound == false) && (!displayNonFaultyItems_))
																				{
																					*out << "<br>";
																					*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
																					*out << ". No faulty FEC CHANNEL found";
																				}
																			}
																		}
																	}//End of if CCU number X exists
																}//End of loop on CCU numbers
																if ((oneFecCcuFound == false) && (!displayNonFaultyItems_))
																{
																	*out << "<br>";
																	*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
																	*out << ". No faulty FEC CCU found";
																}
															}
														}
													}//End of if PMC number X exists
												}//End of loop on PMC numbers
												if ((oneFecPmcFound == false) && (!displayNonFaultyItems_))
												{
													*out << "<br>";
													*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
													*out << ". No faulty FEC PMC found";
												}
											}
										}
									}//End of loop slot per slot
								}//End of SLOT per SLOT processing
								if ((oneFecSlotFound == false) && (!displayNonFaultyItems_))
								{
									*out << "<br>";
									*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
									*out << ". No faulty FEC SLOT found";
								}
							}//End of loop fec per fec
						}
					}//End of FEC per FEC processing
				} //Ferme le FORCRATE
				if ((oneFecCrateFound == false) && (!displayNonFaultyItems_))
				{
					*out << "<br>";
					*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
					*out << ". No faulty FEC CRATE found";
				}
			} //ferme le //ELSEBLOCK
	



			////////////////////////////////////////////////////////////////////////
			//Manage FED View Block
			////////////////////////////////////////////////////////////////////////
			fedNodeErrors = errorsTreeBuilder->getFedNodeError();
			snprintf(convertToAscii, 9 , "%lu", fedNodeErrors);
			directHitsString = "";
			if (fedNodeErrors == 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hit</span>)";
			else if (fedNodeErrors > 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hits</span>)";


       	    std::string switchAllFedsView_ = "/";
			switchAllFedsView_ += getApplicationDescriptor()->getURN();
			switchAllFedsView_ += "/switchAllFedsView";
			if (allFedsShowDetails_ == false)//IFBLOCK
			{
				*out << "<br><br>";
       		    *out << "&nbsp;&nbsp;&nbsp;&nbsp;<a href=" << switchAllFedsView_ << ">+</a>";
				*out << "&nbsp;Display FED views of errors" <<
					directHitsString <<
					" (" << FED_fec << " FEC errors)" <<
					" (" << FED_fed << " FED errors)" <<
					" (" << FED_psu << " PSU errors)" << std::endl;
			}
			else//ELSEBLOCK
			{
				*out << "<br><br>";
       		    *out << "&nbsp;&nbsp;&nbsp;&nbsp;<a href=" << switchAllFedsView_ << ">-</a>";
				*out << "&nbsp;Hide FED views of errors" <<
					directHitsString <<
					" (" << FED_fec << " FEC errors)" <<
					" (" << FED_fed << " FED errors)" <<
					" (" << FED_psu << " PSU errors)" << std::endl;


				////////////////////////////////////////////////////////////////////////
				//Manage FED CRATE View Block
				////////////////////////////////////////////////////////////////////////
				//Find which fed crates exists in the current configuration
				errorsChecker->getFedCratesList(fedCratesList, tkDiagErrorAnalyser);

				//For Each of these crates holding FEDs
				bool oneFedCrateFound = false;
				for (unsigned int fedCrateNumber=0; fedCrateNumber<NUMBER_OF_FED_CRATES; fedCrateNumber++) //FORCRATE
				{
					if (fedCratesList[fedCrateNumber]==true)
					{

						//Count how many errors we have on one specific crate
						unsigned int FEDCRATE_fec=0, FEDCRATE_fed=0, FEDCRATE_psu=0;
						errorsChecker->getFedViewErrorsForCrate( &FEDCRATE_fec,
																	&FEDCRATE_fed,
																	&FEDCRATE_psu,
																	tkDiagErrorAnalyser,
																	fedCrateNumber);

/*
						//count how many errors we can sum up on the FEDs attached to this FEC crate
						unsigned int FECCRATES_fec=0, FECCRATES_fed=0, FECCRATES_psu=0;
						errorsChecker->getFecErrorsForOneFedCrate( &FECCRATES_fec,
																	&FECCRATES_fed,
																	&FECCRATES_psu,
																	tkDiagErrorAnalyser,
																	fedCrateNumber);
						

						//count how many errors we can sum up on the PSUs attached to this FEC crate
						unsigned int PSUCRATES_fec=0, PSUCRATES_fed=0, PSUCRATES_psu=0;
						errorsChecker->getPsuErrorsForOneFedCrate( &PSUCRATES_fec,
																	&PSUCRATES_fed,
																	&PSUCRATES_psu,
																	tkDiagErrorAnalyser,
																	fedCrateNumber);
*/

						////////////////////////////////////////////////////////////////////////
						//Build Output
						////////////////////////////////////////////////////////////////////////
						fedNodeErrors = errorsTreeBuilder->getFedNodeError(fedCrateNumber);
						snprintf(convertToAscii, 9 , "%lu", fedNodeErrors);
						directHitsString = "";
						if (fedNodeErrors == 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hit</span>)";
						else if (fedNodeErrors > 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hits</span>)";


			            std::string switchFedCratesView_ = "/";
						switchFedCratesView_ += getApplicationDescriptor()->getURN();
						switchFedCratesView_ += "/switchFedCratesView";
							
						std::ostringstream switchFedCratesView_FullUrl;
						switchFedCratesView_FullUrl << switchFedCratesView_ << "?FEDCRATENUM=" << fedCrateNumber << "&";
						if (((FEDCRATE_fec || FEDCRATE_fed || FEDCRATE_psu) && (!displayNonFaultyItems_)) || (displayNonFaultyItems_))
						{
							oneFedCrateFound = true;
							if (fedCratesShowDetails_[fedCrateNumber] == false)
							{
								*out << "<br>";
		    	    		    *out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" <<
										"<a href=" << switchFedCratesView_FullUrl.str() << ">+</a>";
								*out << "&nbsp;(Expand) : FED Crate number " << fedCrateNumber << directHitsString << " (" << FEDCRATE_fec << " FEC errors) " << 
										" ("<< FEDCRATE_fed << " FED errors) (" << FEDCRATE_psu << " PSU errors)" << std::endl;
							}
							else
							{
								*out << "<br>";
		    	    		    *out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" <<
										"<a href=" << switchFedCratesView_FullUrl.str() << ">-</a>";
								*out << "&nbsp;(Hide) : FED Crate number " << fedCrateNumber << directHitsString << " (" << FEDCRATE_fec << " FEC errors) " << 
										" ("<< FEDCRATE_fed << " FED errors) (" << FEDCRATE_psu << " PSU errors)" << std::endl;

								////////////////////////////////////////////////////////////////////////
								//Manage FEC SLOT View Block
								////////////////////////////////////////////////////////////////////////
								//Find which fec slots are in use for a given crate
								errorsChecker->getFedSlotsListForOneCrate(fedSlotsList, tkDiagErrorAnalyser, fedCrateNumber);

								bool oneFedSlotFound = false;
								//For Each of these Slots holding one FED
								for (unsigned int fedSlotNumber=0; fedSlotNumber<NUMBER_OF_FED_SLOTS; fedSlotNumber++)
								{
									if (fedSlotsList[fedSlotNumber]==true)
									{

										//Count how many errors we have on one specific slot
										unsigned int FEDSLOT_fec=0, FEDSLOT_fed=0, FEDSLOT_psu=0;
										unsigned int lclFedId;
										errorsChecker->getFedViewErrorsForSlot( &FEDSLOT_fec,
																				&FEDSLOT_fed,
																				&FEDSLOT_psu,
																				tkDiagErrorAnalyser,
																				fedCrateNumber,
																				fedSlotNumber,
																				&lclFedId);
/*
										//count how many errors we can sum up on the FEDs attached to this FEC crate
										unsigned int FECSLOTS_fec=0, FECSLOTS_fed=0, FECSLOTS_psu=0;
										errorsChecker->getFecErrorsForOneFedSlot( &FECSLOTS_fec,
																					&FECSLOTS_fed,
																					&FECSLOTS_psu,
																					tkDiagErrorAnalyser,
																					fedCrateNumber,
																					fedSlotNumber);
						

										//count how many errors we can sum up on the PSUs attached to this FEC crate
										unsigned int PSUSLOTS_fec=0, PSUSLOTS_fed=0, PSUSLOTS_psu=0;
										errorsChecker->getPsuErrorsForOneFedSlot( &PSUSLOTS_fec,
																					&PSUSLOTS_fed,
																					&PSUSLOTS_psu,
																					tkDiagErrorAnalyser,
																					fedCrateNumber,
																					fedSlotNumber);
*/

										////////////////////////////////////////////////////////////////////////
										//Build Output
										////////////////////////////////////////////////////////////////////////
										fedNodeErrors = errorsTreeBuilder->getFedNodeError(fedCrateNumber, fedSlotNumber);
										snprintf(convertToAscii, 9 , "%lu", fedNodeErrors);
										directHitsString = "";
										if (fedNodeErrors == 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hit</span>)";
										else if (fedNodeErrors > 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hits</span>)";

							            std::string switchFedSlotsView_ = "/";
										switchFedSlotsView_ += getApplicationDescriptor()->getURN();
										switchFedSlotsView_ += "/switchFedSlotsView";
							
										std::ostringstream switchFedSlotsView_FullUrl;
										switchFedSlotsView_FullUrl << switchFedSlotsView_ << "?FEDCRATENUM=" << fedCrateNumber << "&" << "FEDSLOTNUM=" << fedSlotNumber;

										if (((FEDSLOT_fec || FEDSLOT_fed || FEDSLOT_psu) && (!displayNonFaultyItems_)) || (displayNonFaultyItems_))
										{
											oneFedSlotFound = true;

											if (fedSlotItemShowDetails_[fedCrateNumber][fedSlotNumber] == false)
											{
												*out << "<br>";
		    	    		    				*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << 
														"<a href=" << switchFedSlotsView_FullUrl.str() << ">+</a>";
												*out << "&nbsp;(Expand) : FED (id:" << lclFedId << ") in slot number " << fedSlotNumber << directHitsString << " (" << FEDSLOT_fec << " FEC errors) " << 
														" ("<< FEDSLOT_fed << " FED errors) (" << FEDSLOT_psu << " PSU errors)" << std::endl;
											}
											else
											{
												*out << "<br>";
	    	    			    				*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" << 
														"<a href=" << switchFedSlotsView_FullUrl.str() << ">-</a>";
												*out << "&nbsp;(Hide) : FED (id:" << lclFedId << ") in slot number " << fedSlotNumber << directHitsString << " (" << FEDSLOT_fec << " FEC errors) " << 
														" ("<< FEDSLOT_fed << " FED errors) (" << FEDSLOT_psu << " PSU errors)" << std::endl;
	
												////////////////////////////////////////////////////////////////////////
												//Manage FED Connection View Block
												////////////////////////////////////////////////////////////////////////
												//Find which fec slots are in use for a given crate
												errorsChecker->getFedChannelsListForOneSlot(fedChannelsList, tkDiagErrorAnalyser, fedCrateNumber, fedSlotNumber);

												bool oneFedChannelFound = false;
												//For Each of these Slots holding one FEC, loop on pmc's
												for (unsigned int fedChannelNumber=0; fedChannelNumber<NUMBER_OF_FED_CHANNELS; fedChannelNumber++)
												{
													if (fedChannelsList[fedChannelNumber]==true)
													{

														//Count how many errors we have on one specific slot
														unsigned int FEDPMC_fec=0, FEDPMC_fed=0, FEDPMC_psu=0;
														errorsChecker->getFedViewErrorsForChannel(	&FEDPMC_fec,
																							&FEDPMC_fed,
																							&FEDPMC_psu,
																							tkDiagErrorAnalyser,
																							fedCrateNumber,
																							fedSlotNumber,
																							fedChannelNumber);
	/*
														//count how many errors we can sum up on the FEDs attached to this FEC crate
														unsigned int FECPMC_fec=0, FECPMC_fed=0, FECPMC_psu=0;
														errorsChecker->getFecErrorsForOneFedChannel(	&FECPMC_fec,
																								&FECPMC_fed,
																								&FECPMC_psu,
																								tkDiagErrorAnalyser,
																								fedCrateNumber,
																								fedSlotNumber,
																								fedChannelNumber);
							
														//count how many errors we can sum up on the PSUs attached to this FEC crate
														unsigned int PSUPMC_fec=0, PSUPMC_fed=0, PSUPMC_psu=0;
														errorsChecker->getPsuErrorsForOneFedChannel(	&PSUPMC_fec,
																								&PSUPMC_fed,
																								&PSUPMC_psu,
																								tkDiagErrorAnalyser,
																								fedCrateNumber,
																								fedSlotNumber,
																								fedChannelNumber);
*/
														//Build Output
														fedNodeErrors = errorsTreeBuilder->getFedNodeError(fedCrateNumber, fedSlotNumber, fedChannelNumber);
														snprintf(convertToAscii, 9 , "%lu", fedNodeErrors);
														directHitsString = "";
														if (fedNodeErrors == 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hit</span>)";
														else if (fedNodeErrors > 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hits</span>)";

														if (((FEDPMC_fec || FEDPMC_fed || FEDPMC_psu) && (!displayNonFaultyItems_)) || (displayNonFaultyItems_))
														{
															oneFedChannelFound = true;
															*out << "<br>";
		    	    					    				*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
															*out << ". FED Channel number " << fedChannelNumber << directHitsString << " (" << FEDPMC_fec << " FEC errors) " << 
																	" ("<< FEDPMC_fed << " FED errors) (" << FEDPMC_psu << " PSU errors)" << std::endl;
														}
													}//end of IF fed connection holds something
												}//End of loop on FED connections
												if ((oneFedChannelFound == false) && (!displayNonFaultyItems_))
												{
													*out << "<br>";
													*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
													*out << ". No faulty FED CHANNEL found";	
												}
											}//ELSE-SLOT END
										}
									}//end of IF fed slot holds something
								}//End of loop on FED slots
								if ((oneFedSlotFound == false) && (!displayNonFaultyItems_))
								{
									*out << "<br>";
									*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
									*out << ". No faulty FED SLOT found";
								}	
							}//ELSE-CRATE END
						}
					}//end of IF fed crate holds something
				}//End of loop on FED crates
				if ((oneFedCrateFound == false) && (!displayNonFaultyItems_))
				{
					*out << "<br>";
					*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
					*out << ". No faulty FED CRATE found";
				}
			}//ELSE-BLOCK END




			////////////////////////////////////////////////////////////////////////
			//Manage PSU View Block
			////////////////////////////////////////////////////////////////////////
			psuNodeErrors = errorsTreeBuilder->getPsuNodeError();
			snprintf(convertToAscii, 9 , "%lu", psuNodeErrors);
			directHitsString = "";
			if (psuNodeErrors == 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hit</span>)";
			else if (psuNodeErrors > 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hits</span>)";

       	    std::string switchAllPsusView_ = "/";
			switchAllPsusView_ += getApplicationDescriptor()->getURN();
			switchAllPsusView_ += "/switchAllPsusView";
			if (allPsusShowDetails_ == false)//IFBLOCK
			{
				*out << "<br><br>";
       		    *out << "&nbsp;&nbsp;&nbsp;&nbsp;<a href=" << switchAllPsusView_ << ">+</a>";
				*out << "&nbsp;Display PSU views of errors" <<
					directHitsString <<
					" (" << PSU_fec << " FEC errors)" <<
					" (" << PSU_fed << " FED errors)" <<
					" (" << PSU_psu << " PSU errors)" << std::endl;
			}
			else//ELSEBLOCK
			{
				*out << "<br><br>";
       		    *out << "&nbsp;&nbsp;&nbsp;&nbsp;<a href=" << switchAllPsusView_ << ">-</a>";
				*out << "&nbsp;Hide PSU views of errors" <<
					directHitsString <<
					" (" << PSU_fec << " FEC errors)" <<
					" (" << PSU_fed << " FED errors)" <<
					" (" << PSU_psu << " PSU errors)" << std::endl;

				////////////////////////////////////////////////////////////////////////
				//Manage PSU DETID View Block
				////////////////////////////////////////////////////////////////////////
				bool onePsuDetidFound = false;
			    std::vector<unsigned int> listDetId = tkDiagErrorAnalyser->getListOfDetId();
			    for (std::vector<unsigned int>::iterator it = listDetId.begin() ; it != listDetId.end() ; it ++)
				{
					unsigned int fecErrorCounter=0, fedErrorCounter=0, psuErrorCounter=0;
					tkDiagErrorAnalyser->getDetIdErrorCounter(*it,fecErrorCounter,fedErrorCounter,psuErrorCounter) ;
					if (((fecErrorCounter || fedErrorCounter || psuErrorCounter) && (!displayNonFaultyItems_)) || (displayNonFaultyItems_))
					{

						unsigned int psuRank = errorsChecker->getPsuIdRange((*it), tkDiagErrorAnalyser);

						psuNodeErrors = errorsTreeBuilder->getPsuNodeError(psuRank);
						snprintf(convertToAscii, 9 , "%lu", psuNodeErrors);
						directHitsString = "";
						if (psuNodeErrors == 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hit</span>)";
						else if (psuNodeErrors > 1) directHitsString = (std::string)" (<span style=\"color: rgb(204, 0, 0);\">" + (std::string)convertToAscii + (std::string)" Direct Hits</span>)";

						*out << "<br>";
						*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
						*out << ". PSU DETID number " << (*it) << directHitsString << " (" << fecErrorCounter << " FEC errors) " << 
							" ("<< fedErrorCounter << " FED errors) (" << psuErrorCounter << " PSU errors)" << std::endl;
							onePsuDetidFound = true;
					}
				}
				if ((onePsuDetidFound == false) && (!displayNonFaultyItems_))
				{
						*out << "<br>";
						*out << "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
						*out << ". No faulty PSU DETID found";
				}

			}
		} //End of Detector management Block
	} //End of Method

#endif




/* xgi method called when the default link is clicked */
void LogReader::Default1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 0;
	Default(in, out);
}

// xgi method called when the link <display_diagsystem> is clicked
void LogReader::callDiagSystemPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 1;
	Default(in, out);
}

// xgi method called when the link <display_fsm> is clicked
void LogReader::callFsmPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 2;
	Default(in, out);
}

// xgi method called when the link <display_logs> is clicked
void LogReader::callVisualizeLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 3;
	Default(in, out);
}

// xgi method called when the link <display_logs> is clicked
void LogReader::callVisualizePreviousLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 4;
	Default(in, out);
}

// xgi method called when the link <display_logs> is clicked
void LogReader::callVisualizeNextLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 5;
	Default(in, out);
}

// xgi method called when the link <display_logs> is clicked
void LogReader::closeFile(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 6;
	Default(in, out);
}

// xgi method called when the link <display_one_app> is clicked
void LogReader::callAppsSpyingPanelPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 7;
	Default(in, out);
}

// xgi method called when the link <display_group_app> is clicked
void LogReader::callGroupSpyingPanelPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 8;
	Default(in, out);
}

// xgi method called when the link <display_worldwide_app> is clicked
void LogReader::callWorldSpyingPanelPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 9;
	Default(in, out);
}

#ifdef DATABASE
// xgi method called when the link <display_ANALYSE_LOGS> is clicked
void LogReader::callLogsSummaryPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 10;
	Default(in, out);
}

void LogReader::callLogsNoDbPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 11;
	Default(in, out);
}

#endif



void LogReader::genericSort(int rowOffset, bool sortGrowing)
{

	std::string Message;
	std::string Origine;
	std::string Timestamp;
	std::string Level;
	std::string Errcode;

	if (sortGrowing == true)
	// Tri croissant
	{
		for (int i=0; i<LOGARRAYLINES; i++)
		{
			for (int j=1; j<LOGARRAYLINES; j++)
			{
				if (copyOfLogsMemory[j-1][rowOffset] > copyOfLogsMemory[j][rowOffset])
				{
					// backup lowest value
					Message = copyOfLogsMemory[j][MSGOFFSET];
					Origine = copyOfLogsMemory[j][ORGOFFSET];
					Timestamp = copyOfLogsMemory[j][DIAGTSOFFSET];
					Level = copyOfLogsMemory[j][LVLOFFSET];
					Errcode = copyOfLogsMemory[j][ECOFFSET];

					// permute values
					copyOfLogsMemory[j][MSGOFFSET] = copyOfLogsMemory[j-1][MSGOFFSET];
					copyOfLogsMemory[j][ORGOFFSET] = copyOfLogsMemory[j-1][ORGOFFSET];
					copyOfLogsMemory[j][DIAGTSOFFSET] = copyOfLogsMemory[j-1][DIAGTSOFFSET];
					copyOfLogsMemory[j][LVLOFFSET] = copyOfLogsMemory[j-1][LVLOFFSET];
					copyOfLogsMemory[j][ECOFFSET] = copyOfLogsMemory[j-1][ECOFFSET];

					// Restore lowest value at right place
					copyOfLogsMemory[j-1][MSGOFFSET] = Message;
					copyOfLogsMemory[j-1][ORGOFFSET] = Origine;
					copyOfLogsMemory[j-1][DIAGTSOFFSET] = Timestamp;
					copyOfLogsMemory[j-1][LVLOFFSET] = Level;
					copyOfLogsMemory[j-1][ECOFFSET] = Errcode;
				}
			}
		}
	}
	else
	{
		for (int i=0; i<LOGARRAYLINES; i++)
		{
			for (int j=1; j<LOGARRAYLINES; j++)
			{
				if (copyOfLogsMemory[j-1][rowOffset] < copyOfLogsMemory[j][rowOffset])
				{
					// backup lowest value
					Message = copyOfLogsMemory[j][MSGOFFSET];
					Origine = copyOfLogsMemory[j][ORGOFFSET];
					Timestamp = copyOfLogsMemory[j][DIAGTSOFFSET];
					Level = copyOfLogsMemory[j][LVLOFFSET];
					Errcode = copyOfLogsMemory[j][ECOFFSET];

					// permute values
					copyOfLogsMemory[j][MSGOFFSET] = copyOfLogsMemory[j-1][MSGOFFSET];
					copyOfLogsMemory[j][ORGOFFSET] = copyOfLogsMemory[j-1][ORGOFFSET];
					copyOfLogsMemory[j][DIAGTSOFFSET] = copyOfLogsMemory[j-1][DIAGTSOFFSET];
					copyOfLogsMemory[j][LVLOFFSET] = copyOfLogsMemory[j-1][LVLOFFSET];
					copyOfLogsMemory[j][ECOFFSET] = copyOfLogsMemory[j-1][ECOFFSET];

					// Restore lowest value at right place
					copyOfLogsMemory[j-1][MSGOFFSET] = Message;
					copyOfLogsMemory[j-1][ORGOFFSET] = Origine;
					copyOfLogsMemory[j-1][DIAGTSOFFSET] = Timestamp;
					copyOfLogsMemory[j-1][LVLOFFSET] = Level;
					copyOfLogsMemory[j-1][ECOFFSET] = Errcode;
				}
			}
		}

	}
        
}


void LogReader::sortMessages(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	processingSortRequest = true;
	genericSort(MSGOFFSET, sortMessagesGrowing);
	if (sortMessagesGrowing == true) {sortMessagesGrowing = false;} else {sortMessagesGrowing = true;}
	Default(in, out);
}


void LogReader::sortOrigin(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	processingSortRequest = true;
	genericSort(ORGOFFSET, sortOriginGrowing);
	if (sortOriginGrowing == true) {sortOriginGrowing = false;} else {sortOriginGrowing = true;}
	Default(in, out);
}

void LogReader::sortTimestamp(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	processingSortRequest = true;
	genericSort(DIAGTSOFFSET, sortTimestampGrowing);
	if (sortTimestampGrowing == true) {sortTimestampGrowing = false;} else {sortTimestampGrowing = true;}
	Default(in, out);
}

void LogReader::sortLevel(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	processingSortRequest = true;
	genericSort(LVLOFFSET, sortLevelGrowing);
	if (sortLevelGrowing == true) {sortLevelGrowing = false;} else {sortLevelGrowing = true;}
	Default(in, out);
}

void LogReader::sortErrcode(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	processingSortRequest = true;
	genericSort(ECOFFSET, sortErrcodeGrowing);
	if (sortErrcodeGrowing == true) {sortErrcodeGrowing = false;} else {sortErrcodeGrowing = true;}
	Default(in, out);
}


void LogReader::autoRefresh(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	cgicc::Cgicc cgi(in);
	textRefreshRate_ = cgi["textRefreshRate"]->getValue();
	refreshRate_ = atoi(textRefreshRate_.c_str());
	if (autoRefreshEnabled_ == false)
	{
		autoRefreshEnabled_ = true;
	}
	else autoRefreshEnabled_ = false;
	Default(in, out);
}




void LogReader::getIFrame(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	// Display Logs in a table
	*out << "<table style=\"width: 100%; text-align: left;\" border=\"1\" cellpadding=\"2\" cellspacing=\"2\">";
	*out << "<tbody> ";

	// Column 0
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
             
	// Column 1
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

	// Column 2
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

	// Column 3
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

	// Column 4
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

	// End Table tag
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
		processingSortRequest = true;
		sortTimestampGrowing = false;
		genericSort(DIAGTSOFFSET, sortTimestampGrowing);
		processingSortRequest = false;
		newLogsFound = false;
	}
	else processingSortRequest = false;



	std::string b_color;
	std::string level;
	for (int i=0; i<LOGARRAYLINES; i++)
	{
		if (copyOfLogsMemory[i][MSGOFFSET] != "UNINITIALIZED_BT_ED")
		{
			level = copyOfLogsMemory[i][LVLOFFSET];
			if (level == DIAGTRACE) b_color = TRACECOLOR;
			if (level == DIAGDEBUG) b_color = DEBUGCOLOR;
			if (level == DIAGINFO) b_color = INFOCOLOR;
			if (level == DIAGWARN) b_color = WARNCOLOR;
			if (level == DIAGUSERINFO) b_color = USERINFOCOLOR;
			if (level == DIAGERROR) b_color = ERRORCOLOR;
			if (level == DIAGFATAL) b_color = FATALCOLOR;
					

			*out << "<tr>";
			*out << "<td style=\"vertical-align: top; background-color: " << b_color << ";\">" << copyOfLogsMemory[i][MSGOFFSET] << "</td>";
			*out << "<td style=\"vertical-align: top; background-color: " << b_color << ";\">" << copyOfLogsMemory[i][ORGOFFSET] << "</td>";
			*out << "<td style=\"vertical-align: top; background-color: " << b_color << ";\">" << copyOfLogsMemory[i][DIAGTSOFFSET] << "</td>";
			*out << "<td style=\"vertical-align: top; background-color: " << b_color << ";\">" << copyOfLogsMemory[i][LVLOFFSET] << "</td>";
			*out << "<td style=\"vertical-align: top; background-color: " << b_color << ";\">" << copyOfLogsMemory[i][ECOFFSET] << "</td>";
			*out << "</tr>";
		}
	}
	*out << "</tbody>";
	*out << "</table>";
}



// Displays the available HyperDaq links for this process
void LogReader::displayLinks(xgi::Input * in, xgi::Output * out)
{
	// Display default page
	std::string urlBase_ = "/";
	urlBase_ += getApplicationDescriptor()->getURN();
	urlBase_ += "/Default1";
	*out << "<a href=" << urlBase_ << ">Default Page</a>";
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";

	// Display diagnostic page
	std::string urlDiag_ = "/";
	urlDiag_ += getApplicationDescriptor()->getURN();
	urlDiag_ += "/callDiagSystemPage";
	*out << "<a href=" << urlDiag_ << ">Configure Logs Reader</a>";
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";

	// Display FSM page
	std::string urlFsm_ = "/";
	urlFsm_ += getApplicationDescriptor()->getURN();
	urlFsm_ += "/callFsmPage";
	*out << "<a href=" << urlFsm_ << ">FSM Access</a>";
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";


	// Base display for received logs
	std::string urlVisu_ = "/";
	urlVisu_ += getApplicationDescriptor()->getURN();
	urlVisu_ += "/callVisualizeLogsPage";
	*out << "<a href=" << urlVisu_ << ">Read/Refresh received Logs</a>";
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";


	// browse backward in file of received logs
	std::string urlPreviousVisu_ = "/";
	urlPreviousVisu_ += getApplicationDescriptor()->getURN();
	urlPreviousVisu_ += "/callVisualizePreviousLogsPage";
	*out << "<a href=" << urlPreviousVisu_ << ">See Previous Logs</a>";
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";


	// browse forward in file of received logs
	std::string urlNextVisu_ = "/";
	urlNextVisu_ += getApplicationDescriptor()->getURN();
	urlNextVisu_ += "/callVisualizeNextLogsPage";
	*out << "<a href=" << urlNextVisu_ << ">See Next Logs</a>";
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";


	// Reset log file pointer
	std::string closeLogFile_ = "/";
	closeLogFile_ += getApplicationDescriptor()->getURN();
	closeLogFile_ += "/closeFile";
	*out << "<a href=" << closeLogFile_ << ">Re-initialize file I/O</a>";
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";

	// Display single application list
	std::string callAppsPanel_ = "/";
	callAppsPanel_ += getApplicationDescriptor()->getURN();
	callAppsPanel_ += "/callAppsSpyingPanelPage";
	*out << "<a href=" << callAppsPanel_ << ">Single Application Control Panel</a>";
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";


	// Display group application list
	std::string callGroupsPanel_ = "/";
	callGroupsPanel_ += getApplicationDescriptor()->getURN();
	callGroupsPanel_ += "/callGroupSpyingPanelPage";
	*out << "<a href=" << callGroupsPanel_ << ">Grouped Applications Control Panel</a>";
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";


	// Display worldwide application list
	std::string callWorldPanel_ = "/";
	callWorldPanel_ += getApplicationDescriptor()->getURN();
	callWorldPanel_ += "/callWorldSpyingPanelPage";
	*out << "<a href=" << callWorldPanel_ << ">WorldWide Applications Control Panel</a>";
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";

	#ifdef DATABASE
		// Display logs analysis application list
		std::string urlSummary_ = "/";
		urlSummary_ += getApplicationDescriptor()->getURN();
		urlSummary_ += "/callLogsSummaryPage";

		std::string urlNoDb_ = "/";
		urlNoDb_ += getApplicationDescriptor()->getURN();
		urlNoDb_ += "/callLogsNoDbPage";

		if (isDbUsable == true)
		{
			*out << "<a href=" << urlSummary_ << ">View Hardware Catgorized Logs</a>";
		}
		else *out << "<a href=" << urlNoDb_ << ">View Hardware Catgorized Logs</a>";
	#endif
	*out << "<br>";
	*out << "<br>";
}




// Default() hyperDaq method
void LogReader::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	// Create HTML header
	*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
	*out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
	*out << "<HEAD>" << std::endl;
	*out << cgicc::title("Logs Reader") << std::endl;
	if ( (autoRefreshEnabled_ == true) && (internalState_ == 3) )
	{
		*out << "<META HTTP-EQUIV=\"Refresh\" CONTENT=\"";
		*out << textRefreshRate_;
		*out << "\">" << std::endl;
	}
	*out << "</HEAD>" << std::endl;
	xgi::Utils::getPageHeader(  out, 
								"Logs Reader", 
								getApplicationDescriptor()->getContextDescriptor()->getURL(),
								getApplicationDescriptor()->getURN(),
								"/daq/xgi/images/Application.jpg" );

	// Display available links on top of the HTML page
	displayLinks(in, out);

	// Get page HTML content according to the selected link
	if (internalState_ == 0) getDefault1Content(in, out);
	if (internalState_ == 1) diagService_->getDiagSystemHtmlPage(in, out,getApplicationDescriptor()->getURN());
	if (internalState_ == 2) getFsmStatePage(in, out);
	if (internalState_ == 3) getVisualizeLogsPage(in, out);
	if (internalState_ == 4) getVisualizeLogsPage(in, out);
	if (internalState_ == 5) getVisualizeLogsPage(in, out);
	if (internalState_ == 6)
	{
		if ( (fileoutIsOpen_ == true) && (fileOut_ != NULL) ) fclose(fileOut_);
		fileoutIsOpen_ = false;
	}
	if (internalState_ == 7) getAppsPanelPage(in, out);
	if (internalState_ == 8) getGroupsPanelPage(in, out);
	if (internalState_ == 9) getWorldPanelPage(in, out);
	#ifdef DATABASE
		if (internalState_ == 10) getLogsSummaryPage(in, out);
		if (internalState_ == 11) getLogsNoDbPage(in, out);
	#endif

	// Create HTML footer
	xgi::Utils::getPageFooter(*out);
}







// Turns the LogReader into state INITIALISED (FSM point of view)
void LogReader::configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
	// Apply fireitems value to internal variables and check validity
	DIAG_EXEC_FSM_INIT_TRANS
}



// Turns the LogReader into state HALTED (FSM point of view)
void LogReader::haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
	// Apply fireitems value to internal variables and check validity
	DIAG_EXEC_FSM_STOP_TRANS
}
   

   
// Allows the FSM to be controlled via a soap message carrying a "fireEvent" message
xoap::MessageReference LogReader::fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception)
{
	//#define DEBUG_FIREEVENT
	#ifdef DEBUG_FIREEVENT
		std::cout << "In LogReader : fireEvent received" << std::endl ;
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


// Called when a state transition fails in the FSM
void LogReader::failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
	toolbox::fsm::FailedEvent & fe = dynamic_cast<toolbox::fsm::FailedEvent&>(*e);
	std::ostringstream msgError;
	msgError << "Failure occurred when performing transition from: " << fe.getFromState() << " to: " << fe.getToState() << " exception: " << fe.getException().what();
	diagService_->reportError (msgError.str(), DIAGERROR);
}



// Can be called via the <configure_FSM> hyperdaq HTML page of this process
void LogReader::configureStateMachine(xgi::Input * in, xgi::Output * out)
{
	if (fsm_.getCurrentState() == 'H')
	{
		toolbox::Event::Reference e(new toolbox::Event(EXECTRANSHC, this));
		fsm_.fireEvent(e);
	}
	Default(in, out);
}


// Can be called via the <configure_FSM> hyperdaq HTML page of this process
void LogReader::stopStateMachine(xgi::Input * in, xgi::Output * out)
{
	if (fsm_.getCurrentState() == 'C')
	{
		toolbox::Event::Reference e(new toolbox::Event(EXECTRANSCH, this));
		fsm_.fireEvent(e);
	}
	Default(in, out);
}


#ifdef AUTO_CONFIGURE_PROCESSES
void LogReader::timeExpired (toolbox::task::TimerEvent& e) 
{
	DIAG_EXEC_FSM_INIT_TRANS
	std::cout << "LogReader(LID=" << getApplicationDescriptor()->getLocalId() << ",Instance=" << getApplicationDescriptor()->getInstance() << ") ready" << std::endl;
}
#endif





std::string LogReader::cleanField(std::string& rawField, std::string& tag)
{
	std::string::size_type loc;
	std::string cleanedField;
	loc = rawField.find( tag, 0 );
	if( loc != std::string::npos )
	{
		cleanedField = rawField.substr(tag.length()+1, rawField.length()-tag.length()-tag.length()-3);
	}
	else cleanedField="UNINITIALIZED_BT_ED";
	return cleanedField;
}






void LogReader::readLogsFromFile(int whereFrom)
{
	if (diagService_->useFileLogging_ == false)
	{
		//std::cout << "LogReader requested to work with automatic log file detection" << std::endl;
		//Get logFileName in use from GlobalErrorDispatcher
		// Build SOAP for sending action
		xoap::MessageReference err_msg = xoap::createMessage();
		xoap::SOAPPart err_soap = err_msg->getSOAPPart();
		xoap::SOAPEnvelope err_envelope = err_soap.getEnvelope();
		xoap::SOAPName command = err_envelope.createName("getCurrentLogFileName", "xdaq", XDAQ_NS_URI);
		xoap::SOAPBody err_body = err_envelope.getBody();
		err_body.addBodyElement(command);

		std::string lclLogFileName;
		xoap::MessageReference reply;
		try
		{
			xdaq::ApplicationDescriptor * err_d = getApplicationContext()->getDefaultZone()->getApplicationDescriptor("GlobalErrorDispatcher",0);
            reply = getApplicationContext()->postSOAP(err_msg, *getApplicationDescriptor(), *err_d);

			xoap::SOAPBody mb = reply->getSOAPPart().getEnvelope().getBody();
			std::vector<xoap::SOAPElement> paramsList = mb.getChildElements ();
			if(paramsList.size()==1)
			{
				xoap::SOAPName logFileName ("logFileName", "", "");
				lclLogFileName = paramsList[0].getAttributeValue(logFileName);
			}
		}
		catch (xdaq::exception::Exception& e)
		{
			lclLogFileName = "";
		}
		diagService_->setFilePathAndName(lclLogFileName);

	}
	diagService_->checkLrFileOption();
	// else std::cout << "LogReader requested to work with manual log file specification" << std::endl;

	if (diagService_->logFileNameHasChanged_ == true)
	{
		if (fileOut_ != NULL) fclose(fileOut_);
		fileoutIsOpen_ = false;
		diagService_->logFileNameHasChanged_ = false;
	}

	if (fileoutIsOpen_ == false)
	{
		fileOut_ = fopen(diagService_->logFileName_.c_str(), "r");
		if (fileOut_ != NULL)
		{
			fileoutIsOpen_ = true;
			currentLogNumber = 0;
			arrayDesc = 0;
			lastKnownPos = 0;
			lastKnownGoodPos = lastKnownPos;
			for (int logarrayinitialiser=0; logarrayinitialiser < LOGARRAYLINES ; logarrayinitialiser++)
			{
				logsMemory[logarrayinitialiser][MSGOFFSET] = "UNINITIALIZED_BT_ED";
				copyOfLogsMemory[logarrayinitialiser][MSGOFFSET] = "UNINITIALIZED_BT_ED";
			}
		}
		else
		{
			std::cout << "Unable to read from log file \"" << diagService_->logFileName_.c_str() << "\"" << std::endl;
		}
	}




	if (fileOut_ != NULL)
	{
		char oneLine[1024];
		if (whereFrom == 0)
		{
			//std::cout << "Display REGULAR LOG" << std::endl;
			rewind(fileOut_);
			fseek(fileOut_, lastKnownGoodPos, SEEK_SET);
			while (!(feof(fileOut_)))
			{

				std::string logNumber;

				std::string rawField;
				std::string tag;

				std::string fecHardId;
				std::string ring;
				std::string ccu;
				std::string i2cChannel;
				std::string i2cAddress;
				std::string fedId;
				std::string fedChannel;
				std::string trigger;
				std::string psuId;
				std::string software;

				//read <DATA> line
    	    	fgets(oneLine, 1023, fileOut_);

				//Read <LOGNUMBER>
   	    	    fgets(oneLine, 1023, fileOut_);

				//Read <LEVEL>
   	    	    fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<LEVEL>";
   	    	    logsMemory[arrayDesc][LVLOFFSET] = cleanField(rawField, tag);

				//Read <ERR_CODE>
   	    	    fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<ERR_CODE>";
   	    	    logsMemory[arrayDesc][ECOFFSET] = cleanField(rawField, tag);

				//Read <ERR_MSG>
   	    	    fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<ERR_MSG>";
   	    	    logsMemory[arrayDesc][MSGOFFSET] = cleanField(rawField, tag);

				//Read <ERR_TIMESTAMP>
   	    	    fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<ERR_TIMESTAMP>";
   	    	    logsMemory[arrayDesc][DIAGTSOFFSET] = cleanField(rawField, tag);

				//Read <ERR_UID>
   	    	    fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<ERR_UID>";
   	    	    logsMemory[arrayDesc][ORGOFFSET] = cleanField(rawField, tag);

				//Read tags in range [<RUN_TYPE> .. <ERR_NBR_INSTANCE>]
				for (int i=0; i<3; i++) fgets(oneLine, 1023, fileOut_);

				//Read <FECHARDID>
   	    	    fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<FECHARDID>";
   	    	    fecHardId = cleanField(rawField, tag);

				//Read <RING>
   	    	    fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<RING>";
   	    	    ring = cleanField(rawField, tag);

				//Read <CCU>
   	    	    fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<CCU>";
   	    	    ccu = cleanField(rawField, tag);

				//Read <I2CCHANNEL>
   	    	    fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<I2CCHANNEL>";
   	    	    i2cChannel = cleanField(rawField, tag);

				//Read <I2CADDRESS>
   	    	    fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<I2CADDRESS>";
   	    	    i2cAddress = cleanField(rawField, tag);

				//Read tag <SOFTWARETAGVERSION>
				fgets(oneLine, 1023, fileOut_);


				//Read <FEDID>
   	    	    fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<FEDID>";
   	    	    fedId = cleanField(rawField, tag);

				//Read <FEDCHANNEL>
   	    	    fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<FEDCHANNEL>";
   	    	    fedChannel = cleanField(rawField, tag);




				//Read <TRIGGER>
   	    	    fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<TRIGGER>";
   	    	    trigger = cleanField(rawField, tag);



				//Read <PSUID>
   	    	    fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<PSUID>";
   	    	    psuId = cleanField(rawField, tag);



				//Read <SOFTWARE>
   	    	    fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<SOFTWARE>";
   	    	    software = cleanField(rawField, tag);


				//Read </DATA>
   	    	    fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;



/*
				std::string fecHardId;
				std::string ring;
				std::string ccu;
				std::string i2cChannel;
				std::string i2cAddress;
				std::string fedId;
				std::string fedChannel;
				std::string trigger;
				std::string psuId;
				std::string software;
*/


/*

								errorsTreeBuilder->setFecNodeError( crateSlot,
																	fecSlot,
																	(unsigned int)atoi(simulatedFecRing_.c_str()),
																	(unsigned int)atoi(simulatedCcuAddress_.c_str()),
																	(unsigned int)atoi(simulatedI2cChannel_.c_str()),
																	(unsigned int)atoi(simulatedI2cAddress_.c_str()) );
*/

		    	if ( (feof(fileOut_)) || (strcmp(oneLine,"</DATA>\n") != 0) )
   		    	{
   	    	    	logsMemory[arrayDesc][MSGOFFSET] = "UNINITIALIZED_BT_ED";
	    	    	logsMemory[arrayDesc][ORGOFFSET] = "UNINITIALIZED_BT_ED";
        	    	logsMemory[arrayDesc][DIAGTSOFFSET] = "UNINITIALIZED_BT_ED";
    		    	logsMemory[arrayDesc][LVLOFFSET] = "UNINITIALIZED_BT_ED";
    	        	logsMemory[arrayDesc][ECOFFSET] = "UNINITIALIZED_BT_ED";
		    	}
   		    	else
    	    	{
   	    	    	lastKnownGoodPos = ftell(fileOut_);
   	    	    	arrayDesc++;
   	    	    	currentLogNumber++;
   	    	    	if (arrayDesc==LOGARRAYLINES) arrayDesc=0;
					
					//Analyse logs content to raise a hardware error if needed

	#ifdef DATABASE

if (isDbUsable == true)
{

					//Analyse FEC HARDWARE ERRORS
					if (fecHardId != "NA")
					{
						unsigned int crateSlot;
						unsigned int fecSlot;
						errorsChecker->getCrateAndFecSlotsForFechardid( &crateSlot, &fecSlot, tkDiagErrorAnalyser, fecHardId);
						if (ring != "NA")
						{
							if (ccu != "NA")
							{
								if (i2cChannel != "NA")
								{				
									if (i2cAddress != "NA")
									{
										errorsTreeBuilder->setFecNodeError( crateSlot, fecSlot, (unsigned int)atoi(ring.c_str()),
																								(unsigned int)atoi(ccu.c_str()),
																								(unsigned int)atoi(i2cChannel.c_str()),
																								(unsigned int)atoi(i2cAddress.c_str()) );
										tkDiagErrorAnalyser->setFecError(	fecHardId,
																			(unsigned int)atoi(ring.c_str()),
																			(unsigned int)atoi(ccu.c_str()),
																			(unsigned int)atoi(i2cChannel.c_str()),
																			(unsigned int)atoi(i2cAddress.c_str()) );
									}
									else
									{
										errorsTreeBuilder->setFecNodeError( crateSlot, fecSlot, (unsigned int)atoi(ring.c_str()),
																								(unsigned int)atoi(ccu.c_str()),
																								(unsigned int)atoi(i2cChannel.c_str()) );
										tkDiagErrorAnalyser->setFecError(	fecHardId,
																			(unsigned int)atoi(ring.c_str()),
																			(unsigned int)atoi(ccu.c_str()),
																			(unsigned int)atoi(i2cChannel.c_str()) );
									}
								}
								else
								{
									errorsTreeBuilder->setFecNodeError( crateSlot, fecSlot, (unsigned int)atoi(ring.c_str()),
																							(unsigned int)atoi(ccu.c_str()));
									tkDiagErrorAnalyser->setFecError(	fecHardId,
																		(unsigned int)atoi(ring.c_str()),
																		(unsigned int)atoi(ccu.c_str()) );
								}
							}
							else
							{
								errorsTreeBuilder->setFecNodeError( crateSlot, fecSlot, (unsigned int)atoi(ring.c_str()));
								tkDiagErrorAnalyser->setFecError(	fecHardId,
																	(unsigned int)atoi(ring.c_str()) );

							}
						}
						else
						{
							errorsTreeBuilder->setFecNodeError( crateSlot, fecSlot);
								tkDiagErrorAnalyser->setFecError(	fecHardId );
						}
					}



					//Analyse FED HARDWARE ERRORS
					if (fedId != "NA")
					{
						unsigned int crateSlot;
						unsigned int fedSlot;
						errorsChecker->getCrateAndFedSlotsForFedid( &crateSlot, &fedSlot, tkDiagErrorAnalyser, (unsigned int)atoi(fedId.c_str()) );
						if (fedChannel != "NA")
						{
							errorsTreeBuilder->setFedNodeError( crateSlot, fedSlot, (unsigned int)atoi(fedChannel.c_str()) );
							tkDiagErrorAnalyser->setFedSoftIdError(	(unsigned int)atoi(fedId.c_str()), (unsigned int)atoi(fedChannel.c_str()) );
																							
						}
						else
						{
							errorsTreeBuilder->setFedNodeError( crateSlot, fedSlot);
							tkDiagErrorAnalyser->setFedSoftIdError(	(unsigned int)atoi(fedId.c_str()) );
						}
					}


					//Analyse PSU HARDWARE ERRORS
					if (psuId != "NA")
					{
						tkDiagErrorAnalyser->setDetIdError( (unsigned int)atoi(psuId.c_str()) );
						unsigned int psuRank = errorsChecker->getPsuIdRange((unsigned int)atoi(psuId.c_str()),
												tkDiagErrorAnalyser);
						errorsTreeBuilder->setPsuNodeError( psuRank );
					}

}
#endif






				}
			}
		}
		else if (whereFrom == -1)
		{
			//std::cout << "Display PREVIOUS LOG" << std::endl;
			long readLogNumber = 0;

			// Rewind 1-Block Logs
			rewind(fileOut_);
			if (currentLogNumber >= 2*LOGARRAYLINES)
			{
				numberOfLogsToBypass = currentLogNumber - 2*LOGARRAYLINES + 1;
			}
			else numberOfLogsToBypass = 0;
			if (numberOfLogsToBypass < 0) numberOfLogsToBypass=0;
			//std::cout << "number of logs read till now : " << currentLogNumber << std::endl;
			//std::cout << "number of logs to bypass before re-read : " << numberOfLogsToBypass << std::endl;
			currentLogNumber = 0;
			while ( (!(feof(fileOut_))) && (readLogNumber < numberOfLogsToBypass) )
			{
				//std::cout << "readLogNumber is : " << readLogNumber << std::endl;
				//std::cout << "numberOfLogsToBypass is : " << numberOfLogsToBypass << std::endl;
				//Read tags in range [<DATA> .. </DATA>]
				for (int i=0; i<19; i++) fgets(oneLine, 1023, fileOut_);
				readLogNumber++;
				currentLogNumber++;
			}
			//std::cout << readLogNumber << "logs read before starting regular log reading" << std::endl;

			readLogNumber = 0;
			for (int logarrayinitialiser=0; logarrayinitialiser < LOGARRAYLINES ; logarrayinitialiser++)
			{
				logsMemory[logarrayinitialiser][MSGOFFSET] = "UNINITIALIZED_BT_ED";
				copyOfLogsMemory[logarrayinitialiser][MSGOFFSET] = "UNINITIALIZED_BT_ED";
			}

			while ( (!(feof(fileOut_))) && (readLogNumber < LOGARRAYLINES) )
			{
				std::string rawField;
				std::string tag;

				//read <DATA> line
				fgets(oneLine, 1023, fileOut_);

				//read <LOGNUMBER> line
				fgets(oneLine, 1023, fileOut_);

				//Read <LEVEL>
				fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<LEVEL>";
				logsMemory[arrayDesc][LVLOFFSET] = cleanField(rawField, tag);

				//Read <ERR_CODE>
				fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<ERR_CODE>";
				logsMemory[arrayDesc][ECOFFSET] = cleanField(rawField, tag);

				//Read <ERR_MSG>
				fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<ERR_MSG>";
				logsMemory[arrayDesc][MSGOFFSET] = cleanField(rawField, tag);

				//Read <ERR_TIMESTAMP>
				fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<ERR_TIMESTAMP>";
				logsMemory[arrayDesc][DIAGTSOFFSET] = cleanField(rawField, tag);

				//Read <ERR_UID>
				fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<ERR_UID>";
				logsMemory[arrayDesc][ORGOFFSET] = cleanField(rawField, tag);

				//Read tags in range [<RUN_TYPE> .. </DATA>]
				for (int i=0; i<15; i++) fgets(oneLine, 1023, fileOut_);
				if ( (feof(fileOut_)) || (strcmp(oneLine,"</DATA>\n") != 0) )
				{
					logsMemory[arrayDesc][MSGOFFSET] = "UNINITIALIZED_BT_ED";
					logsMemory[arrayDesc][ORGOFFSET] = "UNINITIALIZED_BT_ED";
					logsMemory[arrayDesc][DIAGTSOFFSET] = "UNINITIALIZED_BT_ED";
					logsMemory[arrayDesc][LVLOFFSET] = "UNINITIALIZED_BT_ED";
					logsMemory[arrayDesc][ECOFFSET] = "UNINITIALIZED_BT_ED";
				}
				else
				{
					lastKnownGoodPos = ftell(fileOut_);
					arrayDesc++;
					currentLogNumber++;
					readLogNumber++;
					if (arrayDesc==LOGARRAYLINES) arrayDesc=0;
				}
			}
			//std::cout << "After BackLoop : currentLogNumber=" << currentLogNumber << "  readLogNumber=" << readLogNumber << std::endl;
		}

		else if (whereFrom == 1)
		{
			//std::cout << "Display NEXT LOG" << std::endl;
			long readLogNumber = 0;
			readLogNumber = 0;
			
			while ( (!(feof(fileOut_))) && (readLogNumber < LOGARRAYLINES) )
			{
				std::string rawField;
				std::string tag;

				//read <DATA> line
				fgets(oneLine, 1023, fileOut_);

				//read <LOGNUMBER> line
				fgets(oneLine, 1023, fileOut_);

				//Read <LEVEL>
				fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<LEVEL>";
				logsMemory[arrayDesc][LVLOFFSET] = cleanField(rawField, tag);

				//Read <ERR_CODE>
				fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<ERR_CODE>";
				logsMemory[arrayDesc][ECOFFSET] = cleanField(rawField, tag);

				//Read <ERR_MSG>
				fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<ERR_MSG>";
				logsMemory[arrayDesc][MSGOFFSET] = cleanField(rawField, tag);

				//Read <ERR_TIMESTAMP>
				fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<ERR_TIMESTAMP>";
				logsMemory[arrayDesc][DIAGTSOFFSET] = cleanField(rawField, tag);

				//Read <ERR_UID>
				fgets(oneLine, 1023, fileOut_);
				rawField = oneLine;
				tag = "<ERR_UID>";
				logsMemory[arrayDesc][ORGOFFSET] = cleanField(rawField, tag);

				//Read tags in range [<RUN_TYPE> .. </DATA>]
				for (int i=0; i<15; i++) fgets(oneLine, 1023, fileOut_);
				if ( (feof(fileOut_)) || (strcmp(oneLine,"</DATA>\n") != 0) )
				{
					logsMemory[arrayDesc][MSGOFFSET] = "UNINITIALIZED_BT_ED";
					logsMemory[arrayDesc][ORGOFFSET] = "UNINITIALIZED_BT_ED";
					logsMemory[arrayDesc][DIAGTSOFFSET] = "UNINITIALIZED_BT_ED";
					logsMemory[arrayDesc][LVLOFFSET] = "UNINITIALIZED_BT_ED";
					logsMemory[arrayDesc][ECOFFSET] = "UNINITIALIZED_BT_ED";
				}
				else
				{
					lastKnownGoodPos = ftell(fileOut_);
					arrayDesc++;
					currentLogNumber++;
					readLogNumber++;
					if (arrayDesc==LOGARRAYLINES) arrayDesc=0;
				}
			}
			//std::cout << "After BackLoop : currentLogNumber=" << currentLogNumber << "  readLogNumber=" << readLogNumber << std::endl;
		}
    }
   	//else std::cout << "Cannot open file for reading" << std::endl;
}

