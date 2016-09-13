/*
   FileName : 		DiagBag.h

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

#ifndef _DiagBagWizard_h_
#define _DiagBagWizard_h_

#include <sstream>
#include <sys/time.h>
#include <netdb.h>
#include <sys/time.h>


#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/HTTPResponseHeader.h"


#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"
#include "xdaq/NamespaceURI.h"
#include "xdaq/WebApplication.h"
#include "xdaq/exception/ApplicationNotFound.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/exception/Exception.h"


#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPElement.h"
#include "xoap/Method.h"
#include "xoap/domutils.h"

#include "xdata/String.h"
#include "xdata/UnsignedLong.h"
#include "xdata/Integer.h"
#include "xdata/Boolean.h"


#include "xcept/tools.h"
#include "toolbox/utils.h"

/*
#include "sentinel/Interface.h"
#include "sentinel/Listener.h"
*/

#include <log4cplus/config.hxx>
#include <log4cplus/appender.h>
#include <log4cplus/helpers/socket.h>
#include "log4cplus/logger.h"
#include "log4cplus/consoleappender.h"
#include "log4cplus/helpers/appenderattachableimpl.h"
#include "log4cplus/helpers/loglog.h"
#include "log4cplus/helpers/pointer.h"
#include "log4cplus/spi/loggingevent.h"
#include <log4cplus/layout.h>
#include <log4cplus/socketappender.h>

//#include <XRelay.h>
#include <xrelay/Application.h>

#include "toolbox/reportErrorReturnType.h"
#include "DiagCompileOptions.h"
//#include <soapdiagappender/SOAPDiagAppender.h>
#include "diagbag/helpers/clog.h"
#include "diagbag/DiagBag.h"
#include <xmldiagappender/XMLDiagAppender.h>
#include <diagbag/helpers/DiagBagWizardMacros.h>

#include "toolbox/BSem.h"
#include "xcept/tools.h"
#include "toolbox/utils.h"

#include <diagbag/ChainSawManager.hh>


#define NB_MAX_MSG_IN_GED_BUFFER 5000
/*
    #include "toolbox/task/Timer.h"
    #include "toolbox/task/TimerFactory.h"
    #include "toolbox/TimeInterval.h"
*/

#define NB_MAX_MSG_IN_LONGCLOGLIST_BUFFER 5000
#define NB_MAX_MSG_IN_SHORTCLOGLIST_BUFFER 5000




//class DiagBagWizard : public DiagBag, public toolbox::task::TimerListener
class DiagBagWizard : public DiagBag
{

/*
void timeExpired (toolbox::task::TimerEvent& e);
toolbox::task::Timer * timer_;
std::string timerName_;
*/

    protected:
//SENTMOD
//        sentinel::Interface * sentinel_;
		xdaq::ApplicationDescriptor * selfAppDescriptor_;
    public:
        /* Pointer should be unused now ; to check in .cc file. */
//        SOAPDiagAppender * soapPtr_;

        DiagBagWizard(std::string l1name,
                    log4cplus::Logger & logger,
                    std::string classname,
                    unsigned long instance,
                    unsigned long lid,
                    xdaq::WebApplication * wa,
					std::string systemID,
					std::string subSystemID,
					std::stringstream * pixelsConsolePointer) throw (xdaq::exception::Exception);



        DiagBagWizard(std::string l1name,
                    log4cplus::Logger & logger,
                    std::string classname,
                    unsigned long instance,
                    unsigned long lid,
                    xdaq::WebApplication * wa,
					std::string systemID,
					std::string subSystemID) throw (xdaq::exception::Exception);




        /* Ctors */
        DiagBagWizard(std::string l1name,
                    log4cplus::Logger & logger,
                    std::string classname,
                    unsigned long instance,
                    unsigned long lid,
                    xdaq::WebApplication * wa) throw (xdaq::exception::Exception);


        DiagBagWizard(std::string l1name) throw (xdaq::exception::Exception);

        /* Dtor */
        virtual ~DiagBagWizard();

