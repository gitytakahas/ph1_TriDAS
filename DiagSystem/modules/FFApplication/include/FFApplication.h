// $Id: FFApplication.h,v 1.3 2007/05/04 08:18:59 lgross Exp $

#ifndef _FFApplication_h_
#define _FFApplication_h_

#ifndef _SOAPStateMachine_h_
#define _SOAPStateMachine_h_


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


#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/HTTPResponseHeader.h"

#include "xdata/String.h"
#include "xdata/UnsignedLong.h"
#include "xdata/Integer.h"

#include <sys/time.h>

#include "xcept/tools.h"
#include "toolbox/utils.h"

#include "sentinel/Interface.h"
#include "sentinel/Listener.h"


#include "FFApplicationV.h"

class FFApplication: public xdaq::WebApplication, sentinel::Listener
{

protected:
	sentinel::Interface * sentinel_;

public:
	
	//! define factory method for instantion of HelloWorld application
	XDAQ_INSTANTIATOR();


	FFApplication(xdaq::ApplicationStub * s) throw (xdaq::exception::Exception) : xdaq::WebApplication(s)
	{
		//Give funny and useless informations at load time
		std::stringstream mmesg;
		mmesg << "Process version " << FFAPPLICATION_PACKAGE_VERSION << " loaded";
		this->getApplicationLogger().setLogLevel(INFO_LOG_LEVEL);
		LOG4CPLUS_INFO(this->getApplicationLogger(), mmesg.str());

		sentinel_ = 0;

		// retrieve the Sentinel
		try
		{
			sentinel_ = dynamic_cast<sentinel::Interface*>(getApplicationContext()->getFirstApplication("Sentinel"));
			sentinel_->join(this);
			// add this class as a listener to the sentinel to intercept incoming exceptions
			sentinel_->setListener(this,this);
			// Will handle exception in the group 'FFApplication' an exception handling context
			sentinel_->attachContext("TkDiagErrorsPool", this);
			//sentinel_->attachContext("toto", this);
		}
		catch(xdaq::exception::ApplicationNotFound & e)
		{
			LOG4CPLUS_WARN(getApplicationLogger(), "no sentinel found, remote notification not enable");
		}
		catch(sentinel::exception::Exception & se)
		{
			LOG4CPLUS_WARN(getApplicationLogger(), se.what());
		}
		//export a Web interface for easy self test command
		xgi::bind(this,&FFApplication::Default, "Default");
		xgi::bind(this,&FFApplication::sendLog, "sendLog");
		xgi::bind(this,&FFApplication::selfTestLog, "selfTestLog");
	}



void onException(xcept::Exception& e)
{

	std::cout << "In DIAGSYSTEM SIDE Context : Recived exception from ";

	xcept::ExceptionHistory history0(e);
	std::string mycontext;
	mycontext="NULL";

	while ( history0.hasMore() )
	{
		xcept::ExceptionInformation & info = history0.getPrevious();
		std::map<std::string, std::string, std::less<std::string> > & properties = info.getProperties();
		for  (std::map<std::string, std::string, std::less<std::string> >::iterator j = properties.begin(); j != properties.end(); j++)
		{
			//std::cout << (*j).first << " = " << (*j).second << std::endl;
			if ( (*j).first == "module" ) mycontext = (*j).second;
		}
	}

	if (mycontext == "FFApplication.h")
	{
		std::cout << "FILTER FARM application." << std::endl;
	}
	else
	{
		if ( (mycontext == "ErrorDispatcher.h") || (mycontext == "DiagSentinelErrorsGrabber.h") )
		{
			std::cout << "DIAGSYSTEM application." << std::endl;
		}
		else std::cout << std::endl;
	}


	std::cout << "Recived exception:" << std::endl;
	std::cout << xcept::stdformat_exception_history(e) << std::endl;	
	std::cout << "Exception details:" << std::endl;
	
	xcept::ExceptionHistory history(e);
	while ( history.hasMore() )
	{
		xcept::ExceptionInformation & info = history.getPrevious();
		std::map<std::string, std::string, std::less<std::string> > & properties = info.getProperties();
		for  (std::map<std::string, std::string, std::less<std::string> >::iterator j = properties.begin(); j != properties.end(); j++)
		{
			std::cout << (*j).first << " = " << (*j).second << std::endl;
		}
		
		std::cout << std::endl;
		
	}
	
}	





