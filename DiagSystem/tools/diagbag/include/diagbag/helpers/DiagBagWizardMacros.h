/*
   FileName : 		DiagBagMacros.h

   Content : 		a bunch of macros

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


/*******************************************************
********************************************************
********************************************************
MACROS cut & past to be inserted into target code
********************************************************
********************************************************
*******************************************************/

#define DIAG_XGI_BIND_CALLS() \
    xgi::bind(this,&getApplicationDescriptor()->getClassName()::configureDiagSystem, "configureDiagSystem"); \
    xgi::bind(this,&getApplicationDescriptor()->getClassName()::applyConfigureDiagSystem, "applyConfigureDiagSystem");


#define DIAG_SET_SOCKETSOAP_APPENDER(ip, port, lid, command, server) \
    SharedAppenderPtr append_1(new SOAPDiagAppender(LOG4CPLUS_TEXT(ip), port, lid, LOG4CPLUS_TEXT(command),LOG4CPLUS_TEXT(server))); \
    append_1->setName(LOG4CPLUS_TEXT("SocketSOAPDiagAppender")); \
    this->getApplicationLogger().setAdditivity(0); \
    this->getApplicationLogger().removeAllAppenders(); \
    this->getApplicationLogger().addAppender(append_1);



#define DIAG_SET_CONFIG_BUTTON_CALLBACK() \
    std::string urlDiag_ = "/"; \
    urlDiag_ += getApplicationDescriptor()->getURN(); \
    urlDiag_ += "/configureDiagSystem"; \
    std::string buttonName = "Configure Diagnostic System for process : " +	getApplicationDescriptor()->getClassName(); \
    *out << cgicc::form().set("method","post").set("action", urlDiag_).set("enctype","multipart/form-data") << std::endl; \
    *out << cgicc::input().set("type", "submit").set("name", "Apply").set("value", buttonName); \
    *out << cgicc::p() << std::endl; \
    *out << cgicc::form() << std::endl;

#define DIAG_SET_CONFIG_LINK_CALLBACK() \
    std::string urlDiag_ = "/"; \
    urlDiag_ += getApplicationDescriptor()->getURN(); \
    urlDiag_ += "/configureDiagSystem"; \
    *out << "<a href=" << urlDiag_ << " target=\"_blank\">Configure DiagSystem</a>";


#define DIAG_SET_CONFIG_CALLBACK() \
    std::string urlDiag_ = "/"; \
    urlDiag_ += getApplicationDescriptor()->getURN(); \
    urlDiag_ += "/configureDiagSystem"; \
    *out << "<a href=" << urlDiag_ << " target=\"_blank\">Configure DiagSystem</a>";

#define DIAG_SET_CONFIG_FIRE_CALLBACK() \
    std::string urlFireDiag_ = "/"; \
    urlFireDiag_ += getApplicationDescriptor()->getURN(); \
    urlFireDiag_ += "/configureFireDiagSystem"; \
    *out << "<a href=" << urlFireDiag_ << ">Initialise DiagSystem</a>";

	
#define DIAG_CONFIGURE_CALLBACK() \
    configureDiagSystem(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) \
    { \
        diagService_->configureCallback(in, out, getApplicationDescriptor()->getClassName(), \
        getApplicationDescriptor()->getContextDescriptor()->getURL(), \
        getApplicationDescriptor()->getURN() ); \
    }



#define DIAG_REQUEST_ENTRYPOINT() \
    xoap::MessageReference processOnlineDiagRequest(xoap::MessageReference msg) throw (xoap::exception::Exception) \
    { \
	    xoap::MessageReference reply = xoap::createMessage(); \
		diagService_->processOnlineDiagRequest(msg, reply); \
        return reply; \
    } \




#define DIAG_FREELCLSEM_CALLBACK() \
    xoap::MessageReference freeLclDiagSemaphore(xoap::MessageReference msg) throw (xoap::exception::Exception) \
    { \
	    xoap::MessageReference reply = xoap::createMessage(); \
    	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope(); \
	    xoap::SOAPName responseName = envelope.createName( "freeLclDiagSemaphoreResponse", "xdaq", XDAQ_NS_URI); \
    	envelope.getBody().addBodyElement ( responseName ); \
        /* #ifdef NOSLOWDOWN */ \
                    /* std::cout << "In freeLclDiagSemaphore :: setting isWaitingForLclLogAck_ to FALSE" << std::endl; */ \
                    diagService_->isWaitingForLclLogAck_ = false; \
                    /* if (diagService_->isWaitingForLclLogAck_ == true) \
                    { \
                        std::cout << "In freeLclDiagSemaphore :: isWaitingForLclLogAck_ is TRUE" << std::endl << std::endl << std::endl << std::endl; \
                    } \
                    else std::cout << "In freeLclDiagSemaphore :: isWaitingForLclLogAck_ is FALSE" << std::endl << std::endl << std::endl << std::endl; */ \
        /* ##else \
                diagService_->lclMessagesPooled_--; \
        ##endif */ \
        return reply; \
    } \


#define DIAG_FREEGLBSEM_CALLBACK() \
    xoap::MessageReference freeGlbDiagSemaphore(xoap::MessageReference msg) throw (xoap::exception::Exception) \
    { \
	    xoap::MessageReference reply = xoap::createMessage(); \
    	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope(); \
	    xoap::SOAPName responseName = envelope.createName( "freeGlbDiagSemaphoreResponse", "xdaq", XDAQ_NS_URI); \
    	envelope.getBody().addBodyElement ( responseName ); \
        /* #ifdef NOSLOWDOWN */ \
                diagService_->isWaitingForGlbLogAck_ = false; \
        /* ##else \
                diagService_->glbMessagesPooled_--; \
        ##endif */ \
        return reply; \
    } \