        /* Methods */
        pthread_attr_t commit_thread_attr_;
        pthread_t commit_thread_id_;
/*
        void postXoapMessage(std::string time,
                            std::string plevel,
                            std::string ppthread,
                            std::string plogger,
                            std::string pmessage,
                            std::string pfile,
                            std::string pline,
                            std::string pndc);
*/

        void postXoapMessageForLclEd(std::string);
        void postXoapMessageForGlbEd(std::string);

/*
        std::string forgeErrorString(std::string msg,
                                    std::string text,
                                    int errorcode,
                                    std::string faultstate,
                                    std::string systemid,
                                    std::string subsystemid,
                                    std::string classname,
                                    unsigned long instance,
                                    unsigned long lid,
                                    std::string lvl1name,
                                    unsigned long lvl1instance,
                                    bool emUsable,
                                    std::string dbloglevel,
                                    std::string csloglevel,
                                    std::string ccloglevel,
                                    bool forceddblogs,
                                    bool forcedcslogs,
                                    bool forcedcclogs,
                                    std::string gloglevel,
                                    bool relayedEmUsable,
                                    bool useLocalEd);

        std::string forgeErrorString(std::string msg,
                                    std::string text,
                                    int errorcode,
                                    std::string faultstate,
                                    std::string systemid,
                                    std::string subsystemid,
                                    std::string classname,
                                    unsigned long instance,
                                    unsigned long lid,
                                    bool useLocalEd);

        std::string forgeErrorString(std::string msg,
                                    std::string classname,
                                    unsigned long instance,
                                    unsigned long lid,
                                    bool useLocalEd);
*/
        int configureCallback(xgi::Input * in,
                            xgi::Output * out,
                            std::string classname,
                            std::string URL,
                            std::string URN);

        int configureCallback(xgi::Input * in,
                            xgi::Output * out,
                            std::string classname,
                            std::string URL,
                            std::string URN,
                            std::string resumeUrl);

        int getFormValues(xgi::Input * in,
                        xgi::Output * out);

        bool checkUseDiag();
		bool checkUseDiagSystem();

        bool setUseDiagOnName(bool lcl_use_diag);
        bool setUseDiagOnLid(unsigned long proc_lid);
        bool setUseDiagOnUrl(std::string l_url);


        bool checkUseGlbDiag();
        bool setUseGlbDiagOnName(bool lcl_use_diag);
        bool setUseGlbDiagOnLid(unsigned long proc_lid);
        bool setUseGlbDiagOnUrl(std::string l_url);


        bool checkUseLvl1();
        bool setUseLvl1OnName(bool lcl_use_lvl1);
        bool setUseLvl1OnLid(unsigned long proc_lid);
        bool setUseLvl1OnUrl(std::string l_url);

        std::string getDiagName();
        std::string getDiagInstance();
        std::string getLvl1Name();
        std::string getLvl1Instance();

        std::string getDiagIP();
        std::string getDiagPort();
        std::string getDiagLid();
        std::string getGLogLevel();
        void setGLogLevel(std::string logLevel);
        long getProcessLogLevel(std::string source);

        std::string getOldDiagIP();
        void setOldDiagIP(std::string s);
        std::string getOldDiagPort();
        void setOldDiagPort(std::string s);
        std::string getOldDiagName();
        void setOldDiagName(std::string s);
        std::string getOldDiagInstance();
        void setOldDiagInstance(std::string s);
        std::string getOldDiagLID();
        void setOldDiagLID(std::string s);
        bool getSocketSoapExists();
        void setSocketSoapExists(bool b);
        std::string getDiagLID();
        void setDiagLID(std::string s);
/*
        void zeroLvlLog(std::string msg, std::string lvl, log4cplus::Logger & logger);
        void minLvlLog(std::string msg, std::string lvl, log4cplus::Logger & logger, std::string classname, unsigned long instance, unsigned long lid, std::string initialLogger, std::string initialTimeStamp, std::string relayedEM);
        void regularLvlLog(std::string msg, std::string lvl, log4cplus::Logger & logger, std::string classname, unsigned long instance, unsigned long lid, std::string text, int errorcode, std::string faultstate, std::string systemid, std::string subsystemid, std::string initialLogger, std::string initialTimeStamp, std::string relayedEM);
*/


