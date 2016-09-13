/*

   FileName : 		ErrorDbUploader.cc

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


#include "ErrorDbUploader.h"

#define NOT_EOF !(feof(fileIn_))

//#define MAX_LOGS_PER_BURST	25

#define TEMP_DIRECTORY	"/tmp"

#ifdef WILL_USE_PIXELS_ERRDB
	#define TMPPIXELSLOGFILEPREFIX	"LogFileForPixelsErrorDb-"
#endif
#ifdef WILL_USE_TRACKER_ERRDB
	#define TMPTRACKERLOGFILEPREFIX	"LogFileForTrackerErrorDb-"
#endif
#define TMPXMLLOGFILESUFFIX	".xml"
#define TMPZIPLOGFILESUFFIX	".zip"

#define DIAG_MAX_MSG_BLOCK_LENGTH_FOR_DB_UPLOAD	25
#define DIAG_MAX_STORAGE_TICK_COUNT 2
XDAQ_INSTANTIATOR_IMPL(ErrorDbUploader);






#ifdef WILL_USE_GWT_FIREITEMS_VIEWER
	ErrorDbUploader::ErrorDbUploader(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): xdaq::Application(s), JsInterface(s)
#else
	ErrorDbUploader::ErrorDbUploader(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): xdaq::Application(s)
#endif
{



	//Define FSM states
	fsm_.addState ('C', "Configured") ;
	fsm_.addState ('H', "Halted");
	fsm_.addStateTransition ('C','H', EXECTRANSCH, this, &ErrorDbUploader::haltAction);
	fsm_.addStateTransition ('H','C', EXECTRANSHC, this, &ErrorDbUploader::configureAction);
	fsm_.setFailedStateTransitionAction( this, &ErrorDbUploader::failedTransition );

	//Initialize FSM state at load
	fsm_.setInitialState('H');
	fsm_.reset();

	// Import/Export the stateName variable
	getApplicationInfoSpace()->fireItemAvailable("stateName", &fsm_.stateName_);



#ifdef WILL_USE_TRACKER_ERRDB
	dbAccess_ = NULL;
#endif

	successLogsUploadAttempts = 0;
	failLogsUploadAttempts = 0;
	summedSuccessfulInsertionTime = 0;

	currentRunNumberAsString_ = "";
	runNumberIsSet_ = false;
	filePosForFirstNoRunNumberLog_ = 0;
	filePosForFirstNoRunNumberLogIsSetted_ = false;

	lastKnownGoodTimestamp = "UNINITIALIZED";

	currentRunNumberFromGedAsString_ = "0";

	errDbTimerHalfRate_ = DIAG_DB_UPLOAD_HALF_RATE;

	isFirstTimerTick_ = true;

	logsReadFromFile = 0;
	isInProcessingStage = false;

	maxDbUploadTime = 0;
	minDbUploadTime = 1000000000;


	storageTickCount_ = 0;

	#ifdef WILL_USE_PIXELS_ERRDB
		errDbPixelsFileNameNumericPart_ = "";
	#endif

	#ifdef WILL_USE_TRACKER_ERRDB
		errDbTrackerFileNameNumericPart_ = "";
	#endif

	//Initialise input logs file pointer
	fileIn_ = NULL;
	#ifdef WILL_USE_PIXELS_ERRDB
		pixelsFileOut_ = NULL;
	#endif

	#ifdef WILL_USE_TRACKER_ERRDB
		trackerFileOut_ = NULL;
	#endif

	//Initialise input logs file state
	fileinIsOpen_ = false;
	#ifdef WILL_USE_PIXELS_ERRDB
		pixelsFileoutIsOpen_ = false;
	#endif
	#ifdef WILL_USE_TRACKER_ERRDB
		trackerFileoutIsOpen_ = false;
	#endif


//FILTER_BEGIN
fedMonitorLogAmount_ = 0;
dcuStructErrorsLogAmount_ = 0;
fecWorkLoopErrorsLogAmount_ = 0;

fecWorkLoopLogPattern = FEC_WORKLOOPERROR_PATTERN;
fedMonitorLogPattern = FED_DEBUG_PATTERN;
storedDcuErrorStructs_ = 0;

dcuStructErrorLogPattern = FEC_DCUREADOUTERROR_PATTERN;
//FILTER_END







//CCC	    
	filePos_ = 0;



	// Display module version on console at load time
	std::stringstream mmesg;
	mmesg << "Process version " << ERRORDBUPLOADER_PACKAGE_VERSION << " loaded";
	this->getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);
	LOG4CPLUS_INFO(this->getApplicationLogger(), mmesg.str());
	this->getApplicationLogger().setLogLevel(ERROR_LOG_LEVEL);


	// Set Default() method state as : initialising....
	internalState_ = 0;

	//logcounter = 0;







			
	// Instanciate diagnostic object
	diagService_ = new DiagBagWizard(
                                    getApplicationDescriptor()->getClassName() + "DiagLvlOne" ,
                                    this->getApplicationLogger(),
                                    getApplicationDescriptor()->getClassName(),                                    getApplicationDescriptor()->getInstance(),
                                    getApplicationDescriptor()->getLocalId(),
                                    (xdaq::WebApplication *)this );


	// bind xgi and xoap commands specific to this application
	xgi::bind(this,&ErrorDbUploader::Default, "Default");
	xgi::bind(this,&ErrorDbUploader::Default1, "Default1");
	xgi::bind(this,&ErrorDbUploader::callDiagSystemPage, "callDiagSystemPage");
	xgi::bind(this,&ErrorDbUploader::callFsmPage, "callFsmPage");
	xgi::bind(this,&ErrorDbUploader::callWriteLogsPage, "callWriteLogsPage");


	xgi::bind(this,&ErrorDbUploader::configureStateMachine, "configureStateMachine");
	xgi::bind(this,&ErrorDbUploader::stopStateMachine, "stopStateMachine");



	//Bind diagnostic applications
	xgi::bind(this,&ErrorDbUploader::configureDiagSystem, "configureDiagSystem");
	xgi::bind(this,&ErrorDbUploader::applyConfigureDiagSystem, "applyConfigureDiagSystem");


	// Call a macro needed for running a diagService in this application
	DIAG_DECLARE_LR_APP



	// Bind SOAP callbacks for FSM control messages 
	xoap::bind (this, &ErrorDbUploader::fireEvent, EXECTRANSCH, XDAQ_NS_URI);
	xoap::bind (this, &ErrorDbUploader::fireEvent, EXECTRANSHC, XDAQ_NS_URI);

	#ifdef AUTO_CONFIGURE_PROCESSES
		std::stringstream timerName;
		timerName << getApplicationDescriptor()->getContextDescriptor()->getURL() << ":";
		timerName << getApplicationDescriptor()->getClassName() << ":" << getApplicationDescriptor()->getLocalId() << ":" << getApplicationDescriptor()->getInstance();
//		toolbox::task::Timer * timer = toolbox::task::getTimerFactory()->createTimer(timerName.str());
		timer_ = toolbox::task::getTimerFactory()->createTimer(timerName.str());
		toolbox::TimeInterval interval(AUTO_LR_CONFIGURE_DELAY,0);
//		toolbox::TimeInterval interval(10,0);

//		toolbox::TimeInterval interval(1,0);

		toolbox::TimeVal start;
//		start = toolbox::TimeVal::gettimeofday() + interval;
		start = toolbox::TimeVal::gettimeofday() + interval;
//		timer->schedule( this, start,  0, "" );
timer_->scheduleAtFixedRate( start, this, interval, this->getApplicationDescriptor()->getContextDescriptor(), "" );
	#endif



	#ifdef WILL_USE_PIXELS_ERRDB
		pixelsErrDbIsUsable_ = false;
		
		char * readEnv1=getenv ("DIAG_PATH_TO_ERRDB_CREDENTIALS") ;
		if (readEnv1 != NULL)
		{
			path_to_errdb_credentials_ = std::string(readEnv1);
			pixelsErrDbIsUsable_ = true;
		}
		else
		{
			std::cerr << "Please set the DIAG_PATH_TO_ERRDB_CREDENTIALS environemental variable to store logs into pixels ErrDb" << std::endl ;
			path_to_errdb_credentials_ = "";
			pixelsErrDbIsUsable_ = false;
		}


		char * readEnv2=getenv ("DIAG_ERRDB_SPOOLAREA_SCP_PARAMETERS") ;
		if (readEnv2 != NULL)
		{
			errdb_scptools_param_ = std::string(readEnv2);
		}
		else
		{
			std::cerr << "Please set the DIAG_ERRDB_SPOOLAREA_SCP_PARAMETERS environemental variable to store logs into pixels ErrDb" << std::endl ;
			errdb_scptools_param_ = "";
			pixelsErrDbIsUsable_ = false;
		}

	#endif

	#ifdef WILL_USE_TRACKER_ERRDB
		char * readEnv=getenv ("CONFDB") ;
		if (readEnv != NULL)
		{
			tracker_conf_db_ = std::string(readEnv);
			trackerErrDbIsUsable_ = true;
		}
		else
		{
			std::cerr << "Please set the CONFDB environemental variable to store logs into tracker ErrDb" << std::endl ;
			tracker_conf_db_ = "";
			trackerErrDbIsUsable_ = false;
		}



//std::cout << "Beginning creation of the DB access" << std::endl;
  // Create the database access
  // retreive the connection of the database through an environmental variable called CONFDB
  std::string login="nil", passwd="nil", path="nil" ;
  DbCommonAccess::getDbConfiguration (login, passwd, path) ;

  if (login != "nil" && passwd !="nil" && path != "nil") {

    try {
      // create the database access
      dbAccess_ = new DbTkDcuInfoAccess ( login, passwd, path ) ;
      std::cout << "Database access created" << std::endl;
    }
    catch (oracle::occi::SQLException &e) {
      std::cerr << "Cannot create the access to the database: " << login << "/" << "********" << "@" << path << std::endl ;
      std::cerr << e.what() ;
      std::cerr << "Exiting ..." << std::endl ;
    }
  }
  else {
    std::cout << "Please specify a database ; CONFDB environnement variable not set" << std::endl ;
  }



	#endif


}



ErrorDbUploader::~ErrorDbUploader()
{
	//If logfile is still open, close it
	if (fileinIsOpen_ == true)
	{
		if (fileIn_ != NULL) fclose(fileIn_);
	}


	while (dcuStructList_.empty() == false) dcuStructList_.pop_back();

#ifdef WILL_USE_TRACKER_ERRDB
	if (dbAccess_ != NULL) delete dbAccess_ ;
#endif

}


// Returns the HTML page displayed from the Default1() method
void ErrorDbUploader::getDefault1Content(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{

	*out << "<br>ErrorsDbUploader Home page<br>";


	std::string lclFileNAme;
	if (diagService_->logFileName_ == "" )
	{
		lclFileNAme = "ErrorsDbUploader is reading from file : Undefined. (Run number must be defined at RCMS level) + (Some logs must have been received by GED) before first logfile access attempt.";
	}
	else lclFileNAme = diagService_->logFileName_;
	*out << "<br>ErrorsDbUploader is reading from file : " << lclFileNAme.c_str() << "<br>" << std::endl;



	if (currentRunNumberFromGedAsString_ == "0" )
	{
		*out << "<br>Logging for run number : Undefined for now, GED must get it from RCMS before it can be used in this logging process<br>" << std::endl;
	}
	else *out << "<br>Logging for run number : " << currentRunNumberFromGedAsString_ << "<br>" << std::endl;
	*out << "<br>";

	*out << "<br>Number of logs read from log file and validated for errDB upload: " << logsReadFromFile << "<br>" << std::endl;
	*out << "<br>Number of logs successfully inserted into ErrDB : " << successLogsUploadAttempts << "<br>" << std::endl;
	*out << "<br>Number of logs unsuccessfully inserted into ErrDB : " << (logsReadFromFile - successLogsUploadAttempts) << "<br>" << std::endl;

	if (successLogsUploadAttempts > 0)
	{
		*out << "<br>Average insertion time in microseconds for 1 log : " << (long long)(summedSuccessfulInsertionTime/successLogsUploadAttempts) << "<br>" << std::endl;
	}
	else *out << "<br>Average insertion time in microseconds for 1 log : Undefined, no log successfully inserted into ErrDB for now <br>" << std::endl;



	if (maxDbUploadTime != 0)
	{
		*out << "<br>Max insertion time in microseconds measured for 1 log : " << maxDbUploadTime << "<br>" << std::endl;
	}
	else *out << "<br>Max insertion time in microseconds measured for 1 log :  : Undefined, no log successfully inserted into ErrDB for now <br>" << std::endl;


	if (minDbUploadTime != 1000000000)
	{
		*out << "<br>Min insertion time in microseconds measured for 1 log : " << minDbUploadTime << "<br>" << std::endl;
	}
	else *out << "<br>Min insertion time in microseconds measured for 1 log :  : Undefined, no log successfully inserted into ErrDB for now <br>" << std::endl;

	*out << "<br>" << std::endl;

	*out << "<br>Number of FED Monitoring debug logs ignored for upload : " << fedMonitorLogAmount_ << "<br>" << std::endl;

	if (fecWorkLoopErrorsLogAmount_ > 0)
	{
		*out << "<br>Number of FEC DCU waitingWorkLoop queue full warn messages ignored for upload : " << (fecWorkLoopErrorsLogAmount_ - 1) << "<br>" << std::endl;
	}
	else *out << "<br>Number of FEC DCU waitingWorkLoop queue full warn messages ignored for upload : 0<br>" << std::endl;



	*out << "<br>Number of DCU readout error messages ignored for upload : " << dcuStructErrorsLogAmount_ << std::endl;
	*out << "<br>Details on DCU readout error messages : First occurence of each DCU hardware chain has been uploaded into DB" << std::endl;

	int dcuStructCounter = 0;
	if (dcuStructList_.empty() == false)
	{
		//std::cout << "Got at least one DCU record" << std::endl;
		std::vector<dcuStructure>::iterator dcuIterator = dcuStructList_.begin();
		for (dcuIterator = dcuStructList_.begin(); dcuIterator < dcuStructList_.end(); dcuIterator++)
		{
			dcuStructCounter++;
			*out << "<br>DCU STRUCT NUMBER&nbsp;" << dcuStructCounter << "&nbsp;:&nbsp;";
			struct dcuStructure oneLclDcuStruct;
			oneLclDcuStruct = *(dcuIterator);
			*out << "&nbsp;DCU crate=" << oneLclDcuStruct.crate << "&nbsp;";
			*out << "&nbsp;DCU slot = " << oneLclDcuStruct.slot << "&nbsp;";
			*out << "&nbsp;DCU ring = " << oneLclDcuStruct.ring << "&nbsp;";
			*out << "&nbsp;DCU ccu = " << oneLclDcuStruct.ccu << "&nbsp;";
			*out << "&nbsp;DCU i2cChannel = " << oneLclDcuStruct.i2cChannel << "&nbsp;";
			*out << "&nbsp;DCU i2cAddress = " << oneLclDcuStruct.i2cAddress << "&nbsp;";
			*out << "&nbsp;DCU occurences = " << oneLclDcuStruct.occurences << "&nbsp;" << std::endl;
		}
	}
			*out << "<br>" << std::endl;




}





// Returns the HTML page for FSM status
void ErrorDbUploader::getFsmStatePage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
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


void ErrorDbUploader::getForceWriteLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	//	std::cout << "Forced Logs file creation called." << std::endl;


		if (isInProcessingStage == false)
		{
			//DEBUGPOINT
			std::cout << "User triggered ; fireing readLogsFromFile(0) command" << std::endl;

			readLogsFromFile(0);
		}
		//DEBUGPOINT
		else
		{
			// on ne fait rien, le processing est déja en cours
			std::cout << "User triggered ; readLogsFromFile(0) already running ; aborted" << std::endl;
		}

//	readLogsFromFile(0);
	//	std::cout << "Forced Logs file creation done." << std::endl;
}


/* xgi method called when the default link is clicked */
void ErrorDbUploader::Default1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 0;
	Default(in, out);
}

