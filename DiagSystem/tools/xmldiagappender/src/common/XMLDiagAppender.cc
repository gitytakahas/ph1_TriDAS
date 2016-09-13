/*
   FileName : 		XMLDiagAppender.cc

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

#include <xmldiagappender/XMLDiagAppender.h>

XMLDiagAppender::XMLDiagAppender(const log4cplus::tstring& host, 
                                int port,
                                const log4cplus::tstring& serverName)
                                : host_(host),
                                port_(port),
                                serverName_(serverName)
{
    closed = true;

//Commented due to an init bug occuring at P5 - no connection can be opened during init stage ?!
//    this->openConnection();

}

/*

XMLDiagAppender::XMLDiagAppender(const log4cplus::helpers::Properties properties)
                                : log4cplus::Appender(properties),
                                port_(1946)
{
    host_ = properties.getProperty( LOG4CPLUS_TEXT("host") );
    if(properties.exists( LOG4CPLUS_TEXT("port") )) 
    {
        log4cplus::tstring tmp = properties.getProperty( LOG4CPLUS_TEXT("port") );
        port_ = atoi(LOG4CPLUS_TSTRING_TO_STRING(tmp).c_str());
    }
    serverName_ = properties.getProperty( LOG4CPLUS_TEXT("ServerName") );
    this->openConnection();
}

*/


XMLDiagAppender::~XMLDiagAppender()
{
//    destructorImpl();
}




void XMLDiagAppender::close()
{
    closed = true;
    socket.close(); 
}




void XMLDiagAppender::openConnection()
{
    if(!socket.isOpen()) 
    {
        socket = log4cplus::helpers::Socket(host_, port_);
        closed = false;

    }		
}



void XMLDiagAppender::appendToSocket(const CLog c)
{
    if(!socket.isOpen()) 
    {
        openConnection();
        if(!socket.isOpen()) 
        {
            /* still not open after 2nd try, give up */
            std::cout << "XMLDiagAppender::append() - Cannot connect to server (are you trying to connect to a Chainsaw appender?)" << std::endl;
            return;
        }
    }
    log4cplus::helpers::SocketBuffer buffer = convertToXMLBuffer(c, serverName_);
    log4cplus::helpers::SocketBuffer msgBuffer(LOG4CPLUS_XML_MAX_MESSAGE_SIZE);
    msgBuffer.appendBuffer(buffer);
    socket.write(msgBuffer);
}






void XMLDiagAppender::append(const log4cplus::spi::InternalLoggingEvent& event)
{
}




log4cplus::helpers::SocketBuffer XMLDiagAppender::convertToXMLBuffer(CLog c, const log4cplus::tstring& serverName)
{
    log4cplus::helpers::SocketBuffer buffer(LOG4CPLUS_XML_MAX_MESSAGE_SIZE - sizeof(unsigned int));
    buffer.appendString("<log4j:event logger=\""+c.getLogger()+"\" ");
    buffer.appendString("timestamp=\"");

    std::ostringstream number_str;
    /* Time format required is milliseconds since 1/1/1970. Take seconds and append the milliseconds */
    number_str << c.getTimeStamp();
    buffer.appendString(number_str.str());

    buffer.appendString("\" level=\"");
    if (c.getLevel() != DIAGUSERINFO) {buffer.appendString(c.getLevel());} else buffer.appendString(DIAGINFO);
/*
    buffer.appendString("\" thread=\""+c.getThread()+"\">");
*/
//    buffer.appendString("\" thread=\""+c.getThread()+"\">");
    buffer.appendString("\" thread=\"0\">");

    buffer.appendString("<log4j:message> Message="+c.getMessage());
/*    buffer.appendString(" Text="+c.getText());*/
    buffer.appendString(" ErrorCode="+c.getErrorCode());
/*    buffer.appendString(" FaultState="+c.getFaultState());
    buffer.appendString(" SystemID="+c.getSystemID());
    buffer.appendString(" SubSystemID="+c.getSubSystemID());
    buffer.appendString(" ClassName="+c.getClassName());
    buffer.appendString(" Instance=");
    std::ostringstream number_str1;
    number_str1 << c.getInstance();
    buffer.appendString(number_str1.str());
*/
/*
    buffer.appendString(" Lid="); 
    std::ostringstream number_str2;
    number_str2 << c.getLid();
    buffer.appendString(number_str2.str());
    buffer.appendString(" ErrorDispatcherTimestamp=");
    std::ostringstream number_str3;
    number_str3 << c.getEdTimeStamp();
    buffer.appendString(number_str3.str());
    buffer.appendString(" Lvl1Name="+c.getLvl1Name());
    buffer.appendString(" Lvl1Instance="+c.getLvl1Instance());
    buffer.appendString(" RouteToErrorManager="+c.getRouteToEm());
    buffer.appendString(" DatabaseLogLevel="+c.getDbLogLevel());
    buffer.appendString(" ChainsawLogLevel="+c.getCsLogLevel());
    buffer.appendString(" ConsoleLogLevel="+c.getCcLogLevel());
    buffer.appendString(" ForceDatabaseLogging="+c.getForcedDbLog());
    buffer.appendString(" ForceChainsawLogging="+c.getForcedCsLog());
    buffer.appendString(" ForceConsoleLogging="+c.getForcedCcLog());
    buffer.appendString(" GlobalLogLevel="+c.getGLogLevel());
    buffer.appendString(" EdReconfStatus=");
    if (c.getEdReconfStatus()==true) buffer.appendString("true"); else buffer.appendString("false");
*/
    buffer.appendString("</log4j:message>");
//    buffer.appendString("<log4j:NDC>"+ serverName + " - " + c.getNDC() +"</log4j:NDC>");
    buffer.appendString("<log4j:NDC>" + c.getLogger() + "</log4j:NDC>");
    buffer.appendString("<log4j:throwable></log4j:throwable>");
    buffer.appendString("<log4j:locationInfo class=\"MyClass\" method=\"MyMethod\" file=\"MyFile\" line=\"0\"");
    buffer.appendString("/></log4j:event>");
return buffer;
}
