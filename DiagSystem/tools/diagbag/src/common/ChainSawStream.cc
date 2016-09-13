#include <diagbag/ChainSawStream.hh>


ChainSawStream::ChainSawStream()/* throw (xdaq::exception::Exception)*/
{
	this->chainsawAppender_ = NULL;
}

ChainSawStream::ChainSawStream(std::string ip, std::string port, std::string receiver, std::string name)/* throw (xdaq::exception::Exception)*/
{
	csIP_ = ip;
	csPort_ = port;
	csReceiver_ = receiver;
	streamName_ = name;

	struct hostent * host = gethostbyname( this->csIP_.c_str() );
	std::string csMainIP;
	if (host != NULL)
	{
		csMainIP = (std::string)inet_ntoa(*((struct in_addr * )host->h_addr_list[0])) ;
	}
	csMainIP = this->csIP_;
	chainsawAppender_ = new XMLDiagAppender(LOG4CPLUS_TEXT(csMainIP), atoi(this->csPort_.c_str()), LOG4CPLUS_TEXT(this->csReceiver_));

}



ChainSawStream::~ChainSawStream()/* throw (xdaq::exception::Exception)*/
{
	if (this->chainsawAppender_ != NULL)
	{
		delete this->chainsawAppender_ ;
		chainsawAppender_ = NULL;
	}
		
}


void ChainSawStream::setCsRoute(bool value) {this->csRoute_ = value;}
bool ChainSawStream::getCsRoute(void) {return this->csRoute_;}

void ChainSawStream::setCsRouteValidated(bool value) {this->csRouteValidated_ = value;}
bool ChainSawStream::getCsRouteValidated(void) {return this->csRouteValidated_;}

void ChainSawStream::setCsRouteStatus(std::string value) {this->csRouteStatus_ = value;}
std::string ChainSawStream::getCsRouteStatus(void) {return this->csRouteStatus_;}




void ChainSawStream::setCsReceiver(std::string value) {this->csReceiver_ = value;}
std::string ChainSawStream::getCsReceiver(void) {return this->csReceiver_;}

void ChainSawStream::setCsReceiverStatus(std::string value) {this->csReceiverStatus_ = value;}
std::string ChainSawStream::getCsReceiverStatus(void) {return this->csReceiverStatus_;}

void ChainSawStream::setCsIPStatus(std::string value) {this->csIPStatus_ = value;}
std::string ChainSawStream::getCsIPStatus(void) {return this->csIPStatus_;}

void ChainSawStream::setCsIP(std::string value) {this->csIP_ = value;}
std::string ChainSawStream::getCsIP(void) {return this->csIP_;}

void ChainSawStream::setCsPortStatus(std::string value) {this->csPortStatus_ = value;}
std::string ChainSawStream::getCsPortStatus(void) {return this->csPortStatus_;}

void ChainSawStream::setCsPort(std::string value) {this->csPort_ = value;}
std::string ChainSawStream::getCsPort(void) {return this->csPort_;}


void ChainSawStream::setChainsawAppender(void)
{
	struct hostent * host = gethostbyname( this->csIP_.c_str() );
	std::string csMainIP;
	if (host != NULL)
	{
		csMainIP = (std::string)inet_ntoa(*((struct in_addr * )host->h_addr_list[0])) ;
	}
	csMainIP = this->csIP_;
	this->chainsawAppender_ = new XMLDiagAppender(LOG4CPLUS_TEXT(csMainIP), atoi(this->csPort_.c_str()), LOG4CPLUS_TEXT(this->csReceiver_));
}

XMLDiagAppender * ChainSawStream::getChainsawAppender(void)
{
	return this->chainsawAppender_;
}

void ChainSawStream::setStreamName(std::string name) {this->streamName_ = name;}
std::string ChainSawStream::getStreamName(void) {return this->streamName_;}