// xgi method called when the link <display_diagsystem> is clicked
void ErrorDbUploader::callDiagSystemPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 1;
	Default(in, out);
}

// xgi method called when the link <display_fsm> is clicked
void ErrorDbUploader::callFsmPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 2;
	Default(in, out);
}


// xgi method called when the link <display_fsm> is clicked
void ErrorDbUploader::callWriteLogsPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 3;
	Default(in, out);
}




// Displays the available HyperDaq links for this process
void ErrorDbUploader::displayLinks(xgi::Input * in, xgi::Output * out)
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
	*out << "<a href=" << urlDiag_ << ">Configure ErrorsDbUploader</a>";
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";

	// Display FSM page
	std::string urlFsm_ = "/";
	urlFsm_ += getApplicationDescriptor()->getURN();
	urlFsm_ += "/callFsmPage";
	*out << "<a href=" << urlFsm_ << ">FSM Access</a>";
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";


	// Display FSM page
	std::string urlWriteLogs_ = "/";
	urlWriteLogs_ += getApplicationDescriptor()->getURN();
	urlWriteLogs_ += "/callWriteLogsPage";
	*out << "<a href=" << urlWriteLogs_ << ">Force Logs file creation for error DB</a>";
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";


	*out << "<br>";
}




// Default() hyperDaq method
void ErrorDbUploader::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{

	// Create HTML header
	*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
	*out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
	*out << "<HEAD>" << std::endl;
	*out << cgicc::title("ErrorsDbUploader") << std::endl;
	*out << "</HEAD>" << std::endl;
	xgi::Utils::getPageHeader(  out, 
								"ErrorsDbUploader", 
								getApplicationDescriptor()->getContextDescriptor()->getURL(),
								getApplicationDescriptor()->getURN(),
								"/daq/xgi/images/Application.jpg" );

	// Display available links on top of the HTML page
	displayLinks(in, out);

	// Get page HTML content according to the selected link
	if (internalState_ == 0) getDefault1Content(in, out);
	if (internalState_ == 1) diagService_->getDiagSystemHtmlPage(in, out,getApplicationDescriptor()->getURN());
	if (internalState_ == 2) getFsmStatePage(in, out);
	if (internalState_ == 3) getForceWriteLogsPage(in, out);




	// Create HTML footer
	xgi::Utils::getPageFooter(*out);


}







