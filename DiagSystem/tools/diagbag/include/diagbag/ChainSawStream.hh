#ifndef _ChainSawStream_h_
#define _ChainSawStream_h_


#include <sstream>
#include <sys/time.h>
#include <netdb.h>
#include <sys/time.h>


#include "xgi/Utils.h"
#include "xgi/Method.h"
#include "cgicc/CgiDefs.h"
#include "cgicc/Cgicc.h"
#include "cgicc/HTTPHTMLHeader.h"
#include "cgicc/HTMLClasses.h"
#include "cgicc/HTTPResponseHeader.h"


#include "xdaq/Application.h"
#include "xdaq/ApplicationGroup.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/ApplicationStub.h"
#include "xdaq/exception/Exception.h"
#include "xdaq/NamespaceURI.h"
#include "xdaq/WebApplication.h"
#include "xdaq/exception/ApplicationNotFound.h"
#include "xdaq/ApplicationContext.h"
#include "xdaq/exception/Exception.h"


#include "xoap/MessageReference.h"
#include "xoap/MessageFactory.h"
#include "xoap/SOAPEnvelope.h"
#include "xoap/SOAPBody.h"
#include "xoap/SOAPElement.h"
#include "xoap/Method.h"
#include "xoap/domutils.h"

#include "xdata/String.h"
#include "xdata/UnsignedLong.h"
#include "xdata/Integer.h"
#include "xdata/Boolean.h"


#include "xcept/tools.h"
#include "toolbox/utils.h"

/*
#include "sentinel/Interface.h"
#include "sentinel/Listener.h"
*/

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

//#include <XRelay.h>
#include <xrelay/Application.h>

#include "DiagCompileOptions.h"
//#include <soapdiagappender/SOAPDiagAppender.h>
#include "diagbag/helpers/clog.h"
#include "diagbag/DiagBag.h"
#include <xmldiagappender/XMLDiagAppender.h>
/*
#ifdef WILL_USE_GWT_FIREITEMS_VIEWER
	#include <diagbag/helpers/GwtCompliantDiagBagWizardMacros.h>
#else
	#include <diagbag/helpers/DiagBagWizardMacros.h>
#endif
*/


#include "toolbox/BSem.h"
#include "xcept/tools.h"
#include "toolbox/utils.h"


class ChainSawStream
{
public:
        XMLDiagAppender * chainsawAppender_;

        bool csRoute_;
        bool csRouteValidated_;
        std::string csRouteStatus_;

        std::string csReceiver_;
        std::string csReceiverStatus_;
        std::string csIPStatus_;
        std::string csIP_;
        std::string csPortStatus_;
        std::string csPort_;
		
		std::string streamName_;

	ChainSawStream(void)/* throw (xdaq::exception::Exception)*/;
	ChainSawStream(std::string ip, std::string port, std::string receiver, std::string name)/* throw (xdaq::exception::Exception)*/;
	~ChainSawStream(void)/* throw (xdaq::exception::Exception)*/;



	void setCsRoute(bool value);
	bool getCsRoute(void);

	void setCsRouteValidated(bool value);
	bool getCsRouteValidated(void);

	void setCsRouteStatus(std::string value);
	std::string getCsRouteStatus(void);



	void setCsReceiver(std::string value);
	std::string getCsReceiver(void);

	void setCsReceiverStatus(std::string value);
	std::string getCsReceiverStatus(void);

	void setCsIPStatus(std::string value);
	std::string getCsIPStatus(void);

	void setCsIP(std::string value);
	std::string getCsIP(void);

	void setCsPortStatus(std::string value);
	std::string getCsPortStatus(void);

	void setCsPort(std::string value);
	std::string getCsPort(void);

	void setChainsawAppender(void);
	XMLDiagAppender * getChainsawAppender(void);

	void setStreamName(std::string name);
	std::string getStreamName(void);


};


#endif


















