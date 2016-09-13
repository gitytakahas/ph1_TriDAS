/*

   FileName : 		SOAPDiagAppender.cc

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



#include <soapdiagappender/SOAPDiagAppender.h>





SOAPDiagAppender::SOAPDiagAppender(const log4cplus::tstring& host,
                                    int port,
                                    int lid,
                                    const log4cplus::tstring& command,
                                    const log4cplus::tstring& serverName)
                                    : host_(host),
                                    port_(port),
                                    lid_(lid),
                                    command_(command),
                                    serverName_(serverName)
{
    closed = true;
    this->openConnection();
}



SOAPDiagAppender::~SOAPDiagAppender()
{
    /* destructor Impl calls close() */
    destructorImpl();
}


void SOAPDiagAppender::close()
{
    closed = true;
    socket.close(); 
}




void SOAPDiagAppender::openConnection()
{
    if(!socket.isOpen()) 
    {
        socket = log4cplus::helpers::Socket(host_, port_);
        closed = false;
    }
}


void SOAPDiagAppender::append(const log4cplus::spi::InternalLoggingEvent& event)
{
    /* check that socket is available */
    if(!socket.isOpen()) 
    {
        openConnection();
        if(!socket.isOpen()) 
        {
            /* still not open after 2nd try, give up */
            std::cerr << "SOAPDiagAppender::append() - Cannot connect to server" << std::endl;
            return;
        }
    }
    int soapBufferSize=0;
    log4cplus::helpers::SocketBuffer buffer = convertToSOAPBuffer(event, host_, port_, lid_, command_, &soapBufferSize);
    log4cplus::helpers::SocketBuffer msgBuffer(soapBufferSize);
    msgBuffer.appendBuffer(buffer);
    socket.write(msgBuffer);
}



log4cplus::helpers::SocketBuffer SOAPDiagAppender::convertToSOAPBuffer(const log4cplus::spi::InternalLoggingEvent& event,
                                                                        log4cplus::tstring& host,
                                                                        int port,
                                                                        int lid,
                                                                        log4cplus::tstring& command,
                                                                        int * soapBufferSize)
{

    /* Forge BODY */
	std::string strSoap="<soap-env:Envelope soap-env:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:soap-env=\"http://schemas.xmlsoap.org/soap/envelope/\"><soap-env:Header/><soap-env:Body><xdaq:";
    /* XDAQ process part */
    strSoap += command;
    strSoap += " logger=\""+event.getLoggerName()+"\"";
    strSoap += " timestamp=\"";
    /* timestamp is in milliseconds since 1/1/1970 */
    unsigned long resu1, resu2;
    resu1 = event.getTimestamp().sec();
    resu2 = (event.getTimestamp().usec())/1000;
    char s1[100];
    sprintf(s1,"%lu",resu1);
    char s2[100];
    sprintf(s2,"%lu",resu2);
    char s3[100];
    strcpy(s3,s1);
    strcat(s3,s2);
    strSoap += s3;
    strSoap += "\"";
    strSoap += " level=\"";
    strSoap += log4cplus::getLogLevelManager().toString(event.getLogLevel())+"\"";;
    strSoap += " thread=\""+event.getThread()+"\"";
    strSoap += " message=\""+event.getMessage()+"\"";
    strSoap += " ndc=\""+event.getNDC()+"\"";
    strSoap += " file=\""+event.getFile()+"\"";
    strSoap += " line=\"";
    std::ostringstream number3_str;
    number3_str << event.getLine();
    strSoap += number3_str.str();
    strSoap += "\"";
    strSoap += " xmlns:xdaq=\"urn:xdaq-soap:3.0\"/>";
    /* Terminate BODY */
    strSoap += "</soap-env:Body></soap-env:Envelope>";
    int soapBodyLen=strSoap.length();
    /* Forge HEADER */
    std::string header="";
    header = "POST / HTTP/1.1\r\nHost: ";
    header += host;
    header += ":";
    char portStr[30];
    std::sprintf (portStr, "%d", port);
    header += portStr;
    header += "\r\nConnection: keep-alive\r\nContent-type: text/xml; charset=utf-8\r\nContent-length: ";
    char lenStr[10];
    std::sprintf (lenStr, "%d", soapBodyLen);
    header += lenStr;
    header += "\r\n";
    header += "SOAPAction: ";
    header += "urn:xdaq-application:lid=";
    char lidStr[10];
    std::sprintf (lidStr, "%d", lid);
    header += lidStr;
    header += "\r\n";
    header += "Content-Description: XDAQ SOAP.\r\n\r\n";
    #ifdef DEBUG_SOAP_DIAG_APPENDER
        std::cout << "\t\tconvertToSOAPBuffer::HEADER is : " << std::endl << header << std::endl; 
        std::cout << "\t\tconvertToSOAPBuffer::SOAP BODY is : " << std::endl << strSoap << std::endl;
    #endif
    int soapHeaderLen=header.length();
    log4cplus::helpers::SocketBuffer buffer(soapBodyLen + soapHeaderLen + sizeof(unsigned int));
    *soapBufferSize = soapBodyLen + soapHeaderLen + sizeof(unsigned int);
    /* Fill the buffer with HEADER & BODY */
    buffer.appendString(header);
    buffer.appendString(strSoap);
return buffer;
}

