/************************************************************
FileName : 	DiagCompileOptions.h
Content :		Compilation options for Diagnostic System
Programmer :	Laurent GROSS
Version : 		TDS 4.0
Last modification date	: 19/12/2006
Support : 		mail to : laurent.gross@ires.in2p3.fr
online help : https://uimon.cern.ch/twiki/bin/view/CMS/DiagnosticSystem
************************************************************/


//These options are now defined at command line during package configuration stage
//Type ./configure --help for more detailed instructions
//#define WILL_COMPILE_FOR_GENERIC
//#define WILL_COMPILE_FOR_PIXELS
//#define WILL_COMPILE_FOR_TRACKER




//#define WILL_COMPILE_AS_STANDALONE

#define XDAQ373


//#define USE_XRELAY
#define XR_TOPNODE "relay"
#define XR_PREFIX   "xr"
#define XR_HTTPADD  "http://xdaq.web.cern.ch/xdaq/xsd/2004/XRelay-10"
#define XR_ACTORNAME   "actor"
#define XR_TONODENAME   "to"
#define XR_URLNAME   "url"
#define XR_URNNAME   "urn"
#define XR_CONTEXTOFFSET   4


//#define NOSLOWDOWN
//#define MAX_LCL_MSG_POOL    5
//#define MAX_GLB_MSG_POOL    5


//#define DEBUG_SENTINEL


#define PUSH_INITIAL_LOGS_DELAY	5


/* If FSM is not used or not implemented, uncomment next lines
for process auto-configuration at load time */
#define AUTO_CONFIGURE_PROCESSES

/* How many seconds after load-time will we wait
before initializing the ErrorDispatcher process ?
This value is useless if AUTO_CONFIGURE_PROCESSES is disabled */
#define AUTO_ED_CONFIGURE_DELAY  5



/* How many seconds after load-time will we wait
before initializing the ErrorDispatcher process ?
This value is useless if AUTO_CONFIGURE_PROCESSES is disabled */
#define AUTO_LED_CONFIGURE_DELAY  5


/* How many seconds after load-time will we wait
before initializing the ErrorDispatcher process ?
This value is useless if AUTO_CONFIGURE_PROCESSES is disabled */
#define AUTO_GED_CONFIGURE_DELAY  1


/* How many seconds after load-time will we wait
before initializing the user process ?
This value is useless if AUTO_CONFIGURE_PROCESSES is disabled */
#define AUTO_UP_CONFIGURE_DELAY  15

/* How many seconds after load-time will we wait
before initializing the sentinel listener process ?
This value is useless if AUTO_CONFIGURE_PROCESSES is disabled */
#define AUTO_SG_CONFIGURE_DELAY  10

/* How many seconds after load-time will we wait
before initializing the sentinel listener process ?
This value is useless if AUTO_CONFIGURE_PROCESSES is disabled */
#define AUTO_LVL1_CONFIGURE_DELAY  10

#define AUTO_LR_CONFIGURE_DELAY  5


#define DIAG_DB_UPLOAD_HALF_RATE  10


#define COMMAND_TO_TRIG_WHEN_ED_RECEIVE_A_LOG	"receivedLog"
#define ANSWER_TO_COMMAND_TRIGGED_WHEN_ED_RECEIVED_A_LOG    "receivedLogResponse"

#define MESSAGE_TYPE_IS_SHORT	"SHORT"
#define MESSAGE_TYPE_IS_LONG	"LONG"

#define FREE_DISTANT_SEMAPHORE_FOR_LCL_ED_COMMAND	"freeLclDiagSemaphore"
#define FREE_DISTANT_SEMAPHORE_FOR_GLB_ED_COMMAND	"freeGlbDiagSemaphore"


#define USLEEP_DELAY_FOR_WAIT_SOAP_ACK	100
#define LOOP_SIZE_FOR_WAIT_SOAP_ACK	10


#define RECONF_COMMAND_ENTRY	"reconfLog"
#define REPLY_TO_RECONF_COMMAND_ENTRY	"reconfLogResponse"




#define TK_ENABLE_WS_EVENTING


#define FEC_DCUREADOUTERROR_PATTERN	"Error during the upload of the DCUs"
#define FEC_WORKLOOPERROR_PATTERN	"Failed to submit, waitingWorkLoop queue ful"
#define FED_DEBUG_PATTERN	"*** Fed Monitor Log ***"
#define FED_TEMP_MON_PATTERN	"*** Fed Tmon Log ***"
#define FED_SPY_PATTERN	"*** Fed Spy Log ***"













































