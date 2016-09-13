#include <diagbag/JSONItem.h>


JSONItem::JSONItem( std::string level,
					std::string errcode,
					std::string message,
					std::string timestamp,
					std::string source,
					std::string systemid,
					std::string subsystemid,
					std::string extraBuffer,
					std::string machine,
					std::string port,
					std::string procname,
					std::string procinstance,
					std::string lognumber,
					std::string fecHardId,
					std::string ring,
					std::string ccu,
					std::string i2cChannel,
					std::string i2cAddress,
					std::string fedId,
					std::string fedChannel,
					std::string crate,
					std::string slot,
					std::string nestedclasspath,
					std::string nestedfilename,
					std::string nestedlinenumber,
                                        std::string partitionname,
					std::string logfilename,
					std::string gedspecs,
					std::string isonline,
					std::string ajaxspecs)



{
	level_ = level;
	errcode_ = errcode;
	message_ = message;
	timestamp_ = timestamp;
	source_ = source;
	systemid_ = systemid;
	subsystemid_ = subsystemid;
	extraBuffer_ = extraBuffer;
	machine_ = machine;
	port_ = port;
	procname_ = procname;
	procinstance_ = procinstance;
	lognumber_ = lognumber;
	fecHardId_ = fecHardId;
	ring_ = ring;
	ccu_ = ccu;
	i2cChannel_ = i2cChannel;
	i2cAddress_ = i2cAddress;
	fedId_ = fedId;
	fedChannel_ = fedChannel;
	crate_ = crate;
	slot_ = slot;
	nestedclasspath_ = nestedclasspath;
	nestedfilename_ = nestedfilename;
	nestedlinenumber_ = nestedlinenumber;
        partitionname_ = partitionname;
        logfilename_ = logfilename;
	
        gedspecs_ = gedspecs;
        isonline_ = isonline;
        ajaxspecs_ = ajaxspecs;
	
	
	
	
}



JSONItem::~JSONItem(void)
{
}





