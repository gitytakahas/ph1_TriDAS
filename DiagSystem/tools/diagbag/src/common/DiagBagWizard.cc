/*
   FileName : 		DiagBag.cc

   Content : 		DiagBag module

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
#include <time.h>

#include <diagbag/DiagBagWizard.h>







DiagBagWizard::DiagBagWizard(std::string l1name,
                            log4cplus::Logger& logger,
                            std::string classname,
                            unsigned long instance,
                            unsigned long lid,
                            xdaq::WebApplication * wa,
							std::string systemID,
							std::string subSystemID,
							std::stringstream * pixelsConsolePointer) throw (xdaq::exception::Exception)
                            : DiagBag (false, DIAGTRACE)//BSEMMOD     , sendMsgMutex(BSem::FULL),sendReconfRequestMutex(BSem::FULL)
							

{
//ChainSawManager csm;
//chainSawManager = &csm;
chainSawManager_ = new ChainSawManager();
nbrOfChainsawStreams_ = 1;

for (int i=0; i<DIAGMAX_CS_STREAMS; i++)
{
	csRoute_[i] = false;
	csFilter_[i] = "";

}

    diagName_= "LocalErrorDispatcher";
    useDiag_=false;
    diagInstance_="0";
    diagIP_="127.0.0.1";
    diagPort_="1972";
    diagLID_="31";

    diagGlbName_= "GlobalErrorDispatcher";
    useGlbDiag_=false;
    diagGlbInstance_="0";
    diagGlbIP_="127.0.0.1";
    diagGlbPort_="1972";
    diagGlbLID_="31";

    lvl1Name_=l1name;
    useLvl1_=false;
    lvl1Instance_="0";
    lvl1IP_="127.0.0.1";
    lvl1Port_="1972";
    lvl1LID_="32";

    diagNameStatus_ = "unchecked";
    useDiagStatus_ =  "unchecked";
    diagInstanceStatus_ =  "unchecked";
    diagIPStatus_ =  "unchecked";
    diagPortStatus_ =  "unchecked";
    diagLIDStatus_ =  "unchecked";

    diagGlbNameStatus_ = "unchecked";
    useGlbDiagStatus_ =  "unchecked";
    diagGlbInstanceStatus_ =  "unchecked";
    diagGlbIPStatus_ =  "unchecked";
    diagGlbPortStatus_ =  "unchecked";
    diagGlbLIDStatus_ =  "unchecked";

    lvl1NameStatus_ =  "unchecked";
    useLvl1Status_ =  "unchecked";
    lvl1InstanceStatus_ =  "unchecked";
    lvl1IPStatus_ =  "unchecked";
    lvl1PortStatus_ =  "unchecked";
    lvl1LIDStatus_ =  "unchecked";

    edUsable_=false;
    edGlbUsable_=false;

    emUsable_=false;

    forcedDbLogs_ = false;
    forcedCsLogs_ = false;
    forcedCcLogs_ = false;

    dbLogLevel_=DIAGTRACE;
for (int csStreamNum = 0; csStreamNum < DIAGMAX_CS_STREAMS ; csStreamNum++)
{
    csLogLevel_[csStreamNum]=DIAGTRACE;
    socketXMLExists_[csStreamNum] = false;
    oldFirstTime_[csStreamNum] = true;

}
    ccLogLevel_=DIAGTRACE;
    gLogLevel_=DIAGTRACE;

    oldDiagIP_="";
    oldDiagPort_="";
    oldDiagName_="";
    oldDiagInstance_="";
    oldDiagLID_="";
    socketSoapExists_ = false;


    callingAppLogger_ = &logger;
    callingAppClassName_ = classname;
    callingAppInstance_ = instance;
    callingAppLID_ = lid;

	DiagBag::setConsoleDumpEnabled(false);
	DiagBag::setConsoleDumpLevel(DIAGTRACE);
    wa_ = wa;
    useSentinel_ = false;
    sentinelContextName_ = "UpperLayerErrorsPool";
    sentinelUseRelayedEd_ = false;
    sentinelLogLevel_ = DIAGTRACE;
    edLogLevel_ = DIAGTRACE;
    edGlbLogLevel_ = DIAGTRACE;

    sentinelUseRelayedLvl1_ = false;

    std::string relayedEdStatus_ = "unchecked";
    std::string relayedLvl1Status_ = "unchecked";

    relayedEdUsable_ = false;
    relayedEmUsable_ = false;
    sentinelUsable_ = false;

    useDefaultAppender_ = true;
    defaultAppenderStatus_ = "checked";
    defaultAppenderUsable_ = true;

    useConsoleDump_ = false;
    std::string consoleDumpStatus_ = "unchecked";
    consoleDumpUsable_ = false;

    isInSentinelListener_ = false;
    contextHasSentinel_ = false;
    sentinelUsableStatus_ = "unchecked";
    fileLoggingStatus_ = "unchecked";

    isEdRoutingToSentinel_ = false;
    areSentinelScanned_ = false;
    isSentinelContextPushed_ = false;
    
    /* Used to know if this is the first creation of an XML socket for chainsaw logging */

    isInErrorDispatcher_ = false;
    isInGlobalErrorDispatcher_ = false;
    isInUserProcess_ = true;
    isInSentinelListener_ = false;
    isInErrorManager_ = false;	

    grabberContextName_ =  "NONE";
    oldGrabberContextName_ =  "NONE";
    grabberUsable_ = false;
    grabberIsAttached_ = false;
	
    /* Initialise FireItems default values */
	feUseDiagSystem_ = "oldway";
	feLogLevel_ = DIAGWARN;
	feDiagSystemSettings_ = "";


//	diagSystemStatus_ = "unchecked";
	
    xdata::Boolean feUseDefaultAppender_ = true;
    feDefaultAppenderLogLevel_ = DIAGTRACE;
    feDuplicateMsgToLocalConsole_ = false;

    feUseSentinel_ = false;
    feSentinelAppenderLogLevel_ = DIAGTRACE;
    fePushErrorsToContext_ = "TestErrorsPool";
    feUseSentinelRelayedEm_ = false;

    feUseErrorDispatcher_ = false;
    feErrorDispatcherLogLevel_ = DIAGTRACE;
    feErrorDispatcherName_ = "LocalErrorDispatcher";
    feErrorDispatcherInstance_ = "0";
    feErrorDispatcherLID_ = "30";
    feErrorDispatcherIP_ = "127.0.0.1";
    feErrorDispatcherPort_ = "1972";

    feUseErrorManager_ = false;
    feErrorManagerName_ = "DiagLevelOneModule";
    feErrorManagerInstance_ = "0";
    feErrorManagerLID_ = "32";
    feErrorManagerIP_ = "127.0.0.1";
    feErrorManagerPort_ = "1972";

    feForceDbLogging_ = false;
    feForcedDbLoggingLevel_ = DIAGTRACE;
    feForceChainsawLogging_ = false;
    feForcedChainsawLoggingLevel_ = DIAGTRACE;
    feForceConsoleLogging_ = false;
    feForcedConsoleLoggingLevel_ = DIAGTRACE;

    /* ERROR DISPATCHER BLOCK */
    feUseChainsaw_ = false;
    feChainsawServerHost_= "Chainsaw host";
    feChainsawServerPort_ = "1972";
    feChainsawReceiverName_ = "socket name";
    feChainsawLogLevel_ = DIAGTRACE;

    feUseConsoleLogging_ = false;
    feConsoleLogLevel_ = DIAGTRACE;

    feUseDbLogging_ = false;
    feDbLogLevel_ = DIAGTRACE;

    feUseFileLogging_ = false;
    feFileLogLevel_ = DIAGTRACE;
    feFilePathAndName_ = "/tmp/errorsLogFile.txt";

    /* LEVEL ONE BLOCK */
    feCloseReconfLoop_ = false;
    feReconfProcessName_ = "Reconfigurator process";
    feReconfProcessInstance_ = "0";
    feReconfProcessLID_ = "42";
    feReconfProcessIP_ = "cmstkint02.cern.ch";
    feReconfProcessPort_ = "13000";

    /* SENTINEL GRABBER BLOCK */
    feListenSentinel_ = false;
    feListenContext_ = "TestErrorsPool";

    feUseGlbErrorDispatcher_ = false;
    feGlbErrorDispatcherLogLevel_ = DIAGTRACE;
    feGlbErrorDispatcherName_ = "GlobalErrorDispatcher";
    feGlbErrorDispatcherInstance_ = "0";
    feGlbErrorDispatcherLID_ = "30";
    feGlbErrorDispatcherIP_ = "127.0.0.1";
    feGlbErrorDispatcherPort_ = "1972";

	feLogFilePath_ = "/tmp/";

	//Initialise run runmber when unassigned
	feRunFromRcms_ = 0;
	runFromRcms_ = 0;

	#ifdef USE_XRELAY
    	isWaitingForLclLogAck_ = false;
	    missedLclEmissions_ = 0;
    	isWaitingForGlbLogAck_ = false;
	    missedGlbEmissions_ = 0;
	#endif

	std::string logLevel_ = DIAGWARN;
	useDiagSystem_ = "oldway";
	diagSystemSettings_ = "";

//	activateDiagSystemForFirstTime_ = true;

systemID_ = systemID;
subSystemID_ = subSystemID;

logFileNameHasChanged_ = true;
pixelsLogFileNameHasChanged_ = true;
log4jLogFileNameHasChanged_ = true;


dbFileNameHasChanged_ = true;

feDbFilePathAndName_ = "/tmp/errorsDbSwapFile.txt";

#ifdef XDAQ373
	selfAppDescriptor_ = wa_->getApplicationContext()->getDefaultZone()->getApplicationDescriptor(classname, instance);
#else
	selfAppDescriptor_ = wa_->getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(classname, instance);
#endif

sendMsgMutex = new toolbox::BSem(toolbox::BSem::FULL);
sendReconfRequestMutex = new toolbox::BSem(toolbox::BSem::FULL);
oldLogFileName_ = "oldLogFileName_";
oldPixelsLogFileName_ = "oldPixelsLogFileName_";
oldLog4jLogFileName_ = "oldLog4jLogFileName_";



fiAreInitialized_ = false;
/*
std::cout << "Launching timer for unprocessed logs" << std::endl;
std::stringstream timerName;
timerName << wa_->getApplicationDescriptor()->getContextDescriptor()->getURL() << ":";
timerName << wa_->getApplicationDescriptor()->getClassName() << ":" << wa_->getApplicationDescriptor()->getLocalId() << ":" << wa_->getApplicationDescriptor()->getInstance();
timerName << "DiagBagTimer";
timerName_ = timerName.str();
toolbox::task::Timer * timer_ = toolbox::task::getTimerFactory()->createTimer(timerName.str());
toolbox::TimeInterval interval(PUSH_INITIAL_LOGS_DELAY,0);
toolbox::TimeVal start;
start = toolbox::TimeVal::gettimeofday() + interval;
//timer_->schedule( this, start,  0, "" );
timer_->scheduleAtFixedRate( start, this, interval, wa_->getApplicationDescriptor()->getContextDescriptor(), "" );
*/

isProcessingNestedShortClog_ = false;
isProcessingNestedLongClog_ = false;

pixelsConsolePointer_ = pixelsConsolePointer;
}






































DiagBagWizard::DiagBagWizard(std::string l1name,
                            log4cplus::Logger& logger,
                            std::string classname,
                            unsigned long instance,
                            unsigned long lid,
                            xdaq::WebApplication * wa,
							std::string systemID,
							std::string subSystemID) throw (xdaq::exception::Exception)
                            : DiagBag (false, DIAGTRACE)//BSEMMOD     , sendMsgMutex(BSem::FULL),sendReconfRequestMutex(BSem::FULL)
							

{
//ChainSawManager csm;
//chainSawManager = &csm;
chainSawManager_ = new ChainSawManager();
nbrOfChainsawStreams_ = 1;

for (int i=0; i<DIAGMAX_CS_STREAMS; i++)
{
	csRoute_[i] = false;
	csFilter_[i] = "";

}

    diagName_= "LocalErrorDispatcher";
    useDiag_=false;
    diagInstance_="0";
    diagIP_="127.0.0.1";
    diagPort_="1972";
    diagLID_="31";

    diagGlbName_= "GlobalErrorDispatcher";
    useGlbDiag_=false;
    diagGlbInstance_="0";
    diagGlbIP_="127.0.0.1";
    diagGlbPort_="1972";
    diagGlbLID_="31";

    lvl1Name_=l1name;
    useLvl1_=false;
    lvl1Instance_="0";
    lvl1IP_="127.0.0.1";
    lvl1Port_="1972";
    lvl1LID_="32";

    diagNameStatus_ = "unchecked";
    useDiagStatus_ =  "unchecked";
    diagInstanceStatus_ =  "unchecked";
    diagIPStatus_ =  "unchecked";
    diagPortStatus_ =  "unchecked";
    diagLIDStatus_ =  "unchecked";

    diagGlbNameStatus_ = "unchecked";
    useGlbDiagStatus_ =  "unchecked";
    diagGlbInstanceStatus_ =  "unchecked";
    diagGlbIPStatus_ =  "unchecked";
    diagGlbPortStatus_ =  "unchecked";
    diagGlbLIDStatus_ =  "unchecked";

    lvl1NameStatus_ =  "unchecked";
    useLvl1Status_ =  "unchecked";
    lvl1InstanceStatus_ =  "unchecked";
    lvl1IPStatus_ =  "unchecked";
    lvl1PortStatus_ =  "unchecked";
    lvl1LIDStatus_ =  "unchecked";

    edUsable_=false;
    edGlbUsable_=false;

    emUsable_=false;

    forcedDbLogs_ = false;
    forcedCsLogs_ = false;
    forcedCcLogs_ = false;

    dbLogLevel_=DIAGTRACE;
for (int csStreamNum = 0; csStreamNum < DIAGMAX_CS_STREAMS ; csStreamNum++)
{
    csLogLevel_[csStreamNum]=DIAGTRACE;
    socketXMLExists_[csStreamNum] = false;
    oldFirstTime_[csStreamNum] = true;

}
    ccLogLevel_=DIAGTRACE;
    gLogLevel_=DIAGTRACE;

    oldDiagIP_="";
    oldDiagPort_="";
    oldDiagName_="";
    oldDiagInstance_="";
    oldDiagLID_="";
    socketSoapExists_ = false;


    callingAppLogger_ = &logger;
    callingAppClassName_ = classname;
    callingAppInstance_ = instance;
    callingAppLID_ = lid;

	DiagBag::setConsoleDumpEnabled(false);
	DiagBag::setConsoleDumpLevel(DIAGTRACE);
    wa_ = wa;
    useSentinel_ = false;
    sentinelContextName_ = "UpperLayerErrorsPool";
    sentinelUseRelayedEd_ = false;
    sentinelLogLevel_ = DIAGTRACE;
    edLogLevel_ = DIAGTRACE;
    edGlbLogLevel_ = DIAGTRACE;

    sentinelUseRelayedLvl1_ = false;

    std::string relayedEdStatus_ = "unchecked";
    std::string relayedLvl1Status_ = "unchecked";

    relayedEdUsable_ = false;
    relayedEmUsable_ = false;
    sentinelUsable_ = false;

    useDefaultAppender_ = true;
    defaultAppenderStatus_ = "checked";
    defaultAppenderUsable_ = true;

    useConsoleDump_ = false;
    std::string consoleDumpStatus_ = "unchecked";
    consoleDumpUsable_ = false;

    isInSentinelListener_ = false;
    contextHasSentinel_ = false;
    sentinelUsableStatus_ = "unchecked";
    fileLoggingStatus_ = "unchecked";

    isEdRoutingToSentinel_ = false;
    areSentinelScanned_ = false;
    isSentinelContextPushed_ = false;
    
    /* Used to know if this is the first creation of an XML socket for chainsaw logging */

    isInErrorDispatcher_ = false;
    isInGlobalErrorDispatcher_ = false;
    isInUserProcess_ = true;
    isInSentinelListener_ = false;
    isInErrorManager_ = false;	

    grabberContextName_ =  "NONE";
    oldGrabberContextName_ =  "NONE";
    grabberUsable_ = false;
    grabberIsAttached_ = false;
	
    /* Initialise FireItems default values */
	feUseDiagSystem_ = "oldway";
	feLogLevel_ = DIAGWARN;
	feDiagSystemSettings_ = "";


//	diagSystemStatus_ = "unchecked";
	
    xdata::Boolean feUseDefaultAppender_ = true;
    feDefaultAppenderLogLevel_ = DIAGTRACE;
    feDuplicateMsgToLocalConsole_ = false;

    feUseSentinel_ = false;
    feSentinelAppenderLogLevel_ = DIAGTRACE;
    fePushErrorsToContext_ = "TestErrorsPool";
    feUseSentinelRelayedEm_ = false;

    feUseErrorDispatcher_ = false;
    feErrorDispatcherLogLevel_ = DIAGTRACE;
    feErrorDispatcherName_ = "LocalErrorDispatcher";
    feErrorDispatcherInstance_ = "0";
    feErrorDispatcherLID_ = "30";
    feErrorDispatcherIP_ = "127.0.0.1";
    feErrorDispatcherPort_ = "1972";

    feUseErrorManager_ = false;
    feErrorManagerName_ = "DiagLevelOneModule";
    feErrorManagerInstance_ = "0";
    feErrorManagerLID_ = "32";
    feErrorManagerIP_ = "127.0.0.1";
    feErrorManagerPort_ = "1972";

    feForceDbLogging_ = false;
    feForcedDbLoggingLevel_ = DIAGTRACE;
    feForceChainsawLogging_ = false;
    feForcedChainsawLoggingLevel_ = DIAGTRACE;
    feForceConsoleLogging_ = false;
    feForcedConsoleLoggingLevel_ = DIAGTRACE;

    /* ERROR DISPATCHER BLOCK */
    feUseChainsaw_ = false;
    feChainsawServerHost_= "Chainsaw host";
    feChainsawServerPort_ = "1972";
    feChainsawReceiverName_ = "socket name";
    feChainsawLogLevel_ = DIAGTRACE;

    feUseConsoleLogging_ = false;
    feConsoleLogLevel_ = DIAGTRACE;

    feUseDbLogging_ = false;
    feDbLogLevel_ = DIAGTRACE;

    feUseFileLogging_ = false;
    feFileLogLevel_ = DIAGTRACE;
    feFilePathAndName_ = "/tmp/errorsLogFile.txt";

    /* LEVEL ONE BLOCK */
    feCloseReconfLoop_ = false;
    feReconfProcessName_ = "Reconfigurator process";
    feReconfProcessInstance_ = "0";
    feReconfProcessLID_ = "42";
    feReconfProcessIP_ = "cmstkint02.cern.ch";
    feReconfProcessPort_ = "13000";

    /* SENTINEL GRABBER BLOCK */
    feListenSentinel_ = false;
    feListenContext_ = "TestErrorsPool";

    feUseGlbErrorDispatcher_ = false;
    feGlbErrorDispatcherLogLevel_ = DIAGTRACE;
    feGlbErrorDispatcherName_ = "GlobalErrorDispatcher";
    feGlbErrorDispatcherInstance_ = "0";
    feGlbErrorDispatcherLID_ = "30";
    feGlbErrorDispatcherIP_ = "127.0.0.1";
    feGlbErrorDispatcherPort_ = "1972";

	feLogFilePath_ = "/tmp/";

	//Initialise run runmber when unassigned
	feRunFromRcms_ = 0;
	runFromRcms_ = 0;

	#ifdef USE_XRELAY
    	isWaitingForLclLogAck_ = false;
	    missedLclEmissions_ = 0;
    	isWaitingForGlbLogAck_ = false;
	    missedGlbEmissions_ = 0;
	#endif

	std::string logLevel_ = DIAGWARN;
	useDiagSystem_ = "oldway";
	diagSystemSettings_ = "";

//	activateDiagSystemForFirstTime_ = true;

systemID_ = systemID;
subSystemID_ = subSystemID;

logFileNameHasChanged_ = true;
pixelsLogFileNameHasChanged_ = true;
log4jLogFileNameHasChanged_ = true;


dbFileNameHasChanged_ = true;

feDbFilePathAndName_ = "/tmp/errorsDbSwapFile.txt";

#ifdef XDAQ373
	selfAppDescriptor_ = wa_->getApplicationContext()->getDefaultZone()->getApplicationDescriptor(classname, instance);
#else
	selfAppDescriptor_ = wa_->getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(classname, instance);
#endif

sendMsgMutex = new toolbox::BSem(toolbox::BSem::FULL);
sendReconfRequestMutex = new toolbox::BSem(toolbox::BSem::FULL);
oldLogFileName_ = "oldLogFileName_";
oldPixelsLogFileName_ = "oldPixelsLogFileName_";
oldLog4jLogFileName_ = "oldLog4jLogFileName_";



fiAreInitialized_ = false;
/*
std::cout << "Launching timer for unprocessed logs" << std::endl;
std::stringstream timerName;
timerName << wa_->getApplicationDescriptor()->getContextDescriptor()->getURL() << ":";
timerName << wa_->getApplicationDescriptor()->getClassName() << ":" << wa_->getApplicationDescriptor()->getLocalId() << ":" << wa_->getApplicationDescriptor()->getInstance();
timerName << "DiagBagTimer";
timerName_ = timerName.str();
toolbox::task::Timer * timer_ = toolbox::task::getTimerFactory()->createTimer(timerName.str());
toolbox::TimeInterval interval(PUSH_INITIAL_LOGS_DELAY,0);
toolbox::TimeVal start;
start = toolbox::TimeVal::gettimeofday() + interval;
//timer_->schedule( this, start,  0, "" );
timer_->scheduleAtFixedRate( start, this, interval, wa_->getApplicationDescriptor()->getContextDescriptor(), "" );
*/

isProcessingNestedShortClog_ = false;
isProcessingNestedLongClog_ = false;

pixelsConsolePointer_ = NULL;
}


DiagBagWizard::DiagBagWizard(std::string l1name,
                            log4cplus::Logger& logger,
                            std::string classname,
                            unsigned long instance,
                            unsigned long lid,
                            xdaq::WebApplication * wa) throw (xdaq::exception::Exception)
                            : DiagBag (false, DIAGTRACE)//BSEMMOD     , sendMsgMutex(BSem::FULL), sendReconfRequestMutex(BSem::FULL)
{

//ChainSawManager csm;
//chainSawManager = &csm;
chainSawManager_ = new ChainSawManager();
nbrOfChainsawStreams_ = 1;
for (int i=0; i<DIAGMAX_CS_STREAMS; i++)
{
	csRoute_[i] = false;
	csFilter_[i] = "";
}


    diagName_= "LocalErrorDispatcher";
    useDiag_=false;
    diagInstance_="0";
    diagIP_="127.0.0.1";
    diagPort_="1972";
    diagLID_="31";

    diagGlbName_= "GlobalErrorDispatcher";
    useGlbDiag_=false;
    diagGlbInstance_="0";
    diagGlbIP_="127.0.0.1";
    diagGlbPort_="1972";
    diagGlbLID_="31";

    lvl1Name_=l1name;
    useLvl1_=false;
    lvl1Instance_="0";
    lvl1IP_="127.0.0.1";
    lvl1Port_="1972";
    lvl1LID_="32";

    diagNameStatus_ = "unchecked";
    useDiagStatus_ =  "unchecked";
    diagInstanceStatus_ =  "unchecked";
    diagIPStatus_ =  "unchecked";
    diagPortStatus_ =  "unchecked";
    diagLIDStatus_ =  "unchecked";

    diagGlbNameStatus_ = "unchecked";
    useGlbDiagStatus_ =  "unchecked";
    diagGlbInstanceStatus_ =  "unchecked";
    diagGlbIPStatus_ =  "unchecked";
    diagGlbPortStatus_ =  "unchecked";
    diagGlbLIDStatus_ =  "unchecked";

    lvl1NameStatus_ =  "unchecked";
    useLvl1Status_ =  "unchecked";
    lvl1InstanceStatus_ =  "unchecked";
    lvl1IPStatus_ =  "unchecked";
    lvl1PortStatus_ =  "unchecked";
    lvl1LIDStatus_ =  "unchecked";

    edUsable_=false;
    edGlbUsable_=false;

    emUsable_=false;

    forcedDbLogs_ = false;
    forcedCsLogs_ = false;
    forcedCcLogs_ = false;

    dbLogLevel_=DIAGTRACE;
for (int csStreamNum = 0; csStreamNum < DIAGMAX_CS_STREAMS ; csStreamNum++)
{
    csLogLevel_[csStreamNum]=DIAGTRACE;
    socketXMLExists_[csStreamNum] = false;
    oldFirstTime_[csStreamNum] = true;

}

//    csLogLevel_[0]=DIAGTRACE;
    ccLogLevel_=DIAGTRACE;
    gLogLevel_=DIAGTRACE;

    oldDiagIP_="";
    oldDiagPort_="";
    oldDiagName_="";
    oldDiagInstance_="";
    oldDiagLID_="";
    socketSoapExists_ = false;

//    socketXMLExists_[0] = false;

    callingAppLogger_ = &logger;
    callingAppClassName_ = classname;
    callingAppInstance_ = instance;
    callingAppLID_ = lid;

	DiagBag::setConsoleDumpEnabled(false);
	DiagBag::setConsoleDumpLevel(DIAGTRACE);
    wa_ = wa;
    useSentinel_ = false;
    sentinelContextName_ = "UpperLayerErrorsPool";
    sentinelUseRelayedEd_ = false;
    sentinelLogLevel_ = DIAGTRACE;
    edLogLevel_ = DIAGTRACE;
    edGlbLogLevel_ = DIAGTRACE;









    sentinelUseRelayedLvl1_ = false;

    std::string relayedEdStatus_ = "unchecked";
    std::string relayedLvl1Status_ = "unchecked";

    relayedEdUsable_ = false;
    relayedEmUsable_ = false;
    sentinelUsable_ = false;

    useDefaultAppender_ = true;
    defaultAppenderStatus_ = "checked";
    defaultAppenderUsable_ = true;

    useConsoleDump_ = false;
    std::string consoleDumpStatus_ = "unchecked";
    consoleDumpUsable_ = false;

    isInSentinelListener_ = false;
    contextHasSentinel_ = false;
    sentinelUsableStatus_ = "unchecked";
    fileLoggingStatus_ = "unchecked";

    isEdRoutingToSentinel_ = false;
    areSentinelScanned_ = false;
    isSentinelContextPushed_ = false;
    
    /* Used to know if this is the first creation of an XML socket for chainsaw logging */
//    oldFirstTime_[0] = true;

    isInErrorDispatcher_ = false;
    isInGlobalErrorDispatcher_ = false;
    isInUserProcess_ = true;
    isInSentinelListener_ = false;
    isInErrorManager_ = false;	

    grabberContextName_ =  "NONE";
    oldGrabberContextName_ =  "NONE";
    grabberUsable_ = false;
    grabberIsAttached_ = false;
	
    /* Initialise FireItems default values */
	feUseDiagSystem_ = "oldway";
	feLogLevel_ = DIAGWARN;
	feDiagSystemSettings_ = "";


//	diagSystemStatus_ = "unchecked";
	
    xdata::Boolean feUseDefaultAppender_ = true;
    feDefaultAppenderLogLevel_ = DIAGTRACE;
    feDuplicateMsgToLocalConsole_ = false;

    feUseSentinel_ = false;
    feSentinelAppenderLogLevel_ = DIAGTRACE;
    fePushErrorsToContext_ = "TestErrorsPool";
    feUseSentinelRelayedEm_ = false;

    feUseErrorDispatcher_ = false;
    feErrorDispatcherLogLevel_ = DIAGTRACE;
    feErrorDispatcherName_ = "LocalErrorDispatcher";
    feErrorDispatcherInstance_ = "0";
    feErrorDispatcherLID_ = "30";
    feErrorDispatcherIP_ = "127.0.0.1";
    feErrorDispatcherPort_ = "1972";

    feUseErrorManager_ = false;
    feErrorManagerName_ = "DiagLevelOneModule";
    feErrorManagerInstance_ = "0";
    feErrorManagerLID_ = "32";
    feErrorManagerIP_ = "127.0.0.1";
    feErrorManagerPort_ = "1972";

    feForceDbLogging_ = false;
    feForcedDbLoggingLevel_ = DIAGTRACE;
    feForceChainsawLogging_ = false;
    feForcedChainsawLoggingLevel_ = DIAGTRACE;
    feForceConsoleLogging_ = false;
    feForcedConsoleLoggingLevel_ = DIAGTRACE;

    /* ERROR DISPATCHER BLOCK */
    feUseChainsaw_ = false;
    feChainsawServerHost_= "Chainsaw host";
    feChainsawServerPort_ = "1972";
    feChainsawReceiverName_ = "socket name";
    feChainsawLogLevel_ = DIAGTRACE;

    feUseConsoleLogging_ = false;
    feConsoleLogLevel_ = DIAGTRACE;

    feUseDbLogging_ = false;
    feDbLogLevel_ = DIAGTRACE;

    feUseFileLogging_ = false;
    feFileLogLevel_ = DIAGTRACE;
    feFilePathAndName_ = "/tmp/errorsLogFile.txt";

    /* LEVEL ONE BLOCK */
    feCloseReconfLoop_ = false;
    feReconfProcessName_ = "Reconfigurator process";
    feReconfProcessInstance_ = "0";
    feReconfProcessLID_ = "42";
    feReconfProcessIP_ = "cmstkint02.cern.ch";
    feReconfProcessPort_ = "13000";

    /* SENTINEL GRABBER BLOCK */
    feListenSentinel_ = false;
    feListenContext_ = "TestErrorsPool";

    feUseGlbErrorDispatcher_ = false;
    feGlbErrorDispatcherLogLevel_ = DIAGTRACE;
    feGlbErrorDispatcherName_ = "GlobalErrorDispatcher";
    feGlbErrorDispatcherInstance_ = "0";
    feGlbErrorDispatcherLID_ = "30";
    feGlbErrorDispatcherIP_ = "127.0.0.1";
    feGlbErrorDispatcherPort_ = "1972";

	feLogFilePath_ = "/tmp/";

	//Initialise run runmber when unassigned
	feRunFromRcms_ = 0;
	runFromRcms_ = 0;

	#ifdef USE_XRELAY
    	isWaitingForLclLogAck_ = false;
	    missedLclEmissions_ = 0;
    	isWaitingForGlbLogAck_ = false;
	    missedGlbEmissions_ = 0;
	#endif

	std::string logLevel_ = DIAGWARN;
	useDiagSystem_ = "oldway";
	diagSystemSettings_ = "";

//	activateDiagSystemForFirstTime_ = true;
logFileNameHasChanged_ = true;
pixelsLogFileNameHasChanged_ = true;
log4jLogFileNameHasChanged_ = true;


dbFileNameHasChanged_ = true;

feDbFilePathAndName_ = "/tmp/errorsDbSwapFile.txt";

#ifdef XDAQ373
	selfAppDescriptor_ = wa_->getApplicationContext()->getDefaultZone()->getApplicationDescriptor(classname, instance);
#else
	selfAppDescriptor_ = wa_->getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(classname, instance);
#endif

sendMsgMutex = new toolbox::BSem(toolbox::BSem::FULL);
sendReconfRequestMutex = new toolbox::BSem(toolbox::BSem::FULL);
oldLogFileName_ = "oldLogFileName_";
oldPixelsLogFileName_ = "oldPixelsLogFileName_";
oldLog4jLogFileName_ = "oldLog4jLogFileName_";

fiAreInitialized_ = false;

/*
std::cout << "Launching timer for unprocessed logs" << std::endl;
std::stringstream timerName;
timerName << wa_->getApplicationDescriptor()->getContextDescriptor()->getURL() << ":";
timerName << wa_->getApplicationDescriptor()->getClassName() << ":" << wa_->getApplicationDescriptor()->getLocalId() << ":" << wa_->getApplicationDescriptor()->getInstance();
timerName << "DiagBagTimer";
timerName_ = timerName.str();
toolbox::task::Timer * timer_ = toolbox::task::getTimerFactory()->createTimer(timerName.str());
toolbox::TimeInterval interval(PUSH_INITIAL_LOGS_DELAY,0);
toolbox::TimeVal start;
start = toolbox::TimeVal::gettimeofday() + interval;
//timer_->schedule( this, start,  0, "" );
timer_->scheduleAtFixedRate( start, this, interval, wa_->getApplicationDescriptor()->getContextDescriptor(), "" );
*/
isProcessingNestedShortClog_ = false;
isProcessingNestedLongClog_ = false;

pixelsConsolePointer_ = NULL;
}




DiagBagWizard::DiagBagWizard(std::string l1name) throw (xdaq::exception::Exception)//BSEMMOD: sendMsgMutex(BSem::FULL), sendReconfRequestMutex(BSem::FULL)
{
//ChainSawManager csm;
chainSawManager_ = new ChainSawManager();
nbrOfChainsawStreams_ = 1;
for (int i=0; i<DIAGMAX_CS_STREAMS; i++)
{
	csRoute_[i] = false;
	csFilter_[i] = "";
}


    diagName_= "LocalErrorDispatcher";
    useDiag_=false;
    diagInstance_="0";
    diagIP_="127.0.0.1";
    diagPort_="1972";
    diagLID_="30";

    diagGlbName_= "GlobalErrorDispatcher";
    useGlbDiag_=false;
    diagGlbInstance_="0";
    diagGlbIP_="127.0.0.1";
    diagGlbPort_="1972";
    diagGlbLID_="31";

    lvl1Name_=l1name;
    useLvl1_=false;
    lvl1Instance_="0";
    lvl1IP_="127.0.0.1";
    lvl1Port_="1972";
    lvl1LID_="32";

    diagNameStatus_ = "unchecked";
    useDiagStatus_ =  "unchecked";
    diagInstanceStatus_ =  "unchecked";
    diagIPStatus_ =  "unchecked";
    diagPortStatus_ =  "unchecked";
    diagLIDStatus_ =  "unchecked";

    diagGlbNameStatus_ = "unchecked";
    useGlbDiagStatus_ =  "unchecked";
    diagGlbInstanceStatus_ =  "unchecked";
    diagGlbIPStatus_ =  "unchecked";
    diagGlbPortStatus_ =  "unchecked";
    diagGlbLIDStatus_ =  "unchecked";

    lvl1NameStatus_ =  "unchecked";
    useLvl1Status_ =  "unchecked";
    lvl1InstanceStatus_ =  "unchecked";
    lvl1IPStatus_ =  "unchecked";
    lvl1PortStatus_ =  "unchecked";
    lvl1LIDStatus_ =  "unchecked";

    edUsable_=false;
    edGlbUsable_=false;
    
    emUsable_=false;

    forcedDbLogs_ = false;
    forcedCsLogs_ = false;
    forcedCcLogs_ = false;

    dbLogLevel_=DIAGTRACE;
for (int csStreamNum = 0; csStreamNum < DIAGMAX_CS_STREAMS ; csStreamNum++)
{
    csLogLevel_[csStreamNum]=DIAGTRACE;
    socketXMLExists_[csStreamNum] = false;
    oldFirstTime_[csStreamNum] = true;

}

//    csLogLevel_[0]=DIAGTRACE;
    ccLogLevel_=DIAGTRACE;
    gLogLevel_=DIAGTRACE;

    oldDiagIP_="";
    oldDiagPort_="";
    oldDiagName_="";
    oldDiagInstance_="";
    oldDiagLID_="";
    socketSoapExists_ = false;

//    socketXMLExists_[0] = false;
	DiagBag::setConsoleDumpEnabled(false);
	DiagBag::setConsoleDumpLevel(DIAGTRACE);

    useSentinel_ = false;
    sentinelContextName_ = "TestErrorsPool";
    sentinelUseRelayedEd_ = false;
    sentinelLogLevel_ = DIAGTRACE;
    edLogLevel_ = DIAGTRACE;
    edGlbLogLevel_ = DIAGTRACE;

    sentinelUseRelayedLvl1_ = false;
    sentinelUsable_ = false;

    useDefaultAppender_ = true;
	defaultAppenderStatus_ = "checked";
    defaultAppenderUsable_ = true;

    useConsoleDump_ = false;
    std::string consoleDumpStatus_ = "unchecked";
    consoleDumpUsable_ = false;

    isInSentinelListener_ = false;
    contextHasSentinel_ = false;
    sentinelUsableStatus_ = "unchecked";

    fileLoggingStatus_ = "unchecked";

    isEdRoutingToSentinel_ = false;
    areSentinelScanned_ = false;
    isSentinelContextPushed_ = false;


    /* Used to know if this is the first creation of an XML socket for chainsaw logging */
//    oldFirstTime_[0] = true;
    isInErrorDispatcher_ = false;
    isInGlobalErrorDispatcher_ = false;
    isInUserProcess_ = true;
    isInSentinelListener_ = false;
    isInErrorManager_ = false;

    //Initialise FireItems default values
	feUseDiagSystem_ = "oldway";
	feLogLevel_ = DIAGWARN;
	feDiagSystemSettings_ = "";
//	diagSystemStatus_ = "unchecked";


    xdata::Boolean feUseDefaultAppender_ = true;
    feDefaultAppenderLogLevel_ = DIAGTRACE;
    feDuplicateMsgToLocalConsole_ = false;

    feUseSentinel_ = false;
    feSentinelAppenderLogLevel_ = DIAGTRACE;
    fePushErrorsToContext_ = "TestErrorsPool";
    feUseSentinelRelayedEm_ = false;

    feUseErrorDispatcher_ = false;
    feErrorDispatcherLogLevel_ = DIAGTRACE;
    feErrorDispatcherName_ = "LocalErrorDispatcher";
    feErrorDispatcherInstance_ = "0";
    feErrorDispatcherLID_ = "30";
    feErrorDispatcherIP_ = "127.0.0.1";
    feErrorDispatcherPort_ = "1972";

    feUseErrorManager_ = false;
    feErrorManagerName_ = "DiagLevelOneModule";
    feErrorManagerInstance_ = "0";
    feErrorManagerLID_ = "32";
    feErrorManagerIP_ = "127.0.0.1";
    feErrorManagerPort_ = "1972";

    feForceDbLogging_ = false;
    feForcedDbLoggingLevel_ = DIAGTRACE;
    feForceChainsawLogging_ = false;
    feForcedChainsawLoggingLevel_ = DIAGTRACE;
    feForceConsoleLogging_ = false;
    feForcedConsoleLoggingLevel_ = DIAGTRACE;

    /* ERROR DISPATCHER BLOCK */
    feUseChainsaw_ = false;
    feChainsawServerHost_= "Chainsaw host";
    feChainsawServerPort_ = "1972";
    feChainsawReceiverName_ = "socket name";
    feChainsawLogLevel_ = DIAGTRACE;

    feUseConsoleLogging_ = false;
    feConsoleLogLevel_ = DIAGTRACE;

    feUseDbLogging_ = false;
    feDbLogLevel_ = DIAGTRACE;

    feUseFileLogging_ = false;
    feFileLogLevel_ = DIAGTRACE;
    feFilePathAndName_ = "/tmp/errorsLogFile.txt";

    /* LEVEL ONE BLOCK */
    feCloseReconfLoop_ = false;
    feReconfProcessName_ = "Reconfigurator process";
    feReconfProcessInstance_ = "0";
    feReconfProcessLID_ = "42";
    feReconfProcessIP_ = "cmstkint02.cern.ch";
    feReconfProcessPort_ = "13000";

    /* SENTINEL GRABBER BLOCK */
    feListenSentinel_ = false;
    feListenContext_ = "TestErrorsPool";

    feUseGlbErrorDispatcher_ = false;
    feGlbErrorDispatcherLogLevel_ = DIAGTRACE;
    feGlbErrorDispatcherName_ = "GlobalErrorDispatcher";
    feGlbErrorDispatcherInstance_ = "0";
    feGlbErrorDispatcherLID_ = "30";
    feGlbErrorDispatcherIP_ = "127.0.0.1";
    feGlbErrorDispatcherPort_ = "1972";

	feLogFilePath_ = "/tmp/";

	//Initialise run runmber when unassigned
	feRunFromRcms_ = 0;
	runFromRcms_ = 0;

	#ifdef USE_XRELAY
    	isWaitingForLclLogAck_ = false;
	    missedLclEmissions_ = 0;
    	isWaitingForGlbLogAck_ = false;
	    missedGlbEmissions_ = 0;
	#endif

	std::string logLevel_ = DIAGWARN;
	useDiagSystem_ = "oldway";
	diagSystemSettings_ = "";


//	activateDiagSystemForFirstTime_ = true;
logFileNameHasChanged_ = true;
pixelsLogFileNameHasChanged_ = true;
log4jLogFileNameHasChanged_ = true;


dbFileNameHasChanged_ = true;

feDbFilePathAndName_ = "/tmp/errorsDbSwapFile.txt";


#ifdef XDAQ373
	selfAppDescriptor_ = NULL;
#else
	selfAppDescriptor_ = NULL;
#endif

sendMsgMutex = new toolbox::BSem(toolbox::BSem::FULL);
sendReconfRequestMutex = new toolbox::BSem(toolbox::BSem::FULL);
oldLogFileName_ = "oldLogFileName_";
oldPixelsLogFileName_ = "oldPixelsLogFileName_";
oldLog4jLogFileName_ = "oldLog4jLogFileName_";


fiAreInitialized_ = false;

/*
std::cout << "Launching timer for unprocessed logs" << std::endl;
std::stringstream timerName;
timerName << wa_->getApplicationDescriptor()->getContextDescriptor()->getURL() << ":";
timerName << wa_->getApplicationDescriptor()->getClassName() << ":" << wa_->getApplicationDescriptor()->getLocalId() << ":" << wa_->getApplicationDescriptor()->getInstance();
timerName << "DiagBagTimer";
timerName_ = timerName.str();
toolbox::task::Timer * timer_ = toolbox::task::getTimerFactory()->createTimer(timerName.str());
toolbox::TimeInterval interval(PUSH_INITIAL_LOGS_DELAY,0);
toolbox::TimeVal start;
start = toolbox::TimeVal::gettimeofday() + interval;
//timer_->schedule( this, start,  0, "" );
timer_->scheduleAtFixedRate( start, this, interval, wa_->getApplicationDescriptor()->getContextDescriptor(), "" );
*/
isProcessingNestedShortClog_ = false;
isProcessingNestedLongClog_ = false;

pixelsConsolePointer_ = NULL;

}


