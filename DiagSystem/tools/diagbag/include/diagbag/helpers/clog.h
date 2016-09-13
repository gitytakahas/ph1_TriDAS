/*
   FileName : 		clog.h

   Content : 		clog class

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

/* Begin Block - avoid multiple inclusions of this file */
#ifndef _clog_h_
#define _clog_h_

#include <string>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "DiagCompileOptions.h"

#define DIAGTRACE       "TRACE"
#define DIAGDEBUG       "DEBUG"
#define DIAGINFO        "INFO"
#define DIAGWARN        "WARN"
#define DIAGUSERINFO    "USERINFO"
#define DIAGERROR       "ERROR"
#define DIAGFATAL       "FATAL"
#define DIAGOFF         "OFF"
#define DIAGUNCHANGED   "UNCHANGED"


#define TRACELEVEL      0
#define DEBUGLEVEL      1
#define INFOLEVEL       2
#define WARNLEVEL       3
#define USERINFOLEVEL   4
#define ERRORLEVEL      5
#define FATALLEVEL      6
#define OFFLEVEL        7


#define TKDIAG_WHITE	"rgb(255, 255, 255)"
#define TKDIAG_GREY	"rgb(204, 204, 204)"
#define TKDIAG_DARKGREY	"rgb(180, 180, 180)"
#define TKDIAG_YELLOW	"rgb(255, 255, 0)"
#define TKDIAG_ORANGE	"rgb(255, 153, 0)"
#define TKDIAG_GREEN	"rgb(60, 180, 0)"
#define TKDIAG_RED		"rgb(255, 0, 0)"
#define TKDIAG_DARKRED	"rgb(130, 0, 0)"


#define TRACECOLOR		TKDIAG_WHITE
#define DEBUGCOLOR		TKDIAG_GREY
#define INFOCOLOR		TKDIAG_GREY
#define WARNCOLOR		TKDIAG_DARKGREY
#define USERINFOCOLOR	    	TKDIAG_GREY
#define ERRORCOLOR		TKDIAG_RED
#define FATALCOLOR		TKDIAG_DARKRED


#define CONSOLE_DUMP_ENABLED	true
#define CONSOLE_DUMP_DISABLED	false




#ifdef WILL_COMPILE_FOR_TRACKER
	#define DIAG_DEFAULT_EBUFF	"<SOFTWARE>TRUE</SOFTWARE>"
#else
	#ifdef WILL_COMPILE_FOR_PIXELS
		#define DIAG_DEFAULT_EBUFF	""
	#else
		//Generic case
		#define DIAG_DEFAULT_EBUFF	""
	#endif
#endif


#ifdef WILL_COMPILE_FOR_TRACKER
	#define DIAG_DEFAULT_EBUFF	"<SOFTWARE>TRUE</SOFTWARE>"
	#else
#endif

// SYSTEM_ID DEFINITIONS
#define DIAG_TRACKER_SYSTEMID	"TRACKER"

// SUB_SYSTEM_ID DEFINITIONS for FEC
#define DIAG_TRACKERFEC_SUBSYSTEMID	"FEC_SUPERVISOR"
#define DIAG_TRACKERDCUFILTER_SUBSYSTEMID	"DCU_FILTER"
#define DIAG_TRACKERCRATECONTROLLER_SUBSYSTEMID	"CRATE_CONTROLLER"
#define DIAG_TRACKERTKCONFDB_SUBSYSTEMID	"TK_CONFIGURATION_DB"


#define DIAG_TRACKERDBCACHE_SUBSYSTEMID	"TK_DB_CACHE"
#define DIAG_TRACKERDS_SUBSYSTEMID	"TK_DATA_SENDER"
#define DIAG_TRACKERFED9USUPERVISOR_SUBSYSTEMID	"FED_9U_SUPERVISOR"
#define DIAG_TRACKERGENLTCSUP_SUBSYSTEMID	"GENERIC_LTC_SUPERVISOR"
#define DIAG_TRACKERGENTTCCISUP_SUBSYSTEMID	"GENERIC_TTCCI_SUPERVISOR"
#define DIAG_TRACKERRA_SUBSYSTEMID	"ROOT_ANALYZER"
#define DIAG_TRACKERSUP_SUBSYSTEMID	"TRACKER_SUPERVISOR"
#define DIAG_TRACKERTSCSUP_SUBSYSTEMID	"TRACKER_TSC_SUPERVISOR"

/*
#ifndef UNICODE
    #define LOG4CPLUS_XML_MAX_MESSAGE_SIZE (8*1024)
#else
    #define LOG4CPLUS_XML_MAX_MESSAGE_SIZE (2*8*1024)
#endif
#include <log4cplus/config.hxx>
#include <log4cplus/appender.h>
#include <log4cplus/helpers/socket.h>
#include <log4cplus/layout.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/spi/loggingevent.h>
#include <log4cplus/socketappender.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
*/
class CLog
{
    private :
        long long timestamp;
        std::string level;
        std::string errorcode;
        std::string message;
        std::string timestampAsString;
        std::string logger;

        std::string text;
        std::string faultstate;
        std::string systemid;
        std::string subsystemid;
        std::string classname;
        int instance;
/*        int lid;
        long long edtimestamp;
        std::string lvl1name;
        std::string lvl1instance;
        std::string routetoem;
        std::string dbloglevel;
        std::string csloglevel;
        std::string ccloglevel;
        std::string forceddblog;
        std::string forcedcslog;
        std::string forcedcclog; */
/*        std::string gloglevel;
        bool edReconfStatus; */
		#ifdef USE_XRELAY
	        std::string senderURL;
	        std::string senderURN;
    	    bool talkToLocalEd;
	        int missedMessages;
    		std::string missedMessagesAsString;
		#endif
		std::string typeOfMessage;
        std::string edTimestampAsString;
        std::string extraBuffer;
	
		bool isNested;
        std::string subClassPath;
        std::string fileName;
        int lineNumber;
		bool addSubClassPath;
		bool addFileName;
		bool addLineNumber;

		//Not filled at item creation
	    std::string lineNumberAsString;

		std::string crateId;
		std::string slotId;	
		
		std::string partitionName_;	


    public :
        CLog()
        {
            timestamp=0;
            level="";
/*            thread=""; */
            logger="";
			message="";
/*            file="";
            line=0;
            NDC=""; */
            text="";
            errorcode="";
            faultstate="";
            systemid="";
            subsystemid="";
            classname="";
            instance=0;
/*            lid=0;
            edtimestamp=0;
            lvl1name="";
            lvl1instance="";
            routetoem="";
            dbloglevel="";
            csloglevel="";
            ccloglevel="";
            forceddblog="";
            forcedcslog="";
            forcedcclog="";
            gloglevel="";
            edReconfStatus=false; */
			#ifdef USE_XRELAY
	        	senderURL = "";
	        	senderURN = "";
    	    	talkToLocalEd = false;
	        	missedMessages = 0;
    			missedMessagesAsString = "";
			#endif
			typeOfMessage = "";
        	timestampAsString = "";
        	edTimestampAsString = "";
        	extraBuffer = "";


			isNested = false;
        	subClassPath = "";
        	fileName = "";
        	lineNumber = 0;
			addSubClassPath = false;
			addFileName = false;
			addLineNumber = false;

			crateId = "NA";
			slotId = "NA";
			partitionName_ = "NA";
        }


        ~CLog(){}