		void setRunFromRcms(int runFromRcms);



    //Wrap to next call



void repostError(std::string msgType,
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
								std::string extraBuffer);
void logsReRouter();

/*
//MODHERE
#ifdef WILL_COMPILE_FOR_PIXELS


	#define REPORT_ERROR_RETURN_TYPE	std::string
	#define REPORT_ERROR_RETURN_COMMAND	return(msg);


//	#define REPORT_ERROR_RETURN_TYPE	void
//	#define REPORT_ERROR_RETURN_COMMAND	//return(msg);


#else
	#define REPORT_ERROR_RETURN_TYPE	void
	#define REPORT_ERROR_RETURN_COMMAND	//return(msg);
#endif
*/

        REPORT_ERROR_RETURN_TYPE reportError(std::string msg, std::string lvl, std::string extraBuffer=DIAG_DEFAULT_EBUFF);

//        std::string reportError(std::string msg, std::string lvl, std::string extraBuffer=DIAG_DEFAULT_EBUFF);



/*		
        void reportErrorvoid(std::string msg, std::string lvl, std::string extraBuffer=DIAG_DEFAULT_EBUFF);
        int reportErrorint(std::string msg, std::string lvl, std::string extraBuffer=DIAG_DEFAULT_EBUFF);
*/
/*
        std::string * reportErrorstrptr(std::string msg, std::string lvl, std::string extraBuffer=DIAG_DEFAULT_EBUFF);
        const char * reportErrorchrptr(std::string msg, std::string lvl, std::string extraBuffer=DIAG_DEFAULT_EBUFF);
*/

		
        REPORT_ERROR_RETURN_TYPE reportError(bool addSubClassPath, std::string subClassPath, std::string msg, std::string lvl, std::string extraBuffer=DIAG_DEFAULT_EBUFF);
        REPORT_ERROR_RETURN_TYPE reportError(bool addSubClassPath, std::string subClassPath, bool addFileName, std::string fileName, std::string msg, std::string lvl, std::string extraBuffer=DIAG_DEFAULT_EBUFF);
        REPORT_ERROR_RETURN_TYPE reportError(bool addSubClassPath, std::string subClassPath, bool addFileName, std::string fileName, bool addLineNumber, int lineNumber, std::string msg, std::string lvl, std::string extraBuffer=DIAG_DEFAULT_EBUFF);

        REPORT_ERROR_RETURN_TYPE reportError(std::string msg, std::string lvl, std::string text, int errorcode, std::string faultstate, std::string extraBuffer=DIAG_DEFAULT_EBUFF);
        REPORT_ERROR_RETURN_TYPE reportError(bool addSubClassPath, std::string subClassPath, std::string msg, std::string lvl, std::string text, int errorcode, std::string faultstate, std::string extraBuffer=DIAG_DEFAULT_EBUFF);
        REPORT_ERROR_RETURN_TYPE reportError(bool addSubClassPath, std::string subClassPath, bool addFileName, std::string fileName, std::string msg, std::string lvl, std::string text, int errorcode, std::string faultstate, std::string extraBuffer=DIAG_DEFAULT_EBUFF);
        REPORT_ERROR_RETURN_TYPE reportError(bool addSubClassPath, std::string subClassPath, bool addFileName, std::string fileName, bool addLineNumber, int lineNumber, std::string msg, std::string lvl, std::string text, int errorcode, std::string faultstate, std::string extraBuffer=DIAG_DEFAULT_EBUFF);