DiagBagWizard::~DiagBagWizard ( )
{
	delete sendMsgMutex;
	delete sendReconfRequestMutex;	
	chainSawManager_->deleteAllStreams();
	delete chainSawManager_;
}






void DiagBagWizard::getDiagSystemHtmlPage(xgi::Input * in, xgi::Output * out, std::string URN)
{
    // Begin HTML Form() related to Diag
    std::string url = "/";
    url += URN;
    url += "/applyConfigureDiagSystem";	

    *out << cgicc::form().set("method","post")
    .set("action", url)
    .set("enctype","multipart/form-data") << std::endl;

    *out << cgicc::p() << cgicc::input().set("type", "submit")
    .set("name", "submit")
    .set("value", "Apply Parameters");
    *out << cgicc::p() << std::endl ;

    configureCallback(in, out, "", "", "");

    *out << cgicc::p() << cgicc::input().set("type", "submit")
    .set("name", "submit")
    .set("value", "Apply Parameters");
    *out << cgicc::p() << std::endl ;

    *out << cgicc::form() << std::endl;
    /* Ends HTML Form() related to Diag */
}





int DiagBagWizard::configureCallback(xgi::Input * in,
                                    xgi::Output * out,
                                    std::string classname,
                                    std::string URL,
                                    std::string URN)
{
	/* draw a separation line */
	*out << "<hr style=\"width: 100%; height: 2px;\">";

    /* Set General Logging level for this module */
    *out << "<span style=\"font-weight: bold;\">";
    *out << cgicc::label("Select a pre-defined configuration or use hyperdaq configuration panel") << "</span><br>" << std::endl;


	// if hyperdaq page display is called, force status to : hyperdaq
	//Therefore modifications made via webpage will be auto-enabled and allowed
	useDiagSystem_ = "hyperdaq";
	
    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::label("Set the type of configuration you want for the diagnostic system attached to this process :") << std::endl;
    *out << "&nbsp;<select name=\"useDiagSystem\">";
    if (useDiagSystem_ != "off") *out << "<option>" << "off" << "</option>"; else *out << "<option selected>" << "off" << "</option>";
    if (useDiagSystem_ != "default") *out << "<option>" << "default" << "</option>"; else *out << "<option selected>" << "default" << "</option>";
    if (useDiagSystem_ != "tuned") *out << "<option>" << "tuned" << "</option>"; else *out << "<option selected>" << "tuned" << "</option>";
/*    if (useDiagSystem_ != "oldway") *out << "<option>" << "oldway" << "</option>"; else *out << "<option selected>" << "oldway" << "</option>"; */
    if (useDiagSystem_ != "hyperdaq") *out << "<option>" << "hyperdaq" << "</option>"; else *out << "<option selected>" << "hyperdaq" << "</option>";
    *out << "</select>"<< std::endl;
    *out << cgicc::p() << std::endl;


	/* draw a separation line */
	*out << "<hr style=\"width: 100%; height: 2px;\">";

/*

    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::label("Set the log level to use in case of default Diagnostic System configuration (different from option hyperdaq) usage :") << std::endl;
    *out << "&nbsp;<select name=\"loglevel\">";
    if (logLevel_ != DIAGTRACE) *out << "<option>" << DIAGTRACE << "</option>"; else *out << "<option selected>" << DIAGTRACE << "</option>";
    if (logLevel_ != DIAGDEBUG) *out << "<option>" << DIAGDEBUG << "</option>"; else *out << "<option selected>" << DIAGDEBUG << "</option>";
    if (logLevel_ != DIAGINFO) *out << "<option>" << DIAGINFO << "</option>"; else *out << "<option selected>" << DIAGINFO << "</option>";
    if (logLevel_ != DIAGWARN) *out << "<option>" << DIAGWARN << "</option>"; else *out << "<option selected>" << DIAGWARN << "</option>";
    if (logLevel_ != DIAGUSERINFO) *out << "<option>" << DIAGUSERINFO << "</option>"; else *out << "<option selected>" << DIAGUSERINFO << "</option>";
    if (logLevel_ != DIAGERROR) *out << "<option>" << DIAGERROR << "</option>"; else *out << "<option selected>" << DIAGERROR << "</option>";
    if (logLevel_ != DIAGFATAL) *out << "<option>" << DIAGFATAL << "</option>"; else *out << "<option selected>" << DIAGFATAL << "</option>";
    if (logLevel_ != DIAGOFF) *out << "<option>" << DIAGOFF << "</option>"; else *out << "<option selected>" << DIAGOFF << "</option>";
    *out << "</select>"<< std::endl;
    *out << cgicc::p() << std::endl;
*/

/*
    if (useDiagSystem_ == "default")
    {
        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
        .set("name","useDiagSystem").set("checked","true") << std::endl;
    }
    else
    {
        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
        .set("name","useDiagSystem").set("unchecked","false") << std::endl;
    }
    *out << cgicc::label("Use Diagnostic System default configuration (talk to Global Error Dispatcher) and bypass all other options ?") << std::endl;
    if (diagSystemStatus_ == "checked")
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
    if ( (diagSystemStatus_ != "checked") && (diagSystemStatus_ != "unchecked") )
        *out << "<span style=\"color: rgb(204, 0, 0);\">--" <<  diagSystemStatus_ << "</span>" << std::endl;

    *out << cgicc::p() << std::endl;


	// draw a separation line 
	*out << "<hr style=\"width: 100%; height: 2px;\">";

*/




    /* Set General Logging level for this module */
    *out << "<span style=\"font-weight: bold;\">";
    *out << cgicc::label("Process Appender Settings") << "</span><br>" << std::endl;

    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    *out << cgicc::label("Set the general log level for this process to :") << std::endl;
    *out << "&nbsp;<select name=\"gloglevel\">";
    if (gLogLevel_ != DIAGTRACE) *out << "<option>" << DIAGTRACE << "</option>"; else *out << "<option selected>" << DIAGTRACE << "</option>";
    if (gLogLevel_ != DIAGDEBUG) *out << "<option>" << DIAGDEBUG << "</option>"; else *out << "<option selected>" << DIAGDEBUG << "</option>";
    if (gLogLevel_ != DIAGINFO) *out << "<option>" << DIAGINFO << "</option>"; else *out << "<option selected>" << DIAGINFO << "</option>";
    if (gLogLevel_ != DIAGWARN) *out << "<option>" << DIAGWARN << "</option>"; else *out << "<option selected>" << DIAGWARN << "</option>";
    if (gLogLevel_ != DIAGUSERINFO) *out << "<option>" << DIAGUSERINFO << "</option>"; else *out << "<option selected>" << DIAGUSERINFO << "</option>";
    if (gLogLevel_ != DIAGERROR) *out << "<option>" << DIAGERROR << "</option>"; else *out << "<option selected>" << DIAGERROR << "</option>";
    if (gLogLevel_ != DIAGFATAL) *out << "<option>" << DIAGFATAL << "</option>"; else *out << "<option selected>" << DIAGFATAL << "</option>";
    if (gLogLevel_ != DIAGOFF) *out << "<option>" << DIAGOFF << "</option>"; else *out << "<option selected>" << DIAGOFF << "</option>";
    *out << "</select>"<< std::endl;
    *out << cgicc::p() << std::endl;

    if (useDefaultAppender_ == true)
    {
        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
        .set("name","useDefaultAppender").set("checked","true") << std::endl;
    }
    else
    {
        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
        .set("name","useDefaultAppender").set("unchecked","false") << std::endl;
    }
    *out << cgicc::label("Use Default appender for routing error messages issued from this process.") << std::endl;
    if (defaultAppenderStatus_ == "checked")
        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
    if ( (defaultAppenderStatus_ != "checked") && (defaultAppenderStatus_ != "unchecked") )
        *out << "<span style=\"color: rgb(204, 0, 0);\">--" <<  defaultAppenderStatus_ << "</span>" << std::endl;

    *out << cgicc::p() << std::endl;

    if ( (this->getIsInUserProcess() == true) || (this->getIsInErrorManager() == true) ||
		 (this->getIsInSentinelListener() == true) || (this->getIsInLogsReader() == true ) )
    {
        if (useConsoleDump_ == true)
        {
            *out << cgicc::p() << cgicc::input().set("type", "checkbox")
            .set("name","useConsoleDump").set("checked","true") << std::endl;
        }
        else
        {
            *out << cgicc::p() << cgicc::input().set("type", "checkbox")
            .set("name","useConsoleDump").set("unchecked","false") << std::endl;
        }
        *out << cgicc::label("Duplicate error messages issued from this process to locale console.") << std::endl;
        if (useConsoleDump_ == true)
        {
            *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;Messages duplication Activated</span>" << std::endl;
            DiagBag::setConsoleDumpEnabled(true);
        }
        else DiagBag::setConsoleDumpEnabled(false);
        *out << cgicc::p() << std::endl;
    }






    /* Create a CheckBox : Use Error Dispatcher Y/N */
//MDEB
/*
    if ( (this->getIsInErrorDispatcher() == false) && (this->getIsInGlobalErrorDispatcher() == false) )
    {

*/
	    /* draw a separation line */
	    *out << "<hr style=\"width: 100%; height: 2px;\">";

	    /* Create a CheckBox : Use Sentinel System Y/N */
    	*out << "<span style=\"font-weight: bold;\">";
	    *out << cgicc::label("Sentinel System configuration") << "</span>" << std::endl;
    	*out << cgicc::p() << std::endl;
	    *out << cgicc::p() << std::endl;

    	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
	    *out << cgicc::label("Set the Sentinel log level for this process (overrides general log level) :") << std::endl;
    	*out << "&nbsp;<select name=\"sentinelloglevel\">";
	    if (sentinelLogLevel_ != DIAGTRACE) *out << "<option>" << DIAGTRACE << "</option>"; else *out << "<option selected>" << DIAGTRACE << "</option>";
    	if (sentinelLogLevel_ != DIAGDEBUG) *out << "<option>" << DIAGDEBUG << "</option>"; else *out << "<option selected>" << DIAGDEBUG << "</option>";
	    if (sentinelLogLevel_ != DIAGINFO) *out << "<option>" << DIAGINFO << "</option>"; else *out << "<option selected>" << DIAGINFO << "</option>";
    	if (sentinelLogLevel_ != DIAGWARN) *out << "<option>" << DIAGWARN << "</option>"; else *out << "<option selected>" << DIAGWARN << "</option>";
	    if (sentinelLogLevel_ != DIAGUSERINFO) *out << "<option>" << DIAGUSERINFO << "</option>"; else *out << "<option selected>" << DIAGUSERINFO << "</option>";
    	if (sentinelLogLevel_ != DIAGERROR) *out << "<option>" << DIAGERROR << "</option>"; else *out << "<option selected>" << DIAGERROR << "</option>";
	    if (sentinelLogLevel_ != DIAGFATAL) *out << "<option>" << DIAGFATAL << "</option>"; else *out << "<option selected>" << DIAGFATAL << "</option>";
    	if (sentinelLogLevel_ != DIAGOFF) *out << "<option>" << DIAGOFF << "</option>"; else *out << "<option selected>" << DIAGOFF << "</option>";
	    *out << "</select>"<< std::endl;
    	*out << cgicc::p() << std::endl;

	    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
    	/* Create an input : Sentinel default context ? */
	    *out << cgicc::label("In which Sentinel error context should I push errors issued from this process ?") << std::endl;
    	*out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	    *out << cgicc::input().set("type","text")
    	.set("name","sentinelContextName")
	    .set("size","40")
    	.set("value",sentinelContextName_) << std::endl;

	    if (useSentinel_ == true)
    	{
        	*out << cgicc::p() << cgicc::input().set("type", "checkbox")
	        .set("name","useSentinel").set("checked","true") << std::endl;
    	}
	    else
    	{
	        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
    	    .set("name","useSentinel").set("unchecked","false") << std::endl;
	    }
    	*out << cgicc::label("Use Sentinel System for this process") << std::endl;
	    if (sentinelUsableStatus_ == "checked")
		        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;Sentinel Activated</span>" << std::endl;
	    if ( (sentinelUsableStatus_ != "checked") && (sentinelUsableStatus_ != "unchecked") )
    		    *out << "<span style=\"color: rgb(204, 0, 0);\">--" <<  sentinelUsableStatus_ << "</span>" << std::endl;
	    *out << cgicc::p() << std::endl;






	    if ( this->getIsInSentinelListener() == true )
    	{
        	/* draw a separation line */
	        *out << "<hr style=\"width: 100%; height: 2px;\">";
    	    *out << "&nbsp;&nbsp;&nbsp;&nbsp;";

        	/* Create an input : Sentinel context to listen ? */
	        *out << cgicc::label("Which Context should this application listen to ?") << std::endl;
    	    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
        	*out << cgicc::input().set("type","text")
	        .set("name","grabberContextName")
    	    .set("size","40")
        	.set("value",grabberContextName_) << std::endl;

	        *out << cgicc::p() << std::endl;

    	    if (useGrabber_ == true)
        	{
            	*out << cgicc::p() << cgicc::input().set("type", "checkbox")
	            .set("name","useGrabber").set("checked","true") << std::endl;
    	    }
        	else
	        {
    	        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
        	    .set("name","useGrabber").set("unchecked","false") << std::endl;
	        }
    	    *out << cgicc::label("Listen to specific context error") << std::endl;
        	if (grabberUsableStatus_ == "checked")
		            *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;Sentinel Activated</span>" << std::endl;
        	if ( (grabberUsableStatus_ != "checked") && (grabberUsableStatus_ != "unchecked") )
            		*out << "<span style=\"color: rgb(204, 0, 0);\">--" <<  grabberUsableStatus_ << "</span>" << std::endl;
	        *out << cgicc::p() << std::endl;


    	}
		/* draw a separation line */
    	*out << "<hr style=\"width: 100%; height: 2px;\">";


	    *out << "<span style=\"font-weight: bold;\">";
        *out << cgicc::label("Local Error Dispatcher configuration") << "</span>" << cgicc::p() << std::endl;

		*out << cgicc::p() << std::endl;

	    *out << cgicc::label("Set the local Error Dispatcher log level for this process (overrides general log level) :") << std::endl;
    	*out << "&nbsp;<select name=\"edloglevel\">";
	    if (edLogLevel_ != DIAGTRACE) *out << "<option>" << DIAGTRACE << "</option>"; else *out << "<option selected>" << DIAGTRACE << "</option>";
    	if (edLogLevel_ != DIAGDEBUG) *out << "<option>" << DIAGDEBUG << "</option>"; else *out << "<option selected>" << DIAGDEBUG << "</option>";
	    if (edLogLevel_ != DIAGINFO) *out << "<option>" << DIAGINFO << "</option>"; else *out << "<option selected>" << DIAGINFO << "</option>";
    	if (edLogLevel_ != DIAGWARN) *out << "<option>" << DIAGWARN << "</option>"; else *out << "<option selected>" << DIAGWARN << "</option>";
	    if (edLogLevel_ != DIAGUSERINFO) *out << "<option>" << DIAGUSERINFO << "</option>"; else *out << "<option selected>" << DIAGUSERINFO << "</option>";
    	if (edLogLevel_ != DIAGERROR) *out << "<option>" << DIAGERROR << "</option>"; else *out << "<option selected>" << DIAGERROR << "</option>";
	    if (edLogLevel_ != DIAGFATAL) *out << "<option>" << DIAGFATAL << "</option>"; else *out << "<option selected>" << DIAGFATAL << "</option>";
    	if (edLogLevel_ != DIAGOFF) *out << "<option>" << DIAGOFF << "</option>"; else *out << "<option selected>" << DIAGOFF << "</option>";
	   *out << "</select>"<< std::endl;

	    if (useDiag_ == true)
    	{
        	*out << cgicc::p() << cgicc::input().set("type", "checkbox")
	        .set("name","useDiag").set("checked","true") << std::endl;
    	}
	    else
    	{
	        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
    	    .set("name","useDiag").set("unchecked","false") << std::endl;
	    }

    if ( (this->getIsInErrorDispatcher() == false) && (this->getIsInGlobalErrorDispatcher() == false) )
    {
    	*out << cgicc::label("Use Local Error Dispatcher for this process") << std::endl;
	}
	else *out << cgicc::label("Forward received error messages to another local error dispatcher") << std::endl;

	    if (useDiag_ == true)
    	{
        	if (edUsable_==true)
            		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
	        if ((useDiag_ == true) && (useDiagStatus_ != "unchecked") && (useDiagStatus_ != "checked"))
    		        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  useDiagStatus_ << "</span><br>" << std::endl;
	    }
    	*out << cgicc::p() << std::endl;

	    if ( (useDiagStatus_ == "unchecked") || (useDiagStatus_ == "checked") )
    	{
        	/* display status error if needed */
	        if ( (diagNameStatus_ != "checked") && (diagNameStatus_ != "unchecked") )
    		        *out << "<span style=\"color: rgb(204, 0, 0);\">--" <<  diagNameStatus_ << "</span>" << std::endl;
	        *out << cgicc::p() << std::endl;
    	    if ( (diagLIDStatus_ != "checked") && (diagLIDStatus_ != "unchecked") )
        		    *out << "<span style=\"color: rgb(204, 0, 0);\">" <<  diagLIDStatus_ << "</span>" << std::endl;
	        if ( (diagIPStatus_ != "checked") && (diagIPStatus_ != "unchecked") )
    		        *out << "<span style=\"color: rgb(204, 0, 0);\">" <<  diagIPStatus_ << "</span>" << std::endl;
	    }
	    *out << cgicc::p() << std::endl;


	    /* Create an input : ED server name ? */
    	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
	    *out << cgicc::label("Error Dispatcher Server Name") << std::endl;
    	if (diagNameStatus_ == "checked")
        		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
	    if ( (diagNameStatus_ != "checked") && (diagNameStatus_ != "unchecked") )
    		    *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
	    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    	*out << cgicc::input().set("type","text")
	    .set("name","diagName")
    	.set("size","40")
	    .set("value",diagName_) << std::endl;
    	*out << cgicc::p() << std::endl;


	    /* Create an input : ED Instance ? */
    	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
	    *out << cgicc::label("Error Dispatcher Server Instance") << std::endl;
    	if (diagNameStatus_ == "checked")
        		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
	    if ( (diagNameStatus_ != "checked") && (diagNameStatus_ != "unchecked") )
    		    *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" << "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
	    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    	*out << cgicc::input().set("type","text")
	    .set("name","diagInstance")
    	.set("size","10")
	    .set("value",diagInstance_) << std::endl;
    	*out << cgicc::p() << std::endl;
	
/*
	    // Create an input : ED LID ?
    	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
	    *out << cgicc::label("Error Dispatcher Server LID") << std::endl;
    	if (diagLIDStatus_ == "checked")
		        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
	    if ( (diagLIDStatus_ != "checked") && (diagLIDStatus_ != "unchecked") )
    		    *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
	    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    	*out << cgicc::input().set("type","text")
	    .set("name","diagLID")
    	.set("size","10")
	    .set("value",diagLID_) << std::endl;
    	*out << cgicc::p() << std::endl;

	    // Create an input : ED IP ?
    	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
	    *out << cgicc::label("Error Dispatcher Server IP") << std::endl;
    	if (diagIPStatus_ == "checked")
        		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
	    if ( (diagIPStatus_ != "checked") && (diagIPStatus_ != "unchecked") )
    		    *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
	    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    	*out << cgicc::input().set("type","text")
	    .set("name","diagIP")
    	.set("size","20")
	    .set("value",diagIP_) << std::endl;
    	*out << cgicc::p() << std::endl;

	    // Create an input : ED port ?
    	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
	    *out << cgicc::label("Error Dispatcher Server port") << std::endl;
    	if (diagIPStatus_ == "checked")
        		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
	    if ( (diagIPStatus_ != "checked") && (diagIPStatus_ != "unchecked") )
    		    *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
	    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    	*out << cgicc::input().set("type","text")
	    .set("name","diagPort")
    	.set("size","10")
	    .set("value",diagPort_) << std::endl;
    	*out << cgicc::p() << std::endl;
*/
	    /* draw a separation line */
    	*out << "<hr style=\"width: 100%; height: 2px;\">";




	    *out << "<span style=\"font-weight: bold;\">";
		*out << cgicc::label("Global Error Dispatcher configuration") << "</span>" << cgicc::p() << std::endl;
	    *out << cgicc::p() << std::endl;

	    *out << cgicc::label("Set the Global Error Dispatcher log level for this process (overrides general log level) :") << std::endl;
    	*out << "&nbsp;<select name=\"edGlbloglevel\">";
	    if (edGlbLogLevel_ != DIAGTRACE) *out << "<option>" << DIAGTRACE << "</option>"; else *out << "<option selected>" << DIAGTRACE << "</option>";
    	if (edGlbLogLevel_ != DIAGDEBUG) *out << "<option>" << DIAGDEBUG << "</option>"; else *out << "<option selected>" << DIAGDEBUG << "</option>";
	    if (edGlbLogLevel_ != DIAGINFO) *out << "<option>" << DIAGINFO << "</option>"; else *out << "<option selected>" << DIAGINFO << "</option>";
    	if (edGlbLogLevel_ != DIAGWARN) *out << "<option>" << DIAGWARN << "</option>"; else *out << "<option selected>" << DIAGWARN << "</option>";
	    if (edGlbLogLevel_ != DIAGUSERINFO) *out << "<option>" << DIAGUSERINFO << "</option>"; else *out << "<option selected>" << DIAGUSERINFO << "</option>";
    	if (edGlbLogLevel_ != DIAGERROR) *out << "<option>" << DIAGERROR << "</option>"; else *out << "<option selected>" << DIAGERROR << "</option>";
	    if (edGlbLogLevel_ != DIAGFATAL) *out << "<option>" << DIAGFATAL << "</option>"; else *out << "<option selected>" << DIAGFATAL << "</option>";
    	if (edGlbLogLevel_ != DIAGOFF) *out << "<option>" << DIAGOFF << "</option>"; else *out << "<option selected>" << DIAGOFF << "</option>";
	    *out << "</select>"<< std::endl;

	    if (useGlbDiag_ == true)
    	{
	        *out << cgicc::p() << cgicc::input().set("type", "checkbox")
    	    .set("name","useGlbDiag").set("checked","true") << std::endl;
	    }
    	else
	    {
    	    *out << cgicc::p() << cgicc::input().set("type", "checkbox")
        	.set("name","useGlbDiag").set("unchecked","false") << std::endl;
	    }
	

    if ( (this->getIsInErrorDispatcher() == true) || (this->getIsInGlobalErrorDispatcher() == true) )
    {
    	*out << cgicc::label("Forward received error messages to a Global Error Dispatcher") << std::endl;
	}
	else *out << cgicc::label("Use Global Error Dispatcher for this process") << std::endl;

	    if (useGlbDiag_ == true)
    	{
        	if (edGlbUsable_==true)
            		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
	        if ((useGlbDiag_ == true) && (useGlbDiagStatus_ != "unchecked") && (useGlbDiagStatus_ != "checked"))
    		        *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  useGlbDiagStatus_ << "</span><br>" << std::endl;
	    }
    	*out << cgicc::p() << std::endl;

	    if ( (useGlbDiagStatus_ == "unchecked") || (useGlbDiagStatus_ == "checked") )
    	{
	        /* display status error if needed */
    	    if ( (diagGlbNameStatus_ != "checked") && (diagGlbNameStatus_ != "unchecked") )
        		    *out << "<span style=\"color: rgb(204, 0, 0);\">--" <<  diagGlbNameStatus_ << "</span>" << std::endl;
	        *out << cgicc::p() << std::endl;
    	    if ( (diagGlbLIDStatus_ != "checked") && (diagGlbLIDStatus_ != "unchecked") )
        		    *out << "<span style=\"color: rgb(204, 0, 0);\">" <<  diagLIDStatus_ << "</span>" << std::endl;
	        if ( (diagGlbIPStatus_ != "checked") && (diagGlbIPStatus_ != "unchecked") )
    		        *out << "<span style=\"color: rgb(204, 0, 0);\">" <<  diagGlbIPStatus_ << "</span>" << std::endl;
	    }
    	*out << cgicc::p() << std::endl;


	    /* Create an input : ED server name ? */
    	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
	    *out << cgicc::label("Higher Level Error Dispatcher Server Name") << std::endl;
    	if (diagGlbNameStatus_ == "checked")
        		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
	    if ( (diagGlbNameStatus_ != "checked") && (diagGlbNameStatus_ != "unchecked") )
    		    *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
	    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    	*out << cgicc::input().set("type","text")
	    .set("name","diagGlbName")
    	.set("size","40")
	    .set("value",diagGlbName_) << std::endl;
    	*out << cgicc::p() << std::endl;


	    /* Create an input : ED Instance ? */
    	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
	    *out << cgicc::label("Higher Level Error Dispatcher Server Instance") << std::endl;
    	if (diagGlbNameStatus_ == "checked")
        		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
	    if ( (diagGlbNameStatus_ != "checked") && (diagGlbNameStatus_ != "unchecked") )
    		    *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" << "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
	    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    	*out << cgicc::input().set("type","text")
	    .set("name","diagGlbInstance")
    	.set("size","10")
	    .set("value",diagGlbInstance_) << std::endl;
    	*out << cgicc::p() << std::endl;
/*
	    // Create an input : ED LID ?
    	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
	    *out << cgicc::label("Higher Level Error Dispatcher Server LID") << std::endl;
    	if (diagGlbLIDStatus_ == "checked")
        		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
	    if ( (diagGlbLIDStatus_ != "checked") && (diagGlbLIDStatus_ != "unchecked") )
    		    *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
	    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    	*out << cgicc::input().set("type","text")
	    .set("name","diagGlbLID")
    	.set("size","10")
	    .set("value",diagGlbLID_) << std::endl;
    	*out << cgicc::p() << std::endl;

	    // Create an input : ED IP ?
    	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
	    *out << cgicc::label("Higher Level Error Dispatcher Server IP") << std::endl;
    	if (diagGlbIPStatus_ == "checked")
        		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
	    if ( (diagGlbIPStatus_ != "checked") && (diagGlbIPStatus_ != "unchecked") )
    		    *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
	    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    	*out << cgicc::input().set("type","text")
	    .set("name","diagGlbIP")
    	.set("size","20")
	    .set("value",diagGlbIP_) << std::endl;
    	*out << cgicc::p() << std::endl;

	    // Create an input : ED port ?
    	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
	    *out << cgicc::label("Higher Level Error Dispatcher Server port") << std::endl;
    	if (diagGlbIPStatus_ == "checked")
        		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
	    if ( (diagGlbIPStatus_ != "checked") && (diagGlbIPStatus_ != "unchecked") )
    		    *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
	    *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
    	*out << cgicc::input().set("type","text")
	    .set("name","diagGlbPort")
    	.set("size","10")
	    .set("value",diagGlbPort_) << std::endl;
    	*out << cgicc::p() << std::endl;
*/
	    /* draw a separation line */
    	*out << "<hr style=\"width: 100%; height: 2px;\">";









		/* Add Reconfiguration code block here */
	//Create a CheckBox : Use Diag Level One Error Manager Y/N
	*out << "<span style=\"font-weight: bold;\">";
	*out << cgicc::label("Error Recovery process") << "</span>" << std::endl;
	*out << cgicc::p() << std::endl;
	*out << cgicc::p() << std::endl;

	
	
	if (useLvl1_ == true)
	{
		*out << cgicc::p() << cgicc::input().set("type", "checkbox")
		.set("name","useLvl1").set("checked","true") << std::endl;
	}
	else
	{
		*out << cgicc::p() << cgicc::input().set("type", "checkbox")
		.set("name","useLvl1").set("unchecked","false") << std::endl;
	}
	*out << cgicc::label("Use an Error Recovery process") << std::endl;

	if (useLvl1_ == true)
	{
		if (useLvl1Status_ == "checked")
			*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
		if ( (useLvl1Status_ != "checked") && (useLvl1Status_ != "unchecked") )
			*out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  useLvl1Status_ << "</span><br>" << std::endl;
	}
	*out << cgicc::p() << std::endl;

	//display status error if needed
	if ( (lvl1NameStatus_ != "checked") && (lvl1NameStatus_ != "unchecked") )
		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  lvl1NameStatus_ << "</span><br>" << std::endl;
	if ( (lvl1LIDStatus_ != "checked") && (lvl1LIDStatus_ != "unchecked") )
		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  lvl1LIDStatus_ << "</span><br>" << std::endl;
	if ( (lvl1IPStatus_ != "checked") && (lvl1IPStatus_ != "unchecked") )
		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  lvl1IPStatus_ << "</span><br>" << std::endl;

	//Create an input :Lvl1 server name ?
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
	*out << cgicc::label("Error Recovery process Name") << std::endl;
	if (lvl1NameStatus_ == "checked")
		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
	if ( (lvl1NameStatus_ != "checked") && (lvl1NameStatus_ != "unchecked") )
		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
	*out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	*out << cgicc::input().set("type","text")
	.set("name","lvl1Name")
	.set("size","40")
	.set("value",lvl1Name_) << std::endl;
	*out << cgicc::p() << std::endl;

	//Create an input : EM Instance ?
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
	*out << cgicc::label("Error Recovery process Instance") << std::endl;
	if (lvl1NameStatus_ == "checked")
		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
	if ( (lvl1NameStatus_ != "checked") && (lvl1NameStatus_ != "unchecked") )
		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
	*out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	*out << cgicc::input().set("type","text")
	.set("name","lvl1Instance")
	.set("size","10")
	.set("value",lvl1Instance_) << std::endl;
	*out << cgicc::p() << std::endl;

/*
	//Create an input : EM LID ?
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
	*out << cgicc::label("Error Manager Server LID") << std::endl;
	if (lvl1LIDStatus_ == "checked")
		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
	if ( (lvl1LIDStatus_ != "checked") && (lvl1LIDStatus_ != "unchecked") )
		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
	*out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	*out << cgicc::input().set("type","text")
	.set("name","lvl1LID")
	.set("size","10")
	.set("value",lvl1LID_) << std::endl;
	*out << cgicc::p() << std::endl;
*/

/*
	//Create an input : EM IP ?
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
	*out << cgicc::label("Error Recovery process Server IP") << std::endl;
	if (lvl1IPStatus_ == "checked")
		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
	if ( (lvl1IPStatus_ != "checked") && (lvl1IPStatus_ != "unchecked") )
		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
	*out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	*out << cgicc::input().set("type","text")
	.set("name","lvl1IP")
	.set("size","20")
	.set("value",lvl1IP_) << std::endl;
	*out << cgicc::p() << std::endl;
*/

/*
	//Create an input : EM port ?
	*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
	*out << cgicc::label("Error Recovery process Server port") << std::endl;
	if (lvl1IPStatus_ == "checked")
		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
	if ( (lvl1IPStatus_ != "checked") && (lvl1IPStatus_ != "unchecked") )
		*out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
	*out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
	*out << cgicc::input().set("type","text")
	.set("name","lvl1Port")
	.set("size","10")
	.set("value",lvl1Port_) << std::endl;
	*out << cgicc::p() << std::endl;
*/
	//draw a separation line
	*out << "<hr style=\"width: 100%; height: 2px;\">";


//MDEB
/*
	}
*/






    if ( (this->getIsInErrorDispatcher() == true) || (this->getIsInGlobalErrorDispatcher() == true) )
    {
	    /* draw a separation line */
//    	*out << "<hr style=\"width: 100%; height: 2px;\">";






for (int csStreamNum = 0; csStreamNum < nbrOfChainsawStreams_ ; csStreamNum++)
{
std::stringstream streamNb;
streamNb << csStreamNum;
std::string csRouteNumedName = "csRoute" + streamNb.str();
std::string csIPNumedName = "csIP" + streamNb.str();
std::string csPortNumedName = "csPort" + streamNb.str();
std::string csReceiverNumedName = "csReceiver" + streamNb.str();
std::string csLogLevelNumedName = "csLogLevel" + streamNb.str();
std::string csFilterNumedName = "csFilter" + streamNb.str();

        /* Route incoming messages to Chainsaw */
        /* Create a CheckBox : Create a socketXML to route ED log messages to chainsaw */
        if (csRoute_[csStreamNum] == true)
        {
            *out << cgicc::p() << cgicc::input().set("type", "checkbox")
            .set("name",csRouteNumedName).set("checked","true") << std::endl;
        }
        else
        {
            *out << cgicc::p() << cgicc::input().set("type", "checkbox")
            .set("name",csRouteNumedName).set("unchecked","false") << std::endl;
        }
        *out << "<span style=\"font-weight: bold;\">";
        *out << cgicc::label("Route log messages received by Error Dispatcher to Chainsaw") << "</span>" << std::endl;

        if (csRoute_[csStreamNum] == true)
        {
            if (csRouteValidated_[csStreamNum]==true)
                *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
            if ((csRouteValidated_[csStreamNum]==false) && (csReceiverStatus_[csStreamNum] != "unchecked"))
                *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">&nbsp;&nbsp;Error during process configuration</span>" << std::endl;
        }
        *out << cgicc::p() << std::endl;

        /* display status error if needed */
        if ( (csReceiverStatus_[csStreamNum] != "checked") && (csReceiverStatus_[csStreamNum] != "unchecked") )
            *out << "<span style=\"color: rgb(204, 0, 0);\">--" <<  csReceiverStatus_[csStreamNum] << "</span>" << std::endl;

        /* Create an input : CS IP ? */
        *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::label("Chainsaw Server IP") << std::endl;
        if (csIPStatus_[csStreamNum] == "checked")
            *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
        *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::input().set("type","text")
        .set("name",csIPNumedName)
        .set("size","20")
        .set("value",csIP_[csStreamNum]) << std::endl;
        *out << cgicc::p() << std::endl;

        /* Create an input : CS port ? */
        *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::label("Chainsaw Server port") << std::endl;
        if (csPortStatus_[csStreamNum] == "checked")
            *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
        *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::input().set("type","text")
        .set("name",csPortNumedName)
        .set("size","10")
        .set("value",csPort_[csStreamNum]) << std::endl;
        *out << cgicc::p() << std::endl;

        /* Create an input : CS receiver name ? */
        *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::label("Chainsaw Receiver Name") << std::endl;
        if (csReceiverStatus_[csStreamNum] == "checked")
            *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
        *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::input().set("type","text")
        .set("name",csReceiverNumedName)
        .set("size","40")
        .set("value",csReceiver_[csStreamNum]) << std::endl;
        *out << cgicc::p() << std::endl;

        /* Set Specific Logging level for Chainsaw */
        *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::label("Route log messages to Chainsaw for levels equal to or higher than&nbsp;:&nbsp;") << std::endl;
        *out << "&nbsp;<select name=\"" << csLogLevelNumedName << "\">";
        if (csLogLevel_[csStreamNum] != DIAGTRACE) *out << "<option>" << DIAGTRACE << "</option>"; else *out << "<option selected>" << DIAGTRACE << "</option>";
        if (csLogLevel_[csStreamNum] != DIAGDEBUG) *out << "<option>" << DIAGDEBUG << "</option>"; else *out << "<option selected>" << DIAGDEBUG << "</option>";
        if (csLogLevel_[csStreamNum] != DIAGINFO) *out << "<option>" << DIAGINFO << "</option>"; else *out << "<option selected>" << DIAGINFO << "</option>";
        if (csLogLevel_[csStreamNum] != DIAGWARN) *out << "<option>" << DIAGWARN << "</option>"; else *out << "<option selected>" << DIAGWARN << "</option>";
        if (csLogLevel_[csStreamNum] != DIAGUSERINFO) *out << "<option>" << DIAGUSERINFO << "</option>"; else *out << "<option selected>" << DIAGUSERINFO << "</option>";
        if (csLogLevel_[csStreamNum] != DIAGERROR) *out << "<option>" << DIAGERROR << "</option>"; else *out << "<option selected>" << DIAGERROR << "</option>";
        if (csLogLevel_[csStreamNum] != DIAGFATAL) *out << "<option>" << DIAGFATAL << "</option>"; else *out << "<option selected>" << DIAGFATAL << "</option>";
        if (csLogLevel_[csStreamNum] != DIAGOFF) *out << "<option>" << DIAGOFF << "</option>"; else *out << "<option selected>" << DIAGOFF << "</option>";
        *out << "</select>"<< std::endl;
        *out << cgicc::p() << std::endl;



        /* Create an input : CS filter ? */
        *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::label("Filter messages ; use this stream only for messages where URL/Logger contains the sentence : ") << std::endl;
        *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::input().set("type","text")
        .set("name",csFilterNumedName)
        .set("size","50")
        .set("value",csFilter_[csStreamNum]) << std::endl;
        *out << cgicc::p() << std::endl;


        /* draw a separation line */
        *out << "<hr style=\"width: 100%; height: 2px;\">";

}







        /* Route incoming messages to Console */
        if (ccRoute_ == true)
        {
            *out << cgicc::p() << cgicc::input().set("type", "checkbox")
            .set("name","ccRoute").set("checked","true") << std::endl;
        }
        else
        {
            *out << cgicc::p() << cgicc::input().set("type", "checkbox")
            .set("name","ccRoute").set("unchecked","false") << std::endl;
        }
        *out << "<span style=\"font-weight: bold;\">";
        *out << cgicc::label("Route log messages received by Error Dispatcher to Console") << "</span>" << std::endl;

        if (ccRoute_ == true)
        {
            if (ccRouteValidated_==true)
                *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
            if ((ccRouteValidated_==false) && (ccRouteStatus_ != "unchecked"))
                *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">&nbsp;&nbsp;Error in process configuration</span>" << std::endl;
        }
        *out << cgicc::p() << std::endl;

        /* display status error if needed */
        if ( (ccRouteStatus_ != "checked") && (ccRouteStatus_ != "unchecked") )
            *out << "<span style=\"color: rgb(204, 0, 0);\">--" <<  ccRouteStatus_ << "</span>" << cgicc::p() << std::endl;

        /* Set Specific Logging level for Console */
        *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::label("Route log messages to Console for levels equal to or higher than&nbsp;:&nbsp;") << std::endl;
        *out << "&nbsp;<select name=\"ccloglevel\">";
        if (ccLogLevel_ != DIAGTRACE) *out << "<option>" << DIAGTRACE << "</option>"; else *out << "<option selected>" << DIAGTRACE << "</option>";
        if (ccLogLevel_ != DIAGDEBUG) *out << "<option>" << DIAGDEBUG << "</option>"; else *out << "<option selected>" << DIAGDEBUG << "</option>";
        if (ccLogLevel_ != DIAGINFO) *out << "<option>" << DIAGINFO << "</option>"; else *out << "<option selected>" << DIAGINFO << "</option>";
        if (ccLogLevel_ != DIAGWARN) *out << "<option>" << DIAGWARN << "</option>"; else *out << "<option selected>" << DIAGWARN << "</option>";
        if (ccLogLevel_ != DIAGUSERINFO) *out << "<option>" << DIAGUSERINFO << "</option>"; else *out << "<option selected>" << DIAGUSERINFO << "</option>";
        if (ccLogLevel_ != DIAGERROR) *out << "<option>" << DIAGERROR << "</option>"; else *out << "<option selected>" << DIAGERROR << "</option>";
        if (ccLogLevel_ != DIAGFATAL) *out << "<option>" << DIAGFATAL << "</option>"; else *out << "<option selected>" << DIAGFATAL << "</option>";
        if (ccLogLevel_ != DIAGOFF) *out << "<option>" << DIAGOFF << "</option>"; else *out << "<option selected>" << DIAGOFF << "</option>";
        *out << "</select>"<< std::endl;
        *out << cgicc::p() << std::endl;

        /* draw a separation line */
        *out << "<hr style=\"width: 100%; height: 2px;\">";





        /* Route incoming messages to Database */
//FINDME
//std::cout << "Entering DB options display : dbRoute_=" << dbRoute_ << " : dbRouteValidated_=" << dbRouteValidated_ << " : dbRouteStatus_=" << dbRouteStatus_ << std::endl;



        if (dbRoute_ == true)
        {
            *out << cgicc::p() << cgicc::input().set("type", "checkbox")
            .set("name","dbRoute").set("checked","true") << std::endl;
        }
        else
        {
            *out << cgicc::p() << cgicc::input().set("type", "checkbox")
            .set("name","dbRoute").set("unchecked","false") << std::endl;
        }
        *out << "<span style=\"font-weight: bold;\">";
        *out << cgicc::label("Route log messages received by Error Dispatcher to DataBase") << "</span>" << std::endl;

        if (dbRoute_ == true)
        {
            if (dbRouteValidated_==true)
                *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
            if ((dbRouteValidated_==false) && (dbRouteStatus_ != "unchecked"))
                *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">&nbsp;&nbsp;Error in process configuration</span>" << std::endl;
        }
        *out << cgicc::p() << std::endl;

        /* display status error if needed */
        if ( (dbRouteStatus_ != "checked") && (dbRouteStatus_ != "unchecked") )
            *out << "<span style=\"color: rgb(204, 0, 0);\">--" <<  dbRouteStatus_ << "</span>" << cgicc::p() << std::endl;

        /* Set Specific Logging level for Database */
        *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::label("Route log messages to Database for levels equal to or higher than&nbsp;:&nbsp;") << std::endl;
        *out << "&nbsp;<select name=\"dbloglevel\">";
        if (dbLogLevel_ != DIAGTRACE) *out << "<option>" << DIAGTRACE << "</option>"; else *out << "<option selected>" << DIAGTRACE << "</option>";
        if (dbLogLevel_ != DIAGDEBUG) *out << "<option>" << DIAGDEBUG << "</option>"; else *out << "<option selected>" << DIAGDEBUG << "</option>";
        if (dbLogLevel_ != DIAGINFO) *out << "<option>" << DIAGINFO << "</option>"; else *out << "<option selected>" << DIAGINFO << "</option>";
        if (dbLogLevel_ != DIAGWARN) *out << "<option>" << DIAGWARN << "</option>"; else *out << "<option selected>" << DIAGWARN << "</option>";
        if (dbLogLevel_ != DIAGUSERINFO) *out << "<option>" << DIAGUSERINFO << "</option>"; else *out << "<option selected>" << DIAGUSERINFO << "</option>";
        if (dbLogLevel_ != DIAGERROR) *out << "<option>" << DIAGERROR << "</option>"; else *out << "<option selected>" << DIAGERROR << "</option>";
        if (dbLogLevel_ != DIAGFATAL) *out << "<option>" << DIAGFATAL << "</option>"; else *out << "<option selected>" << DIAGFATAL << "</option>";
        if (dbLogLevel_ != DIAGOFF) *out << "<option>" << DIAGOFF << "</option>"; else *out << "<option selected>" << DIAGOFF << "</option>";
        *out << "</select>"<< std::endl;
        *out << cgicc::p() << std::endl;

        /* Create an input : FILE logging path ? */
        *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::label("File Path + Name (e.g. /data/logs/myDBFile.txt)") << std::endl;
        *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::input().set("type","text")
        .set("name","dbFileName")
        .set("size","40")
        .set("value",dbFileName_) << std::endl;
        *out << cgicc::p() << std::endl;

        /* draw a separation line */
        *out << "<hr style=\"width: 100%; height: 2px;\">";







/*
        // Create a CheckBox : use FILE LOGGING capabilities ?
        if (useFileLogging_ == true)
        {
            *out << cgicc::p() << cgicc::input().set("type", "checkbox")
            .set("name","useFileLogging").set("checked","true") << std::endl;
        }
        else
        {
            *out << cgicc::p() << cgicc::input().set("type", "checkbox")
            .set("name","useFileLogging").set("unchecked","false") << std::endl;
        }
        *out << "<span style=\"font-weight: bold;\">";
        *out << cgicc::label("Append error messages to FILE") << "</span>";

		// *out << "&nbsp;&nbsp;(max. 10,000 messages per ErrorDispatcher)" << std::endl;

        if (useFileLogging_ == true)
        {
            if (fileLoggingUsable_==true)
                *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
            if ((fileLoggingUsable_==false) && (fileLoggingStatus_ != "unchecked"))
                *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">&nbsp;&nbsp;Error in process configuration</span>" << std::endl;
        }
        *out << cgicc::p() << std::endl;

        // display status error if needed
        if ( (fileLoggingStatus_ != "checked") && (fileLoggingStatus_ != "unchecked") )
            *out << "<span style=\"color: rgb(204, 0, 0);\">--" <<  fileLoggingStatus_ << "</span>" << cgicc::p() << std::endl;

        // Set General Logging level for FILE
        *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::label("Set the general FILE log level for this process to :") << std::endl;
        *out << "&nbsp;<select name=\"fileLogLevel\">";
        if (fileLogLevel_ != DIAGTRACE) *out << "<option>" << DIAGTRACE << "</option>"; else *out << "<option selected>" << DIAGTRACE << "</option>";
        if (fileLogLevel_ != DIAGDEBUG) *out << "<option>" << DIAGDEBUG << "</option>"; else *out << "<option selected>" << DIAGDEBUG << "</option>";
        if (fileLogLevel_ != DIAGINFO) *out << "<option>" << DIAGINFO << "</option>"; else *out << "<option selected>" << DIAGINFO << "</option>";
        if (fileLogLevel_ != DIAGWARN) *out << "<option>" << DIAGWARN << "</option>"; else *out << "<option selected>" << DIAGWARN << "</option>";
        if (fileLogLevel_ != DIAGUSERINFO) *out << "<option>" << DIAGUSERINFO << "</option>"; else *out << "<option selected>" << DIAGUSERINFO << "</option>";
        if (fileLogLevel_ != DIAGERROR) *out << "<option>" << DIAGERROR << "</option>"; else *out << "<option selected>" << DIAGERROR << "</option>";
        if (fileLogLevel_ != DIAGFATAL) *out << "<option>" << DIAGFATAL << "</option>"; else *out << "<option selected>" << DIAGFATAL << "</option>";
        if (fileLogLevel_ != DIAGOFF) *out << "<option>" << DIAGOFF << "</option>"; else *out << "<option selected>" << DIAGOFF << "</option>";
        *out << "</select>"<< std::endl;
        *out << cgicc::p() << std::endl;



        // Create an input : FILE logging path ? 
        *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::label("File Path + Name (e.g. /data/logs/mylogs.txt)") << std::endl;
        *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::input().set("type","text")
        .set("name","logFileName")
        .set("size","40")
        .set("value",logFileName_) << std::endl;
        *out << cgicc::p() << std::endl;
*/

        /* draw a separation line */
        *out << "<hr style=\"width: 100%; height: 2px;\">";
    }




    if (this->getIsInLogsReader() == true)
    {

        /* draw a separation line */
//        *out << "<hr style=\"width: 100%; height: 2px;\">";

        /* Create a CheckBox : use ED capabilities ? */
        if (useFileLogging_ == true)
        {
            *out << cgicc::p() << cgicc::input().set("type", "checkbox")
            .set("name","useFileLogging").set("checked","true") << std::endl;
        }
        else
        {
            *out << cgicc::p() << cgicc::input().set("type", "checkbox")
            .set("name","useFileLogging").set("unchecked","false") << std::endl;
        }
        *out << "<span style=\"font-weight: bold;\">";
        *out << cgicc::label("Read error messages from FILE") << "</span>";
		*out << "<br>If unchecked, Logs filename will be automatically grabbed from GlobalErrorDispatcher<br>";
		

		//*out << "&nbsp;&nbsp;(max. 10,000 messages per ErrorDispatcher)" << std::endl;


//                errMsg << "&nbsp;&nbsp;Using automatic file naming convention - Log file name grabbed from GlobalErrorDispatcher" << std::endl;

        if (useFileLogging_ == true)
        {
            if (fileLoggingUsable_==true)
                *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
            if (fileLoggingUsable_==false)
                *out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">&nbsp;&nbsp;Error in process configuration</span>" << std::endl;
        }
		else *out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;Using automatic file naming convention - Log file name grabbed from GlobalErrorDispatcher</span>" << std::endl;
        *out << cgicc::p() << std::endl;

        /* display status error if needed */
        if ( (fileLoggingStatus_ != "checked") && (fileLoggingStatus_ != "unchecked") )
            *out << "<span style=\"color: rgb(204, 0, 0);\">--" <<  fileLoggingStatus_ << "</span>" << cgicc::p() << std::endl;


        /* Create an input : FILE logging path ? */
        *out << "&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::label("LogMessages File Path + Name (e.g. /data/logs/mylogs.txt)") << std::endl;
        *out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
        *out << cgicc::input().set("type","text")
        .set("name","logFileName")
        .set("size","40")
        .set("value",logFileName_) << std::endl;
        *out << cgicc::p() << std::endl;

        /* draw a separation line */
        *out << "<hr style=\"width: 100%; height: 2px;\">";
    }


return 0;
}