#define DIAG_CONFIGURE_FIRE_CALLBACK() \
    configureFireDiagSystem(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) \
    { \
        if (diagService_->getIsInUserProcess() == true) \
        { \
            diagService_->applyUserAppFireItems(); \
            /* std::cout << "DEBUG : calling DIAG_APPLY_CALLBACK_FOR_FIRST_TIME" << std::endl;*/ \
            DIAG_APPLY_CALLBACK_FOR_FIRST_TIME \
        } \
        if (diagService_->getIsInErrorDispatcher() == true)\
        { \
            diagService_->applyEdAppFireItems(); \
            /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
            DIAG_APPLY_ED_CALLBACK_FOR_FIRST_TIME \
        } \
        if (diagService_->getIsInGlobalErrorDispatcher() == true)\
        { \
            diagService_->applyGlbEdAppFireItems(); \
            /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
            DIAG_APPLY_GLB_ED_CALLBACK_FOR_FIRST_TIME \
        } \
        if (diagService_->getIsInErrorManager() == true) \
        { \
            diagService_->applyEmAppFireItems(); \
            /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
            DIAG_APPLY_EM_CALLBACK_FOR_FIRST_TIME \
        } \
        if (diagService_->getIsInSentinelListener() == true) \
        { \
            diagService_->applySentinelGrabberAppFireItems(); \
            /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
            DIAG_APPLY_SENTINEL_CALLBACK_FOR_FIRST_TIME\
        } \
        if (diagService_->getIsInLogsReader() == true) \
        { \
            diagService_->applyLogsReaderAppFireItems(); \
            /* std::cout << "Calling Callback for LogsReader" << std::endl; */ \
            DIAG_APPLY_LOGS_READER_CALLBACK_FOR_FIRST_TIME\
        } \
        this->Default(in,out) ; \
    }


#define DIAG_APPLY_CALLBACK_FOR_FIRST_TIME \
	/* diagService_->checkUseDiagSystem();*/ \
	/* std::cout << "Applying user callback" << std::endl;*/ \
	if (diagService_->checkUseDiagSystem() == false) \
	{ \
    /* std::cout << "DEBUG :  checkUseEd() fired" << std::endl; */ \
    diagService_->checkUseEd(); \
    /* std::cout << "DEBUG :  checkUseEd() ended" << std::endl; */ \
    /* std::cout << "DEBUG :  checkUseEm() fired" << std::endl; */ \
    diagService_->checkUseEm(); \
    /* std::cout << "DEBUG :  checkUseEm() ended" << std::endl; */ \
    /* std::cout << "DEBUG :  checkUseSentinel() fired" << std::endl; */ \
    diagService_->checkUseSentinel(); \
    /* std::cout << "DEBUG :  checkUseSentinel() ended" << std::endl; */ \
    /* std::cout << "DEBUG :  checkRelayedLvl1Status() fired" << std::endl; */ \
    /*diagService_->checkRelayedLvl1Status();*/ \
    /* std::cout << "DEBUG :  checkRelayedLvl1Status() ended" << std::endl; */\
    /*std::cout << "DEBUG :  checkUseDefaultAppender() fired" << std::endl; */\
    diagService_->checkUseDefaultAppender(); \
    /*std::cout << "DEBUG :  checkUseDefaultAppender() ended" << std::endl; */\
    /*std::cout << "DEBUG :  setLogLevel() fired" << std::endl; */\
    diagService_->checkUseGlbEd(); \
	 } \
    this->getApplicationLogger().setLogLevel(diagService_->getProcessLogLevel("DEFAULT")); \
    /*std::cout << "DEBUG :  setLogLevel() ended" << std::endl; */\
    /*std::cout << "DEBUG :  setConsoleDumpLevel() fired" << std::endl; */\
    diagService_->setConsoleDumpLevel(diagService_->getGLogLevel()); \
    /*std::cout << "DEBUG :  setConsoleDumpLevel() ended" << std::endl; */\





#define DIAG_APPLY_ED_CALLBACK_FOR_FIRST_TIME \
    /* std::cout << "Entering DIAG_APPLY_ED_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
	if (diagService_->checkUseDiagSystem() == false) \
	{ \
    /*std::cout << "DEBUG :  checkUseEd() fired" << std::endl; */\
    diagService_->checkUseEd(); \
    /*std::cout << "DEBUG :  checkUseEd() ended" << std::endl; */\
    /*std::cout << "DEBUG :  checkEdOptions() fired" << std::endl; */\
    diagService_->checkEdOptions(); \
    /*std::cout << "DEBUG :  checkEdOptions() ended" << std::endl; */\
    /* std::cout << "DEBUG :  checkUseEm() fired" << std::endl; */ \
    diagService_->checkUseEm(); \
    /* std::cout << "DEBUG :  checkUseEm() ended" << std::endl; */ \
    /*std::cout << "DEBUG :  checkUseSentinel() fired" << std::endl; */\
    diagService_->checkUseSentinel(); \
    /*std::cout << "DEBUG :  checkUseSentinel() ended" << std::endl; */\
    /*std::cout << "DEBUG :  checkUseDefaultAppender() fired" << std::endl; */\
    diagService_->checkUseDefaultAppender(); \
    /*std::cout << "DEBUG :  checkUseDefaultAppender() ended" << std::endl; */\
    /*std::cout << "DEBUG :  setLogLevel() fired" << std::endl; */\
    diagService_->checkUseGlbEd(); \
	} \
    this->getApplicationLogger().setLogLevel(diagService_->getProcessLogLevel("DEFAULT")); \
    /*std::cout << "DEBUG :  setLogLevel() ended" << std::endl; */\
    /*std::cout << "DEBUG :  setConsoleDumpLevel() fired" << std::endl; */\
    diagService_->setConsoleDumpLevel(diagService_->getGLogLevel()); \
    /*std::cout << "DEBUG :  setConsoleDumpLevel() fired" << std::endl; */\


#define DIAG_APPLY_GLB_ED_CALLBACK_FOR_FIRST_TIME \
     /* std::cout << "Entering DIAG_APPLY_GLB_ED_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
	if (diagService_->checkUseDiagSystem() == false) \
	{ \
    /* std::cout << "DEBUG :  checkUseEd() fired" << std::endl; */\
    diagService_->checkUseEd(); \
    /*std::cout << "DEBUG :  checkUseEd() ended" << std::endl; */\
    /* std::cout << "DEBUG :  checkEdOptions() fired" << std::endl; */\
    diagService_->checkEdOptions(); \
    /*std::cout << "DEBUG :  checkEdOptions() ended" << std::endl; */\
     /* std::cout << "DEBUG :  checkUseEm() fired" << std::endl;  */\
    diagService_->checkUseEm(); \
    /* std::cout << "DEBUG :  checkUseEm() ended" << std::endl; */ \
    /*std::cout << "DEBUG :  checkUseSentinel() fired" << std::endl; */\
    diagService_->checkUseSentinel(); \
    /*std::cout << "DEBUG :  checkUseSentinel() ended" << std::endl; */\
    /*std::cout << "DEBUG :  checkUseDefaultAppender() fired" << std::endl; */\
    diagService_->checkUseDefaultAppender(); \
    /*std::cout << "DEBUG :  checkUseDefaultAppender() ended" << std::endl; */\
     /*std::cout << "DEBUG :  checkUseGlbEd() fired" << std::endl; */\
    diagService_->checkUseGlbEd(); \
	} \
    /*std::cout << "DEBUG :  setLogLevel() fired" << std::endl; */\
    this->getApplicationLogger().setLogLevel(diagService_->getProcessLogLevel("DEFAULT")); \
    /*std::cout << "DEBUG :  setLogLevel() ended" << std::endl; */\
     /*std::cout << "DEBUG :  setConsoleDumpLevel() fired" << std::endl; */\
    diagService_->setConsoleDumpLevel(diagService_->getGLogLevel()); \



