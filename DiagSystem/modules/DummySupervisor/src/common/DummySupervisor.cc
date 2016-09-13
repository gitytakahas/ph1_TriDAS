/*
   FileName : 		DummySupervisor.cc

   Content : 		DummySupervisor module

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

#include "DummySupervisor.h"

XDAQ_INSTANTIATOR_IMPL(DummySupervisor)




	


	#ifdef WILL_USE_GWT_FIREITEMS_VIEWER
		DummySupervisor::DummySupervisor(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): xdaq::Application(s), JsInterface(s)
	#else
		DummySupervisor::DummySupervisor(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): xdaq::Application(s)
	#endif
	{

  // ---------------------------------- State machine
  // Initiliase state
  /* Liste des etats qu'on va trouver dans la FSM */
  fsm_.addState ('C', "Configured") ;
  fsm_.addState ('H', "Halted");


  /* Methode initialiseAction qui doit gerer les constructions, etc... qui surviennent lors de la
  transition entre l'etat Initial et l'etat Halted */
  fsm_.addStateTransition ('C','H', EXECTRANSCH, this, &DummySupervisor::haltAction);
  fsm_.addStateTransition ('H','C', EXECTRANSHC, this, &DummySupervisor::configureAction);

  /* Si une transition merde, on appel la methode failedTransition */
  fsm_.setFailedStateTransitionAction( this, &DummySupervisor::failedTransition );

  /* On synchronise la FASM avec l'etat du process apres load, a savoir : etat HALTED */
  fsm_.setInitialState('H');
  fsm_.reset();

  //Export the stateName variable
  getApplicationInfoSpace()->fireItemAvailable("stateName", &fsm_.stateName_);

		executeReconfMethodMutex = new toolbox::BSem(toolbox::BSem::FULL);
		
		
		//Give funny and useless informations at load time
		std::stringstream mmesg;
		mmesg << "Process version " << DUMMYSUPERVISOR_PACKAGE_VERSION << " loaded";
		this->getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);
		LOG4CPLUS_INFO(this->getApplicationLogger(), mmesg.str());

		this->getApplicationLogger().setLogLevel(ERROR_LOG_LEVEL);

		internalState_ = 0;
		
		//DIAGREQUESTED

/*
		 diagService_ = new DiagBagWizard(
 			 (getApplicationDescriptor()->getClassName() + "DiagLvlOne") ,
 			 this->getApplicationLogger(),
			 getApplicationDescriptor()->getClassName(),
			 getApplicationDescriptor()->getInstance(),
			 getApplicationDescriptor()->getLocalId(),
			 (xdaq::WebApplication *)this
			  );
*/

		 diagService_ = new DiagBagWizard(
 			 ("ReconfigurationModule") ,
 			 this->getApplicationLogger(),
			 getApplicationDescriptor()->getClassName(),
			 getApplicationDescriptor()->getInstance(),
			 getApplicationDescriptor()->getLocalId(),
			 (xdaq::WebApplication *)this,
			 "MYSYSTEM",
			 "MYSUBSYTSTEM",
			  &myPixelsConsole);

		#ifdef WILL_COMPILE_FOR_PIXELS
			std::string errMsg = "DiagSystem object is instancing";
			diagService_->reportError(errMsg,DIAGUSERINFO);
			std::cout << "DUPED MESSAGE : " << errMsg << std::endl;
		#else
			diagService_->reportError("DiagSystem object is instancing",DIAGUSERINFO);
		#endif

		// A simple web control interface
		xgi::bind(this,&DummySupervisor::Default, "Default");
		xgi::bind(this,&DummySupervisor::Default1, "Default1");
		xgi::bind(this,&DummySupervisor::sendReconfAutoLog, "sendReconfAutoLog");
		xgi::bind(this,&DummySupervisor::sendReconfStopLog, "sendReconfStopLog");
		xgi::bind(this,&DummySupervisor::sendReconfRunLog, "sendReconfRunLog");
		xgi::bind(this,&DummySupervisor::sendReconfLog, "sendReconfLog");




		xgi::bind(this,&DummySupervisor::sendTraceLog, "sendTraceLog");
		xgi::bind(this,&DummySupervisor::sendDebugLog, "sendDebugLog");
		xgi::bind(this,&DummySupervisor::sendInfoLog, "sendInfoLog");
		xgi::bind(this,&DummySupervisor::sendWarnLog, "sendWarnLog");
		xgi::bind(this,&DummySupervisor::sendUserInfoLog, "sendUserInfoLog");
		xgi::bind(this,&DummySupervisor::sendErrorLog, "sendErrorLog");
		xgi::bind(this,&DummySupervisor::sendFatalLog, "sendFatalLog");


		xgi::bind(this,&DummySupervisor::callDiagSystemPage, "callDiagSystemPage");
		xgi::bind(this,&DummySupervisor::callFsmPage, "callFsmPage");

		xgi::bind(this,&DummySupervisor::configureStateMachine, "configureStateMachine");
		xgi::bind(this,&DummySupervisor::stopStateMachine, "stopStateMachine");


		//DIAGREQUESTED
		xgi::bind(this,&DummySupervisor::configureDiagSystem, "configureDiagSystem");
		xgi::bind(this,&DummySupervisor::applyConfigureDiagSystem, "applyConfigureDiagSystem");