int DiagBagWizard::getFormValues(xgi::Input * in,
                                xgi::Output * out)
{
//#define DEBUG_GET_FORM_VALUES
    if ( this->getIsInUserProcess() == true )
    {
        /* Create a new Cgicc object containing all the CGI data */
        cgicc::Cgicc cgi(in);

        useDiagSystem_ = cgi["useDiagSystem"]->getValue();
        gLogLevel_ = cgi["gloglevel"]->getValue();

        /* Query the use error dispatcher checkbox */
        useDiag_ = cgi.queryCheckbox("useDiag");
        /* Query ED name */
        diagName_ = cgi["diagName"]->getValue() ;
        /* Query ED instance */
        diagInstance_ = cgi["diagInstance"]->getValue() ;


        /* Query ED IP */
//        diagIP_ = cgi["diagIP"]->getValue() ;
        /* Query ED port */
//        diagPort_ = cgi["diagPort"]->getValue() ;
        /* Query ED lid */
//        diagLID_ = cgi["diagLID"]->getValue() ;
        /* Query ED loglevel */
        edLogLevel_ = cgi["edloglevel"]->getValue();


        useSentinel_ = cgi.queryCheckbox("useSentinel");
        sentinelContextName_ = cgi["sentinelContextName"]->getValue() ;
        sentinelLogLevel_ = cgi["sentinelloglevel"]->getValue();
        useDefaultAppender_ = cgi.queryCheckbox("useDefaultAppender");
        useConsoleDump_ = cgi.queryCheckbox("useConsoleDump");

        useGlbDiag_ = cgi.queryCheckbox("useGlbDiag");
        edGlbLogLevel_ = cgi["edGlbloglevel"]->getValue();
        /* Query ED name */
        diagGlbName_ = cgi["diagGlbName"]->getValue() ;
        /* Query ED instance */
        diagGlbInstance_ = cgi["diagGlbInstance"]->getValue() ;


        /* Query ED IP */
//        diagGlbIP_ = cgi["diagGlbIP"]->getValue() ;
        /* Query ED port */
//        diagGlbPort_ = cgi["diagGlbPort"]->getValue() ;
        /* Query ED lid */
//        diagGlbLID_ = cgi["diagGlbLID"]->getValue() ;



		//Query the use error manager checkbox
		useLvl1_ = cgi.queryCheckbox("useLvl1");

		// Query EM name
		lvl1Name_ = cgi["lvl1Name"]->getValue() ;
		// Query EM instance
		lvl1Instance_ = cgi["lvl1Instance"]->getValue() ;


        #ifdef DEBUG_GET_FORM_VALUES
            std::cout << "useDiag_ : " << useDiag_ << std::endl;
            std::cout << "diagName_ : " << diagName_ << std::endl;
            std::cout << "diagInstance_ : " << diagInstance_ << std::endl;
            std::cout << "diagIP_ : " << diagIP_ << std::endl;
            std::cout << "diagPort_ : " << diagPort_ << std::endl;
            std::cout << "diagLID_ : " << diagLID_ << std::endl;
            std::cout << "edLogLevel_ : " << edLogLevel_ << std::endl;
            std::cout << std::endl << std::endl;
            std::cout << "useSentinel_ : " << useSentinel_ << std::endl;
            std::cout << "sentinelContextName_ : " << sentinelContextName_ << std::endl;
            std::cout << "sentinelLogLevel_ : " << sentinelLogLevel_ << std::endl;
			std::cout << "useLvl1_ : " << useLvl1_ << std::endl;
			std::cout << "lvl1Name_ : " << lvl1Name_ << std::endl;
			std::cout << "lvl1Instance_ : " << lvl1Instance_ << std::endl;
			std::cout << "lvl1IP_ : " << lvl1IP_ << std::endl;
			std::cout << "lvl1Port_ : " << lvl1Port_ << std::endl;
			std::cout << "lvl1LID_ : " << lvl1LID_ << std::endl;
        #endif
    }


    if ( this->getIsInErrorManager() == true )
    {
        /* Create a new Cgicc object containing all the CGI data */
        cgicc::Cgicc cgi(in);

        useDiagSystem_ = cgi["useDiagSystem"]->getValue();
        gLogLevel_ = cgi["gloglevel"]->getValue();

        /* Query the use error dispatcher checkbox */
        useDiag_ = cgi.queryCheckbox("useDiag");
        /* Query ED name */
        diagName_ = cgi["diagName"]->getValue() ;
        /* Query ED instance */
        diagInstance_ = cgi["diagInstance"]->getValue() ;

        /* Query ED IP */
//        diagIP_ = cgi["diagIP"]->getValue() ;
        /* Query ED port */
//        diagPort_ = cgi["diagPort"]->getValue() ;
        /* Query ED lid */
//        diagLID_ = cgi["diagLID"]->getValue() ;
        /* Query ED loglevel */
        edLogLevel_ = cgi["edloglevel"]->getValue();


        useSentinel_ = cgi.queryCheckbox("useSentinel");
        sentinelContextName_ = cgi["sentinelContextName"]->getValue() ;
        sentinelLogLevel_ = cgi["sentinelloglevel"]->getValue();
        useDefaultAppender_ = cgi.queryCheckbox("useDefaultAppender");
        useConsoleDump_ = cgi.queryCheckbox("useConsoleDump");

        useGlbDiag_ = cgi.queryCheckbox("useGlbDiag");
        edGlbLogLevel_ = cgi["edGlbloglevel"]->getValue();
        /* Query ED name */
        diagGlbName_ = cgi["diagGlbName"]->getValue() ;
        /* Query ED instance */
        diagGlbInstance_ = cgi["diagGlbInstance"]->getValue() ;

        /* Query ED IP */
//        diagGlbIP_ = cgi["diagGlbIP"]->getValue() ;
        /* Query ED port */
//        diagGlbPort_ = cgi["diagGlbPort"]->getValue() ;
        /* Query ED lid */
//        diagGlbLID_ = cgi["diagGlbLID"]->getValue() ;



		//Query the use error manager checkbox
		useLvl1_ = cgi.queryCheckbox("useLvl1");

		// Query EM name
		lvl1Name_ = cgi["lvl1Name"]->getValue() ;
		// Query EM instance
		lvl1Instance_ = cgi["lvl1Instance"]->getValue() ;


        #ifdef DEBUG_GET_FORM_VALUES
            std::cout << "useDiag_ : " << useDiag_ << std::endl;
            std::cout << "diagName_ : " << diagName_ << std::endl;
            std::cout << "diagInstance_ : " << diagInstance_ << std::endl;
            std::cout << "diagIP_ : " << diagIP_ << std::endl;
            std::cout << "diagPort_ : " << diagPort_ << std::endl;
            std::cout << "diagLID_ : " << diagLID_ << std::endl;
            std::cout << "edLogLevel_ : " << edLogLevel_ << std::endl;
            std::cout << std::endl << std::endl;
            std::cout << "useSentinel_ : " << useSentinel_ << std::endl;
            std::cout << "sentinelContextName_ : " << sentinelContextName_ << std::endl;
            std::cout << "sentinelLogLevel_ : " << sentinelLogLevel_ << std::endl;
			std::cout << "useLvl1_ : " << useLvl1_ << std::endl;
			std::cout << "lvl1Name_ : " << lvl1Name_ << std::endl;
			std::cout << "lvl1Instance_ : " << lvl1Instance_ << std::endl;
			std::cout << "lvl1IP_ : " << lvl1IP_ << std::endl;
			std::cout << "lvl1Port_ : " << lvl1Port_ << std::endl;
			std::cout << "lvl1LID_ : " << lvl1LID_ << std::endl;
        #endif
    }




    if (this->getIsInErrorDispatcher() == true)
    {
        // Create a new Cgicc object containing all the CGI data
        cgicc::Cgicc cgi(in);

        useDiagSystem_ = cgi["useDiagSystem"]->getValue();
        gLogLevel_ = cgi["gloglevel"]->getValue();

        /* Query the use error dispatcher checkbox */
        useDiag_ = cgi.queryCheckbox("useDiag");
        /* Query ED name */
        diagName_ = cgi["diagName"]->getValue() ;
        /* Query ED instance */
        diagInstance_ = cgi["diagInstance"]->getValue() ;


        /* Query ED IP */
//        diagIP_ = cgi["diagIP"]->getValue() ;
        /* Query ED port */
//        diagPort_ = cgi["diagPort"]->getValue() ;
        /* Query ED lid */
//        diagLID_ = cgi["diagLID"]->getValue() ;
        /* Query ED loglevel */
        edLogLevel_ = cgi["edloglevel"]->getValue();


        useSentinel_ = cgi.queryCheckbox("useSentinel");
        sentinelContextName_ = cgi["sentinelContextName"]->getValue() ;
        sentinelLogLevel_ = cgi["sentinelloglevel"]->getValue();
        useDefaultAppender_ = cgi.queryCheckbox("useDefaultAppender");
//        useConsoleDump_ = cgi.queryCheckbox("useConsoleDump");

        useGlbDiag_ = cgi.queryCheckbox("useGlbDiag");
        edGlbLogLevel_ = cgi["edGlbloglevel"]->getValue();
        /* Query ED name */
        diagGlbName_ = cgi["diagGlbName"]->getValue() ;
        /* Query ED instance */
        diagGlbInstance_ = cgi["diagGlbInstance"]->getValue() ;


        /* Query ED IP */
//        diagGlbIP_ = cgi["diagGlbIP"]->getValue() ;
        /* Query ED port */
//        diagGlbPort_ = cgi["diagGlbPort"]->getValue() ;
        /* Query ED lid */
//        diagGlbLID_ = cgi["diagGlbLID"]->getValue() ;



		//Query the use error manager checkbox
		useLvl1_ = cgi.queryCheckbox("useLvl1");

		// Query EM name
		lvl1Name_ = cgi["lvl1Name"]->getValue() ;
		// Query EM instance
		lvl1Instance_ = cgi["lvl1Instance"]->getValue() ;




for (int csStreamNum = 0; csStreamNum < nbrOfChainsawStreams_ ; csStreamNum++)
{
std::stringstream streamNb;
streamNb << csStreamNum;
std::string csRouteNumedName = "csRoute" + streamNb.str();
std::string csIPNumedName = "csIP" + streamNb.str();
std::string csPortNumedName = "csPort" + streamNb.str();
std::string csReceiverNumedName = "csReceiver" + streamNb.str();
std::string csLogLevelNumedName = "csLogLevel" + streamNb.str();
std::string csFilterNumedName = "csFilter" + streamNb.str();


        /* Get Chainsaw related informations */
        csRoute_[csStreamNum] = cgi.queryCheckbox(csRouteNumedName);
        csReceiver_[csStreamNum] = cgi[csReceiverNumedName]->getValue();
        csPort_[csStreamNum] = cgi[csPortNumedName]->getValue();
        csIP_[csStreamNum] = cgi[csIPNumedName]->getValue();
        csLogLevel_[csStreamNum] = cgi[csLogLevelNumedName]->getValue();
        csFilter_[csStreamNum] = cgi[csFilterNumedName]->getValue();
}


        /* Get Console related informations */
        ccRoute_ = cgi.queryCheckbox("ccRoute");
        ccLogLevel_ = cgi["ccloglevel"]->getValue();;


        /* Get Database related informations */
        dbRoute_ = cgi.queryCheckbox("dbRoute");
        dbLogLevel_ = cgi["dbloglevel"]->getValue();;
        dbFileName_ = cgi["dbFileName"]->getValue();

/*
        useFileLogging_ = cgi.queryCheckbox("useFileLogging");
        fileLogLevel_ = cgi["fileLogLevel"]->getValue();
        logFileName_ = cgi["logFileName"]->getValue();
*/

/*
        #ifdef DEBUG_GET_ED_FORM_VALUES
            std::cout << std::endl << std::endl;
            std::cout << "useDefaultAppender_ : " << useDefaultAppender_ << std::endl;
            std::cout << "useFileLogging_ : " << useFileLogging_ << std::endl;
            std::cout << "fileLogLevel_ : " << fileLogLevel_ << std::endl;
            std::cout << "logFileName_ : " << logFileName_ << std::endl;
for (int csStreamNum = 0; csStreamNum < nbrOfChainsawStreams_ ; csStreamNum++)
{

            std::cout << "csRoute_[" << csStreamNum << "] : " << csRoute_[csStreamNum] << std::endl;
            std::cout << "csPort_[" << csStreamNum << "] : " << csPort_[csStreamNum] << std::endl;
            std::cout << "csIP_[" << csStreamNum << "] : " << csIP_[csStreamNum] << std::endl;
            std::cout << "csReceiver_[" << csStreamNum << "] : " << csReceiver_[csStreamNum] << std::endl;
            std::cout << "csLogLevel_[" << csStreamNum << "] : " << csLogLevel_[csStreamNum] << std::endl;
            std::cout << "csFilter_[" << csStreamNum << "] : " << csFilter_[csStreamNum] << std::endl;
}

            std::cout << "ccRoute_ : " << ccRoute_ << std::endl;
            std::cout << "ccLogLevel_ : " << ccLogLevel_ << std::endl;
            std::cout << "dbRoute_ : " << dbRoute_ << std::endl;
            std::cout << "dbLogLevel_ : " << dbLogLevel_ << std::endl;
        #endif
*/
    }



    if (this->getIsInGlobalErrorDispatcher() == true)
    {
//std::cout << "Enetring in section getIsInGlobalErrorDispatcher" << std::endl;
        // Create a new Cgicc object containing all the CGI data
        cgicc::Cgicc cgi(in);

        useDiagSystem_ = cgi["useDiagSystem"]->getValue();
        gLogLevel_ = cgi["gloglevel"]->getValue();

        /* Query the use error dispatcher checkbox */
        useDiag_ = cgi.queryCheckbox("useDiag");
        /* Query ED name */
        diagName_ = cgi["diagName"]->getValue() ;
        /* Query ED instance */
        diagInstance_ = cgi["diagInstance"]->getValue() ;


        /* Query ED IP */
//        diagIP_ = cgi["diagIP"]->getValue() ;
        /* Query ED port */
//        diagPort_ = cgi["diagPort"]->getValue() ;
        /* Query ED lid */
//        diagLID_ = cgi["diagLID"]->getValue() ;
        /* Query ED loglevel */
        edLogLevel_ = cgi["edloglevel"]->getValue();


        useSentinel_ = cgi.queryCheckbox("useSentinel");
        sentinelContextName_ = cgi["sentinelContextName"]->getValue() ;
        sentinelLogLevel_ = cgi["sentinelloglevel"]->getValue();
        useDefaultAppender_ = cgi.queryCheckbox("useDefaultAppender");
//        useConsoleDump_ = cgi.queryCheckbox("useConsoleDump");

        useGlbDiag_ = cgi.queryCheckbox("useGlbDiag");
        edGlbLogLevel_ = cgi["edGlbloglevel"]->getValue();
        /* Query ED name */
        diagGlbName_ = cgi["diagGlbName"]->getValue() ;
        /* Query ED instance */
        diagGlbInstance_ = cgi["diagGlbInstance"]->getValue() ;

        /* Query ED IP */
//        diagGlbIP_ = cgi["diagGlbIP"]->getValue() ;
        /* Query ED port */
//        diagGlbPort_ = cgi["diagGlbPort"]->getValue() ;
        /* Query ED lid */
//        diagGlbLID_ = cgi["diagGlbLID"]->getValue() ;



		//Query the use error manager checkbox
		useLvl1_ = cgi.queryCheckbox("useLvl1");
		// Query EM name
		lvl1Name_ = cgi["lvl1Name"]->getValue() ;
		// Query EM instance
		lvl1Instance_ = cgi["lvl1Instance"]->getValue() ;

for (int csStreamNum = 0; csStreamNum < nbrOfChainsawStreams_ ; csStreamNum++)
{
std::stringstream streamNb;
streamNb << csStreamNum;
std::string csRouteNumedName = "csRoute" + streamNb.str();
std::string csIPNumedName = "csIP" + streamNb.str();
std::string csPortNumedName = "csPort" + streamNb.str();
std::string csReceiverNumedName = "csReceiver" + streamNb.str();
std::string csLogLevelNumedName = "csLogLevel" + streamNb.str();
std::string csFilterNumedName = "csFilter" + streamNb.str();


        /* Get Chainsaw related informations */
        csRoute_[csStreamNum] = cgi.queryCheckbox(csRouteNumedName);
        csReceiver_[csStreamNum] = cgi[csReceiverNumedName]->getValue();
        csPort_[csStreamNum] = cgi[csPortNumedName]->getValue();
        csIP_[csStreamNum] = cgi[csIPNumedName]->getValue();
        csLogLevel_[csStreamNum] = cgi[csLogLevelNumedName]->getValue();
        csFilter_[csStreamNum] = cgi[csFilterNumedName]->getValue();

}

		//If we are in GED AND CS env. settings are ok then override hyperdaq CS values for first CS occurence
if ( useCsEnvSettings_ == true )
{
	overrideCsSettings();
}

        // Get Console related informations
        ccRoute_ = cgi.queryCheckbox("ccRoute");
        ccLogLevel_ = cgi["ccloglevel"]->getValue();;


        // Get Database related informations
        dbRoute_ = cgi.queryCheckbox("dbRoute");
        dbLogLevel_ = cgi["dbloglevel"]->getValue();;
        dbFileName_ = cgi["dbFileName"]->getValue();

/*
        useFileLogging_ = cgi.queryCheckbox("useFileLogging");
        fileLogLevel_ = cgi["fileLogLevel"]->getValue();
        logFileName_ = cgi["logFileName"]->getValue();
*/

/*
        #ifdef DEBUG_GET_ED_FORM_VALUES
            std::cout << std::endl << std::endl;

            std::cout << "useDefaultAppender_ : " << useDefaultAppender_ << std::endl;
            std::cout << "useFileLogging_ : " << useFileLogging_ << std::endl;
            std::cout << "fileLogLevel_ : " << fileLogLevel_ << std::endl;
            std::cout << "logFileName_ : " << logFileName_ << std::endl;

for (int csStreamNum = 0; csStreamNum < nbrOfChainsawStreams_ ; csStreamNum++)
{

            std::cout << "csRoute_[" << csStreamNum << "] : " << csRoute_[csStreamNum] << std::endl;
            std::cout << "csPort_[" << csStreamNum << "] : " << csPort_[csStreamNum] << std::endl;
            std::cout << "csIP_[" << csStreamNum << "] : " << csIP_[csStreamNum] << std::endl;
            std::cout << "csReceiver_[" << csStreamNum << "] : " << csReceiver_[csStreamNum] << std::endl;
            std::cout << "csLogLevel_[" << csStreamNum << "] : " << csLogLevel_[csStreamNum] << std::endl;
            std::cout << "csFilter_[" << csStreamNum << "] : " << csFilter_[csStreamNum] << std::endl;
}

            std::cout << "ccRoute_ : " << ccRoute_ << std::endl;
            std::cout << "ccLogLevel_ : " << ccLogLevel_ << std::endl;
            std::cout << "dbRoute_ : " << dbRoute_ << std::endl;
            std::cout << "dbLogLevel_ : " << dbLogLevel_ << std::endl;

        #endif
*/
//std::cout << "Exiting in section getIsInGlobalErrorDispatcher" << std::endl;

    }



    if ( this->getIsInSentinelListener() == true )
    {
        // std::cout << "getIsInSentinelListener is TRUE" << std::endl;
        // Create a new Cgicc object containing all the CGI data
        cgicc::Cgicc cgi(in);

        useDiagSystem_ = cgi["useDiagSystem"]->getValue();
        gLogLevel_ = cgi["gloglevel"]->getValue();

        // Get Error Dispatcher related informations
        // Query the use error dispatcher checkbox
        useDiag_ = cgi.queryCheckbox("useDiag");

        /* Query ED name */
        diagName_ = cgi["diagName"]->getValue() ;
        /* Query ED instance */
        diagInstance_ = cgi["diagInstance"]->getValue() ;

        /* Query ED IP */
//        diagIP_ = cgi["diagIP"]->getValue() ;
        /* Query ED port */
//        diagPort_ = cgi["diagPort"]->getValue() ;
        /* Query ED lid */
//        diagLID_ = cgi["diagLID"]->getValue() ;


        useSentinel_ = cgi.queryCheckbox("useSentinel");
        sentinelContextName_ = cgi["sentinelContextName"]->getValue() ;
        sentinelLogLevel_ = cgi["sentinelloglevel"]->getValue();
        edLogLevel_ = cgi["edloglevel"]->getValue();
        useDefaultAppender_ = cgi.queryCheckbox("useDefaultAppender");
        useConsoleDump_ = cgi.queryCheckbox("useConsoleDump");
        grabberContextName_ = cgi["grabberContextName"]->getValue() ;
        useGrabber_ = cgi.queryCheckbox("useGrabber");



        useGlbDiag_ = cgi.queryCheckbox("useGlbDiag");
        edGlbLogLevel_ = cgi["edGlbloglevel"]->getValue();
        /* Query ED name */
        diagGlbName_ = cgi["diagGlbName"]->getValue() ;
        /* Query ED instance */
        diagGlbInstance_ = cgi["diagGlbInstance"]->getValue() ;


        /* Query ED IP */
//        diagGlbIP_ = cgi["diagGlbIP"]->getValue() ;
        /* Query ED port */
//        diagGlbPort_ = cgi["diagGlbPort"]->getValue() ;
        /* Query ED lid */
//        diagGlbLID_ = cgi["diagGlbLID"]->getValue() ;


        #ifdef DEBUG_GET_EM_FORM_VALUES
            std::cout << std::endl << std::endl;
            std::cout << "useDiag_ : " << useDiag_ << std::endl;
            std::cout << "diagName_ : " << diagName_ << std::endl;
            std::cout << "diagInstance_ : " << diagInstance_ << std::endl;
            std::cout << "diagIP_ : " << diagIP_ << std::endl;
            std::cout << "diagPort_ : " << diagPort_ << std::endl;
            std::cout << "diagLID_ : " << diagLID_ << std::endl;
            std::cout << "useSentinel_ : " << useSentinel_ << std::endl;
            std::cout << "sentinelContextName_ : " << sentinelContextName_ << std::endl;
            std::cout << "sentinelLogLevel_ : " << sentinelLogLevel_ << std::endl;
            std::cout << "edLogLevel_ : " << edLogLevel_ << std::endl;
            std::cout << "useDefaultAppender_ : " << useDefaultAppender_ << std::endl;
            std::cout << "useFileLogging_ : " << useFileLogging_ << std::endl;
            std::cout << "fileLogLevel_ : " << fileLogLevel_ << std::endl;
            std::cout << "logFileName_ : " << logFileName_ << std::endl;
            std::cout << "ccRoute_ : " << ccRoute_ << std::endl;
            std::cout << "ccLogLevel_ : " << ccLogLevel_ << std::endl;
            std::cout << "dbRoute_ : " << dbRoute_ << std::endl;
            std::cout << "dbLogLevel_ : " << dbLogLevel_ << std::endl;
        #endif
    }







    if ( this->getIsInLogsReader() == true )
    {
	
        /* Create a new Cgicc object containing all the CGI data */
        cgicc::Cgicc cgi(in);

        useDiagSystem_ = cgi["useDiagSystem"]->getValue();
        gLogLevel_ = cgi["gloglevel"]->getValue();

        /* Query the use error dispatcher checkbox */
        useDiag_ = cgi.queryCheckbox("useDiag");
        /* Query ED name */
        diagName_ = cgi["diagName"]->getValue() ;
        /* Query ED instance */
        diagInstance_ = cgi["diagInstance"]->getValue() ;


        /* Query ED IP */
//        diagIP_ = cgi["diagIP"]->getValue() ;
        /* Query ED port */
//        diagPort_ = cgi["diagPort"]->getValue() ;
        /* Query ED lid */
//        diagLID_ = cgi["diagLID"]->getValue() ;
        /* Query ED loglevel */
        edLogLevel_ = cgi["edloglevel"]->getValue();


        useSentinel_ = cgi.queryCheckbox("useSentinel");
        sentinelContextName_ = cgi["sentinelContextName"]->getValue() ;
        sentinelLogLevel_ = cgi["sentinelloglevel"]->getValue();
        useDefaultAppender_ = cgi.queryCheckbox("useDefaultAppender");
        useConsoleDump_ = cgi.queryCheckbox("useConsoleDump");

        useGlbDiag_ = cgi.queryCheckbox("useGlbDiag");
        edGlbLogLevel_ = cgi["edGlbloglevel"]->getValue();
        /* Query ED name */
        diagGlbName_ = cgi["diagGlbName"]->getValue() ;
        /* Query ED instance */
        diagGlbInstance_ = cgi["diagGlbInstance"]->getValue() ;

        /* Query ED IP */
//        diagGlbIP_ = cgi["diagGlbIP"]->getValue() ;
        /* Query ED port */
//        diagGlbPort_ = cgi["diagGlbPort"]->getValue() ;
        /* Query ED lid */
//        diagGlbLID_ = cgi["diagGlbLID"]->getValue() ;


        useFileLogging_ = cgi.queryCheckbox("useFileLogging");
        logFileName_ = cgi["logFileName"]->getValue();

		//Query the use error manager checkbox
		useLvl1_ = cgi.queryCheckbox("useLvl1");
		// Query EM name
		lvl1Name_ = cgi["lvl1Name"]->getValue() ;
		// Query EM instance
		lvl1Instance_ = cgi["lvl1Instance"]->getValue() ;


        #ifdef DEBUG_GET_FORM_VALUES
            std::cout << "useDiag_ : " << useDiag_ << std::endl;
            std::cout << "diagName_ : " << diagName_ << std::endl;
            std::cout << "diagInstance_ : " << diagInstance_ << std::endl;
            std::cout << "diagIP_ : " << diagIP_ << std::endl;
            std::cout << "diagPort_ : " << diagPort_ << std::endl;
            std::cout << "diagLID_ : " << diagLID_ << std::endl;
            std::cout << "edLogLevel_ : " << edLogLevel_ << std::endl;
            std::cout << "useSentinel_ : " << useSentinel_ << std::endl;
            std::cout << "sentinelContextName_ : " << sentinelContextName_ << std::endl;
            std::cout << "sentinelLogLevel_ : " << sentinelLogLevel_ << std::endl;
            std::cout << "logFileName_ : " << logFileName_ << std::endl;
        #endif
	}

return 0;
}