        CLog(long long t,
            std::string l,
            /*std::string th,*/
            std::string logg,
            std::string m,
            /*std::string f,
            int ln,
            std::string N,*/
            std::string txt,
            std::string ec,
            std::string fs,
            std::string sid,
            std::string ssid,
            std::string cn,
            int inst,
/*            int locid,
            long long edts,
            std::string l1n,
            std::string l1i,
            std::string rtem,
            std::string dbll,
            std::string csll,
            std::string ccll,
            std::string fdbl,
            std::string fcsl,
            std::string fccl,
            std::string gll,
            bool ers, */
			#ifdef USE_XRELAY
	        	std::string sURL,
	        	std::string sURN,
    	    	bool talkLclEd,
	        	int mm,
    			std::string mms,
			#endif
			std::string tOfM,
        	std::string tss,
        	std::string edtss,
			std::string extBuffer,
		bool isnested,
		bool addsubclasspath,
        std::string subclasspath,
		bool addfilename,
        std::string filename,
		bool addlinelumber,
        int lineNumber)
        {
            timestamp=t;
            level=l;
/*            thread=th;*/
            logger=logg;
            message = m;
/*            file=f;
            line=ln;
            NDC=N;*/
            text=txt;
            errorcode=ec;
            faultstate=fs;
            systemid=sid;
            subsystemid=ssid;
            classname=cn;
            instance=inst;
/*            lid=locid;
            edtimestamp=edts;
            lvl1name=l1n;
            lvl1instance=l1i;
            routetoem=rtem;
            dbloglevel=dbll;
            csloglevel=csll;
            ccloglevel=ccll;
            forceddblog=fdbl;
            forcedcslog=fcsl;
            forcedcclog=fccl;
            gloglevel=gll;
            edReconfStatus=ers;*/
			#ifdef USE_XRELAY
	        	senderURL = sURL;
	        	senderURN = sURN;
    	    	talkToLocalEd = talkLclEd;
	        	missedMessages = mm;
    			missedMessagesAsString = mms;
			#endif
			typeOfMessage = tOfM;
        	timestampAsString = tss;
        	edTimestampAsString = edtss;
			extraBuffer = extBuffer;

			isNested = isnested;
			addSubClassPath = addsubclasspath;
        	subClassPath = subclasspath;
			addFileName = addfilename;
        	fileName = filename;
			addLineNumber = addlinelumber;
        	lineNumber = lineNumber;

			crateId = "NA";
			slotId = "NA";
			partitionName_ = "NA";

        }

        long long getTimeStamp() {return timestamp;}
        void setTimeStamp(long long ts)	{timestamp=ts;}

        std::string getLevel() {return level;}
        void setLevel(std::string s) {level=s;}
/*
        std::string getThread() {return thread;}
        void setThread(std::string s) {thread=s;}
*/
        std::string getLogger() {return logger;}
        void setLogger(std::string l) {logger = l;}

        std::string getMessage() {return message;}
        void setMessage(std::string s) {message=s;}
/*
        std::string getFile() {return file;}
        void setFile(std::string s) {file=s;}

        int getLine() {return line;}
        void setLine(int l) {line=l;}

        std::string getNDC() {return NDC;}
        void setNDC(std::string s) {NDC=s;}
*/
        std::string getText() {return text;}
        void setText(std::string s) {text=s;}

        std::string getErrorCode() {return errorcode;}
        void setErrorCode(std::string s) {errorcode=s;}

        std::string getFaultState() {return faultstate;}
        void setFaultState(std::string s) {faultstate=s;}

        std::string getSystemID() {return systemid;}
        void setSystemID(std::string s) {systemid=s;}

        std::string getSubSystemID() {return subsystemid;}
        void setSubSystemID(std::string s) {subsystemid=s;}

        std::string getClassName() {return classname;}
        void setClassName(std::string s) {classname=s;}

        int getInstance() {return instance;}
        void setInstance(int l) {instance=l;}
/*
        int getLid() {return lid;}
        void setLid(int l) {lid=l;}

        long long getEdTimeStamp() {return edtimestamp;}
        void setEdTimeStamp(long long ts) {edtimestamp=ts;}

        std::string getLvl1Name() {return lvl1name;}
        void setLvl1Name(std::string s) {lvl1name=s;}

        std::string getLvl1Instance() {return lvl1instance;}
        void setLvl1Instance(std::string s) {lvl1instance=s;}

        std::string getRouteToEm() {return routetoem;}
        void setRouteToEm(std::string s) {routetoem=s;}

        std::string getDbLogLevel() {return dbloglevel;}
        void setDbLogLevel(std::string s) {dbloglevel=s;}

        std::string getCsLogLevel() {return csloglevel;}
        void setCsLogLevel(std::string s) {csloglevel=s;}

        std::string getCcLogLevel() {return ccloglevel;}
        void setCcLogLevel(std::string s) {ccloglevel=s;}

        std::string getForcedDbLog() {return forceddblog;}
        void setForcedDbLog(std::string s) {forceddblog=s;}

        std::string getForcedCsLog() {return forcedcslog;}
        void setForcedCsLog(std::string s) {forcedcslog=s;}

        std::string getForcedCcLog() {return forcedcclog;}
        void setForcedCcLog(std::string s) {forcedcclog=s;}

        std::string getGLogLevel() {return gloglevel;}
        void setGLogLevel(std::string s) {gloglevel=s;}

        bool getEdReconfStatus() {return edReconfStatus;}
        void setEdReconfStatus(bool b) {edReconfStatus=b;}

*/
		#ifdef USE_XRELAY
	        std::string getSenderURL() {return senderURL;}
    	    void setSenderURL(std::string s) {senderURL=s;}

	        std::string getSenderURN() {return senderURN;}
    	    void setSenderURN(std::string s) {senderURN=s;}

	        bool getTalkToLocalEd() {return talkToLocalEd;}
    	    void setTalkToLocalEd(bool b) {talkToLocalEd=b;}

	        int getMissedMessages() {return missedMessages;}
    	    void setMissedMessages(int l) {missedMessages=l;}

	        std::string getMissedMessagesAsString() {return missedMessagesAsString;}
    	    void setMissedMessagesAsString(std::string s) {missedMessagesAsString=s;}
		#endif

		std::string getTypeOfMessage() {return typeOfMessage;}
		void setTypeOfMessage(std::string s) {typeOfMessage=s;}

        std::string getTimestampAsString() {return timestampAsString;}
   	    void setTimestampAsString(std::string s) {timestampAsString=s;}

        std::string getEdTimestampAsString() {return edTimestampAsString;}
        void setEdTimestampAsString(std::string s) {edTimestampAsString=s;}



        std::string getExtraBuffer() {return extraBuffer;}
        void setExtraBuffer(std::string s) {extraBuffer=s;}



        bool getIsNested() {return isNested;}
   	    void setIsNested(bool b) {isNested=b;}


        bool getAddSubClassPath() {return addSubClassPath;}
   	    void setAddSubClassPath(bool b) {addSubClassPath=b;}

        std::string getSubClassPath() {return subClassPath;}
        void setSubClassPath(std::string s) {subClassPath=s;}


        bool getAddFileName() {return addFileName;}
   	    void setAddFileName(bool b) {addFileName=b;}

        std::string getFileName() {return fileName;}
        void setFileName(std::string s) {fileName=s;}


        bool getAddLineNumber() {return addLineNumber;}
   	    void setAddLineNumber(bool b) {addLineNumber=b;}

        int getLineNumber() {return lineNumber;}
        void setLineNumber(int i) {lineNumber=i;}

        std::string getLineNumberAsString() {return lineNumberAsString;}
        void setLineNumberAsString(std::string s) {lineNumberAsString=s;}



        std::string getCrateId() {return crateId;}
        void setCrateId(std::string s) {crateId=s;}

        std::string getSlotId() {return slotId;}
        void setSlotId(std::string s) {slotId=s;}


