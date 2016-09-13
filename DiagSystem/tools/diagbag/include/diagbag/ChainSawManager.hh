#ifndef _ChainSawManager_h_
#define _ChainSawManager_h_


#include <diagbag/ChainSawStream.hh>

#define DIAGMAX_CS_STREAMS	10


class ChainSawManager
{
public:

	ChainSawManager(void)/* throw (xdaq::exception::Exception)*/;


	std::set<ChainSawStream *> ChainSawStreams_;
	std::set<ChainSawStream *>::iterator ChainSawStreamsIterator_;

	void addStream(std::string csIP, std::string csPort, std::string csReceiver, std::string streamName);

	void removeStream(std::string streamName);

	void pushMessageToStream(std::string streamName, std::string message);

	void pushClogToStream(std::string streamName, CLog l);


	void pushClogToFilteredStream(CLog l, std::string streamName, std::string filter);


	int countStreams(void);

	void deleteAllStreams(void);

};


#endif


