void DiagBagWizard::checkEmOptions()
{
    if (useReconf_ == true)
    {
        if ((useDiag_ == true) && (edUsable_ == true))
        {
            reconfUsable_ = false;
            xdaq::ApplicationDescriptor * d;
            try
            {
    	    	#ifdef XDAQ373
    	    	    d = wa_->getApplicationContext()->getDefaultZone()->getApplicationDescriptor(reconfName_, atoi(reconfInstance_.c_str()));
    	    	#else
    	    	    d = wa_->getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(reconfName_, atoi(reconfInstance_.c_str()));
    	    	#endif		
                reconfNameStatus_ = "checked";
                if (d->getLocalId() != (unsigned long)atol(reconfLID_.c_str()))
                {
                    std::stringstream errMsg;
                    errMsg << "&nbsp;&nbsp;Error : process " << reconfName_ << " with instance " << reconfInstance_ << " has LID " << d->getLocalId() << " and not " << reconfLID_ << std::endl;
                    reconfLIDStatus_ = errMsg.str();
                    reconfIPStatus_="unchecked";
                }
                else
                {
                    reconfLIDStatus_ = "checked";
                    std::string givenURL="http://" + reconfIP_ + ":" + reconfPort_;
                    if (givenURL != d->getContextDescriptor()->getURL())
                    {
                        std::stringstream errMsg;				
                        errMsg << "&nbsp;&nbsp;Error : process " << reconfName_ << " with instance " << reconfInstance_ << " has url " << d->getContextDescriptor()->getURL() << " and not " << givenURL << std::endl;
                        reconfIPStatus_ = errMsg.str();
                    }
                    else
                    {
                        reconfIPStatus_="checked";
                        reconfUsable_=true;
                        #ifdef DEBUG_APPLY_CONFIGURE_DUMMY_LVL_ONE
                            std::cout << "Reconfiguration loop closed and validated" << std::endl;
                        #endif
                    }
                }
            }
            catch (xdaq::exception::Exception& e)
            {
                reconfUsable_ = false;
                std::stringstream errMsg;
                errMsg << "ErrorInfo : process " << reconfName_ << " with instance " << reconfInstance_ << " does not exists" << std::endl;
                reconfNameStatus_ = errMsg.str();
                reconfLIDStatus_="unchecked";
                reconfIPStatus_="unchecked";
            }
        }
        else
        {
            reconfUsable_ = false;
            std::stringstream errMsg;
            errMsg << "ErrorInfo : You must enable the use of an Error Dispatcher before closing the reconfiguration loop." << std::endl;
            reconfNameStatus_ = errMsg.str();
            reconfLIDStatus_="unchecked";
            reconfIPStatus_="unchecked";
        }
    }
    else
    {
        reconfUsable_ = false;
        reconfNameStatus_ = "unchecked";
        reconfLIDStatus_="unchecked";
        reconfIPStatus_="unchecked";
    }
}



void DiagBagWizard::checkEdOptions()
{




//    #define DEBUG_APPLY_CONFIGURE_ED
    #ifdef DEBUG_APPLY_CONFIGURE_ED
        std::cout << "Entering DiagBagWizard::checkEdOptions()" << std::endl;
for (int csStreamNum = 0; csStreamNum < nbrOfChainsawStreams_ ; csStreamNum++)
{
        std::cout << "csRoute_[" << csStreamNum << "] == " << csRoute_[csStreamNum] << std::endl;
        if (csRoute_[csStreamNum]==true) std::cout << "csRoute_[" << csStreamNum << "]==true" << std::endl;
        if (csRoute_[csStreamNum]==false) std::cout << "csRoute_[" << csStreamNum << "]==false" << std::endl;
        std::cout << "socketXMLExists_[" << csStreamNum << "] == " << socketXMLExists_[csStreamNum] << std::endl;
        if (socketXMLExists_[csStreamNum] == true) std::cout << "socketXMLExists_[" << csStreamNum << "]==true" << std::endl;
        if (socketXMLExists_[csStreamNum] == false) std::cout << "socketXMLExists_[" << csStreamNum << "]==false" << std::endl;
}
    #endif









for (int csStreamNum = 0; csStreamNum < nbrOfChainsawStreams_ ; csStreamNum++)
{

   /* Check consistency of informations for CHAINSAW routing */
    if (csRoute_[csStreamNum]==true)
    {

/*
        if (useDefaultAppender_==true)
        {
            stringstream errMsg;				
            errMsg << "&nbsp;&nbsp;Error : You can not route error messages to chainsaw when using the Errror Dispatcher Default Logger" << std::endl;
            csReceiverStatus_[0] = errMsg.str();
            csIPStatus_[0]="unchecked";
            csPortStatus_[0]="unchecked";
            csRouteValidated_=false;
        }
        else
        {
*/
            /* If this is our first socket creation, */
            if (oldFirstTime_[csStreamNum] == true)
            {

                oldCsIP_[csStreamNum] = csIP_[csStreamNum];
                oldCsPort_[csStreamNum] = csPort_[csStreamNum];
                oldCsReceiver_[csStreamNum] = csReceiver_[csStreamNum];
                oldFirstTime_[csStreamNum] = false;

                #ifdef DEBUG_APPLY_CONFIGURE_ED
                    std::cout << "At first time creation" << std::endl;
                    std::cout << "    csIP_[" << csStreamNum << "] = " << csIP_[csStreamNum] <<std::endl;
                    std::cout << "    oldCsIP_[" << csStreamNum << "] = " << oldCsIP_[csStreamNum] <<std::endl;
                    std::cout << "    csPort_[" << csStreamNum << "] = " << csPort_[csStreamNum] <<std::endl;
                    std::cout << "    oldCsPort_[" << csStreamNum << "] = " << oldCsPort_[csStreamNum] <<std::endl;
                    std::cout << "    csReceiver_[" << csStreamNum << "] = " << csReceiver_[csStreamNum] <<std::endl;
                    std::cout << "    oldCsReceiver_[" << csStreamNum << "] = " << oldCsReceiver_[csStreamNum] <<std::endl;
                #endif

            }

            if ( ((csIP_[csStreamNum] != oldCsIP_[csStreamNum]) || (csPort_[csStreamNum] != oldCsPort_[csStreamNum]) || (csReceiver_[csStreamNum] != oldCsReceiver_[csStreamNum]) ) && (socketXMLExists_[csStreamNum] == true) )
            {

                #ifdef DEBUG_APPLY_CONFIGURE_ED
                    std::cout << "Deleting XML appender." << std::endl;
                    std::cout << "    csIP_[" << csStreamNum << "] = " << csIP_[csStreamNum] <<std::endl;
                    std::cout << "    oldCsIP_[" << csStreamNum << "] = " << oldCsIP_[csStreamNum] <<std::endl;
                    std::cout << "    csPort_[" << csStreamNum << "] = " << csPort_[csStreamNum] <<std::endl;
                    std::cout << "    oldCsPort_[" << csStreamNum << "] = " << oldCsPort_[csStreamNum] <<std::endl;
                    std::cout << "    csReceiver_[" << csStreamNum << "] = " << csReceiver_[csStreamNum] <<std::endl;
                    std::cout << "    oldCsReceiver_[" << csStreamNum << "] = " << oldCsReceiver_[csStreamNum] <<std::endl;
              #endif

				chainSawManager_->removeStream(oldCsReceiver_[csStreamNum]);



int lcl_counter = csStreamNum;


while (lcl_counter < (nbrOfChainsawStreams_-1) )
{

csLogLevel_[lcl_counter] = csLogLevel_[lcl_counter+1];
csRoute_[lcl_counter] = csRoute_[lcl_counter+1];
csRouteValidated_[lcl_counter] = csRouteValidated_[lcl_counter+1];
csRouteStatus_[lcl_counter] = csRouteStatus_[lcl_counter+1];
csReceiver_[lcl_counter] = csReceiver_[lcl_counter+1];
csReceiverStatus_[lcl_counter] = csReceiverStatus_[lcl_counter+1];
csIPStatus_[lcl_counter] = csIPStatus_[lcl_counter+1];
csIP_[lcl_counter] = csIP_[lcl_counter+1];
csPortStatus_[lcl_counter] = csPortStatus_[lcl_counter+1];
csPort_[lcl_counter] = csPort_[lcl_counter+1];
oldCsIP_[lcl_counter] = oldCsIP_[lcl_counter+1];
oldCsPort_[lcl_counter] = oldCsPort_[lcl_counter+1];
oldCsReceiver_[lcl_counter] = oldCsReceiver_[lcl_counter+1];
oldFirstTime_[lcl_counter] = oldFirstTime_[lcl_counter+1];
socketXMLExists_[lcl_counter] = socketXMLExists_[lcl_counter+1];
csFilter_[lcl_counter] = csFilter_[lcl_counter+1];
lcl_counter++;
}


			//free(chainsawAppender_);
				
                socketXMLExists_[csStreamNum] = false;
                oldCsIP_[csStreamNum] = csIP_[csStreamNum];
                oldCsPort_[csStreamNum] = csPort_[csStreamNum];
                oldCsReceiver_[csStreamNum] = csReceiver_[csStreamNum];

            }




            if (socketXMLExists_[csStreamNum] == false)
            {

                /* create default XML appender, but don't use it as Log4C+ router. We will rather attack it in direct mode */

                #ifdef DEBUG_APPLY_CONFIGURE_ED
                    std::cout << "Creating XML appender for routing messages to Chainsaw." << std::endl;
                    std::cout << "    csIP_[" << csStreamNum << "] = " << csIP_[csStreamNum] <<std::endl;
                    std::cout << "    oldCsIP_[" << csStreamNum << "] = " << oldCsIP_[csStreamNum] <<std::endl;
                    std::cout << "    csPort_[" << csStreamNum << "] = " << csPort_[csStreamNum] <<std::endl;
                    std::cout << "    oldCsPort_[" << csStreamNum << "] = " << oldCsPort_[csStreamNum] <<std::endl;
                    std::cout << "    csReceiver_[" << csStreamNum << "] = " << csReceiver_[csStreamNum] <<std::endl;
                    std::cout << "    oldCsReceiver_[" << csStreamNum << "] = " << oldCsReceiver_[csStreamNum] <<std::endl;
                #endif


                struct hostent * host_ = gethostbyname( csIP_[csStreamNum].c_str() );
                std::string csMainIP;

                if (host_ != NULL)
                {

                    csMainIP = (std::string)inet_ntoa(*((struct in_addr * )host_->h_addr_list[0])) ;

                }
				else csMainIP = csIP_[csStreamNum];

				chainSawManager_->addStream(csMainIP, csPort_[csStreamNum], csReceiver_[csStreamNum], csReceiver_[csStreamNum]);
                //chainsawAppender_ = new XMLDiagAppender(LOG4CPLUS_TEXT(csMainIP), atoi(csPort_[csStreamNum].c_str()), LOG4CPLUS_TEXT(csReceiver_[csStreamNum]));

                socketXMLExists_[csStreamNum] = true;

            }

            csIPStatus_[csStreamNum]="checked";
            csPortStatus_[csStreamNum]="checked";
            csReceiverStatus_[csStreamNum]="checked";
            csRouteValidated_[csStreamNum]=true;
/*        }
*/

    }
    else
    {

        csRouteValidated_[csStreamNum]=false;
        csIPStatus_[csStreamNum]="unchecked";
        csPortStatus_[csStreamNum]="unchecked";
        csReceiverStatus_[csStreamNum]="unchecked";
    }


}


//std::cout << "Now in GED check ; useFileLogging_ == " << useFileLogging_ << std::endl;


    // Check consistency of informations for FILE routing
/*
    if (useFileLogging_==true)
    {
            if (logFileName_ == "")
            {
                std::stringstream errMsg;				
                errMsg << "&nbsp;&nbsp;Error : You must provide a Path & FileName for the ErrorsLogging destination file" << std::endl;
                fileLoggingStatus_ = errMsg.str();
                fileLoggingUsable_ = false;
            }
            else
            {
                FILE * fileOut;
				if (logFileName_ != oldLogFileName_)
				{
					logFileNameHasChanged_ = true;

	                fileOut = fopen(logFileName_.c_str(), "a");
    	            if (fileOut == NULL)
        	        {
            	        std::stringstream errMsg;				
                	    errMsg << "Error : Unable to open/create destination file \"" << logFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
						std::cout << errMsg.str() << std::endl;
                    	fileLoggingStatus_ = errMsg.str();
	                    fileLoggingUsable_ = false;
    	            }
        	        else
            	    {
                	    fclose(fileOut);
                    	fileLoggingStatus_="checked";
	                    fileLoggingUsable_=true;
    	            }
					
					oldLogFileName_ = logFileName_;
				}
				else
				{
					logFileNameHasChanged_ = false;
					fileLoggingStatus_="checked";
	                fileLoggingUsable_=true;

				}
				
            }
    }
    else
    {
        fileLoggingUsable_=false;
        fileLoggingStatus_ = "unchecked";
    }
*/




    /* Check consistency of informations for CONSOLE routing */
    if (ccRoute_==true)
    {
/*
        if (useDefaultAppender_==true)
        {
            stringstream errMsg;				
            errMsg << "&nbsp;&nbsp;Error : You can not route error messages to console when using the Errror Disaptcher Default Logger" << std::endl;
            ccRouteStatus_ = errMsg.str();
            ccRouteValidated_ = false;
        }
        else
        {
*/
            ccRouteStatus_="checked";
            ccRouteValidated_=true;
/*        }
*/
    }
    else
    {
        ccRouteValidated_=false;
        ccRouteStatus_ = "unchecked";
    }





    /* Check consistency of informations for DB routing */
    if (dbRoute_==true)
    {
//std::cout << "AAAAAAAAAAAA" << std::endl;

            if (dbFileName_ == "")
            {
//std::cout << "BBBBBBBBB" << std::endl;

                std::stringstream errMsg;				
                errMsg << "&nbsp;&nbsp;Error : You must provide a Path & FileName for the DB swap file" << std::endl;
                dbRouteStatus_ = errMsg.str();
                dbRouteValidated_ = false;
            }
            else
            {
//std::cout << "CCCCCCCCCCCCCC" << std::endl;

                FILE * fileOut;
				if (dbFileName_ != oldDbFileName_)
				{
//std::cout << "DDDDDDDDDDDDDd" << std::endl;

					dbFileNameHasChanged_ = true;

	                fileOut = fopen(dbFileName_.c_str(), "a");
    	            if (fileOut == NULL)
        	        {
//std::cout << "FFFFFFFFFFFFFFF" << std::endl;

            	        std::stringstream errMsg;				
                	    errMsg << "Error : Unable to open/create destination file \"" << dbFileName_.c_str() << "\". Maybe a permissions problem?" << std::endl;
						std::cout << errMsg.str() << std::endl;
                    	dbRouteStatus_ = errMsg.str();
	                    dbRouteValidated_ = false;
    	            }
        	        else
            	    {
//std::cout << "GGGGGGGGGGGGGGGG" << std::endl;

                	    fclose(fileOut);
                    	dbRouteStatus_="checked";
	                    dbRouteValidated_=true;
    	            }
					
					oldDbFileName_ = dbFileName_;
				}
				else
				{
					dbFileNameHasChanged_ = false;
                   	dbRouteStatus_="checked";
                    dbRouteValidated_=true;
//std::cout << "EEEEEEEEEEEEEEEe" << std::endl;
				}

				
            }


    }
    else
    {
        dbRouteValidated_=false;
        dbRouteStatus_ = "unchecked";
    }



}




/*
void DiagBagWizard::checkLrOptions()
{
    if (useFileLogging_==true)
    {

            if (logFileName_ == "")
            {
                std::stringstream errMsg;				
                errMsg << "&nbsp;&nbsp;Error : You must provide a Path & FileName for the Logs source file" << std::endl;
                fileLoggingStatus_ = errMsg.str();
                fileLoggingUsable_ = false;
             }
            else
            {
                FILE * fileOut;
				if (logFileName_ != oldLogFileName_)
				{
					logFileNameHasChanged_ = true;

	                fileOut = fopen(logFileName_.c_str(), "r");
    	            if (fileOut == NULL)
        	        {
            	        std::stringstream errMsg;				
                	    errMsg << "Error : Unable to open log file \"" << logFileName_.c_str() << "\" in read mode. Maybe a permissions problem?" << std::endl;
						std::cout << errMsg.str() << std::endl;
                    	fileLoggingStatus_ = errMsg.str();
	                    fileLoggingUsable_ = false;
    	            }
        	        else
            	    {
                	    fclose(fileOut);
                    	fileLoggingStatus_="checked";
	                    fileLoggingUsable_=true;
    	            }
					
					oldLogFileName_ = logFileName_;
				}
				else logFileNameHasChanged_ = false;
				
            }

    }
    else
    {
		fileLoggingStatus_="checked";
		fileLoggingUsable_=true;
    }
}
*/


void DiagBagWizard::checkLrFileOption()
{
                FILE * fileOut;
				if (logFileName_ != oldLogFileName_)
				{
					logFileNameHasChanged_ = true;

	                fileOut = fopen(logFileName_.c_str(), "r");
    	            if (fileOut == NULL)
        	        {
            	        std::stringstream errMsg;				
                	    errMsg << "Error : Unable to open log file \"" << logFileName_.c_str() << "\" in read mode. Maybe a permissions problem?" << std::endl;
						std::cout << errMsg.str() << std::endl;
                    	fileLoggingStatus_ = errMsg.str();
	                    fileLoggingUsable_ = false;
    	            }
        	        else
            	    {
                	    fclose(fileOut);
                    	fileLoggingStatus_="checked";
	                    fileLoggingUsable_=true;
    	            }
					
					oldLogFileName_ = logFileName_;
				}
				else logFileNameHasChanged_ = false;
}














bool DiagBagWizard::checkUseDiag()
{
//#define DEBUG_CHECK_USE_DIAG
    if (useDiag_ == false)
    {
        #ifdef DEBUG_CHECK_USE_DIAG
            std::cout << "in DiagBagWizard::checkUseDiag() : Usage of Diag NOT requested." << std::endl;
        #endif
        edUsable_=false;
        useDiagStatus_ = "unchecked";
        diagNameStatus_="unchecked";
        diagLIDStatus_="unchecked";
        diagIPStatus_="unchecked";
        return false;
    }
    else 
    {
        #ifdef DEBUG_CHECK_USE_DIAG
            std::cout << "in DiagBagWizard::checkUseDiag() : Usage of Diag requested." << std::endl;
        #endif
        useDiagStatus_ = "checked";
        edUsable_ = true;
        return true;
    }
}


bool DiagBagWizard::checkUseGlbDiag()
{
//#define DEBUG_CHECK_USE_GLB_DIAG
    if (useGlbDiag_ == false)
    {
        #ifdef DEBUG_CHECK_USE_GLB_DIAG
            std::cout << "in DiagBagWizard::checkUseGlbDiag() : Usage of GLB-Diag NOT requested." << std::endl;
        #endif
        edGlbUsable_=false;
        useGlbDiagStatus_ = "unchecked";
        diagGlbNameStatus_="unchecked";
        diagGlbLIDStatus_="unchecked";
        diagGlbIPStatus_="unchecked";
        return false;
    }
    else 
    {
        #ifdef DEBUG_CHECK_USE_GLB_DIAG
            std::cout << "in DiagBagWizard::checkUseGlbDiag() : Usage of GLB-Diag requested." << std::endl;
        #endif
        useGlbDiagStatus_ = "checked";
        edGlbUsable_ = true;
        return true;
    }
}



bool DiagBagWizard::setUseDiagOnName(bool usediag)
{
//#define DEBUG_SET_USE_DIAG_ON_NAME
    if ( usediag == true)
    {
        #ifdef DEBUG_SET_USE_DIAG_ON_NAME
            std::cout << "in DiagBagWizard::setUseDiagOnName() : name/instance detected" << std::endl;
        #endif
        edUsable_ = true;
        diagNameStatus_ = "checked";
        return true;
    }
    else
    {
        #ifdef DEBUG_SET_USE_DIAG_ON_NAME
            std::cout << "in DiagBagWizard::setUseDiagOnName() : name/instance NOT detected" << std::endl;
        #endif
        edUsable_ = false;
        std::stringstream errMsg;
        errMsg << "ErrorInfo : process " << diagName_ << " with instance " << diagInstance_ << " does not exists" << std::endl;
        diagNameStatus_ = errMsg.str();
        diagLIDStatus_="unchecked";
        diagIPStatus_="unchecked";
        return false;
    }
}



bool DiagBagWizard::setUseGlbDiagOnName(bool usediag)
{
//#define DEBUG_SET_USE_GLB_DIAG_ON_NAME
    if ( usediag == true)
    {
        #ifdef DEBUG_SET_USE_GLB_DIAG_ON_NAME
            std::cout << "in DiagBagWizard::setUseGlbDiagOnName() : name/instance detected" << std::endl;
        #endif
        edGlbUsable_ = true;
        diagGlbNameStatus_ = "checked";
        return true;
    }
    else
    {
        #ifdef DEBUG_SET_USE_GLB_DIAG_ON_NAME
            std::cout << "in DiagBagWizard::setUseGlbDiagOnName() : name/instance NOT detected" << std::endl;
        #endif
        edGlbUsable_ = false;
        std::stringstream errMsg;
        errMsg << "ErrorInfo : process " << diagGlbName_ << " with instance " << diagGlbInstance_ << " does not exists" << std::endl;
        diagGlbNameStatus_ = errMsg.str();
        diagGlbLIDStatus_="unchecked";
        diagGlbIPStatus_="unchecked";
        return false;
    }
}


bool DiagBagWizard::setUseDiagOnLid(unsigned long proc_lid)
{
//MTCCCMOD
/*
    if (proc_lid != (unsigned long)atol(diagLID_.c_str()))
    {
        std::stringstream errMsg;
        errMsg << "&nbsp;&nbsp;Error : process " << diagName_ << " with instance " << diagInstance_ << " has LID " << proc_lid << " and not " << diagLID_ << std::endl;
        diagLIDStatus_ = errMsg.str();
        diagIPStatus_="unchecked";
        edUsable_ = false;
        return false;
    }
    else
    {
        diagLIDStatus_ = "checked";
        return true;
    }
*/
    diagLIDStatus_ = "checked";
    return true;
}



bool DiagBagWizard::setUseGlbDiagOnLid(unsigned long proc_lid)
{
    diagGlbLIDStatus_ = "checked";
    return true;
}




bool DiagBagWizard::setUseDiagOnUrl(std::string l_url)
{

    std::string givenURL="http://" + diagIP_ + ":" + diagPort_;
    std::string lcl_IP;
    unsigned int i = 7;
    while ( (l_url[i] != ':') && (i < strlen(l_url.c_str())) )
    {
        lcl_IP = lcl_IP + l_url[i];
        i++;
    }
    std::string IPBuiltFromContext="";
    struct hostent * host_ = gethostbyname( lcl_IP.c_str() );
    if (host_ != NULL)
    {
        IPBuiltFromContext = (std::string)inet_ntoa(*((struct in_addr * )host_->h_addr_list[0]));
    }
    // std::cout << "IP built from context : " << IPBuiltFromContext << std::endl;

    std::string IPBuiltFromInput="";
    struct hostent * host1_ = gethostbyname( diagIP_.c_str() );
    if (host1_ != NULL)
    {
        IPBuiltFromInput = (std::string)inet_ntoa(*((struct in_addr * )host1_->h_addr_list[0]));
    }
    // std::cout << "IP built from Input : " << IPBuiltFromInput << std::endl;

    if ( (IPBuiltFromContext != IPBuiltFromInput) || (IPBuiltFromContext == "") || (IPBuiltFromInput == "") )
    {
        std::stringstream errMsg;				
        errMsg << "&nbsp;&nbsp;Error : process " << diagName_ << " with instance " << diagInstance_ << " has url " << l_url << " and not " << givenURL << std::endl;
        diagIPStatus_ = errMsg.str();
        edUsable_=false;
        return false;
    }
    else
    {
        diagIPStatus_="checked";
        return true;
    }
/*
        diagIPStatus_="checked";
        return true;
*/
}



bool DiagBagWizard::setUseGlbDiagOnUrl(std::string l_url)
{

    std::string givenURL="http://" + diagGlbIP_ + ":" + diagGlbPort_;
    std::string lcl_IP;
    unsigned int i = 7;
    while ( (l_url[i] != ':') && (i < strlen(l_url.c_str())) )
    {
        lcl_IP = lcl_IP + l_url[i];
        i++;
    }
    std::string IPBuiltFromContext="";
    struct hostent * host_ = gethostbyname( lcl_IP.c_str() );
    if (host_ != NULL)
    {
        IPBuiltFromContext = (std::string)inet_ntoa(*((struct in_addr * )host_->h_addr_list[0]));
    }
     //std::cout << "IP built from context : " << IPBuiltFromContext << std::endl;

    std::string IPBuiltFromInput="";
    struct hostent * host1_ = gethostbyname( diagGlbIP_.c_str() );
    if (host1_ != NULL)
    {
        IPBuiltFromInput = (std::string)inet_ntoa(*((struct in_addr * )host1_->h_addr_list[0]));
    }
     //std::cout << "IP built from Input : " << IPBuiltFromInput << std::endl;

    if ( (IPBuiltFromContext != IPBuiltFromInput) || (IPBuiltFromContext == "") || (IPBuiltFromInput == "") )
    {
        std::stringstream errMsg;				
        errMsg << "&nbsp;&nbsp;Error : process " << diagGlbName_ << " with instance " << diagGlbInstance_ << " has url " << l_url << " and not " << givenURL << std::endl;
        diagGlbIPStatus_ = errMsg.str();
        edGlbUsable_=false;
        return false;
    }
    else
    {
        diagGlbIPStatus_="checked";
        return true;
    }

/*
        diagGlbIPStatus_="checked";
        return true;
*/
}





bool DiagBagWizard::checkUseLvl1()
{
    //#define DEBUG_CHECK_USE_LVL1
    /* cout << "in DiagBagWizard::checkUseLvl1() : Usage of Lvl1 requested." << std::endl; */

    if (useLvl1_ == false)
    {
        #ifdef DEBUG_CHECK_USE_LVL1
            std::cout << "in DiagBagWizard::checkUseLvl1() : Usage of Lvl1 requested." << std::endl;
        #endif
        emUsable_=false;
        lvl1NameStatus_="unchecked";
        lvl1LIDStatus_="unchecked";
        lvl1IPStatus_="unchecked";
        return false;
    }
    else
    {
/*
        if ((useDiag_ == false) || (edUsable_==false))
        {
            if (sentinelUseRelayedLvl1_ == false)
            {
                emUsable_=false;
                lvl1NameStatus_="unchecked";
                lvl1LIDStatus_="unchecked";
                lvl1IPStatus_="unchecked";
                std::stringstream errMsg;
                errMsg << "Error : You can't use an Error Manager without setting up an Error Dispatcher" << std::endl;
                useLvl1Status_ = errMsg.str();
                return false;
            }
            else
            {
                emUsable_ = true;
                lvl1NameStatus_ = "unchecked";
                lvl1LIDStatus_ = "unchecked";
                lvl1IPStatus_ = "unchecked";
                return true;
            }
        }
        else
        {
*/
            useLvl1Status_ = "checked";
            return true;
/*        }				 */
    }
}




bool DiagBagWizard::setUseLvl1OnName(bool uselvl1)
{
    //#define DEBUG_SET_USE_LVL1_ON_NAME
    if ( uselvl1 == true)
    {
        #ifdef DEBUG_SET_USE_LVL1_ON_NAME
            std::cout << "in DiagBagWizard::setUseLvl1OnName() : name/instance detected" << std::endl;
        #endif
        emUsable_ = true;
        lvl1NameStatus_ = "checked";
        return true;
    }
    else
    {
        #ifdef DEBUG_SET_USE_LVL1_ON_NAME
            std::cout << "in DiagBagWizard::setUseLvl1OnName() : name/instance NOT detected" << std::endl;
        #endif
        emUsable_ = false;
        std::stringstream errMsg;
        errMsg << "ErrorInfo : process " << lvl1Name_ << " with instance " << lvl1Instance_ << " does not exists" << std::endl;
        lvl1NameStatus_ = errMsg.str();
        lvl1LIDStatus_="unchecked";
        lvl1IPStatus_="unchecked";
        useLvl1Status_ = "unchecked";
        return false;
    }
}



bool DiagBagWizard::setUseLvl1OnLid(unsigned long proc_lid)
{
    //MTCCCMOD
    /*
    if (proc_lid != (unsigned long)atol(lvl1LID_.c_str()))
    {
        std::stringstream errMsg;
        errMsg << "&nbsp;&nbsp;Error : process " << lvl1Name_ << " with instance " << lvl1Instance_ << " has LID " << proc_lid << " and not " << lvl1LID_ << std::endl;
        lvl1LIDStatus_ = errMsg.str();
        lvl1IPStatus_="unchecked";
        useLvl1Status_ = "unchecked";
        emUsable_ = false;
        return false;
    }
    else
    {
        lvl1LIDStatus_ = "checked";
        return true;
    }
    */
    lvl1LIDStatus_ = "checked";
    return true;

}



bool DiagBagWizard::setUseLvl1OnUrl(std::string l_url)
{

    std::string givenURL="http://" + lvl1IP_ + ":" + lvl1Port_;
    std::string lcl_IP;
    unsigned int i = 7;
    while ( (l_url[i] != ':') && (i < strlen(l_url.c_str())) )
    {
        lcl_IP = lcl_IP + l_url[i];
        i++;
    }
    std::string IPBuiltFromContext="";
    struct hostent * host_ = gethostbyname( lcl_IP.c_str() );
    if (host_ != NULL)
    {
        IPBuiltFromContext = (std::string)inet_ntoa(*((struct in_addr * )host_->h_addr_list[0]));
    }
    // std::cout << "IP built from context : " << IPBuiltFromContext << std::endl;

    std::string IPBuiltFromInput="";
    struct hostent * host1_ = gethostbyname( lvl1IP_.c_str() );
    if (host1_ != NULL)
    {
        IPBuiltFromInput = (std::string)inet_ntoa(*((struct in_addr * )host1_->h_addr_list[0]));
    }
    // std::cout << "IP built from Input : " << IPBuiltFromInput << std::endl;

    if ( (IPBuiltFromContext != IPBuiltFromInput) || (IPBuiltFromContext == "") || (IPBuiltFromInput == "") )
    {
        std::stringstream errMsg;				
        errMsg << "&nbsp;&nbsp;Error : process " << lvl1Name_ << " with instance " << lvl1Instance_ << " has url " << l_url << " and not " << givenURL << std::endl;
        lvl1IPStatus_ = errMsg.str();
        useLvl1Status_ = "unchecked";
        emUsable_=false;
        return false;
    }
    else
    {
        lvl1IPStatus_="checked";
        return true;
    }
/*
        lvl1IPStatus_="checked";
        return true;
*/
}



bool DiagBagWizard::checkUseSentinel()
{    
    if (useSentinel_ == true)
    {
		sentinelUsableStatus_ = "checked";
		sentinelUsable_ = true;
		return true;

    }
    else
    {
        sentinelUsable_ = false;
        sentinelUsableStatus_ = "unchecked";
        return false;
    }
}




bool DiagBagWizard::checkUseSentinelGrabber()
{
	grabberUsable_ = false;
	grabberUsableStatus_ = "unchecked";
	return false;
}




bool DiagBagWizard::checkRelayedEdStatus()
{
	relayedEdStatus_="unchecked";
	relayedEdUsable_ = false;
	return false;
}



bool DiagBagWizard::checkRelayedLvl1Status()
{
	relayedLvl1Status_ = "unchecked";
	relayedEmUsable_ = false;
	return false;
}



bool DiagBagWizard::checkUseDefaultAppender()
{
    if (useDefaultAppender_ == true)
    {
        defaultAppenderUsable_ = true;
        defaultAppenderStatus_ = "checked";
        return true;
    }
    else
    {
        defaultAppenderStatus_ = "unchecked";
        defaultAppenderUsable_ = false;
        return false;
    }
}


bool DiagBagWizard::checkUseEd()
{
//    bool haveAnError;
    bool lcl_use_diag=true;
    if (this->checkUseDiag() == true)
    {
        //xdaq::ApplicationDescriptor * d;
        try
        {
/*
    	    #ifdef XDAQ373
    			d = wa_->getApplicationContext()->getDefaultZone()->getApplicationDescriptor(diagName_, atoi(diagInstance_.c_str()));
    	    #else
		    	d = wa_->getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(diagName_, atoi(diagInstance_.c_str()));
			#endif
*/
            lcl_use_diag = this->setUseDiagOnName(lcl_use_diag);
			
/*
			
			
            if (lcl_use_diag == true)
            {
                lcl_use_diag = this->setUseDiagOnLid(d->getLocalId());
                if (lcl_use_diag == true)
                {
                    lcl_use_diag = this->setUseDiagOnUrl(d->getContextDescriptor()->getURL());
                    #ifdef USE_XRELAY
                        if (lcl_use_diag == true)
                        {
                            lclEdURL_ = d->getContextDescriptor()->getURL();
                            lclEdURN_ = d->getURN();
                        }
                    #endif
                }
            }
			
*/
        }
        catch (xdaq::exception::Exception& e)
        {
            lcl_use_diag=false;
            this->setUseDiagOnName(lcl_use_diag);
            //haveAnError = true;
        }
        //if (lcl_use_diag == false) haveAnError = true;
    }
    else lcl_use_diag=false;
/*
    if (lcl_use_diag == true)
    {
        if ((this->getDiagIP() != this->getOldDiagIP()) ||
            (this->getDiagPort() != this->getOldDiagPort()) ||
            (this->getDiagName() != this->getOldDiagName()) ||
            (this->getDiagInstance() != this->getOldDiagInstance()) ||
            (this->getDiagLID() != this->getOldDiagLID()) )
        {
            this->setSocketSoapExists(false);
            this->setOldDiagIP(this->getDiagIP());
            this->setOldDiagPort(this->getDiagPort()); 
            this->setOldDiagName(this->getDiagName());
            this->setOldDiagInstance(this->getDiagInstance());
            this->setOldDiagLID(this->getDiagLID());
        }
        if (this->getSocketSoapExists() == false)
        {
            this->setSocketSoapExists(true);
        }

    }
    else
    {
        if (this->getSocketSoapExists() == true)
        {
            this->setSocketSoapExists(false);
            //route to console
        }
    }
*/
return 0;
}




bool DiagBagWizard::checkUseGlbEd()
{
	
    //bool haveAnError;
    bool lcl_use_diag=true;
    if (this->checkUseGlbDiag() == true)
    {
        //std::cout << "In checkUseGlbEd :: checkUseGlbDiag() is true" << std::endl;
        //xdaq::ApplicationDescriptor * d;
        try
        {
/*
   	    	#ifdef XDAQ373
   	    	    d = wa_->getApplicationContext()->getDefaultZone()->getApplicationDescriptor(diagGlbName_, atoi(diagGlbInstance_.c_str()));
   	    	#else
			    d = wa_->getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(diagGlbName_, atoi(diagGlbInstance_.c_str()));
			#endif
*/
            //std::cout << "In checkUseGlbEd :: Get AppDesc. SUCCESS for process:" << diagGlbName_ << " Instance:" << diagGlbInstance_ << std::endl;


            lcl_use_diag = this->setUseGlbDiagOnName(lcl_use_diag);
            //std::cout << "In checkUseGlbEd :: setUseGlbDiagOnName() has returned true" << std::endl;

/*
            if (lcl_use_diag == true)
            {
                lcl_use_diag = this->setUseGlbDiagOnLid(d->getLocalId());
                if (lcl_use_diag == true)
                {
                    //std::cout << "In checkUseGlbEd :: setUseGlbDiagOnLid() has returned true" << std::endl;
                    lcl_use_diag = this->setUseGlbDiagOnUrl(d->getContextDescriptor()->getURL());
                    #ifdef USE_XRELAY
					if (lcl_use_diag == true)
					{
						glbEdURL_ = d->getContextDescriptor()->getURL();
						glbEdURN_ = d->getURN();
					}
                    #endif
                }
            }

*/

        }
        catch (xdaq::exception::Exception& e)
        {
            //std::cout << "In checkUseGlbEd :: Got AppDesc. FAILURE for process:" << diagGlbName_ << " Instance:" << diagGlbInstance_ << std::endl;
            lcl_use_diag=false;
            this->setUseGlbDiagOnName(lcl_use_diag);
            //haveAnError = true;
        }
        // if (lcl_use_diag == false) haveAnError = true;
    }
    else
    {
        //std::cout << "In checkUseGlbEd :: checkUseGlbDiag() is false" << std::endl;
        lcl_use_diag=false;
    }
return 0;
}




bool DiagBagWizard::checkUseEm()
{
/*    bool haveAnError; */
    if (this->checkUseLvl1() == true)
    {
/*
        if (sentinelUseRelayedLvl1_ == false)
        {
*/
            xdaq::ApplicationDescriptor * l1d;
            bool lcl_use_lvl1=true;
            try
            {
    	    	#ifdef XDAQ373
    	    	    l1d = wa_->getApplicationContext()->getDefaultZone()->getApplicationDescriptor(this->getLvl1Name(), atoi(this->getLvl1Instance().c_str()));
				#else
		    		l1d = wa_->getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(this->getLvl1Name(), atoi(this->getLvl1Instance().c_str()));
				#endif

				this->setUseLvl1OnName(lcl_use_lvl1);
/*
                if (lcl_use_lvl1 == true)
                {
                    lcl_use_lvl1 = this->setUseLvl1OnLid(l1d->getLocalId());
                    if (lcl_use_lvl1 == true)
                    {
                        lcl_use_lvl1 = this->setUseLvl1OnUrl(l1d->getContextDescriptor()->getURL());
                    }
                }
*/
            }
            catch (xdaq::exception::Exception& e)
            {
                lcl_use_lvl1=false;
                this->setUseLvl1OnName(lcl_use_lvl1);
//                haveAnError = true;
            }
/*
            if (lcl_use_lvl1 == false) haveAnError = true;

        }
        else
        {
            lvl1NameStatus_ = "unchecked";
            lvl1LIDStatus_="unchecked";
            lvl1IPStatus_="unchecked";
            useLvl1Status_ = "unchecked";
            emUsable_ = true;
        }
*/
    }
return 0;
}