        std::string getPartitionName() {return partitionName_;}
        void setPartitionName(std::string s) {partitionName_=s;}



/*

        std::string toStringBis()
        {
            std::string r="";
            r += "Message:" + message;
            r += " Origin:" + logger;
            std::ostringstream converter_str;
            converter_str << timestamp;
            r += " Timestamp:" + converter_str.str();

            std::ostringstream converter_str0;
            converter_str0 << missedMessages;
            r += " MissedMessages:" + converter_str0.str();

            r += " Thread:" + thread;
            r += " Level:" + level;
            r += " File:" + file;
            converter_str.str("");
            converter_str << line;
            r += " Line:"+converter_str.str();
            r += " NDC:"+NDC;
            r += " Text:"+text;
            r += " ErrorCode:"+errorcode;
            r += " FaultState:"+faultstate;
            r += " SystemID:"+systemid;
            r += " SubSystemID:"+subsystemid;
            r += " ClassName:"+classname;
            std::ostringstream converter_str1;
            converter_str1 << instance;
            r += " Instance:" + converter_str1.str();
            std::ostringstream converter_str2;
            converter_str2 << lid;
            r += " LocalID:" + converter_str2.str();
            std::ostringstream converter_str3;
            converter_str3 << edtimestamp;
            r += " ErrorDispatcherTimestamp:" + converter_str3.str();
            r += " Lvl1Name:"+lvl1name;
            r += " Lvl1Instance:"+lvl1instance;
            r += " RouteToErrorManager:"+routetoem;
            r += " DatabaseLogLevel:"+dbloglevel;
            r += " ChainsawLogLevel:"+csloglevel;
            r += " ConsoleLogLevel:"+ccloglevel;
            r += " ForceDatabaseLogging:"+forceddblog;
            r += " ForceChainsawLogging:"+forcedcslog;
            r += " ForceConsoleLogging:"+forcedcclog;
            r += " GlobalLogLevel:"+gloglevel;
            r += " EDReconfStatus:";
            if (edReconfStatus==true) r += "true"; else r += "false";
        return r;
        }

*/


/*

        std::string toParsableString(std::string separator, std::string startlogtag, std::string endlogtag)
        {
            std::string itemSeparator=separator;
            std::string r="";
            r += startlogtag;
            r += itemSeparator +"Message=\"" + message + "\"";
            r += itemSeparator + "Origin=\"" + logger+ "\"";
            std::ostringstream converter_str;
            converter_str << timestamp;
            r += itemSeparator + "Timestamp=\"" + converter_str.str() + "\"";
            r += itemSeparator + "Thread=\"" + thread + "\"";
            r += itemSeparator + "Level=\"" + level + "\"";
            r += itemSeparator + "File=\"" + file + "\"";
            converter_str.str("");
            converter_str << line;
            r += itemSeparator + "Line=\""+converter_str.str() + "\"";
            r += itemSeparator + "NDC=\""+NDC + "\"";
            r += itemSeparator + "Text=\""+text + "\"";
            r += itemSeparator + "ErrorCode=\""+errorcode + "\"";
            r += itemSeparator + "FaultState=\""+faultstate + "\"";
            r += itemSeparator + "SystemID=\""+systemid + "\"";
            r += itemSeparator + "SubSystemID=\""+subsystemid + "\"";
            r += itemSeparator + "ClassName=\""+classname + "\"";
            std::ostringstream converter_str1;
            converter_str1 << instance;
            r += itemSeparator + "Instance=\"" + converter_str1.str() + "\"";
            std::ostringstream converter_str2;
            converter_str2 << lid;
            r += itemSeparator + "LocalID=\"" + converter_str2.str() + "\"";
            std::ostringstream converter_str3;
            converter_str3 << edtimestamp;
            r += itemSeparator + "ErrorDispatcherTimestamp=\"" + converter_str3.str() + "\"";
            r += itemSeparator + "Lvl1Name=\""+lvl1name + "\"";
            r += itemSeparator + "Lvl1Instance=\""+lvl1instance + "\"";
            r += itemSeparator + "RouteToErrorManager=\""+routetoem + "\"";
            r += itemSeparator + "DatabaseLogLevel=\""+dbloglevel + "\"";
            r += itemSeparator + "ChainsawLogLevel=\""+csloglevel + "\"";
            r += itemSeparator + "ConsoleLogLevel=\""+ccloglevel + "\"";
            r += itemSeparator + "ForceDatabaseLogging=\""+forceddblog + "\"";
            r += itemSeparator + "ForceChainsawLogging=\""+forcedcslog + "\"";
            r += itemSeparator + "ForceConsoleLogging=\""+forcedcclog + "\"";
            r += itemSeparator + "GlobalLogLevel=\""+gloglevel + "\"";
            r += itemSeparator + "EDReconfStatus=\"";
            if (edReconfStatus==true) r += "true"; else r += "false";
            r += "\"";
            r += itemSeparator + endlogtag;
        return r;
        }

*/

/*
        std::string tostring()
        {
            std::string r="";
            r += "Logger : "+logger+"\n";
            std::ostringstream converter_str;
            converter_str << timestamp;
            r += "Timestamp : "+converter_str.str()+"\n";
            r += "Level : "+level+"\n";
            r += "Message : "+message+"\n";
            r += "Thread : "+thread+"\n";
            r += "File : "+file+"\n";
            converter_str.str("");
            converter_str << line;
            r += "Line : "+converter_str.str()+"\n";
            r += "NDC : "+NDC+"\n";
            return r;
        }


*/



