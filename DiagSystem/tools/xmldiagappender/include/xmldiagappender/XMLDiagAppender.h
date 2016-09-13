/*
   FileName : 		XMLDiagAppender.h

   Content : 		XMLDiagAppender module

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

#ifndef _XMLDiagAppender_h_
#define _XMLDiagAppender_h_

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

#include "DiagCompileOptions.h"
#include <diagbag/helpers/clog.h>

#ifndef UNICODE
    #define LOG4CPLUS_XML_MAX_MESSAGE_SIZE (8*1024)
#else
    #define LOG4CPLUS_XML_MAX_MESSAGE_SIZE (2*8*1024)
#endif

class XMLDiagAppender/* : public log4cplus::Appender */
{
    public:

bool closed;
        /* Ctors */
        XMLDiagAppender(const log4cplus::tstring& host, 
                        int port,
                        const log4cplus::tstring& serverName);

        XMLDiagAppender(const log4cplus::helpers::Properties properties);

        /* Dtor */
        ~XMLDiagAppender();
	
        /* Public commit thread specific data */

        pthread_attr_t commit_thread_attr_;
        pthread_t commit_thread_id_;


        /* Methods */
/*
        virtual void close();
        virtual void appendToSocket(const CLog a);
*/
        void close();
        void appendToSocket(const CLog a);

        log4cplus::helpers::SocketBuffer convertToXMLBuffer(CLog c, const log4cplus::tstring& serverName);

    protected:
        /* Methods */
        void openConnection();
/*
        virtual void append(const log4cplus::spi::InternalLoggingEvent& event);
*/
        void append(const log4cplus::spi::InternalLoggingEvent& event);
        /* Data */
        log4cplus::tstring host_;
        int port_;
        log4cplus::tstring serverName_;
        log4cplus::helpers::Socket socket;

    private:
        // Disallow copying of instances of this class
        XMLDiagAppender(const XMLDiagAppender&);
        XMLDiagAppender& operator=(const XMLDiagAppender&);

};	
#endif // _XMLDiagAppender_h_

