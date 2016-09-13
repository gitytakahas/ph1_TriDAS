/*

   FileName : 		AjaxLogReaderWithFileServer.cc

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


#include "AjaxLogReaderWithFileServer.h"

#define NOT_EOF !(feof(appletUidPointer->sourceFile_))

#define MAX_LOGS_PER_BURST	25


#define ONE_LINE_LENGTH	1100

XDAQ_INSTANTIATOR_IMPL(AjaxLogReaderWithFileServer);





#ifdef WILL_USE_GWT_FIREITEMS_VIEWER
	AjaxLogReaderWithFileServer::AjaxLogReaderWithFileServer(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): xdaq::Application(s), JsInterface(s)
#else
	AjaxLogReaderWithFileServer::AjaxLogReaderWithFileServer(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): xdaq::Application(s)
#endif
{




	//Define FSM states
	fsm_.addState ('C', "Configured") ;
	fsm_.addState ('H', "Halted");
	fsm_.addStateTransition ('C','H', EXECTRANSCH, this, &AjaxLogReaderWithFileServer::haltAction);
	fsm_.addStateTransition ('H','C', EXECTRANSHC, this, &AjaxLogReaderWithFileServer::configureAction);
	fsm_.setFailedStateTransitionAction( this, &AjaxLogReaderWithFileServer::failedTransition );

	//Initialize FSM state at load
	fsm_.setInitialState('H');
	fsm_.reset();

	// Import/Export the stateName variable
	getApplicationInfoSpace()->fireItemAvailable("stateName", &fsm_.stateName_);





handleIncomingJsonConnection_ = new toolbox::BSem(toolbox::BSem::FULL);
	//Initialise input logs file pointer
	//fileOut_ = NULL;

	//Initialise input logs file state
	//fileoutIsOpen_ = false;

//CCC	    
	//filePos_ = 0;

//lastLogReadInFile_ = 0;



	// Display module version on console at load time
	std::stringstream mmesg;
	mmesg << "Process version " << AJAXLOGREADERWITHFILESERVER_PACKAGE_VERSION << " loaded";
	this->getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);
	LOG4CPLUS_INFO(this->getApplicationLogger(), mmesg.str());
	this->getApplicationLogger().setLogLevel(ERROR_LOG_LEVEL);


	// Set Default() method state as : initialising....
	internalState_ = 0;

	//logcounter = 0;



#ifdef WILL_COMPILE_FOR_PIXELS
//FILTERING-BEGIN
allowFedDebugDisplay_ = true;
allowDcuAccessErrorsDisplay_ = true;
allowDcuWorkloopFullWarnDisplay_ = true;
#else
allowFedDebugDisplay_ = false;
allowDcuAccessErrorsDisplay_ = false;
allowDcuWorkloopFullWarnDisplay_ = false;
#endif
//FILTERING-GWT
validateLogForDisplay_ = false;

//FILTERING-END


	allowSendTraceLevels_ = true;
	allowSendDebugLevels_ = true;
	allowSendInfoLevels_ = true;
	allowSendWarnLevels_ = true;
	allowSendUserinfoLevels_ = true;
	allowSendErrorLevels_ = true;
	allowSendFatalLevels_ = true;


	//logsBurstLength_ = MAX_LOGS_PER_BURST;

			
	// Instanciate diagnostic object
	diagService_ = new DiagBagWizard(
                                    getApplicationDescriptor()->getClassName() + "DiagLvlOne" ,
                                    this->getApplicationLogger(),
                                    getApplicationDescriptor()->getClassName(),                                    getApplicationDescriptor()->getInstance(),
                                    getApplicationDescriptor()->getLocalId(),
                                    (xdaq::WebApplication *)this );


	// bind xgi and xoap commands specific to this application
	xgi::bind(this,&AjaxLogReaderWithFileServer::Default, "Default");
	xgi::bind(this,&AjaxLogReaderWithFileServer::Default1, "Default1");
	xgi::bind(this,&AjaxLogReaderWithFileServer::callDiagSystemPage, "callDiagSystemPage");
	xgi::bind(this,&AjaxLogReaderWithFileServer::callFsmPage, "callFsmPage");

	xgi::bind(this,&AjaxLogReaderWithFileServer::configureStateMachine, "configureStateMachine");
	xgi::bind(this,&AjaxLogReaderWithFileServer::stopStateMachine, "stopStateMachine");

	xgi::bind(this,&AjaxLogReaderWithFileServer::getJSONDataList, "getJSONDataList");
	xgi::bind(this,&AjaxLogReaderWithFileServer::getJsJSONDataList, "getJsJSONDataList");

	xgi::bind(this,&AjaxLogReaderWithFileServer::getJSONConnectionParam, "getJSONConnectionParam");
	xgi::bind(this,&AjaxLogReaderWithFileServer::getJsJSONConnectionParam, "getJsJSONConnectionParam");


	xgi::bind(this,&AjaxLogReaderWithFileServer::getJSONDiskTree, "getJSONDiskTree");
	xgi::bind(this,&AjaxLogReaderWithFileServer::getJsJSONDiskTree, "getJsJSONDiskTree");


//FILTERING-BEGIN
	xgi::bind(this,&AjaxLogReaderWithFileServer::viewFedDebugPage, "viewFedDebugPage");
	xgi::bind(this,&AjaxLogReaderWithFileServer::viewDcuWorkLoopFullWarnPage, "viewDcuWorkLoopFullWarnPage");
	xgi::bind(this,&AjaxLogReaderWithFileServer::viewDcuAccessErrorPage, "viewDcuAccessErrorPage");
//FILTERING-END



	//Bind diagnostic applications
	xgi::bind(this,&AjaxLogReaderWithFileServer::configureDiagSystem, "configureDiagSystem");
	xgi::bind(this,&AjaxLogReaderWithFileServer::applyConfigureDiagSystem, "applyConfigureDiagSystem");



/* ADDONS FOR FILES PARSING/READING */
            xgi::bind(this,&AjaxLogReaderWithFileServer::callParseDiskPage, "callParseDiskPage");



	// Call a macro needed for running a diagService in this application
	DIAG_DECLARE_LR_APP



	// Bind SOAP callbacks for FSM control messages 
	xoap::bind (this, &AjaxLogReaderWithFileServer::fireEvent, EXECTRANSCH, XDAQ_NS_URI);
	xoap::bind (this, &AjaxLogReaderWithFileServer::fireEvent, EXECTRANSHC, XDAQ_NS_URI);

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

//fileSelectionOngoing_ = false;
//appletUIDSelectingFile_ = "VOID";
//pointerToAppSelectingFile_ = NULL;

}



AjaxLogReaderWithFileServer::~AjaxLogReaderWithFileServer()
{

	delete handleIncomingJsonConnection_;

	for (unsigned int i = 0; i < AppletsUidManagementList_.size(); i++)
	{
		AppletsUidManagement * localAppletUidPointer = *(AppletsUidManagementList_.begin() + i);

		if (localAppletUidPointer->sourceFileIsOpen_ == true)
		{
			if (localAppletUidPointer->sourceFile_ != NULL)
			{
				fclose(localAppletUidPointer->sourceFile_);
				localAppletUidPointer->sourceFile_ = NULL;
				localAppletUidPointer->sourceFileIsOpen_ = false;
			}
		}
	}



	std::cout << "Deleting AppletsUidManagement list" << std::endl;
	while (AppletsUidManagementList_.empty() == false)
	{
		AppletsUidManagement * appletUidPointer = *(AppletsUidManagementList_.end() - 1);
		delete(appletUidPointer);
		AppletsUidManagementList_.pop_back();
	}
	std::cout << "AppletsUidManagement list items deleted" << std::endl;

}


// Returns the HTML page displayed from the Default1() method
void AjaxLogReaderWithFileServer::getDefault1Content(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	*out << "<br>";
	*out << "Ajax Logs Reader Default page";
	*out << "<br>";
	*out << "<br>";

	std::string lclFileNAme;
	if (diagService_->logFileName_ == "" )
	{
		lclFileNAme = "Undefined log file until first access attempt";
	}
	else lclFileNAme = diagService_->logFileName_;
	*out << "Log Reader is reading from file : " << lclFileNAme.c_str() << std::endl;
	*out << "<br>";
}



/* ADDONS FOR FILES PARSING/READING */
/* Returns the HTML page displayed from the Default() method when the <default_page> link is clicked */
void AjaxLogReaderWithFileServer::getRepositoryContentPage(xgi::Input * in, xgi::Output * out, std::string repositoryToParse ) throw (xgi::exception::Exception)
{






	//Get repository content
	getDiskContent(repositoryToParse);

	//Now build page content
	displayDiskContent(in, out);

}



// Returns the HTML page for FSM status
void AjaxLogReaderWithFileServer::getFsmStatePage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
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





/* xgi method called when the default link is clicked */
void AjaxLogReaderWithFileServer::Default1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 0;
	Default(in, out);
}


/* ADDONS FOR FILES PARSING/READING */
        /* xgi method called when the link <display_diagsystem> is clicked */
        void AjaxLogReaderWithFileServer::callParseDiskPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
	
		//A repository link has been clicked
		//Change current repository_ value so the newq rep content will be displayed
		//when Default() calls getRepositoryContentPage(in, out, currentRepository_);
	
		
		std::string requestedRepository = extractParameterValueFromURL(in, "REPOSITORY=");
		std::cout << "Request to move to repository : " << requestedRepository << std::endl;

		std::string requestedFile = extractParameterValueFromURL(in, "FILENAME=");
		std::cout << "Request to process file : " << requestedFile << std::endl;

/*
		//IF a repository change is detected (VOID operation onb file), then change repository
		if ((requestedFile == "VOID") || (requestedFile == ""))
		{
			currentRepository_= requestedRepository;
		}
		else
		{
			if (pointerToAppSelectingFile_ != NULL)
			{
				pointerToAppSelectingFile_->currentLogFileName_ = requestedFile;
				std::cout << "Setting pointerToAppSelectingFile_->currentLogFileName_ to proper logfilename" << std::endl;
				fileSelectionOngoing_ = false;
			}
			else
			{
				std::cout << "pointerToAppSelectingFile_ is NULL" << std::endl;
			}
		}	

*/



			
            internalState_ = 6;
            Default(in, out);
        }



// xgi method called when the link <display_diagsystem> is clicked
void AjaxLogReaderWithFileServer::callDiagSystemPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 1;
	Default(in, out);
}

// xgi method called when the link <display_fsm> is clicked
void AjaxLogReaderWithFileServer::callFsmPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 2;
	Default(in, out);
}




//FILTERING-BEGIN
// xgi method called when the link <ViewFedDebugPage> is clicked
void AjaxLogReaderWithFileServer::viewFedDebugPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 3;
	Default(in, out);
}

void AjaxLogReaderWithFileServer::viewDcuWorkLoopFullWarnPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 4;
	Default(in, out);
}


void AjaxLogReaderWithFileServer::viewDcuAccessErrorPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 5;
	Default(in, out);
}

//FILTERING-END