// Turns the ErrorDbUploader into state INITIALISED (FSM point of view)
void ErrorDbUploader::configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
	// Apply fireitems value to internal variables and check validity
	DIAG_EXEC_FSM_INIT_TRANS
}



// Turns the ErrorDbUploader into state HALTED (FSM point of view)
void ErrorDbUploader::haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
	// Apply fireitems value to internal variables and check validity
	DIAG_EXEC_FSM_STOP_TRANS
}
   

   
// Allows the FSM to be controlled via a soap message carrying a "fireEvent" message
xoap::MessageReference ErrorDbUploader::fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception)
{
	//#define DEBUG_FIREEVENT
	#ifdef DEBUG_FIREEVENT
		std::cout << "In ErrorDbUploader : fireEvent received" << std::endl ;
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
void ErrorDbUploader::failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
	toolbox::fsm::FailedEvent & fe = dynamic_cast<toolbox::fsm::FailedEvent&>(*e);
	std::ostringstream msgError;
	msgError << "Failure occurred when performing transition from: " << fe.getFromState() << " to: " << fe.getToState() << " exception: " << fe.getException().what();
	diagService_->reportError (msgError.str(), DIAGERROR);
}



// Can be called via the <configure_FSM> hyperdaq HTML page of this process
void ErrorDbUploader::configureStateMachine(xgi::Input * in, xgi::Output * out)
{
	if (fsm_.getCurrentState() == 'H')
	{
		toolbox::Event::Reference e(new toolbox::Event(EXECTRANSHC, this));
		fsm_.fireEvent(e);
	}
	Default(in, out);
}