	std::string buildStringForConsoleOutput()
	{
	    std::string newMsg;
	    if (getTypeOfMessage() == MESSAGE_TYPE_IS_LONG)
	    {
	    	newMsg = getLevel() +
    	    	    	 " <<>> Origin:" + getLogger() +
    	    	    	 " <<>> Timestamp:" + getTimestampAsString() +
    	    	    	 " <<>> ErrorCode:" + getErrorCode() +
						 /*
    	    	    	 " <<>> FaultState:" + getFaultState() +
    	    	    	 " <<>> SystemID:" + getSystemID() +
    	    	    	 " <<>> SubSystemID:" + getSubSystemID() +
						 */
    	    	    	 "\nMessage : " + getMessage() + "\n";
			if (getText() == "RECONFCMD") newMsg = newMsg + "This is a RECONFIGURATION command.\n";
	    }
	    else
	    {
	    	newMsg = getLevel() +
    	    	    	 " <<>> Origin:" + getLogger() +
    	    	    	 " <<>> Timestamp:" + getTimestampAsString() +
    	    	    	 "\nMessage : " + getMessage() + "\n";
	    }
	    
	    return newMsg;

	}


/*
	std::string buildStringForLogFileOutput(unsigned long logNumber)
	{
	    std::string newMsg;
	    char ln[20];
	    snprintf(ln, 19, "%.6lu", logNumber);
	    newMsg = (std::string)ln + "\n";
	    newMsg = newMsg + (std::string)getMessage() + "\n";
	    newMsg = newMsg + (std::string)getLogger() + "\n";
	    newMsg = newMsg + (std::string)getTimestampAsString() + "\n";
	    newMsg = newMsg + (std::string)getLevel() + "\n";
	    newMsg = newMsg + (std::string)getErrorCode() + "\n";
	    newMsg = newMsg + (std::string)"END_OF_LOG_RECORD" + "\n";

	    return newMsg;

	}
*/


/*


	std::string buildStringForDbFileOutput()
	{
	    std::string newMsg;
	    newMsg = newMsg + (std::string)"START_OF_DB_RECORD" + "\n";
	    newMsg = newMsg + (std::string)"MESSAGE=" + (std::string)getMessage() + "\n";
		expandLoggerInfos(newMsg);
	    // newMsg = newMsg + (std::string)"LOGGER=" + (std::string)getLogger() + "\n";
	    newMsg = newMsg + (std::string)"TIMESTAMP=" + (std::string)getTimestampAsString() + "\n";
	    newMsg = newMsg + (std::string)"LEVEL=" + (std::string)getLevel() + "\n";
	    newMsg = newMsg + (std::string)"ERRORCODE=" + (std::string)getErrorCode() + "\n";
	    newMsg = newMsg + (std::string)"SYSTEMID=" + (std::string)getSystemID() + "\n";
	    newMsg = newMsg + (std::string)"SUBSYSTEMID=" + (std::string)getSubSystemID() + "\n";
	    newMsg = newMsg + (std::string)"EXTRABUFFER=" + (std::string)getExtraBuffer() + "\n";
	    newMsg = newMsg + (std::string)"END_OF_DB_RECORD" + "\n";
	    return newMsg;

	}


	void expandLoggerInfos(std::string& newMsg)
	{
		int loc = 0;
		int len;

		//Find machine name
		len = getLogger().rfind(".p:");
		std::string s1 = getLogger().substr(loc, len);
	    newMsg = newMsg + (std::string)"MACHINE=" + s1 + "\n";

		//Find Port number
		loc = len+3;
		len = 0;
		while ( (getLogger()).c_str()[loc + len] != '.' )
		{
			len++;
		}
		std::string s2 = getLogger().substr(loc, len);
	    newMsg = newMsg + (std::string)"PORT=" + s2 + "\n";


		//Find Process Name
		loc = loc + len + 1;
		len = 0;
		while ( (getLogger()).c_str()[loc + len] != '.' )
		{
			len++;
		}
		std::string s3 = getLogger().substr(loc, len);
	    newMsg = newMsg + (std::string)"PROCESSNAME=" + s3 + "\n";


		//Find Process instance
		loc = loc + len + 1;
		while ( (getLogger()).c_str()[loc] != '(' )
		{
			loc++;
		}
		loc++;
		len = 0;
		while ( (getLogger()).c_str()[loc + len] != ')' )
		{
			len++;
		}
		std::string s4 = getLogger().substr(loc, len);
	    newMsg = newMsg + (std::string)"PROCESSINSTANCE=" + s4 + "\n";
	}
*/


/*
	std::string buildStringForDbFileOutput(unsigned long logNumber)
	{
	    std::string newMsg;
	    char ln[20];
	    newMsg = (std::string)"<ONELOG>" + "\n";
	    snprintf(ln, 19, "%.6lu", logNumber);
	    newMsg = newMsg + (std::string)"<LOGNUMBER>" + (std::string)ln + (std::string)"</LOGNUMBER>\n";
	    newMsg = newMsg + (std::string)"<MESSAGE>" + (std::string)getMessage() + (std::string)"</MESSAGE>\n";
		expandLoggerInfos(newMsg);
	    // newMsg = newMsg + (std::string)"<LOGGER>" + (std::string)getLogger() + (std::string)"</LOGGER>\n";
	    newMsg = newMsg + (std::string)"<TIMESTAMP>" + (std::string)getTimestampAsString() + (std::string)"</TIMESTAMP>\n";
	    newMsg = newMsg + (std::string)"<LEVEL>" + (std::string)getLevel() + (std::string)"</LEVEL>\n";
	    newMsg = newMsg + (std::string)"<ERRORCODE>" + (std::string)getErrorCode() + (std::string)"</ERRORCODE>\n";
	    newMsg = newMsg + (std::string)"<SYSTEMID>" + (std::string)getSystemID() + (std::string)"</SYSTEMID>\n";
	    newMsg = newMsg + (std::string)"<SUBSYSTEMID>" + (std::string)getSubSystemID() + (std::string)"</SUBSYSTEMID>\n";
	    newMsg = newMsg + (std::string)"<EXTRABUFFER>" + (std::string)getExtraBuffer() + (std::string)"</EXTRABUFFER>\n";
	    newMsg = newMsg + (std::string)"</ONELOG>" + "\n";
	    return newMsg;

	}
*/





/*
//std::string removeFromExtraBuffer(std::string openingTag, std::string closingTag, std::string& eBuff, int * pixType)
std::string removeFromExtraBuffer(std::string openingTag, std::string closingTag, int * pixType)
{
//#define DEBUG_EBUFFXTRACT
std::string eBuff = getExtraBuffer();
std::string resu = eBuff;
std::string resu1 = "-1";

		std::string::size_type locB = eBuff.find( openingTag, 0 );
		if( locB != std::string::npos )
		{
			#ifdef DEBUG_EBUFFXTRACT
				std::cout << openingTag << " found at loc " << locB << ". ";
			#endif
			std::string::size_type locE = eBuff.find( closingTag, 0 );
			if( locE != std::string::npos )
			{
				#ifdef DEBUG_EBUFFXTRACT
					std::cout << closingTag << " found at loc " << locE << ". ";
				#endif
				resu1 = eBuff.substr( (locB + openingTag.length()), (locE - openingTag.length() - locB) );
				resu = eBuff.substr( locE + closingTag.length(), eBuff.length() );
				setExtraBuffer(resu);
				int lclPixType = atoi( resu1.c_str() );
				*pixType = lclPixType;

				// *isKindOf = true;
				#ifdef DEBUG_EBUFFXTRACT
					std::cout << "Remaining Substring after extraction: " << std::endl << resu  << std::endl;
					std::cout << "Error is of type: " << lclPixType  << std::endl;
				#endif
			}
			else
			{
				#ifdef DEBUG_EBUFFXTRACT
					std::cout << closingTag << " not found. ";
				#endif
			}
		}
		else
		{
			#ifdef DEBUG_EBUFFXTRACT
				std::cout << openingTag << " not found. ";
			#endif
		}
		#ifdef DEBUG_EBUFFXTRACT
			std::cout << std::endl;
		#endif
return resu;
}

*/



std::string removeFromExtraBuffer(std::string openingTag, std::string closingTag)
{
//#define DEBUG_EBUFFXTRACT
std::string eBuff = getExtraBuffer();
std::string resu = eBuff;
std::string resu1 = "-1";

		std::string::size_type locB = eBuff.find( openingTag, 0 );
		if( locB != std::string::npos )
		{
			#ifdef DEBUG_EBUFFXTRACT
				std::cout << openingTag << " found at loc " << locB << ". ";
			#endif
			std::string::size_type locE = eBuff.find( closingTag, 0 );
			if( locE != std::string::npos )
			{
				#ifdef DEBUG_EBUFFXTRACT
					std::cout << closingTag << " found at loc " << locE << ". ";
				#endif
				resu1 = eBuff.substr( (locB + openingTag.length()), (locE - openingTag.length() - locB) );
				resu = eBuff.substr( 0, locB ) + eBuff.substr( locE + closingTag.length(), eBuff.length() );
				setExtraBuffer(resu);

				#ifdef DEBUG_EBUFFXTRACT
					std::cout << "Remaining Substring after extraction: " << std::endl << resu  << std::endl;
				#endif
			}
			else
			{
				#ifdef DEBUG_EBUFFXTRACT
					std::cout << closingTag << " not found. ";
				#endif
			}
		}
		else
		{
			#ifdef DEBUG_EBUFFXTRACT
				std::cout << openingTag << " not found. ";
			#endif
		}
		#ifdef DEBUG_EBUFFXTRACT
			std::cout << std::endl;
		#endif
return resu;
}





//std::string findInExtraBuffer(std::string openingTag, std::string closingTag, std::string& eBuff, bool * isKindOf)
std::string findInExtraBuffer(std::string openingTag, std::string closingTag, std::string& eBuff)
{
//#define DEBUG_EBUFF
std::string resu = "NA";
		std::string::size_type locB = eBuff.find( openingTag, 0 );
		if( locB != std::string::npos )
		{
			#ifdef DEBUG_EBUFF
				std::cout << openingTag << " found at loc " << locB << ". ";
			#endif
			std::string::size_type locE = eBuff.find( closingTag, 0 );
			if( locE != std::string::npos )
			{
				#ifdef DEBUG_EBUFF
					std::cout << closingTag << " found at loc " << locE << ". ";
				#endif
				resu = eBuff.substr( (locB + openingTag.length()), (locE - openingTag.length() - locB) );
				// *isKindOf = true;
				#ifdef DEBUG_EBUFF
					std::cout << "Found Substring : " << resu;
				#endif
			}
			else
			{
				#ifdef DEBUG_EBUFF
					std::cout << closingTag << " not found. ";
				#endif
			}
		}
		else
		{
			#ifdef DEBUG_EBUFF
				std::cout << openingTag << " not found. ";
			#endif
		}
		#ifdef DEBUG_EBUFF
			std::cout << std::endl;
		#endif
return resu;
}



/*
std::string reworkBuffer(std::string XtraBuff)
{
char buff[2048];
strcpy(buff, XtraBuff.c_str());
	for (unsigned int i=0; i<XtraBuff.length(); i++)
	{
		//std::cout << buff[i] << "   ";
		if (buff[i] == '<')
		{
			buff[i] = '-';
			if ( (i<XtraBuff.length()-1) && (buff[i+1] == '/') ) buff[i+1] = '-';
		}
		if (buff[i] == '>') buff[i] = '-';
		if (buff[i] == '&') buff[i] = ' ';
		if (buff[i] == '\t') buff[i] = ' ';
		if (buff[i] == '\n') buff[i] = ' ';
		if (buff[i] == '\\') buff[i] = ' ';
		//std::cout << buff[i] << std::endl;
	}
return (std::string)buff;
}
*/




void reworkBuffer1(std::string * buff)
{
//char buff[2048];
//strcpy(buff, XtraBuff.c_str());
	for (unsigned int i=0; i<buff->length(); i++)
	{
		//std::cout << (*buff)[i] << "   ";
		if ((*buff)[i] == '<')
		{
			(*buff)[i] = '-';
			if ( (i<buff->length()-1) && ((*buff)[i+1] == '/') ) (*buff)[i+1] = '-';
		}
		if ((*buff)[i] == '>') (*buff)[i] = '-';
		if ((*buff)[i] == '&') (*buff)[i] = ' ';
		if ((*buff)[i] == '\t') (*buff)[i] = ' ';
		if ((*buff)[i] == '\n') (*buff)[i] = ' ';
		if ((*buff)[i] == '\\') (*buff)[i] = ' ';
		//std::cout << (*buff)[i] << std::endl;
	}
//return (std::string)buff;
}



/*
	std::string buildStringForLogFileOutput(unsigned long logNumber)
	{
		bool isFecLog = false;
		bool isFedLog = false;
		bool isTriggerLog = false;
		bool isPsuLog = false;
		bool isSoftwareLog = false;
		bool isOtherLog = false;
		
	    std::string newMsg;
		std::string eBuff = getExtraBuffer();
	    newMsg = (std::string)"<DATA>" + "\n";
	    newMsg = newMsg + (std::string)"\t<LEVEL>" + (std::string)getLevel() + (std::string)"</LEVEL>\n";
	    newMsg = newMsg + (std::string)"\t<ERR_CODE>" + (std::string)getErrorCode() + (std::string)"</ERR_CODE>\n";
	    newMsg = newMsg + (std::string)"\t<ERR_MSG>" + (std::string)getMessage() + (std::string)"</ERR_MSG>\n";
	    newMsg = newMsg + (std::string)"\t<ERR_TIMESTAMP>" + (std::string)getTimestampAsString() + (std::string)"</ERR_TIMESTAMP>\n";
	    newMsg = newMsg + (std::string)"\t<ERR_UID>" + (std::string)getLogger() + (std::string)"</ERR_UID>\n";
	    newMsg = newMsg + (std::string)"\t<RUN_TYPE>" + (std::string)"NA" + (std::string)"</RUN_TYPE>\n";
	    newMsg = newMsg + (std::string)"\t<RUN_NUMBER>" + (std::string)"0" + (std::string)"</RUN_NUMBER>\n";
	    newMsg = newMsg + (std::string)"\t<ERR_NBR_INSTANCE>" + (std::string)"NA" + (std::string)"</ERR_NBR_INSTANCE>\n";



	    newMsg = newMsg + (std::string)"\t<FECHARDID>" + findInExtraBuffer("<FECHARDID>", "</FECHARDID>",eBuff, &isFecLog) + (std::string)"</FECHARDID>\n";
	    newMsg = newMsg + (std::string)"\t<RING>" + findInExtraBuffer("<RING>", "</RING>",eBuff, &isOtherLog) + (std::string)"</RING>\n";
	    newMsg = newMsg + (std::string)"\t<CCU>" + findInExtraBuffer("<CCU>", "</CCU>",eBuff, &isOtherLog) + (std::string)"</CCU>\n";
	    newMsg = newMsg + (std::string)"\t<I2CCHANNEL>" + findInExtraBuffer("<I2CCHANNEL>", "</I2CCHANNEL>",eBuff, &isOtherLog) + (std::string)"</I2CCHANNEL>\n";
	    newMsg = newMsg + (std::string)"\t<I2CADDRESS>" + findInExtraBuffer("<I2CADDRESS>", "</I2CADDRESS>",eBuff, &isOtherLog) + (std::string)"</I2CADDRESS>\n";
	    newMsg = newMsg + (std::string)"\t<SOFTWARETAGVERSION>" + findInExtraBuffer("<SOFTWARETAGVERSION>", "</SOFTWARETAGVERSION>",eBuff, &isOtherLog) + (std::string)"</SOFTWARETAGVERSION>\n";

	    newMsg = newMsg + (std::string)"\t<FEDID>" + findInExtraBuffer("<FEDID>", "</FEDID>",eBuff, &isFedLog) + (std::string)"</FEDID>\n";
	    newMsg = newMsg + (std::string)"\t<FEDCHANNEL>" + findInExtraBuffer("<FEDCHANNEL>", "</FEDCHANNEL>",eBuff, &isOtherLog) + (std::string)"</FEDCHANNEL>\n";

	    newMsg = newMsg + (std::string)"\t<TRIGGER>" + findInExtraBuffer("<TRIGGER>", "</TRIGGER>",eBuff, &isTriggerLog) + (std::string)"</TRIGGER>\n";

	    newMsg = newMsg + (std::string)"\t<PSUID>" + findInExtraBuffer("<PSUID>", "</PSUID>",eBuff, &isPsuLog) + (std::string)"</PSUID>\n";

	    


if ( (isFecLog == false) && (isFedLog == false) && (isTriggerLog == false) && (isPsuLog == false) )
{
	newMsg = newMsg + (std::string)"\t<SOFTWARE>TRUE</SOFTWARE>\n";
	isSoftwareLog = true;
}
else newMsg = newMsg + (std::string)"\t<SOFTWARE>NA</SOFTWARE>\n";

if (isFecLog == true) std::cout << "FEC log identified" << std::endl;
if (isFedLog == true) std::cout << "FED log identified" << std::endl;
if (isTriggerLog == true) std::cout << "TRIGGER log identified" << std::endl;
if (isPsuLog == true) std::cout << "PSU log identified" << std::endl;
if (isSoftwareLog == true) std::cout << "SOFTWARE log identified" << std::endl;

	    newMsg = newMsg + (std::string)"</DATA>" + "\n";

	    return newMsg;
	}

*/




