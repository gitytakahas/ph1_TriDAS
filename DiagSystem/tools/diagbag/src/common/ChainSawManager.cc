#include <diagbag/ChainSawManager.hh>


ChainSawManager::ChainSawManager()/* throw (xdaq::exception::Exception)*/
{
}


void ChainSawManager::addStream(std::string csIP, std::string csPort, std::string csReceiver, std::string streamName)
{
	ChainSawStream * chainSawStream = new ChainSawStream(csIP, csPort, csReceiver, streamName);
	ChainSawStreams_.insert(chainSawStream);
}


void ChainSawManager::removeStream(std::string streamName)
{
	for (ChainSawStreamsIterator_=ChainSawStreams_.begin();ChainSawStreamsIterator_!=ChainSawStreams_.end();ChainSawStreamsIterator_++)
	{

		if ( (*ChainSawStreamsIterator_)->getStreamName() == streamName )
		{
			delete (*ChainSawStreamsIterator_);
			ChainSawStreams_.erase((*ChainSawStreamsIterator_));
		}
	}
}



void ChainSawManager::pushMessageToStream(std::string streamName, std::string message)
{
	for (ChainSawStreamsIterator_=ChainSawStreams_.begin();ChainSawStreamsIterator_!=ChainSawStreams_.end();ChainSawStreamsIterator_++)
	{
		if ( (*ChainSawStreamsIterator_)->getStreamName() == streamName )
		{
			std::cout << "Pushing messages to stream will be implemented soon" << std::endl;
		}
	}
}



void ChainSawManager::pushClogToStream(std::string streamName, CLog l)
{
	for (ChainSawStreamsIterator_=ChainSawStreams_.begin();ChainSawStreamsIterator_!=ChainSawStreams_.end();ChainSawStreamsIterator_++)
	{
		if ( (*ChainSawStreamsIterator_)->getStreamName() == streamName )
		{

(*ChainSawStreamsIterator_)->getChainsawAppender()->appendToSocket(l);
//diagService_->chainsawAppender_->appendToSocket(l);
//std::cout << "WRITING TO STREAM HAHAHA" << std::endl;
			//Write Buffer to XML socket held by stream
		}
	}
}






void ChainSawManager::pushClogToFilteredStream(CLog l, std::string streamName, std::string filter)
{
//DEBUGME
//std::cout << "Logging to streamName : " << streamName << " with filter : " << filter << std::endl;
	for (ChainSawStreamsIterator_=ChainSawStreams_.begin();ChainSawStreamsIterator_!=ChainSawStreams_.end();ChainSawStreamsIterator_++)
	{
		if ( (*ChainSawStreamsIterator_)->getStreamName() == streamName )
		{
			if (filter != "")
			{
				if (filter != "NOFILTER")
				{
					std::string logger = l.getLogger();
					std::string::size_type loc = logger.find( filter, 0 );
					if( loc != std::string::npos )
					{
						//std::cout << "Found one stream for filter " << filter << " on streamname " << streamName << std::endl;
						(*ChainSawStreamsIterator_)->getChainsawAppender()->appendToSocket(l);
					}
				}
				else
				{
					(*ChainSawStreamsIterator_)->getChainsawAppender()->appendToSocket(l);
				}
			}
			else (*ChainSawStreamsIterator_)->getChainsawAppender()->appendToSocket(l);
		}
	}

}







int ChainSawManager::countStreams(void)
{
	int i=0;
	for (ChainSawStreamsIterator_=ChainSawStreams_.begin();ChainSawStreamsIterator_!=ChainSawStreams_.end();ChainSawStreamsIterator_++) i++;
	return i;
}





void ChainSawManager::deleteAllStreams(void)
{
	for (ChainSawStreamsIterator_=ChainSawStreams_.begin();ChainSawStreamsIterator_!=ChainSawStreams_.end();ChainSawStreamsIterator_++)
	{
			delete (*ChainSawStreamsIterator_);
			ChainSawStreams_.erase((*ChainSawStreamsIterator_));
	}
}