// Can be called via the <configure_FSM> hyperdaq HTML page of this process
void ErrorDbUploader::stopStateMachine(xgi::Input * in, xgi::Output * out)
{
	if (fsm_.getCurrentState() == 'C')
	{
		toolbox::Event::Reference e(new toolbox::Event(EXECTRANSCH, this));
		fsm_.fireEvent(e);
	}
	Default(in, out);
}





#ifdef AUTO_CONFIGURE_PROCESSES
void ErrorDbUploader::timeExpired (toolbox::task::TimerEvent& e) 
{


//std::cout << "Error DB Uploader timer triggered" << std::endl;


//If RunNumber has not been setted yet, ask GED to give us one
if (currentRunNumberFromGedAsString_ == "0")
{
//std::cout << "Trying to get currentRunNumberFromGedAsString_ from GED" << std::endl;
    /* Build SOAP for sending action */
    xoap::MessageReference err_msg = xoap::createMessage();
    xoap::SOAPPart err_soap = err_msg->getSOAPPart();
    xoap::SOAPEnvelope err_envelope = err_soap.getEnvelope();
    xoap::SOAPName command = err_envelope.createName("runNumberRequest", "xdaq", XDAQ_NS_URI);
    xoap::SOAPBody err_body = err_envelope.getBody();
    err_body.addBodyElement(command);


    xdaq::ApplicationDescriptor * selfAppDescriptor_;

	xoap::MessageReference reply;

        try
        {
    	    	xdaq::ApplicationDescriptor * err_d = getApplicationContext()->getDefaultZone()->getApplicationDescriptor("GlobalErrorDispatcher", 0);
		selfAppDescriptor_ = getApplicationContext()->getDefaultZone()->getApplicationDescriptor(getApplicationDescriptor()->getClassName(), getApplicationDescriptor()->getInstance());
               reply = getApplicationContext()->postSOAP(err_msg, *selfAppDescriptor_, *err_d);
    	}
    	catch (xdaq::exception::Exception& e)
	{
	}
	xoap::SOAPBody mb = reply->getSOAPPart().getEnvelope().getBody();
	std::vector<xoap::SOAPElement> paramsList = mb.getChildElements ();
	if(paramsList.size()==1)
	{
		xoap::SOAPName currentRunNumber ("currentRunNumber", "", "");
		currentRunNumberFromGedAsString_ = paramsList[0].getAttributeValue(currentRunNumber);
	}
//std::cout << "Run number returned by GED is : " << currentRunNumberFromGedAsString_ << std::endl;
}
//else std::cout << "Not trying to get currentRunNumberFromGedAsString_ from GED ; currentRunNumberFromGedAsString_ != 0" << std::endl;
//If RunNumber has already been setted, it's ok do nothing



	if (isFirstTimerTick_ == true)
	{
		DIAG_EXEC_FSM_INIT_TRANS
		std::cout << "ErrorDbUploader(LID=" << getApplicationDescriptor()->getLocalId() << ",Instance=" << getApplicationDescriptor()->getInstance() << ") ready" << std::endl;
		isFirstTimerTick_ = false;

		//After first tick, modify tickrate to approx. 1 minute between each ErrorDb connaction attempt
		toolbox::TimeInterval interval(errDbTimerHalfRate_,0);
		toolbox::TimeVal start;
		start = toolbox::TimeVal::gettimeofday() + interval;
		timer_->scheduleAtFixedRate( start, this, interval, this->getApplicationDescriptor()->getContextDescriptor(), "" );


	}
	else
	{
		//If at that point we have a valid run number, then begin to process logs reading and DB upload
		if (currentRunNumberFromGedAsString_ != "0")
		{
		//std::cout << "I got a valid run number (" << currentRunNumberFromGedAsString_ << ") from GED ; processing logs" << std::endl;
		
			//std::cout << "Polling for logs to upload into the DB." << std::endl;
			if (isInProcessingStage == false)
			{
				//std::cout << "Tick..." << std::endl;

				//DEBUGPOINT
				//std::cout << "Timer triggered ; fireing readLogsFromFile(0) command" << std::endl;
				storageTickCount_++;
				readLogsFromFile(0);
			}
			//DEBUGPOINT
			else
			{
				//std::cout << "Tock..." << std::endl;

				// on ne fait rien, le processing est déja en cours
				storageTickCount_ = 0;
				//std::cout << "Timer triggered ; readLogsFromFile(0) already running ; aborted" << std::endl;
			}
		}
		//else std::cout << "Waiting for a valid run number from GED before beginning the logs processing batch" << std::endl;

	}
}
#endif