	void buildStringForLogFileOutput(std::string * newMsg, unsigned long logNumber)
	{
	
	    //std::string newMsg;
		std::string eBuff = getExtraBuffer();
		std::string stringToRework="";

	    (*newMsg) = (std::string)"<DATA>" + "\n";

	    char ln[20];
	    snprintf(ln, 19, "%.6lu", logNumber);
	    (*newMsg) = (*newMsg) + (std::string)"\t<LOGNUMBER>" + (std::string)ln + (std::string)"</LOGNUMBER>\n";
	    (*newMsg) = (*newMsg) + (std::string)"\t<LEVEL>" + (std::string)getLevel() + (std::string)"</LEVEL>\n";
	    (*newMsg) = (*newMsg) + (std::string)"\t<ERR_CODE>" + (std::string)getErrorCode() + (std::string)"</ERR_CODE>\n";


//	    (*newMsg) = (*newMsg) + (std::string)"\t<ERR_MSG>" + reworkBuffer(getMessage()) + (std::string)"</ERR_MSG>\n";


		stringToRework = getMessage();
		reworkBuffer1(&stringToRework);
	    (*newMsg) = (*newMsg) + (std::string)"\t<ERR_MSG>" + stringToRework + (std::string)"</ERR_MSG>\n";




	    (*newMsg) = (*newMsg) + (std::string)"\t<ERR_TIMESTAMP>" + (std::string)getTimestampAsString() + (std::string)"</ERR_TIMESTAMP>\n";
	    (*newMsg) = (*newMsg) + (std::string)"\t<ERR_UID>" + (std::string)getLogger() + (std::string)"</ERR_UID>\n";


//	    (*newMsg) = (*newMsg) + (std::string)"\t<XTRA_BUFFER>" + reworkBuffer(getExtraBuffer()) + (std::string)"</XTRA_BUFFER>\n";

		stringToRework = getExtraBuffer();
		reworkBuffer1(&stringToRework);
	    (*newMsg) = (*newMsg) + (std::string)"\t<XTRA_BUFFER>" + stringToRework + (std::string)"</XTRA_BUFFER>\n";

	    (*newMsg) = (*newMsg) + (std::string)"</DATA>" + "\n";

	    //return newMsg;
	}






