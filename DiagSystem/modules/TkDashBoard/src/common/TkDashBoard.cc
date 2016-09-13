/*

   FileName : 		TkDashBoard.cc

   Content : 		TkDashBoard module

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


#include "TkDashBoard.h"

//#include <sentinel/Sentinel.h>
#include <sentinel/utils/NewsEvent.h>
#include <toolbox/TimeVal.h>


XDAQ_INSTANTIATOR_IMPL(TkDashBoard);




//        TkDashBoard::TkDashBoard(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): xdaq::WebApplication(s)
//        TkDashBoard::TkDashBoard(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): sentinel::dashboard::Application(s)
        TkDashBoard::TkDashBoard(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): sentinel::dashboard::Application(s)
		{
            xgi::bind(this,&TkDashBoard::Default, "Default");

		}


/*
        TkDashBoard::TkDashBoard(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception): sentinel::dashboard::Application
        {
            // bind xgi and xoap commands specific to this application

            xgi::bind(this,&TkDashBoard::Default, "Default");
			xgi::bind(this,&TkDashBoard::sendLogOne, "sendLogOne");
			xgi::bind(this,&TkDashBoard::sendLogTwo, "sendLogTwo");
			xgi::bind(this,&TkDashBoard::sendNewsOne, "sendNewsOne");
			xgi::bind(this,&TkDashBoard::sendNewsTwo, "sendNewsTwo");

        }
*/






        // Default() hyperDaq method
        void TkDashBoard::Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
        {

std::cout << "CALLING DEFAULT METHOD" << std::endl;
sentinel::dashboard::Application::Default(in, out);

		}


void TkDashBoard::actionPerformed ( xdata::Event& e )
{
	std::cout << "CALLING actionPerformed ( xdata::Event& e ) METHOD" << std::endl;
	sentinel::dashboard::Application::actionPerformed(e);
}

void TkDashBoard::actionPerformed( toolbox::Event& event )
{
	
	std::cout << "CALLING actionPerformed ( toolbox::Event& event ) METHOD" << std::endl;




	if (event.type() == "urn:xoap-event:Message") // A monitor report has been received
	{
		xoap::MessageReference msg = dynamic_cast<xoap::Event&>(event).getMessage();
		xoap::SOAPPart soap = msg->getSOAPPart();
		xoap::SOAPEnvelope envelope = soap.getEnvelope();
		xoap::SOAPBody body = envelope.getBody();
		DOMNode * node = body.getDOMNode();
		DOMNodeList* notifyNodeList = node->getOwnerDocument()->getElementsByTagNameNS(xoap::XStr("*"), xoap::XStr("notify"));
		xcept::Exception newException; 
		sentinel::utils::Serializer::importFrom (notifyNodeList->item(0), newException);

		xcept::ExceptionHistory history(newException);
		while ( history.hasMore() )
		{
		    xcept::ExceptionInformation & info = history.getPrevious();
		    std::map<std::string, std::string, std::less<std::string> > & properties = info.getProperties();
		    for  (std::map<std::string, std::string, std::less<std::string> >::iterator j = properties.begin(); 
        		  j != properties.end(); 
		          j++)
		    {
        		std::cout << (*j).first << " = " << (*j).second << std::endl;
		    }  
		    std::cout << std::endl;      
		}
	}




	sentinel::dashboard::Application::actionPerformed(event);
}