        REPORT_ERROR_RETURN_TYPE reportError(std::string msg, std::string lvl, std::string text, int errorcode, std::string faultstate, std::string systemid, std::string subsystemid, std::string extraBuffer=DIAG_DEFAULT_EBUFF);
        REPORT_ERROR_RETURN_TYPE reportError(bool addSubClassPath, std::string subClassPath, std::string msg, std::string lvl, std::string text, int errorcode, std::string faultstate, std::string systemid, std::string subsystemid, std::string extraBuffer=DIAG_DEFAULT_EBUFF);
        REPORT_ERROR_RETURN_TYPE reportError(bool addSubClassPath, std::string subClassPath, bool addFileName, std::string fileName, std::string msg, std::string lvl, std::string text, int errorcode, std::string faultstate, std::string systemid, std::string subsystemid, std::string extraBuffer=DIAG_DEFAULT_EBUFF);
        REPORT_ERROR_RETURN_TYPE reportError(bool addSubClassPath, std::string subClassPath, bool addFileName, std::string fileName, bool addLineNumber, int lineNumber, std::string msg, std::string lvl, std::string text, int errorcode, std::string faultstate, std::string systemid, std::string subsystemid, std::string extraBuffer=DIAG_DEFAULT_EBUFF);

        void setUseDiag(bool diagStatus);
        void setGlbUseDiag(bool diagStatus);
        void setUseEd(bool diagStatus);

        bool getUseDiag(void);
        bool getUseEd(void);

        void setUseSentinel(bool useSentinel);
        bool getUseSentinel(void);
        void setSentinelContextName(std::string contextName);
        std::string getSentinelContextName(void);
        void setSentinelUseRelayedEd(bool sentinelUseRelayedEd);
        bool getSentinelUseRelayedEd(void);
        void setSentinelLogLevel(std::string logLevel);
        std::string getSentinelLogLevel(void);
        void setEdLogLevel(std::string logLevel);
        void setGlbEdLogLevel(std::string logLevel);
        std::string getEdLogLevel(void);
        void setSentinelUseRelayedLvl1(bool sentinelUseRelayedLvl1);
        bool getSentinelUseRelayedLvl1(void);
	
        bool checkRelayedEdStatus(void);
        bool checkRelayedLvl1Status(void);

        bool checkUseSentinel(void);
        bool checkUseDefaultAppender(void);
	
        void setDefaultAppenderUsable(bool isUsable);
	
        bool getIsInSentinelListener(void);
        void setIsInSentinelListener(bool);
	
//SENTMOD
//        sentinel::Interface * getSentinelInterface(void);
	
        void display_internals(void);


		void setUseDiagSystem(std::string useDiagSystem);
		void setLogLevel(std::string logLevel);

        void setUseDefaultAppender(bool useAppender);
        void setDefaultLogLevel(std::string logLevel);

        void setDiagName(std::string name);
        void setGlbDiagName(std::string name);
        void setDiagInstance(std::string instance);
        void setGlbDiagInstance(std::string instance);
        void setLvl1Name(std::string name);

        void setLvl1Instance(std::string instance);
        void setDiagIP(std::string ip);
        void setDiagPort(std::string port);
        void setDiagLid(std::string lid);

        void setGlbDiagIP(std::string ip);
        void setGlbDiagPort(std::string port);
        void setGlbDiagLid(std::string lid);

        std::string getLvl1IP();
        void setLvl1IP(std::string ip);
        std::string getLvl1Port();
        void setLvl1Port(std::string port);
        std::string getLvl1Lid();
        void setLvl1Lid(std::string lid);
        bool getcontextHasSentinel(void);
        void setcontextHasSentinel(bool hasSentinel);

        void setUseConsoleDump(bool useConsoleDump);
        void setUseLvl1(bool useLvl1);
        void setForceDbLogging(bool forceDbLogging);
        void setForcedDbLoggingLevel(std::string forcedDbLoggingLevel);
        void setForceCsLogging(bool forceCsLogging);
        void setForcedCsLoggingLevel(std::string forcedCsLoggingLevel);
        void setForceCcLogging(bool forceCcLogging);
        void setForcedCcLoggingLevel(std::string forcedCcLoggingLevel);
        void setIsInErrorDispatcher(bool isInErrorDispatcher);
        bool getIsInErrorDispatcher(void);
        void setIsInGlobalErrorDispatcher(bool isInGlobalErrorDispatcher);
        bool getIsInGlobalErrorDispatcher(void);
        void checkEdOptions(void);
        void checkEmOptions(void);
        bool isFileLoggingUsable(void);
        void setIsInUserProcess(bool isInUserProcess);
        bool getIsInUserProcess(void);
        void setIsInErrorManager(bool isInErrorManager);
        bool getIsInErrorManager(void);
	