// Displays the available HyperDaq links for this process
void AjaxLogReaderWithFileServer::displayLinks(xgi::Input * in, xgi::Output * out)
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
	*out << "<a href=" << urlDiag_ << ">Configure Ajax Logs Reader</a>";
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";

	// Display FSM page
	std::string urlFsm_ = "/";
	urlFsm_ += getApplicationDescriptor()->getURN();
	urlFsm_ += "/callFsmPage";
	*out << "<a href=" << urlFsm_ << ">FSM Access</a>";
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";


	/* Display JSON visu page */
	std::string urlJson_;
	urlJson_ += getApplicationDescriptor()->getContextDescriptor()->getURL();
//	urlJson_ += "/gwtapplets/LogReader/LogReader.html";

	urlJson_ += "/" + (std::string)PATH_TO_GWT_APPLETS_ROOT + (std::string)"/LogReader/LogReader.html";

//	*out << "<a target=\"_self\" href=" << urlJson_ << ">testSound panel</a>";
	*out << "<a target=\"_blank\" href=" << urlJson_ << ">Ajax logs display panel</a>";
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";


	/* Display JSON & fileParser visu page */
	std::string urlJsonWithFiles_;
	urlJsonWithFiles_ += getApplicationDescriptor()->getContextDescriptor()->getURL();
//	urlJson_ += "/gwtapplets/LogReader/LogReader.html";

	urlJsonWithFiles_ += "/" + (std::string)PATH_TO_GWT_APPLETS_ROOT + (std::string)"/LogReaderWithFileServer/LogReaderWithFileServer.html";

//	*out << "<a target=\"_self\" href=" << urlJson_ << ">testSound panel</a>";
	*out << "<a target=\"_blank\" href=" << urlJsonWithFiles_ << ">Ajax logs display panel with file server</a>";
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";



/* ADDONS FOR FILES PARSING/READING */

            /* Display diagnostic page */

/*
            std::string urlParseFiles_ = "/";
            urlParseFiles_ += getApplicationDescriptor()->getURN();
            urlParseFiles_ += "/callParseDiskPage";
            *out << "<a href=" << urlParseFiles_ << ">Files Navigator Page</a>";
            *out << "&nbsp;&nbsp;&nbsp;&nbsp;";

*/


//FILTERING-BEGIN
	// Allow FED debug messages display in GWT applet
	std::string urlFedDebug_ = "/";
	urlFedDebug_ += getApplicationDescriptor()->getURN();
	urlFedDebug_ += "/viewFedDebugPage";
	*out << "<br>" << std::endl;
	*out << "<br>" << std::endl;
	*out << "<br>" << std::endl;
	if (allowFedDebugDisplay_ == true)
	{
		*out << "<a href=" << urlFedDebug_ << ">Click to Disable display of Fed monitoring messages in LogReader applet</a>";
		*out << "&nbsp;&nbsp;--&nbsp;&nbsp;Current status is <b>Display Enabled</b>";
	}
	else
	{
		*out << "<a href=" << urlFedDebug_ << ">Click to Enable display of Fed monitoring messages in LogReader applet</a>";
		*out << "&nbsp;&nbsp;--&nbsp;&nbsp;Current status is <b>Display Disabled</b>";
	}





	// Allow DCU WORKLOOP FULL warn messages display in GWT applet
	std::string urlDcuWorkloopFullWarn_ = "/";
	urlDcuWorkloopFullWarn_ += getApplicationDescriptor()->getURN();
	urlDcuWorkloopFullWarn_ += "/viewDcuWorkLoopFullWarnPage";
	*out << "<br>" << std::endl;
	*out << "<br>" << std::endl;
	if (allowDcuWorkloopFullWarnDisplay_ == true)
	{
		*out << "<a href=" << urlDcuWorkloopFullWarn_ << ">Click to Disable display of -...DCU workloop full...- messages in LogReader applet</a>";
		*out << "&nbsp;&nbsp;--&nbsp;&nbsp;Current status is <b>Display Enabled</b>";

	}
	else
	{
		*out << "<a href=" << urlDcuWorkloopFullWarn_ << ">Click to Enable display of of -...DCU workloop full...- messages in LogReader applet</a>";
		*out << "&nbsp;&nbsp;--&nbsp;&nbsp;Current status is <b>Display Disabled</b>";
	}






	// Allow DCU ACCESS ERROR messages display in GWT applet
	std::string urlDcuAccessError_ = "/";
	urlDcuAccessError_ += getApplicationDescriptor()->getURN();
	urlDcuAccessError_ += "/viewDcuAccessErrorPage";
	*out << "<br>" << std::endl;
	*out << "<br>" << std::endl;
	if (allowDcuAccessErrorsDisplay_ == true)
	{
		*out << "<a href=" << urlDcuAccessError_ << ">Click to Disable display of DCU access error messages in LogReader applet</a>";
		*out << "&nbsp;&nbsp;--&nbsp;&nbsp;Current status is <b>Display Enabled</b>";
	}
	else
	{
		*out << "<a href=" << urlDcuAccessError_ << ">Click to Enable display of DCU access error messages in LogReader applet</a>";
		*out << "&nbsp;&nbsp;--&nbsp;&nbsp;Current status is <b>Display Disabled</b>";
	}



//FILTERING-END




	*out << "<br>";
}




// Default() hyperDaq method
void AjaxLogReaderWithFileServer::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{

	// Create HTML header
	*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
	*out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
	*out << "<HEAD>" << std::endl;
	*out << cgicc::title("Ajax Logs Reader") << std::endl;
	*out << "</HEAD>" << std::endl;
	xgi::Utils::getPageHeader(  out, 
								"Ajax Logs Reader", 
								getApplicationDescriptor()->getContextDescriptor()->getURL(),
								getApplicationDescriptor()->getURN(),
								"/daq/xgi/images/Application.jpg" );

//FILTERING-BEGIN

	if (internalState_ == 3)
	{
		if (allowFedDebugDisplay_ == true)
		{
			allowFedDebugDisplay_ = false;
		}
		else allowFedDebugDisplay_ = true;
	}//displayLinks


	if (internalState_ == 4)
	{
		if (allowDcuWorkloopFullWarnDisplay_ == true)
		{
			allowDcuWorkloopFullWarnDisplay_ = false;
		}
		else allowDcuWorkloopFullWarnDisplay_ = true;
	}//displayLinks


	if (internalState_ == 5)
	{
		if (allowDcuAccessErrorsDisplay_ == true)
		{
			allowDcuAccessErrorsDisplay_ = false;
		}
		else allowDcuAccessErrorsDisplay_ = true;
	}//displayLinks



//FILTERING-END



	// Display available links on top of the HTML page
	displayLinks(in, out);

	// Get page HTML content according to the selected link
	if (internalState_ == 0) getDefault1Content(in, out);
	if (internalState_ == 1) diagService_->getDiagSystemHtmlPage(in, out,getApplicationDescriptor()->getURN());
	if (internalState_ == 2) getFsmStatePage(in, out);

/* ADDONS FOR FILES PARSING/READING */
            if (internalState_ == 6) getRepositoryContentPage(in, out, currentRepository_);




	// Create HTML footer
	xgi::Utils::getPageFooter(*out);


}







// Turns the AjaxLogReaderWithFileServer into state INITIALISED (FSM point of view)
void AjaxLogReaderWithFileServer::configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
	// Apply fireitems value to internal variables and check validity
	DIAG_EXEC_FSM_INIT_TRANS
}



// Turns the AjaxLogReaderWithFileServer into state HALTED (FSM point of view)
void AjaxLogReaderWithFileServer::haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
	// Apply fireitems value to internal variables and check validity
	DIAG_EXEC_FSM_STOP_TRANS
}
   

   
// Allows the FSM to be controlled via a soap message carrying a "fireEvent" message
xoap::MessageReference AjaxLogReaderWithFileServer::fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception)
{
	//#define DEBUG_FIREEVENT
	#ifdef DEBUG_FIREEVENT
		std::cout << "In AjaxLogReaderWithFileServer : fireEvent received" << std::endl ;
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


// Called when a state transition fails in the FSM
void AjaxLogReaderWithFileServer::failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
	toolbox::fsm::FailedEvent & fe = dynamic_cast<toolbox::fsm::FailedEvent&>(*e);
	std::ostringstream msgError;
	msgError << "Failure occurred when performing transition from: " << fe.getFromState() << " to: " << fe.getToState() << " exception: " << fe.getException().what();
	diagService_->reportError (msgError.str(), DIAGERROR);
}



// Can be called via the <configure_FSM> hyperdaq HTML page of this process
void AjaxLogReaderWithFileServer::configureStateMachine(xgi::Input * in, xgi::Output * out)
{
	if (fsm_.getCurrentState() == 'H')
	{
		toolbox::Event::Reference e(new toolbox::Event(EXECTRANSHC, this));
		fsm_.fireEvent(e);
	}
	Default(in, out);
}


// Can be called via the <configure_FSM> hyperdaq HTML page of this process
void AjaxLogReaderWithFileServer::stopStateMachine(xgi::Input * in, xgi::Output * out)
{
	if (fsm_.getCurrentState() == 'C')
	{
		toolbox::Event::Reference e(new toolbox::Event(EXECTRANSCH, this));
		fsm_.fireEvent(e);
	}
	Default(in, out);
}


#ifdef AUTO_CONFIGURE_PROCESSES
void AjaxLogReaderWithFileServer::timeExpired (toolbox::task::TimerEvent& e) 
{
	DIAG_EXEC_FSM_INIT_TRANS
	std::cout << "AjaxLogReaderWithFileServer(LID=" << getApplicationDescriptor()->getLocalId() << ",Instance=" << getApplicationDescriptor()->getInstance() << ") ready" << std::endl;
}
#endif





std::string AjaxLogReaderWithFileServer::cleanField(std::string& rawField, std::string& tag)
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



std::string AjaxLogReaderWithFileServer::getTag(std::string& rawField)
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


void AjaxLogReaderWithFileServer::cleanupMsgString(std::string * msg)
{

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
    	    if ((*msg)[sl] == '&') (*msg)[sl]=' ';
        }
}