	void buildStringForTrackerLogFileOutput(std::string * newMsg, unsigned long logNumber, int runNumberFromRcms)
	{

//	    std::string newMsg;
		std::string eBuff = getExtraBuffer();
		std::string stringToRework="";




	    (*newMsg) = (std::string)"<DATA>" + "\n";

	    char ln[20];
	    snprintf(ln, 19, "%.6lu", logNumber);
	    (*newMsg) = (*newMsg) + (std::string)"\t<LOGNUMBER>" + (std::string)ln + (std::string)"</LOGNUMBER>\n";

	    (*newMsg) = (*newMsg) + (std::string)"\t<LEVEL>" + (std::string)getLevel() + (std::string)"</LEVEL>\n";
	    (*newMsg) = (*newMsg) + (std::string)"\t<ERR_CODE>" + (std::string)getErrorCode() + (std::string)"</ERR_CODE>\n";

		stringToRework = getMessage();
		reworkBuffer1(&stringToRework);
	    (*newMsg) = (*newMsg) + (std::string)"\t<ERR_MSG>" + stringToRework + (std::string)"</ERR_MSG>\n";


	    (*newMsg) = (*newMsg) + (std::string)"\t<ERR_TIMESTAMP>" + (std::string)getTimestampAsString() + (std::string)"</ERR_TIMESTAMP>\n";
	    (*newMsg) = (*newMsg) + (std::string)"\t<ERR_UID>" + (std::string)getLogger() + (std::string)"</ERR_UID>\n";
	    (*newMsg) = (*newMsg) + (std::string)"\t<RUN_TYPE>" + (std::string)"NA" + (std::string)"</RUN_TYPE>\n";


	    char rn[20];
	    snprintf(rn, 19, "%i", runNumberFromRcms);
	    (*newMsg) = (*newMsg) + (std::string)"\t<RUN_NUMBER>" + (std::string)rn + (std::string)"</RUN_NUMBER>\n";



	    (*newMsg) = (*newMsg) + (std::string)"\t<ERR_NBR_INSTANCE>" + (std::string)"NA" + (std::string)"</ERR_NBR_INSTANCE>\n";

	    (*newMsg) = (*newMsg) + (std::string)"\t<SYSID>" + (std::string)getSystemID() + (std::string)"</SYSID>\n";
	    (*newMsg) = (*newMsg) + (std::string)"\t<SUBSYSID>" + (std::string)getSubSystemID() + (std::string)"</SUBSYSID>\n";

	    (*newMsg) = (*newMsg) + (std::string)"\t<PARTITIONNAME>" + (std::string)getPartitionName() + (std::string)"</PARTITIONNAME>\n";

		#ifdef PARSE_XTRA_BUFFER
	    	(*newMsg) = (*newMsg) + (std::string)"\t<FECHARDID>" + findInExtraBuffer("<FECHARDID>", "</FECHARDID>",eBuff) + (std::string)"</FECHARDID>\n";
			removeFromExtraBuffer((std::string)"<FECHARDID>", (std::string)"</FECHARDID>");
		#else
			(*newMsg) = (*newMsg) + (std::string)"\t<FECHARDID>NA</FECHARDID>\n";
		#endif



		#ifdef PARSE_XTRA_BUFFER
		    (*newMsg) = (*newMsg) + (std::string)"\t<RING>" + findInExtraBuffer("<RING>", "</RING>",eBuff) + (std::string)"</RING>\n";
			removeFromExtraBuffer((std::string)"<RING>", (std::string)"</RING>");
		#else
			(*newMsg) = (*newMsg) + (std::string)"\t<RING>NA</RING>\n";
		#endif


		#ifdef PARSE_XTRA_BUFFER
		    (*newMsg) = (*newMsg) + (std::string)"\t<CCU>" + findInExtraBuffer("<CCU>", "</CCU>",eBuff) + (std::string)"</CCU>\n";
			removeFromExtraBuffer((std::string)"<CCU>", (std::string)"</CCU>");
		#else
			(*newMsg) = (*newMsg) + (std::string)"\t<CCU>NA</CCU>\n";
		#endif


		#ifdef PARSE_XTRA_BUFFER
		    (*newMsg) = (*newMsg) + (std::string)"\t<I2CCHANNEL>" + findInExtraBuffer("<I2CCHANNEL>", "</I2CCHANNEL>",eBuff) + (std::string)"</I2CCHANNEL>\n";
			removeFromExtraBuffer((std::string)"<I2CCHANNEL>", (std::string)"</I2CCHANNEL>");
		#else
			(*newMsg) = (*newMsg) + (std::string)"\t<I2CCHANNEL>NA</I2CCHANNEL>\n";
		#endif


		#ifdef PARSE_XTRA_BUFFER
		    (*newMsg) = (*newMsg) + (std::string)"\t<I2CADDRESS>" + findInExtraBuffer("<I2CADDRESS>", "</I2CADDRESS>",eBuff) + (std::string)"</I2CADDRESS>\n";
			removeFromExtraBuffer((std::string)"<I2CADDRESS>", (std::string)"</I2CADDRESS>");
		#else
			(*newMsg) = (*newMsg) + (std::string)"\t<I2CADDRESS>NA</I2CADDRESS>\n";
		#endif


		#ifdef PARSE_XTRA_BUFFER
		    (*newMsg) = (*newMsg) + (std::string)"\t<SOFTWARETAGVERSION>" + findInExtraBuffer("<SOFTWARETAGVERSION>", "</SOFTWARETAGVERSION>",eBuff) + (std::string)"</SOFTWARETAGVERSION>\n";
			removeFromExtraBuffer((std::string)"<SOFTWARETAGVERSION>", (std::string)"</SOFTWARETAGVERSION>");
		#else
			(*newMsg) = (*newMsg) + (std::string)"\t<SOFTWARETAGVERSION>NA</SOFTWARETAGVERSION>\n";
		#endif


		#ifdef PARSE_XTRA_BUFFER
		    (*newMsg) = (*newMsg) + (std::string)"\t<FEDID>" + findInExtraBuffer("<FEDID>", "</FEDID>",eBuff) + (std::string)"</FEDID>\n";
			removeFromExtraBuffer((std::string)"<FEDID>", (std::string)"</FEDID>");
		#else
			(*newMsg) = (*newMsg) + (std::string)"\t<FEDID>NA</FEDID>\n";
		#endif



		#ifdef PARSE_XTRA_BUFFER
		    (*newMsg) = (*newMsg) + (std::string)"\t<FEDCHANNEL>" + findInExtraBuffer("<FEDCHANNEL>", "</FEDCHANNEL>",eBuff) + (std::string)"</FEDCHANNEL>\n";
			removeFromExtraBuffer((std::string)"<FEDCHANNEL>", (std::string)"</FEDCHANNEL>");
		#else
			(*newMsg) = (*newMsg) + (std::string)"\t<FEDCHANNEL>NA</FEDCHANNEL>\n";
		#endif


		#ifdef PARSE_XTRA_BUFFER
		    (*newMsg) = (*newMsg) + (std::string)"\t<TRIGGER>" + findInExtraBuffer("<TRIGGER>", "</TRIGGER>",eBuff) + (std::string)"</TRIGGER>\n";
			removeFromExtraBuffer((std::string)"<TRIGGER>", (std::string)"</TRIGGER>");
		#else
			(*newMsg) = (*newMsg) + (std::string)"\t<TRIGGER>NA</TRIGGER>\n";
		#endif


		#ifdef PARSE_XTRA_BUFFER
		    (*newMsg) = (*newMsg) + (std::string)"\t<PSUID>" + findInExtraBuffer("<PSUID>", "</PSUID>",eBuff) + (std::string)"</PSUID>\n";
			removeFromExtraBuffer((std::string)"<PSUID>", (std::string)"</PSUID>");
		#else
			(*newMsg) = (*newMsg) + (std::string)"\t<PSUID>NA</PSUID>\n";
		#endif

/////////////

		#ifdef PARSE_XTRA_BUFFER
			std::string crate;
			if ( (findInExtraBuffer("<CRATE>", "</CRATE>",eBuff) == "NA") || (findInExtraBuffer("<CRATE>", "</CRATE>",eBuff) == "") )
			{
					crate = getCrateId();
			}
			else crate = findInExtraBuffer("<CRATE>", "</CRATE>",eBuff);
			(*newMsg) = (*newMsg) + (std::string)"\t<CRATE>" + crate + (std::string)"</CRATE>\n";
			removeFromExtraBuffer((std::string)"<CRATE>", (std::string)"</CRATE>");
		#else
			(*newMsg) = (*newMsg) + (std::string)"\t<CRATE>NA</CRATE>\n";
		#endif


		#ifdef PARSE_XTRA_BUFFER
			std::string slot;
			if ( (findInExtraBuffer("<SLOT>", "</SLOT>",eBuff) == "NA") || (findInExtraBuffer("<SLOT>", "</SLOT>",eBuff) == "") )
			{
					slot = getSlotId();
			}
			else slot = findInExtraBuffer("<SLOT>", "</SLOT>",eBuff);
		    (*newMsg) = (*newMsg) + (std::string)"\t<SLOT>" + slot + (std::string)"</SLOT>\n";
			removeFromExtraBuffer((std::string)"<SLOT>", (std::string)"</SLOT>");
		#else
			(*newMsg) = (*newMsg) + (std::string)"\t<SLOT>NA</SLOT>\n";
		#endif


		#ifdef PARSE_XTRA_BUFFER
		    (*newMsg) = (*newMsg) + (std::string)"\t<SOFTWARE>" + findInExtraBuffer("<SOFTWARE>", "</SOFTWARE>",eBuff) + (std::string)"</SOFTWARE>\n";
			removeFromExtraBuffer((std::string)"<SOFTWARE>", (std::string)"</SOFTWARE>");
		#else
			(*newMsg) = (*newMsg) + (std::string)"\t<SOFTWARE>NA</SOFTWARE>\n";
		#endif





		if (getSubClassPath() == "") setSubClassPath("NA");
	    (*newMsg) = (*newMsg) + (std::string)"\t<SUBCLASSPATH>" + (std::string)getSubClassPath() + (std::string)"</SUBCLASSPATH>\n";

		if (getFileName() == "") setFileName("NA");
	    (*newMsg) = (*newMsg) + (std::string)"\t<NESTEDFILENAME>" + (std::string)getFileName() + (std::string)"</NESTEDFILENAME>\n";

		if ((getLineNumberAsString() == "") || (getLineNumberAsString() == "0")) setLineNumberAsString("NA");
	    (*newMsg) = (*newMsg) + (std::string)"\t<NESTEDLINENUMBER>" + (std::string)getLineNumberAsString() + (std::string)"</NESTEDLINENUMBER>\n";



		stringToRework = getExtraBuffer();
		reworkBuffer1(&stringToRework);
	    (*newMsg) = (*newMsg) + (std::string)"\t<XTRA_BUFFER>" + stringToRework + (std::string)"</XTRA_BUFFER>\n";



	    (*newMsg) = (*newMsg) + (std::string)"</DATA>" + "\n";




//	    return newMsg;
//	    return (std::string)"";
	}