std::string DiagBagWizard::getDiagName() {return diagName_;}

void DiagBagWizard::setDiagName(std::string name) {diagName_ = name;}

std::string DiagBagWizard::getDiagInstance() {return diagInstance_;}

void DiagBagWizard::setDiagInstance(std::string instance) {diagInstance_ = instance;}

std::string DiagBagWizard::getDiagIP() {return diagIP_;}

void DiagBagWizard::setDiagIP(std::string ip) {diagIP_ = ip;}

std::string DiagBagWizard::getDiagPort() {return diagPort_;}

void DiagBagWizard::setDiagPort(std::string port) {diagPort_ = port;}

std::string DiagBagWizard::getDiagLid() {return diagLID_;}

void DiagBagWizard::setDiagLid(std::string lid) {diagLID_ = lid;}






std::string DiagBagWizard::getLvl1Name() {return lvl1Name_;}

void DiagBagWizard::setLvl1Name(std::string name) {lvl1Name_ = name;}

std::string DiagBagWizard::getLvl1Instance() {return lvl1Instance_;}

void DiagBagWizard::setLvl1Instance(std::string instance) {lvl1Instance_ = instance;}

std::string DiagBagWizard::getLvl1IP() {return lvl1IP_;}

void DiagBagWizard::setLvl1IP(std::string ip) {lvl1IP_ = ip;}

std::string DiagBagWizard::getLvl1Port() {return lvl1Port_;}

void DiagBagWizard::setLvl1Port(std::string port) {lvl1Port_ = port;}

std::string DiagBagWizard::getLvl1Lid() {return lvl1LID_;}

void DiagBagWizard::setLvl1Lid(std::string lid) {lvl1LID_ = lid;}



std::string DiagBagWizard::getGLogLevel() {return gLogLevel_;}
void DiagBagWizard::setGLogLevel(std::string logLevel) {gLogLevel_ = logLevel;}


long DiagBagWizard::getProcessLogLevel(std::string source)
{
if (source == "DEFAULT")
{
    if (gLogLevel_ == DIAGTRACE) return log4cplus::TRACE_LOG_LEVEL;
    if (gLogLevel_ == DIAGDEBUG) return log4cplus::DEBUG_LOG_LEVEL;
    if (gLogLevel_ == DIAGINFO) return log4cplus::INFO_LOG_LEVEL;
    if (gLogLevel_ == DIAGWARN) return log4cplus::WARN_LOG_LEVEL;
    if (gLogLevel_ == DIAGUSERINFO) return log4cplus::WARN_LOG_LEVEL;
    if (gLogLevel_ == DIAGERROR) return log4cplus::ERROR_LOG_LEVEL;
    if (gLogLevel_ == DIAGFATAL) return log4cplus::FATAL_LOG_LEVEL;
    if (gLogLevel_ == DIAGOFF) return log4cplus::OFF_LOG_LEVEL;
}
else if (source == "ED")
{
    if (edLogLevel_ == DIAGTRACE) return log4cplus::TRACE_LOG_LEVEL;
    if (edLogLevel_ == DIAGDEBUG) return log4cplus::DEBUG_LOG_LEVEL;
    if (edLogLevel_ == DIAGINFO) return log4cplus::INFO_LOG_LEVEL;
    if (edLogLevel_ == DIAGWARN) return log4cplus::WARN_LOG_LEVEL;
    if (edLogLevel_ == DIAGUSERINFO) return log4cplus::WARN_LOG_LEVEL;
    if (edLogLevel_ == DIAGERROR) return log4cplus::ERROR_LOG_LEVEL;
    if (edLogLevel_ == DIAGFATAL) return log4cplus::FATAL_LOG_LEVEL;
    if (edLogLevel_ == DIAGOFF) return log4cplus::OFF_LOG_LEVEL;
}

return log4cplus::NOT_SET_LOG_LEVEL;
}



std::string DiagBagWizard::getOldDiagIP() {return oldDiagIP_;}
void DiagBagWizard::setOldDiagIP(std::string s) {oldDiagIP_=s;}

std::string DiagBagWizard::getOldDiagPort() {return oldDiagPort_;}
void DiagBagWizard::setOldDiagPort(std::string s) {oldDiagPort_=s;}

std::string DiagBagWizard::getOldDiagName() {return oldDiagName_;}
void DiagBagWizard::setOldDiagName(std::string s) {oldDiagName_=s;}

std::string DiagBagWizard::getOldDiagInstance() {return oldDiagInstance_;}
void DiagBagWizard::setOldDiagInstance(std::string s) {oldDiagInstance_=s;}

std::string DiagBagWizard::getOldDiagLID() {return oldDiagLID_;}
void DiagBagWizard::setOldDiagLID(std::string s) {oldDiagLID_=s;}

bool DiagBagWizard::getSocketSoapExists() {return socketSoapExists_;}
void DiagBagWizard::setSocketSoapExists(bool b) {socketSoapExists_=b;}

std::string DiagBagWizard::getDiagLID() {return diagLID_;}
void DiagBagWizard::setDiagLID(std::string s) {diagLID_=s;}



bool DiagBagWizard::getSentinelUseRelayedLvl1() {return sentinelUseRelayedLvl1_;}
std::string DiagBagWizard::getEdLogLevel() {return edLogLevel_;}
std::string DiagBagWizard::getSentinelLogLevel() {return sentinelLogLevel_;}
bool DiagBagWizard::getSentinelUseRelayedEd() {return sentinelUseRelayedEd_;}
std::string DiagBagWizard::getSentinelContextName() {return sentinelContextName_;}
bool DiagBagWizard::getUseSentinel() {return useSentinel_;}