#define DIAG_APPLY_EM_CALLBACK_FOR_FIRST_TIME \
	if (diagService_->checkUseDiagSystem() == false) \
	{ \
    diagService_->checkUseEd(); \
    diagService_->checkUseEm(); \
    diagService_->checkUseSentinel(); \
    diagService_->checkUseDefaultAppender(); \
    diagService_->checkUseGlbEd(); \
	} \
    this->getApplicationLogger().setLogLevel(diagService_->getProcessLogLevel("DEFAULT")); \
    diagService_->setConsoleDumpLevel(diagService_->getGLogLevel()); \




#define DIAG_APPLY_SENTINEL_CALLBACK_FOR_FIRST_TIME \
	if (diagService_->checkUseDiagSystem() == false) \
	{ \
    diagService_->checkUseEd(); \
    diagService_->checkUseEm(); \
    diagService_->checkUseSentinel(); \
    diagService_->checkUseSentinelGrabber(); \
    diagService_->checkUseDefaultAppender(); \
    diagService_->checkUseGlbEd(); \
	} \
    this->getApplicationLogger().setLogLevel(diagService_->getProcessLogLevel("DEFAULT")); \
    diagService_->setConsoleDumpLevel(diagService_->getGLogLevel()); \





#define DIAG_APPLY_LOGS_READER_CALLBACK_FOR_FIRST_TIME \
    /* std::cout << "Entering DIAG_APPLY_ED_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
	if (diagService_->checkUseDiagSystem() == false) \
	{ \
    /*std::cout << "DEBUG :  checkUseEd() fired" << std::endl; */\
    diagService_->checkUseEd(); \
    /*std::cout << "DEBUG :  checkUseEd() ended" << std::endl; */\
    /* std::cout << "DEBUG :  checkUseEm() fired" << std::endl; */ \
    diagService_->checkUseEm(); \
    /* std::cout << "DEBUG :  checkUseEm() ended" << std::endl; */ \
    /*std::cout << "DEBUG :  checkEdOptions() fired" << std::endl; */\
    /* diagService_->checkEdOptions(); */ \
    /*std::cout << "DEBUG :  checkEdOptions() ended" << std::endl; */\
    /*std::cout << "DEBUG :  checkUseSentinel() fired" << std::endl; */\
    diagService_->checkUseSentinel(); \
    /*std::cout << "DEBUG :  checkUseSentinel() ended" << std::endl; */\
    /*std::cout << "DEBUG :  checkUseDefaultAppender() fired" << std::endl; */\
    diagService_->checkUseDefaultAppender(); \
    /*std::cout << "DEBUG :  checkUseDefaultAppender() ended" << std::endl; */\
    /*std::cout << "DEBUG :  setLogLevel() fired" << std::endl; */\
    diagService_->checkUseGlbEd(); \
    /*diagService_->checkLrOptions(); */\
	} \
    this->getApplicationLogger().setLogLevel(diagService_->getProcessLogLevel("DEFAULT")); \
    /*std::cout << "DEBUG :  setLogLevel() ended" << std::endl; */\
    /*std::cout << "DEBUG :  setConsoleDumpLevel() fired" << std::endl; */\
    diagService_->setConsoleDumpLevel(diagService_->getGLogLevel()); \
    /*std::cout << "DEBUG :  setConsoleDumpLevel() fired" << std::endl; */\
    /*diagService_->checkLrOptions(); */\