        void setIsInLogsReader(bool isInLogsReader);
        bool getIsInLogsReader(void);

        bool checkUseSentinelGrabber(void);


        void setRouteToCs(bool routeToCs, int csStreamIndex);
        void setCsIP(std::string csIP, int csStreamIndex);
        void setCsPort(std::string csPort, int csStreamIndex);
        void setCsReceiver(std::string csReceiver, int csStreamIndex);
        void setCsLogLevel(std::string csLogLevel, int csStreamIndex);
        void setRouteToCc(bool routeToCc);
        void setCcLogLevel(std::string ccLogLevel);
        void setRouteToDb(bool routeToDb);
        void setDbLogLevel(std::string dbLogLevel);
        void setRouteToFile(bool routeToFile);
        void setFileLogLevel(std::string fileLogLevel);
        void setFilePathAndName(std::string fileName);
        void setDbFilePathAndName(std::string dbFileName);


        void setLogFilePath(std::string logFilePath);


        void setCloseReconfLoop(bool closeLoop);
        void setReconfProcessName(std::string pName);
        void setReconfProcessInstance(std::string instance);
        void setReconfProcessLID(std::string lid);
        void setReconfProcessIP(std::string ip);
        void setReconfProcessPort(std::string port);

        void setUseGrabber(bool useGrabber);
        void setGrabberContextName(std::string name);

        bool checkUseEd(void);
        bool checkUseGlbEd(void);
        bool checkUseEm(void);

        void applyUserAppFireItems(void);
        void applyEmAppFireItems(void);
        void applySentinelGrabberAppFireItems(void);
        void applyEdAppFireItems(void);
        void applyGlbEdAppFireItems(void);
        void applyLogsReaderAppFireItems(void);

        void getDiagSystemHtmlPage(xgi::Input * in, xgi::Output * out, std::string URN);
//        SOAPDiagAppender * getSoapPointer(void);

		void setDiagSystemSettings(std::string diagSystemSettings);


        void joinSentinel(void);
        void leaveSentinel(void);
/*
        void joinSentinelOnInit(xdaq::Application * wa);
        void leaveSentinelOnInit(xdaq::Application * wa);
*/
        /* Data dedicated to fireEvent() */







		/* All Client Apps -- Shortcut for GlobalErrorDispatcher Usage */
//        xdata::Boolean feUseDiagSystem_;
        xdata::String feUseDiagSystem_;
        xdata::String feDiagSystemSettings_;


		std::stringstream * pixelsConsolePointer_;



        xdata::String feLogLevel_;

        /* CLIENT APPLICATION BLOCK */
        xdata::Boolean feUseDefaultAppender_;
        xdata::String feDefaultAppenderLogLevel_;
        xdata::Boolean feDuplicateMsgToLocalConsole_;

        xdata::Boolean feUseSentinel_;
        xdata::String feSentinelAppenderLogLevel_;
        xdata::String fePushErrorsToContext_;
        xdata::Boolean feUseSentinelRelayedEm_;

        xdata::Boolean feUseErrorDispatcher_;
        xdata::String feErrorDispatcherLogLevel_;
        xdata::String feErrorDispatcherName_;
        xdata::String feErrorDispatcherInstance_;
        xdata::String feErrorDispatcherLID_;
        xdata::String feErrorDispatcherIP_;
        xdata::String feErrorDispatcherPort_;

        xdata::Boolean feUseErrorManager_;
        xdata::String feErrorManagerName_;
        xdata::String feErrorManagerInstance_;
        xdata::String feErrorManagerLID_;
        xdata::String feErrorManagerIP_;
        xdata::String feErrorManagerPort_;

        xdata::Boolean feForceDbLogging_;
        xdata::String feForcedDbLoggingLevel_;
        xdata::Boolean feForceChainsawLogging_;
        xdata::String feForcedChainsawLoggingLevel_;
        xdata::Boolean feForceConsoleLogging_;
        xdata::String feForcedConsoleLoggingLevel_;