void ErrorDbUploader::cleanupMsgString(std::string * msg)
{


		if (msg->length() > 1024) msg->resize(1024);

    	for (unsigned int sl=0 ; sl < strlen((*msg).c_str()) ; sl++)
        {
	    /* Security check : Parse message and if any double quote appears in then replace it with space */
    	    if ((*msg)[sl] == '\"') (*msg)[sl]=' ';
	    /* Security check : Parse message and if any \n appears in then replace it with space */
    	    if ((*msg)[sl] == '\n') (*msg)[sl]='\t';
	    /* Security check : Parse message and if any < appears in then replace it with space */
    	    if ((*msg)[sl] == '<') (*msg)[sl]=' ';
	    /* Security check : Parse message and if any > appears in then replace it with space */
    	    if ((*msg)[sl] == '>') (*msg)[sl]=' ';
	    /* Security check : Parse message and if any & appears in then replace it with space - Chainsaw display does not supports & chars in message
	    fields!!!*/
    	    if ((*msg)[sl] == '&') (*msg)[sl]='A';
        }
}



std::string ErrorDbUploader::cleanField(std::string& rawField, std::string& tag)
{
	std::string::size_type loc;
	std::string cleanedField="";
	loc = rawField.find( tag, 0 );
	if( loc != std::string::npos )
	{
		cleanedField = rawField.substr(tag.length()+1, rawField.length()-tag.length()-tag.length()-3);
	}
	return cleanedField;
}



std::string ErrorDbUploader::getTag(std::string& rawField)
{
	std::string tag="";
	std::string::size_type locEnd;
	locEnd = rawField.find( ">", 0 );
	std::string::size_type locBegin;
	locBegin = rawField.find( "<", 0 );
	if ( (locBegin != std::string::npos) && (locEnd != std::string::npos) )
	{
		tag = rawField.substr(locBegin, locEnd);
	}
	return tag;
}