#define DIAG_APPLY_CALLBACK() \
    applyConfigureDiagSystem(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception) \
    { \
        /* std::cout << "Calling applyConfigureDiagSystem()" << std::endl; */\
        diagService_->getFormValues(in, out); \
        /* std::cout << "Called getFormValues()" << std::endl; */\
        if (diagService_->getIsInUserProcess() == true) \
        { \
            /* std::cout << "Calling DIAG_APPLY_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
            DIAG_APPLY_CALLBACK_FOR_FIRST_TIME \
            /* std::cout << "Exiting DIAG_APPLY_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
        } \
        if (diagService_->getIsInErrorDispatcher() == true)\
        { \
            /* std::cout << "Calling DIAG_APPLY_ED_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
            DIAG_APPLY_ED_CALLBACK_FOR_FIRST_TIME \
            /* std::cout << "Exiting DIAG_APPLY_ED_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
        } \
        if (diagService_->getIsInGlobalErrorDispatcher() == true)\
        { \
            /* std::cout << "Calling DIAG_APPLY_GLB_ED_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
            DIAG_APPLY_GLB_ED_CALLBACK_FOR_FIRST_TIME \
            /* std::cout << "Exiting DIAG_APPLY_GLB_ED_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
        } \
        if (diagService_->getIsInErrorManager() == true) \
        { \
            /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
            DIAG_APPLY_EM_CALLBACK_FOR_FIRST_TIME \
        } \
        if (diagService_->getIsInSentinelListener() == true) \
        { \
            /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
            DIAG_APPLY_SENTINEL_CALLBACK_FOR_FIRST_TIME\
        } \
        if (diagService_->getIsInLogsReader() == true) \
        { \
            /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
            DIAG_APPLY_LOGS_READER_CALLBACK_FOR_FIRST_TIME\
        } \
        this->Default(in,out) ;\
	}



#define DIAG_DECLARE_USER_APP \
    diagService_->setIsInUserProcess(true); \
    diagService_->setIsInErrorDispatcher(false); \
    diagService_->setIsInGlobalErrorDispatcher(false); \
    diagService_->setIsInSentinelListener(false); \
    diagService_->setIsInErrorManager(false); \
    diagService_->setIsInLogsReader(false); \
\
    diagService_->feUseDefaultAppender_ = true; \
    diagService_->feDefaultAppenderLogLevel_ = DIAGERROR; \
\
/*
declareParameter(this,"UseDiagSystem",&diagService_->feUseDiagSystem_,"Use dedicated diagnostic system","Diagnostic System");\
declareParameter(this,"LogLevel",&diagService_->feLogLevel_,"Dedicated diagnostic system default log level","Diagnostic System");\
declareParameter(this,"DiagSystemSettings",&diagService_->feDiagSystemSettings_,"Configuration parameters source","Diagnostic System");\
\
declareParameter(this,"UseDefaultAppender",&diagService_->feDefaultAppenderLogLevel_,"Route messages to Log4CPlus logger","Diagnostic System");\
declareParameter(this,"DefaultAppenderLogLevel",&diagService_->feDefaultAppenderLogLevel_,"Log4CPlus logger default log level","Diagnostic System");\
\
declareParameter(this,"UseSentinel",&diagService_->feUseSentinel_,"Route messages to sentinel logger","Diagnostic System");\
declareParameter(this,"SentinelAppenderLogLevel",&diagService_->feSentinelAppenderLogLevel_,"Sentinel logger default log level","Diagnostic System");\
\
declareParameter(this,"UseGlbErrorDispatcher",&diagService_->feUseGlbErrorDispatcher_,"Route messages to a Global Error Dispatcher","Diagnostic System");\
declareParameter(this,"GlbErrorDispatcherLogLevel",&diagService_->feGlbErrorDispatcherLogLevel_,"Global Error Dispatcher default log level","Diagnostic System");*/\
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseDiagSystem"),&diagService_->feUseDiagSystem_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("LogLevel"),&diagService_->feLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DiagSystemSettings"),&diagService_->feDiagSystemSettings_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseDefaultAppender"),&diagService_->feUseDefaultAppender_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DefaultAppenderLogLevel"),&diagService_->feDefaultAppenderLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DuplicateMsgToLocalConsole"),&diagService_->feDuplicateMsgToLocalConsole_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseSentinel"),&diagService_->feUseSentinel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("SentinelAppenderLogLevel"),&diagService_->feSentinelAppenderLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("PushErrorsToContext"),&diagService_->fePushErrorsToContext_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseSentinelRelayedEm"),&diagService_->feUseSentinelRelayedEm_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseErrorDispatcher"),&diagService_->feUseErrorDispatcher_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherLogLevel"),&diagService_->feErrorDispatcherLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherName"),&diagService_->feErrorDispatcherName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherInstance"),&diagService_->feErrorDispatcherInstance_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherLID"),&diagService_->feErrorDispatcherLID_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherIP"),&diagService_->feErrorDispatcherIP_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherPort"),&diagService_->feErrorDispatcherPort_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseGlbErrorDispatcher"),&diagService_->feUseGlbErrorDispatcher_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherLogLevel"),&diagService_->feGlbErrorDispatcherLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherName"),&diagService_->feGlbErrorDispatcherName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherInstance"),&diagService_->feGlbErrorDispatcherInstance_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherLID"),&diagService_->feGlbErrorDispatcherLID_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherIP"),&diagService_->feGlbErrorDispatcherIP_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherPort"),&diagService_->feGlbErrorDispatcherPort_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseErrorManager"),&diagService_->feUseErrorManager_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorManagerName"),&diagService_->feErrorManagerName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorManagerInstance"),&diagService_->feErrorManagerInstance_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorManagerLID"),&diagService_->feErrorManagerLID_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorManagerIP"),&diagService_->feErrorManagerIP_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorManagerPort"),&diagService_->feErrorManagerPort_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("ForceDbLogging"),&diagService_->feForceDbLogging_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ForcedDbLoggingLevel"),&diagService_->feForcedDbLoggingLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ForceChainsawLogging"),&diagService_->feForceChainsawLogging_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ForcedChainsawLoggingLevel"),&diagService_->feForcedChainsawLoggingLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ForceConsoleLogging"),&diagService_->feForceConsoleLogging_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ForcedConsoleLoggingLevel"),&diagService_->feForcedConsoleLoggingLevel_); \
\
\





#define DIAG_DECLARE_EM_APP \
    diagService_->setIsInSentinelListener(false); \
    diagService_->setIsInUserProcess(false); \
    diagService_->setIsInErrorDispatcher(false); \
    diagService_->setIsInGlobalErrorDispatcher(false); \
    diagService_->setIsInErrorManager(true); \
    diagService_->setIsInLogsReader(false); \
\
    diagService_->feUseDefaultAppender_ = true; \
    diagService_->feDefaultAppenderLogLevel_ = DIAGERROR; \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseDiagSystem"),&diagService_->feUseDiagSystem_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("LogLevel"),&diagService_->feLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DiagSystemSettings"),&diagService_->feDiagSystemSettings_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseDefaultAppender"),&diagService_->feUseDefaultAppender_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DefaultAppenderLogLevel"),&diagService_->feDefaultAppenderLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DuplicateMsgToLocalConsole"),&diagService_->feDuplicateMsgToLocalConsole_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseSentinel"),&diagService_->feUseSentinel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("SentinelAppenderLogLevel"),&diagService_->feSentinelAppenderLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("PushErrorsToContext"),&diagService_->fePushErrorsToContext_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseErrorDispatcher"),&diagService_->feUseErrorDispatcher_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherLogLevel"),&diagService_->feErrorDispatcherLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherName"),&diagService_->feErrorDispatcherName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherInstance"),&diagService_->feErrorDispatcherInstance_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherLID"),&diagService_->feErrorDispatcherLID_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherIP"),&diagService_->feErrorDispatcherIP_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherPort"),&diagService_->feErrorDispatcherPort_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("CloseReconfLoop"),&diagService_->feCloseReconfLoop_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ReconfProcessName"),&diagService_->feReconfProcessName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ReconfProcessInstance"),&diagService_->feReconfProcessInstance_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ReconfProcessLID"),&diagService_->feReconfProcessLID_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ReconfProcessIP"),&diagService_->feReconfProcessIP_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ReconfProcessPort"),&diagService_->feReconfProcessPort_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseGlbErrorDispatcher"),&diagService_->feUseGlbErrorDispatcher_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherLogLevel"),&diagService_->feGlbErrorDispatcherLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherName"),&diagService_->feGlbErrorDispatcherName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherInstance"),&diagService_->feGlbErrorDispatcherInstance_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherLID"),&diagService_->feGlbErrorDispatcherLID_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherIP"),&diagService_->feGlbErrorDispatcherIP_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherPort"),&diagService_->feGlbErrorDispatcherPort_); \