/*
		xoap::bind(this, &DummySupervisor::freeLclDiagSemaphore, "freeLclDiagSemaphore", XDAQ_NS_URI );
		xoap::bind(this, &DummySupervisor::freeGlbDiagSemaphore, "freeGlbDiagSemaphore", XDAQ_NS_URI );
		xoap::bind(this, &DummySupervisor::processOnlineDiagRequest, "processOnlineDiagRequest", XDAQ_NS_URI );
*/

		xgi::bind(this,&DummySupervisor::simulateFecError, "simulateFecError");
		xgi::bind(this,&DummySupervisor::simulateFedError, "simulateFedError");
		xgi::bind(this,&DummySupervisor::simulatePsuError, "simulatePsuError");


		//DIAGREQUESTED
		DIAG_DECLARE_USER_APP

		//RECONFREQUESTED
		xoap::bind(this, &DummySupervisor::recoveryEntry, "recoveryEntry", XDAQ_NS_URI );

  // Bind SOAP callbacks for control messages
  xoap::bind (this, &DummySupervisor::fireEvent, EXECTRANSCH, XDAQ_NS_URI);
  xoap::bind (this, &DummySupervisor::fireEvent, EXECTRANSHC, XDAQ_NS_URI);

	diagService_->reportError("DiagSystem object has been instanciated",DIAGUSERINFO);

	diagService_->reportError("This is an example of error reporting with additional informations into extra buffer", DIAGERROR, (std::string)"MYVALUE=2, MYSTRINGOFINFO=this is a message, MYOTHERVALUE=544");