        /* ERROR DISPATCHER BLOCK */
        xdata::Boolean feUseChainsaw_;
        xdata::String feChainsawServerHost_;
        xdata::String feChainsawServerPort_;
        xdata::String feChainsawReceiverName_;
        xdata::String feChainsawLogLevel_;

        xdata::Boolean feUseConsoleLogging_;
        xdata::String feConsoleLogLevel_;

        xdata::Boolean feUseDbLogging_;
        xdata::String feDbLogLevel_;

        xdata::Boolean feUseFileLogging_;
        xdata::String feFileLogLevel_;
        xdata::String feFilePathAndName_;
        xdata::String feDbFilePathAndName_;

        /* LEVEL ONE BLOCK */
        xdata::Boolean feCloseReconfLoop_;
        xdata::String feReconfProcessName_;
        xdata::String feReconfProcessInstance_;
        xdata::String feReconfProcessLID_;
        xdata::String feReconfProcessIP_;
        xdata::String feReconfProcessPort_;

        /* SENTINEL GRABBER BLOCK */
        xdata::Boolean feListenSentinel_;
        xdata::String feListenContext_;


        xdata::Boolean feUseGlbErrorDispatcher_;
        xdata::String feGlbErrorDispatcherLogLevel_;
        xdata::String feGlbErrorDispatcherName_;
        xdata::String feGlbErrorDispatcherInstance_;
        xdata::String feGlbErrorDispatcherLID_;
        xdata::String feGlbErrorDispatcherIP_;
        xdata::String feGlbErrorDispatcherPort_;
        xdata::String feLogFilePath_;

		//Add fireitem to get RunNumber
        xdata::Integer feRunFromRcms_;




	public: 


		std::string logFilePath_;

/*
        bool csRoute_;
        bool csRouteValidated_;
        std::string csRouteStatus_;

        std::string csReceiver_;
        std::string csReceiverStatus_;
        std::string csIPStatus_;
        std::string csIP_;
        std::string csPortStatus_;
        std::string csPort_;

*/

        /* Data */
        bool useDiag_;
        std::string diagName_;
        std::string diagInstance_;
        std::string diagIP_;
        std::string diagPort_;
        std::string diagLID_;


        bool useGlbDiag_;
        std::string diagGlbName_;
        std::string diagGlbInstance_;
        std::string diagGlbIP_;
        std::string diagGlbPort_;
        std::string diagGlbLID_;



        bool useLvl1_;
        std::string lvl1Name_;
        std::string lvl1Instance_;
        std::string lvl1IP_;
        std::string lvl1Port_;
        std::string lvl1LID_;

        std::string useDiagStatus_;
        std::string diagNameStatus_;
        std::string diagInstanceStatus_;
        std::string diagIPStatus_;
        std::string diagPortStatus_;
        std::string diagGlbLIDStatus_;


        std::string useGlbDiagStatus_;
        std::string diagGlbNameStatus_;
        std::string diagGlbInstanceStatus_;
        std::string diagGlbIPStatus_;
        std::string diagGlbPortStatus_;
        std::string diagLIDStatus_;

        std::string useLvl1Status_;
        std::string lvl1NameStatus_;
        std::string lvl1InstanceStatus_;
        std::string lvl1IPStatus_;
        std::string lvl1PortStatus_;
        std::string lvl1LIDStatus_;

        bool edUsable_;
        bool edGlbUsable_;


        std::string logLevel_;
		//bool useDiagSystem_;
		std::string useDiagSystem_;
		std::string diagSystemSettings_;

        bool emUsable_;
        bool forcedDbLogs_;
        bool forcedCsLogs_;
        bool forcedCcLogs_;
        std::string dbLogLevel_;
        std::string csLogLevel_[DIAGMAX_CS_STREAMS];
        std::string ccLogLevel_;	
        std::string gLogLevel_;