#define DIAG_DECLARE_SENTINEL_GRABBER_APP \
    diagService_->setIsInSentinelListener(true); \
    diagService_->setIsInUserProcess(false); \
    diagService_->setIsInErrorDispatcher(false); \
    diagService_->setIsInGlobalErrorDispatcher(false); \
    diagService_->setIsInErrorManager(false); \
    diagService_->setIsInLogsReader(false); \
\
    diagService_->feUseDefaultAppender_ = true; \
    diagService_->feDefaultAppenderLogLevel_ = DIAGERROR; \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseDiagSystem"),&diagService_->feUseDiagSystem_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("LogLevel"),&diagService_->feLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DiagSystemSettings"),&diagService_->feDiagSystemSettings_); \
\
    xgi::bind(this,&DiagSentinelErrorsGrabber::configureDiagSystem, "configureDiagSystem"); \
    xgi::bind(this,&DiagSentinelErrorsGrabber::applyConfigureDiagSystem, "applyConfigureDiagSystem"); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseDefaultAppender"),&diagService_->feUseDefaultAppender_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DefaultAppenderLogLevel"),&diagService_->feDefaultAppenderLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DuplicateMsgToLocalConsole"),&diagService_->feDuplicateMsgToLocalConsole_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseSentinel"),&diagService_->feUseSentinel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("SentinelAppenderLogLevel"),&diagService_->feSentinelAppenderLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("PushErrorsToContext"),&diagService_->fePushErrorsToContext_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseErrorDispatcher"),&diagService_->feUseErrorDispatcher_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherLogLevel"),&diagService_->feErrorDispatcherLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherName"),&diagService_->feErrorDispatcherName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherInstance"),&diagService_->feErrorDispatcherInstance_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherLID"),&diagService_->feErrorDispatcherLID_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherIP"),&diagService_->feErrorDispatcherIP_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherPort"),&diagService_->feErrorDispatcherPort_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("ListenSentinel"),&diagService_->feListenSentinel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ListenContext"),&diagService_->feListenContext_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseGlbErrorDispatcher"),&diagService_->feUseGlbErrorDispatcher_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherLogLevel"),&diagService_->feGlbErrorDispatcherLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherName"),&diagService_->feGlbErrorDispatcherName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherInstance"),&diagService_->feGlbErrorDispatcherInstance_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherLID"),&diagService_->feGlbErrorDispatcherLID_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherIP"),&diagService_->feGlbErrorDispatcherIP_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherPort"),&diagService_->feGlbErrorDispatcherPort_); \






#define DIAG_DECLARE_ED_APP \
    diagService_->setIsInErrorDispatcher(true); \
    diagService_->setIsInGlobalErrorDispatcher(false); \
    diagService_->setIsInSentinelListener(false); \
    diagService_->setIsInUserProcess(false); \
    diagService_->setIsInErrorManager(false); \
    diagService_->setIsInLogsReader(false); \
\
    diagService_->feUseDefaultAppender_ = true; \
    diagService_->feDefaultAppenderLogLevel_ = DIAGERROR; \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseDiagSystem"),&diagService_->feUseDiagSystem_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("LogLevel"),&diagService_->feLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DiagSystemSettings"),&diagService_->feDiagSystemSettings_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseDefaultAppender"),&diagService_->feUseDefaultAppender_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DefaultAppenderLogLevel"),&diagService_->feDefaultAppenderLogLevel_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseSentinel"),&diagService_->feUseSentinel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("SentinelAppenderLogLevel"),&diagService_->feSentinelAppenderLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("PushErrorsToContext"),&diagService_->fePushErrorsToContext_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseErrorDispatcher"),&diagService_->feUseErrorDispatcher_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherLogLevel"),&diagService_->feErrorDispatcherLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherName"),&diagService_->feErrorDispatcherName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherInstance"),&diagService_->feErrorDispatcherInstance_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherLID"),&diagService_->feErrorDispatcherLID_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherIP"),&diagService_->feErrorDispatcherIP_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherPort"),&diagService_->feErrorDispatcherPort_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseChainsaw"),&diagService_->feUseChainsaw_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ChainsawServerHost"),&diagService_->feChainsawServerHost_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ChainsawServerPort"),&diagService_->feChainsawServerPort_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ChainsawReceiverName"),&diagService_->feChainsawReceiverName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ChainsawLogLevel"),&diagService_->feChainsawLogLevel_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseConsoleLogging"),&diagService_->feUseConsoleLogging_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ConsoleLogLevel"),&diagService_->feConsoleLogLevel_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseDbLogging"),&diagService_->feUseDbLogging_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DbLogLevel"),&diagService_->feDbLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DbFilePathAndName"),&diagService_->feDbFilePathAndName_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseFileLogging"),&diagService_->feUseFileLogging_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("FileLogLevel"),&diagService_->feFileLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("FilePathAndName"),&diagService_->feFilePathAndName_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseGlbErrorDispatcher"),&diagService_->feUseGlbErrorDispatcher_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherLogLevel"),&diagService_->feGlbErrorDispatcherLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherName"),&diagService_->feGlbErrorDispatcherName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherInstance"),&diagService_->feGlbErrorDispatcherInstance_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherLID"),&diagService_->feGlbErrorDispatcherLID_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherIP"),&diagService_->feGlbErrorDispatcherIP_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherPort"),&diagService_->feGlbErrorDispatcherPort_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseErrorManager"),&diagService_->feUseErrorManager_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorManagerName"),&diagService_->feErrorManagerName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorManagerInstance"),&diagService_->feErrorManagerInstance_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("LogFilePath"),&diagService_->feLogFilePath_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("RunFromRCMS"),&diagService_->feRunFromRcms_); \




