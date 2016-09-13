#ifndef _JSONItem_h_
#define _JSONItem_h_


#define JSON_LEVEL "level"
#define JSON_ERRCODE "errcode"
#define JSON_MSG "message"
#define JSON_TIMESTAMP "timestamp"
#define JSON_SOURCE "source"


#define JSON_SYSID "systemid"
#define JSON_SUBSYSID "subsystemid"
#define JSON_EBUFF "extraBuffer"
#define JSON_MACHINE "machine"
#define JSON_PORT "port"
#define JSON_PROCNAME "procname"
#define JSON_PROCINSTANCE "procinstance"

#define JSON_LOGNUMBER "lognumber"



#define JSON_FECHARDID "fechardid"
#define JSON_RING "ring"
#define JSON_CCU "ccu"
#define JSON_I2CCHANNEL "i2cchannel"
#define JSON_I2CADDRESS "i2caddress"
#define JSON_FEDID "fedid"
#define JSON_FEDCHANNEL "fedchannel"
#define JSON_CRATE "crate"
#define JSON_SLOT "slot"

#define JSON_NESTEDCLASSPATH "nestedclasspath"
#define JSON_NESTEDFILENAME "nestedfilename"
#define JSON_NESTEDLINENUMBER "nestedlinenumber"

#define JSON_PARTITIONNAME "partitionname"


#include "DiagCompileOptions.h"
#include <diagbag/DiagBagWizard.h>



class JSONItem
{
public:

std::string level_;
std::string errcode_;
std::string message_;
std::string timestamp_;
std::string source_;
std::string systemid_;
std::string subsystemid_;
std::string extraBuffer_;
std::string machine_;
std::string port_;
std::string procname_;
std::string procinstance_;
std::string lognumber_;
std::string fecHardId_;
std::string ring_;
std::string ccu_;
std::string i2cChannel_;
std::string i2cAddress_;
std::string fedId_;
std::string fedChannel_;
std::string crate_;
std::string slot_;
std::string nestedclasspath_;
std::string nestedfilename_;
std::string nestedlinenumber_;
std::string partitionname_;
std::string logfilename_;
std::string gedspecs_;
std::string isonline_;
std::string ajaxspecs_;

	JSONItem(	std::string level,
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
				std::string ajaxspecs);

				
				
				
//	JSONItem(CLog* c);

	~JSONItem(void);

};

#endif