	void buildStringForPixelsLogFileOutput(std::string * newMsg, unsigned long logNumber)
	{
	
	    //std::string newMsg;
	    std::string eBuff = getExtraBuffer();
		std::string stringToRework="";


	    (*newMsg) = (std::string)"<DATA>" + "\n";
		
	    char ln[20];
	    snprintf(ln, 19, "%.6lu", logNumber);
	    (*newMsg) = (*newMsg) + (std::string)"\t<LOGNUMBER>" + (std::string)ln + (std::string)"</LOGNUMBER>\n";
	    (*newMsg) = (*newMsg) + (std::string)"\t<LEVEL>" + (std::string)getLevel() + (std::string)"</LEVEL>\n";
	    (*newMsg) = (*newMsg) + (std::string)"\t<ERR_CODE>" + (std::string)getErrorCode() + (std::string)"</ERR_CODE>\n";
//	    (*newMsg) = (*newMsg) + (std::string)"\t<ERR_MSG>" + reworkBuffer(getMessage()) + (std::string)"</ERR_MSG>\n";
		stringToRework = getMessage();
		reworkBuffer1(&stringToRework);
	    (*newMsg) = (*newMsg) + (std::string)"\t<ERR_MSG>" + stringToRework + (std::string)"</ERR_MSG>\n";

	    (*newMsg) = (*newMsg) + (std::string)"\t<ERR_TIMESTAMP>" + (std::string)getTimestampAsString() + (std::string)"</ERR_TIMESTAMP>\n";
	    (*newMsg) = (*newMsg) + (std::string)"\t<ERR_UID>" + (std::string)getLogger() + (std::string)"</ERR_UID>\n";
	    (*newMsg) = (*newMsg) + (std::string)"\t<SYSID>" + (std::string)getSystemID() + (std::string)"</SYSID>\n";
	    (*newMsg) = (*newMsg) + (std::string)"\t<SUBSYSID>" + (std::string)getSubSystemID() + (std::string)"</SUBSYSID>\n";
//	    (*newMsg) = (*newMsg) + (std::string)"\t<XTRA_BUFFER>" + reworkBuffer(getExtraBuffer()) + (std::string)"</XTRA_BUFFER>\n";
		stringToRework = getExtraBuffer();
		reworkBuffer1(&stringToRework);
	    (*newMsg) = (*newMsg) + (std::string)"\t<XTRA_BUFFER>" + stringToRework + (std::string)"</XTRA_BUFFER>\n";

	    (*newMsg) = (*newMsg) + (std::string)"</DATA>" + "\n";

	    //return newMsg;
	}









	void expandLoggerInfos(std::string& newMsg)
	{
		int loc = 0;
		int len;

		//Find machine name
		len = getLogger().rfind(".p:");
		std::string s1 = getLogger().substr(loc, len);
	    newMsg = newMsg + (std::string)"<MACHINE>" + s1 + (std::string)"</MACHINE>\n";

		//Find Port number
		loc = len+3;
		len = 0;
		while ( (getLogger()).c_str()[loc + len] != '.' )
		{
			len++;
		}
		std::string s2 = getLogger().substr(loc, len);
	    newMsg = newMsg + (std::string)"<PORT>" + s2 + (std::string)"</PORT>\n";


		//Find Process Name
		loc = loc + len + 1;
		len = 0;
		while ( (getLogger()).c_str()[loc + len] != '.' )
		{
			len++;
		}
		std::string s3 = getLogger().substr(loc, len);
	    newMsg = newMsg + (std::string)"<PROCESSNAME>" + s3 + (std::string)"</PROCESSNAME>\n";


		//Find Process instance
		loc = loc + len + 1;
		while ( (getLogger()).c_str()[loc] != '(' )
		{
			loc++;
		}
		loc++;
		len = 0;
		while ( (getLogger()).c_str()[loc + len] != ')' )
		{
			len++;
		}
		std::string s4 = getLogger().substr(loc, len);
	    newMsg = newMsg + (std::string)"<PROCESSINSTANCE>" + s4 + (std::string)"</PROCESSINSTANCE>\n";
	}




	void expandLoggerInfos(std::string* processName,
							std::string* processInstance,
							std::string* machine,
							std::string* port)
	{
		int loc = 0;
		int len;
		std::string lcl_logger = getLogger();
		
		//Find machine name
		len = lcl_logger.rfind(".p:");
		*machine = lcl_logger.substr(loc, len);

		//Find Port number
		loc = len+3;
		len = 0;
		while ( lcl_logger.c_str()[loc + len] != '.' )
		{
			len++;
		}
		*port = lcl_logger.substr(loc, len);


		//Find Process Name
		loc = loc + len + 1;
		len = 0;
		while ( lcl_logger.c_str()[loc + len] != '.' )
		{
			len++;
		}
		*processName = getLogger().substr(loc, len);


		//Find Process instance
		loc = loc + len + 1;
		while ( lcl_logger.c_str()[loc] != '(' )
		{
			loc++;
		}
		loc++;
		len = 0;
		while ( lcl_logger.c_str()[loc + len] != ')' )
		{
			len++;
		}
		*processInstance = lcl_logger.substr(loc, len);

	}


	std::string expandTimeStamp(void)
	{
		std::stringstream tstamp;
		char secs[50];
		char usec[7];

		//Split Secs/Usecs fields
		tstamp << getTimeStamp();
		strcpy(secs, tstamp.str().c_str());
		usec[0] = secs[strlen(secs) - 3];
		usec[1] = secs[strlen(secs) - 2];
		usec[2] = secs[strlen(secs) - 1];
		usec[3] = '0';
		usec[4] = '0';
		usec[5] = '0';
		usec[6] = '\0';
		secs[strlen(secs) - 3] = '\0';

		struct tm * timeSplittedPtr;
		time_t modTS = (time_t)atoll(secs);
		timeSplittedPtr = localtime(&modTS);

		char logTimeStamp[100];
		snprintf(logTimeStamp, 22, "%.4d-%.2d-%.2d %.2d:%.2d:%.2d.",
				(timeSplittedPtr->tm_year + 1900),
				timeSplittedPtr->tm_mon,
				timeSplittedPtr->tm_mday,
				timeSplittedPtr->tm_hour,
				timeSplittedPtr->tm_min,
				timeSplittedPtr->tm_sec);
		strcat(logTimeStamp, usec);
		return (std::string)logTimeStamp;
	}



	void WriteXmlOutputFilePreHeader(	FILE * fileOut,
								std::string xmlVersion,
								std::string xmlEncoding,
								std::string xmlDoctypeRoot)
	{
		fprintf(fileOut,"<?xml version=%s encoding=%s?>\n", xmlVersion.c_str(), xmlEncoding.c_str());
		fprintf(fileOut,"<!DOCTYPE root %s>\n", xmlDoctypeRoot.c_str());
		fprintf(fileOut,"<ROOT>\n");
	}



	FILE * OpenXmlOutputFile(	std::string xmlLogFileName,
								std::string xmlAccessMode)
	{
		FILE * fileOut;
		fileOut = fopen(xmlLogFileName.c_str(), xmlAccessMode.c_str());
		return fileOut;
	}



	void WriteXmlOutputFileHeader(FILE * fileOut,
									std::string extTableName,
									std::string headerName,
									std::string runType,
									std::string runNumber,
									std::string runStart,
									std::string runUser,
									std::string runLocation,
									std::string runComment)
	{
		fprintf(fileOut,"\t<HEADER>\n");

		//Write TYPE block
		fprintf(fileOut,"\t\t<TYPE>\n");
		fprintf(fileOut,"\t\t\t<EXTENSION_TABLE_NAME>%s</EXTENSION_TABLE_NAME>\n", extTableName.c_str());
		fprintf(fileOut,"\t\t\t<NAME>%s</NAME>\n", headerName.c_str());
		fprintf(fileOut,"\t\t</TYPE>\n");

		//Write RUN block
		fprintf(fileOut,"\t\t<RUN>\n");
		fprintf(fileOut,"\t\t\t<RUN_TYPE>%s</RUN_TYPE>\n", runType.c_str());
		fprintf(fileOut,"\t\t\t<RUN_NUMBER>%s</RUN_NUMBER>\n", runNumber.c_str());
		fprintf(fileOut,"\t\t\t<RUN_BEGIN_TIMESTAMP>%s</RUN_BEGIN_TIMESTAMP>\n", runStart.c_str());
		fprintf(fileOut,"\t\t\t<INITIATED_BY_USER>%s</INITIATED_BY_USER>\n", runUser.c_str());
		fprintf(fileOut,"\t\t\t<LOCATION>%s</LOCATION>\n", runLocation.c_str());
		fprintf(fileOut,"\t\t\t<COMMENT_DESCRIPTION>%s</COMMENT_DESCRIPTION>\n", runComment.c_str());
		fprintf(fileOut,"\t\t</RUN>\n");

		fprintf(fileOut,"\t</HEADER>\n");

	}