void AjaxLogReaderWithFileServer::readLogsFromFile(int command, AppletsUidManagement * appletUidPointer)
{



if (appletUidPointer->hasJustBeenInstanciated_ == true)
{
	if (appletUidPointer->isWorkingWithOnlineFile_ == false)
	{
				JSONItem newJsonItem(				"USERINFO",
										"0",
										"Hello! I'm an offline AjaxLogReader capable of parsing logFiles!",
										"Wed Jan 1 00:00:00:000000 00000",
										"tracker.cms.diagnostic.p:40600.DiagSystem.instance(0)",
										//diagService_->systemID_,
										//diagService_->subSystemID_,
										//extraBuffer,
										"SYSTEM",
										"DIAGNOSTIC SYSTEM",
										"",
										"",
										"",
										"",
										"",
										"000000",
										"",
										"",
										"",
										"",
										"",
										"",
										"",
										"",
										"",
										"",
										"",
										"",
                                                                                "",
										"undefined",
										"no_ged_used",
										"FALSE",
										appletUidPointer->ajaxspecs_);
				jsonItemsList_.push_back(newJsonItem);
	}
	appletUidPointer->hasJustBeenInstanciated_ = false;
}




//APP
//int selectedLogsReadFromFile = 0;
appletUidPointer->selectedLogsReadFromFile_ = 0;

//std::cout << "DEBUG 00" << std::endl;
	//First, get the name of the file in which we should be working
	
	//If useFileLogging_ == false then get name from GED
	//DEBUG TODO :: useFileLogging_ is ALWAYS FALSE, TO BE CORRECTED...
	//if (diagService_->useFileLogging_ == false)
	//{
//std::cout << "useFileLogging_ IS FALSE" << std::endl;



//IF order is to have the applet running with online file, then in all cases get current filename from GED




if (appletUidPointer->isWorkingWithOnlineFile_ == true)
{

//std::cout << "I am working with online file" << std::endl;
		xoap::MessageReference err_msg = xoap::createMessage();
		xoap::SOAPPart err_soap = err_msg->getSOAPPart();
		xoap::SOAPEnvelope err_envelope = err_soap.getEnvelope();
		xoap::SOAPName soapcommand = err_envelope.createName("getCurrentLogFileName", "xdaq", XDAQ_NS_URI);
		xoap::SOAPBody err_body = err_envelope.getBody();
		err_body.addBodyElement(soapcommand);

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
			
			std::ostringstream gedSpecsStream;
			
			
			gedSpecsStream << err_d->getContextDescriptor()->getURL() << "/" << err_d->getClassName() << "(" << err_d->getInstance() << "),Lid=" << err_d->getLocalId();
/*
			std::string gedSpecs = err_d->getContextDescriptor()->getURL();
			gedSpecs += "/";
			gedSpecs += err_d->getClassName();
			gedSpecs += "(";
			gedSpecs += err_d->getInstance();
			gedSpecs += "), Lid=";
			gedSpecs += err_d->getLocalId();
*/
			
/*
			std::cout << "GED getClassName is : " << err_d->getClassName() << std::endl;
			std::cout << "GED ->getContextDescriptor()->getURL() is : " << err_d->getContextDescriptor()->getURL() << std::endl;
			
			std::cout << "GED getLocalId is : " << err_d->getLocalId() << std::endl;
			std::cout << "GED getInstance is : " << err_d->getInstance() << std::endl;
*/
			//std::cout << "GEDSPECS : " << gedSpecs << std::endl;
			//std::cout << gedSpecsStream.str() << std::endl;
			appletUidPointer->gedSpecs_ = gedSpecsStream.str();


		}
		catch (xdaq::exception::Exception& e)
		{
			lclLogFileName = "";
		}
		
		//diagService_->setFilePathAndName(lclLogFileName);
		//Replace this one with an applet-specific filenames management
		appletUidPointer->currentLogFileName_ = lclLogFileName;
		
		
}
else
{


//If we are parsing an offline file then
//do nothing, filename to use has been setted during the tag extraction process
//std::cout << "I am NOT working with online file" << std::endl;

//appletUidPointer->currentLogFileName_ = "/opt/trackerDAQ/config/AjaxLogReaderWithFileParserInfos.xml";

appletUidPointer->gedSpecs_ = "not using GED";

}



//std::cout << "File to use was : " << appletUidPointer->previousLogFileName_ << std::endl << std::endl;
//std::cout << "File to use is now : " << appletUidPointer->currentLogFileName_ << std::endl << std::endl;

	//}
	//Else, If useFileLogging_ == true then do nothing - filename change has already been taken into account from the hyperdaq diagsystem page
	//else std::cout << "useFileLogging_ IS TRUE" << std::endl;

	//During next check, if filename has not changed since last access then file will not be closed
	//Replace this one with an applet-specific filenames management
	//diagService_->checkLrFileOption();
	appletUidPointer->checkAndSetIfLogFileHasChanged();




	//If the file to open is a new one
	if (appletUidPointer->logFileNameHasChanged_ == true)
	{
//std::cout << "appletUidPointer->logFileNameHasChanged_ == true" << std::endl;

		//If ANY of the registered applets sees that the filename has changed, then
		//Change filename and close/reopen file pointer for EVERY registered applet




		//for (unsigned int i = 0; i < AppletsUidManagementList_.size(); i++)
		//{
//std::cout << "Applet in deletion should read logs to : " << appletUidPointer->limitReadToLastNLogs_  << std::endl;
			//AppletsUidManagement * localAppletUidPointer = *(AppletsUidManagementList_.begin() + i);
			if (appletUidPointer->sourceFile_ != NULL)
			{
				fclose(appletUidPointer->sourceFile_);
				appletUidPointer->sourceFile_ = NULL;
				appletUidPointer->sourceFileIsOpen_ = false;
			}
			
		//}


		//Close old file
		//APP
		//if (fileOut_ != NULL) fclose(fileOut_);
		//fileoutIsOpen_ = false;
		//Notify that filename change has been taken into account
		appletUidPointer->logFileNameHasChanged_ = false;

//std::cout << "FileChange detected ; Command forced to 1" << std::endl;
		command = 1;
	}
//	else std::cout << "appletUidPointer->logFileNameHasChanged_ == false" << std::endl;
	//If targetted logfile is not opened yet
	//APP
	//if (fileoutIsOpen_ == false)
	if (appletUidPointer->sourceFileIsOpen_ == false)
	{
		appletUidPointer->sourceFile_ = fopen(appletUidPointer->currentLogFileName_.c_str(), "r");
		if (appletUidPointer->sourceFile_ != NULL)
		{
			appletUidPointer->sourceFileIsOpen_ = true;
//			CCC
			appletUidPointer->filePos_ = 0;
			appletUidPointer->logcounter_ = 0;
		}
		else
		{
			std::cout << "Unable to read from log file \"" << appletUidPointer->currentLogFileName_.c_str() << "\"" << std::endl;
		}
	}


