/*
i   FileName : 		SentinelSender.h

   Content : 		SentinelSender module

   Used in : 		Tracker Diagnostic System

   Programmer : 	Laurent GROSS

   Version : 		DiagSystem 4.0.0

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

#ifndef _SentinelSender_h_
#define _SentinelSender_h_

#ifndef _SOAPStateMachine_h_
#define _SOAPStateMachine_h_

#include <sys/time.h>

#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"
#include "xdaq/NamespaceURI.h"

#include "xdaq/WebApplication.h"
#include "xdaq/exception/ApplicationNotFound.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPElement.h"
#include "xoap/Method.h"
#include "xoap/domutils.h"

#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include "xdata/String.h"
#include "xdata/UnsignedLong.h"
#include "xdata/Integer.h"


#include "xcept/tools.h"
#include "toolbox/utils.h"

#include "SentinelSenderV.h"

#include "diagbag/TkExcept.h"



class SentinelSender: public xdaq::WebApplication
{

	public:
        XDAQ_INSTANTIATOR();


        SentinelSender(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception);


        void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);


		void sendLogOne(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

		void sendLogTwo(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

		void sendNewsOne(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);

		void sendNewsTwo(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception);


};

#endif
#endif









