/*
   FileName : 		SOAPDiagAppender.h

   Content : 		SOAPDiagAppender module

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

#ifndef _SOAPDiagAppender_h_
#define _SOAPDiagAppender_h_

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
#include <stdio.h>
#include <unistd.h>
#include <string.h> /* memset() */
#include <sys/time.h> /* select() */ 
#include <iostream>
#include <sstream>



#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/Method.h"
#include "xoap/SOAPElement.h"


#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"
#include "xdaq/WebApplication.h"
#include "xdaq/exception/ApplicationNotFound.h"
#include "xdaq/NamespaceURI.h"


#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include "xdata/String.h"
#include "xdata/UnsignedLong.h"
#include "xdata/Integer.h"

#include "DiagCompileOptions.h"
#include "diagbag/helpers/clog.h"

class SOAPDiagAppender : public log4cplus::Appender 
{
    public:
        /* Ctors */
        SOAPDiagAppender(const log4cplus::tstring& host,
                        int port,
                        int lid,
                        const log4cplus::tstring& command,
                        const log4cplus::tstring& serverName);

        /* Dtor */
        ~SOAPDiagAppender();

        /* Public commit thread specific data */
        pthread_attr_t commit_thread_attr_;
        pthread_t commit_thread_id_;

        /* Methods */
        virtual void close();	

        log4cplus::helpers::SocketBuffer convertToSOAPBuffer(const log4cplus::spi::InternalLoggingEvent& event,
                                                            log4cplus::tstring& host,
                                                            int port,
                                                            int lid,
                                                            log4cplus::tstring& command,
                                                            int * soapBufferSize);


    protected:
        void openConnection();
        virtual void append(const log4cplus::spi::InternalLoggingEvent& event);

        /* Data */
        log4cplus::tstring host_;
        int port_;
        int lid_;
        log4cplus::tstring command_;
        log4cplus::tstring serverName_;
        log4cplus::helpers::Socket socket;

    private:
        /* Disallow copying of instances of this class */
        SOAPDiagAppender(const SOAPDiagAppender&);
        SOAPDiagAppender& operator=(const SOAPDiagAppender&);	

};
#endif // _SOAPDiagAppender_h_