void ErrorDbUploader::readLogsFromFile(int command)
{
isInProcessingStage = true;
//DEBUGPOINT
//std::cout << "Entering readLogsFromFile(0) ; isInProcessingStage now setted to TRUE ; logsReadFromFile = " << logsReadFromFile << std::endl;

//	int logsReadFromFile = 0;
	//std::cout << "DEBUG 00" << std::endl;
	//First, get the name of the file in which we should be working
	
	//If useFileLogging_ == false then get name from GED
	if (diagService_->useFileLogging_ == false)
	{
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
	//Else, If useFileLogging_ == true then do nothing - filename change has already been taken into account from the hyperdaq diagsystem page


	//During next check, if filename has not changed since last access then file will not be closed
	diagService_->checkLrFileOption();

	//If the file to open is a new one
	if (diagService_->logFileNameHasChanged_ == true)
	{
		//Close old file
		if (fileIn_ != NULL) fclose(fileIn_);
		fileinIsOpen_ = false;
		//Notify that filename change has been taken into account
		diagService_->logFileNameHasChanged_ = false;
		//Reset run number properties, in case a run stop/start
		runNumberIsSet_ = false;
	}

	//If targetted logfile is not opened yet
	if (fileinIsOpen_ == false)
	{
		fileIn_ = fopen(diagService_->logFileName_.c_str(), "r");
		if (fileIn_ != NULL)
		{
			fileinIsOpen_ = true;
//			CCC
			filePos_ = 0;
			//logcounter = 0;
		}
		else
		{
			std::cout << "Unable to read from log file \"" << diagService_->logFileName_.c_str() << "\"" << std::endl;
		}
	}


//std::cout << "DEBUG 01" << std::endl;


	if (fileIn_ != NULL) 
	{



		char oneLine[1024];
		CLog c;	
		//std::cout << "filein is not null" << std::endl;


//CCC

		if (filePos_ <0)
		{
			filePos_=0;
			//logcounter = 0;
		}
		fseek(fileIn_, filePos_, SEEK_SET);

		
		//int validatedLogsCounter=0;
		while (NOT_EOF)
		{
		
			//std::cout << "filein is not EOF setted" << std::endl;
			//logsReadFromFile++;
			


			std::string logNumber="";
			std::string level="";
			std::string errCode="";
			std::string errMsg="";
			std::string timestamp="";
			std::string source="";
			std::string runtype="";
			std::string runnumber="";
			std::string errNbrInstance="";
			std::string sysId="";
			std::string subSysId="";
			std::string fecHardId="";
			std::string ring="";
			std::string ccu="";
			std::string i2cChannel="";
			std::string i2cAddress="";
			std::string softVersion="";
			std::string fedId="";
			std::string fedChannel="";
			std::string trigger="";
			std::string psuId="";
			std::string software="";
			std::string xtra_buffer="";
			std::string nestedClassPath="";
			std::string nestedFileName="";
			std::string nestedLineNumber="";
			std::string crate="";
			std::string slot="";
			std::string partitionname="";

			bool dataSetIsValid = false;

			(std::string)oneLine="";

			//Seek for dataset beginning
			//std::cout << "seeking for <DATA> tag" << std::endl;
			do
			{
				fgets(oneLine, 1023, fileIn_);
				//if (NOT_EOF) std::cout << "I have read : " << oneLine << std::endl;
			} while ( ((std::string)oneLine != "<DATA>\n") && (NOT_EOF) );
			//if (NOT_EOF) std::cout << "I have read : " << oneLine;



			do
			{
			
				fgets(oneLine, 1023, fileIn_);
				//if (NOT_EOF) std::cout << "I have read : " << oneLine << std::endl;

				//Convert input to string
				std::string readLine = (std::string)oneLine;
				//Extract tag
				std::string tag = getTag(readLine);
				
				//Act according to tag value
				if (tag == "<LEVEL>") level=cleanField(readLine, tag);
				else if (tag == "<ERR_CODE>") errCode=cleanField(readLine, tag);
				else if (tag == "<ERR_MSG>") errMsg=cleanField(readLine, tag);
				else if (tag == "<ERR_TIMESTAMP>") timestamp=cleanField(readLine, tag);
				else if (tag == "<ERR_UID>") source=cleanField(readLine, tag);
				else if (tag == "<RUN_TYPE>") runtype=cleanField(readLine, tag);
				else if (tag == "<RUN_NUMBER>") runnumber=cleanField(readLine, tag);
				else if (tag == "<ERR_NBR_INSTANCE>") errNbrInstance=cleanField(readLine, tag);
				else if (tag == "<SYSID>") sysId=cleanField(readLine, tag);
				else if (tag == "<SUBSYSID>") subSysId=cleanField(readLine, tag);
				else if (tag == "<FECHARDID>") fecHardId=cleanField(readLine, tag);
				else if (tag == "<RING>") ring=cleanField(readLine, tag);
				else if (tag == "<CCU>") ccu=cleanField(readLine, tag);
				else if (tag == "<I2CCHANNEL>") i2cChannel=cleanField(readLine, tag);
				else if (tag == "<I2CADDRESS>") i2cAddress=cleanField(readLine, tag);
				else if (tag == "<SOFTWARETAGVERSION>") softVersion=cleanField(readLine, tag);
				else if (tag == "<FEDID>") fedId=cleanField(readLine, tag);
				else if (tag == "<FEDCHANNEL>") fedChannel=cleanField(readLine, tag);
				else if (tag == "<TRIGGER>") trigger=cleanField(readLine, tag);
				else if (tag == "<PSUID>") psuId=cleanField(readLine, tag);
				else if (tag == "<SOFTWARE>") software=cleanField(readLine, tag);
				else if (tag == "<LOGNUMBER>") logNumber=cleanField(readLine, tag);
				else if (tag == "<XTRA_BUFFER>") xtra_buffer=cleanField(readLine, tag);
				else if (tag == "<SUBCLASSPATH>") nestedClassPath=cleanField(readLine, tag);
				else if (tag == "<NESTEDFILENAME>") nestedFileName=cleanField(readLine, tag);
				else if (tag == "<NESTEDLINENUMBER>") nestedLineNumber=cleanField(readLine, tag);
				else if (tag == "<CRATE>") crate=cleanField(readLine, tag);
				else if (tag == "<SLOT>") slot=cleanField(readLine, tag);
				else if (tag == "<PARTITIONNAME>") partitionname=cleanField(readLine, tag);
			} while ( ((std::string)oneLine != "</DATA>\n") && (NOT_EOF) );
			//if (NOT_EOF) std::cout << "I have read : " << oneLine;




			if (NOT_EOF)
			{
//std::cout << "Read one log in base logssource file" << std::endl;

				//std::cout << "VALID FOR PROCESSING" << std::endl;
				dataSetIsValid = true;
				//logcounter++;
			}
			else
			{
				//std::cout << "INVALID FOR PROCESSING" << std::endl;
				dataSetIsValid = false;
			}



			//Assume that if we reached this point, the run number has been provided by GED
			runnumber = currentRunNumberFromGedAsString_;




			//Errors sometimes occurs in timestamps as wrote into raw logfile. If this happens, just patch it with last known good one.
			//If timestamp is ok, then fill ts backup with current ts.
			if (timestamp.length() != ERRDBUPLOADER_TIMESTAMP_CORRECT_LENGTH)
			{
				if (lastKnownGoodTimestamp != "UNINITIALIZED") timestamp = lastKnownGoodTimestamp;
			}
			else lastKnownGoodTimestamp = timestamp;
			
			

			//Extra check, avoid badly formed buffers with no primary key for DB
			if ( (level=="") || (errCode=="") || (errMsg=="") || (timestamp=="") ||(source=="") ) dataSetIsValid = false;


//FILTER_BEGIN
		//If message concerns FED debug messages, don't upload it.
		size_t fedStringFound;
		fedStringFound=errMsg.find(fedMonitorLogPattern);
		if (fedStringFound != std::string::npos)
		{
			//std::cout << "FED MONITOR LOG SPOTTED" << std::endl;
			dataSetIsValid = false;
			fedMonitorLogAmount_++;
	
		}
		//else std::cout << "STANDARD LOG SPOTTED" << std::endl;



		//If message concerns FEC workloop messages, don't upload it.
		size_t fecStringFound;
		fecStringFound=errMsg.find(fecWorkLoopLogPattern);
		/*
		std::cout << std::endl << "In FEC pattern recognition, working with message : " << std::endl;
		std::cout << "\t\t" << errMsg << std::endl;
		std::cout << "Trying to recognize pattern : " << std::endl;		
		std::cout << "\t\t" << fecWorkLoopLogPattern << std::endl;
		*/
		if (fecStringFound != std::string::npos)
		{
			//std::cout << "FEC WORKLOOP LOG SPOTTED" << std::endl;
			if (fecWorkLoopErrorsLogAmount_ > 0) dataSetIsValid = false;
			fecWorkLoopErrorsLogAmount_++;
	
		}
		//else std::cout << "STANDARD LOG SPOTTED" << std::endl;



		size_t dcuStringFound;
		//If message concerns DCU READOUT ERRORS, it's not simple...
		dcuStringFound=errMsg.find(dcuStructErrorLogPattern);
		if (dcuStringFound != std::string::npos)
		{
			//std::cout << "DCU struct error spotted" << std::endl;
			
			if (dcuStructList_.empty() == false)
			{
				//std::cout << "Got at least one DCU record" << std::endl;
				std::vector<dcuStructure>::iterator dcuIterator = dcuStructList_.begin();
				bool recordexists = false;
				for (dcuIterator = dcuStructList_.begin(); dcuIterator < dcuStructList_.end(); dcuIterator++)
				{
					struct dcuStructure oneLclDcuStruct;
					oneLclDcuStruct = *(dcuIterator);
					if (	(oneLclDcuStruct.crate == crate) &&
						(oneLclDcuStruct.slot == slot) &&
						(oneLclDcuStruct.ring == ring) &&
						(oneLclDcuStruct.ccu == ccu) &&
						(oneLclDcuStruct.i2cChannel == i2cChannel) &&
						(oneLclDcuStruct.i2cAddress == i2cAddress)
					)
					{
						//std::cout << "Record already exists ; incrementing record counter" << std::endl;
						dcuIterator->occurences++;
						recordexists=true;
						dcuStructErrorsLogAmount_++;
						dataSetIsValid = false;
					}
				}
				if (recordexists == false)
				{
					//std::cout << "Record does not exists yet ; creating it" << std::endl;
					struct dcuStructure oneLclDcuStruct;
					oneLclDcuStruct.crate = crate;
					oneLclDcuStruct.slot = slot;
					oneLclDcuStruct.ring = ring;
					oneLclDcuStruct.ccu = ccu;
					oneLclDcuStruct.i2cChannel = i2cChannel;
					oneLclDcuStruct.i2cAddress = i2cAddress;
					oneLclDcuStruct.occurences = 1;
					dcuStructList_.push_back(oneLclDcuStruct);
					//std::cout << "DCU record created" << std::endl;
					storedDcuErrorStructs_++;

				}
			}
			else
			{
				//std::cout << "DCU record vector is empty for now ; creating record" << std::endl;
				struct dcuStructure oneLclDcuStruct;
				oneLclDcuStruct.crate = crate;
				oneLclDcuStruct.slot = slot;
				oneLclDcuStruct.ring = ring;
				oneLclDcuStruct.ccu = ccu;
				oneLclDcuStruct.i2cChannel = i2cChannel;
				oneLclDcuStruct.i2cAddress = i2cAddress;
				oneLclDcuStruct.occurences = 1;
				dcuStructList_.push_back(oneLclDcuStruct);
				//std::cout << "DCU record created" << std::endl;
				storedDcuErrorStructs_++;
			}
		}
		//else std::cout << "DCU struct error NOT spotted" << std::endl;


//FILTER_END








			if (dataSetIsValid == true)
			{
//std::cout << "In ErrorDbUploader, log number " << logNumber << " ready to be uploaded" << std::endl;

				logsReadFromFile++;
				filePos_ = ftell(fileIn_);
				
		
				#ifdef WILL_USE_TRACKER_ERRDB



std::stringstream tobeuploaded ;
tobeuploaded << "<ROWSET>" << std::endl;
tobeuploaded << "\t<RAWDIAGNOSTICERROR>" << std::endl;



tobeuploaded << "\t\t<LOGNUMBER>" << logNumber << "</LOGNUMBER>" << std::endl;
tobeuploaded << "\t\t<ERR_LEVEL>" << level << "</ERR_LEVEL>" << std::endl;
tobeuploaded << "\t\t<ERR_CODE>" << errCode << "</ERR_CODE>" << std::endl;
tobeuploaded << "\t\t<ERR_MSG>" << errMsg << "</ERR_MSG>" << std::endl;
tobeuploaded << "\t\t<ERR_TIMESTAMP>" << timestamp << "</ERR_TIMESTAMP>" << std::endl;

//std::cout << "Timestamp is " << timestamp.length() << " char length" << std::endl;


tobeuploaded << "\t\t<ERR_UID>" << source << "</ERR_UID>" << std::endl;
tobeuploaded << "\t\t<RUN_TYPE>" << runtype << "</RUN_TYPE>" << std::endl;

tobeuploaded << "\t\t<RUN_NUMBER>" << runnumber << "</RUN_NUMBER>" << std::endl;
//tobeuploaded << "\t\t<RUN_NUMBER>" << "123457" << "</RUN_NUMBER>" << std::endl;


tobeuploaded << "\t\t<ERR_NBR_INSTANCE>" << errNbrInstance << "</ERR_NBR_INSTANCE>" << std::endl;
tobeuploaded << "\t\t<SYSID>" << sysId << "</SYSID>" << std::endl;
tobeuploaded << "\t\t<SUBSYSID>" << subSysId << "</SUBSYSID>" << std::endl;
tobeuploaded << "\t\t<PARTITIONNAME>" << partitionname << "</PARTITIONNAME>" << std::endl;
tobeuploaded << "\t\t<FECHARDID>" << fecHardId << "</FECHARDID>" << std::endl;
tobeuploaded << "\t\t<RING>" << ring << "</RING>" << std::endl;
tobeuploaded << "\t\t<CCU>" << ccu << "</CCU>" << std::endl;
tobeuploaded << "\t\t<I2CCHANNEL>" << i2cChannel << "</I2CCHANNEL>" << std::endl;
tobeuploaded << "\t\t<I2CADDRESS>" << i2cAddress << "</I2CADDRESS>" << std::endl;


tobeuploaded << "\t\t<SOFTWARETAGVERSION>" << softVersion << "</SOFTWARETAGVERSION>" << std::endl;
tobeuploaded << "\t\t<FEDID>" << fedId << "</FEDID>" << std::endl;
tobeuploaded << "\t\t<FEDCHANNEL>" << fedChannel << "</FEDCHANNEL>" << std::endl;
tobeuploaded << "\t\t<ERR_TRIGGER>" << trigger << "</ERR_TRIGGER>" << std::endl;
tobeuploaded << "\t\t<PSUID>" << psuId << "</PSUID>" << std::endl;
tobeuploaded << "\t\t<CRATE>" << crate << "</CRATE>" << std::endl;
tobeuploaded << "\t\t<SLOT>" << slot << "</SLOT>" << std::endl;
tobeuploaded << "\t\t<SOFTWARE>" << software << "</SOFTWARE>" << std::endl;
tobeuploaded << "\t\t<SUBCLASSPATH>" << nestedClassPath << "</SUBCLASSPATH>" << std::endl;
tobeuploaded << "\t\t<NESTEDFILENAME>" << nestedFileName << "</NESTEDFILENAME>" << std::endl;
tobeuploaded << "\t\t<NESTEDLINENUMBER>" << nestedLineNumber << "</NESTEDLINENUMBER>" << std::endl;
tobeuploaded << "\t\t<XTRA_BUFFER>" << xtra_buffer << "</XTRA_BUFFER>" << std::endl;

tobeuploaded << "\t</RAWDIAGNOSTICERROR>" << std::endl;
     
tobeuploaded << "</ROWSET>" << std::endl;
     


//std::cout << "BUILT DB BUFFER : " << std::endl;
//std::cout << tobeuploaded.str() << std::endl;




long long timebegin, timeend, insertiontime;

	struct timeval tv_begin;
	struct timeval tv_end;





  try {
	gettimeofday(&tv_begin,NULL);

      dbAccess_->uploadDiagnoticXMLClob(tobeuploaded.str()) ;
	gettimeofday(&tv_end,NULL);

      
    //std::cout << "SUCCESS while uploading logs into DB" << std::endl;
    
    //std::cout << "Insertion started at " << tv_begin.tv_sec << " seconds " << tv_begin.tv_usec << " microseconds" << std::endl;
    //std::cout << "Insertion ended at " << tv_end.tv_sec << " seconds " << tv_end.tv_usec << " microseconds" << std::endl;

timebegin = tv_begin.tv_sec*1000000 + tv_begin.tv_usec;
timeend = tv_end.tv_sec*1000000 + tv_end.tv_usec;

insertiontime = timeend - timebegin;
//std::cout << "Overall insertion time in microseconds is : " << insertiontime << std::endl;
summedSuccessfulInsertionTime = summedSuccessfulInsertionTime + insertiontime;

if (insertiontime > maxDbUploadTime) maxDbUploadTime = insertiontime;
if (insertiontime < minDbUploadTime) minDbUploadTime = insertiontime;

	successLogsUploadAttempts++;
  }
  catch (FecExceptionHandler &e) {
    std::cout << "ERROR while uploading logs into DB : ";
    std::cerr << e.what() << std::endl ;
    failLogsUploadAttempts++;
  }
  catch (...) {
//    std::cerr << e.what() << std::endl ;
    std::cout << "ERROR (other than FecExceptionHandler) while uploading logs into DB" << std::endl;
    failLogsUploadAttempts++;
  }
tobeuploaded.str("") ;

				#endif
				
			}//End of if dataset is valid

		}
	}

/*
//FILTER_BEGIN
	std::cout << "Successfully uploaded " << successLogsUploadAttempts << " logs" << std::endl;
	std::cout << "Ignored " << fedMonitorLogAmount_ << " FED monitor logs" << std::endl;
	std::cout << "Ignored " << dcuStructErrorsLogAmount_ << " stacked DCU errors logs" << std::endl;
	std::cout << "Ignored " << (fecWorkLoopErrorsLogAmount_ - 1) << " FEC workloop saturating errors logs" << std::endl;
	std::cout << "Failed uploads concerns : " << failLogsUploadAttempts << " logs" << std::endl;
	std::cout << storedDcuErrorStructs_ << "DCU errors structure created."<< std::endl;
	int dcuStructCounter = 0;
	if (dcuStructList_.empty() == false)
	{
		//std::cout << "Got at least one DCU record" << std::endl;
		std::vector<dcuStructure>::iterator dcuIterator = dcuStructList_.begin();
		for (dcuIterator = dcuStructList_.begin(); dcuIterator < dcuStructList_.end(); dcuIterator++)
		{
			dcuStructCounter++;
			std::cout << "\tDCU STRUCT " << dcuStructCounter << " : ";
			struct dcuStructure oneLclDcuStruct;
			oneLclDcuStruct = *(dcuIterator);
			std::cout << " DCU crate = " << oneLclDcuStruct.crate << " ";
			std::cout << " DCU slot = " << oneLclDcuStruct.slot << " ";
			std::cout << " DCU ring = " << oneLclDcuStruct.ring << " ";
			std::cout << " DCU ccu = " << oneLclDcuStruct.ccu << " ";
			std::cout << " DCU i2cChannel = " << oneLclDcuStruct.i2cChannel << " ";
			std::cout << " DCU i2cAddress = " << oneLclDcuStruct.i2cAddress << " ";
			std::cout << " DCU occurences = " << oneLclDcuStruct.occurences << " ";
			std::cout << std::endl;

			//Prepare log structure summary in case of DCU errors stacking




		}
	}
	else std::cout << "DCU record vector is empty." << std::endl;
//FILTER_END
*/



isInProcessingStage = false;
//std::cout << "Exiting readLogsFromFile(0) ; isInProcessingStage now setted to FALSE ; logsReadFromFile = " << logsReadFromFile << std::endl;
}