        std::string oldDiagIP_;
        std::string oldDiagPort_;
        std::string oldDiagName_;
        std::string oldDiagInstance_;
        std::string oldDiagLID_;
        bool socketSoapExists_;

        log4cplus::Logger * callingAppLogger_;
        std::string callingAppClassName_;
        unsigned long callingAppInstance_;
        unsigned long callingAppLID_;

        xdaq::WebApplication * wa_;
        bool useSentinel_;
        std::string sentinelContextName_;
        bool sentinelUseRelayedEd_;
        std::string sentinelLogLevel_;
        std::string edLogLevel_;
        std::string edGlbLogLevel_;
        
        
        bool sentinelUseRelayedLvl1_;
	
        std::string relayedEdStatus_;
        std::string relayedLvl1Status_;

        bool relayedEdUsable_;
        bool relayedEmUsable_;
        bool sentinelUsable_;

        bool useDefaultAppender_;

        std::string diagSystemStatus_;
	
        std::string defaultAppenderStatus_;
        bool defaultAppenderUsable_;

        bool useConsoleDump_;
        std::string consoleDumpStatus_;
        bool consoleDumpUsable_;
	
        bool contextHasSentinel_;
        std::string sentinelUsableStatus_;
	
        bool isInSentinelListener_;
        bool isInUserProcess_;
        bool isInErrorDispatcher_;
        bool isInGlobalErrorDispatcher_;
        bool isInErrorManager_;
        bool isInLogsReader_;

        bool csRoute_[DIAGMAX_CS_STREAMS];
        bool csRouteValidated_[DIAGMAX_CS_STREAMS];
        std::string csRouteStatus_[DIAGMAX_CS_STREAMS];

        std::string csReceiver_[DIAGMAX_CS_STREAMS];
        std::string csReceiverStatus_[DIAGMAX_CS_STREAMS];
        std::string csIPStatus_[DIAGMAX_CS_STREAMS];
        std::string csIP_[DIAGMAX_CS_STREAMS];
        std::string csPortStatus_[DIAGMAX_CS_STREAMS];
        std::string csPort_[DIAGMAX_CS_STREAMS];

        std::string csFilter_[DIAGMAX_CS_STREAMS];

        bool ccRoute_;
        std::string ccRouteStatus_;
        bool ccRouteValidated_;

        bool dbRoute_;
        std::string dbRouteStatus_;
        bool dbRouteValidated_;
        bool useFileLogging_;
        bool fileLoggingUsable_;
        std::string fileLogLevel_;
        std::string logFileName_;
        std::string fileLoggingStatus_;



        std::string oldCsIP_[DIAGMAX_CS_STREAMS];
        std::string oldCsPort_[DIAGMAX_CS_STREAMS];
        std::string oldCsReceiver_[DIAGMAX_CS_STREAMS];
        bool oldFirstTime_[DIAGMAX_CS_STREAMS];

        bool socketXMLExists_[DIAGMAX_CS_STREAMS];
        bool useReconf_;

        std::string reconfIP_;
        std::string reconfPort_;
        std::string reconfLID_;
        std::string oldReconfIP_;
        std::string oldReconfPort_;
        std::string oldReconfName_;
        std::string oldReconfInstance_;
        std::string oldReconfLID_;
        std::string reconfNameStatus_;
        std::string reconfInstanceStatus_;
        std::string reconfIPStatus_;
        std::string reconfPortStatus_;
        std::string reconfLIDStatus_;

    public:
        XMLDiagAppender * chainsawAppender_;
        bool reconfOn_;
        bool reconfUsable_;
        std::string reconfName_;
        std::string reconfInstance_;

        std::string grabberContextName_;
        std::string oldGrabberContextName_;
        bool grabberIsAttached_;
        std::string grabberUsableStatus_;
        bool useGrabber_;
        bool grabberUsable_;

        bool isEdRoutingToSentinel_;
        bool areSentinelScanned_;
        bool isSentinelContextPushed_;


		int runFromRcms_;