/*
	diagService_->reportError("DiagSystem object has been instanciated 02",DIAGUSERINFO);


	diagService_->reportError("DiagSystem object has been instanciated 03",DIAGUSERINFO);

	diagService_->reportError("DiagSystem object has been instanciated 04",DIAGUSERINFO);
*/
            #ifdef AUTO_CONFIGURE_PROCESSES
                std::stringstream timerName;
                timerName << getApplicationDescriptor()->getContextDescriptor()->getURL() << ":";
                timerName << getApplicationDescriptor()->getClassName() << ":" << getApplicationDescriptor()->getLocalId() << ":" << getApplicationDescriptor()->getInstance();
                toolbox::task::Timer * timer = toolbox::task::getTimerFactory()->createTimer(timerName.str());
                toolbox::TimeInterval interval(AUTO_UP_CONFIGURE_DELAY,0);
                toolbox::TimeVal start;
                start = toolbox::TimeVal::gettimeofday() + interval;
                timer->schedule( this, start,  0, "" );
            #endif
	}





	DummySupervisor::~DummySupervisor()
	{
		if (executeReconfMethodMutex != NULL) delete executeReconfMethodMutex;
	}





	void DummySupervisor::getDefault1Content(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{

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







	
			// *out << cgicc::p() << std::endl;
					*out << "<br>";

		std::string urlReconfAuto = "/";
		urlReconfAuto += getApplicationDescriptor()->getURN();
		urlReconfAuto += "/sendReconfAutoLog";	
		*out << cgicc::form().set("method","post").set("action", urlReconfAuto).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Send a reconfiguration request Log with ErrorCode 103");
		// *out << cgicc::p() << std::endl;
		*out << "<br>";
		*out << cgicc::form() << std::endl;


		std::string urlReconfStop = "/";
		urlReconfStop += getApplicationDescriptor()->getURN();
		urlReconfStop += "/sendReconfStopLog";	
		*out << cgicc::form().set("method","post").set("action", urlReconfStop).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Send a reconfiguration request Log with ErrorCode 102");
		// *out << cgicc::p() << std::endl;
		*out << "<br>";
		*out << cgicc::form() << std::endl;


		std::string urlReconfRun = "/";
		urlReconfRun += getApplicationDescriptor()->getURN();
		urlReconfRun += "/sendReconfRunLog";	
		*out << cgicc::form().set("method","post").set("action", urlReconfRun).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Send a reconfiguration request Log with ErrorCode 101");
		// *out << cgicc::p() << std::endl;
		*out << "<br>";
		*out << cgicc::form() << std::endl;


		std::string urlReconf = "/";
		urlReconf += getApplicationDescriptor()->getURN();
		urlReconf += "/sendReconfLog";	
		*out << cgicc::form().set("method","post").set("action", urlReconf).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Send a reconfiguration request Log with ErrorCode 100");
		// *out << cgicc::p() << std::endl;
		*out << "<br>";
		*out << cgicc::form() << std::endl;

		*out << "<br>";

		std::string urlTrace = "/";
		urlTrace += getApplicationDescriptor()->getURN();
		urlTrace += "/sendTraceLog";	
		*out << cgicc::form().set("method","post").set("action", urlTrace).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Send STANDARD log");
		// *out << cgicc::p() << std::endl;
		*out << "<br>";
		*out << cgicc::form() << std::endl;


		std::string urlDebug = "/";
		urlDebug += getApplicationDescriptor()->getURN();
		urlDebug += "/sendDebugLog";	
		*out << cgicc::form().set("method","post").set("action", urlDebug).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Send DCU READOUT ERROR Log");
//		*out << cgicc::p() << std::endl;
		*out << "<br>";
		*out << cgicc::form() << std::endl;

		std::string urlInfo = "/";
		urlInfo += getApplicationDescriptor()->getURN();
		urlInfo += "/sendInfoLog";	
		*out << cgicc::form().set("method","post").set("action", urlInfo).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Send DCU WORKLOOP ERROR Log");
//		*out << cgicc::p() << std::endl;
		*out << "<br>";
		*out << cgicc::form() << std::endl;


		std::string urlWarn = "/";
		urlWarn += getApplicationDescriptor()->getURN();
		urlWarn += "/sendWarnLog";	
		*out << cgicc::form().set("method","post").set("action", urlWarn).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Send FED DEBUG Log");
//		*out << cgicc::p() << std::endl;
*out << "<br>";
		*out << cgicc::form() << std::endl;

		std::string urlUserInfo = "/";
		urlUserInfo += getApplicationDescriptor()->getURN();
		urlUserInfo += "/sendUserInfoLog";	
		*out << cgicc::form().set("method","post").set("action", urlUserInfo).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Send FED TEMPERATURE MONITORING Log");
//		*out << cgicc::p() << std::endl;
*out << "<br>";
		*out << cgicc::form() << std::endl;

		std::string urlError = "/";
		urlError += getApplicationDescriptor()->getURN();
		urlError += "/sendErrorLog";	
		*out << cgicc::form().set("method","post").set("action", urlError).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Send FED SPY CHANNEL Log");
//		*out << cgicc::p() << std::endl;
*out << "<br>";
		*out << cgicc::form() << std::endl;

		std::string urlFatal = "/";
		urlFatal += getApplicationDescriptor()->getURN();
		urlFatal += "/sendFatalLog";	
		*out << cgicc::form().set("method","post").set("action", urlFatal).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Send Fatal (aka FEC) Log");
//		*out << cgicc::p() << std::endl;
*out << "<br>";
		*out << cgicc::form() << std::endl;

	}

	void DummySupervisor::getFsmStatePage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{

//		*out << cgicc::p() << std::endl;
		if (fsm_.getCurrentState() == 'H') *out << "Current State is : Halted" << std::endl;
		if (fsm_.getCurrentState() == 'C') *out << "Current State is : Configured" << std::endl;
		if ( (fsm_.getCurrentState() != 'C') && (fsm_.getCurrentState() != 'H') ) *out << "Current State is : DANS LES CHOUX" << std::endl;
//		*out << cgicc::p() << std::endl;
*out << "<br>";


		std::string urlConfig = "/";
		urlConfig += getApplicationDescriptor()->getURN();
		urlConfig += "/configureStateMachine";	
		*out << cgicc::form().set("method","post").set("action", urlConfig).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Configure State Machine");
//		*out << cgicc::p() << std::endl;
*out << "<br>";
		*out << cgicc::form() << std::endl;

//		*out << cgicc::p() << std::endl;

		std::string urlStop = "/";
		urlStop += getApplicationDescriptor()->getURN();
		urlStop += "/stopStateMachine";	
		*out << cgicc::form().set("method","post").set("action", urlStop).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Stop State Machine");
//		*out << cgicc::p() << std::endl;
*out << "<br>";
		*out << cgicc::form() << std::endl;

//		*out << cgicc::p() << std::endl;
	}

	
	void DummySupervisor::Default1(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		internalState_ = 0;
		Default(in, out);
	}

	void DummySupervisor::callDiagSystemPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		 internalState_ = 1;
		Default(in, out);
	}


	void DummySupervisor::callFsmPage(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		 internalState_ = 2;
		Default(in, out);
	}