#define DIAG_DECLARE_LR_APP \
    diagService_->setIsInErrorDispatcher(false); \
    diagService_->setIsInGlobalErrorDispatcher(false); \
    diagService_->setIsInSentinelListener(false); \
    diagService_->setIsInUserProcess(false); \
    diagService_->setIsInErrorManager(false); \
    diagService_->setIsInLogsReader(true); \
\
    diagService_->feUseDefaultAppender_ = true; \
    diagService_->feDefaultAppenderLogLevel_ = DIAGERROR; \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseDiagSystem"),&diagService_->feUseDiagSystem_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("LogLevel"),&diagService_->feLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DiagSystemSettings"),&diagService_->feDiagSystemSettings_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseDefaultAppender"),&diagService_->feUseDefaultAppender_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DefaultAppenderLogLevel"),&diagService_->feDefaultAppenderLogLevel_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseSentinel"),&diagService_->feUseSentinel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("SentinelAppenderLogLevel"),&diagService_->feSentinelAppenderLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("PushErrorsToContext"),&diagService_->fePushErrorsToContext_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseErrorDispatcher"),&diagService_->feUseErrorDispatcher_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherLogLevel"),&diagService_->feErrorDispatcherLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherName"),&diagService_->feErrorDispatcherName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherInstance"),&diagService_->feErrorDispatcherInstance_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherLID"),&diagService_->feErrorDispatcherLID_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherIP"),&diagService_->feErrorDispatcherIP_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherPort"),&diagService_->feErrorDispatcherPort_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseChainsaw"),&diagService_->feUseChainsaw_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ChainsawServerHost"),&diagService_->feChainsawServerHost_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ChainsawServerPort"),&diagService_->feChainsawServerPort_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ChainsawReceiverName"),&diagService_->feChainsawReceiverName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ChainsawLogLevel"),&diagService_->feChainsawLogLevel_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseConsoleLogging"),&diagService_->feUseConsoleLogging_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ConsoleLogLevel"),&diagService_->feConsoleLogLevel_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseDbLogging"),&diagService_->feUseDbLogging_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DbLogLevel"),&diagService_->feDbLogLevel_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseFileLogging"),&diagService_->feUseFileLogging_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("FileLogLevel"),&diagService_->feFileLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("FilePathAndName"),&diagService_->feFilePathAndName_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseGlbErrorDispatcher"),&diagService_->feUseGlbErrorDispatcher_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherLogLevel"),&diagService_->feGlbErrorDispatcherLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherName"),&diagService_->feGlbErrorDispatcherName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherInstance"),&diagService_->feGlbErrorDispatcherInstance_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherLID"),&diagService_->feGlbErrorDispatcherLID_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherIP"),&diagService_->feGlbErrorDispatcherIP_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherPort"),&diagService_->feGlbErrorDispatcherPort_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseErrorManager"),&diagService_->feUseErrorManager_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorManagerName"),&diagService_->feErrorManagerName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorManagerInstance"),&diagService_->feErrorManagerInstance_); \





#define DIAG_DECLARE_GLB_ED_APP \
    diagService_->setIsInErrorDispatcher(false); \
    diagService_->setIsInGlobalErrorDispatcher(true); \
    diagService_->setIsInSentinelListener(false); \
    diagService_->setIsInUserProcess(false); \
    diagService_->setIsInErrorManager(false); \
    diagService_->setIsInLogsReader(false); \
\
    diagService_->feUseDefaultAppender_ = true; \
    diagService_->feDefaultAppenderLogLevel_ = DIAGERROR; \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseDiagSystem"),&diagService_->feUseDiagSystem_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("LogLevel"),&diagService_->feLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DiagSystemSettings"),&diagService_->feDiagSystemSettings_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseDefaultAppender"),&diagService_->feUseDefaultAppender_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DefaultAppenderLogLevel"),&diagService_->feDefaultAppenderLogLevel_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseSentinel"),&diagService_->feUseSentinel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("SentinelAppenderLogLevel"),&diagService_->feSentinelAppenderLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("PushErrorsToContext"),&diagService_->fePushErrorsToContext_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseErrorDispatcher"),&diagService_->feUseErrorDispatcher_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherLogLevel"),&diagService_->feErrorDispatcherLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherName"),&diagService_->feErrorDispatcherName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherInstance"),&diagService_->feErrorDispatcherInstance_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherLID"),&diagService_->feErrorDispatcherLID_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherIP"),&diagService_->feErrorDispatcherIP_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorDispatcherPort"),&diagService_->feErrorDispatcherPort_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseChainsaw"),&diagService_->feUseChainsaw_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ChainsawServerHost"),&diagService_->feChainsawServerHost_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ChainsawServerPort"),&diagService_->feChainsawServerPort_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ChainsawReceiverName"),&diagService_->feChainsawReceiverName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ChainsawLogLevel"),&diagService_->feChainsawLogLevel_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseConsoleLogging"),&diagService_->feUseConsoleLogging_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ConsoleLogLevel"),&diagService_->feConsoleLogLevel_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseDbLogging"),&diagService_->feUseDbLogging_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DbLogLevel"),&diagService_->feDbLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("DbFilePathAndName"),&diagService_->feDbFilePathAndName_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseFileLogging"),&diagService_->feUseFileLogging_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("FileLogLevel"),&diagService_->feFileLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("FilePathAndName"),&diagService_->feFilePathAndName_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseGlbErrorDispatcher"),&diagService_->feUseGlbErrorDispatcher_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherLogLevel"),&diagService_->feGlbErrorDispatcherLogLevel_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherName"),&diagService_->feGlbErrorDispatcherName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherInstance"),&diagService_->feGlbErrorDispatcherInstance_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherLID"),&diagService_->feGlbErrorDispatcherLID_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherIP"),&diagService_->feGlbErrorDispatcherIP_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("GlbErrorDispatcherPort"),&diagService_->feGlbErrorDispatcherPort_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("UseErrorManager"),&diagService_->feUseErrorManager_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorManagerName"),&diagService_->feErrorManagerName_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("ErrorManagerInstance"),&diagService_->feErrorManagerInstance_); \
    getApplicationInfoSpace()->fireItemAvailable(std::string("LogFilePath"),&diagService_->feLogFilePath_); \
\
    getApplicationInfoSpace()->fireItemAvailable(std::string("RunFromRCMS"),&diagService_->feRunFromRcms_); \