void DiagBagWizard::cleanupMsgString(std::string * msg)
{


        if (msg->length() > 1023) msg->resize(1023);

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

void DiagBagWizard::cleanupXtrabuffString(std::string * xtraBuffer)
{


        if (xtraBuffer->length() > 1023) xtraBuffer->resize(1023);

    	for (unsigned int sl=0 ; sl < strlen((*xtraBuffer).c_str()) ; sl++)
        {
	    /* Security check : Parse message and if any \n appears in then replace it with space */
    	    if ((*xtraBuffer)[sl] == '\n') (*xtraBuffer)[sl]=' ';
        }
}





/************************************************************
LONG LONG LONG LONG LONG LONG LONG LONG LONG LONG LONG LONG 
************************************************************/

void DiagBagWizard::setClogRequestedFieldsForLongPost(std::string * msg, std::string * lvl, std::string * text, int * errorCode, 
    	    	    	    	    	    	    	std::string * faultstate, std::string * systemid, std::string * subsystemid,
														std::string * extraBuffer)
{

	if (*extraBuffer == (std::string)"") *extraBuffer = (std::string)DIAG_DEFAULT_EBUFF;
	cleanupXtrabuffString(extraBuffer);

	/* Set which type of message we are using */
	longClog.setTypeOfMessage(MESSAGE_TYPE_IS_LONG);

	/* set level */
	longClog.setLevel(*lvl);

	cleanupMsgString(msg);
	longClog.setMessage(*msg);

	/* set text field // used for reconfiguration actions */
	longClog.setText(*text);

	/* set extra buffer */
//	cleanupMsgString(extraBuffer);
	longClog.setExtraBuffer(*extraBuffer);

	/* set system timestamp */
	struct timeval tv;
	gettimeofday(&tv,NULL);



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
                    longClog.setTimeStamp(lcl_timestamp);
//std::cout << "in diagbag::long lcl_timestamp = " << s3 << std::endl;




	/* convert to charlist timestamp */
	char ymhTimestamp[50];
	snprintf(ymhTimestamp, 50, ctime(&tv.tv_sec));
	/*extract year */
	char year[5];
	year[0] = ymhTimestamp[strlen(ymhTimestamp)-5];
	year[1] = ymhTimestamp[strlen(ymhTimestamp)-4];
	year[2] = ymhTimestamp[strlen(ymhTimestamp)-3];
	year[3] = ymhTimestamp[strlen(ymhTimestamp)-2];
	year[4] = '\0';
	/* cutdown year part in ascii timestamp */
	ymhTimestamp[strlen(ymhTimestamp)-6] = '\0';
	/* get microseconds info */	
	char microSec[25];
	snprintf(microSec,24,"%.6lu",tv.tv_usec);
	/* build final format ascii timestamp */
	std::string timestp = (std::string)ymhTimestamp + ":" + (std::string)microSec + " " + (std::string)year;
	longClog.setTimestampAsString(timestp);

	/* set logger name */
	longClog.setLogger(wa_->getApplicationLogger().getName());

	/* set errorCode */
	char ec[10];
	snprintf(ec, 9, "%i", *errorCode);
	std::string f_ec = "";
	for (int o=6 ; o>(int)strlen(ec); o--)
	{
		f_ec += "0";
	}
	f_ec += ec;
	longClog.setErrorCode(f_ec);

	/* set faultstate */
	longClog.setFaultState(*faultstate);

	/* set faultstate */
	longClog.setSystemID(*systemid);

	/* set faultstate */
	longClog.setSubSystemID(*subsystemid);

}









REPORT_ERROR_RETURN_TYPE DiagBagWizard::reportError(std::string msg,
                                std::string lvl,
                                std::string text,
                                int errorcode,
                                std::string faultstate,
    	    	    	    	std::string extraBuffer)
{
	if (text == "RECONFCMD")
	{
		//BSEMMOD
		sendReconfRequestMutex->take();

		processReportReconfCommandforLongClog(msg, lvl, text, errorcode, faultstate, systemID_, subSystemID_, extraBuffer);

		//BSEMMOD
		sendReconfRequestMutex->give();
	}
	else
	{
		//BSEMMOD
		sendMsgMutex->take();


		isProcessingNestedLongClog_ = false;

		longClog.setAddSubClassPath(false);
		longClog.setSubClassPath("");

		longClog.setAddFileName(false);
		longClog.setFileName("");

		longClog.setAddLineNumber(false);
		longClog.setLineNumber(0);

		processReportErrorforLongClog(msg, lvl, text, errorcode, faultstate, systemID_, subSystemID_, extraBuffer);

		//BSEMMOD
		sendMsgMutex->give();
	}
REPORT_ERROR_RETURN_COMMAND
}






REPORT_ERROR_RETURN_TYPE DiagBagWizard::reportError(bool addSubClassPath,
								std::string subClassPath,
								std::string msg,
                                std::string lvl,
                                std::string text,
                                int errorcode,
                                std::string faultstate,
    	    	    	    	std::string extraBuffer)
{
	if (text == "RECONFCMD")
	{
		//BSEMMOD
		sendReconfRequestMutex->take();

		processReportReconfCommandforLongClog(msg, lvl, text, errorcode, faultstate, systemID_, subSystemID_, extraBuffer);

		//BSEMMOD
		sendReconfRequestMutex->give();
	}
	else
	{
		//BSEMMOD
		sendMsgMutex->take();


		isProcessingNestedLongClog_ = true;

		longClog.setAddSubClassPath(addSubClassPath);
		longClog.setSubClassPath(subClassPath);

		longClog.setAddFileName(false);
		longClog.setFileName("");

		longClog.setAddLineNumber(false);
		longClog.setLineNumber(0);


		processReportErrorforLongClog(msg, lvl, text, errorcode, faultstate, systemID_, subSystemID_, extraBuffer);

		//BSEMMOD
		sendMsgMutex->give();
	}
REPORT_ERROR_RETURN_COMMAND
}






REPORT_ERROR_RETURN_TYPE DiagBagWizard::reportError(bool addSubClassPath,
								std::string subClassPath,
								bool addFileName,
								std::string fileName,
								std::string msg,
                                std::string lvl,
                                std::string text,
                                int errorcode,
                                std::string faultstate,
    	    	    	    	std::string extraBuffer)
{
	if (text == "RECONFCMD")
	{
		//BSEMMOD
		sendReconfRequestMutex->take();

		processReportReconfCommandforLongClog(msg, lvl, text, errorcode, faultstate, systemID_, subSystemID_, extraBuffer);

		//BSEMMOD
		sendReconfRequestMutex->give();
	}
	else
	{
		//BSEMMOD
		sendMsgMutex->take();


		isProcessingNestedLongClog_ = true;

		longClog.setAddSubClassPath(addSubClassPath);
		longClog.setSubClassPath(subClassPath);

		longClog.setAddFileName(addFileName);
		longClog.setFileName(fileName);

		longClog.setAddLineNumber(false);
		longClog.setLineNumber(0);


		processReportErrorforLongClog(msg, lvl, text, errorcode, faultstate, systemID_, subSystemID_, extraBuffer);

		//BSEMMOD
		sendMsgMutex->give();
	}
REPORT_ERROR_RETURN_COMMAND
}






REPORT_ERROR_RETURN_TYPE DiagBagWizard::reportError(bool addSubClassPath,
								std::string subClassPath,
								bool addFileName,
								std::string fileName,
								bool addLineNumber,
								int lineNumber,
								std::string msg,
                                std::string lvl,
                                std::string text,
                                int errorcode,
                                std::string faultstate,
    	    	    	    	std::string extraBuffer)
{

	if (text == "RECONFCMD")
	{
		//BSEMMOD
		sendReconfRequestMutex->take();

		processReportReconfCommandforLongClog(msg, lvl, text, errorcode, faultstate, systemID_, subSystemID_, extraBuffer);

		//BSEMMOD
		sendReconfRequestMutex->give();
	}
	else
	{
		//BSEMMOD
		sendMsgMutex->take();


		isProcessingNestedLongClog_ = true;

		longClog.setAddSubClassPath(addSubClassPath);
		longClog.setSubClassPath(subClassPath);

		longClog.setAddFileName(addFileName);
		longClog.setFileName(fileName);

		longClog.setAddLineNumber(addLineNumber);
		longClog.setLineNumber(lineNumber);


		processReportErrorforLongClog(msg, lvl, text, errorcode, faultstate, systemID_, subSystemID_, extraBuffer);

		//BSEMMOD
		sendMsgMutex->give();
	}
REPORT_ERROR_RETURN_COMMAND
}












































REPORT_ERROR_RETURN_TYPE DiagBagWizard::reportError(std::string msg,
                                std::string lvl,
                                std::string text,
                                int errorcode,
                                std::string faultstate,
                                std::string systemid,
                                std::string subsystemid,
    	    	    	    	std::string extraBuffer)
{

	if (text == "RECONFCMD")
	{
		//BSEMMOD
		sendReconfRequestMutex->take();

		processReportReconfCommandforLongClog(msg, lvl, text, errorcode, faultstate, systemid, subsystemid, extraBuffer);

		//BSEMMOD
		sendReconfRequestMutex->give();
	}
	else
	{
		//BSEMMOD
		sendMsgMutex->take();


		isProcessingNestedLongClog_ = false;

		longClog.setAddSubClassPath(false);
		longClog.setSubClassPath("");

		longClog.setAddFileName(false);
		longClog.setFileName("");

		longClog.setAddLineNumber(false);
		longClog.setLineNumber(0);

		processReportErrorforLongClog(msg, lvl, text, errorcode, faultstate, systemid, subsystemid, extraBuffer);

		//BSEMMOD
		sendMsgMutex->give();
	}
REPORT_ERROR_RETURN_COMMAND
}






REPORT_ERROR_RETURN_TYPE DiagBagWizard::reportError(bool addSubClassPath,
								std::string subClassPath,
								std::string msg,
                                std::string lvl,
                                std::string text,
                                int errorcode,
                                std::string faultstate,
                                std::string systemid,
                                std::string subsystemid,
    	    	    	    	std::string extraBuffer)
{

	if (text == "RECONFCMD")
	{
		//BSEMMOD
		sendReconfRequestMutex->take();

		processReportReconfCommandforLongClog(msg, lvl, text, errorcode, faultstate, systemid, subsystemid, extraBuffer);

		//BSEMMOD
		sendReconfRequestMutex->give();
	}
	else
	{
		//BSEMMOD
		sendMsgMutex->take();


		isProcessingNestedLongClog_ = true;

		longClog.setAddSubClassPath(addSubClassPath);
		longClog.setSubClassPath(subClassPath);

		longClog.setAddFileName(false);
		longClog.setFileName("");

		longClog.setAddLineNumber(false);
		longClog.setLineNumber(0);


		processReportErrorforLongClog(msg, lvl, text, errorcode, faultstate, systemid, subsystemid, extraBuffer);

		//BSEMMOD
		sendMsgMutex->give();
	}
REPORT_ERROR_RETURN_COMMAND
}






REPORT_ERROR_RETURN_TYPE DiagBagWizard::reportError(bool addSubClassPath,
								std::string subClassPath,
								bool addFileName,
								std::string fileName,
								std::string msg,
                                std::string lvl,
                                std::string text,
                                int errorcode,
                                std::string faultstate,
                                std::string systemid,
                                std::string subsystemid,
    	    	    	    	std::string extraBuffer)
{

	if (text == "RECONFCMD")
	{
		//BSEMMOD
		sendReconfRequestMutex->take();

		processReportReconfCommandforLongClog(msg, lvl, text, errorcode, faultstate, systemid, subsystemid, extraBuffer);

		//BSEMMOD
		sendReconfRequestMutex->give();
	}
	else
	{
		//BSEMMOD
		sendMsgMutex->take();


		isProcessingNestedLongClog_ = true;

		longClog.setAddSubClassPath(addSubClassPath);
		longClog.setSubClassPath(subClassPath);

		longClog.setAddFileName(addFileName);
		longClog.setFileName(fileName);

		longClog.setAddLineNumber(false);
		longClog.setLineNumber(0);


		processReportErrorforLongClog(msg, lvl, text, errorcode, faultstate, systemid, subsystemid, extraBuffer);

		//BSEMMOD
		sendMsgMutex->give();
	}
REPORT_ERROR_RETURN_COMMAND
}






REPORT_ERROR_RETURN_TYPE DiagBagWizard::reportError(bool addSubClassPath,
								std::string subClassPath,
								bool addFileName,
								std::string fileName,
								bool addLineNumber,
								int lineNumber,
								std::string msg,
                                std::string lvl,
                                std::string text,
                                int errorcode,
                                std::string faultstate,
                                std::string systemid,
                                std::string subsystemid,
    	    	    	    	std::string extraBuffer)
{

	if (text == "RECONFCMD")
	{
		//BSEMMOD
		sendReconfRequestMutex->take();

		processReportReconfCommandforLongClog(msg, lvl, text, errorcode, faultstate, systemid, subsystemid, extraBuffer);

		//BSEMMOD
		sendReconfRequestMutex->give();
	}
	else
	{
		//BSEMMOD
		sendMsgMutex->take();


		isProcessingNestedLongClog_ = true;

		longClog.setAddSubClassPath(addSubClassPath);
		longClog.setSubClassPath(subClassPath);

		longClog.setAddFileName(addFileName);
		longClog.setFileName(fileName);

		longClog.setAddLineNumber(addLineNumber);
		longClog.setLineNumber(lineNumber);


		processReportErrorforLongClog(msg, lvl, text, errorcode, faultstate, systemid, subsystemid, extraBuffer);

		//BSEMMOD
		sendMsgMutex->give();
	}
REPORT_ERROR_RETURN_COMMAND
}









































/*







void DiagBagWizard::reportError(std::string msg,
                                std::string lvl,
                                std::string text,
                                int errorcode,
                                std::string faultstate,
                                std::string systemid,
                                std::string subsystemid,
    	    	    	    	std::string extraBuffer)
{
	processReportErrorBranchingforLongClog(msg, lvl, text, errorcode, faultstate, systemid, subsystemid, extraBuffer);
}

*/



/*
void DiagBagWizard::processReportErrorBranchingforLongClog(	std::string msg,
															std::string lvl,
															std::string text,
															int errorcode,
															std::string faultstate,
															std::string systemid,
															std::string subsystemid,
															std::string extraBuffer)
{
	if (text == "RECONFCMD")
	{
		//BSEMMOD
		sendReconfRequestMutex->take();

		processReportReconfCommandforLongClog(msg, lvl, text, errorcode, faultstate, systemid, subsystemid, extraBuffer);

		//BSEMMOD
		sendReconfRequestMutex->give();
	}
	else
	{
		//BSEMMOD
		sendMsgMutex->take();

		processReportErrorforLongClog(msg, lvl, text, errorcode, faultstate, systemid, subsystemid, extraBuffer);

		//BSEMMOD
		sendMsgMutex->give();
	}

}
*/



void DiagBagWizard::processReportReconfCommandforLongClog(	std::string msg,
															std::string lvl,
															std::string text,
															int errorcode,
															std::string faultstate,
															std::string systemid,
															std::string subsystemid,
															std::string extraBuffer)
{


    /* Check message sanity and fill up CLog() base fields */
  	setClogRequestedFieldsForLongPost(&msg, &lvl, &text, &errorcode, &faultstate, &systemid, &subsystemid, &extraBuffer);

	/* reconfiguration debug block */
	/*
	std::cout << std::endl << "DEBUG::longClog.getText() = " << longClog.getText() << std::endl;
	if (emUsable_ == true)
	{
		std::cout << "emUsable_ is TRUE" << std::endl;
	}
	else std::cout << "emUsable_ is FALSE" << std::endl;
	*/
	/* If we have to trigger a reconfiguration action */
	if (emUsable_ == true)	
	{
		if (longClog.getText() == "RECONFCMD")
		{
			// std::cout << "Received a RECONFCMD log and ErrorManager is enabled" << std::endl;

			postXoapMessageForReconfAction();

		}
	}


}



void DiagBagWizard::processReportErrorforLongClog(	std::string msg,
															std::string lvl,
															std::string text,
															int errorcode,
															std::string faultstate,
															std::string systemid,
															std::string subsystemid,
															std::string extraBuffer)
{
/*
//edGlbUsable_ = true;

//MDEBUG
std::cout << std::endl << "*****************************************************************" << std::endl;
std::cout << "*****************************************************************" << std::endl;
std::cout << "DIAGDEBUG :: Long Call requested for posting error" << std::endl;
std::cout << "DIAGDEBUG :: Log Content Summary" << std::endl;
std::cout << "\tMessage is : " << msg << std::endl;
std::cout << "\tLogLevel is : " << lvl << std::endl;
std::cout << "DIAGDEBUG :: Logging Conditions Summary" << std::endl;
std::cout << "\tdefaultAppenderUsable_ is : ";
if (defaultAppenderUsable_ == true) {std::cout << "TRUE" << std::endl;} else {std::cout << "FALSE" << std::endl;}
std::cout << "\tedUsable_ is : ";
if (edUsable_ == true) {std::cout << "TRUE" << std::endl;} else {std::cout << "FALSE" << std::endl;}
std::cout << "\tedGlbUsable_ is : ";
if (edGlbUsable_ == true) {std::cout << "TRUE" << std::endl;} else {std::cout << "FALSE" << std::endl;}
std::cout << "\tsentinelUsable_ is : ";
if (sentinelUsable_ == true) {std::cout << "TRUE" << std::endl;} else {std::cout << "FALSE" << std::endl;}
*/


	/* If the CONSOLE oject defined by Pixels is the call destination, then :
	Report message to console AND
	Do not process the log any further, just exit once the log is dumped into console object */

	/* First check if CONSOLE is the LOG destination */
	if (extraBuffer == "PixelsConsole")
	{
		//std::cout << "PixelsConsole call detected for longLog" << std::endl;

	    /* If extra logger std::stringstream used by pixels has been defined in object constructor, use it */
    	if (pixelsConsolePointer_ != NULL)
	    {
			//std::cout << "PixelsConsole object exists ; logging into it" << std::endl;

	    	/* post message on std::stringstream */
    	    (*pixelsConsolePointer_) << lvl << ":" << msg << std::endl;
    
	    }
		//else std::cout << "PixelsConsole object does not exists ; call aborted" << std::endl;
		return;
	} //else std::cout << "Standard (no PixelsConsole) call detected for longLog" << std::endl;
	






    /* If we want tu use base logger, i.e. console output */
    if (DiagBag::isConsoleDumpEnabled()==true)
    {
        /* Check message sanity and fill up CLog() base fields */
    	setClogRequestedFieldsForLongPost(&msg, &lvl, &text, &errorcode, &faultstate, &systemid, &subsystemid, &extraBuffer);

    	/* post message on console via DiagBag method from parent class */
        DiagBag::consoleLvlLog(longClog.buildStringForConsoleOutput(), longClog.getLevel());
    
    }


	

    /* If we want to use advanced loggers, then prepare for them */
    if ( (defaultAppenderUsable_ == true) || (edUsable_ == true) || (edGlbUsable_ == true) || (sentinelUsable_ == true) || (fiAreInitialized_ == false) )
    {
        /* Check message sanity and fill up CLog() base fields */
    	setClogRequestedFieldsForLongPost(&msg, &lvl, &text, &errorcode, &faultstate, &systemid, &subsystemid, &extraBuffer);




	if (fiAreInitialized_ == true)
	{

//DEBUGME1
//std::cout << "Direct posting of long log message of criticity " << lvl << " for process " << wa_->getApplicationDescriptor()->getClassName() << std::endl;

	    	/* Prepare to post message */
        	logsRouter(&msg, &lvl, MESSAGE_TYPE_IS_LONG);
	}
	else
	{
		if (longClogsList_.size() < NB_MAX_MSG_IN_LONGCLOGLIST_BUFFER)
		{

//DEBUGME1
//std::cout << "Storing log message of criticity " << lvl << " for process " << wa_->getApplicationDescriptor()->getClassName() << std::endl;

			std::vector<CLog>::iterator theIterator = longClogsList_.begin();
			longClogsList_.insert( theIterator, longClog );
		}
	}




    }


/*
//MDEBUG
std::cout << "DIAGDEBUG :: Exiting Long Call" << std::endl;
std::cout << "*****************************************************************" << std::endl << std::endl;
*/

}




















/************************************************************
SHORT SHORT SHORT SHORT SHORT SHORT SHORT SHORT SHORT 
************************************************************/




void DiagBagWizard::setClogRequestedFieldsForShortPost(std::string * msg, std::string * lvl, std::string * extraBuffer)
{

	if (*extraBuffer == (std::string)"") *extraBuffer = (std::string)DIAG_DEFAULT_EBUFF;
	cleanupXtrabuffString(extraBuffer);


	/* Set which type of message we are using */
	shortClog.setTypeOfMessage(MESSAGE_TYPE_IS_SHORT);

	/* set level */
	shortClog.setLevel(*lvl);

	/* set extra buffer */
	shortClog.setExtraBuffer(*extraBuffer);


	cleanupMsgString(msg);
	shortClog.setMessage(*msg);

	/* get system timestamp */
	struct timeval tv;
	gettimeofday(&tv,NULL);
	
	
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
//std::cout << "in diagbag::short lcl_timestamp = " << s3 << std::endl;
					long long lcl_timestamp = atoll(s3);
                    shortClog.setTimeStamp(lcl_timestamp);

	
	
	/* convert to charlist timestamp */
	char ymhTimestamp[50];
	snprintf(ymhTimestamp, 50, ctime(&tv.tv_sec));
	/*extract year */
	char year[5];
	year[0] = ymhTimestamp[strlen(ymhTimestamp)-5];
	year[1] = ymhTimestamp[strlen(ymhTimestamp)-4];
	year[2] = ymhTimestamp[strlen(ymhTimestamp)-3];
	year[3] = ymhTimestamp[strlen(ymhTimestamp)-2];
	year[4] = '\0';
	/* cutdown year part in ascii timestamp */
	ymhTimestamp[strlen(ymhTimestamp)-6] = '\0';
	/* get microseconds info */	
	char microSec[25];
	snprintf(microSec,24,"%.6lu",tv.tv_usec);
	/* build final format ascii timestamp */
	std::string timestp = (std::string)ymhTimestamp + ":" + (std::string)microSec + " " + (std::string)year;
	shortClog.setTimestampAsString(timestp);


	/* get logger name */
	shortClog.setLogger(wa_->getApplicationLogger().getName());




	/* set DEFAULT values for 4 next fields in case of short CALL */

	/* set text field // used for reconfiguration actions */
	shortClog.setText("UNKNOWN");

	/* set error code */
	shortClog.setErrorCode("0");
	/* set faultstate */
	shortClog.setFaultState("UNKNOWN");
	/* set faultstate */
	shortClog.setSystemID(getSystemID());
	/* set faultstate */
	shortClog.setSubSystemID(getSubSystemID());

	


}




REPORT_ERROR_RETURN_TYPE DiagBagWizard::reportError(std::string msg, std::string lvl, std::string extraBuffer)
{
//BSEMMOD
sendMsgMutex->take();
isProcessingNestedShortClog_ = false;
	shortClog.setAddSubClassPath(false);
	shortClog.setSubClassPath("");
	shortClog.setAddFileName(false);
	shortClog.setFileName("");
	shortClog.setAddLineNumber(false);
	shortClog.setLineNumber(0);
processReportErrorforShortClog(msg, lvl, extraBuffer);
//BSEMMOD
sendMsgMutex->give();
REPORT_ERROR_RETURN_COMMAND
}






REPORT_ERROR_RETURN_TYPE DiagBagWizard::reportError(	bool addSubClassPath,
									std::string subClassPath,
									std::string msg,
									std::string lvl,
									std::string extraBuffer)
{

	sendMsgMutex->take();

	isProcessingNestedShortClog_ = true;

	shortClog.setAddSubClassPath(addSubClassPath);
	shortClog.setSubClassPath(subClassPath);

	shortClog.setAddFileName(false);
	shortClog.setFileName("");

	shortClog.setAddLineNumber(false);
	shortClog.setLineNumber(0);

	processReportErrorforShortClog(msg, lvl, extraBuffer);

	sendMsgMutex->give();
REPORT_ERROR_RETURN_COMMAND
}



REPORT_ERROR_RETURN_TYPE DiagBagWizard::reportError(	bool addSubClassPath,
									std::string subClassPath,
									bool addFileName,
									std::string fileName,
									std::string msg,
									std::string lvl,
									std::string extraBuffer)
{

	sendMsgMutex->take();

	isProcessingNestedShortClog_ = true;

	shortClog.setAddSubClassPath(addSubClassPath);
	shortClog.setSubClassPath(subClassPath);

	shortClog.setAddFileName(addFileName);
	shortClog.setFileName(fileName);

	shortClog.setAddLineNumber(false);
	shortClog.setLineNumber(0);

	processReportErrorforShortClog(msg, lvl, extraBuffer);

	sendMsgMutex->give();
REPORT_ERROR_RETURN_COMMAND
}

REPORT_ERROR_RETURN_TYPE DiagBagWizard::reportError(	bool addSubClassPath,
									std::string subClassPath,
									bool addFileName,
									std::string fileName,
									bool addLineNumber,
									int lineNumber,
									std::string msg,
									std::string lvl,
									std::string extraBuffer)
{

	sendMsgMutex->take();

	isProcessingNestedShortClog_ = true;

	shortClog.setAddSubClassPath(addSubClassPath);
	shortClog.setSubClassPath(subClassPath);

	shortClog.setAddFileName(addFileName);
	shortClog.setFileName(fileName);

	shortClog.setAddLineNumber(addLineNumber);
	shortClog.setLineNumber(lineNumber);

	processReportErrorforShortClog(msg, lvl, extraBuffer);

	sendMsgMutex->give();
REPORT_ERROR_RETURN_COMMAND
}












/*********************************************
REPOST LOW-LOGGING CALL
*********************************************/
void DiagBagWizard::processReportErrorforShortClog(std::string msg, std::string lvl, std::string extraBuffer)
{

//MODHERE
//std::cout << "My extra buffer is : " << extraBuffer << std::endl;

//MDEBUG
//std::cout << "DIAGDEBUG :: Short Call requested for posting error" << std::endl;


	/* If the CONSOLE oject defined by Pixels is the call destination, then :
	Report message to console AND
	Do not process the log any further, just exit once the log is dumped into console object */

	/* First check if CONSOLE is the LOG destination */
	if (extraBuffer == "PixelsConsole")
	{
		//std::cout << "PixelsConsole call detected for shortLog" << std::endl;

	    /* If extra logger std::stringstream used by pixels has been defined in object constructor, use it */
    	if (pixelsConsolePointer_ != NULL)
	    {
			//std::cout << "PixelsConsole object exists ; logging into it" << std::endl;

	    	/* post message on std::stringstream */
    	    (*pixelsConsolePointer_) << lvl << ":" << msg << std::endl;
    
	    }
		//else std::cout << "PixelsConsole object does not exists ; call aborted" << std::endl;
		return;
	} //else std::cout << "Standard (no PixelsConsole) call detected for shortLog" << std::endl;






    /* If we want tu use base logger, i.e. console output */
    if (DiagBag::isConsoleDumpEnabled()==true)
    {
        /* Check message sanity and fill up CLog() base fields */
    	setClogRequestedFieldsForShortPost(&msg, &lvl, &extraBuffer);

    	/* post message on console via DiagBag method from parent class */
        DiagBag::consoleLvlLog(shortClog.buildStringForConsoleOutput(), shortClog.getLevel());

    }




    /* If we want to use advanced loggers, then prepare for them */
    if ( (defaultAppenderUsable_ == true) || (edUsable_ == true) || (edGlbUsable_ == true) || (sentinelUsable_ == true) || (fiAreInitialized_ == false) )
    {

        /* Check message sanity and fill up CLog() base fields */
    	setClogRequestedFieldsForShortPost(&msg, &lvl, &extraBuffer);


	if (fiAreInitialized_ == true)
	{

//DEBUGME1
//std::cout << "Direct posting of short log message of criticity " << lvl << " for process " << wa_->getApplicationDescriptor()->getClassName() << std::endl;

	    	/* Prepare to post message */
        	logsRouter(&msg, &lvl, MESSAGE_TYPE_IS_SHORT);
	}
	else
	{
		if (shortClogsList_.size() < NB_MAX_MSG_IN_SHORTCLOGLIST_BUFFER)
		{
//DEBUGME1
//std::cout << "Storing log message of criticity " << lvl << " for process " << wa_->getApplicationDescriptor()->getClassName() << std::endl;

			std::vector<CLog>::iterator theIterator = shortClogsList_.begin();
			shortClogsList_.insert( theIterator, shortClog );
		}
	}


    	/* Prepare to post message */
//        logsRouter(&msg, &lvl, MESSAGE_TYPE_IS_SHORT);
    }
//MDEBUG
//std::cout << "DIAGDEBUG :: Exiting Short Call" << std::endl;


}





/*********************************************
DEFAULT LOW-LOGGING CALL
*********************************************/
void DiagBagWizard::repostError(std::string msgType,
                                std::string lvl,
								std::string msg,
								long long timeStamp,
								std::string timeStampAsString,
								std::string logger,
                                //std::string text,
                                std::string errorcode,
                                std::string faultstate,
                                std::string systemid,
                                std::string subsystemid,
								std::string extraBuffer)
{
//BSEMMOD
//sendMsgMutex->take();

//Build complete CLog in short item

cleanupMsgString(&msg);
//cleanupMsgString(&extraBuffer);

	shortClog.setTypeOfMessage(msgType);
	shortClog.setLevel(lvl);
	shortClog.setMessage(msg);
	shortClog.setTimeStamp(timeStamp);
	shortClog.setTimestampAsString(timeStampAsString);
	shortClog.setLogger(logger);
	shortClog.setText("");
	shortClog.setErrorCode(errorcode);
	shortClog.setFaultState(faultstate);
	shortClog.setSystemID(systemid);
	shortClog.setSubSystemID(subsystemid);
	shortClog.setExtraBuffer(extraBuffer);




	longClog.setTypeOfMessage(msgType);
	longClog.setLevel(lvl);
	longClog.setMessage(msg);
	longClog.setTimeStamp(timeStamp);
	longClog.setTimestampAsString(timeStampAsString);
	longClog.setLogger(logger);
	longClog.setText("");
	longClog.setErrorCode(errorcode);
	longClog.setFaultState(faultstate);
	longClog.setSystemID(systemid);
	longClog.setSubSystemID(subsystemid);
	longClog.setExtraBuffer(extraBuffer);



    /* If we want to use advanced loggers, then prepare for them */
    if ( ( (edUsable_ == true) || (edGlbUsable_ == true) || (sentinelUsable_ == true) ) || (fiAreInitialized_ == false) )
    {
    
	if (fiAreInitialized_ == true)
	{ 
//DEBUGME1
//std::cout << "Reposting log message of criticity " << lvl << " for process " << wa_->getApplicationDescriptor()->getClassName() << std::endl;
	    	/* Prepare to post message */
        	logsReRouter();
	}
	else
	{
		if (shortClogsRepostList_.size() < NB_MAX_MSG_IN_SHORTCLOGLIST_BUFFER)
		{
			std::vector<CLog>::iterator theIterator = shortClogsRepostList_.begin();
			shortClogsRepostList_.insert( theIterator, shortClog );
		}
	}

    
    
        }
//MDEBUG
//std::cout << "DIAGDEBUG :: Exiting Short Call" << std::endl;

//BSEMMOD
//sendMsgMutex->give();

}














































void DiagBagWizard::setEdLogLevel(std::string logLevel) {edLogLevel_ = logLevel;}


void DiagBagWizard::setGlbUseDiag(bool diagStatus) {useGlbDiag_ = diagStatus;}
void DiagBagWizard::setGlbEdLogLevel(std::string logLevel) {edGlbLogLevel_ = logLevel;}
void DiagBagWizard::setGlbDiagName(std::string name) {diagGlbName_ = name;}

void DiagBagWizard::setGlbDiagInstance(std::string instance) {diagGlbInstance_ = instance;}
void DiagBagWizard::setGlbDiagIP(std::string ip) {diagGlbIP_ = ip;}
void DiagBagWizard::setGlbDiagPort(std::string port) {diagGlbPort_ = port;}
void DiagBagWizard::setGlbDiagLid(std::string lid) {diagGlbLID_ = lid;}


void DiagBagWizard::setUseDiag(bool diagStatus) {useDiag_ = diagStatus;}

void DiagBagWizard::setUseEd(bool diagStatus) {edUsable_ = diagStatus;}


bool DiagBagWizard::getUseDiag(void) {return(useDiag_);}

bool DiagBagWizard::getUseEd(void) {return(edUsable_);}

void DiagBagWizard::setDefaultAppenderUsable(bool isUsable) {defaultAppenderUsable_ = isUsable;}

bool DiagBagWizard::getIsInSentinelListener(void) {return isInSentinelListener_;}

void DiagBagWizard::setIsInSentinelListener(bool isInSentinelListener) {isInSentinelListener_ = isInSentinelListener;}

//SENTMOD
//sentinel::Interface * DiagBagWizard::getSentinelInterface(void) {return sentinel_;}


void DiagBagWizard::setRunFromRcms(int runFromRcms) {runFromRcms_ = runFromRcms;}




void DiagBagWizard::display_internals(void)
{
    std::cout << "useDiag_ : " << useDiag_ << std::endl;
    std::cout << "diagName_ : " << diagName_ << std::endl;
    std::cout << "diagInstance_ : " << diagInstance_ << std::endl;
    std::cout << "diagIP_ : " << diagIP_ << std::endl;
    std::cout << "diagPort_ : " << diagPort_ << std::endl;
    std::cout << "diagLID_ : " << diagLID_ << std::endl;
    std::cout << "useLvl1_ : " << useLvl1_ << std::endl;
    std::cout << "lvl1Name_ : " << lvl1Name_ << std::endl;
    std::cout << "lvl1Instance_ : " << lvl1Instance_ << std::endl;
    std::cout << "lvl1IP_ : " << lvl1IP_ << std::endl;
    std::cout << "lvl1Port_ : " << lvl1Port_ << std::endl;
    std::cout << "lvl1LID_ : " << lvl1LID_ << std::endl;
    std::cout << "useSentinel_ : " << useSentinel_ << std::endl;
    std::cout << "sentinelContextName_ : " << sentinelContextName_ << std::endl;
    std::cout << "sentinelUseRelayedEd_ : " << sentinelUseRelayedEd_ << std::endl;
    std::cout << "sentinelLogLevel_ : " << sentinelLogLevel_ << std::endl;
    std::cout << "edLogLevel_ : " << edLogLevel_ << std::endl;
    std::cout << "sentinelUseRelayedLvl1_ : " << sentinelUseRelayedLvl1_ << std::endl;
    std::cout << "relayedEdStatus_ : " << relayedEdStatus_ << std::endl;
    std::cout << "relayedLvl1Status_ : " << relayedLvl1Status_ << std::endl;
    std::cout << "relayedEdUsable_ : " << relayedEdUsable_ << std::endl;
    std::cout << "relayedEmUsable_ : " << relayedEmUsable_ << std::endl;
    std::cout << "sentinelUsable_ : " << sentinelUsable_ << std::endl;
    std::cout << "isInSentinelListener_ : " << isInSentinelListener_ << std::endl;
}


void DiagBagWizard::setSentinelContextName(std::string contextName) {sentinelContextName_ = contextName;}

bool DiagBagWizard::getcontextHasSentinel(void) {return contextHasSentinel_;}

void DiagBagWizard::setcontextHasSentinel(bool hasSentinel) {contextHasSentinel_ = hasSentinel;}


void DiagBagWizard::setUseDiagSystem(std::string useDiagSystem){useDiagSystem_ = useDiagSystem;}
void DiagBagWizard::setLogLevel(std::string logLevel){logLevel_ = logLevel;}
void DiagBagWizard::setDiagSystemSettings(std::string diagSystemSettings){diagSystemSettings_ = diagSystemSettings;}



void DiagBagWizard::setUseDefaultAppender(bool useAppender){useDefaultAppender_ = useAppender;}

void DiagBagWizard::setDefaultLogLevel(std::string logLevel) {gLogLevel_ = logLevel;}

void DiagBagWizard::setUseConsoleDump(bool useConsoleDump)
{
    useConsoleDump_ = useConsoleDump;
    if (useConsoleDump_ == true)
    {
        DiagBag::setConsoleDumpEnabled(true);
    }
    else DiagBag::setConsoleDumpEnabled(false);
}


void DiagBagWizard::setUseSentinel(bool useSentinel) {useSentinel_ = useSentinel;}

void DiagBagWizard::setSentinelLogLevel(std::string logLevel) {sentinelLogLevel_ = logLevel;}

void DiagBagWizard::setSentinelUseRelayedLvl1(bool sentinelUseRelayedLvl1){sentinelUseRelayedLvl1_ = sentinelUseRelayedLvl1;}


void DiagBagWizard::setUseLvl1(bool useLvl1) {useLvl1_ = useLvl1;}


void DiagBagWizard::setForceDbLogging(bool forceDbLogging) {forcedDbLogs_ = forceDbLogging;}
void DiagBagWizard::setForcedDbLoggingLevel(std::string forcedDbLoggingLevel) {dbLogLevel_ = forcedDbLoggingLevel;}

void DiagBagWizard::setForceCsLogging(bool forceCsLogging) {forcedCsLogs_ = forceCsLogging;}
void DiagBagWizard::setForcedCsLoggingLevel(std::string forcedCsLoggingLevel) {csLogLevel_[0] = forcedCsLoggingLevel;}

void DiagBagWizard::setForceCcLogging(bool forceCcLogging) {forcedCcLogs_ = forceCcLogging;}
void DiagBagWizard::setForcedCcLoggingLevel(std::string forcedCcLoggingLevel) {ccLogLevel_ = forcedCcLoggingLevel;}



void DiagBagWizard::setIsInErrorDispatcher(bool isInErrorDispatcher) {isInErrorDispatcher_ = isInErrorDispatcher;}
bool DiagBagWizard::getIsInErrorDispatcher(void) {return isInErrorDispatcher_;}

void DiagBagWizard::setIsInGlobalErrorDispatcher(bool isInGlobalErrorDispatcher) {isInGlobalErrorDispatcher_ = isInGlobalErrorDispatcher;}
bool DiagBagWizard::getIsInGlobalErrorDispatcher(void) {return isInGlobalErrorDispatcher_;}


bool DiagBagWizard::isFileLoggingUsable(void) {return fileLoggingUsable_;}

void DiagBagWizard::setIsInUserProcess(bool isInUserProcess) {isInUserProcess_ = isInUserProcess;}
bool DiagBagWizard::getIsInUserProcess(void) {return isInUserProcess_;}

void DiagBagWizard::setIsInErrorManager(bool isInErrorManager) {isInErrorManager_ = isInErrorManager;}
bool DiagBagWizard::getIsInErrorManager(void) {return isInErrorManager_;}

void DiagBagWizard::setIsInLogsReader(bool isInLogsReader) {isInLogsReader_ = isInLogsReader;}
bool DiagBagWizard::getIsInLogsReader(void) {return isInLogsReader_;}


void DiagBagWizard::setRouteToCs(bool routeToCs, int csStreamIndex) {csRoute_[csStreamIndex] = routeToCs;}
void DiagBagWizard::setCsIP(std::string csIP, int csStreamIndex) {csIP_[csStreamIndex] = csIP;}
void DiagBagWizard::setCsPort(std::string csPort, int csStreamIndex) {csPort_[csStreamIndex] = csPort;}
void DiagBagWizard::setCsReceiver(std::string csReceiver, int csStreamIndex) {csReceiver_[csStreamIndex] = csReceiver;}
void DiagBagWizard::setCsLogLevel(std::string csLogLevel, int csStreamIndex) {csLogLevel_[csStreamIndex] = csLogLevel;}


void DiagBagWizard::setRouteToCc(bool routeToCc) {ccRoute_ = routeToCc;}
void DiagBagWizard::setCcLogLevel(std::string ccLogLevel) {ccLogLevel_ = ccLogLevel;}

void DiagBagWizard::setRouteToDb(bool routeToDb) {dbRoute_ = routeToDb;}
void DiagBagWizard::setDbLogLevel(std::string dbLogLevel) {dbLogLevel_ = dbLogLevel;}

void DiagBagWizard::setRouteToFile(bool routeToFile) {useFileLogging_ = routeToFile;}
void DiagBagWizard::setFileLogLevel(std::string fileLogLevel) {fileLogLevel_ = fileLogLevel;}
void DiagBagWizard::setFilePathAndName(std::string fileName) {logFileName_ = fileName;}
void DiagBagWizard::setDbFilePathAndName(std::string dbFileName) {dbFileName_ = dbFileName;}

void DiagBagWizard::setLogFilePath(std::string logFilePath) {logFilePath_ = logFilePath;}



void DiagBagWizard::setCloseReconfLoop(bool closeLoop) {useReconf_ = closeLoop;}
void DiagBagWizard::setReconfProcessName(std::string pName) {reconfName_ = pName;}
void DiagBagWizard::setReconfProcessInstance(std::string instance) {reconfInstance_ = instance;}
void DiagBagWizard::setReconfProcessLID(std::string lid) {reconfLID_ = lid;}
void DiagBagWizard::setReconfProcessIP(std::string ip) {reconfIP_ = ip;}
void DiagBagWizard::setReconfProcessPort(std::string port) {reconfPort_ = port;}


void DiagBagWizard::setUseGrabber(bool useGrabber) {useGrabber_ = useGrabber;}
void DiagBagWizard::setGrabberContextName(std::string name) {grabberContextName_ = name;}

void DiagBagWizard::applyUserAppFireItems(void)
{
	this->setUseDiagSystem( this->feUseDiagSystem_ );
	this->setDiagSystemSettings( expandFireItemWithEnvVariable(this->feDiagSystemSettings_) );
	this->setLogLevel( this->feLogLevel_ );

    this->setUseDefaultAppender(this->feUseDefaultAppender_);
    this->setDefaultLogLevel(this->feDefaultAppenderLogLevel_.toString());
    this->setUseConsoleDump(this->feDuplicateMsgToLocalConsole_);

    this->setUseSentinel(this->feUseSentinel_);
    this->setSentinelLogLevel(this->feSentinelAppenderLogLevel_);
    this->setSentinelContextName(this->fePushErrorsToContext_);


    this->setUseDiag(this->feUseErrorDispatcher_);
    this->setEdLogLevel(this->feErrorDispatcherLogLevel_);
    this->setDiagName(this->feErrorDispatcherName_);
    this->setDiagInstance(this->feErrorDispatcherInstance_);
    this->setDiagLid(this->feErrorDispatcherLID_);
    this->setDiagIP(this->feErrorDispatcherIP_);
    this->setDiagPort(this->feErrorDispatcherPort_);

    this->setGlbUseDiag(this->feUseGlbErrorDispatcher_);
    this->setGlbEdLogLevel(this->feGlbErrorDispatcherLogLevel_);
    this->setGlbDiagName(this->feGlbErrorDispatcherName_);
    this->setGlbDiagInstance(this->feGlbErrorDispatcherInstance_);
    this->setGlbDiagLid(this->feGlbErrorDispatcherLID_);
    this->setGlbDiagIP(this->feGlbErrorDispatcherIP_);
    this->setGlbDiagPort(this->feGlbErrorDispatcherPort_);
}


void DiagBagWizard::applyEmAppFireItems(void)
{
	this->setUseDiagSystem( this->feUseDiagSystem_ );
	this->setDiagSystemSettings( expandFireItemWithEnvVariable(this->feDiagSystemSettings_) );
	this->setLogLevel( this->feLogLevel_ );

    this->setUseDefaultAppender(this->feUseDefaultAppender_);
    this->setDefaultLogLevel(this->feDefaultAppenderLogLevel_.toString());
    this->setUseConsoleDump(this->feDuplicateMsgToLocalConsole_);

    this->setUseSentinel(this->feUseSentinel_);
    this->setSentinelLogLevel(this->feSentinelAppenderLogLevel_);
    this->setSentinelContextName(this->fePushErrorsToContext_);


    this->setUseDiag(this->feUseErrorDispatcher_);
    this->setEdLogLevel(this->feErrorDispatcherLogLevel_);
    this->setDiagName(this->feErrorDispatcherName_);
    this->setDiagInstance(this->feErrorDispatcherInstance_);
    this->setDiagLid(this->feErrorDispatcherLID_);
    this->setDiagIP(this->feErrorDispatcherIP_);
    this->setDiagPort(this->feErrorDispatcherPort_);

    this->setGlbUseDiag(this->feUseGlbErrorDispatcher_);
    this->setGlbEdLogLevel(this->feGlbErrorDispatcherLogLevel_);
    this->setGlbDiagName(this->feGlbErrorDispatcherName_);
    this->setGlbDiagInstance(this->feGlbErrorDispatcherInstance_);
    this->setGlbDiagLid(this->feGlbErrorDispatcherLID_);
    this->setGlbDiagIP(this->feGlbErrorDispatcherIP_);
    this->setGlbDiagPort(this->feGlbErrorDispatcherPort_);
}


void DiagBagWizard::applySentinelGrabberAppFireItems(void)
{
	this->setUseDiagSystem( this->feUseDiagSystem_ );
	this->setDiagSystemSettings( expandFireItemWithEnvVariable(this->feDiagSystemSettings_) );
	this->setLogLevel( this->feLogLevel_ );


    this->setUseDefaultAppender(this->feUseDefaultAppender_);
    this->setDefaultLogLevel(this->feDefaultAppenderLogLevel_.toString());
    this->setUseConsoleDump(this->feDuplicateMsgToLocalConsole_);

    this->setUseSentinel(this->feUseSentinel_);
    this->setSentinelLogLevel(this->feSentinelAppenderLogLevel_);
    this->setSentinelContextName(this->fePushErrorsToContext_);

    this->setUseDiag(this->feUseErrorDispatcher_);
    this->setEdLogLevel(this->feErrorDispatcherLogLevel_);
    this->setDiagName(this->feErrorDispatcherName_);
    this->setDiagInstance(this->feErrorDispatcherInstance_);
    this->setDiagLid(this->feErrorDispatcherLID_);
    this->setDiagIP(this->feErrorDispatcherIP_);
    this->setDiagPort(this->feErrorDispatcherPort_);

    this->setGlbUseDiag(this->feUseGlbErrorDispatcher_);
    this->setGlbEdLogLevel(this->feGlbErrorDispatcherLogLevel_);
    this->setGlbDiagName(this->feGlbErrorDispatcherName_);
    this->setGlbDiagInstance(this->feGlbErrorDispatcherInstance_);
    this->setGlbDiagLid(this->feGlbErrorDispatcherLID_);
    this->setGlbDiagIP(this->feGlbErrorDispatcherIP_);
    this->setGlbDiagPort(this->feGlbErrorDispatcherPort_);

    this->setUseGrabber(this->feListenSentinel_);
    this->setGrabberContextName(this->feListenContext_);
}



void DiagBagWizard::applyEdAppFireItems(void)
{
//std::cout << "In DiagBag, applying fireitems for error dispatcher" << std::endl;
	this->setUseDiagSystem( this->feUseDiagSystem_ );
	this->setDiagSystemSettings( expandFireItemWithEnvVariable(this->feDiagSystemSettings_) );
	this->setLogLevel( this->feLogLevel_ );


    this->setUseDefaultAppender(this->feUseDefaultAppender_);
    this->setDefaultLogLevel(this->feDefaultAppenderLogLevel_.toString());


    this->setUseSentinel(this->feUseSentinel_);
    this->setSentinelLogLevel(this->feSentinelAppenderLogLevel_);
    this->setSentinelContextName(this->fePushErrorsToContext_);


    this->setUseDiag(this->feUseErrorDispatcher_);
    this->setEdLogLevel(this->feErrorDispatcherLogLevel_);
    this->setDiagName(this->feErrorDispatcherName_);
    this->setDiagInstance(this->feErrorDispatcherInstance_);
    this->setDiagLid(this->feErrorDispatcherLID_);
    this->setDiagIP(this->feErrorDispatcherIP_);
    this->setDiagPort(this->feErrorDispatcherPort_);

    this->setGlbUseDiag(this->feUseGlbErrorDispatcher_);
    this->setGlbEdLogLevel(this->feGlbErrorDispatcherLogLevel_);
    this->setGlbDiagName(this->feGlbErrorDispatcherName_);
    this->setGlbDiagInstance(this->feGlbErrorDispatcherInstance_);
    this->setGlbDiagLid(this->feGlbErrorDispatcherLID_);
    this->setGlbDiagIP(this->feGlbErrorDispatcherIP_);
    this->setGlbDiagPort(this->feGlbErrorDispatcherPort_);

    this->setRouteToCs(this->feUseChainsaw_, 0);
    this->setCsIP(this->feChainsawServerHost_, 0);
    this->setCsPort(this->feChainsawServerPort_, 0);
    this->setCsReceiver(this->feChainsawReceiverName_, 0);
    this->setCsLogLevel(this->feChainsawLogLevel_, 0);

    this->setRouteToCc(this->feUseConsoleLogging_);
    this->setCcLogLevel(this->feConsoleLogLevel_);

    this->setRouteToDb(this->feUseDbLogging_);
    this->setDbLogLevel(this->feDbLogLevel_);

    this->setRouteToFile(this->feUseFileLogging_);
    this->setFileLogLevel(this->feFileLogLevel_);
//Line commented in order to take care of LogFile TimeStamp Tagging
//    this->setFilePathAndName(this->feFilePathAndName_);
    this->setDbFilePathAndName(this->feLogFilePath_);


	this->setLogFilePath( expandFireItemWithEnvVariable(this->feLogFilePath_) );

	//Initialise run runmber
	this->setRunFromRcms( (int)this->feRunFromRcms_ );

}



void DiagBagWizard::applyGlbEdAppFireItems(void)
{
//DEBUGME
//std::cout << "In DiagBag, applying fireitems for global error dispatcher" << std::endl;
	this->setUseDiagSystem( this->feUseDiagSystem_ );
	this->setDiagSystemSettings( expandFireItemWithEnvVariable(this->feDiagSystemSettings_) );
	this->setLogLevel( this->feLogLevel_ );


    this->setUseDefaultAppender(this->feUseDefaultAppender_);
    this->setDefaultLogLevel(this->feDefaultAppenderLogLevel_.toString());


    this->setUseSentinel(this->feUseSentinel_);
    this->setSentinelLogLevel(this->feSentinelAppenderLogLevel_);
    this->setSentinelContextName(this->fePushErrorsToContext_);


    this->setUseDiag(this->feUseErrorDispatcher_);
    this->setEdLogLevel(this->feErrorDispatcherLogLevel_);
    this->setDiagName(this->feErrorDispatcherName_);
    this->setDiagInstance(this->feErrorDispatcherInstance_);
    this->setDiagLid(this->feErrorDispatcherLID_);
    this->setDiagIP(this->feErrorDispatcherIP_);
    this->setDiagPort(this->feErrorDispatcherPort_);

    this->setGlbUseDiag(this->feUseGlbErrorDispatcher_);
    this->setGlbEdLogLevel(this->feGlbErrorDispatcherLogLevel_);
    this->setGlbDiagName(this->feGlbErrorDispatcherName_);
    this->setGlbDiagInstance(this->feGlbErrorDispatcherInstance_);
    this->setGlbDiagLid(this->feGlbErrorDispatcherLID_);
    this->setGlbDiagIP(this->feGlbErrorDispatcherIP_);
    this->setGlbDiagPort(this->feGlbErrorDispatcherPort_);




//If we are in GED AND CS env. settings are ok then override hyperdaq CS values for first CS occurence
if ( useCsEnvSettings_ == true )
{
	overrideCsSettings();
}
else
{
    this->setRouteToCs(this->feUseChainsaw_, 0);
    this->setCsIP(this->feChainsawServerHost_, 0);
    this->setCsPort(this->feChainsawServerPort_, 0);
    this->setCsReceiver(this->feChainsawReceiverName_, 0);
    this->setCsLogLevel(this->feChainsawLogLevel_, 0);
}

    this->setRouteToCc(this->feUseConsoleLogging_);
    this->setCcLogLevel(this->feConsoleLogLevel_);

    this->setRouteToDb(this->feUseDbLogging_);
    this->setDbLogLevel(this->feDbLogLevel_);

    this->setRouteToFile(this->feUseFileLogging_);
    this->setFileLogLevel(this->feFileLogLevel_);
//Line commented in order to take care of LogFile TimeStamp Tagging
//    this->setFilePathAndName(this->feFilePathAndName_);
    this->setDbFilePathAndName(this->feDbFilePathAndName_);

	this->setLogFilePath( expandFireItemWithEnvVariable(this->feLogFilePath_) );
//DEBUGME
/*
std::cout << "In DiagBag, end of applying fireitems for global error dispatcher" << std::endl;
std::cout << "In DiagBag, useDiagSystem_ = " << useDiagSystem_ << std::endl;
std::cout << "In DiagBag, diagSystemSettings_ = " << diagSystemSettings_ << std::endl;
std::cout << "In DiagBag, logFilePath_ = " << logFilePath_ << std::endl;
*/

//std::cout << "In DiagBag, got run number from RCMS = " << this->feRunFromRcms_  << std::endl;


	//Initialise run runmber
	this->setRunFromRcms( (int)this->feRunFromRcms_ );

}



void DiagBagWizard::applyLogsReaderAppFireItems(void)
{
	this->setUseDiagSystem( this->feUseDiagSystem_ );
	this->setDiagSystemSettings( expandFireItemWithEnvVariable(this->feDiagSystemSettings_) );
	this->setLogLevel( this->feLogLevel_ );


    this->setUseDefaultAppender(this->feUseDefaultAppender_);
    this->setDefaultLogLevel(this->feDefaultAppenderLogLevel_.toString());
    this->setUseConsoleDump(this->feDuplicateMsgToLocalConsole_);

    this->setUseSentinel(this->feUseSentinel_);
    this->setSentinelLogLevel(this->feSentinelAppenderLogLevel_);
    this->setSentinelContextName(this->fePushErrorsToContext_);


    this->setUseDiag(this->feUseErrorDispatcher_);
    this->setEdLogLevel(this->feErrorDispatcherLogLevel_);
    this->setDiagName(this->feErrorDispatcherName_);
    this->setDiagInstance(this->feErrorDispatcherInstance_);
    this->setDiagLid(this->feErrorDispatcherLID_);
    this->setDiagIP(this->feErrorDispatcherIP_);
    this->setDiagPort(this->feErrorDispatcherPort_);

    this->setGlbUseDiag(this->feUseGlbErrorDispatcher_);
    this->setGlbEdLogLevel(this->feGlbErrorDispatcherLogLevel_);
    this->setGlbDiagName(this->feGlbErrorDispatcherName_);
    this->setGlbDiagInstance(this->feGlbErrorDispatcherInstance_);
    this->setGlbDiagLid(this->feGlbErrorDispatcherLID_);
    this->setGlbDiagIP(this->feGlbErrorDispatcherIP_);
    this->setGlbDiagPort(this->feGlbErrorDispatcherPort_);
//Line UNcommented in order to take care of LogFile TimeStamp Tagging
//    this->setFilePathAndName(this->feFilePathAndName_);
}




void DiagBagWizard::joinSentinel(void)
{
std::cout << "joinSentinel CALLED" << std::endl;
    try
    {
//SENTMOD
//        sentinel_ = dynamic_cast<sentinel::Interface*>(wa_->getApplicationContext()->getFirstApplication("Sentinel"));
//        sentinel_->join(wa_);
        //sentinel_->pushContext(sentinelContextName_, wa_);
        contextHasSentinel_ = true;
        log4cplus::Logger & e_logger = ((log4cplus::Logger &)(*callingAppLogger_));
        LOG4CPLUS_INFO(e_logger, "Sentinel Joined");
    }
    catch(xdaq::exception::ApplicationNotFound & e)
    {
        log4cplus::Logger & e_logger = ((log4cplus::Logger &)(*callingAppLogger_));
        LOG4CPLUS_ERROR(e_logger, "no sentinel found, remote notification not enabled");
    }
//SENTMOD
/*
    catch(sentinel::exception::Exception & se)
    {
        log4cplus::Logger & e_logger = ((log4cplus::Logger &)(*callingAppLogger_));
        LOG4CPLUS_ERROR(e_logger, se.what());
    }
*/
}


void DiagBagWizard::leaveSentinel(void)
{
    try
    {
//SENTMOD
        //sentinel_->popContext(wa_);
//        sentinel_->leave(wa_);
        contextHasSentinel_ = false;
        log4cplus::Logger & e_logger = ((log4cplus::Logger &)(*callingAppLogger_));

        LOG4CPLUS_INFO(e_logger, "Sentinel Leaved");

    }
    catch(xdaq::exception::ApplicationNotFound & e)
    {
        log4cplus::Logger & e_logger = ((log4cplus::Logger &)(*callingAppLogger_));
        LOG4CPLUS_ERROR(e_logger, "no sentinel found, cannot leave");
    }
//SENTMOD
/*
    catch(sentinel::exception::Exception & se)
    {
        log4cplus::Logger & e_logger = ((log4cplus::Logger &)(*callingAppLogger_));
        LOG4CPLUS_ERROR(e_logger, se.what());
    }
*/
}











void DiagBagWizard::logsReRouter()
{

    int msg_level = INFOLEVEL;
    if (shortClog.getLevel() == DIAGTRACE) msg_level=TRACELEVEL;
    else if (shortClog.getLevel() == DIAGDEBUG) msg_level=DEBUGLEVEL;
    else if (shortClog.getLevel() == DIAGINFO) msg_level=INFOLEVEL;
    else if (shortClog.getLevel() == DIAGWARN) msg_level=WARNLEVEL;
    else if (shortClog.getLevel() == DIAGUSERINFO) msg_level=USERINFOLEVEL;
    else if (shortClog.getLevel() == DIAGERROR) msg_level=ERRORLEVEL;
    else if (shortClog.getLevel() == DIAGFATAL) msg_level=FATALLEVEL;
    else if (shortClog.getLevel() == DIAGOFF) msg_level=OFFLEVEL;


   if (edUsable_ == true)
    {

		int defaultEdLevel = INFOLEVEL;
		if (edLogLevel_ == DIAGTRACE) defaultEdLevel=TRACELEVEL;
		else if (edLogLevel_ == DIAGDEBUG) defaultEdLevel=DEBUGLEVEL;
		else if (edLogLevel_ == DIAGINFO) defaultEdLevel=INFOLEVEL;
		else if (edLogLevel_ == DIAGWARN) defaultEdLevel=WARNLEVEL;
		else if (edLogLevel_ == DIAGUSERINFO) defaultEdLevel=USERINFOLEVEL;
		else if (edLogLevel_ == DIAGERROR) defaultEdLevel=ERRORLEVEL;
		else if (edLogLevel_ == DIAGFATAL) defaultEdLevel=FATALLEVEL;
		else if (edLogLevel_ == DIAGOFF) defaultEdLevel=OFFLEVEL;

		if ( (msg_level >= defaultEdLevel) && (defaultEdLevel != OFFLEVEL) )
		{
			this->postXoapMessageForLclEd(shortClog.getTypeOfMessage());
		}
    }


    if (edGlbUsable_ == true)
    {
		
		int defaultEdLevel = INFOLEVEL;
		if (edGlbLogLevel_ == DIAGTRACE) defaultEdLevel=TRACELEVEL;
		else if (edGlbLogLevel_ == DIAGDEBUG) defaultEdLevel=DEBUGLEVEL;
		else if (edGlbLogLevel_ == DIAGINFO) defaultEdLevel=INFOLEVEL;
		else if (edGlbLogLevel_ == DIAGWARN) defaultEdLevel=WARNLEVEL;
		else if (edGlbLogLevel_ == DIAGUSERINFO) defaultEdLevel=USERINFOLEVEL;
		else if (edGlbLogLevel_ == DIAGERROR) defaultEdLevel=ERRORLEVEL;
		else if (edGlbLogLevel_ == DIAGFATAL) defaultEdLevel=FATALLEVEL;
		else if (edGlbLogLevel_ == DIAGOFF) defaultEdLevel=OFFLEVEL;


//DEBUGME1
//std::cout << "In logsRERouter, posting for GED" << std::endl;
//std::cout << "Msg level is : " << shortClog.getLevel() << "(" << msg_level << ")" << std::endl;
//std::cout << "GED level is : " << defaultEdLevel << std::endl;

		if ( (msg_level >= defaultEdLevel) && (defaultEdLevel != OFFLEVEL) )
		{
			//std::cout << "log REposted" <<std::endl;
			this->postXoapMessageForGlbEd(shortClog.getTypeOfMessage());
		}
		//else std::cout << "log REdiscarded" <<std::endl;
    }







    if (defaultAppenderUsable_ == true)
    {
        int defaultAppenderLevel = INFOLEVEL;
        if (this->getGLogLevel() == DIAGTRACE) defaultAppenderLevel=TRACELEVEL;
        else if (this->getGLogLevel() == DIAGDEBUG) defaultAppenderLevel=DEBUGLEVEL;
        else if (this->getGLogLevel() == DIAGINFO) defaultAppenderLevel=INFOLEVEL;
        else if (this->getGLogLevel() == DIAGWARN) defaultAppenderLevel=WARNLEVEL;
        else if (this->getGLogLevel() == DIAGUSERINFO) defaultAppenderLevel=USERINFOLEVEL;
        else if (this->getGLogLevel() == DIAGERROR) defaultAppenderLevel=ERRORLEVEL;
        else if (this->getGLogLevel() == DIAGFATAL) defaultAppenderLevel=FATALLEVEL;
        else if (this->getGLogLevel() == DIAGOFF) defaultAppenderLevel=OFFLEVEL;

        if ( (msg_level >= defaultAppenderLevel) && (defaultAppenderLevel != OFFLEVEL) )
        {
            if (shortClog.getLevel() == DIAGTRACE) {LOG4CPLUS_TRACE(((log4cplus::Logger &)(*callingAppLogger_)), shortClog.getMessage());}

            else if (shortClog.getLevel() == DIAGDEBUG) {LOG4CPLUS_DEBUG(((log4cplus::Logger &)(*callingAppLogger_)), shortClog.getMessage());}
            else if (shortClog.getLevel() == DIAGINFO) {LOG4CPLUS_INFO(((log4cplus::Logger &)(*callingAppLogger_)), shortClog.getMessage());}
            else if (shortClog.getLevel() == DIAGWARN) {LOG4CPLUS_WARN(((log4cplus::Logger &)(*callingAppLogger_)), shortClog.getMessage());}
            else if (shortClog.getLevel() == DIAGUSERINFO) {LOG4CPLUS_INFO(((log4cplus::Logger &)(*callingAppLogger_)), shortClog.getMessage());}
            else if (shortClog.getLevel() == DIAGERROR) {LOG4CPLUS_ERROR(((log4cplus::Logger &)(*callingAppLogger_)), shortClog.getMessage());}
            else if (shortClog.getLevel() == DIAGFATAL) {LOG4CPLUS_FATAL(((log4cplus::Logger &)(*callingAppLogger_)), shortClog.getMessage());}

        }
    }





//std::cout << "In logsReRouter before define" << std::endl;


#ifdef TK_ENABLE_WS_EVENTING
//std::cout << "In logsReRouter after define" << std::endl;
//std::cout << "In logsReRouter : asentinelUsable_ = " << sentinelUsable_ << std::endl;

    if (sentinelUsable_ == true)
    {
        int sentinelLevel = INFOLEVEL;
        if (this->getSentinelLogLevel() == DIAGTRACE) sentinelLevel=TRACELEVEL;
        else if (this->getSentinelLogLevel() == DIAGDEBUG) sentinelLevel=DEBUGLEVEL;
        else if (this->getSentinelLogLevel() == DIAGINFO) sentinelLevel=INFOLEVEL;
        else if (this->getSentinelLogLevel() == DIAGWARN) sentinelLevel=WARNLEVEL;
        else if (this->getSentinelLogLevel() == DIAGUSERINFO) sentinelLevel=USERINFOLEVEL;
        else if (this->getSentinelLogLevel() == DIAGERROR) sentinelLevel=ERRORLEVEL;
        else if (this->getSentinelLogLevel() == DIAGFATAL) sentinelLevel=FATALLEVEL;
        else if (this->getSentinelLogLevel() == DIAGOFF) sentinelLevel=OFFLEVEL;

		CLog * clog_ptr;
		if (shortClog.getTypeOfMessage() == MESSAGE_TYPE_IS_SHORT)
		{
			clog_ptr = &shortClog;
		}
		else clog_ptr = &longClog;


//std::cout << "msg_level = " << msg_level << std::endl;
//std::cout << "sentinelLevel = " << sentinelLevel << std::endl;

        if ( (msg_level >= sentinelLevel) && (sentinelLevel != OFFLEVEL) )
        {
//std::cout << "building exception" << std::endl;

			xcept::Exception tkExcept(  "xcept::Exception", clog_ptr->getMessage(), __FILE__, __LINE__, __FUNCTION__);		
			std::string notifier = toolbox::toString("%s/%s", wa_->getApplicationDescriptor()->getContextDescriptor()->getURL().c_str() , wa_->getApplicationDescriptor()->getURN().c_str()) ;
			tkExcept.setProperty("notifier", notifier );
			tkExcept.setProperty("qualifiedErrorSchemaURI", "http://xdaq.web.cern.ch/xdaq/xsd/2005/QualifiedSoftwareErrorRecord-10.xsd");
			tkExcept.setProperty("dateTime", toolbox::getDateTime() );
			tkExcept.setProperty("sessionID", "none" );
			if (clog_ptr->getLevel() == DIAGUSERINFO)
			{
				tkExcept.setProperty("severity", "INFO" );
			}
			else tkExcept.setProperty("severity", clog_ptr->getLevel() );

/*
			tkExcept.setProperty("messageType", clog_ptr->getTypeOfMessage() );
			tkExcept.setProperty("timestamp", clog_ptr->getTimestampAsString() );
			std::ostringstream number_str;
			number_str << clog_ptr->getTimeStamp();
			tkExcept.setProperty("numtimestamp", number_str.str() );
			tkExcept.setProperty("level", clog_ptr->getLevel() );
			tkExcept.setProperty("logger", clog_ptr->getLogger() );
			tkExcept.setProperty("message", clog_ptr->getMessage() );
			tkExcept.setProperty("errorCode", clog_ptr->getErrorCode() );
			tkExcept.setProperty("faultstate", clog_ptr->getFaultState() );
			tkExcept.setProperty("systemid", clog_ptr->getSystemID() );
			tkExcept.setProperty("subsystemid", clog_ptr->getSubSystemID() );
			tkExcept.setProperty("extraBuffer", clog_ptr->getExtraBuffer() );

*/
			//std::cout << "In DiagBagWizard : raising EXCEPTION" << std::endl;

//std::cout << "clog_ptr->getLevel = " << clog_ptr->getLevel() << std::endl;

			try
			{

				if (clog_ptr->getLevel() == DIAGWARN) wa_->notifyQualified("warn", tkExcept);
				if (clog_ptr->getLevel() == DIAGERROR) wa_->notifyQualified("error", tkExcept);
				if (clog_ptr->getLevel() == DIAGFATAL) wa_->notifyQualified("fatal", tkExcept);
				//std::cout << "Message sent to Sentinel" << std::endl;				

			}
			catch (xdaq::exception::Exception& e)
			{
				//std::cout << "Failed to remotly notify Log Message via Sentinel" << e.what() << std::endl;
				XCEPT_RETHROW (xgi::exception::Exception, "Failed to remotly notify Exception", e);
			}

			//std::cout << "In DiagBagWizard : EXCEPTION raised" << std::endl;
        }
    }
#endif



}






void DiagBagWizard::logsRouter(std::string * msg, std::string * lvl, std::string msgType) throw (xdaq::exception::Exception)
{
/*
std::cout << "In logsRouter" << std::endl;
std::cout << "Message is : " << *msg << std::endl;
std::cout << "Level is : " << *lvl << std::endl;
std::cout << "Type is : " << msgType << std::endl;
*/

    int msg_level = INFOLEVEL;
    if ((*lvl) == DIAGTRACE) msg_level=TRACELEVEL;
    else if ((*lvl) == DIAGDEBUG) msg_level=DEBUGLEVEL;
    else if ((*lvl) == DIAGINFO) msg_level=INFOLEVEL;
    else if ((*lvl) == DIAGWARN) msg_level=WARNLEVEL;
    else if ((*lvl) == DIAGUSERINFO) msg_level=USERINFOLEVEL;
    else if ((*lvl) == DIAGERROR) msg_level=ERRORLEVEL;
    else if ((*lvl) == DIAGFATAL) msg_level=FATALLEVEL;
    else if ((*lvl) == DIAGOFF) msg_level=OFFLEVEL;




    if (edUsable_ == true)
    {
    	#ifdef USE_XRELAY
		int loopCounter=0;
        while ( (isWaitingForLclLogAck_ == true) && (loopCounter <=LOOP_SIZE_FOR_WAIT_SOAP_ACK) )
		{
	    	usleep(USLEEP_DELAY_FOR_WAIT_SOAP_ACK);
    	    loopCounter++;
		}
		if (loopCounter > LOOP_SIZE_FOR_WAIT_SOAP_ACK)
		{
    	    isWaitingForLclLogAck_ = false;
			missedLclEmissions_++;
	    	return;
		}
    	#endif
	
		int defaultEdLevel = INFOLEVEL;
		if (edLogLevel_ == DIAGTRACE) defaultEdLevel=TRACELEVEL;
		else if (edLogLevel_ == DIAGDEBUG) defaultEdLevel=DEBUGLEVEL;
		else if (edLogLevel_ == DIAGINFO) defaultEdLevel=INFOLEVEL;
		else if (edLogLevel_ == DIAGWARN) defaultEdLevel=WARNLEVEL;
		else if (edLogLevel_ == DIAGUSERINFO) defaultEdLevel=USERINFOLEVEL;
		else if (edLogLevel_ == DIAGERROR) defaultEdLevel=ERRORLEVEL;
		else if (edLogLevel_ == DIAGFATAL) defaultEdLevel=FATALLEVEL;
		else if (edLogLevel_ == DIAGOFF) defaultEdLevel=OFFLEVEL;

		if ( (msg_level >= defaultEdLevel) && (defaultEdLevel != OFFLEVEL) )
		{
			#ifdef USE_XRELAY
				talkToLocalEd_ = true;
			#endif
			this->postXoapMessageForLclEd(msgType);
		}
    }


    if (edGlbUsable_ == true)
    {
    	#ifdef USE_XRELAY
		int loopCounter=0;
        while ( (isWaitingForGlbLogAck_ == true) && (loopCounter <=LOOP_SIZE_FOR_WAIT_SOAP_ACK) )
		{
	    	usleep(USLEEP_DELAY_FOR_WAIT_SOAP_ACK);
    	    loopCounter++;
		}
		if (loopCounter > LOOP_SIZE_FOR_WAIT_SOAP_ACK)
		{
    	    isWaitingForGlbLogAck_ = false;
			missedGlbEmissions_++;
		    return;
		}
        #endif
		
		int defaultEdLevel = INFOLEVEL;
		if (edGlbLogLevel_ == DIAGTRACE) defaultEdLevel=TRACELEVEL;
		else if (edGlbLogLevel_ == DIAGDEBUG) defaultEdLevel=DEBUGLEVEL;
		else if (edGlbLogLevel_ == DIAGINFO) defaultEdLevel=INFOLEVEL;
		else if (edGlbLogLevel_ == DIAGWARN) defaultEdLevel=WARNLEVEL;
		else if (edGlbLogLevel_ == DIAGUSERINFO) defaultEdLevel=USERINFOLEVEL;
		else if (edGlbLogLevel_ == DIAGERROR) defaultEdLevel=ERRORLEVEL;
		else if (edGlbLogLevel_ == DIAGFATAL) defaultEdLevel=FATALLEVEL;
		else if (edGlbLogLevel_ == DIAGOFF) defaultEdLevel=OFFLEVEL;

//DEBUGME1
//std::cout << "In logsRouter, posting for GED" << std::endl;
//std::cout << "Msg level is : " << (*lvl) << std::endl;
//std::cout << "GED level is : " << defaultEdLevel << std::endl;

		if ( (msg_level >= defaultEdLevel) && (defaultEdLevel != OFFLEVEL) )
		{
			#ifdef USE_XRELAY
				talkToLocalEd_ = false;
			#endif
			//std::cout << "message posted" << std::endl;
			this->postXoapMessageForGlbEd(msgType);
		}
		//else std::cout << "message discarded - level too low" << std::endl;
    }



    if (defaultAppenderUsable_ == true)
    {
        int defaultAppenderLevel = INFOLEVEL;
        if (this->getGLogLevel() == DIAGTRACE) defaultAppenderLevel=TRACELEVEL;
        else if (this->getGLogLevel() == DIAGDEBUG) defaultAppenderLevel=DEBUGLEVEL;
        else if (this->getGLogLevel() == DIAGINFO) defaultAppenderLevel=INFOLEVEL;
        else if (this->getGLogLevel() == DIAGWARN) defaultAppenderLevel=WARNLEVEL;
        else if (this->getGLogLevel() == DIAGUSERINFO) defaultAppenderLevel=USERINFOLEVEL;
        else if (this->getGLogLevel() == DIAGERROR) defaultAppenderLevel=ERRORLEVEL;
        else if (this->getGLogLevel() == DIAGFATAL) defaultAppenderLevel=FATALLEVEL;
        else if (this->getGLogLevel() == DIAGOFF) defaultAppenderLevel=OFFLEVEL;

        if ( (msg_level >= defaultAppenderLevel) && (defaultAppenderLevel != OFFLEVEL) )
        {
            if ((*lvl) == DIAGTRACE) {LOG4CPLUS_TRACE(((log4cplus::Logger &)(*callingAppLogger_)), (*msg));}
            else if ((*lvl) == DIAGDEBUG) {LOG4CPLUS_DEBUG(((log4cplus::Logger &)(*callingAppLogger_)), (*msg));}
            else if ((*lvl) == DIAGINFO) {LOG4CPLUS_INFO(((log4cplus::Logger &)(*callingAppLogger_)), (*msg));}
            else if ((*lvl) == DIAGWARN) {LOG4CPLUS_WARN(((log4cplus::Logger &)(*callingAppLogger_)), (*msg));}
            else if ((*lvl) == DIAGUSERINFO) {LOG4CPLUS_INFO(((log4cplus::Logger &)(*callingAppLogger_)), (*msg));}
            else if ((*lvl) == DIAGERROR) {LOG4CPLUS_ERROR(((log4cplus::Logger &)(*callingAppLogger_)), (*msg));}
            else if ((*lvl) == DIAGFATAL) {LOG4CPLUS_FATAL(((log4cplus::Logger &)(*callingAppLogger_)), (*msg));}
        }
    }


#ifdef TK_ENABLE_WS_EVENTING
    if (sentinelUsable_ == true)
    {
        int sentinelLevel = INFOLEVEL;
        if (this->getSentinelLogLevel() == DIAGTRACE) sentinelLevel=TRACELEVEL;
        else if (this->getSentinelLogLevel() == DIAGDEBUG) sentinelLevel=DEBUGLEVEL;
        else if (this->getSentinelLogLevel() == DIAGINFO) sentinelLevel=INFOLEVEL;
        else if (this->getSentinelLogLevel() == DIAGWARN) sentinelLevel=WARNLEVEL;
        else if (this->getSentinelLogLevel() == DIAGUSERINFO) sentinelLevel=USERINFOLEVEL;
        else if (this->getSentinelLogLevel() == DIAGERROR) sentinelLevel=ERRORLEVEL;
        else if (this->getSentinelLogLevel() == DIAGFATAL) sentinelLevel=FATALLEVEL;
        else if (this->getSentinelLogLevel() == DIAGOFF) sentinelLevel=OFFLEVEL;

		CLog * clog_ptr;
		if (msgType == MESSAGE_TYPE_IS_SHORT)
		{
			clog_ptr = &shortClog;
		}
		else clog_ptr = &longClog;

        if ( (msg_level >= sentinelLevel) && (sentinelLevel != OFFLEVEL) )
        {
			xcept::Exception tkExcept(  "xcept::Exception", clog_ptr->getMessage(), __FILE__, __LINE__, __FUNCTION__);		
			std::string notifier = toolbox::toString("%s/%s", wa_->getApplicationDescriptor()->getContextDescriptor()->getURL().c_str() , wa_->getApplicationDescriptor()->getURN().c_str()) ;
			tkExcept.setProperty("notifier", notifier );
			tkExcept.setProperty("qualifiedErrorSchemaURI", "http://xdaq.web.cern.ch/xdaq/xsd/2005/QualifiedSoftwareErrorRecord-10.xsd");
			tkExcept.setProperty("dateTime", toolbox::getDateTime() );
			tkExcept.setProperty("sessionID", "none" );
			if (clog_ptr->getLevel() == DIAGUSERINFO)
			{
				tkExcept.setProperty("severity", "INFO" );
			}
			else tkExcept.setProperty("severity", clog_ptr->getLevel() );

/*
			tkExcept.setProperty("messageType", clog_ptr->getTypeOfMessage() );
			tkExcept.setProperty("timestamp", clog_ptr->getTimestampAsString() );
			std::ostringstream number_str;
			number_str << clog_ptr->getTimeStamp();
			tkExcept.setProperty("numtimestamp", number_str.str() );
			tkExcept.setProperty("level", clog_ptr->getLevel() );
			tkExcept.setProperty("logger", clog_ptr->getLogger() );
			tkExcept.setProperty("message", clog_ptr->getMessage() );
			tkExcept.setProperty("errorCode", clog_ptr->getErrorCode() );
			tkExcept.setProperty("faultstate", clog_ptr->getFaultState() );
			tkExcept.setProperty("systemid", clog_ptr->getSystemID() );
			tkExcept.setProperty("subsystemid", clog_ptr->getSubSystemID() );
			tkExcept.setProperty("extraBuffer", clog_ptr->getExtraBuffer() );

*/
			//std::cout << "In DiagBagWizard : raising EXCEPTION" << std::endl;
			try
			{

				if (clog_ptr->getLevel() == DIAGWARN) wa_->notifyQualified("warn", tkExcept);
				if (clog_ptr->getLevel() == DIAGERROR) wa_->notifyQualified("error", tkExcept);
				if (clog_ptr->getLevel() == DIAGFATAL) wa_->notifyQualified("fatal", tkExcept);
				//std::cout << "Message sent to Sentinel" << std::endl;				

			}
			catch (xdaq::exception::Exception& e)
			{
				//std::cout << "Failed to remotly notify Log Message via Sentinel" << e.what() << std::endl;
				XCEPT_RETHROW (xgi::exception::Exception, "Failed to remotly notify Exception", e);
			}

			//std::cout << "In DiagBagWizard : EXCEPTION raised" << std::endl;
        }
    }
#endif

}




void DiagBagWizard::postXoapMessageForLclEd(std::string msgType)
{

    /* Build SOAP for sending action */
    xoap::MessageReference err_msg = xoap::createMessage();
    xoap::SOAPPart err_soap = err_msg->getSOAPPart();
    xoap::SOAPEnvelope err_envelope = err_soap.getEnvelope();

    #ifdef USE_XRELAY
        std::string err_topNode = XR_TOPNODE;
        std::string err_prefix = XR_PREFIX;
        std::string err_httpAdd = XR_HTTPADD;

        xoap::SOAPName err_envelopeName = err_envelope.getElementName();
        xoap::SOAPHeader err_header = err_envelope.addHeader();

        xoap::SOAPName err_relayName = err_envelope.createName(err_topNode, err_prefix,  err_httpAdd);
        xoap::SOAPHeaderElement err_relayElement = err_header.addHeaderElement(err_relayName);

    	// Add the actor attribute
    	xoap::SOAPName err_actorName = err_envelope.createName(XR_ACTORNAME, err_envelope.getElementName().getPrefix(), err_envelope.getElementName().getURI());
    	err_relayElement.addAttribute(err_actorName,err_httpAdd);
    #endif

    xoap::SOAPName command = err_envelope.createName(COMMAND_TO_TRIG_WHEN_ED_RECEIVE_A_LOG, "xdaq", XDAQ_NS_URI);
    xoap::SOAPBody err_body = err_envelope.getBody();
    err_body.addBodyElement(command);



    /* Add requested attributes to SOAP part */
    xoap::SOAPBody rb = err_msg->getSOAPPart().getEnvelope().getBody();
    std::vector<xoap::SOAPElement> logElement = rb.getChildElements ();


    if (msgType == MESSAGE_TYPE_IS_SHORT)
    {
        xoap::SOAPName messageType ("messageType", "", "");
        logElement[0].addAttribute(messageType,shortClog.getTypeOfMessage());
        xoap::SOAPName timestamp ("timestamp", "", "");
        logElement[0].addAttribute(timestamp,shortClog.getTimestampAsString());

        xoap::SOAPName numtimestamp ("numtimestamp", "", "");
		std::ostringstream number_str;
		number_str << shortClog.getTimeStamp();
        logElement[0].addAttribute(numtimestamp,number_str.str());
//std::cout << "In LCL MESSAGE_TYPE_IS_SHORT : number_str = " << number_str.str() << std::endl;

        xoap::SOAPName level ("level", "", "");
        logElement[0].addAttribute(level,shortClog.getLevel());
        xoap::SOAPName logger ("logger", "", "");
        logElement[0].addAttribute(logger,shortClog.getLogger());
        xoap::SOAPName message ("message", "", "");
        logElement[0].addAttribute(message,shortClog.getMessage());

        xoap::SOAPName errorCode ("errorCode", "", "");
        logElement[0].addAttribute(errorCode,shortClog.getErrorCode());
        xoap::SOAPName faultstate ("faultstate", "", "");
        logElement[0].addAttribute(faultstate,shortClog.getFaultState());
        xoap::SOAPName systemid ("systemid", "", "");
        logElement[0].addAttribute(systemid,shortClog.getSystemID());
        xoap::SOAPName subsystemid ("subsystemid", "", "");
        logElement[0].addAttribute(subsystemid,shortClog.getSubSystemID());

        xoap::SOAPName extraBuffer ("extraBuffer", "", "");
        logElement[0].addAttribute(extraBuffer,shortClog.getExtraBuffer());

        xoap::SOAPName nestedSubClassPath ("nestedSubClassPath", "", "");
        logElement[0].addAttribute(nestedSubClassPath,shortClog.getSubClassPath());

        xoap::SOAPName nestedFileName ("nestedFileName", "", "");
        logElement[0].addAttribute(nestedFileName,shortClog.getFileName());


        xoap::SOAPName nestedLineNumber ("nestedLineNumber", "", "");
		std::ostringstream lineNumber_str;
		lineNumber_str << shortClog.getTimeStamp();
        logElement[0].addAttribute(nestedLineNumber,lineNumber_str.str());




    }
    else
    {
        xoap::SOAPName messageType ("messageType", "", "");
        logElement[0].addAttribute(messageType,longClog.getTypeOfMessage());
        xoap::SOAPName timestamp ("timestamp", "", "");
        logElement[0].addAttribute(timestamp,longClog.getTimestampAsString());

        xoap::SOAPName numtimestamp ("numtimestamp", "", "");
		std::ostringstream number_str;
		number_str << longClog.getTimeStamp();
        logElement[0].addAttribute(numtimestamp,number_str.str());
//std::cout << "In LCL MESSAGE_TYPE_IS_LONG : number_str = " << number_str.str() << std::endl;

        xoap::SOAPName level ("level", "", "");
        logElement[0].addAttribute(level,longClog.getLevel());
        xoap::SOAPName logger ("logger", "", "");
        logElement[0].addAttribute(logger,longClog.getLogger());
        xoap::SOAPName message ("message", "", "");
        logElement[0].addAttribute(message,longClog.getMessage());
        xoap::SOAPName errorCode ("errorCode", "", "");
        logElement[0].addAttribute(errorCode,longClog.getErrorCode());
        xoap::SOAPName faultstate ("faultstate", "", "");
        logElement[0].addAttribute(faultstate,longClog.getFaultState());
        xoap::SOAPName systemid ("systemid", "", "");
        logElement[0].addAttribute(systemid,longClog.getSystemID());
        xoap::SOAPName subsystemid ("subsystemid", "", "");
        logElement[0].addAttribute(subsystemid,longClog.getSubSystemID());

        xoap::SOAPName extraBuffer ("extraBuffer", "", "");
        logElement[0].addAttribute(extraBuffer,longClog.getExtraBuffer());

        xoap::SOAPName nestedSubClassPath ("nestedSubClassPath", "", "");
        logElement[0].addAttribute(nestedSubClassPath,shortClog.getSubClassPath());

        xoap::SOAPName nestedFileName ("nestedFileName", "", "");
        logElement[0].addAttribute(nestedFileName,shortClog.getFileName());


        xoap::SOAPName nestedLineNumber ("nestedLineNumber", "", "");
		std::ostringstream lineNumber_str;
		lineNumber_str << shortClog.getTimeStamp();
        logElement[0].addAttribute(nestedLineNumber,lineNumber_str.str());

    }

    #ifdef USE_XRELAY
        xoap::SOAPName senderURL ("senderURL", "", "");
    	logElement[0].addAttribute(senderURL, wa_->getApplicationDescriptor()->getContextDescriptor()->getURL());
        xoap::SOAPName senderURN ("senderURN", "", "");
    	logElement[0].addAttribute(senderURN, wa_->getApplicationDescriptor()->getURN());
        xoap::SOAPName talkToLocalEd ("talkToLocalEd", "", "");
        xoap::SOAPName missedMessages ("missedMessages", "", "");

		logElement[0].addAttribute(talkToLocalEd, "true");
		std::string nb;
		char s1[100];
		snprintf(s1,20,"%i",missedLclEmissions_);
		nb = s1;
		logElement[0].addAttribute(missedMessages, nb);

    	// Add the "to" node
    	std::string err_childNode = XR_TONODENAME;
    	xoap::SOAPName err_toName = err_envelope.createName(err_childNode, err_prefix, " ");
    	xoap::SOAPElement err_childElement = err_relayElement.addChildElement(err_toName);

    	xoap::SOAPName err_urlName = err_envelope.createName(XR_URLNAME);
    	xoap::SOAPName err_urnName = err_envelope.createName(XR_URNNAME);

		err_childElement.addAttribute(err_urlName,lclEdURL_);
		err_childElement.addAttribute(err_urnName,lclEdURN_);

		/*
         std::cout << "Posting to Local ED with XRELAY :";
         err_msg->writeTo(std::cout);
         std::cout << std::endl;
		*/
        try
        {	
    	    // Get the Xrelay application descriptor and post the message:
			xdaq::ApplicationDescriptor * err_xrelay = wa_->getApplicationContext()->getFirstApplication ("XRelay")->getApplicationDescriptor();

    	    isWaitingForLclLogAck_ = true;
    	    xoap::MessageReference err_reply = wa_->getApplicationContext()->postSOAP(err_msg, *selfAppDescriptor_, *err_xrelay);
			missedLclEmissions_ = 0;
    	} 
    	catch (xdaq::exception::Exception& e) {}
    //OR Send message through XOAP
    #else
		/*
         std::cout << "Posting to Local ED with XOAP :";
         err_msg->writeTo(std::cout);
         std::cout << std::endl;
		 */




	        try
    	    {
		   	    #ifdef XDAQ373
    		    	xdaq::ApplicationDescriptor * err_d = wa_->getApplicationContext()->getDefaultZone()->getApplicationDescriptor(diagName_, atoi(diagInstance_.c_str()));
	    	    #else
    		    	xdaq::ApplicationDescriptor * err_d = wa_->getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(diagName_, atoi(diagInstance_.c_str()));
    		    #endif

/*

				if (lclClogsList_.size() > 0)
				{
					//std::cout << "Flushing stored logs buffer..." << std::endl;
					while (lclClogsList_.empty() == false)
					{
						xoap::MessageReference tmpClog = *(lclClogsList_.end() - 1);
						//std::cout << "message extracted ; now posting" << std::endl;
				        try
				        {
				            wa_->getApplicationContext()->postSOAP(tmpClog, *selfAppDescriptor_, *err_d);
				    	}
				    	catch (xdaq::exception::Exception& e){}
						//std::cout << "Extracted message posted" << std::endl;
						lclClogsList_.pop_back();
					}
				}


*/

	            wa_->getApplicationContext()->postSOAP(err_msg, *selfAppDescriptor_, *err_d);
				//std::cout << "Soap message posted" << std::endl;

			}
			catch (xdaq::exception::Exception& e)
			{
				//std::cout << "SoapPost went wrong ; pushing Clog to storage area" << std::endl;
/*
				if (lclClogsList_.size() < NB_MAX_MSG_IN_GED_BUFFER)
				{

					std::vector<xoap::MessageReference>::iterator theIterator = lclClogsList_.begin();
					lclClogsList_.insert( theIterator, err_msg );

//					lclClogsList_.push_back(err_msg);
				}
*/
				//else std::cout << "Cannot push back ; vector full" << std::endl;
			}



/*


		try
		{	
			#ifdef XDAQ373
				xdaq::ApplicationDescriptor * err_d = wa_->getApplicationContext()->getDefaultZone()->getApplicationDescriptor(diagName_, atoi(diagInstance_.c_str()));
			#else
			    xdaq::ApplicationDescriptor * err_d = wa_->getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(diagName_, atoi(diagInstance_.c_str()));
			#endif
			wa_->getApplicationContext()->postSOAP(err_msg, *selfAppDescriptor_, *err_d);
		}
		catch (xdaq::exception::Exception& e) {}
*/
    #endif
}







void DiagBagWizard::postXoapMessageForGlbEd(std::string msgType)
{

    /* Build SOAP for sending action */
    xoap::MessageReference err_msg = xoap::createMessage();
    xoap::SOAPPart err_soap = err_msg->getSOAPPart();
    xoap::SOAPEnvelope err_envelope = err_soap.getEnvelope();

    #ifdef USE_XRELAY
        std::string err_topNode = XR_TOPNODE;
        std::string err_prefix = XR_PREFIX;
        std::string err_httpAdd = XR_HTTPADD;

        xoap::SOAPName err_envelopeName = err_envelope.getElementName();
        xoap::SOAPHeader err_header = err_envelope.addHeader();

        xoap::SOAPName err_relayName = err_envelope.createName(err_topNode, err_prefix,  err_httpAdd);
        xoap::SOAPHeaderElement err_relayElement = err_header.addHeaderElement(err_relayName);

    	// Add the actor attribute
    	xoap::SOAPName err_actorName = err_envelope.createName(XR_ACTORNAME, err_envelope.getElementName().getPrefix(), err_envelope.getElementName().getURI());
    	err_relayElement.addAttribute(err_actorName,err_httpAdd);
    #endif

    xoap::SOAPName command = err_envelope.createName(COMMAND_TO_TRIG_WHEN_ED_RECEIVE_A_LOG, "xdaq", XDAQ_NS_URI);
    xoap::SOAPBody err_body = err_envelope.getBody();
    err_body.addBodyElement(command);

    /* Add requested attributes to SOAP part */
    xoap::SOAPBody rb = err_msg->getSOAPPart().getEnvelope().getBody();
    std::vector<xoap::SOAPElement> logElement = rb.getChildElements ();

    if (msgType == MESSAGE_TYPE_IS_SHORT)
    {
        xoap::SOAPName messageType ("messageType", "", "");
        logElement[0].addAttribute(messageType,shortClog.getTypeOfMessage());
        xoap::SOAPName timestamp ("timestamp", "", "");
        logElement[0].addAttribute(timestamp,shortClog.getTimestampAsString());

        xoap::SOAPName numtimestamp ("numtimestamp", "", "");
		std::ostringstream number_str;
		number_str << shortClog.getTimeStamp();
        logElement[0].addAttribute(numtimestamp,number_str.str());
//std::cout << "In GLB MESSAGE_TYPE_IS_SHORT : number_str = " << number_str.str() << std::endl;

        xoap::SOAPName level ("level", "", "");
        logElement[0].addAttribute(level,shortClog.getLevel());
        xoap::SOAPName logger ("logger", "", "");
        logElement[0].addAttribute(logger,shortClog.getLogger());
        xoap::SOAPName message ("message", "", "");
        logElement[0].addAttribute(message,shortClog.getMessage());

        xoap::SOAPName errorCode ("errorCode", "", "");
        logElement[0].addAttribute(errorCode,shortClog.getErrorCode());
        xoap::SOAPName faultstate ("faultstate", "", "");
        logElement[0].addAttribute(faultstate,shortClog.getFaultState());
        xoap::SOAPName systemid ("systemid", "", "");
        logElement[0].addAttribute(systemid,shortClog.getSystemID());
        xoap::SOAPName subsystemid ("subsystemid", "", "");
        logElement[0].addAttribute(subsystemid,shortClog.getSubSystemID());

        xoap::SOAPName extraBuffer ("extraBuffer", "", "");
        logElement[0].addAttribute(extraBuffer,shortClog.getExtraBuffer());

        xoap::SOAPName nestedSubClassPath ("nestedSubClassPath", "", "");
        logElement[0].addAttribute(nestedSubClassPath,shortClog.getSubClassPath());

        xoap::SOAPName nestedFileName ("nestedFileName", "", "");
        logElement[0].addAttribute(nestedFileName,shortClog.getFileName());


        xoap::SOAPName nestedLineNumber ("nestedLineNumber", "", "");
		std::ostringstream lineNumber_str;
//std::cout << "Getting raw line number : " << shortClog.getLineNumber() << std::endl;
		lineNumber_str << shortClog.getLineNumber();
//std::cout << "In ostringstream line number is : " << lineNumber_str << std::endl;
        logElement[0].addAttribute(nestedLineNumber,lineNumber_str.str());
//std::cout << "In SOAP line number is : " << lineNumber_str.str() << std::endl;

    }
    else
    {
        xoap::SOAPName messageType ("messageType", "", "");
        logElement[0].addAttribute(messageType,longClog.getTypeOfMessage());
        xoap::SOAPName timestamp ("timestamp", "", "");
        logElement[0].addAttribute(timestamp,longClog.getTimestampAsString());

        xoap::SOAPName numtimestamp ("numtimestamp", "", "");
		std::ostringstream number_str;
		number_str << longClog.getTimeStamp();
        logElement[0].addAttribute(numtimestamp,number_str.str());
//std::cout << "In GLB MESSAGE_TYPE_IS_LONG : number_str = " << number_str.str() << std::endl;

        xoap::SOAPName level ("level", "", "");
        logElement[0].addAttribute(level,longClog.getLevel());
        xoap::SOAPName logger ("logger", "", "");
        logElement[0].addAttribute(logger,longClog.getLogger());
        xoap::SOAPName message ("message", "", "");
        logElement[0].addAttribute(message,longClog.getMessage());
        xoap::SOAPName errorCode ("errorCode", "", "");
        logElement[0].addAttribute(errorCode,longClog.getErrorCode());
        xoap::SOAPName faultstate ("faultstate", "", "");
        logElement[0].addAttribute(faultstate,longClog.getFaultState());
        xoap::SOAPName systemid ("systemid", "", "");
        logElement[0].addAttribute(systemid,longClog.getSystemID());
        xoap::SOAPName subsystemid ("subsystemid", "", "");
        logElement[0].addAttribute(subsystemid,longClog.getSubSystemID());


        xoap::SOAPName extraBuffer ("extraBuffer", "", "");
        logElement[0].addAttribute(extraBuffer,longClog.getExtraBuffer());

        xoap::SOAPName nestedSubClassPath ("nestedSubClassPath", "", "");
        logElement[0].addAttribute(nestedSubClassPath,shortClog.getSubClassPath());

        xoap::SOAPName nestedFileName ("nestedFileName", "", "");
        logElement[0].addAttribute(nestedFileName,shortClog.getFileName());


        xoap::SOAPName nestedLineNumber ("nestedLineNumber", "", "");
		std::ostringstream lineNumber_str;
//std::cout << "Getting raw line number : " << shortClog.getLineNumber() << std::endl;
		lineNumber_str << shortClog.getLineNumber();
//std::cout << "In ostringstream line number is : " << lineNumber_str << std::endl;
        logElement[0].addAttribute(nestedLineNumber,lineNumber_str.str());
//std::cout << "In SOAP line number is : " << lineNumber_str.str() << std::endl;

    }
    
    
    #ifdef USE_XRELAY
        xoap::SOAPName senderURL ("senderURL", "", "");
    	logElement[0].addAttribute(senderURL, wa_->getApplicationDescriptor()->getContextDescriptor()->getURL());
        xoap::SOAPName senderURN ("senderURN", "", "");
    	logElement[0].addAttribute(senderURN, wa_->getApplicationDescriptor()->getURN());
        xoap::SOAPName talkToLocalEd ("talkToLocalEd", "", "");
        xoap::SOAPName missedMessages ("missedMessages", "", "");

		logElement[0].addAttribute(talkToLocalEd, "false");
		std::string nb;
		char s1[100];
		snprintf(s1,20,"%i",missedGlbEmissions_);
		nb = s1;
		logElement[0].addAttribute(missedMessages, nb);


    	// Add the "to" node
    	std::string err_childNode = XR_TONODENAME;
		xoap::SOAPName err_toName = err_envelope.createName(err_childNode, err_prefix, " ");
		xoap::SOAPElement err_childElement = err_relayElement.addChildElement(err_toName);

    	xoap::SOAPName err_urlName = err_envelope.createName(XR_URLNAME);
    	xoap::SOAPName err_urnName = err_envelope.createName(XR_URNNAME);

		err_childElement.addAttribute(err_urlName,glbEdURL_);
		err_childElement.addAttribute(err_urnName,glbEdURN_);

		/*
		std::cout << "Posting to Global ED with XRELAY :";
		err_msg->writeTo(std::cout);
		std::cout << std::endl;
		*/
        try
        {	
    	    // Get the Xrelay application descriptor and post the message:
			xdaq::ApplicationDescriptor * err_xrelay = wa_->getApplicationContext()->getFirstApplication ("XRelay")->getApplicationDescriptor();

			isWaitingForGlbLogAck_ = true;
    	    xoap::MessageReference err_reply = wa_->getApplicationContext()->postSOAP(err_msg, *selfAppDescriptor_, *err_xrelay);
			missedGlbEmissions_ = 0;
    	} 
    	catch (xdaq::exception::Exception& e) {}
    //OR Send message through XOAP
    #else
/*
         std::cout << "Posting to Global ED with XOAP :";
         err_msg->writeTo(std::cout);
         std::cout << std::endl;
*/


	        try
    	    {
		   	    #ifdef XDAQ373
    		    	xdaq::ApplicationDescriptor * err_d = wa_->getApplicationContext()->getDefaultZone()->getApplicationDescriptor(diagGlbName_, atoi(diagGlbInstance_.c_str()));
	    	    #else
    		    	xdaq::ApplicationDescriptor * err_d = wa_->getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(diagGlbName_, atoi(diagGlbInstance_.c_str()));
    		    #endif

/*

				if (glbClogsList_.size() > 0)
				{
					//std::cout << "Flushing stored logs buffer..." << std::endl;
					while (glbClogsList_.empty() == false)
					{
						xoap::MessageReference tmpClog = *(glbClogsList_.end() - 1);
						//std::cout << "message extracted ; now posting" << std::endl;
				        try
				        {
				            wa_->getApplicationContext()->postSOAP(tmpClog, *selfAppDescriptor_, *err_d);

				    	}
				    	catch (xdaq::exception::Exception& e){}
						//std::cout << "Extracted message posted" << std::endl;
						glbClogsList_.pop_back();

					}
				}

*/

	            wa_->getApplicationContext()->postSOAP(err_msg, *selfAppDescriptor_, *err_d);
				//std::cout << "Soap message posted" << std::endl;



			}
			catch (xdaq::exception::Exception& e)
			{
/*
				//std::cout << "SoapPost went wrong ; pushing Clog to storage area" << std::endl;
				if (glbClogsList_.size() < NB_MAX_MSG_IN_GED_BUFFER)
				{

					std::vector<xoap::MessageReference>::iterator theIterator = glbClogsList_.begin();
					glbClogsList_.insert( theIterator, err_msg );

//					glbClogsList_.push_back(err_msg);
				}
				//else std::cout << "Cannot push back ; vector full" << std::endl;
*/
			}


/*
        try
        {
    	    #ifdef XDAQ373
    	    	xdaq::ApplicationDescriptor * err_d = wa_->getApplicationContext()->getDefaultZone()->getApplicationDescriptor(diagGlbName_, atoi(diagGlbInstance_.c_str()));
    	    #else
    	    	xdaq::ApplicationDescriptor * err_d = wa_->getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(diagGlbName_, atoi(diagGlbInstance_.c_str()));
    	    #endif
            wa_->getApplicationContext()->postSOAP(err_msg, *selfAppDescriptor_, *err_d);
    	}
    	catch (xdaq::exception::Exception& e){}
*/
    #endif
}








































bool DiagBagWizard::checkUseDiagSystem()
{

//std::cout << "useDiagSystem_ is : " << useDiagSystem_ << std::endl;
/*
	if (activateDiagSystemForFirstTime_ == true)
	{
*/
    	if ( this->getIsInUserProcess() == true )
		{
			//std::cout << "I am in UP" << std::endl;

			if (useDiagSystem_ == "default") return setAppDiagSystemToDefault();
			if (useDiagSystem_ == "off") return setAppDiagSystemToOff();
			if (useDiagSystem_ == "tuned") return setDiagSystemToTuned(0);
			if (useDiagSystem_ == "oldway") return false; //Use fireitems value if any
			if (useDiagSystem_ == "hyperdaq") return false; //Use hyperdaq value if any
		}

	    if (this->getIsInErrorDispatcher() == true)
		{
			//std::cout << "I am in ED" << std::endl;

			if (useDiagSystem_ == "default") return setEdDiagSystemToDefault();
			if (useDiagSystem_ == "off") return setEdDiagSystemToOff();
			if (useDiagSystem_ == "tuned") return setDiagSystemToTuned(3);
			if (useDiagSystem_ == "oldway") return false;
			if (useDiagSystem_ == "hyperdaq") return false; //Use hyperdaq value if any
		}

	    if ( (this->getIsInGlobalErrorDispatcher() == true) || (this->getIsInErrorDispatcher() == true) )
		{
			//std::cout << "I am in GLB-ED" << std::endl;

			if (useDiagSystem_ == "default") return setGlbEdDiagSystemToDefault();
			if (useDiagSystem_ == "off") return setGlbEdDiagSystemToOff();
			if (useDiagSystem_ == "tuned") return setDiagSystemToTuned(1);
			if (useDiagSystem_ == "oldway") return false;
			if (useDiagSystem_ == "hyperdaq") return false; //Use hyperdaq value if any
		}

	    if ( this->getIsInLogsReader() == true )
		{
			//std::cout << "I am in LOGREADER" << std::endl;
			if (useDiagSystem_ == "default") return setLrDiagSystemToDefault();
			if (useDiagSystem_ == "off") return setLrDiagSystemToOff();
			if (useDiagSystem_ == "tuned") return setDiagSystemToTuned(2);
			if (useDiagSystem_ == "oldway") return false;
			if (useDiagSystem_ == "hyperdaq") return false; //Use hyperdaq value if any
		}


	    if ( this->getIsInErrorManager() == true )
		{
			//std::cout << "I am in LOGREADER" << std::endl;
			if (useDiagSystem_ == "default") return setEmDiagSystemToDefault();
			if (useDiagSystem_ == "off") return setEmDiagSystemToOff();
			if (useDiagSystem_ == "tuned") return setDiagSystemToTuned(4);
			if (useDiagSystem_ == "oldway") return false;
			if (useDiagSystem_ == "hyperdaq") return false; //Use hyperdaq value if any
		}

		return false;
/*
	}
	else return false;
*/
}






bool DiagBagWizard::setAppDiagSystemToOff()
{
	useDefaultAppender_ = true;
	defaultAppenderUsable_ = true;
	defaultAppenderStatus_ = "checked";
	gLogLevel_ = DIAGUSERINFO;
			
	useConsoleDump_ = false;

	useSentinel_ = false;
	sentinelUsable_ = false;
	sentinelUsableStatus_ = "unchecked";
	contextHasSentinel_ = false;

	useDiag_ = false;
	edUsable_ = false;
	useDiagStatus_ = "unchecked";
	diagNameStatus_ = "unchecked";
	diagLIDStatus_ = "unchecked";
	diagIPStatus_ = "unchecked";

	useGlbDiag_ = false;
	edGlbUsable_ = false;
	useGlbDiagStatus_ = "unchecked";
	diagGlbNameStatus_ = "unchecked";
	diagGlbLIDStatus_ = "unchecked";
	diagGlbIPStatus_ = "unchecked";

	return true;
}


bool DiagBagWizard::setEdDiagSystemToOff()
{
	useDefaultAppender_ = true;
	defaultAppenderUsable_ = true;
	defaultAppenderStatus_ = "checked";
	gLogLevel_ = DIAGUSERINFO;

	useConsoleDump_ = false;

	useSentinel_ = false;
	sentinelUsable_ = false;
	sentinelUsableStatus_ = "unchecked";
	contextHasSentinel_ = false;

	useDiag_ = false;
	edUsable_ = false;
	useDiagStatus_ = "unchecked";
	diagNameStatus_ = "unchecked";
	diagLIDStatus_ = "unchecked";
	diagIPStatus_ = "unchecked";

	useGlbDiag_ = false;
	edGlbUsable_ = false;
	useGlbDiagStatus_ = "unchecked";
	diagGlbNameStatus_ = "unchecked";
	diagGlbLIDStatus_ = "unchecked";
	diagGlbIPStatus_ = "unchecked";

for (int csStreamNum = 0; csStreamNum < nbrOfChainsawStreams_ ; csStreamNum++)
{

	csRoute_[csStreamNum] = false;
	csRouteValidated_[csStreamNum] = false;
	csReceiverStatus_[csStreamNum] = "unchecked";
	csIPStatus_[csStreamNum] = "unchecked";
	csPortStatus_[csStreamNum] = "unchecked";
}
	ccRoute_ = false;
	ccRouteValidated_ = false;
	ccRouteStatus_ = "unchecked";
	
	dbRoute_ = false;
	dbRouteValidated_ = false;
	dbRouteStatus_ = "unchecked";
	dbLogLevel_ = DIAGERROR;
	dbFileName_ = "/tmp/errorsLocalDbSwapFile.txt";

	useFileLogging_ = false;
	fileLoggingUsable_ = false;
	fileLoggingStatus_ = "unchecked";
	fileLogLevel_ = DIAGERROR;
	logFileName_ = "/tmp/errorsLocalLogFile.txt";

//	activateDiagSystemForFirstTime_ = false;
	return true;
}


bool DiagBagWizard::setGlbEdDiagSystemToOff()
{
	useDefaultAppender_ = true;
	defaultAppenderUsable_ = true;
	defaultAppenderStatus_ = "checked";
	gLogLevel_ = DIAGUSERINFO;

	useConsoleDump_ = false;

	useSentinel_ = false;
	sentinelUsable_ = false;
	sentinelUsableStatus_ = "unchecked";
	contextHasSentinel_ = false;

	useDiag_ = false;
	edUsable_ = false;
	useDiagStatus_ = "unchecked";
	diagNameStatus_ = "unchecked";
	diagLIDStatus_ = "unchecked";
	diagIPStatus_ = "unchecked";

	useGlbDiag_ = false;
	edGlbUsable_ = false;
	useGlbDiagStatus_ = "unchecked";
	diagGlbNameStatus_ = "unchecked";
	diagGlbLIDStatus_ = "unchecked";
	diagGlbIPStatus_ = "unchecked";

for (int csStreamNum = 0; csStreamNum < nbrOfChainsawStreams_ ; csStreamNum++)
{

	csRoute_[csStreamNum] = false;
	csRouteValidated_[csStreamNum] = false;
	csReceiverStatus_[csStreamNum] = "unchecked";
	csIPStatus_[csStreamNum] = "unchecked";
	csPortStatus_[csStreamNum] = "unchecked";
}
	ccRoute_ = false;
	ccRouteValidated_ = false;
	ccRouteStatus_ = "unchecked";
	
	dbRoute_ = false;
	dbRouteValidated_ = false;
	dbRouteStatus_ = "unchecked";
	dbLogLevel_ = DIAGERROR;
	dbFileName_ = "/tmp/errorsDbSwapFile.txt";

	useFileLogging_ = false;
	fileLoggingUsable_ = false;
	fileLoggingStatus_ = "unchecked";
	fileLogLevel_ = DIAGERROR;
	logFileName_ = "/tmp/errorsLogFile.txt";

//	activateDiagSystemForFirstTime_ = false;
	return true;
}



bool DiagBagWizard::setLrDiagSystemToOff()
{
	useDefaultAppender_ = true;
	defaultAppenderUsable_ = true;
	defaultAppenderStatus_ = "checked";
	gLogLevel_ = DIAGUSERINFO;
			
	useConsoleDump_ = false;

	useSentinel_ = false;
	sentinelUsable_ = false;
	sentinelUsableStatus_ = "unchecked";
	contextHasSentinel_ = false;

	useDiag_ = false;
	edUsable_ = false;
	useDiagStatus_ = "unchecked";
	diagNameStatus_ = "unchecked";
	diagLIDStatus_ = "unchecked";
	diagIPStatus_ = "unchecked";

	useGlbDiag_ = false;
	edGlbUsable_ = false;
	useGlbDiagStatus_ = "unchecked";
	diagGlbNameStatus_ = "unchecked";
	diagGlbLIDStatus_ = "unchecked";
	diagGlbIPStatus_ = "unchecked";

	useFileLogging_ = false;
	fileLoggingUsable_ = false;
	fileLoggingStatus_ = "unchecked";
	fileLogLevel_ = DIAGERROR;
	logFileName_ = "/tmp/errorsLogFile.txt";

//	activateDiagSystemForFirstTime_ = false;
	return true;
}


bool DiagBagWizard::setEmDiagSystemToOff()
{
	useDefaultAppender_ = true;
	defaultAppenderUsable_ = true;
	defaultAppenderStatus_ = "checked";
	gLogLevel_ = DIAGUSERINFO;
			
	useConsoleDump_ = false;

	useSentinel_ = false;
	sentinelUsable_ = false;
	sentinelUsableStatus_ = "unchecked";
	contextHasSentinel_ = false;

	useDiag_ = false;
	edUsable_ = false;
	useDiagStatus_ = "unchecked";
	diagNameStatus_ = "unchecked";
	diagLIDStatus_ = "unchecked";
	diagIPStatus_ = "unchecked";

	useGlbDiag_ = false;
	edGlbUsable_ = false;
	useGlbDiagStatus_ = "unchecked";
	diagGlbNameStatus_ = "unchecked";
	diagGlbLIDStatus_ = "unchecked";
	diagGlbIPStatus_ = "unchecked";

	return true;
}






bool DiagBagWizard::setAppDiagSystemToDefault()
{

//std::cout << "Entering Default for App" << std::endl;
    	    xdaq::ApplicationDescriptor * d;
        	try
	        {
    		    #ifdef XDAQ373
    				d = wa_->getApplicationContext()->getDefaultZone()->getApplicationDescriptor("GlobalErrorDispatcher", 0);
    	    	#else
			    	d = wa_->getApplicationContext()->getApplicationGroup()->getApplicationDescriptor("GlobalErrorDispatcher", 0);
				#endif

				#ifdef USE_XRELAY
					glbEdURL_ = d->getContextDescriptor()->getURL();
					glbEdURN_ = d->getURN();
				#endif

				// Set GLOBAL ED settings
				useGlbDiag_ = true;
				edGlbUsable_= true;
				edGlbLogLevel_ = DIAGWARN;
				
	    	    diagGlbIPStatus_="checked";
    	    	diagGlbNameStatus_ = "checked";
	    	    diagGlbLIDStatus_="checked";
				useGlbDiagStatus_ = "checked";
				diagGlbPortStatus_ = "checked";
				diagGlbInstanceStatus_ = "checked";

				diagGlbName_ = "GlobalErrorDispatcher";
				diagGlbInstance_ = "0";


				//Set other parameters to OFF
				useDefaultAppender_ = true;
				defaultAppenderUsable_ = true;
				defaultAppenderStatus_ = "checked";
				gLogLevel_ = DIAGWARN;
			
				useConsoleDump_ = false;

				useSentinel_ = false;
				sentinelUsable_ = false;
				sentinelUsableStatus_ = "unchecked";
				contextHasSentinel_ = false;
				sentinelLogLevel_ = DIAGWARN;

				useDiag_ = false;
				edUsable_ = false;
				useDiagStatus_ = "unchecked";
				diagNameStatus_ = "unchecked";
				diagLIDStatus_ = "unchecked";
				diagIPStatus_ = "unchecked";

				emUsable_ = false;
				return true;
			}
        	catch (xdaq::exception::Exception& e)
			{
//				diagSystemStatus_  = "unchecked";
	    	    log4cplus::Logger & e_logger = ((log4cplus::Logger &)(*callingAppLogger_));
				LOG4CPLUS_ERROR(e_logger, "Cannot detect Global Error Dispatcher process ; Messages will be logged on console.");
				useDiagSystem_ = "off";
				return setAppDiagSystemToOff();
			}


}



bool DiagBagWizard::setEdDiagSystemToDefault()
{
//std::cout << "Entering Default for GLB" << std::endl;

				// Set GLOBAL ED settings
				useGlbDiag_ = false;
				edGlbUsable_= false;
				edGlbLogLevel_ = DIAGWARN;
				
	    	    diagGlbIPStatus_="unchecked";
    	    	diagGlbNameStatus_ = "unchecked";
	    	    diagGlbLIDStatus_="unchecked";
				useGlbDiagStatus_ = "unchecked";
				diagGlbPortStatus_ = "unchecked";
				diagGlbInstanceStatus_ = "unchecked";

				diagGlbName_ = "GlobalErrorDispatcher";
				diagGlbInstance_ = "0";


				//Set other parameters to OFF
				useDefaultAppender_ = true;
				defaultAppenderUsable_ = true;
				defaultAppenderStatus_ = "checked";
				gLogLevel_ = DIAGWARN;
			
				useConsoleDump_ = false;

				useSentinel_ = false;
				sentinelUsable_ = false;
				sentinelUsableStatus_ = "unchecked";
				contextHasSentinel_ = false;
				sentinelLogLevel_ = DIAGWARN;

				useDiag_ = false;
				edUsable_ = false;
				useDiagStatus_ = "unchecked";
				diagNameStatus_ = "unchecked";
				diagLIDStatus_ = "unchecked";
				diagIPStatus_ = "unchecked";

for (int csStreamNum = 0; csStreamNum < nbrOfChainsawStreams_ ; csStreamNum++)
{

	csRoute_[csStreamNum] = false;
	csRouteValidated_[csStreamNum] = false;
	csReceiverStatus_[csStreamNum] = "unchecked";
	csIPStatus_[csStreamNum] = "unchecked";
	csPortStatus_[csStreamNum] = "unchecked";
}
	ccRoute_ = false;
	ccRouteValidated_ = false;
	ccRouteStatus_ = "unchecked";
	
	dbRoute_ = false;
	dbRouteValidated_ = false;
	dbRouteStatus_ = "unchecked";
	dbLogLevel_ = DIAGTRACE;
	dbFileName_ = "";

	useFileLogging_ = false;
	fileLoggingUsable_ = false;
	fileLoggingStatus_ = "unchecked";
	fileLogLevel_ = DIAGTRACE;
	logFileName_ = "";

//	activateDiagSystemForFirstTime_ = false;
//std::cout << "Exiting Default for GLB" << std::endl;
				emUsable_ = false;

	return true;
}




bool DiagBagWizard::setGlbEdDiagSystemToDefault()
{
//std::cout << "Entering Default for GLB" << std::endl;

				// Set GLOBAL ED settings
				useGlbDiag_ = false;
				edGlbUsable_= false;
				edGlbLogLevel_ = DIAGUSERINFO;
				
	    	    diagGlbIPStatus_="unchecked";
    	    	diagGlbNameStatus_ = "unchecked";
	    	    diagGlbLIDStatus_="unchecked";
				useGlbDiagStatus_ = "unchecked";
				diagGlbPortStatus_ = "unchecked";
				diagGlbInstanceStatus_ = "unchecked";

				diagGlbName_ = "GlobalErrorDispatcher";
				diagGlbInstance_ = "0";


				//Set other parameters to OFF
				useDefaultAppender_ = true;
				defaultAppenderUsable_ = true;
				defaultAppenderStatus_ = "checked";
				gLogLevel_ = DIAGWARN;
			
				useConsoleDump_ = false;

				useSentinel_ = false;
				sentinelUsable_ = false;
				sentinelUsableStatus_ = "unchecked";
				contextHasSentinel_ = false;
				sentinelLogLevel_ = DIAGWARN;

				useDiag_ = false;
				edUsable_ = false;
				useDiagStatus_ = "unchecked";
				diagNameStatus_ = "unchecked";
				diagLIDStatus_ = "unchecked";
				diagIPStatus_ = "unchecked";

for (int csStreamNum = 0; csStreamNum < nbrOfChainsawStreams_ ; csStreamNum++)
{

	csRoute_[csStreamNum] = false;
	csRouteValidated_[csStreamNum] = false;
	csReceiverStatus_[csStreamNum] = "unchecked";
	csIPStatus_[csStreamNum] = "unchecked";
	csPortStatus_[csStreamNum] = "unchecked";
}
	//Override first CS stream parameters with env settings if any
if ( useCsEnvSettings_ == true )
{
	overrideCsSettings();
}

	ccRoute_ = false;
	ccRouteValidated_ = false;
	ccRouteStatus_ = "unchecked";
	
	dbRoute_ = false;
	dbRouteValidated_ = false;
	dbRouteStatus_ = "unchecked";
	dbLogLevel_ = DIAGTRACE;
	dbFileName_ = "";

	useFileLogging_ = false;
	fileLoggingUsable_ = false;
	fileLoggingStatus_ = "unchecked";
	fileLogLevel_ = DIAGTRACE;
//	logFileName_ = "/tmp/errorsLogFile.txt";
	logFileName_ = "";

//	activateDiagSystemForFirstTime_ = false;
//std::cout << "Exiting Default for GLB" << std::endl;
				emUsable_ = false;

	return true;
}

bool DiagBagWizard::setLrDiagSystemToDefault()
{
	xdaq::ApplicationDescriptor * d;
	try
	{
    		    #ifdef XDAQ373
    				d = wa_->getApplicationContext()->getDefaultZone()->getApplicationDescriptor("GlobalErrorDispatcher", 0);
    	    	#else
			    	d = wa_->getApplicationContext()->getApplicationGroup()->getApplicationDescriptor("GlobalErrorDispatcher", 0);
				#endif

				#ifdef USE_XRELAY
					glbEdURL_ = d->getContextDescriptor()->getURL();
					glbEdURN_ = d->getURN();
				#endif

				// Set GLOBAL ED settings
				useGlbDiag_ = true;
				edGlbUsable_= true;
				edGlbLogLevel_ = DIAGWARN;
				
	    	    diagGlbIPStatus_="checked";
    	    	diagGlbNameStatus_ = "checked";
	    	    diagGlbLIDStatus_="checked";
				useGlbDiagStatus_ = "checked";
				diagGlbPortStatus_ = "checked";
				diagGlbInstanceStatus_ = "checked";

				diagGlbName_ = "GlobalErrorDispatcher";
				diagGlbInstance_ = "0";


				//Set other parameters to OFF
				useDefaultAppender_ = true;
				defaultAppenderUsable_ = true;
				defaultAppenderStatus_ = "checked";
				gLogLevel_ = DIAGWARN;
			
				useConsoleDump_ = false;

				useSentinel_ = false;
				sentinelUsable_ = false;
				sentinelUsableStatus_ = "unchecked";
				contextHasSentinel_ = false;
				sentinelLogLevel_ = DIAGWARN;

				useDiag_ = false;
				edUsable_ = false;
				useDiagStatus_ = "unchecked";
				diagNameStatus_ = "unchecked";
				diagLIDStatus_ = "unchecked";
				diagIPStatus_ = "unchecked";

for (int csStreamNum = 0; csStreamNum < nbrOfChainsawStreams_ ; csStreamNum++)
{

	csRoute_[csStreamNum] = false;
	csRouteValidated_[csStreamNum] = false;
	csReceiverStatus_[csStreamNum] = "unchecked";
	csIPStatus_[csStreamNum] = "unchecked";
	csPortStatus_[csStreamNum] = "unchecked";
}
	ccRoute_ = false;
	ccRouteValidated_ = false;
	ccRouteStatus_ = "unchecked";
	
	dbRoute_ = false;
	dbRouteValidated_ = false;
	dbRouteStatus_ = "unchecked";
	dbLogLevel_ = DIAGTRACE;
	dbFileName_ = "/tmp/errorsDbSwapFile.txt";

	useFileLogging_ = false;
	fileLoggingUsable_ = false;
	fileLoggingStatus_ = "unchecked";
	fileLogLevel_ = DIAGTRACE;
//	logFileName_ = "/tmp/errorsLogFile.txt";
	logFileName_ = "";

//	activateDiagSystemForFirstTime_ = false;
	return true;
	}
	catch (xdaq::exception::Exception& e)
	{
		log4cplus::Logger & e_logger = ((log4cplus::Logger &)(*callingAppLogger_));
		LOG4CPLUS_ERROR(e_logger, "Cannot detect Global Error Dispatcher process ; Messages will be logged on console.");
		useDiagSystem_ = "off";
		return setLrDiagSystemToOff();
	}

}


bool DiagBagWizard::setEmDiagSystemToDefault()
{

//std::cout << "Entering Default for App" << std::endl;
    	    xdaq::ApplicationDescriptor * d;
        	try
	        {
    		    #ifdef XDAQ373
    				d = wa_->getApplicationContext()->getDefaultZone()->getApplicationDescriptor("GlobalErrorDispatcher", 0);
    	    	#else
			    	d = wa_->getApplicationContext()->getApplicationGroup()->getApplicationDescriptor("GlobalErrorDispatcher", 0);
				#endif

				#ifdef USE_XRELAY
					glbEdURL_ = d->getContextDescriptor()->getURL();
					glbEdURN_ = d->getURN();
				#endif

				// Set GLOBAL ED settings
				useGlbDiag_ = true;
				edGlbUsable_= true;
				edGlbLogLevel_ = DIAGWARN;
				
	    	    diagGlbIPStatus_="checked";
    	    	diagGlbNameStatus_ = "checked";
	    	    diagGlbLIDStatus_="checked";
				useGlbDiagStatus_ = "checked";
				diagGlbPortStatus_ = "checked";
				diagGlbInstanceStatus_ = "checked";

				diagGlbName_ = "GlobalErrorDispatcher";
				diagGlbInstance_ = "0";


				//Set other parameters to OFF
				useDefaultAppender_ = true;
				defaultAppenderUsable_ = true;
				defaultAppenderStatus_ = "checked";
				gLogLevel_ = DIAGWARN;
			
				useConsoleDump_ = false;
/*
				useSentinel_ = true;
				sentinelUsable_ = true;
				sentinelUsableStatus_ = "checked";
				contextHasSentinel_ = true;
				sentinelLogLevel_ = DIAGWARN;
*/
				useSentinel_ = false;
				sentinelUsable_ = false;
				sentinelUsableStatus_ = "unchecked";
				contextHasSentinel_ = false;
				sentinelLogLevel_ = DIAGWARN;

				useDiag_ = false;
				edUsable_ = false;
				useDiagStatus_ = "unchecked";
				diagNameStatus_ = "unchecked";
				diagLIDStatus_ = "unchecked";
				diagIPStatus_ = "unchecked";

				emUsable_ = false;
				return true;
			}
        	catch (xdaq::exception::Exception& e)
			{
//				diagSystemStatus_  = "unchecked";
	    	    log4cplus::Logger & e_logger = ((log4cplus::Logger &)(*callingAppLogger_));
    	    	LOG4CPLUS_ERROR(e_logger, "Cannot detect Global Error Dispatcher process ; Messages will be logged on console.");
				useDiagSystem_ = "off";
				return setEmDiagSystemToOff();
			}


}


bool DiagBagWizard::setDiagSystemToTuned(int process_kind)
{


			FILE * confFile;
			char oneWord[256];
//DEBUGME
//std::cout << "Working with configuration file : " << diagSystemSettings_ << std::endl;
			confFile = fopen(diagSystemSettings_.c_str(), "r");



			if (confFile != NULL)
			{
			
				if (process_kind == 0) //USER PROCESS
				{
					setAppDiagSystemToOff();
				}
				if (process_kind == 1) //ED or GLB_ED
				{
					setGlbEdDiagSystemToOff();
				}
				if (process_kind == 2) // LR
				{
					setLrDiagSystemToOff();
				}

			while (!(feof(confFile)))
			{
				
				fscanf(confFile,"%s",oneWord);

				/****** get rid of commented lines *******/
				if ( strcmp(oneWord, "##") == 0 )
				{
					fgets(oneWord, 255, confFile);
				}


				/****** DEFAULT APPENDER PARAMETERS *******/
				if ( strcmp(oneWord, "UseDefaultAppender") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					if ( strcmp(oneWord, "TRUE") == 0 )
					{
						useDefaultAppender_ = true;
					}
					else  useDefaultAppender_ = false;
				}

				if ( strcmp(oneWord, "DefaultAppenderLogLevel") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					gLogLevel_ = oneWord;
				}
				

				/****** BASE CLASS PARAMETERS *******/
				if ( strcmp(oneWord, "DuplicateMsgToLocalConsole") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					if ( strcmp(oneWord, "TRUE") == 0 )
					{
						useConsoleDump_ = true;
						setUseConsoleDump(true);
					}
					else
					{
						useConsoleDump_ = false;
						setUseConsoleDump(false);
					}	
				}

				/****** LOCAL ERROR DISPATCHER PARAMETERS *******/
				if ( strcmp(oneWord, "UseErrorDispatcher") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					if ( strcmp(oneWord, "TRUE") == 0 )
					{
						useDiag_ = true;
					}
					else useDiag_ = false;
				}

				if ( strcmp(oneWord, "ErrorDispatcherName") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					diagName_ = oneWord;
				}

				if ( strcmp(oneWord, "ErrorDispatcherInstance") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					diagInstance_ = oneWord;
				}
				
				if ( strcmp(oneWord, "ErrorDispatcherIP") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					diagIP_ = oneWord;
				}
				

				if ( strcmp(oneWord, "ErrorDispatcherPort") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					diagPort_ = oneWord;
				}


				if ( strcmp(oneWord, "ErrorDispatcherLID") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					diagLID_ = oneWord;
				}

				if ( strcmp(oneWord, "ErrorDispatcherLogLevel") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					edLogLevel_ = oneWord;
				}



				/****** GLOBAL ERROR DISPATCHER PARAMETERS *******/
				if ( strcmp(oneWord, "UseGlbErrorDispatcher") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					if ( strcmp(oneWord, "TRUE") == 0 )
					{
						useGlbDiag_ = true;
					}
					else useGlbDiag_ = false;
				}

				if ( strcmp(oneWord, "GlbErrorDispatcherName") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					diagGlbName_ = oneWord;
				}

				if ( strcmp(oneWord, "GlbErrorDispatcherInstance") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					diagGlbInstance_ = oneWord;
				}
				
				if ( strcmp(oneWord, "GlbErrorDispatcherIP") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					diagGlbIP_ = oneWord;
				}
				

				if ( strcmp(oneWord, "GlbErrorDispatcherPort") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					diagGlbPort_ = oneWord;
				}


				if ( strcmp(oneWord, "GlbErrorDispatcherLID") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					diagGlbLID_ = oneWord;
				}

				if ( strcmp(oneWord, "GlbErrorDispatcherLogLevel") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					edGlbLogLevel_ = oneWord;
				}
/*
    useSentinel_ = false;
    sentinelContextName_ = "UpperLayerErrorsPool";
    sentinelLogLevel_ = DIAGTRACE;
    sentinelUsable_ = false;
				sentinelUsable_ = false;
				sentinelUsableStatus_ = "unchecked";
				contextHasSentinel_ = false;
*/

				/****** SENTINEL PARAMETERS *******/
				if ( strcmp(oneWord, "UseSentinel") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					if ( strcmp(oneWord, "TRUE") == 0 )
					{
						useSentinel_ = true;
						sentinelContextName_ = "";
						sentinelUsable_ = true;
						sentinelUsableStatus_ = "checked";
						contextHasSentinel_ = true;
					}
					else
					{
						useSentinel_ = false;
						sentinelUsable_ = false;
						sentinelUsableStatus_ = "unchecked";
					}
				}

				if ( strcmp(oneWord, "SentinelAppenderLogLevel") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					sentinelLogLevel_ = oneWord;
				}
/*
				if ( strcmp(oneWord, "PushErrorsToContext") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					sentinelContextName_ = oneWord;
				}
*/

				/****** RECONFIGURATION PARAMETERS *******/
				if ( strcmp(oneWord, "UseErrorManager") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					if ( strcmp(oneWord, "TRUE") == 0 )
					{
						emUsable_ = true;
					}
					else emUsable_ = false;
				}

				if ( strcmp(oneWord, "ErrorManagerName") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					lvl1Name_ = oneWord;
				}


				if ( strcmp(oneWord, "ErrorManagerInstance") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					lvl1Instance_ = oneWord;
				}
				
				
				/****** CHAINSAW PARAMETERS *******/
				if ( strcmp(oneWord, "UseChainsaw") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					if ( strcmp(oneWord, "TRUE") == 0 )
					{
						csRoute_[0] = true;
					}
					else csRoute_[0] = false;
				}

				if ( strcmp(oneWord, "ChainsawServerHost") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					csIP_[0] = oneWord;
				}
				if ( strcmp(oneWord, "ChainsawServerPort") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					csPort_[0] = oneWord;
				}

				if ( strcmp(oneWord, "ChainsawReceiverName") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					csReceiver_[0] = oneWord;
				}

				if ( strcmp(oneWord, "ChainsawLogLevel") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					csLogLevel_[0] = oneWord;
				}

				if ( strcmp(oneWord, "ChainsawFilter") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					csFilter_[0] = oneWord;
				}



				/****** EXTRA CHAINSAW PARAMETERS *******/
				if ( strcmp(oneWord, "UseChainsawExtra") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					if ( strcmp(oneWord, "TRUE") == 0 )
					{
						csRoute_[nbrOfChainsawStreams_] = true;
					}
					else csRoute_[nbrOfChainsawStreams_] = false;
				}

				if ( strcmp(oneWord, "ChainsawServerHostExtra") == 0 )
				{

					fscanf(confFile,"%s",oneWord);
					csIP_[nbrOfChainsawStreams_] = oneWord;
				}
				if ( strcmp(oneWord, "ChainsawServerPortExtra") == 0 )
				{

					fscanf(confFile,"%s",oneWord);
					csPort_[nbrOfChainsawStreams_] = oneWord;
				}

				if ( strcmp(oneWord, "ChainsawReceiverNameExtra") == 0 )
				{

					fscanf(confFile,"%s",oneWord);
					csReceiver_[nbrOfChainsawStreams_] = oneWord;
				}

				if ( strcmp(oneWord, "ChainsawLogLevelExtra") == 0 )
				{

					fscanf(confFile,"%s",oneWord);
					csLogLevel_[nbrOfChainsawStreams_] = oneWord;
				}

				if ( strcmp(oneWord, "ChainsawFilterExtra") == 0 )
				{

					fscanf(confFile,"%s",oneWord);
					csFilter_[nbrOfChainsawStreams_] = oneWord;
				}

				if ( strcmp(oneWord, "EndOfChainsawExtra") == 0 )
				{
					nbrOfChainsawStreams_++;
					
				}








				/****** CONSOLE PARAMETERS *******/
				if ( strcmp(oneWord, "UseConsoleLogging") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					if ( strcmp(oneWord, "TRUE") == 0 )
					{
						ccRoute_ = true;
					}
					else ccRoute_ = false;
				}

				if ( strcmp(oneWord, "ConsoleLogLevel") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					ccLogLevel_ = oneWord;
				}


				/****** DB PARAMETERS *******/
				if ( strcmp(oneWord, "UseDbLogging") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					if ( strcmp(oneWord, "TRUE") == 0 )
					{
						dbRoute_ = true;
						dbRouteStatus_ = "checked";
						dbRouteValidated_ = true;
					}
					else
					{
						dbRoute_ = false;
						dbRouteStatus_ = "unchecked";
						dbRouteValidated_ = false;
					}
				}

				if ( strcmp(oneWord, "DbLogLevel") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					dbLogLevel_ = oneWord;
				}

				if ( strcmp(oneWord, "DbFilePathAndName") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					dbFileName_ = oneWord;
				}



				/****** FILE PARAMETERS *******/
				if ( strcmp(oneWord, "UseFileLogging") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					if ( strcmp(oneWord, "TRUE") == 0 )
					{
						useFileLogging_ = true;
						fileLoggingStatus_ = "checked";
						fileLoggingUsable_ = true;
					}
					else
					{
						useFileLogging_ = false;
						fileLoggingStatus_ = "unchecked";
						fileLoggingUsable_ = false;
					}
				}

				if ( strcmp(oneWord, "FileLogLevel") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					fileLogLevel_ = oneWord;
				}

				if ( strcmp(oneWord, "LogsFilePathAndName") == 0 )
				{
					fscanf(confFile,"%s",oneWord);
					logFileName_ = oneWord;
				}


				if ( strcmp(oneWord, "LogFilePath") == 0 )
				{

//DEBUGME
//std::cout << "Reading logFilePath_ from configuration file : " << diagSystemSettings_ << std::endl;
					fscanf(confFile,"%s",oneWord);
					logFilePath_ = expandFireItemWithEnvVariable((std::string)oneWord);
//DEBUGME
//std::cout << "logFilePath_ read as " << logFilePath_ << std::endl;

				}



			}
			fclose(confFile);
			//Override first CS stream parameters with env settings if any
if ( this->getIsInGlobalErrorDispatcher() == true )
{
if ( useCsEnvSettings_ == true )
{
	overrideCsSettings();
}
}
			}
			else
			{
	    	    log4cplus::Logger & e_logger = ((log4cplus::Logger &)(*callingAppLogger_));
				std::string lmsg = (std::string)"Parameters file " + diagSystemSettings_.c_str() + (std::string)" needed for tuned configuration not found ; DiagSystem usage turned to DEFAULT for this process.";
    	    	LOG4CPLUS_ERROR(e_logger, lmsg);
				if (process_kind == 0) //USER PROCESS
				{
					//useDiagSystem_ = "off";
					//return setAppDiagSystemToOff();
					useDiagSystem_ = "default";
					return setAppDiagSystemToDefault();
				}
				if (process_kind == 1) //ED or GLB_ED
				{
					//useDiagSystem_ = "off";
					//return setGlbEdDiagSystemToOff();
					useDiagSystem_ = "default";
					return setGlbEdDiagSystemToDefault();
				}
				if (process_kind == 2) // LR
				{
					//useDiagSystem_ = "off";
					//return setLrDiagSystemToOff();
					useDiagSystem_ = "default";
					return setLrDiagSystemToDefault();
				}
			}
//			activateDiagSystemForFirstTime_ = false;
			return false;

}


void DiagBagWizard::setSystemID(std::string systemID) {systemID_ = systemID;}
std::string DiagBagWizard::getSystemID() {return systemID_;}
void DiagBagWizard::setSubSystemID(std::string subSystemID) {subSystemID_ = subSystemID;}
std::string DiagBagWizard::getSubSystemID() {return subSystemID_;}




void DiagBagWizard::processOnlineDiagRequest(xoap::MessageReference msg, xoap::MessageReference reply)
{
	int lclCmd=0;

//std::cout << "In DIAGBAG SOAP CALLBACK" << std::endl;

	//Analyze received msg structure and command
	xoap::SOAPBody mb = msg->getSOAPPart().getEnvelope().getBody();
	std::vector<xoap::SOAPElement> paramsList = mb.getChildElements ();
	
	if(paramsList.size()==1)
	{
		xoap::SOAPName remoteCommand ("remoteCommand", "", "");
		lclCmd = atoi(paramsList[0].getAttributeValue(remoteCommand).c_str());
	}

	
	
	// Prepare reply body
	xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
	xoap::SOAPName responseName = envelope.createName( "processOnlineDiagRequestResponse", "xdaq", XDAQ_NS_URI);
	envelope.getBody().addBodyElement ( responseName );


// std::cout << "In DIAGBAG SOAP CALLBACK I see a lclCmd=" << lclCmd << std::endl;


	//According to command number, attach body parameters
	if (lclCmd == 1) //Reply with diag settings parameter strings
	{

	    /* Add requested attributes to SOAP part */
    	xoap::SOAPBody rb = reply->getSOAPPart().getEnvelope().getBody();
	    std::vector<xoap::SOAPElement> logElement = rb.getChildElements ();

		xoap::SOAPName ccLogLevel ("ccLogLevel", "", "");
		logElement[0].addAttribute(ccLogLevel,getGLogLevel());

		xoap::SOAPName lclLogLevel ("lclLogLevel", "", "");
		logElement[0].addAttribute(lclLogLevel,edLogLevel_);

		xoap::SOAPName glbLogLevel ("glbLogLevel", "", "");
		logElement[0].addAttribute(glbLogLevel,edGlbLogLevel_);

	}



	if (lclCmd == 2) //Force diag settings parameters
	{

	    /* Add requested attributes to SOAP part */
		xoap::SOAPName ccLogLevel ("ccLogLevel", "", "");
		if (paramsList[0].getAttributeValue(ccLogLevel) != DIAGUNCHANGED)
		{
			setGLogLevel( paramsList[0].getAttributeValue(ccLogLevel) );
			wa_->getApplicationLogger().setLogLevel(getProcessLogLevel("DEFAULT"));
		}
		

		xoap::SOAPName lclLogLevel ("lclLogLevel", "", "");
		if (paramsList[0].getAttributeValue(lclLogLevel) != DIAGUNCHANGED)
		{
			setEdLogLevel( paramsList[0].getAttributeValue(lclLogLevel) );
		}

		xoap::SOAPName glbLogLevel ("glbLogLevel", "", "");
		if (paramsList[0].getAttributeValue(glbLogLevel) != DIAGUNCHANGED)
		{
			setGlbEdLogLevel( paramsList[0].getAttributeValue(glbLogLevel) );
		}
// std::cout << "In DIAGBAG SOAP CALLBACK setting globalErrorDispatcherLogLevel to " << paramsList[0].getAttributeValue(glbLogLevel) << std::endl;

	}


}





void DiagBagWizard::postXoapMessageForReconfAction()
{
    /* Build SOAP for sending action */
    xoap::MessageReference err_msg = xoap::createMessage();
    xoap::SOAPPart err_soap = err_msg->getSOAPPart();
    xoap::SOAPEnvelope err_envelope = err_soap.getEnvelope();

    xoap::SOAPName command = err_envelope.createName(RECONF_COMMAND_ENTRY, "xdaq", XDAQ_NS_URI);
    xoap::SOAPBody err_body = err_envelope.getBody();
    err_body.addBodyElement(command);



    /* Add requested attributes to SOAP part */
    xoap::SOAPBody rb = err_msg->getSOAPPart().getEnvelope().getBody();
    std::vector<xoap::SOAPElement> logElement = rb.getChildElements ();

	xoap::SOAPName className ("className", "", "");
	logElement[0].addAttribute(className,wa_->getApplicationDescriptor()->getClassName());

	std::stringstream inst;
	inst << wa_->getApplicationDescriptor()->getInstance();
	xoap::SOAPName instance ("instance", "", "");
	logElement[0].addAttribute(instance,inst.str());

	xoap::SOAPName errorCode ("errorCode", "", "");
	logElement[0].addAttribute(errorCode,longClog.getErrorCode());
	
	xoap::SOAPName extraBuffer ("extraBuffer", "", "");
	logElement[0].addAttribute(extraBuffer,longClog.getExtraBuffer());


	/*
	std::cout << "Posting to Local ED with XOAP :";
	err_msg->writeTo(std::cout);
	std::cout << std::endl;
	*/
	try
	{	
		#ifdef XDAQ373
			xdaq::ApplicationDescriptor * err_d = wa_->getApplicationContext()->getDefaultZone()->getApplicationDescriptor(lvl1Name_, atoi(lvl1Instance_.c_str()));
		#else
			xdaq::ApplicationDescriptor * err_d = wa_->getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(lvl1Name_, atoi(lvl1Instance_.c_str()));
		#endif
		wa_->getApplicationContext()->postSOAP(err_msg, *selfAppDescriptor_, *err_d);
	}
	catch (xdaq::exception::Exception& e) {}

}




void DiagBagWizard::pushClogToFilteredStreams(CLog l, int streamNum)
{
//	for (int csStreamNum = 0; csStreamNum < nbrOfChainsawStreams_ ; csStreamNum++)
//	{
		chainSawManager_->pushClogToFilteredStream(l, csReceiver_[streamNum], csFilter_[streamNum]);
//	}
}








std::string DiagBagWizard::expandFireItemWithEnvVariable(std::string rawFireItem)
{

//#define DEBUG_FIREITEM_EXPAND

std::string openingTag = "$${";
std::string closingTag = "}$$";

std::string resu;
std::string::size_type seekIndex=0;

int openingTags=0;
int closingTags=0;

std::string interpretedFireItem = "";

	do
	{
		#ifdef DEBUG_FIREITEM_EXPAND
			std::cout << std::endl << std::endl;
		#endif
		resu = "NA";
		std::string::size_type locB = rawFireItem.find( openingTag, seekIndex );
		if( locB != std::string::npos )
		{
			#ifdef DEBUG_FIREITEM_EXPAND
				std::cout << openingTag << " found at loc " << locB << std::endl;
			#endif
			openingTags++;
			std::string::size_type locE = rawFireItem.find( closingTag, seekIndex );
			if( locE != std::string::npos )
			{
				#ifdef DEBUG_FIREITEM_EXPAND
					std::cout << closingTag << " found at loc " << locE << std::endl;
				#endif
				closingTags++;
				std::string stringBeforeTag = rawFireItem.substr( seekIndex, (locB-seekIndex) );
				resu = rawFireItem.substr( (locB + openingTag.length()), (locE - openingTag.length() - locB) );
				seekIndex = locE + closingTag.length();
				#ifdef DEBUG_FIREITEM_EXPAND
					std::cout << "Found Pre-Substring : " << stringBeforeTag << std::endl;
					std::cout << "Found Substring : " << resu << std::endl;
				#endif
				interpretedFireItem = interpretedFireItem + stringBeforeTag;
				char * readEnv = getenv (resu.c_str());
				if (readEnv != NULL)
				{
					#ifdef DEBUG_FIREITEM_EXPAND
						std::cout << "Got ENV value : " << (std::string)readEnv << std::endl;
					#endif
					interpretedFireItem = interpretedFireItem + std::string(readEnv);
				}

			}
			else
			{
				#ifdef DEBUG_FIREITEM_EXPAND
					std::cout << closingTag << " not found. ";
				#endif
				std::cout << "Error in fireitem <DiagSystemSettings> ; missing tag \"}$$\"" << std::endl;
			}
		}
		else
		{
			#ifdef DEBUG_FIREITEM_EXPAND
				std::cout << openingTag << " not found. ";
			#endif
			if (openingTags == closingTags)
			{
				std::string stringAfterTag = rawFireItem.substr( seekIndex );
				#ifdef DEBUG_FIREITEM_EXPAND
					std::cout << "Terminal Substring : " << stringAfterTag << std::endl;
				#endif
				interpretedFireItem = interpretedFireItem + stringAfterTag;
			}
		}
		#ifdef DEBUG_FIREITEM_EXPAND
			std::cout << std::endl;
		#endif
	} while (resu != "NA");
	#ifdef DEBUG_FIREITEM_EXPAND
		std::cout << "Opening Tags found : " << openingTags << std::endl;
		std::cout << "Closing Tags found : " << closingTags << std::endl;
		std::cout << "Expanded string identified as  : " << interpretedFireItem << std::endl;
		std::cout << std::endl << std::endl;
	#endif
return interpretedFireItem;
}






void DiagBagWizard::flushFirstMessages () 

{


//std::cout << "Entering flushFirstMessages" << std::endl;
//std::cout << "Flushing longLogs" << std::endl;
/*
if (fiAreInitialized_ == true)
{
	std::cout << "TRUE" << std::endl;
	std::cout << "Process log level is now : " << edGlbLogLevel_ << std::endl;
}
else std::cout << "FALSE" << std::endl;
*/


		if (longClogsList_.size() > 0)
		{
			while (longClogsList_.empty() == false)
			{
				CLog lclClog = *(longClogsList_.end() - 1);
//DEBUGME1
//std::cout << "flushed longLogs log level is : " << lclClog.getLevel() << std::endl;
				repostError(	lclClog.getTypeOfMessage(),
						lclClog.getLevel(),
						lclClog.getMessage(),
						lclClog.getTimeStamp(),
						lclClog.getTimestampAsString(),
						lclClog.getLogger(),
						lclClog.getErrorCode(),
						lclClog.getFaultState(),
						lclClog.getSystemID(),
						lclClog.getSubSystemID(),
						lclClog.getExtraBuffer() );

				longClogsList_.pop_back();
			}
		}


//std::cout << "Flushing shortLogs" << std::endl;

		if (shortClogsList_.size() > 0)
		{
			while (shortClogsList_.empty() == false)
			{
				CLog lclClog = *(shortClogsList_.end() - 1);
//DEBUGME1
//std::cout << "flushed shortLogs log level is : " << lclClog.getLevel() << std::endl;

				repostError(	lclClog.getTypeOfMessage(),
						lclClog.getLevel(),
						lclClog.getMessage(),
						lclClog.getTimeStamp(),
						lclClog.getTimestampAsString(),
						lclClog.getLogger(),
						lclClog.getErrorCode(),
						lclClog.getFaultState(),
						lclClog.getSystemID(),
						lclClog.getSubSystemID(),
						lclClog.getExtraBuffer() );

				shortClogsList_.pop_back();
			}
		}


/*
//std::cout << "Flushing repostLogs" << std::endl;

		if (shortClogsRepostList_.size() > 0)
		{
			while (shortClogsRepostList_.empty() == false)
			{
				CLog lclClog = *(shortClogsRepostList_.end() - 1);
//DEBUGME1
//std::cout << "flushed repostLogs log level is : " << lclClog.getLevel() << std::endl;

				repostError(	lclClog.getTypeOfMessage(),
						lclClog.getLevel(),
						lclClog.getMessage(),
						lclClog.getTimeStamp(),
						lclClog.getTimestampAsString(),
						lclClog.getLogger(),
						lclClog.getErrorCode(),
						lclClog.getFaultState(),
						lclClog.getSystemID(),
						lclClog.getSubSystemID(),
						lclClog.getExtraBuffer() );

				shortClogsRepostList_.pop_back();
			}
		}
*/

}



void DiagBagWizard::overrideCsSettings (void) 
{
	if ( (useDiagSystem_ == "default") || (useDiagSystem_ == "tuned") || (useDiagSystem_ == "oldway") )
	{
//	std::cout << "BEFORE OVERRIDING:csReceiver_[0]=" << csReceiver_[0] << std::endl;
//	std::cout << "OVERRIDING CS FIREITEMS VALUES" << std::endl;
	csRoute_[0] = true;
	csRouteValidated_[0] = true;

	csReceiverStatus_[0] = "checked";
	csIPStatus_[0] = "checked";
	csPort_[0] = "checked";
	csRouteStatus_[0] = "checked";


	csIP_[0] = csConfMachine_;
	csPort_[0] = csConfPort_;
	csReceiver_[0] = csConfReceiverName_;
	csLogLevel_[0] = csConfLogLevel_;
	csFilter_[0]= "NOFILTER";
//	std::cout << "AFTER OVERRIDING:csReceiver_[0]=" << csReceiver_[0] << std::endl;
	}


}