//std::cout << "DEBUG 01" << std::endl;


	if (appletUidPointer->sourceFile_ != NULL) 
	{


//command==1 veux dire DO_REWIND
//Si le limitReadToLastNLogs_ n'est pas à -1, alors on ne rewind pas jusqu'au début du fichier
//mais jusqu'à limitReadToLastNLogs_ de la fin du fichier.
if (command==1)
{
//std::cout << "Entering Command=1 block" << std::endl;
	if (appletUidPointer->limitReadToLastNLogs_ <= 0)
	{
		//std::cout << "Rewind file to beginning" << std::endl;
		rewind(appletUidPointer->sourceFile_);
		appletUidPointer->filePos_ = 0;
		appletUidPointer->logcounter_ = 0;
	}
	else
	{
		//std::cout << "Rewind file to " << appletUidPointer->limitReadToLastNLogs_ << " logs before EOF" << std::endl;
		char oneLine[ONE_LINE_LENGTH];
		long recordsCounter = 0;

		//First count how many records we have
		rewind(appletUidPointer->sourceFile_);
		do
		{
			fgets(oneLine, ONE_LINE_LENGTH-1, appletUidPointer->sourceFile_);
			if ((std::string)oneLine == "<DATA>\n") recordsCounter++;
			
		} while (  (NOT_EOF) );
		//std::cout << "Found " << recordsCounter << " records in file" << std::endl;

		//Then advance up to N records before file head
		if (appletUidPointer->limitReadToLastNLogs_ < recordsCounter)
		{
			long recordsPositionning = 0;
			rewind(appletUidPointer->sourceFile_);
			do
			{
				fgets(oneLine, ONE_LINE_LENGTH-1, appletUidPointer->sourceFile_);
				if ((std::string)oneLine == "</DATA>\n") recordsPositionning++;
			
			} while (  (NOT_EOF) && (recordsPositionning < (recordsCounter - appletUidPointer->limitReadToLastNLogs_)) );
			//std::cout << "Repositionned after  " << recordsPositionning << " records in file" << std::endl;
/*
rewind(appletUidPointer->sourceFile_);
appletUidPointer->filePos_ = 0;
appletUidPointer->logcounter_ = 0;
*/
			appletUidPointer->filePos_ = ftell(appletUidPointer->sourceFile_);
			appletUidPointer->logcounter_ = recordsPositionning;
		}
		else
		{
			rewind(appletUidPointer->sourceFile_);
			appletUidPointer->filePos_ = 0;
			appletUidPointer->logcounter_ = 0;
		}
	}


}

		char oneLine[ONE_LINE_LENGTH];
		CLog c;	
//std::cout << "DEBUG 02" << std::endl;


//CCC

		if (appletUidPointer->filePos_ <0)
		{
			appletUidPointer->filePos_=0;
			appletUidPointer->logcounter_ = 0;
		}
		fseek(appletUidPointer->sourceFile_, appletUidPointer->filePos_, SEEK_SET);

		
		
//		while ( (NOT_EOF) && (selectedLogsReadFromFile < MAX_LOGS_PER_BURST) )
		while ( (NOT_EOF) && (appletUidPointer->selectedLogsReadFromFile_ < appletUidPointer->appletlogsBurstLength_) )
		{
			//logsReadFromFile++;
			
//std::cout << "DEBUG 03" << std::endl;


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
                        std::string partitionname="";
			std::string slot="";

			bool dataSetIsValid = false;

			(std::string)oneLine="";

			//Seek for dataset beginning
			//std::cout << "seeking for <DATA> tag" << std::endl;
			do
			{
//std::cout << "DEBUG 03-A" << std::endl;
				fgets(oneLine, ONE_LINE_LENGTH-1, appletUidPointer->sourceFile_);
//std::cout << "Read from file:" << oneLine << std::endl;

				//std::cout << "I have read : " << oneLine << std::endl;
			} while ( ((std::string)oneLine != "<DATA>\n") && (NOT_EOF) );


//std::cout << "DEBUG 04" << std::endl;

			do
			{
			
//std::cout << "DEBUG 04-A" << std::endl;
				fgets(oneLine, ONE_LINE_LENGTH-1, appletUidPointer->sourceFile_);
				//std::cout << "I have read : " << oneLine << std::endl;
//std::cout << "Read from file:" << oneLine << std::endl;

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
				else if (tag == "<LOGNUMBER>"){ logNumber=cleanField(readLine, tag);/* lastLogReadInFile_ = atol(logNumber.c_str());*//*std::cout << "From file readout : Log number " << logNumber << " read." << std::endl;*/}
				else if (tag == "<XTRA_BUFFER>")
				{
					//std::cout << "I have read extra buffer content : " << readLine << std::endl;
					xtra_buffer=cleanField(readLine, tag);
					cleanupMsgString(&xtra_buffer);
					//std::cout << "After extraction, my extra buffer is : " << xtra_buffer << std::endl;

				}
				else if (tag == "<SUBCLASSPATH>") nestedClassPath=cleanField(readLine, tag);
				else if (tag == "<NESTEDFILENAME>") nestedFileName=cleanField(readLine, tag);
				else if (tag == "<NESTEDLINENUMBER>") nestedLineNumber=cleanField(readLine, tag);
				else if (tag == "<CRATE>") crate=cleanField(readLine, tag);
				else if (tag == "<PARTITIONNAME>") partitionname=cleanField(readLine, tag);
				else if (tag == "<SLOT>") slot=cleanField(readLine, tag);


/*
				else if (tag == "<XTRA_BUFFER>")
				{
					std::string lcltag = "";
					std::string lclLineRead = "";
					do
					{
						std::cout << "DEBUG 04-B" << std::endl;

						fgets(oneLine, ONE_LINE_LENGTH-1, fileOut_);
std::cout << "Read from file:" << oneLine << std::endl;
						lclLineRead = (std::string)oneLine;
						lcltag = getTag(lclLineRead);
std::cout << "local tag extracted:" << lcltag << std::endl;
						
						if (lcltag != "</XTRA_BUFFER>")
						{
							xtra_buffer = xtra_buffer + (std::string)oneLine;
							std::cout << "xtra_buffer appended now=" << xtra_buffer << std::endl;
						}
					} while ( (lcltag != "</XTRA_BUFFER>") && (NOT_EOF) );
					//REMOVE THE \n CHAR
					if ((xtra_buffer != "") && (xtra_buffer != "NA")) xtra_buffer = xtra_buffer.substr(0, xtra_buffer.length()-1);
					std::cout << "now xtra_buffer = :" << xtra_buffer << ":" << std::endl;
				}
*/
			} while ( ((std::string)oneLine != "</DATA>\n") && (NOT_EOF) );

//std::cout << "DEBUG 05" << std::endl;



			if (NOT_EOF)
			{
				//std::cout << "VALID FOR PROCESSING" << std::endl;
				dataSetIsValid = true;
				appletUidPointer->logcounter_++;
//CCC
				appletUidPointer->filePos_ = ftell(appletUidPointer->sourceFile_);
			}
			else
			{
				//std::cout << "INVALID FOR PROCESSING" << std::endl;
				dataSetIsValid = false;
			}


			if (dataSetIsValid == true)
			{

				c.setLogger(source);
				std::string machine;
				std::string port;
				std::string procname;
				std::string procinstance;
				c.expandLoggerInfos(&procname, &procinstance, &machine, &port);




/*
			if ((xtra_buffer == "") || (xtra_buffer == "NA"))
			{
*/

#ifdef PARSE_XTRA_BUFFER
#ifdef WILL_COMPILE_FOR_TRACKER
				std::string extraBuffer = "";

				//FEC EXTRA BUFFER EXPLOITATION--DISPLAY
				if (fecHardId != "NA")
				{
					extraBuffer += " FECHARDID=";
					extraBuffer += fecHardId;

					extraBuffer += "(crate " + crate + ", slot " + slot + ")";
					
				}
				
				if (ring != "NA")
				{
					extraBuffer += " RING=";
					extraBuffer += ring;
				}

				if (ccu != "NA")
				{
					extraBuffer += " CCU=";
					extraBuffer += ccu;
				}

				if (i2cChannel != "NA")
				{
					extraBuffer += " I2CCHANNEL=";
					extraBuffer += i2cChannel;
				}

				if (i2cAddress != "NA")
				{
					extraBuffer += " I2CADDRESS=";
					extraBuffer += i2cAddress;
				}


				//FED EXTRA BUFFER EXPLOITATION--DISPLAY
				if (fedId != "NA")
				{
					extraBuffer += " FEDID=";
					extraBuffer += fedId;
					
					extraBuffer += "(crate " + crate + ", slot " + slot + ")";

				}

				if (fedChannel != "NA")
				{
					extraBuffer += " FEDCHANNEL=";
					extraBuffer += fedChannel;
				}



				//PSU EXTRA BUFFER EXPLOITATION--DISPLAY
				//std::cout << "Before psuId field check Vs. NA" << std::endl; 
				if (psuId != "NA")
				{
					//std::cout << "In psuId field check Vs. NA ; field != NA" << std::endl;
					extraBuffer += " PSUID=";
					extraBuffer += psuId;
				}// else std::cout << "In psuId field check Vs. NA ; field == NA" << std::endl;



				//ASSIGN IDENTIFIED EXTRA BUFFER CONTENT TO APPLET-DISPLAYED EBUFF
				if ((xtra_buffer == "") || (xtra_buffer == "NA"))
				{
					xtra_buffer = extraBuffer;
				}
				else xtra_buffer = extraBuffer + (std::string)" Unparsed extra buffer content=" + xtra_buffer;






#endif
#endif
/*
			}
*/			
			
//std::cout << "Building newJsonItem with Log number " << logNumber << " read." << std::endl;

				if ( (logNumber == "") || (logNumber == "NA") )
				{
					std::ostringstream tmpVar;
					tmpVar << appletUidPointer->logcounter_;
					logNumber = tmpVar.str();					
				}











//FILTERING-BEGIN

//FILTERING-GWT

validateLogForDisplay_ = true;


//std::cout << "LOG read ; now going thru validation/filters methods" << std::endl;
//std::cout << "validateLogForDisplay_ is : " << validateLogForDisplay_ << std::endl;


if (allowFedDebugDisplay_ == false)
{
		//If message concerns FED debug messages, don't upload it.
		size_t fedStringFound;
		fedStringFound=errMsg.find(FED_MONITORLOG_PATTERN);
		if (fedStringFound != std::string::npos)
		{
			validateLogForDisplay_ = false;
		}
}


if (allowDcuWorkloopFullWarnDisplay_ == false)
{
		//If message concerns FEC workloop messages, don't upload it.
		size_t fecStringFound;
		fecStringFound=errMsg.find(FEC_WORKLOOPERROR_PATTERN);
		if (fecStringFound != std::string::npos)
		{
			validateLogForDisplay_ = false;	
		}
}



if (allowDcuAccessErrorsDisplay_ == false)
{
		size_t dcuStringFound;
		//If message concerns DCU READOUT ERRORS, it's not simple...
		dcuStringFound=errMsg.find(FEC_DCUREADOUTERROR_PATTERN);
		if (dcuStringFound != std::string::npos)
		{
			validateLogForDisplay_ = false;	
		}
}


//std::cout << "Validation/filters methods passed" << std::endl;
//std::cout << "validateLogForDisplay_ is : " << validateLogForDisplay_ << std::endl;
















if (validateLogForDisplay_ == true)
{


//std::cout << "Pushing log on clog vector" << std::endl;

//std::cout << "Building JSONItem, xtra_buffer field = :" << xtra_buffer << ":" << std::endl;
/*
if (allowSendTraceLevels_ == true) {std::cout << "TRACE is TRUE";} else {std::cout << "TRACE is FALSE";} std::cout << std::endl;
if (allowSendDebugLevels_ == true) {std::cout << "DEBUG is TRUE";} else {std::cout << "DEBUG is FALSE";} std::cout << std::endl;
if (allowSendInfoLevels_ == true) {std::cout << "INFO is TRUE";} else {std::cout << "INFO is FALSE";} std::cout << std::endl;
if (allowSendWarnLevels_ == true) {std::cout << "WARN is TRUE";} else {std::cout << "WARN is FALSE";} std::cout << std::endl;
if (allowSendUserinfoLevels_ == true) {std::cout << "USERINFO is TRUE";} else {std::cout << "USERINFO is FALSE";} std::cout << std::endl;
if (allowSendErrorLevels_ == true) {std::cout << "ERROR is TRUE";} else {std::cout << "ERROR is FALSE";} std::cout << std::endl;
if (allowSendFatalLevels_ == true) {std::cout << "FATAL is TRUE";} else {std::cout << "FATAL is FALSE";} std::cout << std::endl;
*/
if (
( (level == (std::string)"TRACE") && (allowSendTraceLevels_ == true) ) ||
( (level == (std::string)"DEBUG") && (allowSendDebugLevels_ == true) ) ||
( (level == (std::string)"INFO") && (allowSendInfoLevels_ == true) ) ||
( (level == (std::string)"WARN") && (allowSendWarnLevels_ == true) ) ||
( (level == (std::string)"USERINFO") && (allowSendUserinfoLevels_ == true) ) ||
( (level == (std::string)"ERROR") && (allowSendErrorLevels_ == true) ) ||
( (level == (std::string)"FATAL") && (allowSendFatalLevels_ == true) )
)
{


//logcounter++;
appletUidPointer->selectedLogsReadFromFile_++;


std::string isWorkingOnline = "";
if (appletUidPointer->isWorkingWithOnlineFile_ == true)
{
	isWorkingOnline = "TRUE";
}
else isWorkingOnline = "FALSE";


				JSONItem newJsonItem(	level,
										errCode,
										errMsg,
										timestamp,
										source,
										//diagService_->systemID_,
										//diagService_->subSystemID_,
										//extraBuffer,
										sysId,
										subSysId,
										xtra_buffer,
										machine,
										port,
										procname,
										procinstance,
										logNumber,
										fecHardId,
										ring,
										ccu,
										i2cChannel,
										i2cAddress,
										fedId,
										fedChannel,
										crate,
										slot,
										nestedClassPath,
										nestedFileName,
										nestedLineNumber,
                                                                                partitionname,
										appletUidPointer->currentLogFileName_,
										appletUidPointer->gedSpecs_,
										isWorkingOnline,
										appletUidPointer->ajaxspecs_);


/*
std::cout << "Before JSON connection: fecHardId=" << fecHardId << ", ring=" << ring << std::endl;
				JSONItem newJsonItem(	"USERINFO",
										"AA",
										"BB",
										"CC",
										"DD",
										"EE",
										"FF",
										"GG",
										"HH",
										"II",
										"JJ",
										"KK",
										"LL",
										"MM",//FecHardId
										"NN",
										"OO",
										"PP",
										"QQ",
										"RR",
										"SS",
										"TT",
										"UU",
										"VV",
										"WW",
										"XX");
*/
			
				jsonItemsList_.push_back(newJsonItem);
}


}//end of if (validateLogForDisplay_ == true)
//else std::cout << "NOT Pushing log on clog vector" << std::endl;
			}

		} //end of while (NOT_EOF) block
	}



}