#define DIAG_EXEC_FSM_TRANS \
    std::cout << "WARNING : OBSOLETE CALL TO DIAG_EXEC_FSM_TRANS" << std::endl; \
    std::cout << "Please modify your source code to call either DIAG_EXEC_FSM_INIT_TRANS or DIAG_EXEC_FSM_STOP_TRANS" << std::endl; \
    if (diagService_->getIsInUserProcess() == true) \
    { \
    	/*std::cout << " In DIAG_EXEC_FSM_TRANS : fireing applyUserAppFireItems" << std::endl; */\
        diagService_->applyUserAppFireItems(); \
    	/*std::cout << " In DIAG_EXEC_FSM_TRANS : exiting applyUserAppFireItems - fireing DIAG_APPLY_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
        /* std::cout << "DEBUG : calling DIAG_APPLY_CALLBACK_FOR_FIRST_TIME" << std::endl;*/ \
        DIAG_APPLY_CALLBACK_FOR_FIRST_TIME \
    	/*std::cout << " In DIAG_EXEC_FSM_TRANS : exiting DIAG_APPLY_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
    } \
    if (diagService_->getIsInErrorDispatcher() == true)\
    { \
    	/*std::cout << " In DIAG_EXEC_FSM_TRANS : fireing applyEdAppFireItems" << std::endl; */\
        diagService_->applyEdAppFireItems(); \
    	/*std::cout << " In DIAG_EXEC_FSM_TRANS : exiting applyEdAppFireItems - fireing DIAG_APPLY_ED_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
        /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
        DIAG_APPLY_ED_CALLBACK_FOR_FIRST_TIME \
    	/*std::cout << " In DIAG_EXEC_FSM_TRANS : exiting DIAG_APPLY_ED_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
    } \
    if (diagService_->getIsInGlobalErrorDispatcher() == true)\
    { \
    	/*std::cout << " In DIAG_EXEC_FSM_TRANS : fireing applyGlbEdAppFireItems" << std::endl; */\
        diagService_->applyGlbEdAppFireItems(); \
    	/*std::cout << " In DIAG_EXEC_FSM_TRANS : exiting applyGlbEdAppFireItems - fireing DIAG_APPLY_GLB_ED_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
        /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
        DIAG_APPLY_GLB_ED_CALLBACK_FOR_FIRST_TIME \
    	/*std::cout << " In DIAG_EXEC_FSM_TRANS : exiting DIAG_APPLY_GLB_ED_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
    } \
    if (diagService_->getIsInErrorManager() == true) \
    { \
    	/*std::cout << " In DIAG_EXEC_FSM_TRANS : fireing applyEmAppFireItems" << std::endl; */\
        diagService_->applyEmAppFireItems(); \
    	/*std::cout << " In DIAG_EXEC_FSM_TRANS : exiting applyEmAppFireItems - fireing DIAG_APPLY_EM_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
        /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
        DIAG_APPLY_EM_CALLBACK_FOR_FIRST_TIME \
    	/*std::cout << " In DIAG_EXEC_FSM_TRANS : exiting DIAG_APPLY_EM_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
    } \
    if (diagService_->getIsInSentinelListener() == true) \
    { \
    	/*std::cout << " In DIAG_EXEC_FSM_TRANS : fireing applySentinelGrabberAppFireItems" << std::endl; */\
        diagService_->applySentinelGrabberAppFireItems(); \
    	/*std::cout << " In DIAG_EXEC_FSM_TRANS : exiting applySentinelGrabberAppFireItems - fireing DIAG_APPLY_SENTINEL_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
        /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
        DIAG_APPLY_SENTINEL_CALLBACK_FOR_FIRST_TIME\
    	/*std::cout << " In DIAG_EXEC_FSM_TRANS : exiting DIAG_APPLY_SENTINEL_CALLBACK_FOR_FIRST_TIME" << std::endl; */\
    } \
    /*std::cout << "WARNING : DIAG_EXEC_FSM_TRANS ended" << std::endl; */\


#define DIAG_EXEC_FSM_INIT_TRANS \
    /*std::cout << "DEBUG : DIAG_EXEC_FSM_INIT_TRANS fired" << std::endl; */\
    if (diagService_->getIsInUserProcess() == true) \
    { \
        /*std::cout << "DEBUG : applyUserAppFireItems fired" << std::endl; */\
        diagService_->applyUserAppFireItems();\
        /*std::cout << "DEBUG : applyUserAppFireItems ended : DIAG_APPLY_CALLBACK_FOR_FIRST_TIME fired" << std::endl; */\
        /* std::cout << "DEBUG : calling DIAG_APPLY_CALLBACK_FOR_FIRST_TIME" << std::endl;*/ \
        DIAG_APPLY_CALLBACK_FOR_FIRST_TIME \
        /*std::cout << "DEBUG : DIAG_APPLY_CALLBACK_FOR_FIRST_TIME ended" << std::endl; */\
    } \
    if (diagService_->getIsInErrorDispatcher() == true)\
    { \
        /*std::cout << "DEBUG : applyEdAppFireItems fired" << std::endl; */\
        diagService_->applyEdAppFireItems(); \
        /*std::cout << "DEBUG : applyEdAppFireItems ended : DIAG_APPLY_ED_CALLBACK_FOR_FIRST_TIME fired" << std::endl; */\
        /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
        DIAG_APPLY_ED_CALLBACK_FOR_FIRST_TIME \
        /*std::cout << "DEBUG : DIAG_APPLY_ED_CALLBACK_FOR_FIRST_TIME ended" << std::endl; */\
    } \
    if (diagService_->getIsInGlobalErrorDispatcher() == true)\
    { \
        /*std::cout << "DEBUG : applyGlbEdAppFireItems fired" << std::endl; */\
        diagService_->applyGlbEdAppFireItems(); \
        /*std::cout << "DEBUG : applyGlbEdAppFireItems ended : DIAG_APPLY_GLB_ED_CALLBACK_FOR_FIRST_TIME fired" << std::endl; */\
        /* std::cout << "Calling Callback for GlobalErrorDispatcher" << std::endl; */ \
        DIAG_APPLY_GLB_ED_CALLBACK_FOR_FIRST_TIME \
        /*std::cout << "DEBUG : DIAG_APPLY_GLB_ED_CALLBACK_FOR_FIRST_TIME ended" << std::endl; */\
    } \
    if (diagService_->getIsInErrorManager() == true) \
    { \
        /*std::cout << "DEBUG : applyEmAppFireItems fired" << std::endl; */\
        diagService_->applyEmAppFireItems(); \
        /*std::cout << "DEBUG : applyEmAppFireItems ended : DIAG_APPLY_EM_CALLBACK_FOR_FIRST_TIME fired" << std::endl; */\
        /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
        DIAG_APPLY_EM_CALLBACK_FOR_FIRST_TIME \
        /*std::cout << "DEBUG : DIAG_APPLY_EM_CALLBACK_FOR_FIRST_TIME ended" << std::endl; */\
    } \
    if (diagService_->getIsInSentinelListener() == true) \
    { \
        /*std::cout << "DEBUG : applySentinelGrabberAppFireItems fired" << std::endl; */\
        diagService_->applySentinelGrabberAppFireItems(); \
        /*std::cout << "DEBUG : applySentinelGrabberAppFireItems ended : DIAG_APPLY_SENTINEL_CALLBACK_FOR_FIRST_TIME fired" << std::endl; */\
        /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
        DIAG_APPLY_SENTINEL_CALLBACK_FOR_FIRST_TIME\
        /*std::cout << "DEBUG : DIAG_APPLY_SENTINEL_CALLBACK_FOR_FIRST_TIME ended" << std::endl; */\
    } \
    if (diagService_->getIsInLogsReader() == true) \
    { \
        /*std::cout << "DEBUG : applyLogsReaderAppFireItems fired" << std::endl; */\
        diagService_->applyLogsReaderAppFireItems(); \
        /*std::cout << "DEBUG : applyLogsReaderAppFireItems ended : DIAG_APPLY_LOGS_READER_CALLBACK_FOR_FIRST_TIME fired" << std::endl; */\
        /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
        DIAG_APPLY_LOGS_READER_CALLBACK_FOR_FIRST_TIME\
        /*std::cout << "DEBUG : DIAG_APPLY_LOGS_READER_CALLBACK_FOR_FIRST_TIME ended" << std::endl; */\
    } \
    /*std::cout << "DEBUG : DIAG_EXEC_FSM_INIT_TRANS ended" << std::endl;*/ \
	diagService_->fiAreInitialized_ = true; \
	diagService_->flushFirstMessages();
	