        bool talkToLocalEd_;

bool isWaitingForLclLogAck_;
bool isWaitingForGlbLogAck_;



#ifdef USE_XRELAY
//bool isWaitingForLclLogAck_;
int missedLclEmissions_;
std::string lclEdURL_;
std::string lclEdURN_;

//bool isWaitingForGlbLogAck_;
int missedGlbEmissions_;
std::string glbEdURL_;
std::string glbEdURN_;

#ifndef NOSLOWDOWN
int lclMessagesPooled_;
int glbMessagesPooled_;
#endif
#endif

CLog shortClog;
CLog longClog;
//CLog longClog;

/*
  BSem *mutexLclDiagLock_ ;
  BSem *mutexGlbDiagLock_ ;
*/


void logsRouter(std::string *, std::string *, std::string) throw (xdaq::exception::Exception);

void setClogRequestedFieldsForShortPost(std::string *, std::string *, std::string *);
void setClogRequestedFieldsForLongPost(std::string *, std::string *, std::string *, int *, std::string *, std::string *, std::string *, std::string *);
void cleanupMsgString(std::string *);
void cleanupXtrabuffString(std::string *);



//bool activateDiagSystemForFirstTime_;
bool setAppDiagSystemToOff();
bool setAppDiagSystemToDefault();
bool setDiagSystemToTuned(int process_kind);

bool setEdDiagSystemToDefault();
bool setEdDiagSystemToOff();
bool setGlbEdDiagSystemToDefault();
bool setGlbEdDiagSystemToOff();
bool setLrDiagSystemToDefault();
bool setLrDiagSystemToOff();
bool setEmDiagSystemToDefault();
bool setEmDiagSystemToOff();

std::string systemID_;
std::string subSystemID_;
void setSystemID(std::string systemID);
std::string getSystemID();
void setSubSystemID(std::string systemID);
std::string getSubSystemID();


bool logFileNameHasChanged_;
std::string oldLogFileName_;

bool pixelsLogFileNameHasChanged_;
std::string oldPixelsLogFileName_;

bool log4jLogFileNameHasChanged_;
std::string oldLog4jLogFileName_;

void processOnlineDiagRequest(xoap::MessageReference msg, xoap::MessageReference reply);



void postXoapMessageForReconfAction();
std::string reconfAction_;

//BSEMMOD
toolbox::BSem * sendMsgMutex;
toolbox::BSem * sendReconfRequestMutex;


std::string dbFileName_;
std::string oldDbFileName_;
bool dbFileNameHasChanged_;
/* void checkLrOptions(); */
void checkLrFileOption();


ChainSawManager * chainSawManager_;


int nbrOfChainsawStreams_;


void pushClogToFilteredStreams(CLog l, int streamNum);

std::string logFileRootPrefix_;

std::string expandFireItemWithEnvVariable(std::string rawFireItem);



//std::vector<xoap::MessageReference> glbClogsList_;
//std::vector<xoap::MessageReference> lclClogsList_;
bool fiAreInitialized_;

bool useCsEnvSettings_;

void overrideCsSettings (void);

//Used for connection to CS via environment variables
std::string csConfMachine_;
std::string csConfPort_;
std::string csConfReceiverName_;
std::string csConfLogLevel_;



std::vector<CLog> longClogsList_;
std::vector<CLog> shortClogsList_;
std::vector<CLog> shortClogsRepostList_;

void flushFirstMessages (void);


bool isProcessingNestedShortClog_;

bool isProcessingNestedLongClog_;


void processReportErrorforShortClog(std::string msg, std::string lvl, std::string extraBuffer);


void processReportReconfCommandforLongClog(	std::string msg, std::string lvl, std::string text, int errorcode, std::string faultstate, std::string systemid, std::string subsystemid, std::string extraBuffer);


void processReportErrorforLongClog(	std::string msg, std::string lvl, std::string text, int errorcode, std::string faultstate, std::string systemid, std::string subsystemid, std::string extraBuffer);

/*
void processReportErrorBranchingforLongClog(	std::string msg, std::string lvl, std::string text, int errorcode, std::string faultstate, std::string systemid, std::string subsystemid, std::string extraBuffer);
*/


};


#endif // _DiagBagWizard_h_























