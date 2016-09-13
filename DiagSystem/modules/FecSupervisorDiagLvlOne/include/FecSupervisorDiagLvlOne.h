/*
   FileName : 		FecSupervisorDiagLvlOne.h

   Content : 		FecSupervisorDiagLvlOne module

   Used in : 		Tracker Diagnostic System

   Programmer : 	Laurent GROSS

   Version : 		TDS 1.3

   Date of last modification : 03/03/2006

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

#ifndef _FecSupervisorDiagLvlOne_h_
#define _FecSupervisorDiagLvlOne_h_


#ifndef _SOAPStateMachine_h_
#define _SOAPStateMachine_h_

#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"

#include "xdaq/NamespaceURI.h"

#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPElement.h"
#include "xoap/Method.h"

#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"

#include "xdata/String.h"
#include "xdata/UnsignedLong.h"
#include "xdata/Integer.h"


#include <diagbag/DiagBagWizard.h>


#include "FecSupervisorDiagLvlOneV.h"


class FecSupervisorDiagLvlOne: public xdaq::Application  
{
	public:

	bool diagUsable_;
	bool useDiag_;
	std::string diagName_;
	std::string diagInstance_;
	std::string diagIP_;
	std::string diagPort_;
	std::string diagLID_;
	std::string oldDiagIP_;
	std::string oldDiagPort_;
	std::string oldDiagName_;
	std::string oldDiagInstance_;
	std::string oldDiagLID_;
	bool socketSoapExists_;
	std::string diagNameStatus_;
	std::string diagInstanceStatus_;
	std::string diagIPStatus_;
	std::string diagPortStatus_;
	std::string diagLIDStatus_;
	std::string gLogLevel_;
	bool reconfUsable_;
	bool useReconf_;
	std::string reconfName_;
	std::string reconfInstance_;
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


	//DIAGREQUESTED
	DiagBagWizard * diagService_;

	XDAQ_INSTANTIATOR();
	
	FecSupervisorDiagLvlOne(xdaq::ApplicationStub * s): xdaq::Application(s) 
	{	
		//Give funny and useless informations at load time
		std::stringstream mmesg;
		mmesg << "Process version " << FECSUPERVISORDIAGLVLONE_PACKAGE_VERSION << " loaded";
		this->getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);
		LOG4CPLUS_INFO(this->getApplicationLogger(), mmesg.str());

		diagName_= "ErrorDispatcher";
		useDiag_=false;
		diagInstance_="0";
		diagIP_="127.0.0.1";
		diagPort_="1972";
		diagLID_="30";
		diagUsable_=false;

		oldDiagIP_="";
		oldDiagPort_="";
		oldDiagName_="";
		oldDiagInstance_="";
		oldDiagLID_="";

		diagNameStatus_ = "unchecked";
		diagInstanceStatus_ =  "unchecked";
		diagIPStatus_ =  "unchecked";
		diagPortStatus_ =  "unchecked";
		diagLIDStatus_ =  "unchecked";

		socketSoapExists_ = false;
		
		reconfName_= "FecSupervisor";
		useReconf_=false;
		reconfInstance_="0";
		reconfIP_="127.0.0.1";
		reconfPort_="1972";
		reconfLID_="31";
		reconfUsable_=false;

		oldReconfIP_="";
		oldReconfPort_="";
		oldReconfName_="";
		oldReconfInstance_="";
		oldReconfLID_="";
		
		reconfNameStatus_ = "unchecked";
		reconfInstanceStatus_ =  "unchecked";
		reconfIPStatus_ =  "unchecked";
		reconfPortStatus_ =  "unchecked";
		reconfLIDStatus_ =  "unchecked";
	
		//DIAGREQUESTED
		 diagService_ = new DiagBagWizard(
 			 (getApplicationDescriptor()->getClassName() + "NONE") ,
 			 this->getApplicationLogger(),
			 getApplicationDescriptor()->getClassName(),
			 getApplicationDescriptor()->getInstance(),
			 getApplicationDescriptor()->getLocalId(),
			 (xdaq::WebApplication *)this
			  );

		// Bind SOAP callback
		xoap::bind(this, &FecSupervisorDiagLvlOne::onIncomingErr, "onIncomingErr", XDAQ_NS_URI );
		
		//Bind to default Web ITF
		xgi::bind(this,&FecSupervisorDiagLvlOne::Default, "Default");
		xgi::bind(this,&FecSupervisorDiagLvlOne::configureLvlOne, "configureLvlOne");
		xgi::bind(this,&FecSupervisorDiagLvlOne::applyConfigureLvlOne, "applyConfigureLvlOne");


	}
	


	void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
		*out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
		*out << cgicc::title("FecSupervisorDiagLvlOne Applet") << std::endl;

		xgi::Utils::getPageHeader
			(out, 
			getApplicationDescriptor()->getClassName()+" configuration page", 
			getApplicationDescriptor()->getContextDescriptor()->getURL(),
			getApplicationDescriptor()->getURN(),
			"/daq/xgi/images/Application.jpg"
			);

		std::string urlConf_ = "/";
		urlConf_ += getApplicationDescriptor()->getURN();
		urlConf_ += "/configureLvlOne";
		std::string buttonName = "Configure process : " +	getApplicationDescriptor()->getClassName();
		*out << cgicc::form().set("method","post").set("action", urlConf_).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "Apply").set("value", buttonName);
		*out << cgicc::p() << std::endl;
		*out << cgicc::form() << std::endl;
	}


	void configureLvlOne(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
		*out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
		*out << cgicc::title(getApplicationDescriptor()->getClassName()) << std::endl;

		xgi::Utils::getPageHeader
			(out, 
			getApplicationDescriptor()->getClassName(), 
			getApplicationDescriptor()->getContextDescriptor()->getURL(),
			getApplicationDescriptor()->getURN(),
			"/daq/xgi/images/Application.jpg"
			);

		std::string resumeUrl = "/";
		resumeUrl += getApplicationDescriptor()->getURN();;
		resumeUrl += "/Default";

		// Apply parameters
		*out << cgicc::form().set("method","post")
		.set("action", resumeUrl)
		.set("enctype","multipart/form-data") << std::endl;

		*out << cgicc::input().set("type", "submit")
		.set("name", "submit")
		.set("value", "Resume / Back to Default page");
		*out << cgicc::p() << std::endl;

		  // End of the form
		  *out << cgicc::form() << std::endl;

		std::string applyUrl = "/";
		applyUrl += getApplicationDescriptor()->getURN();
		applyUrl += "/applyConfigureLvlOne";

		// Apply parameters
		*out << cgicc::form().set("method","post")
		.set("action", applyUrl)
		.set("enctype","multipart/form-data") << std::endl;

		// Apply Changes
		*out << cgicc::input().set("type", "submit")
		.set("name", "submit")
		.set("value", "Apply Settings");
		*out << cgicc::p() << std::endl;

		//draw a separation line
		*out << "<hr style=\"width: 100%; height: 2px;\">";

		//Set General Logging level for this module
		*out << "<span style=\"font-weight: bold;\">";
		*out << cgicc::label("Error Manager Appender Settings") << "</span><br>" << std::endl;
		*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
		*out << cgicc::label("Set the general log level for this process to :") << std::endl;
		*out << "&nbsp;<select name=\"gloglevel\">";
		if (gLogLevel_ != "TRACE") *out << "<option>TRACE</option>"; else *out << "<option selected>TRACE</option>";
		if (gLogLevel_ != "DEBUG") *out << "<option>DEBUG</option>"; else *out << "<option selected>DEBUG</option>";
		if (gLogLevel_ != "INFO") *out << "<option>INFO</option>"; else *out << "<option selected>INFO</option>";
		if (gLogLevel_ != "WARN") *out << "<option>WARN</option>"; else *out << "<option selected>WARN</option>";
		if (gLogLevel_ != "ERROR") *out << "<option>ERROR</option>"; else *out << "<option selected>ERROR</option>";
		if (gLogLevel_ != "FATAL") *out << "<option>FATAL</option>"; else *out << "<option selected>FATAL</option>";
		if (gLogLevel_ != "OFF") *out << "<option>OFF</option>"; else *out << "<option selected>OFF</option>";
		*out << "</select>"<< std::endl;
		*out << cgicc::p() << std::endl;

		//draw a separation line
		*out << "<hr style=\"width: 100%; height: 2px;\">";

		//Create a CheckBox : Use Error Dispatcher Y/N
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
		*out << "<span style=\"font-weight: bold;\">";
		*out << cgicc::label("Use Error Dispatcher for this process") << "</span>" << std::endl;
		if (useDiag_ == true)
		{
			if (diagUsable_==true)
				*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
			if ((diagUsable_==false) && (diagNameStatus_ != "unchecked"))
				*out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">&nbsp;&nbsp;Error in process configuration</span>" << std::endl;
		}
		*out << cgicc::p() << std::endl;

		//display status error if needed
		if ( (diagNameStatus_ != "checked") && (diagNameStatus_ != "unchecked") )
			*out << "<span style=\"color: rgb(204, 0, 0);\">--" <<  diagNameStatus_ << "</span>" << std::endl;
		*out << cgicc::p() << std::endl;
		if ( (diagLIDStatus_ != "checked") && (diagLIDStatus_ != "unchecked") )
			*out << "<span style=\"color: rgb(204, 0, 0);\">" <<  diagLIDStatus_ << "</span>" << std::endl;
		if ( (diagIPStatus_ != "checked") && (diagIPStatus_ != "unchecked") )
			*out << "<span style=\"color: rgb(204, 0, 0);\">" <<  diagIPStatus_ << "</span>" << std::endl;

		//Create an input : ED server name ?
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

		//Create an input : ED Instance ?
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
		
		//Create an input : ED LID ?
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

		//Create an input : ED IP ?
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

		//Create an input : ED port ?
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

		//draw a separation line
		*out << "<hr style=\"width: 100%; height: 2px;\">";

		//Create a CheckBox : Close Reconf loop Y/N
		if (useReconf_ == true)
		{
			*out << cgicc::p() << cgicc::input().set("type", "checkbox")
			.set("name","useReconf").set("checked","true") << std::endl;
		}
		else
		{
			*out << cgicc::p() << cgicc::input().set("type", "checkbox")
			.set("name","useReconf").set("unchecked","false") << std::endl;
		}
		*out << "<span style=\"font-weight: bold;\">";
		*out << cgicc::label("Close reconfiguration loop for the error manager") << "</span>" << std::endl;
		if (useReconf_ == true)
		{
			if (reconfUsable_==true)
				*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
			if ((reconfUsable_==false) && (reconfNameStatus_ != "unchecked"))
				*out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">&nbsp;&nbsp;Error in process configuration</span>" << std::endl;
		}
		*out << cgicc::p() << std::endl;

		//display status error if needed
		if ( (reconfNameStatus_ != "checked") && (reconfNameStatus_ != "unchecked") )
			*out << "<span style=\"color: rgb(204, 0, 0);\">--" <<  reconfNameStatus_ << "</span>" << std::endl;
		*out << cgicc::p() << std::endl;
		if ( (reconfLIDStatus_ != "checked") && (reconfLIDStatus_ != "unchecked") )
			*out << "<span style=\"color: rgb(204, 0, 0);\">" <<  reconfLIDStatus_ << "</span>" << std::endl;
		if ( (reconfIPStatus_ != "checked") && (reconfIPStatus_ != "unchecked") )
			*out << "<span style=\"color: rgb(204, 0, 0);\">" <<  reconfIPStatus_ << "</span>" << std::endl;

		//Create an input : EMLoop server name ?
		*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
		*out << cgicc::label("Reconfiguration process Server Name") << std::endl;
		if (reconfNameStatus_ == "checked")
			*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
		if ( (reconfNameStatus_ != "checked") && (reconfNameStatus_ != "unchecked") )
			*out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
		*out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
		*out << cgicc::input().set("type","text")
		.set("name","reconfName")
		.set("size","40")
		.set("value",reconfName_) << std::endl;
		*out << cgicc::p() << std::endl;

		//Create an input : EMLoop Instance ?
		*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
		*out << cgicc::label("Reconfiguration process Server Instance") << std::endl;
		if (reconfNameStatus_ == "checked")
			*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
		if ( (reconfNameStatus_ != "checked") && (reconfNameStatus_ != "unchecked") )
			*out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" << "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
		*out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
		*out << cgicc::input().set("type","text")
		.set("name","reconfInstance")
		.set("size","10")
		.set("value",reconfInstance_) << std::endl;
		*out << cgicc::p() << std::endl;
		
		//Create an input : EMLoop LID ?
		*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
		*out << cgicc::label("Reconfiguration process Server LID") << std::endl;
		if (reconfLIDStatus_ == "checked")
			*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
		if ( (reconfLIDStatus_ != "checked") && (reconfLIDStatus_ != "unchecked") )
			*out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
		*out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
		*out << cgicc::input().set("type","text")
		.set("name","reconfLID")
		.set("size","10")
		.set("value",reconfLID_) << std::endl;
		*out << cgicc::p() << std::endl;

		//Create an input : EMLoop IP ?
		*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
		*out << cgicc::label("Reconfiguration process Server IP") << std::endl;
		if (reconfIPStatus_ == "checked")
			*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
		if ( (reconfIPStatus_ != "checked") && (reconfIPStatus_ != "unchecked") )
			*out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
		*out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
		*out << cgicc::input().set("type","text")
		.set("name","reconfIP")
		.set("size","20")
		.set("value",reconfIP_) << std::endl;
		*out << cgicc::p() << std::endl;

		//Create an input : EMLoop port ?
		*out << "&nbsp;&nbsp;&nbsp;&nbsp;";
		*out << cgicc::label("Reconfiguration process Server port") << std::endl;
		if (reconfIPStatus_ == "checked")
			*out << "&nbsp;&nbsp;:<span style=\"color: rgb(0, 204, 0);\">&nbsp;&nbsp;(checked : ok)</span>" << std::endl;
		if ( (reconfIPStatus_ != "checked") && (reconfIPStatus_ != "unchecked") )
			*out << "&nbsp;&nbsp;:<span style=\"color: rgb(204, 0, 0);\">" <<  "&nbsp;&nbsp;(checked : Error)" << "</span>" << std::endl;
		*out << "<br>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
		*out << cgicc::input().set("type","text")
		.set("name","reconfPort")
		.set("size","10")
		.set("value",reconfPort_) << std::endl;
		*out << cgicc::p() << std::endl;

		//draw a separation line
		*out << "<hr style=\"width: 100%; height: 2px;\">";

		// End of the form
		*out << cgicc::form() << std::endl;	
	}


	void applyConfigureLvlOne(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		//#define DEBUG_APPLY_CONFIGURE_FEC_LVL_ONE
		
		// Create a new Cgicc object containing all the CGI data
		cgicc::Cgicc cgi(in);

		//Query required log level for this module
		gLogLevel_ = cgi["gloglevel"]->getValue();
		//Query the use error dispatcher checkbox
		useDiag_ = cgi.queryCheckbox("useDiag");
		// Query ED name
		diagName_ = cgi["diagName"]->getValue() ;
		// Query ED instance
		diagInstance_ = cgi["diagInstance"]->getValue() ;
		// Query ED IP
		diagIP_ = cgi["diagIP"]->getValue() ;
		// Query ED port
		diagPort_ = cgi["diagPort"]->getValue() ;
		// Query ED lid
		diagLID_ = cgi["diagLID"]->getValue() ;
		//Query the close reconfiguration loop checkbox
		useReconf_ = cgi.queryCheckbox("useReconf");
		// Query ED name
		reconfName_ = cgi["reconfName"]->getValue() ;
		// Query ED instance
		reconfInstance_ = cgi["reconfInstance"]->getValue() ;
		// Query ED IP
		reconfIP_ = cgi["reconfIP"]->getValue() ;
		// Query ED port
		reconfPort_ = cgi["reconfPort"]->getValue() ;
		// Query ED lid
		reconfLID_ = cgi["reconfLID"]->getValue() ;

		if (useDiag_ == true)
		{
			diagUsable_ = false;
			xdaq::ApplicationDescriptor * d;
			try
			{
				d = getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(diagName_, atoi(diagInstance_.c_str()));
				diagNameStatus_ = "checked";
				if (d->getLocalId() != (unsigned long)atol(diagLID_.c_str()))
				{
					stringstream errMsg;
					errMsg << "&nbsp;&nbsp;Error : process " << diagName_ << " with instance " << diagInstance_ << " has LID " << d->getLocalId() << " and not " << diagLID_ << std::endl;
					diagLIDStatus_ = errMsg.str();
					diagIPStatus_="unchecked";
				}
				else
				{
					diagLIDStatus_ = "checked";
					std::string givenURL="http://" + diagIP_ + ":" + diagPort_;
					if (givenURL != d->getContextDescriptor()->getURL())
					{
						stringstream errMsg;				
						errMsg << "&nbsp;&nbsp;Error : process " << diagName_ << " with instance " << diagInstance_ << " has url " << d->getContextDescriptor()->getURL() << " and not " << givenURL << std::endl;
						diagIPStatus_ = errMsg.str();
					}
					else
					{
						diagIPStatus_="checked";
						diagUsable_=true;
					}
				}
			}
			catch (xdaq::exception::Exception& e)
			{
				diagUsable_ = false;
				stringstream errMsg;
				errMsg << "ErrorInfo : process " << diagName_ << " with instance " << diagInstance_ << " does not exists" << std::endl;
				diagNameStatus_ = errMsg.str();
				diagLIDStatus_="unchecked";
				diagIPStatus_="unchecked";
			}
		}
		else
		{
				diagUsable_ = false;
				diagNameStatus_ = "unchecked";
				diagLIDStatus_="unchecked";
				diagIPStatus_="unchecked";

		}




		if (diagUsable_ == true)
		{
			if ( (diagIP_ != oldDiagIP_) || (diagPort_ != oldDiagPort_) || 
			(diagName_ != oldDiagName_) || (diagInstance_ != oldDiagInstance_) || (diagLID_ != oldDiagLID_) )
			{
				socketSoapExists_ = false;
				oldDiagIP_ = diagIP_;
				oldDiagPort_ = diagPort_;
				oldDiagName_ = diagName_;
				oldDiagInstance_ = diagInstance_;
				oldDiagLID_ = diagLID_;
			}
			if (socketSoapExists_ == false)
			{
				#ifdef DEBUG_APPLY_CONFIGURE_FEC_LVL_ONE
					std::cout << "Creating new socket soap appender for DIAGLEVELONE and routing error messages to it." << std::endl;
				#endif
				SharedAppenderPtr append_3(new SOAPDiagAppender(LOG4CPLUS_TEXT(diagIP_), atoi(diagPort_.c_str()), atoi(diagLID_.c_str()),
				LOG4CPLUS_TEXT("receivedLog"),LOG4CPLUS_TEXT(diagName_),
				(xdaq::WebApplication *)this,
				(unsigned long)atol(diagInstance_.c_str())
				));

/*
				SharedAppenderPtr append_3(new SOAPDiagAppender(
				LOG4CPLUS_TEXT(this->getDiagIP()),
				atoi(this->getDiagPort().c_str()),
				atoi(this->getDiagLid().c_str()),
				LOG4CPLUS_TEXT("receivedLog"),
				LOG4CPLUS_TEXT(this->getDiagName()),
				wa_,
				(unsigned long)atol(diagInstance_.c_str())
				));
*/
				append_3->setName(LOG4CPLUS_TEXT("EMSocketSOAPDiagAppender"));
				this->getApplicationLogger().setAdditivity(0);
				this->getApplicationLogger().removeAllAppenders();
				this->getApplicationLogger().addAppender(append_3);
				socketSoapExists_ = true;
			}
		}
		else
		{
			if (socketSoapExists_ == true)
			{
				#ifdef DEBUG_APPLY_CONFIGURE_FEC_LVL_ONE
					std::cout << "Deleting existing socket soap appender for DIAGLEVELONE and routing error messages to console." << std::endl;
				#endif
				this->getApplicationLogger().removeAllAppenders();
				this->getApplicationLogger().setAdditivity(1);
				socketSoapExists_ = false;
			}
		}

		//Set DiagBagWizard internals according to configuration form inputs & tests results
		diagService_->setUseDiag(useDiag_);
		diagService_->setUseEd(diagUsable_);




		if (useReconf_ == true)
		{
			if ((useDiag_ == true) && (diagUsable_ == true))
			{	
				reconfUsable_ = false;
				xdaq::ApplicationDescriptor * d;
				try
				{
					d = getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(reconfName_, atoi(reconfInstance_.c_str()));
					reconfNameStatus_ = "checked";
					if (d->getLocalId() != (unsigned long)atol(reconfLID_.c_str()))
					{
						stringstream errMsg;
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
							stringstream errMsg;				
							errMsg << "&nbsp;&nbsp;Error : process " << reconfName_ << " with instance " << reconfInstance_ << " has url " << d->getContextDescriptor()->getURL() << " and not " << givenURL << std::endl;
							reconfIPStatus_ = errMsg.str();
						}
						else
						{
							reconfIPStatus_="checked";
							reconfUsable_=true;
							#ifdef DEBUG_APPLY_CONFIGURE_FEC_LVL_ONE
								std::cout << "Reconfiguration loop closed and validated" << std::endl;
							#endif
						}
					}
				}
				catch (xdaq::exception::Exception& e)
				{
					reconfUsable_ = false;
					stringstream errMsg;
					errMsg << "ErrorInfo : process " << reconfName_ << " with instance " << reconfInstance_ << " does not exists" << std::endl;
					reconfNameStatus_ = errMsg.str();
					reconfLIDStatus_="unchecked";
					reconfIPStatus_="unchecked";
				}
			}
			else
			{
				reconfUsable_ = false;
				stringstream errMsg;
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



		//In all cases, set general log level for the current appender
		if (gLogLevel_ == "TRACE") this->getApplicationLogger().setLogLevel(TRACE_LOG_LEVEL);
		if (gLogLevel_ == "DEBUG") this->getApplicationLogger().setLogLevel(DEBUG_LOG_LEVEL);
		if (gLogLevel_ == "INFO") this->getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);
		if (gLogLevel_ == "WARN") this->getApplicationLogger().setLogLevel(WARN_LOG_LEVEL);
		if (gLogLevel_ == "ERROR") this->getApplicationLogger().setLogLevel(ERROR_LOG_LEVEL);
		if (gLogLevel_ == "FATAL") this->getApplicationLogger().setLogLevel(FATAL_LOG_LEVEL);
		if (gLogLevel_ == "OFF") this->getApplicationLogger().setLogLevel(OFF_LOG_LEVEL);

		#ifdef DEBUG_APPLY_CONFIGURE_FEC_LVL_ONE
			if (useDiag_ == true) std::cout << "After Aplly Configure : useDiag_ is TRUE" << std::endl;
			if (useDiag_ == false) std::cout << "After Aplly Configure : useDiag_ is FALSE" << std::endl;
		#endif

		this->configureLvlOne(in,out) ; \
	}


	void fillClog(xoap::MessageReference msg, CLog * c)
	{
		//Fill CLog with received data
		xoap::SOAPBody rb = msg->getSOAPPart().getEnvelope().getBody();
		vector<xoap::SOAPElement> logElement = rb.getChildElements ();
		xoap::SOAPName timestamp ("timestamp", "", "");
		c->setTimeStamp(atoll(logElement[0].getAttributeValue(timestamp).c_str()));
		xoap::SOAPName logger ("logger", "", "");
		c->setLogger(logElement[0].getAttributeValue(logger));
		xoap::SOAPName level ("level", "", "");
		c->setLevel(logElement[0].getAttributeValue(level));
		xoap::SOAPName thread ("thread", "", "");
		c->setThread(logElement[0].getAttributeValue(thread));
		xoap::SOAPName message ("message", "", "");
		c->setMessage(logElement[0].getAttributeValue(message));
		xoap::SOAPName ndc ("ndc", "", "");
		c->setNDC(logElement[0].getAttributeValue(ndc));
		xoap::SOAPName file ("file", "", "");
		c->setFile(logElement[0].getAttributeValue(file));
		xoap::SOAPName line ("line", "", "");
		c->setLine(atoi(logElement[0].getAttributeValue(line).c_str()));
		xoap::SOAPName text ("text", "", "");
		c->setText(logElement[0].getAttributeValue(text));
		xoap::SOAPName errorcode ("errorcode", "", "");
		c->setErrorCode(logElement[0].getAttributeValue(errorcode));
		xoap::SOAPName faultstate ("faultstate", "", "");
		c->setFaultState(logElement[0].getAttributeValue(faultstate));
		xoap::SOAPName systemid ("systemid", "", "");
		c->setSystemID(logElement[0].getAttributeValue(systemid));
		xoap::SOAPName subsystemid ("subsystemid", "", "");
		c->setSubSystemID(logElement[0].getAttributeValue(subsystemid));
		xoap::SOAPName classname ("classname", "", "");
		c->setClassName(logElement[0].getAttributeValue(classname));
		xoap::SOAPName instance ("instance", "", "");
		c->setInstance(atoi(logElement[0].getAttributeValue(instance).c_str()));
		xoap::SOAPName lid ("lid", "", "");
		c->setLid(atoi(logElement[0].getAttributeValue(lid).c_str()));
		xoap::SOAPName edtimestamp ("edtimestamp", "", "");
		c->setEdTimeStamp(atoll(logElement[0].getAttributeValue(edtimestamp).c_str()));
		xoap::SOAPName lvl1name ("lvl1name", "", "");
		c->setLvl1Name(logElement[0].getAttributeValue(lvl1name));
		xoap::SOAPName lvl1instance ("lvl1instance", "", "");
		c->setLvl1Instance(logElement[0].getAttributeValue(lvl1instance));
		xoap::SOAPName routetoem ("routetoem", "", "");
		c->setRouteToEm(logElement[0].getAttributeValue(routetoem));
		xoap::SOAPName dbloglevel ("dbloglevel", "", "");
		c->setDbLogLevel(logElement[0].getAttributeValue(dbloglevel));
		xoap::SOAPName csloglevel ("csloglevel", "", "");
		c->setCsLogLevel(logElement[0].getAttributeValue(csloglevel));
		xoap::SOAPName ccloglevel ("ccloglevel", "", "");
		c->setCcLogLevel(logElement[0].getAttributeValue(ccloglevel));
		xoap::SOAPName forceddblogs ("forceddblogs", "", "");
		c->setForcedDbLog(logElement[0].getAttributeValue(forceddblogs));
		xoap::SOAPName forcedcslogs ("forcedcslogs", "", "");
		c->setForcedCsLog(logElement[0].getAttributeValue(forcedcslogs));
		xoap::SOAPName forcedcclogs ("forcedcclogs", "", "");
		c->setForcedCcLog(logElement[0].getAttributeValue(forcedcclogs));
		xoap::SOAPName gloglevel ("gloglevel", "", "");
		c->setGLogLevel(logElement[0].getAttributeValue(gloglevel));
	}


	void displayClog(CLog * c)
	{
	if (reconfUsable_ == false)
	{
		std::cout << "RECONF message received, but I will not use it." << std::endl;
	}
	else
	{
		std::cout << std::endl << std::endl << std::endl;
		std::cout << "in FecSupervisorDiagLvlOne, CLog filled" << std::endl;
		std::cout << "TimeStamp is : " << c->getTimeStamp() << std::endl;
		std::cout << "Logger is : " << c->getLogger() << std::endl;
		std::cout << "Level is : " << c->getLevel() << std::endl;
		std::cout << "Threads is : " << c->getThread() << std::endl;
		std::cout << "Message is : " << c->getMessage() << std::endl;
		std::cout << "NDC is : " << c->getNDC() << std::endl;
		std::cout << "File is : " << c->getFile() << std::endl;
		std::cout << "Line is : " << c->getLine() << std::endl;
		std::cout << "Text is : " << c->getText() << std::endl;
		std::cout << "ErrorCode is : " << c->getErrorCode() << std::endl;
		std::cout << "FaultState is : " << c->getFaultState() << std::endl;
		std::cout << "SystemID is : " << c->getSystemID() << std::endl;
		std::cout << "SubSystemID is : " << c->getSubSystemID() << std::endl;
		std::cout << "ClassName is : " << c->getClassName() << std::endl;
		std::cout << "Instance is : " << c->getInstance() << std::endl;
		std::cout << "LocalID is : " << c->getLid() << std::endl;
		std::cout << "ErrorDispatcherTimeStamp is : " << c->getEdTimeStamp() << std::endl;
		std::cout << "Lvl1Name is : " << c->getLvl1Name() << std::endl;
		std::cout << "Lvl1Instance is : " << c->getLvl1Instance() << std::endl;
		std::cout << "RouteToErrorManager is : " << c->getRouteToEm() << std::endl;
		std::cout << "DbLogLevel is : " << c->getDbLogLevel() << std::endl;
		std::cout << "CsLogLevel is : " << c->getCsLogLevel() << std::endl;
		std::cout << "CcLogLevel is : " << c->getCcLogLevel() << std::endl;
		std::cout << "ForcedDbLogs is : " << c->getForcedDbLog() << std::endl;
		std::cout << "ForcedCsLogs is : " << c->getForcedCsLog() << std::endl;
		std::cout << "ForcedCcLogs is : " << c->getForcedCcLog() << std::endl;
		std::cout << "GlobalLogLevel is : " << c->getGLogLevel() << std::endl;
	}
	}



	// SOAP Callback triggered on ReceivedError
	xoap::MessageReference onIncomingErr (xoap::MessageReference msg) throw (xoap::exception::Exception)
	{
		//#define DEBUG_FECLVL1_ON_INCOMING_ERR
		CLog c;
		// reply to caller
		xoap::MessageReference reply = xoap::createMessage();
		xoap::SOAPEnvelope envelope = reply->getSOAPPart().getEnvelope();
		xoap::SOAPName responseName = envelope.createName( "onIncomingErrResponse", "xdaq", XDAQ_NS_URI);
		envelope.getBody().addBodyElement ( responseName );

		//Fill CLOG with SOAP message content
		fillClog(msg,&c);
		
		//If we are not in reconfiguration stage, just ACK the reconf command to ED
		if ( (reconfUsable_ == false) && (c.getText() == "RECONFCMD") )
		{
			#ifdef DEBUG_FECLVL1_ON_INCOMING_ERR
				std::cout << "Reconf cmd on open loop received ; auto-ack sent" << std::endl;
			#endif

			diagService_->reportError("This is a Reconfiguration auto ACK Log",
			DIAGINFO,
			"RECONFACK",
			atoi(c.getErrorCode().c_str()),
			"STEADY",
			"TRACKER",
			"TEC");
		}

		/*
		USERS_NOTE
		
		The following block must be modified to set up a home-made system recovery loop.
		
		One example is harcoded here.
		- When a reconfiguration request message arrives, sended by a process to the Error Dispatcher
		and routed from the Error Dispatcher to here, and if the error code associated
		to this message is equal to 100, then the recovery function "recovery100", located
		in the file DummySupervisor.h, is fired.
		- If a reconfiguration request message arrives with an associated error code different from 100,
		as we do not have any specific reconfiguration procedure to fire in this example for error codes
		different from 100, nothing is done. We simply ACK the reconfiguration request to the Error
		Dispatcher, like if we were in an opened loop case.
		- If you have more than one error code to manage (i.e. more than one error code which will
		fire a reconfiguration function, you have two solutions :
		  --Call a general reconfiguration function and take appropriate actions in it, according to the
		    error code
		  --Set up in this method as many callbacks to fire as you have error codes to process (1 error code
		  links to 1 reconfiguration function).
		*/

		if (reconfUsable_ == true)
		{
			if (c.getText() == "RECONFCMD")
			{
				switch ( atoi(c.getErrorCode().c_str()) )
				{
					case 100 :
						#ifdef DEBUG_FECLVL1_ON_INCOMING_ERR
							std::cout << "Reconf cmd on closed loop received for ErrNo:" << atoi(c.getErrorCode().c_str()) << " ; Automated System Recovery launched." << std::endl;
							std::cout << "Calling System Recovery Callback" << std::endl;
						#endif
						try
						{
						  	xoap::MessageReference msg = xoap::createMessage();
					                xoap::SOAPPart soap = msg->getSOAPPart();
					                xoap::SOAPEnvelope envelope = soap.getEnvelope();
				        	        xoap::SOAPBody body = envelope.getBody();
			        	        	xoap::SOAPName command = envelope.createName("recovery100","xdaq", "urn:xdaq-soap:3.0");
				                	body.addBodyElement(command);
							xdaq::ApplicationDescriptor * d = getApplicationContext()->getApplicationGroup()->getApplicationDescriptor(reconfName_, atol(reconfInstance_.c_str()));
							xoap::MessageReference reply = getApplicationContext()->postSOAP(msg, d);
						} 
						catch (xdaq::exception::Exception& e)
						{
							#ifdef DEBUG_FECLVL1_ON_INCOMING_ERR
								std::cout << "Cannot reach callback for recovery100 !" << std::endl;
								std::cout << "I will at least try to ACK the ReconfigurationRunning status from ErrorDispatcher." << std::endl;
							#endif
							diagService_->reportError("This is a Reconfiguration Call Failure ACK Log",
							 DIAGINFO,
							 "RECONFACK",
							 atoi(c.getErrorCode().c_str()),
							 "STEADY",
							 "TRACKER",
							 "TEC");
						}

					break;
					default :
						#ifdef DEBUG_FECLVL1_ON_INCOMING_ERR
							std::cout << "Reconf cmd on closed loop received for ErrNo " << atoi(c.getErrorCode().c_str()) << " ; ErrNo not linked to specific reconfiguration code ; doing nothing" << std::endl;
						#endif

						diagService_->reportError("This is a Reconfiguration auto ACK Log",
						 DIAGINFO,
						 "RECONFACK",
						 atoi(c.getErrorCode().c_str()),
						 "STEADY",
						 "TRACKER",
						 "TEC");
					 break;
				}

			}
			
			if (c.getText() == "RECONFRUN")
			{
				/*
				USERS_NOTE
				
				In this section, you can extract the content of the fields of an error message sent with a
				"RECONFRUN" tag in the "text" field of this error message.
				
				For example, when running the recovery method "recovery100" in file DummySupervisor.h, if an
				error message of kind "RECONFRUN" is sent by this method, the content of the fields
				FaultState, SystemID and SubSystemID can be retrieved here via c.getFaultState(), c.getSystemID()
				and c.getSubSystemID() methods.
				This allows you to send informations about the recovery actions/status from your recovery method
				to this process.
				*/
				
				#ifdef DEBUG_FECLVL1_ON_INCOMING_ERR
					std::cout << "Received reconfiguration info : message is : " << c.getMessage() << std::endl;
				#endif

			}
			if (c.getText() == "RECONFSTOP")
			{
				#ifdef DEBUG_FECLVL1_ON_INCOMING_ERR
					std::cout << "Reconfiguration stop request on closed loop received ; ack sent" << std::endl;
					std::cout << "Attached message was : " << c.getMessage() << std::endl;
				#endif

				diagService_->reportError("This is a Reconfiguration requested ACK Log",
				DIAGINFO,
				"RECONFACK",
				atoi(c.getErrorCode().c_str()),
				"STEADY",
				"TRACKER",
				"TEC");
			}
		}
	return reply;

	}
	
	
};

#endif
#endif