void DummySupervisor::displayLinks(xgi::Input * in, xgi::Output * out)
{

	std::string urlBase_ = "/"; \
	urlBase_ += getApplicationDescriptor()->getURN(); \
	urlBase_ += "/Default1"; \
	*out << "<a href=" << urlBase_ << ">Default Page</a>";

	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";

	std::string urlDiag_ = "/"; \
	urlDiag_ += getApplicationDescriptor()->getURN(); \
	urlDiag_ += "/callDiagSystemPage"; \
	*out << "<a href=" << urlDiag_ << ">Configure DiagSystem</a>";

	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";

	std::string urlFsm_ = "/"; \
	urlFsm_ += getApplicationDescriptor()->getURN(); \
	urlFsm_ += "/callFsmPage"; \
	*out << "<a href=" << urlFsm_ << ">FSM Access</a>";

	*out << "<br>";
//	*out << cgicc::p() << std::endl;

}


	void DummySupervisor::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
//std::cout << "Entering Default() Method" << std::endl;

		*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
		*out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
		*out << cgicc::title("Dummy Supervisor") << std::endl;

		xgi::Utils::getPageHeader
			(out, 
			"Dummy Supervisor", 
			getApplicationDescriptor()->getContextDescriptor()->getURL(),
			getApplicationDescriptor()->getURN(),
			"/daq/xgi/images/Application.jpg"
			);

		displayLinks(in, out);

	if (internalState_ == 0) getDefault1Content(in, out);
	if (internalState_ == 1) diagService_->getDiagSystemHtmlPage(in, out,getApplicationDescriptor()->getURN());
	if (internalState_ == 2) getFsmStatePage(in, out);
	

		xgi::Utils::getPageFooter(*out);

	}




	//DIAGREQUESTED
//	void DummySupervisor::DIAG_CONFIGURE_CALLBACK();
//	void DummySupervisor::DIAG_APPLY_CALLBACK();