	void Default(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
		*out << cgicc::HTMLDoctype(cgicc::HTMLDoctype::eStrict) << std::endl;
		*out << cgicc::html().set("lang", "en").set("dir","ltr") << std::endl;
		*out << cgicc::title("Filter Farm application") << std::endl;

		xgi::Utils::getPageHeader
			(out, 
			"Filter Farm application", 
			getApplicationDescriptor()->getContextDescriptor()->getURL(),
			getApplicationDescriptor()->getURN(),
			"/daq/xgi/images/Application.jpg"
			);

			
		std::string urlSendLog = "/";
		urlSendLog += getApplicationDescriptor()->getURN();
		urlSendLog += "/sendLog";	
		*out << cgicc::form().set("method","post").set("action", urlSendLog).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Push a message in Filter Farm Error Management Context (catched by distant app)");
		*out << cgicc::p() << std::endl;
		*out << cgicc::form() << std::endl;

		std::string urlSelfTestLog = "/";
		urlSelfTestLog += getApplicationDescriptor()->getURN();
		urlSelfTestLog += "/selfTestLog";	
		*out << cgicc::form().set("method","post").set("action", urlSelfTestLog).set("enctype","multipart/form-data") << std::endl;
		*out << cgicc::input().set("type", "submit").set("name", "send").set("value", "Push a message in DiagSystem Error Management Context (catched by self)");
		*out << cgicc::p() << std::endl;
		*out << cgicc::form() << std::endl;

		xgi::Utils::getPageFooter(*out);
	}


	void sendLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
	try
	{
		if ( sentinel_ != 0 )
		{
			sentinel_->pushContext("TkDiagErrorsPool", this);
		}
		else
		{
			XCEPT_RAISE (xgi::exception::Exception, "Failed to remotly notify Exception, no sentinel");
		}
		
		xcept::Exception exception3(  "xcept::Exception", "Erreur/Message emis par l'application Filter Farm", __FILE__, __LINE__, __FUNCTION__);	
	
		std::string notifier = toolbox::toString("%s/%s", getApplicationDescriptor()->getContextDescriptor()->getURL().c_str() , getApplicationDescriptor()->getURN().c_str()) ;
		// Fill last exception according to schema
		// Mandatory fields
		exception3.setProperty("notifier", notifier );
		exception3.setProperty("qualifiedErrorSchemaURI", "http://xdaq.web.cern.ch/xdaq/xsd/2005/QualifiedSoftwareErrorRecord-10.xsd");
		exception3.setProperty("dateTime", toolbox::getDateTime() );
		exception3.setProperty("sessionID", "none" );
		exception3.setProperty("severity", "ERROR" );
		
	
		sentinel_->notify(exception3, this);
		
	} catch (sentinel::exception::Exception& e)
	{
		XCEPT_RETHROW (xgi::exception::Exception, "Failed to remotly notify Exception", e);
	}


		this->Default(in,out);
	}





	void selfTestLog(xgi::Input * in, xgi::Output * out ) throw (xgi::exception::Exception)
	{
	try
	{
		if ( sentinel_ != 0 )
		{
			sentinel_->pushContext("TkDiagErrorsPool", this);
		}
		else
		{
			XCEPT_RAISE (xgi::exception::Exception, "Failed to remotly notify Exception, no sentinel");
		}
		
		xcept::Exception exception3(  "xcept::Exception", "Auto-test message for Filter Farm", __FILE__, __LINE__, __FUNCTION__);	
	
		std::string notifier = toolbox::toString("%s/%s", getApplicationDescriptor()->getContextDescriptor()->getURL().c_str() , getApplicationDescriptor()->getURN().c_str()) ;
		// Fill last exception according to schema
		// Mandatory fields
		exception3.setProperty("notifier", notifier );
		exception3.setProperty("qualifiedErrorSchemaURI", "http://xdaq.web.cern.ch/xdaq/xsd/2005/QualifiedSoftwareErrorRecord-10.xsd");
		exception3.setProperty("dateTime", toolbox::getDateTime() );
		exception3.setProperty("sessionID", "none" );
		exception3.setProperty("severity", "ERROR" );
		
	
		sentinel_->notify(exception3, this);
	
	} catch (sentinel::exception::Exception& e)
	{
		XCEPT_RETHROW (xgi::exception::Exception, "Failed to remotly notify Exception", e);
	}


		this->Default(in,out);
	}



		




};

#endif
#endif



