std::string AjaxLogReaderWithFileServer::extractParameterValueFromURL(xgi::Input * in, std::string parameter)
{
        cgicc::CgiEnvironment cgi(in);
		std::string queryString = cgi.getQueryString();
		//std::cout << "Querying parameter : " << parameter << " in URL : " << queryString << std::endl;

		std::string::size_type loc_begin = queryString.find( parameter, 0 );

		if (loc_begin != std::string::npos)
		{
			//std::cout << "Occurence found at string index : " << loc_begin << std::endl;

			char param_value[1000];
			int counter=0;
			int i=(loc_begin+strlen(parameter.c_str()));
			while ( ((queryString.c_str())[i] != '&') && ((unsigned int)i<(queryString.length())) && (counter<990) )
			{
				param_value[counter] = (queryString.c_str())[i];
				counter++;
				i++;
			}
			param_value[counter] = '\0';
			return (std::string)param_value;
		}
		else
		{
			//std::cout << "Occurence " << parameter <<" not found in URL." << std::endl;
			return (std::string)"";
		}
}





        int AjaxLogReaderWithFileServer::processRequestParameters(xgi::Input * in, std::string & appletUidPtr, int & appletlogsBurstLengthPtr, int & limitReadToLastNLogsPointer,
								std::string & logFileNameToUSeForApplet, std::string & repositoryNameToUseForApplet, bool & appletIsWorkingWithOnlineFile,
								bool & setAppletRepository, bool & setAppletLogFile, bool & toggleFileChange)
        {
		

		//oneLogfileHasBeenSelected_ = false;
		cgicc::CgiEnvironment cgi(in);
		std::string queryString = cgi.getQueryString();

					/* ADDONS FOR FILES PARSING/READING */

			std::string repositoryNameToUse = extractParameterValueFromURL(in, "REPOSITORY=");
			//std::cout << "Extracted repository : " << repositoryNameToUse << std::endl;
			if ((repositoryNameToUse != "") && (repositoryNameToUse != "VOID")) setAppletRepository = true;
			


			std::string logFileNameToUse = extractParameterValueFromURL(in, "FILENAME=");
			//if ((logFileNameToUse != "") && (logFileNameToUse != "VOID")) logFileNameToUSeForApplet_ = logFileNameToUse;
			//std::cout << "Extracted filename : " << logFileNameToUse << std::endl;
			if ((logFileNameToUse != "") && (logFileNameToUse != "VOID")) setAppletLogFile = true;



			//Check if some action has been requested on file selection from applet
			std::string fileSelectionOngoing = extractParameterValueFromURL(in, "FILESELECTIONONGOING=");
			//std::cout << "Extracted fileSelectionOngoing : " << fileSelectionOngoing << std::endl;

			//std::cout << "Extracted doRewindparameter : " << doRewindparameter << std::endl;


			//If filename selection is not void, then a-priori use this logfile as data source
			if ( (logFileNameToUse != "") && (logFileNameToUse != "VOID") && (repositoryNameToUse != "") && (repositoryNameToUse != "VOID") && (fileSelectionOngoing == (std::string)"TRUE")/* && (doRewindparameter == (std::string)"TRUE") */)
			{
			
				//std::cout << "FILECHANGE CONDITIONS OK" << std::endl;
				repositoryNameToUseForApplet = repositoryNameToUse;
				//In this case, the filename rethrown by the applet already contains the full path + filename
				logFileNameToUSeForApplet = logFileNameToUse;
				//oneLogfileHasBeenSelected_ = true;
				toggleFileChange = true;
			
			}
			else
			{
				//std::cout << "FILECHANGE CONDITIONS NOT FULLFILLED" << std::endl;
				repositoryNameToUseForApplet = "";
				logFileNameToUSeForApplet = "";
				//Rem : 
			}



			//If repository selection is not void but filename selection is void,
			//then move to new repository but do not select ant logfile
			//Variable used in getJSONDiskTree() calls
			if ( (repositoryNameToUse != "") && (repositoryNameToUse != "VOID") && (fileSelectionOngoing == (std::string)"TRUE") )
			{
				repositoryNameToUseForApplet = repositoryNameToUse;
			}
			else repositoryNameToUseForApplet = "";



			std::string workWithOnlineFile = extractParameterValueFromURL(in, "ISONLINE=");
			//std::cout << "Found value for parameter FATAL : " << sendFatalparameter << std::endl;
			//if (workWithOnlineFile == (std::string)"TRUE") appletIsWorkingWithOnlineFile_=true;

			if (workWithOnlineFile == (std::string)"TRUE")
			{
				appletIsWorkingWithOnlineFile=true;
				//std::cout << "Applet requested to work with online file" << std::endl;
			}
			else
			{
				appletIsWorkingWithOnlineFile=false;
				//std::cout << "Applet requested to work with offline file" << std::endl;
			}

			

			//Check if some action has been requested on file selection from applet
			//std::string fileSelectionOngoing = extractParameterValueFromURL(in, "FILESELECTIONONGOING=");
			/*
			if ( (fileSelectionOngoing == (std::string)"TRUE") && (fileSelectionOngoing_ == false) )
			{
				std::cout << "Entering logFile selection process on applet request" << std::endl;
				fileSelectionOngoing_ = true;
				appletUIDSelectingFile_ = appletUidparameter;
				//pointerToAppSelectingFile_
			}
			else std::cout << "No logFile selection process requestyed by applet" << std::endl;
			*/
//std::cout << "At exit : oneLogfileHasBeenSelected_ is : ";
//if (oneLogfileHasBeenSelected_==false) {std::cout << "FALSE";} else std::cout << "TRUE"; std::cout << std::endl;
		
		
		return(processRequestParameters(in, appletUidPtr, appletlogsBurstLengthPtr, limitReadToLastNLogsPointer));
		
	}


        int AjaxLogReaderWithFileServer::processRequestParameters(xgi::Input * in, std::string & appletUidPtr, int & appletlogsBurstLengthPtr, int & limitReadToLastNLogsPointer)
        {
			//oneLogfileHasBeenSelected_ = false;
	
			cgicc::CgiEnvironment cgi(in);
			std::string queryString = cgi.getQueryString();
			//std::cout << "Working with URL extension : " << queryString << std::endl;
			
			std::string doRewindparameter = extractParameterValueFromURL(in, "DO_REWIND=");
			//std::cout << "Found value for parameter DO_REWIND : " << doRewindparameter << std::endl;


			std::string sendTraceparameter = extractParameterValueFromURL(in, "TRACE=");
			//std::cout << "Found value for parameter TRACE : " << sendTraceparameter << std::endl;
			if (sendTraceparameter == (std::string)"TRUE") allowSendTraceLevels_=true;
			if (sendTraceparameter == (std::string)"FALSE") allowSendTraceLevels_=false;

			std::string sendDebugparameter = extractParameterValueFromURL(in, "DEBUG=");
			//std::cout << "Found value for parameter DEBUG : " << sendDebugparameter << std::endl;
			if (sendDebugparameter == (std::string)"TRUE") allowSendDebugLevels_=true;
			if (sendDebugparameter == (std::string)"FALSE") allowSendDebugLevels_=false;

			std::string sendInfoparameter = extractParameterValueFromURL(in, "INFO=");
			//std::cout << "Found value for parameter INFO : " << sendInfoparameter << std::endl;
			if (sendInfoparameter == (std::string)"TRUE") allowSendInfoLevels_=true;
			if (sendInfoparameter == (std::string)"FALSE") allowSendInfoLevels_=false;

			std::string sendWarnparameter = extractParameterValueFromURL(in, "WARN=");
			//std::cout << "Found value for parameter WARN : " << sendWarnparameter << std::endl;
			if (sendWarnparameter == (std::string)"TRUE") allowSendWarnLevels_=true;
			if (sendWarnparameter == (std::string)"FALSE") allowSendWarnLevels_=false;

			std::string sendUserinfoparameter = extractParameterValueFromURL(in, "USERINFO=");
			//std::cout << "Found value for parameter USERINFO : " << sendUserinfoparameter << std::endl;
			if (sendUserinfoparameter == (std::string)"TRUE") allowSendUserinfoLevels_=true;
			if (sendUserinfoparameter == (std::string)"FALSE") allowSendUserinfoLevels_=false;

			std::string sendErrorparameter = extractParameterValueFromURL(in, "ERROR=");
			//std::cout << "Found value for parameter ERROR : " << sendErrorparameter << std::endl;
			if (sendErrorparameter == (std::string)"TRUE") allowSendErrorLevels_=true;
			if (sendErrorparameter == (std::string)"FALSE") allowSendErrorLevels_=false;

			std::string sendFatalparameter = extractParameterValueFromURL(in, "FATAL=");
			//std::cout << "Found value for parameter FATAL : " << sendFatalparameter << std::endl;
			if (sendFatalparameter == (std::string)"TRUE") allowSendFatalLevels_=true;
			if (sendFatalparameter == (std::string)"FALSE") allowSendFatalLevels_=false;



			std::string logsBurstLengthparameter = extractParameterValueFromURL(in, "LOGS_BURST_LENGTH=");
			//std::cout << "Found value for parameter FATAL : " << sendFatalparameter << std::endl;
			/*
			if (sendFatalparameter == (std::string)"TRUE") allowSendFatalLevels_=true;
			if (sendFatalparameter == (std::string)"FALSE") allowSendFatalLevels_=false;
			*/
			if (logsBurstLengthparameter != "")
			{
				//logsBurstLength_ = atoi(logsBurstLengthparameter.c_str());
				appletlogsBurstLengthPtr = atoi(logsBurstLengthparameter.c_str());
			}
//			std::cout << "Now reading bursts of " << logsBurstLength_ << " messages." << std::endl;
			
			std::string appletUidparameter = extractParameterValueFromURL(in, "APPLET_UID=");
//			if (appletUidparameter != "") (*appletUidPtr) = atoi(appletUidparameter.c_str());
			appletUidPtr = appletUidparameter;



			std::string limitReadToLastNLogsParameter = extractParameterValueFromURL(in, "READ_LIMITED_LOGS_NUMBER=");
			if (limitReadToLastNLogsParameter != "") limitReadToLastNLogsPointer = atoi(limitReadToLastNLogsParameter.c_str());



			if (doRewindparameter == (std::string)"TRUE") {return(1);} else return(0);
		}




        void AjaxLogReaderWithFileServer::getJSONDataList(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
	    handleIncomingJsonConnection_->take();

//std::cout << "1 : "; if (oneLogfileHasBeenSelected_==false) {std::cout << "oneLogfileHasBeenSelected_ is : FALSE";} else std::cout << "oneLogfileHasBeenSelected_ is : TRUE"; std::cout << std::endl;

		std::string receivedAppletUid = "";
		int appletlogsBurstLengthPtr = 0;
		int limitReadToLastNLogsPtr = -2;
//		int filemove=processRequestParameters(in, receivedAppletUid, appletlogsBurstLengthPtr, limitReadToLastNLogsPtr);

		std::string logFileNameToUSeForApplet;
		std::string repositoryNameToUseForApplet;
		bool appletIsWorkingWithOnlineFile;
		
		bool setAppletRepository = false;
		bool setAppletLogFile = false;
		bool toggleFileChange = false;
		
		
		int filemove=processRequestParameters(in, receivedAppletUid, appletlogsBurstLengthPtr, limitReadToLastNLogsPtr,
							logFileNameToUSeForApplet, repositoryNameToUseForApplet,appletIsWorkingWithOnlineFile,
							setAppletRepository, setAppletLogFile, toggleFileChange);


//std::cout << "At data connection tryout : Applet UID=" << receivedAppletUid << std::endl;

		//std::cout << "Querying data for applet UID " << receivedAppletUid << std::endl;
		//std::cout << "Burst length locally setted to " << appletlogsBurstLengthPtr << std::endl; 
		AppletsUidManagement * appletUidPointer = NULL;
		//bool appletUidFoundInList = false;
		//std::cout << "Checking if Applet UID " << receivedAppletUid << " exists in list of managed applets" << std::endl;
		for (unsigned int i = 0; i < AppletsUidManagementList_.size(); i++)
		{
			//appletUidPointer = *(AppletsUidManagementList_.begin() + i);
			//std::cout << "Comparing with applet UID " << (*(AppletsUidManagementList_.begin() + i))->getAppletUid() << std::endl;
			if ((*(AppletsUidManagementList_.begin() + i))->getAppletUid() == receivedAppletUid)

			{
				appletUidPointer = *(AppletsUidManagementList_.begin() + i);
			}
		}
//std::cout << "2 : "; if (oneLogfileHasBeenSelected_==false) {std::cout << "oneLogfileHasBeenSelected_ is : FALSE";} else std::cout << "oneLogfileHasBeenSelected_ is : TRUE"; std::cout << std::endl;
		if (appletUidPointer == NULL)
		{
		
//std::cout << "At data connection tryout : NO POINTER FOUND fort applet UID=" << receivedAppletUid << std::endl;

			//std::cout << "No applet with UID " << receivedAppletUid << " found in applets pool" << std::endl;
			//Crafter un return JSON array avec failure notification
			JSONUtils jsonUtility;
			*out << jsonUtility.craftJsonBadAppletArray();
			/*
			if (fileSelectionOngoing_ == true)
			{
				pointerToAppSelectingFile_= NULL;
				fileSelectionOngoing_ = false;
				std::cout << "detected NULL appletUidPointer ; forcing fileSelectionOngoing_ to false" << std::endl;
			}
			*/
				

		}
		else
		{
//std::cout << "At data connection tryout : Pointer found fort applet UID=" << appletUidPointer->appletUID_ << std::endl;
//std::cout << "At data connection tryout : Applet UID=" << appletUidPointer->appletUID_ << " uses file " << appletUidPointer->currentLogFileName_ << std::endl;
//std::cout << "At data connection tryout : Applet UID=" << appletUidPointer->appletUID_ << " was using file " << appletUidPointer->previousLogFileName_ << std::endl;
			/*
			if (fileSelectionOngoing_ == true)
			{
				pointerToAppSelectingFile_= appletUidPointer;
				//fileSelectionOngoing_ == false;
				std::cout << "detected NON-NULL appletUidPointer and fileSelectionOngoing_ is true ; forcing appletUID" << std::endl;
			}
			else std::cout << "fileSelectionOngoing_ = false ; not setting anything" << std::endl;
			*/
			if (appletlogsBurstLengthPtr > 0)
			{
				appletUidPointer ->appletlogsBurstLength_ = appletlogsBurstLengthPtr;
			}
			else appletUidPointer ->appletlogsBurstLength_ = MAX_LOGS_PER_BURST;
			//oldLimitReadToLastNLogsPtr = appletUidPointer ->limitReadToLastNLogs_;
			if ( (limitReadToLastNLogsPtr == -1) || (limitReadToLastNLogsPtr > 0) )
			{
				appletUidPointer ->limitReadToLastNLogs_ = limitReadToLastNLogsPtr;
//std::cout << "Applet with UID " << appletUidPointer->getAppletUid() << " has his appletUidPointer ->limitReadToLastNLogs_ setted to " << appletUidPointer ->limitReadToLastNLogs_ << std::endl;
			}
			//else appletUidPointer ->limitReadToLastNLogs_ = -1;
			//std::cout << "Applet with UID " << appletUidPointer->getAppletUid() << " found in applets pool ; reading data with bursts of " << appletUidPointer->appletlogsBurstLength_ << " messages length" << std::endl;

/* ADDONS FOR FILES PARSING/READING */
//appletUidPointer ->currentLogFileName_ = logFileNameToUSeForApplet_;
//appletUidPointer ->currentRepositoryName_ = repositoryNameToUseForApplet_;



			if (setAppletRepository == true) appletUidPointer->currentRepositoryName_ = repositoryNameToUseForApplet;
			if (setAppletLogFile == true) appletUidPointer->currentLogFileName_ = logFileNameToUSeForApplet;

			if (toggleFileChange == true)
			{
				//std::cout << "File change requested from applet ; trying to work with file : " << repositoryNameToUseForApplet_ << "/" << logFileNameToUSeForApplet_ << std::endl;
				appletUidPointer->currentLogFileName_ = repositoryNameToUseForApplet + "/" + logFileNameToUSeForApplet;
				//appletUidPointer ->isWorkingWithOnlineFile_ = appletIsWorkingWithOnlineFile_;

			}
setAppletRepository = false;
setAppletLogFile = false;
toggleFileChange = false;



	if (appletIsWorkingWithOnlineFile == true)
	{
		appletUidPointer->isWorkingWithOnlineFile_ = true;
	}
	else appletUidPointer->isWorkingWithOnlineFile_ = false;

			
			JSONUtils jsonUtility;
			readLogsFromFile(filemove, appletUidPointer);
			*out << jsonUtility.craftJsonArray(&jsonItemsList_);
		}

		handleIncomingJsonConnection_->give();

	}



        void AjaxLogReaderWithFileServer::getJsJSONDataList(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
	    handleIncomingJsonConnection_->take();

		std::string receivedAppletUid = "";
		int appletlogsBurstLengthPtr = 0;
		int limitReadToLastNLogsPtr = -2;
		//int filemove=processRequestParameters(in, receivedAppletUid, appletlogsBurstLengthPtr, limitReadToLastNLogsPtr);



		std::string logFileNameToUSeForApplet;
		std::string repositoryNameToUseForApplet;
		bool appletIsWorkingWithOnlineFile;
		bool setAppletRepository = false;
		bool setAppletLogFile = false;
		bool toggleFileChange = false;		
		int filemove=processRequestParameters(in, receivedAppletUid, appletlogsBurstLengthPtr, limitReadToLastNLogsPtr,
							logFileNameToUSeForApplet, repositoryNameToUseForApplet,appletIsWorkingWithOnlineFile,
							setAppletRepository, setAppletLogFile, toggleFileChange);


		//std::cout << "Querying data for applet UID : " << receivedAppletUid << std::endl;
		//std::cout << "Burst length locally setted to " << appletlogsBurstLengthPtr << std::endl; 
		AppletsUidManagement * appletUidPointer = NULL;
		//bool appletUidFoundInList = false;
		//std::cout << "Checking if Applet UID " << receivedAppletUid << " exists in list of managed applets" << std::endl;
		for (unsigned int i = 0; i < AppletsUidManagementList_.size(); i++)
		{
			//appletUidPointer = *(AppletsUidManagementList_.begin() + i);
			//std::cout << "Comparing with applet UID " << (*(AppletsUidManagementList_.begin() + i))->getAppletUid() << std::endl;
			if ((*(AppletsUidManagementList_.begin() + i))->getAppletUid() == receivedAppletUid)

			{
				appletUidPointer = *(AppletsUidManagementList_.begin() + i);
			}
		}

		if (appletUidPointer == NULL)
		{
			//std::cout << "No applet with UID " << receivedAppletUid << " found in applets pool" << std::endl;
			//Crafter un return JSON array avec failure notification
			JSONUtils jsonUtility;
			*out << "jsonCallback(" << jsonUtility.craftJsJsonBadAppletArray() << ")";
			/*
			if (fileSelectionOngoing_ == true)
			{
				pointerToAppSelectingFile_= NULL;
				fileSelectionOngoing_ = false;
			}
			*/

		}
		else
		{
			if (appletlogsBurstLengthPtr > 0)
			{
				appletUidPointer ->appletlogsBurstLength_ = appletlogsBurstLengthPtr;
			}
			else appletUidPointer ->appletlogsBurstLength_ = MAX_LOGS_PER_BURST;
			if ( (limitReadToLastNLogsPtr == -1) || (limitReadToLastNLogsPtr > 0) )
			{
				appletUidPointer ->limitReadToLastNLogs_ = limitReadToLastNLogsPtr;
//std::cout << "Applet with UID " << appletUidPointer->getAppletUid() << " has his appletUidPointer ->limitReadToLastNLogs_ setted to " << appletUidPointer ->limitReadToLastNLogs_ << std::endl;
			}
			//std::cout << "Applet with UID " << appletUidPointer->getAppletUid() << " found in applets pool ; reading data with bursts of " << appletUidPointer->appletlogsBurstLength_ << " messages length" << std::endl;


/* ADDONS FOR FILES PARSING/READING */
//appletUidPointer ->currentLogFileName_ = logFileNameToUSeForApplet_;
//appletUidPointer ->currentRepositoryName_ = repositoryNameToUseForApplet_;
//appletUidPointer ->isWorkingWithOnlineFile_ = appletIsWorkingWithOnlineFile_;
			if (setAppletRepository == true) appletUidPointer->currentRepositoryName_ = repositoryNameToUseForApplet;
			if (setAppletLogFile == true) appletUidPointer->currentLogFileName_ = logFileNameToUSeForApplet;

			if (toggleFileChange == true)
			{
				//std::cout << "File change requested from applet ; trying to work with file : " << repositoryNameToUseForApplet_ << "/" << logFileNameToUSeForApplet_ << std::endl;
				appletUidPointer->currentLogFileName_ = repositoryNameToUseForApplet + "/" + logFileNameToUSeForApplet;
				//appletUidPointer ->isWorkingWithOnlineFile_ = appletIsWorkingWithOnlineFile_;

			}
setAppletRepository = false;
setAppletLogFile = false;
toggleFileChange = false;
			


			JSONUtils jsonUtility;
			readLogsFromFile(filemove, appletUidPointer);
			*out << "jsonCallback(" << jsonUtility.craftJsJsonArray(&jsonItemsList_) << ")";
		}
		handleIncomingJsonConnection_->give();

	}





    void AjaxLogReaderWithFileServer::getJSONConnectionParam(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
    {
	handleIncomingJsonConnection_->take();
    	//std::cout << "Got a connection request ; creating a new AppletsUidManagement" << std::endl;
    	//std::cout << "Before creation, my appUid list length is : " << AppletsUidManagementList_.size() << std::endl;
	AppletsUidManagement * oneApplet = new AppletsUidManagement();
	oneApplet->defineAppletUid();

//std::cout << "At instanciation : Applet UID=" << oneApplet->appletUID_ << " will work with file " << oneApplet->currentLogFileName_ << std::endl;

	oneApplet->appletlogsBurstLength_ = MAX_LOGS_PER_BURST;
	
	oneApplet->ajaxspecs_ = getApplicationLogger().getName();
	
	
	std::vector<AppletsUidManagement *>::iterator myIterator = AppletsUidManagementList_.begin();
	AppletsUidManagementList_.insert( myIterator, oneApplet );
    	//std::cout << "After creation, my appUid list length is : " << AppletsUidManagementList_.size() << std::endl;
    	//std::cout << "New AppletUID is : " << oneApplet->getAppletUid() << std::endl;

    
	
//	std::cout << "METHOD CALLED" << std::endl;
		std::string array="";
		array += "[";

		array += "{\"";
		array += "PROC_NAME";
		array += "\": \"";
		array += getApplicationDescriptor()->getClassName();
		array += "\",";


//Useless return in JSON array, now the applet defines this value and does not requires it anymore from server side
		array += "\"";
		array += "LOGS_BURST_LENGTH";
		array += "\": \"";
//		array += "3";
		std::ostringstream mylburst;
		mylburst << MAX_LOGS_PER_BURST;
		array += mylburst.str();
		array += "\",";


		array += "\"";
		array += "APPLET_UID";
		array += "\": \"";
		std::ostringstream myAppUid;
		myAppUid << oneApplet->getAppletUid();
		array += myAppUid.str();
		array += "\",";


		array += "\"";
		array += "LOGFILENAME";
		array += "\": \"";
		array += oneApplet->currentLogFileName_;
		array += "\",";



		array += "\"";
		array += "GEDSPECS";
		array += "\": \"";
		array += oneApplet->gedSpecs_;
		array += "\",";


		array += "\"";
		array += "AJAXSPECS";
		array += "\": \"";
		array += oneApplet->ajaxspecs_;
		array += "\",";


		array += "\"";
		array += "ISONLINE";
		array += "\": \"";
		if (oneApplet->isWorkingWithOnlineFile_ == true)
		{
			array += "TRUE";
		}
		else array += "FALSE";
		array += "\",";


		array += "\"";
		array += "PROC_LID";
		array += "\": \"";
		std::ostringstream myLid;
		myLid << getApplicationDescriptor()->getLocalId();
		array += myLid.str();
		array += "\"}";


		array += "]";
		
		*out << array;
		handleIncomingJsonConnection_->give();
		
//std::cout << "Exiting instanciation : Applet UID=" << oneApplet->appletUID_ << " will work with file " << oneApplet->currentLogFileName_ << std::endl;

	}



    void AjaxLogReaderWithFileServer::getJsJSONConnectionParam(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
    {
	    handleIncomingJsonConnection_->take();

    	//std::cout << "Got a connection request ; creating a new AppletsUidManagement" << std::endl;
    	//std::cout << "Before creation, my appUid list length is : " << AppletsUidManagementList_.size() << std::endl;
	AppletsUidManagement * oneApplet = new AppletsUidManagement();
	oneApplet->defineAppletUid();
	oneApplet->appletlogsBurstLength_ = MAX_LOGS_PER_BURST;
	
	oneApplet->ajaxspecs_ = getApplicationLogger().getName();



	std::vector<AppletsUidManagement *>::iterator myIterator = AppletsUidManagementList_.begin();
	AppletsUidManagementList_.insert( myIterator, oneApplet );
    	//std::cout << "After creation, my appUid list length is : " << AppletsUidManagementList_.size() << std::endl;
    	//std::cout << "New AppletUID is : " << oneApplet->getAppletUid() << std::endl;

		std::string array="";

		array += "{Connection:[";

//		std::cout << "METHOD Connection CALLED" << std::endl;
//		array += "[";

//Useless return in JSON array, now the applet defines this value and does not requires it anymore from server side
		array += "{\"";
		array += "LOGS_BURST_LENGTH";
		array += "\": \"";
		std::ostringstream mylburst;
		mylburst << MAX_LOGS_PER_BURST;
		array += mylburst.str();
		array += "\",";

		array += "\"";
		array += "APPLET_UID";
		array += "\": \"";
		std::ostringstream myAppUid;
		myAppUid << oneApplet->getAppletUid();
		array += myAppUid.str();
		array += "\",";


		array += "\"";
		array += "LOGFILENAME";
		array += "\": \"";
		array += oneApplet->currentLogFileName_;
		array += "\",";



		array += "\"";
		array += "GEDSPECS";
		array += "\": \"";
		array += oneApplet->gedSpecs_;
		array += "\",";

		array += "\"";
		array += "AJAXSPECS";
		array += "\": \"";
		array += oneApplet->ajaxspecs_;
		array += "\",";


		array += "\"";
		array += "ISONLINE";
		array += "\": \"";
		if (oneApplet->isWorkingWithOnlineFile_ == true)
		{
			array += "TRUE";
		}
		else array += "FALSE";
		array += "\",";



		array += "\"";
		array += "CONNECT";
		array += "\": \"";
		array += "ok";
		array += "\"}";


		array += "]";


		array += "}";


		*out << "jsonConnectionCallback(" << array << ")";
		
		handleIncomingJsonConnection_->give();
	}






/* ADDONS FOR FILES PARSING/READING */
/* Displays the available HyperDaq links for this process */
void AjaxLogReaderWithFileServer::getDiskContent(std::string activeRepository, std::string callingAppletUid)
{
	    
	//Make a system call to dump the working repository in temporary file
		std::string dirRepFileName = "/tmp/repName-" + callingAppletUid + ".txt";
		std::string systemCommand = "/bin/pwd > " + dirRepFileName;
		//system("/bin/pwd > /tmp/repName.txt");
		system(systemCommand.c_str());
		
//std::cout << "command fired : " << systemCommand << std::endl;
		   
		//Read back the working repository from file	    
		std::ifstream repfile;
		std::string repName;

		//repfile.open("/tmp/repName.txt");
		repfile.open(dirRepFileName.c_str());
//std::cout << "reading reps from file : " << dirRepFileName << std::endl;
    		while (! repfile.eof() )
        	{
    			std::getline (repfile,repName);
	      		if (repName != "") currentRepository_ = repName;
        	}
        	repfile.close();

		systemCommand = "/bin/rm -f " + dirRepFileName;
		system(systemCommand.c_str());
//std::cout << "Reps file " << dirRepFileName << " deleted" << std::endl;




	if ( (activeRepository == "") || (activeRepository == "VOID") )
	{
		if (currentRepository_ == "") currentRepository_ = "/tmp";
	}
	else currentRepository_ = activeRepository;


//std::cout << "Current repository identified as : " << currentRepository_ << std::endl;

	//Fill in previous repository variable
	std::string::size_type lastSlashPos = currentRepository_.find_last_of("/");
	previousRepository_ = currentRepository_.substr(0,lastSlashPos);
	
	if (previousRepository_ == "") previousRepository_ = "/";
	//std::cout << "substracting /gloubi ends up to @" << previousRepository_ << "@" << std::endl;
	if (currentRepository_ == "/") previousRepository_ = "/";
	
//std::cout << "Previous repository identified as : " << currentRepository_ << std::endl;



	//Make a system call to dump the working repository in temporary file
	
	std::string filesRepFileName = "/tmp/filesList-" + callingAppletUid + ".txt";
	std::string sysCommand = "/bin/ls -F -1 " + currentRepository_ + " > " + filesRepFileName;
	system(sysCommand.c_str());
//std::cout << "command fired : " << sysCommand << std::endl;



	//Read back the working repository from file	    
	std::ifstream flistfile;
	std::string oneFileName;

	flistfile.open(filesRepFileName.c_str());
//std::cout << "reading files from file : " << filesRepFileName << std::endl;

    	while (! flistfile.eof() )
        {
    		std::getline (flistfile,oneFileName);
	      if (oneFileName != "")
	      {
	      //Si la ligne lue finit par '/', c'est un répertoire. Test fait sur la condition : Si il y a au moins un '/' dans la ligne lue
	      std::string::size_type lastCharPos = std::string::npos;

	      lastCharPos = oneFileName.find_last_of("/");
	      	if (lastCharPos != std::string::npos)
	      	{
			oneFileName.erase(lastCharPos);
			repositoriesList_.push_back(oneFileName);
		}
		else
		{
			
			lastCharPos = oneFileName.find_last_of("*");
			if (lastCharPos != std::string::npos)
			{
				oneFileName.erase(lastCharPos);
				filesList_.push_back(oneFileName);
			}
			else
			{
				lastCharPos = oneFileName.find_last_of("@");
				if (lastCharPos != std::string::npos)
				{
					oneFileName.erase(lastCharPos);
					filesList_.push_back(oneFileName);
				}
				else
				{
					lastCharPos = oneFileName.find_last_of("=");
					if (lastCharPos != std::string::npos)
					{
						oneFileName.erase(lastCharPos);
						filesList_.push_back(oneFileName);
					}
					else filesList_.push_back(oneFileName);
				}
			}
			
				
		}
	    }
        }
        flistfile.close();
sysCommand = "/bin/rm -f " + filesRepFileName;
system(sysCommand.c_str());
//std::cout << "Reps file " << filesRepFileName << " deleted" << std::endl;

}







/* Returns the HTML page displayed from the Default() method when the <default_page> link is clicked */
void AjaxLogReaderWithFileServer::displayDiskContent(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	    
	*out << cgicc::p() << std::endl;



/*
if (fileSelectionOngoing_ == false)
{
	*out << "Files parsing Area<br>" << std::endl;
	return;
}
*/





	std::string urlRepository_ = "/";
	urlRepository_ += getApplicationDescriptor()->getURN();
	urlRepository_ += "/callParseDiskPage";





	//Build back URL
	if (currentRepository_ != "/")
	{		
		*out << "Top level repository : <br>&nbsp;&nbsp;&nbsp;&nbsp;" << "<a href=" << urlRepository_;
		*out << "?REPOSITORY=" << previousRepository_;
		*out << "&FILENAME=VOID";
		*out << ">" << previousRepository_ << "</a>" << "<br>" << std::endl;		
		*out << "<br>" << std::endl;
	}
	else
	{
		*out << "Top level repository : <br>&nbsp;&nbsp;&nbsp;&nbsp;Already at topmost level!<br>";
		*out << "<br>" << std::endl;
	}
 
 
 
 	//Build current rep URL
	*out << "Active Repository : <br>&nbsp;&nbsp;&nbsp;&nbsp;" << "<a href=" << urlRepository_;
	*out << "?REPOSITORY=" << currentRepository_;
	*out << "&FILENAME=VOID";
	*out << ">" << currentRepository_ << "</a>" << "<br>" << std::endl;

	 *out << "<br>" << std::endl;



 
	//Build repositories list
	if (repositoriesList_.size() > 0)
	{
		*out << "Local Repositories list : <br>";
		while (repositoriesList_.empty() == false)
		{
			*out << "&nbsp;&nbsp;&nbsp;&nbsp;" << "<a href=" << urlRepository_;
			*out << "?REPOSITORY=" << currentRepository_ << "/" << *(repositoriesList_.end() - 1);
			*out << "&FILENAME=VOID";
			 *out << ">" << *(repositoriesList_.end() - 1) << "/" << "</a>" << "<br>" << std::endl;
			repositoriesList_.pop_back();

		}
		*out << "<br>" << std::endl;
	}
	else
	{
		*out << "Local Repositories list : <br>&nbsp;&nbsp;&nbsp;&nbsp;No sub-repositories detected<br>";
		*out << "<br>" << std::endl;
	}
	    
	 


	//Build files list
	if (filesList_.size() > 0)
	{
		*out << "Files list : <br>";
		while (filesList_.empty() == false)
		{
			*out << "&nbsp;&nbsp;&nbsp;&nbsp;" << "<a href=" << urlRepository_;
			*out << "?REPOSITORY=" << currentRepository_;
			*out << "&FILENAME=" << currentRepository_ << "/" << *(filesList_.end() - 1);
			*out << ">" << *(filesList_.end() - 1) << "</a>" << "<br>" << std::endl;
			filesList_.pop_back();

		}
		*out << "<br>" << std::endl;
	}
	else
	{
		*out << "Files list : <br>&nbsp;&nbsp;&nbsp;&nbsp;No files detected in this repository<br>";
		*out << "<br>" << std::endl;
	}

	
	    
	*out << cgicc::p() << std::endl;
}













        void AjaxLogReaderWithFileServer::getJSONDiskTree(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
	    handleIncomingJsonConnection_->take();

//std::cout << "entering getJSONDiskTree" << std::endl;
		std::string receivedAppletUid = "";
		int appletlogsBurstLengthPtr = 0;
		int limitReadToLastNLogsPtr = -2;
//		processRequestParameters(in, receivedAppletUid, appletlogsBurstLengthPtr, limitReadToLastNLogsPtr);
		std::string logFileNameToUSeForApplet;
		std::string repositoryNameToUseForApplet;
		bool appletIsWorkingWithOnlineFile;
		
		bool setAppletRepository = false;
		bool setAppletLogFile = false;
		bool toggleFileChange = false;
		processRequestParameters(in, receivedAppletUid, appletlogsBurstLengthPtr, limitReadToLastNLogsPtr,
							logFileNameToUSeForApplet, repositoryNameToUseForApplet,appletIsWorkingWithOnlineFile,
							setAppletRepository, setAppletLogFile, toggleFileChange);

//std::cout << "At data connection tryout : Applet UID=" << receivedAppletUid << std::endl;

		//std::cout << "Querying data for applet UID " << receivedAppletUid << std::endl;
		//std::cout << "Burst length locally setted to " << appletlogsBurstLengthPtr << std::endl; 
		AppletsUidManagement * appletUidPointer = NULL;
		//bool appletUidFoundInList = false;
		//std::cout << "Checking if Applet UID " << receivedAppletUid << " exists in list of managed applets" << std::endl;
		for (unsigned int i = 0; i < AppletsUidManagementList_.size(); i++)
		{
			//appletUidPointer = *(AppletsUidManagementList_.begin() + i);
			//std::cout << "Comparing with applet UID " << (*(AppletsUidManagementList_.begin() + i))->getAppletUid() << std::endl;
			if ((*(AppletsUidManagementList_.begin() + i))->getAppletUid() == receivedAppletUid)

			{
				appletUidPointer = *(AppletsUidManagementList_.begin() + i);
			}
		}

		if (appletUidPointer == NULL)
		{
		
//std::cout << "At data connection tryout : NO POINTER FOUND fort applet UID=" << receivedAppletUid << std::endl;

			//std::cout << "No applet with UID " << receivedAppletUid << " found in applets pool" << std::endl;
			//Crafter un return JSON array avec failure notification
			JSONUtils jsonUtility;
			
			
			
//MUST CRAFT A BAD APPLET ARRAY FOR TREE HERE !!!!
			
			*out << jsonUtility.craftJsonBadFileParsingArray(&jsonFilesItemsList_);
			/*
			if (fileSelectionOngoing_ == true)
			{
				pointerToAppSelectingFile_= NULL;
				fileSelectionOngoing_ = false;
				std::cout << "detected NULL appletUidPointer ; forcing fileSelectionOngoing_ to false" << std::endl;
			}
			*/
				

		}
		else
		{
//std::cout << "At data connection tryout : Pointer found fort applet UID=" << appletUidPointer->appletUID_ << std::endl;
//std::cout << "At data connection tryout : Applet UID=" << appletUidPointer->appletUID_ << " uses file " << appletUidPointer->currentLogFileName_ << std::endl;
//std::cout << "At data connection tryout : Applet UID=" << appletUidPointer->appletUID_ << " was using file " << appletUidPointer->previousLogFileName_ << std::endl;
			/*
			if (fileSelectionOngoing_ == true)
			{
				pointerToAppSelectingFile_= appletUidPointer;
				//fileSelectionOngoing_ == false;
				std::cout << "detected NON-NULL appletUidPointer and fileSelectionOngoing_ is true ; forcing appletUID" << std::endl;
			}
			else std::cout << "fileSelectionOngoing_ = false ; not setting anything" << std::endl;
			*/

			if (appletlogsBurstLengthPtr > 0)
			{
				appletUidPointer ->appletlogsBurstLength_ = appletlogsBurstLengthPtr;
			}
			else appletUidPointer ->appletlogsBurstLength_ = MAX_LOGS_PER_BURST;
			//oldLimitReadToLastNLogsPtr = appletUidPointer ->limitReadToLastNLogs_;
			if ( (limitReadToLastNLogsPtr == -1) || (limitReadToLastNLogsPtr > 0) )
			{
				appletUidPointer ->limitReadToLastNLogs_ = limitReadToLastNLogsPtr;
//std::cout << "Applet with UID " << appletUidPointer->getAppletUid() << " has his appletUidPointer ->limitReadToLastNLogs_ setted to " << appletUidPointer ->limitReadToLastNLogs_ << std::endl;
			}
			//else appletUidPointer ->limitReadToLastNLogs_ = -1;
			//std::cout << "Applet with UID " << appletUidPointer->getAppletUid() << " found in applets pool ; reading data with bursts of " << appletUidPointer->appletlogsBurstLength_ << " messages length" << std::endl;


/* ADDONS FOR FILES PARSING/READING */
//appletUidPointer ->currentLogFileName_ = logFileNameToUSeForApplet_;
//appletUidPointer ->currentRepositoryName_ = repositoryNameToUseForApplet_;
//appletUidPointer ->isWorkingWithOnlineFile_ = appletIsWorkingWithOnlineFile_;



			if (repositoryNameToUseForApplet != "") appletUidPointer->currentRepositoryName_ = repositoryNameToUseForApplet;

			//readLogsFromFile(filemove, appletUidPointer);
			//Read repository & files and put results in jsonFilesItemsList_
			//So :  -Call parseDiskContent
			//	-From  parseDiskContent returns, build jsonFilesItemsList_ vector
			
			getDiskContent(appletUidPointer->currentRepositoryName_, appletUidPointer->appletUID_);
			
			castDiskContentToJsonArray();
//			castDiskContentToJsonArray(appletUidPointer->appletPreviousRepository_, appletUidPointer->currentRepositoryName_, appletUidPointer->appletRepositoriesList_, appletUidPointer->appletFilesList_);


			JSONUtils jsonUtility;
			
			std::string tempArray = jsonUtility.craftJsonFileParsingArray(&jsonFilesItemsList_);
			*out << tempArray;
			//*out << jsonUtility.craftJsonFileParsingArray(&jsonFilesItemsList_);
		}
//std::cout << "exiting getJSONDiskTree" << std::endl;
		handleIncomingJsonConnection_->give();

	}


















        void AjaxLogReaderWithFileServer::getJsJSONDiskTree(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
	    handleIncomingJsonConnection_->take();

		std::string receivedAppletUid = "";
		int appletlogsBurstLengthPtr = 0;
		int limitReadToLastNLogsPtr = -2;
//		processRequestParameters(in, receivedAppletUid, appletlogsBurstLengthPtr, limitReadToLastNLogsPtr);
		std::string logFileNameToUSeForApplet;
		std::string repositoryNameToUseForApplet;
		bool appletIsWorkingWithOnlineFile;
		
		bool setAppletRepository = false;
		bool setAppletLogFile = false;
		bool toggleFileChange = false;
		processRequestParameters(in, receivedAppletUid, appletlogsBurstLengthPtr, limitReadToLastNLogsPtr,
							logFileNameToUSeForApplet, repositoryNameToUseForApplet,appletIsWorkingWithOnlineFile,
							setAppletRepository, setAppletLogFile, toggleFileChange);


		//std::cout << "Querying data for applet UID : " << receivedAppletUid << std::endl;
		//std::cout << "Burst length locally setted to " << appletlogsBurstLengthPtr << std::endl; 
		AppletsUidManagement * appletUidPointer = NULL;
		//bool appletUidFoundInList = false;
		//std::cout << "Checking if Applet UID " << receivedAppletUid << " exists in list of managed applets" << std::endl;
		for (unsigned int i = 0; i < AppletsUidManagementList_.size(); i++)
		{
			//appletUidPointer = *(AppletsUidManagementList_.begin() + i);
			//std::cout << "Comparing with applet UID " << (*(AppletsUidManagementList_.begin() + i))->getAppletUid() << std::endl;
			if ((*(AppletsUidManagementList_.begin() + i))->getAppletUid() == receivedAppletUid)

			{
				appletUidPointer = *(AppletsUidManagementList_.begin() + i);
			}
		}

		if (appletUidPointer == NULL)
		{
			//std::cout << "No applet with UID " << receivedAppletUid << " found in applets pool" << std::endl;
			//Crafter un return JSON array avec failure notification
			JSONUtils jsonUtility;
			*out << "jsonDiskTreeCallback(" << jsonUtility.craftJsJsonBadFileParsingArray(&jsonFilesItemsList_) << ")";
			/*
			if (fileSelectionOngoing_ == true)
			{
				pointerToAppSelectingFile_= NULL;
				fileSelectionOngoing_ = false;
			}
			*/

		}
		else
		{
			if (appletlogsBurstLengthPtr > 0)
			{
				appletUidPointer ->appletlogsBurstLength_ = appletlogsBurstLengthPtr;
			}
			else appletUidPointer ->appletlogsBurstLength_ = MAX_LOGS_PER_BURST;
			if ( (limitReadToLastNLogsPtr == -1) || (limitReadToLastNLogsPtr > 0) )
			{
				appletUidPointer ->limitReadToLastNLogs_ = limitReadToLastNLogsPtr;
//std::cout << "Applet with UID " << appletUidPointer->getAppletUid() << " has his appletUidPointer ->limitReadToLastNLogs_ setted to " << appletUidPointer ->limitReadToLastNLogs_ << std::endl;
			}
			//std::cout << "Applet with UID " << appletUidPointer->getAppletUid() << " found in applets pool ; reading data with bursts of " << appletUidPointer->appletlogsBurstLength_ << " messages length" << std::endl;


/* ADDONS FOR FILES PARSING/READING */
//appletUidPointer ->currentLogFileName_ = logFileNameToUSeForApplet_;
//appletUidPointer ->currentRepositoryName_ = repositoryNameToUseForApplet_;
//appletUidPointer ->isWorkingWithOnlineFile_ = appletIsWorkingWithOnlineFile_;

			if (repositoryNameToUseForApplet != "") appletUidPointer->currentRepositoryName_ = repositoryNameToUseForApplet;



			getDiskContent(appletUidPointer->currentRepositoryName_, appletUidPointer->appletUID_);
			
			castDiskContentToJsonArray();
//			castDiskContentToJsonArray(appletUidPointer->appletPreviousRepository_, appletUidPointer->currentRepositoryName_, appletUidPointer->appletRepositoriesList_, appletUidPointer->appletFilesList_);
		
			
			JSONUtils jsonUtility;
			//readLogsFromFile(filemove, appletUidPointer);
			//Read repository & files and put results in jsonFilesItemsList_
			//*out << jsonUtility.craftJsJsonFileParsingArray(&jsonFilesItemsList_);

			//So :  -Call parseDiskContent
			//	-From  parseDiskContent returns, build jsonFilesItemsList_ vector


			std::string tempArray = "jsonDiskTreeCallback(" + jsonUtility.craftJsonFileParsingArray(&jsonFilesItemsList_) + ")";
			*out << tempArray;
			//*out << "jsonDiskTreeCallback(" << jsonUtility.craftJsJsonFileParsingArray(&jsonFilesItemsList_) << ")";
		}
		handleIncomingJsonConnection_->give();

	}














void AjaxLogReaderWithFileServer::castDiskContentToJsonArray()
{
//std::cout << "Now casting ; previous=@" << previousRepository_ << "@" << std::endl;
//std::cout << "Now casting ; current=@" << currentRepository_ << "@" << std::endl;


JSONFileItem newJsonFileItem1(JSON_TOPREP, previousRepository_);
jsonFilesItemsList_.push_back(newJsonFileItem1);

JSONFileItem newJsonFileItem2(JSON_CURREP, currentRepository_);
jsonFilesItemsList_.push_back(newJsonFileItem2);


	//Build repositories list
	if (repositoriesList_.size() > 0)
	{
		while (repositoriesList_.empty() == false)
		{
			
			std::string currRep = (*(repositoriesList_.end() - 1));
			repositoriesList_.pop_back();
			
			JSONFileItem newJsonFileItem(JSON_ONEREP, currRep);
			jsonFilesItemsList_.push_back(newJsonFileItem);
		}
	}

	 


	//Build files list
	if (filesList_.size() > 0)
	{
		while (filesList_.empty() == false)
		{
			std::string currFile = (*(filesList_.end() - 1));
			filesList_.pop_back();

			JSONFileItem newJsonFileItem(JSON_ONEFILE, currFile);
			jsonFilesItemsList_.push_back(newJsonFileItem);

		}
	}
}



