/*
  	DIAG_FREELCLSEM_CALLBACK();
    DIAG_FREEGLBSEM_CALLBACK();
	DIAG_REQUEST_ENTRYPOINT();
*/



	void DummySupervisor::simulateFecError(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		cgicc::Cgicc cgi(in);
        simulatedFecHardId_ = cgi["simulatedFecHardId"]->getValue();
        simulatedFecRing_ = cgi["simulatedFecRing"]->getValue();
        simulatedCcuAddress_ = cgi["simulatedCcuAddress"]->getValue();
        simulatedI2cChannel_ = cgi["simulatedI2cChannel"]->getValue();
        simulatedI2cAddress_ = cgi["simulatedI2cAddress"]->getValue();

		std::string extraBuffer;
		if (simulatedFecHardId_ != "") extraBuffer = "<FECHARDID>" + simulatedFecHardId_ + "</FECHARDID>";
		if (simulatedFecRing_ != "") extraBuffer = extraBuffer + "<RING>" + simulatedFecRing_ + "</RING>";
		if (simulatedCcuAddress_ != "") extraBuffer = extraBuffer + "<CCU>" + simulatedCcuAddress_ + "</CCU>";
		if (simulatedI2cChannel_ != "") extraBuffer = extraBuffer + "<I2CCHANNEL>" + simulatedI2cChannel_ + "</I2CCHANNEL>";
		if (simulatedI2cAddress_ != "") extraBuffer = extraBuffer + "<I2CADDRESS>" + simulatedI2cAddress_ + "</I2CADDRESS>";


		diagService_->reportError("This is a simulated FEC ERROR log",DIAGUSERINFO,
		"",
		1000,
		"STEADY",
		"TRACKER",
		"TEC",
		extraBuffer);

		this->Default(in,out);
	}



	void DummySupervisor::simulateFedError(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		cgicc::Cgicc cgi(in);
        simulatedFedId_ = cgi["simulatedFedId"]->getValue();
        simulatedFedChannel_ = cgi["simulatedFedChannel"]->getValue();

		std::string extraBuffer;
		if (simulatedFedId_ != "") extraBuffer = "<FEDID>" + simulatedFedId_ + "</FEDID>";
		if (simulatedFedChannel_ != "") extraBuffer = extraBuffer + "<FEDCHANNEL>" + simulatedFedChannel_ + "</FEDCHANNEL>";

		diagService_->reportError("This is a simulated FED ERROR log",DIAGUSERINFO,
		"",
		1001,
		"STEADY",
		"TRACKER",
		"TEC",
		extraBuffer);

		this->Default(in,out);
	}



	void DummySupervisor::simulatePsuError(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		cgicc::Cgicc cgi(in);
        simulatedPsuId_ = cgi["simulatedPsuId"]->getValue();

		std::string extraBuffer;
		if (simulatedPsuId_ != "") extraBuffer = "<PSUID>" + simulatedPsuId_ + "</PSUID>";


		diagService_->reportError("This is a simulated PSU ERROR log",DIAGUSERINFO,
		"",
		1002,
		"STEADY",
		"TRACKER",
		"TEC",
		extraBuffer);

		this->Default(in,out);
	}




	void DummySupervisor::sendReconfAutoLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		diagService_->reportError("Send a reconfiguration request Log with ErrorCode 103",DIAGUSERINFO,
		"RECONFCMD",
		103,
		"STEADY",
		"TRACKER",
		"TEC");

		this->Default(in,out);
	}


	void DummySupervisor::sendReconfStopLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		diagService_->reportError("Send a reconfiguration request Log with ErrorCode 102",DIAGUSERINFO,
		 "RECONFCMD",
		 102,
		 "STEADY",
		 "TRACKER",
		 "TEC");

		this->Default(in,out);
	}


	void DummySupervisor::sendReconfRunLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
	
		diagService_->reportError("Send a reconfiguration request Log with ErrorCode 101",DIAGUSERINFO,
		"RECONFCMD",
		101,
		"STEADY",
		"TRACKER",
		"TEC");

		this->Default(in,out);
	}


	void DummySupervisor::sendReconfLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		diagService_->reportError("Send a reconfiguration request Log with ErrorCode 100 + ExtraBuffer",DIAGUSERINFO,
		"RECONFCMD",
		100,
		"STEADY",
		"TRACKER",
		"TEC",
		"THIS IS MY EXTRA BUFFER CONTENT");
		this->Default(in,out);
	}




	void DummySupervisor::sendTraceLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		diagService_->reportError("This is a STANDARD USERINFO log",DIAGUSERINFO);
		this->Default(in,out);



		//std::cout << "myPixelsConsole is : " << myPixelsConsole.str() << std::endl;
		myPixelsConsole.str("");



	}



	void DummySupervisor::sendDebugLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		diagService_->reportError("Error during the upload of the DCUs.... This is a DCU READOUT ERROR log",DIAGERROR);
		this->Default(in,out);
	}



	void DummySupervisor::sendInfoLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		diagService_->reportError("Failed to submit, waitingWorkLoop queue ful.... This is a DCU WORKLOOP ERROR log",DIAGERROR);
		this->Default(in,out);

	}


	void DummySupervisor::sendWarnLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{


//		diagService_->reportError(true, "HelperClassOne:HelperClassTwo", true, "toto.h", "This is a WARN simple Log",DIAGWARN);
		diagService_->reportError("*** Fed Monitor Log *** ... This is FED DEBUG log",DIAGUSERINFO);
		this->Default(in,out);

	}



	void DummySupervisor::sendUserInfoLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