	void WriteXmlOutputFileEndHeader(	FILE * fileOut)
	{
		fprintf(fileOut,"</ROOT>\n");
	}



	void WriteXmlOutputFileDataSet( FILE * fileOut,
									std::string commentDescription,
									std::string dataFileName,
									std::string errxml_version,
									std::string partSerialNumber,
									std::string criticityLevel,
									std::string logNumber,
									std::string trkFecName,
									std::string errCode,
									std::string errMsg,
									std::string errTimeStamp,
									std::string processName,
									std::string processInstance,
									std::string machine,
									std::string port)
									/*
									//Begin Extra-Buffer
									std::string vmeAddr,
									std::string crateSlot,
									std::string fecSlot,
									std::string ringSlot,
									std::string fecStatReg0)
									*/
	{
		fprintf(fileOut,"\t<DATA_SET>\n");

		fprintf(fileOut, "\t\t<COMMENT_DESCRIPTION>%s</COMMENT_DESCRIPTION>\n", commentDescription.c_str());
		fprintf(fileOut, "\t\t<DATA_FILE_NAME>%s</DATA_FILE_NAME>\n", dataFileName.c_str());
		fprintf(fileOut, "\t\t<VERSION>%s</VERSION>\n", errxml_version.c_str());

		fprintf(fileOut, "\t\t<PART>\n");
		fprintf(fileOut, "\t\t\t<SERIAL_NUMBER>%s</SERIAL_NUMBER>\n", partSerialNumber.c_str());
		fprintf(fileOut, "\t\t</PART>\n");

		fprintf(fileOut,"\t\t<DATA>\n");

		fprintf(fileOut,"\t\t\t<LEVEL>%s</LEVEL>\n", criticityLevel.c_str());
		fprintf(fileOut,"\t\t\t<LOG_NUM>%s</LOG_NUM>\n", logNumber.c_str());
		fprintf(fileOut,"\t\t\t<TRKFEC_NAME>%s</TRKFEC_NAME>\n", trkFecName.c_str());
		fprintf(fileOut,"\t\t\t<ERR_CODE>%s</ERR_CODE>\n", errCode.c_str());
		fprintf(fileOut,"\t\t\t<ERR_MSG>%s</ERR_MSG>\n", errMsg.c_str());
		fprintf(fileOut,"\t\t\t<ERR_TIMESTAMP>%s</ERR_TIMESTAMP>\n", errTimeStamp.c_str());
		fprintf(fileOut,"\t\t\t<PROCESSNAME>%s</PROCESSNAME>\n", processName.c_str());
		fprintf(fileOut,"\t\t\t<PROCESSINSTANCE>%s</PROCESSINSTANCE>\n", processInstance.c_str());
		fprintf(fileOut,"\t\t\t<MACHINE>%s</MACHINE>\n", machine.c_str());
		fprintf(fileOut,"\t\t\t<PORT>%s</PORT>\n", port.c_str());
		fprintf(fileOut,"\t\t\t<SYSTEM>%s</SYSTEM>\n", getSystemID().c_str());
		fprintf(fileOut,"\t\t\t<SUBSYSTEM>%s</SUBSYSTEM>\n", getSubSystemID().c_str());
		//Start of Extra buffer writeout



//		setExtraBuffer("<FECHARDID>3020140B9CCEC6</FECHARDID><FEC>4</FEC><RING>6</RING><CCU>63</CCU><INFORMATION=FEC status register 0>17480</INFORMATION>");
		/*
		fprintf(fileOut,"\t\t\t<VME_ADDR>%s</VME_ADDR>\n", vmeAddr.c_str());
		fprintf(fileOut,"\t\t\t<CRATE>%s</CRATE>\n", crateSlot.c_str());
		fprintf(fileOut,"\t\t\t<SLOT>%s</SLOT>\n", fecSlot.c_str());
		fprintf(fileOut,"\t\t\t<RING>%s</RING>\n", ringSlot.c_str());
		fprintf(fileOut,"\t\t\t<FEC_STATREG0>%s</FEC_STATREG0>\n", fecStatReg0.c_str());
		*/
		
		std::string eBuff;
		std::string newEBuff;
		int loc=0;
		int len=0;
		int overallLength=0;

		eBuff = getExtraBuffer();
		//std::cout << "got extra buffer : " << eBuff << std::endl;

		newEBuff = "";
		overallLength = eBuff.length();
		//std::cout << "extra buffer length is : " << overallLength << std::endl;
		

		do
		{
			//std::cout << "Starting to work at index : " << loc << std::endl;
			//Find 2 occurences as > ... >
			while ( (eBuff.c_str()[loc + len] != '>') && ((loc + len) < overallLength) ) len++;
			//std::cout << "first target at pos : " << len << std::endl;
			len++;
			while ( (eBuff.c_str()[loc + len] != '>') && ((loc + len) < overallLength) ) len++;
			//std::cout << "second target at pos : " << len << std::endl;
			len++;
			newEBuff = newEBuff + (std::string)"\t\t\t" + eBuff.substr(loc, len) + (std::string)"\n";
			//std::cout << "reprocessed extra buffer : " << newEBuff << std::endl;
			loc = loc + len;
			len = 0;
		} while (loc < overallLength);



		//std::cout << "Reprocessed extra buffer : " << std::endl;
		if (getExtraBuffer() != (std::string)"NOP")
		{
			fprintf(fileOut,"%s", newEBuff.c_str());
			//std::cout << newEBuff << std::endl;
		}
		//else std::cout << "empty field" << std::endl;

		fprintf(fileOut,"\t\t</DATA>\n");
		fprintf(fileOut,"\t</DATA_SET>\n");

		//fflush(fileOut);
	}








	void CloseXmlOutputFile(FILE * fileOut)
	{
//		fprintf(fileOut,"</ROOT>\n");
		if (fileOut != NULL) fclose(fileOut);
	}








	std::string buildStringForLog4jLogFileOutput(unsigned long logNumber)
	{
	
	    std::string newMsg;

	    newMsg = newMsg + "<log4j:event logger=\""+getLogger()+"\" ";
	    newMsg = newMsg + "timestamp=\"";

	    std::ostringstream number_str;
	    /* Time format required is milliseconds since 1/1/1970. Take seconds and append the milliseconds */
	    number_str << getTimeStamp();
	    newMsg = newMsg + number_str.str();

	    newMsg = newMsg + "\" level=\"";
	    if (getLevel() != DIAGUSERINFO) {newMsg = newMsg + getLevel();} else newMsg = newMsg + DIAGINFO;
	    newMsg = newMsg + "\" thread=\"\">";
	
	    newMsg = newMsg + "<log4j:message> Message="+ getMessage();

		newMsg = newMsg + " ErrorCode="+ getErrorCode();


	    newMsg = newMsg + "</log4j:message>";

	    newMsg = newMsg + "<log4j:NDC></log4j:NDC>";
	    newMsg = newMsg + "<log4j:throwable></log4j:throwable>";
	    newMsg = newMsg + "<log4j:locationInfo class=\"\" method=\"\" file=\"\" line=\"\"";
	    newMsg = newMsg + "/></log4j:event>\n";

/*

	    newMsg = (std::string)"<DATA>" + "\n";
	    newMsg = newMsg + (std::string)"\t<LEVEL>" + (std::string)getLevel() + (std::string)"</LEVEL>\n";
	    newMsg = newMsg + (std::string)"\t<ERR_CODE>" + (std::string)getErrorCode() + (std::string)"</ERR_CODE>\n";
	    newMsg = newMsg + (std::string)"\t<ERR_MSG>" + (std::string)getMessage() + (std::string)"</ERR_MSG>\n";
	    newMsg = newMsg + (std::string)"\t<ERR_TIMESTAMP>" + (std::string)getTimestampAsString() + (std::string)"</ERR_TIMESTAMP>\n";

*/

	    return newMsg;
	}











};

/* End Block - avoid multiple inclusions of this file */
#endif