#define DIAG_EXEC_FSM_STOP_TRANS \
    std::cout << "DEBUG : DIAG_EXEC_FSM_STOP_TRANS fired" << std::endl; \
    if (diagService_->getIsInUserProcess() == true) \
    { \
        diagService_->useDefaultAppender_ = false; \
        diagService_->useConsoleDump_ = false; \
        diagService_->useSentinel_ = false; \
        diagService_->sentinelUseRelayedEd_ = false; \
        diagService_->sentinelUseRelayedLvl1_ = false; \
        diagService_->useDiag_ = false; \
        diagService_->useGlbDiag_ = false; \
        diagService_->useLvl1_ = false; \
        diagService_->forcedDbLogs_ = false; \
        diagService_->forcedCsLogs_ = false; \
        diagService_->forcedCcLogs_ = false; \
        /* std::cout << "DEBUG : calling DIAG_APPLY_CALLBACK_FOR_FIRST_TIME" << std::endl;*/ \
        DIAG_APPLY_CALLBACK_FOR_FIRST_TIME \
    } \
    if (diagService_->getIsInErrorDispatcher() == true)\
    { \
        diagService_->useDefaultAppender_ = false; \
        diagService_->useSentinel_ = false; \
        diagService_->useDiag_ = false; \
        diagService_->useGlbDiag_ = false; \
		for (int i=0; i<DIAGMAX_CS_STREAMS; i++) \
		{ \
        	diagService_->csRoute_[i] = false; \
		} \
        diagService_->ccRoute_ = false; \
        diagService_->dbRoute_ = false; \
        diagService_->useFileLogging_ = false; \
        /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
        DIAG_APPLY_ED_CALLBACK_FOR_FIRST_TIME \
    } \
    if (diagService_->getIsInGlobalErrorDispatcher() == true)\
    { \
        diagService_->useDefaultAppender_ = false; \
        diagService_->useSentinel_ = false; \
        diagService_->useDiag_ = false; \
        diagService_->useGlbDiag_ = false; \
		for (int i=0; i<DIAGMAX_CS_STREAMS; i++) \
		{ \
        	diagService_->csRoute_[i] = false; \
		} \
        diagService_->ccRoute_ = false; \
        diagService_->dbRoute_ = false; \
        diagService_->useFileLogging_ = false; \
        /* std::cout << "Calling Callback for GlobalErrorDispatcher" << std::endl; */ \
        DIAG_APPLY_GLB_ED_CALLBACK_FOR_FIRST_TIME \
    } \
    if (diagService_->getIsInErrorManager() == true) \
    { \
        diagService_->useDefaultAppender_ = false; \
        diagService_->useConsoleDump_ = false; \
        diagService_->useSentinel_ = false; \
        diagService_->useDiag_ = false; \
        diagService_->useGlbDiag_ = false; \
        diagService_->useReconf_ = false; \
        /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
        DIAG_APPLY_EM_CALLBACK_FOR_FIRST_TIME \
    } \
    if (diagService_->getIsInSentinelListener() == true) \
    { \
        diagService_->useDefaultAppender_ = false; \
        diagService_->useConsoleDump_ = false; \
        diagService_->useSentinel_ = false; \
        diagService_->useGrabber_ = false; \
        diagService_->useDiag_ = false; \
        diagService_->useGlbDiag_ = false; \
        /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
        DIAG_APPLY_SENTINEL_CALLBACK_FOR_FIRST_TIME\
    } \
    if (diagService_->getIsInLogsReader() == true) \
    { \
        diagService_->useDefaultAppender_ = false; \
        diagService_->useSentinel_ = false; \
        diagService_->useDiag_ = false; \
        diagService_->useGlbDiag_ = false; \
        diagService_->ccRoute_ = false; \
        diagService_->dbRoute_ = false; \
        diagService_->useFileLogging_ = false; \
        /* std::cout << "Calling Callback for ErrorDispatcher" << std::endl; */ \
        DIAG_APPLY_LOGS_READER_CALLBACK_FOR_FIRST_TIME \
    } \
    /* std::cout << "DEBUG : DIAG_EXEC_FSM_STOP_TRANS ended" << std::endl; */