//		std::string extraBuffer = "<FEDID>106</FEDID>";
//		diagService_->reportError(true, "HelperClassOne", true, "toto.h", "This is a USERINFO simple Log",DIAGUSERINFO, extraBuffer);
		diagService_->reportError("*** Fed Tmon Log *** ... This is FED TEMPERATURE MONITORING log",DIAGUSERINFO);
		this->Default(in,out);
	}



	void DummySupervisor::sendErrorLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
	
//	std::string extraBuffer = "<FECHARDID>3020140B35928A</FECHARDID>";
//		diagService_->reportError(true, "SELF", true, "toto.h", true, 666, "This is a ERROR Log with an unnested extra buffer",DIAGERROR, extraBuffer);
		diagService_->reportError("*** Fed Spy Log *** ... This is FED SPY CHANNEL log",DIAGERROR);
		this->Default(in,out);
	}


	void DummySupervisor::sendFatalLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
/*

std::string extraBuffer = \
"<PIXELS_ERROR_TYPE>0</PIXELS_ERROR_TYPE>\n \
\t\t<TAG1>\n \
\t\t\t<NESTED_TAG2>\n \
\t\t\t\t<NESTED_TAG3> my value </NESTED_TAG3>\n \
\t\t\t</NESTED_TAG2>\n \
\t\t</TAG1>";
*/
//diagService_->reportError(true, "HelperClassOne:HelperClassTwo:Class3", "This is a FATAL example Log With a nested extra buffer", DIAGFATAL, extraBuffer);
diagService_->reportError("This is a FATAL example Log With a nested extra buffer", DIAGFATAL);

		this->Default(in,out);
	}





  /** \brief initiliase all the paremters for the FecSupervisor
   */
void DummySupervisor::configureAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{

	//Apply fireitems value to internals and check validity
	DIAG_EXEC_FSM_INIT_TRANS

}

void DummySupervisor::haltAction(toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
	//Apply fireitems value to internals and check validity
	DIAG_EXEC_FSM_STOP_TRANS

}
   
   
   
xoap::MessageReference DummySupervisor::fireEvent (xoap::MessageReference msg) throw (xoap::exception::Exception)
{
std::cout << "fireEvent" << std::endl ;

  xoap::SOAPPart part = msg->getSOAPPart();
  xoap::SOAPEnvelope env = part.getEnvelope();
  xoap::SOAPBody body = env.getBody();
  DOMNode* node = body.getDOMNode();
  DOMNodeList* bodyList = node->getChildNodes();
  for (unsigned int i = 0; i < bodyList->getLength(); i++)  {

    DOMNode* command = bodyList->item(i);

    if (command->getNodeType() == DOMNode::ELEMENT_NODE) {

      std::string commandName = xoap::XMLCh2String (command->getLocalName());
                    if ( (fsm_.getCurrentState() == 'H') && (commandName == EXECTRANSHC) || (fsm_.getCurrentState() == 'C') && (commandName == EXECTRANSCH) )
                    {

      try {
	toolbox::Event::Reference e(new toolbox::Event(commandName, this));
	fsm_.fireEvent(e);
	// Synchronize Web state machine
	//	      wsm_.setInitialState(fsm_.getCurrentState());
      }
      catch (toolbox::fsm::exception::Exception & e) {
	XCEPT_RETHROW(xcept::Exception, "invalid command", e);
	diagService_->reportError (e.what(), DIAGERROR) ;
      }

      xoap::MessageReference reply = xoap::createMessage();
      xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
      xoap::SOAPName responseName = envelope.createName( commandName +"Response", "xdaq", XDAQ_NS_URI);
      envelope.getBody().addBodyElement ( responseName );
      return reply;
                }
    }
  }

  XCEPT_RAISE(xcept::Exception,"command not found");	
  diagService_->reportError ("Command not found", DIAGERROR) ;

}



