/*

   FileName : 		AjaxLogReader.cc

   Content : 		AjaxLogReader module

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


#include "AjaxLogReader.h"

#define NOT_EOF !(feof(appletUidPointer->sourceFile_))

#define MAX_LOGS_PER_BURST	25


#define ONE_LINE_LENGTH	1100

XDAQ_INSTANTIATOR_IMPL(AjaxLogReader);





#ifdef WILL_USE_GWT_FIREITEMS_VIEWER
	AjaxLogReader::AjaxLogReader(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): xdaq::Application(s), JsInterface(s)
#else
	AjaxLogReader::AjaxLogReader(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): xdaq::Application(s)
#endif
{




	//Define FSM states
	fsm_.addState ('C', "Configured") ;
	fsm_.addState ('H', "Halted");
	fsm_.addStateTransition ('C','H', EXECTRANSCH, this, &AjaxLogReader::haltAction);
	fsm_.addStateTransition ('H','C', EXECTRANSHC, this, &AjaxLogReader::configureAction);
	fsm_.setFailedStateTransitionAction( this, &AjaxLogReader::failedTransition );

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
	mmesg << "Process version " << AJAXLOGREADER_PACKAGE_VERSION << " loaded";
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
	xgi::bind(this,&AjaxLogReader::Default, "Default");
	xgi::bind(this,&AjaxLogReader::Default1, "Default1");
	xgi::bind(this,&AjaxLogReader::callDiagSystemPage, "callDiagSystemPage");
	xgi::bind(this,&AjaxLogReader::callFsmPage, "callFsmPage");

	xgi::bind(this,&AjaxLogReader::configureStateMachine, "configureStateMachine");
	xgi::bind(this,&AjaxLogReader::stopStateMachine, "stopStateMachine");

	xgi::bind(this,&AjaxLogReader::getJSONDataList, "getJSONDataList");
	xgi::bind(this,&AjaxLogReader::getJsJSONDataList, "getJsJSONDataList");

	xgi::bind(this,&AjaxLogReader::getJSONConnectionParam, "getJSONConnectionParam");
	xgi::bind(this,&AjaxLogReader::getJsJSONConnectionParam, "getJsJSONConnectionParam");



//FILTERING-BEGIN
	xgi::bind(this,&AjaxLogReader::viewFedDebugPage, "viewFedDebugPage");
	xgi::bind(this,&AjaxLogReader::viewDcuWorkLoopFullWarnPage, "viewDcuWorkLoopFullWarnPage");
	xgi::bind(this,&AjaxLogReader::viewDcuAccessErrorPage, "viewDcuAccessErrorPage");
//FILTERING-END



	//Bind diagnostic applications
	xgi::bind(this,&AjaxLogReader::configureDiagSystem, "configureDiagSystem");
	xgi::bind(this,&AjaxLogReader::applyConfigureDiagSystem, "applyConfigureDiagSystem");


	// Call a macro needed for running a diagService in this application
	DIAG_DECLARE_LR_APP



	// Bind SOAP callbacks for FSM control messages 
	xoap::bind (this, &AjaxLogReader::fireEvent, EXECTRANSCH, XDAQ_NS_URI);
	xoap::bind (this, &AjaxLogReader::fireEvent, EXECTRANSHC, XDAQ_NS_URI);

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


}



AjaxLogReader::~AjaxLogReader()
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
void AjaxLogReader::getDefault1Content(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
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


// Returns the HTML page for FSM status
void AjaxLogReader::getFsmStatePage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
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
void AjaxLogReader::Default1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 0;
	Default(in, out);
}

// xgi method called when the link <display_diagsystem> is clicked
void AjaxLogReader::callDiagSystemPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 1;
	Default(in, out);
}

// xgi method called when the link <display_fsm> is clicked
void AjaxLogReader::callFsmPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 2;
	Default(in, out);
}




//FILTERING-BEGIN
// xgi method called when the link <ViewFedDebugPage> is clicked
void AjaxLogReader::viewFedDebugPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 3;
	Default(in, out);
}

void AjaxLogReader::viewDcuWorkLoopFullWarnPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 4;
	Default(in, out);
}


void AjaxLogReader::viewDcuAccessErrorPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
{
	internalState_ = 5;
	Default(in, out);
}

//FILTERING-END



// Displays the available HyperDaq links for this process
void AjaxLogReader::displayLinks(xgi::Input * in, xgi::Output * out)
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
void AjaxLogReader::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
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




	// Create HTML footer
	xgi::Utils::getPageFooter(*out);


}







// Turns the AjaxLogReader into state INITIALISED (FSM point of view)
void AjaxLogReader::configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
	// Apply fireitems value to internal variables and check validity
	DIAG_EXEC_FSM_INIT_TRANS
}



// Turns the AjaxLogReader into state HALTED (FSM point of view)
void AjaxLogReader::haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
	// Apply fireitems value to internal variables and check validity
	DIAG_EXEC_FSM_STOP_TRANS
}
   

   
// Allows the FSM to be controlled via a soap message carrying a "fireEvent" message
xoap::MessageReference AjaxLogReader::fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception)
{
	//#define DEBUG_FIREEVENT
	#ifdef DEBUG_FIREEVENT
		std::cout << "In AjaxLogReader : fireEvent received" << std::endl ;
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
void AjaxLogReader::failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
	toolbox::fsm::FailedEvent & fe = dynamic_cast<toolbox::fsm::FailedEvent&>(*e);
	std::ostringstream msgError;
	msgError << "Failure occurred when performing transition from: " << fe.getFromState() << " to: " << fe.getToState() << " exception: " << fe.getException().what();
	diagService_->reportError (msgError.str(), DIAGERROR);
}



// Can be called via the <configure_FSM> hyperdaq HTML page of this process
void AjaxLogReader::configureStateMachine(xgi::Input * in, xgi::Output * out)
{
	if (fsm_.getCurrentState() == 'H')
	{
		toolbox::Event::Reference e(new toolbox::Event(EXECTRANSHC, this));
		fsm_.fireEvent(e);
	}
	Default(in, out);
}


// Can be called via the <configure_FSM> hyperdaq HTML page of this process
void AjaxLogReader::stopStateMachine(xgi::Input * in, xgi::Output * out)
{
	if (fsm_.getCurrentState() == 'C')
	{
		toolbox::Event::Reference e(new toolbox::Event(EXECTRANSCH, this));
		fsm_.fireEvent(e);
	}
	Default(in, out);
}


#ifdef AUTO_CONFIGURE_PROCESSES
void AjaxLogReader::timeExpired (toolbox::task::TimerEvent& e) 
{
	DIAG_EXEC_FSM_INIT_TRANS
	std::cout << "AjaxLogReader(LID=" << getApplicationDescriptor()->getLocalId() << ",Instance=" << getApplicationDescriptor()->getInstance() << ") ready" << std::endl;
}
#endif





std::string AjaxLogReader::cleanField(std::string& rawField, std::string& tag)
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



std::string AjaxLogReader::getTag(std::string& rawField)
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


void AjaxLogReader::cleanupMsgString(std::string * msg)
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




void AjaxLogReader::readLogsFromFile(int command, AppletsUidManagement * appletUidPointer)
{
//APP
//int selectedLogsReadFromFile = 0;
appletUidPointer->selectedLogsReadFromFile_ = 0;

//std::cout << "DEBUG 00" << std::endl;
	//First, get the name of the file in which we should be working
	
	//If useFileLogging_ == false then get name from GED
	//DEBUG TODO :: useFileLogging_ is ALWAYS FALSE, TO BE CORRECTED...
	if (diagService_->useFileLogging_ == false)
	{
//std::cout << "useFileLogging_ IS FALSE" << std::endl;
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
		}
		catch (xdaq::exception::Exception& e)
		{
			lclLogFileName = "";
		}
		diagService_->setFilePathAndName(lclLogFileName);

	}
	//Else, If useFileLogging_ == true then do nothing - filename change has already been taken into account from the hyperdaq diagsystem page
	//else std::cout << "useFileLogging_ IS TRUE" << std::endl;

	//During next check, if filename has not changed since last access then file will not be closed
	diagService_->checkLrFileOption();

	//If the file to open is a new one
	if (diagService_->logFileNameHasChanged_ == true)
	{
		//If ANY of the registered applets sees that the filename has changed, then
		//Change filename and close/reopen file pointer for EVERY registered applet




		for (unsigned int i = 0; i < AppletsUidManagementList_.size(); i++)
		{
//std::cout << "Applet in deletion should read logs to : " << appletUidPointer->limitReadToLastNLogs_  << std::endl;
			AppletsUidManagement * localAppletUidPointer = *(AppletsUidManagementList_.begin() + i);
			if (localAppletUidPointer->sourceFile_ != NULL)
			{
				fclose(localAppletUidPointer->sourceFile_);
				localAppletUidPointer->sourceFile_ = NULL;
				localAppletUidPointer->sourceFileIsOpen_ = false;
			}
			
		}


		//Close old file
		//APP
		//if (fileOut_ != NULL) fclose(fileOut_);
		//fileoutIsOpen_ = false;
		//Notify that filename change has been taken into account
		diagService_->logFileNameHasChanged_ = false;

//std::cout << "FileChange detected ; Command forced to 1" << std::endl;
		command = 1;
	}

	//If targetted logfile is not opened yet
	//APP
	//if (fileoutIsOpen_ == false)
	if (appletUidPointer->sourceFileIsOpen_ == false)
	{
		appletUidPointer->sourceFile_ = fopen(diagService_->logFileName_.c_str(), "r");
		if (appletUidPointer->sourceFile_ != NULL)
		{
			appletUidPointer->sourceFileIsOpen_ = true;
//			CCC
			appletUidPointer->filePos_ = 0;
			appletUidPointer->logcounter_ = 0;
		}
		else
		{
			std::cout << "Unable to read from log file \"" << diagService_->logFileName_.c_str() << "\"" << std::endl;
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
										(std::string)"",
										(std::string)"",
										(std::string)"TRUE",
										(std::string)"");


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




std::string AjaxLogReader::extractParameterValueFromURL(xgi::Input * in, std::string parameter)
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





        int AjaxLogReader::processRequestParameters(xgi::Input * in, std::string & appletUidPtr, int & appletlogsBurstLengthPtr, int & limitReadToLastNLogsPointer)
        {
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
			//appletUidPtr = appletUidparameter;


			if (doRewindparameter == (std::string)"TRUE") {return(1);} else return(0);
		}




        void AjaxLogReader::getJSONDataList(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
	    handleIncomingJsonConnection_->take();

		std::string receivedAppletUid = "";
		int appletlogsBurstLengthPtr = 0;
		int limitReadToLastNLogsPtr = -2;
		int filemove=processRequestParameters(in, receivedAppletUid, appletlogsBurstLengthPtr, limitReadToLastNLogsPtr);
		
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
			//std::cout << "No applet with UID " << receivedAppletUid << " found in applets pool" << std::endl;
			//Crafter un return JSON array avec failure notification
			JSONUtils jsonUtility;
			*out << jsonUtility.craftJsonBadAppletArray();

		}
		else
		{
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
			JSONUtils jsonUtility;
			readLogsFromFile(filemove, appletUidPointer);
			*out << jsonUtility.craftJsonArray(&jsonItemsList_);
		}

		handleIncomingJsonConnection_->give();

	}



        void AjaxLogReader::getJsJSONDataList(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {
	    handleIncomingJsonConnection_->take();

		std::string receivedAppletUid = "";
		int appletlogsBurstLengthPtr = 0;
		int limitReadToLastNLogsPtr = -2;
		int filemove=processRequestParameters(in, receivedAppletUid, appletlogsBurstLengthPtr, limitReadToLastNLogsPtr);

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
			JSONUtils jsonUtility;
			readLogsFromFile(filemove, appletUidPointer);
			*out << "jsonCallback(" << jsonUtility.craftJsJsonArray(&jsonItemsList_) << ")";
		}
		handleIncomingJsonConnection_->give();

	}





    void AjaxLogReader::getJSONConnectionParam(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
    {
	handleIncomingJsonConnection_->take();
    	//std::cout << "Got a connection request ; creating a new AppletsUidManagement" << std::endl;
    	//std::cout << "Before creation, my appUid list length is : " << AppletsUidManagementList_.size() << std::endl;
	AppletsUidManagement * oneApplet = new AppletsUidManagement();
	oneApplet->defineAppletUid();
	oneApplet->appletlogsBurstLength_ = MAX_LOGS_PER_BURST;
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
		array += "PROC_LID";
		array += "\": \"";
		std::ostringstream myLid;
		myLid << getApplicationDescriptor()->getLocalId();
		array += myLid.str();
		array += "\"}";


		array += "]";
		
		*out << array;
		handleIncomingJsonConnection_->give();
	}



    void AjaxLogReader::getJsJSONConnectionParam(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
    {
	    handleIncomingJsonConnection_->take();

    	//std::cout << "Got a connection request ; creating a new AppletsUidManagement" << std::endl;
    	//std::cout << "Before creation, my appUid list length is : " << AppletsUidManagementList_.size() << std::endl;
	AppletsUidManagement * oneApplet = new AppletsUidManagement();
	oneApplet->defineAppletUid();
	oneApplet->appletlogsBurstLength_ = MAX_LOGS_PER_BURST;
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
		array += "CONNECT";
		array += "\": \"";
		array += "ok";
		array += "\"}";


		array += "]";


		array += "}";


		*out << "jsonConnectionCallback(" << array << ")";
		
		handleIncomingJsonConnection_->give();
	}