/*
            // Create HTML header
            *out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
            *out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
            *out << cgicc::title("Sentinel Sender") << std::endl;
            xgi::Utils::getPageHeader(  out, 
                                        "TkDashBoard", 
                                        getApplicationDescriptor()->getContextDescriptor()->getURL(),
                                        getApplicationDescriptor()->getURN(),
                                        "/daq/xgi/images/Application.jpg" );




			std::string urlSendLogOne = "/";
			urlSendLogOne += getApplicationDescriptor()->getURN();
			urlSendLogOne += "/sendLogOne";
			*out << cgicc::form().set("method","post").set("action", urlSendLogOne).set("enctype","multipart/form-data") << std::endl;
			*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Push a qualified Xcept in Sentinel - message ONE");
			*out << cgicc::p() << std::endl;
			*out << cgicc::form() << std::endl;


			std::string urlSendLogTwo = "/";
			urlSendLogTwo += getApplicationDescriptor()->getURN();
			urlSendLogTwo += "/sendLogTwo";
			*out << cgicc::form().set("method","post").set("action", urlSendLogTwo).set("enctype","multipart/form-data") << std::endl;
			*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Push a qualified Xcept in Sentinel - message TWO");
			*out << cgicc::p() << std::endl;
			*out << cgicc::form() << std::endl;

			std::string urlSendNewsOne = "/";
			urlSendNewsOne += getApplicationDescriptor()->getURN();
			urlSendNewsOne += "/sendNewsOne";
			*out << cgicc::form().set("method","post").set("action", urlSendNewsOne).set("enctype","multipart/form-data") << std::endl;
			*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Push a News in ws-eventing - message ONE");
			*out << cgicc::p() << std::endl;
			*out << cgicc::form() << std::endl;

			std::string urlSendNewsTwo = "/";
			urlSendNewsTwo += getApplicationDescriptor()->getURN();
			urlSendNewsTwo += "/sendNewsTwo";
			*out << cgicc::form().set("method","post").set("action", urlSendNewsTwo).set("enctype","multipart/form-data") << std::endl;
			*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Push a News in ws-eventing - message TWO");
			*out << cgicc::p() << std::endl;
			*out << cgicc::form() << std::endl;


            // Create HTML footer
            xgi::Utils::getPageFooter(*out);

        }
*/

/*
	void TkDashBoard::sendLogOne(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		//std::cout << "Qualifying exception by filling field ..." << std::endl;


		xcept::Exception exception3(  "xcept::Exception", "Erreur/Message ONE emis par l'application SentinelSennder", __FILE__, __LINE__, __FUNCTION__);
		std::string notifier = toolbox::toString("%s/%s", getApplicationDescriptor()->getContextDescriptor()->getURL().c_str() , getApplicationDescriptor()->getURN().c_str()) ;
		exception3.setProperty("notifier", notifier );
		exception3.setProperty("qualifiedErrorSchemaURI", "http://xdaq.web.cern.ch/xdaq/xsd/2005/QualifiedSoftwareErrorRecord-10.xsd");
		exception3.setProperty("sessionID", "none" );
		exception3.setProperty("severity", "ERROR" );

		std::cout << "Notifying exception ..." << std::endl;
		std::cout << "Running in zones : " << toolbox::printTokenSet(this->getApplicationContext()->getZoneNames(),",") << std::endl;
		std::cout << "Application belongs to group : " << this->getApplicationDescriptor()->getAttribute("group") << std::endl;

		this->notifyQualified("fatal", exception3);

		std::cout << "Exception notified" << std::endl;

		this->Default(in,out);
	}
*/

/*
	void TkDashBoard::sendLogTwo(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		//std::cout << "Qualifying exception by filling field ..." << std::endl;


		xcept::Exception exception3(  "xcept::Exception", "Erreur/Message TWO emis par l'application SentinelSennder", __FILE__, __LINE__, __FUNCTION__);
		std::string notifier = toolbox::toString("%s/%s", getApplicationDescriptor()->getContextDescriptor()->getURL().c_str() , getApplicationDescriptor()->getURN().c_str()) ;
		exception3.setProperty("notifier", notifier );
		exception3.setProperty("qualifiedErrorSchemaURI", "http://xdaq.web.cern.ch/xdaq/xsd/2005/QualifiedSoftwareErrorRecord-10.xsd");
		exception3.setProperty("sessionID", "none" );
		exception3.setProperty("severity", "ERROR" );

		std::cout << "Notifying exception ..." << std::endl;
		std::cout << "Running in zones : " << toolbox::printTokenSet(this->getApplicationContext()->getZoneNames(),",") << std::endl;
		std::cout << "Application belongs to group : " << this->getApplicationDescriptor()->getAttribute("group") << std::endl;

		this->notifyQualified("fatal", exception3);

		std::cout << "Exception notified" << std::endl;

		this->Default(in,out);
	}
*/