void DummySupervisor::failedTransition (toolbox::Event::Reference e) throw (toolbox::fsm::exception::Exception)
{
  toolbox::fsm::FailedEvent & fe = dynamic_cast<toolbox::fsm::FailedEvent&>(*e);
  std::ostringstream msgError ;
  msgError << "Failure occurred when performing transition from: " << fe.getFromState() << " to: " << fe.getToState() << " exception: " << fe.getException().what() ;
  diagService_->reportError (msgError.str(), DIAGERROR) ;
}

void DummySupervisor::configureStateMachine(xgi::Input * in, xgi::Output * out)
{
            if (fsm_.getCurrentState() == 'H')
            {

	toolbox::Event::Reference e(new toolbox::Event(EXECTRANSHC, this));
	fsm_.fireEvent(e);
            }
	Default(in, out);
}

void DummySupervisor::stopStateMachine(xgi::Input * in, xgi::Output * out)
{
            if (fsm_.getCurrentState() == 'C')
            {

	toolbox::Event::Reference e(new toolbox::Event(EXECTRANSCH, this));
	fsm_.fireEvent(e);
            }
	Default(in, out);
}


        #ifdef AUTO_CONFIGURE_PROCESSES
            void DummySupervisor::timeExpired (toolbox::task::TimerEvent& e) 
            {
                DIAG_EXEC_FSM_INIT_TRANS
            }
        #endif





	//RECONFREQUESTED
	xoap::MessageReference DummySupervisor::recoveryEntry(xoap::MessageReference msg) throw (xoap::exception::Exception)
	{
//BSEMMOD
executeReconfMethodMutex->take();
		diagService_->reportError("Entering recoveryEntry method : processing reconfiguration request", DIAGUSERINFO);
		
		// prepare to reply to caller
		xoap::MessageReference reply = xoap::createMessage();
		xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
		xoap::SOAPName responseName = envelope.createName( "receivedLogResponse", "xdaq", XDAQ_NS_URI);
		envelope.getBody().addBodyElement ( responseName );


		//Extract action to perform from message
		std::string reconfAction_;
		xoap::SOAPBody rb = msg->getSOAPPart().getEnvelope().getBody();
		if (rb.hasFault() )
		{
			// MUST raise an exception here
		}
		else
		{
			std::vector<xoap::SOAPElement> reconfElement = rb.getChildElements ();
			if(reconfElement.size()==1)
			{
				xoap::SOAPName reconfAction ("reconfAction", "", "");
				reconfAction_ = reconfElement[0].getAttributeValue(reconfAction);
			}
		}
		std::string infoMsg = (std::string)"I have been asked to perform the recovery action : " + reconfAction_;
		diagService_->reportError(infoMsg, DIAGUSERINFO);


		/* And now perfom action according to reconfAction_ value */
		/*
		execute some recovery code here...
		*/

std::cout << "Starting to sleep..." << std::endl;
sleep((unsigned int)10);

std::cout << "End of sleep" << std::endl;

		diagService_->reportError("Exiting recoveryEntry method",
		 DIAGUSERINFO,
		 "SOME TEXT",
		 587,
		 "STEADY",
		 "TRACKER",
		 "TEC");

//BSEMMOD
executeReconfMethodMutex->give();
	    return reply;
	}


