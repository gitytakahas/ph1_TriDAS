/*
   FileName : 		DiagBag.h

   Content : 		DiagBag module

   Used in : 		Tracker Diagnostic System

   Programmer : 	Laurent GROSS

   Version : 		DiagSystem 2.1

   Date of last modification : 20/06/2006

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

#ifndef _DiagBagXdaq_h_
#define _DiagBagXdaq_h_

/* Include Root Class files */
#include <diagbag/helpers/clog.h>
#include <diagbag/DiagBag.h>

#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include <soapdiagappender/SOAPDiagAppender.h>
#include <xmldiagappender/XMLDiagAppender.h>
#include <diagbag/helpers/DiagBagXdaqMacros.h>


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

class DiagBagXdaq : public DiagBag
{
    public:
    
        /* Ctors */
        DiagBagXdaq(std::string l1name, log4cplus::Logger & logger, std::string classname, unsigned long instance, unsigned long lid);
        DiagBagXdaq(std::string l1name);

        /* Dtor
        ~DiagBagXdaq(); */

        /* Methods */
        pthread_attr_t commit_thread_attr_;
        pthread_t commit_thread_id_;

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
                                    std::string gloglevel);

        std::string forgeErrorString(std::string msg,
                                    std::string text,
                                    int errorcode,
                                    std::string faultstate,
                                    std::string systemid,
                                    std::string subsystemid,
                                    std::string classname,
                                    unsigned long instance,
                                    unsigned long lid);

        std::string forgeErrorString(std::string msg,
                                    std::string classname,
                                    unsigned long instance,
                                    unsigned long lid);

        int configureCallback(  xgi::Input * in,
                                xgi::Output * out,
                                std::string classname,
                                std::string URL,
                                std::string URN);

        int getFormValues(  xgi::Input * in,
                            xgi::Output * out);


        bool checkUseDiag();
        bool setUseDiagOnName(bool lcl_use_diag);
        bool setUseDiagOnLid(unsigned long proc_lid);
        bool setUseDiagOnUrl(std::string l_url);

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
        long getProcessLogLevel();

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
        void setUseDiag(bool diagStatus);
        void setUseEd(bool diagStatus);
        bool getUseDiag(void);
        bool getUseEd(void);

        void zeroLvlLog(std::string msg,
                        std::string lvl,
                        log4cplus::Logger & logger);

        void minLvlLog(std::string msg,
                        std::string lvl,
                        log4cplus::Logger & logger,
                        std::string classname,
                        unsigned long instance,
                        unsigned long lid);

        void regularLvlLog(std::string msg,
                            std::string lvl,
                            log4cplus::Logger & logger,
                            std::string classname,
                            unsigned long instance,
                            unsigned long lid,
                            std::string text,
                            int errorcode,
                            std::string faultstate,
                            std::string systemid,
                            std::string subsystemid);

        void reportError(std::string msg,
                        std::string lvl,
                        std::string text,
                        int errorcode,
                        std::string faultstate,
                        std::string systemid,
                        std::string subsystemid);

        void reportError(std::string msg,
                        std::string lvl);


    private: 
        bool useDiag_;
        std::string diagName_;
        std::string diagInstance_;
        std::string diagIP_;
        std::string diagPort_;
        std::string diagLID_;

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
        std::string diagLIDStatus_;

        std::string useLvl1Status_;
        std::string lvl1NameStatus_;
        std::string lvl1InstanceStatus_;
        std::string lvl1IPStatus_;
        std::string lvl1PortStatus_;
        std::string lvl1LIDStatus_;

        bool edUsable_;
        bool emUsable_;
        bool forcedDbLogs_;
        bool forcedCsLogs_;
        bool forcedCcLogs_;
        std::string dbLogLevel_;
        std::string csLogLevel_;
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
};


#endif // _DiagBagXdaq_h_