/*
	void TkDashBoard::sendNewsOne(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{

		sentinel::utils::NewsEvent newsEvent;
		toolbox::Properties& article = newsEvent.getArticle();
		std::stringstream  url;
		url << this->getApplicationDescriptor()->getContextDescriptor()->getURL() << "/" << this->getApplicationDescriptor()->getURN();

		// Mandatory fields     
		article.setProperty ("timestamp", toolbox::TimeVal::gettimeofday().toString(toolbox::TimeVal::loc));
		article.setProperty("notifier", url.str());
		article.setProperty("sessionID", this->getApplicationContext()->getSessionId() );
		article.setProperty("schemaURI",sentinel::news::NamespaceUri );
		article.setProperty ("text", "This is News number ONE");

		// Optional fields
		article.setProperty("service",this->getApplicationDescriptor()->getAttribute("service"));
		std::set<std::string> zones = this->getApplicationContext()->getZoneNames();
		article.setProperty("zone", toolbox::printTokenSet(zones,"," ));
		article.setProperty("group", this->getApplicationDescriptor()->getAttribute("group"));
		article.setProperty("notifierid", this->getApplicationDescriptor()->getAttribute("uuid"));

		// Retrieve pointer to sentinel application and send the article
		// NOTE: This is temporary code until build 7 of xdaq that will include an API
		//              at xdaq::Application level to send sentinel news articles
		//
		try
		{
		    xdaq::Application* sentinel = (getApplicationContext()->getFirstApplication("sentinel::Application")); 
		    if (sentinel != 0)
		    {
        		sentinel->getApplicationInfoSpace()->fireEvent ( newsEvent );
		    }
		}
		catch (xdaq::exception::Exception& e)
		{
		    XCEPT_RETHROW (xdaq::exception::Exception, "Cannot find sentinel plugin",e);
		}
		catch (xdata::exception::Exception& e)
		{
		    XCEPT_RETHROW (xdaq::exception::Exception, "Failed to send sentinel news article",e);
		}
		std::cout << "News ONE notified" << std::endl;

		this->Default(in,out);

	}
*/


/*

	void TkDashBoard::sendNewsTwo(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		sentinel::utils::NewsEvent newsEvent;
		toolbox::Properties& article = newsEvent.getArticle();
		std::stringstream  url;
		url << this->getApplicationDescriptor()->getContextDescriptor()->getURL() << "/" << this->getApplicationDescriptor()->getURN();

		// Mandatory fields     
		article.setProperty ("timestamp", toolbox::TimeVal::gettimeofday().toString(toolbox::TimeVal::loc));
		article.setProperty("notifier", url.str());
		article.setProperty("sessionID", this->getApplicationContext()->getSessionId() );
		article.setProperty("schemaURI",sentinel::news::NamespaceUri );
		article.setProperty ("text", "This is News number TWO");

		// Optional fields
		article.setProperty("service",this->getApplicationDescriptor()->getAttribute("service"));
		std::set<std::string> zones = this->getApplicationContext()->getZoneNames();
		article.setProperty("zone", toolbox::printTokenSet(zones,"," ));
		article.setProperty("group", this->getApplicationDescriptor()->getAttribute("group"));
		article.setProperty("notifierid", this->getApplicationDescriptor()->getAttribute("uuid"));

		// Retrieve pointer to sentinel application and send the article
		// NOTE: This is temporary code until build 7 of xdaq that will include an API
		//              at xdaq::Application level to send sentinel news articles
		//
		try
		{
		    xdaq::Application* sentinel = (getApplicationContext()->getFirstApplication("sentinel::Application")); 
		    if (sentinel != 0)
		    {
        		sentinel->getApplicationInfoSpace()->fireEvent ( newsEvent );
		    }
		}
		catch (xdaq::exception::Exception& e)
		{
		    XCEPT_RETHROW (xdaq::exception::Exception, "Cannot find sentinel plugin",e);
		}
		catch (xdata::exception::Exception& e)
		{
		    XCEPT_RETHROW (xdaq::exception::Exception, "Failed to send sentinel news article",e);
		}
		std::cout << "News TWO notified" << std::endl;

		this->Default(in,out);


	}
*/


















