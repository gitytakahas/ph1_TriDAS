#ifndef _JSONFileItem_h_
#define _JSONFileItem_h_

/*
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
*/

#include "DiagCompileOptions.h"
#include <diagbag/DiagBagWizard.h>

#define JSON_TOPREP "toplevelrepository"
#define JSON_CURREP "currentrepository"
#define JSON_ONEREP "onerep"
#define JSON_ONEFILE "onefile"


#define JSON_TYPEOFCONTENT "typeofcontent"
#define JSON_CONTENTVALUE "contentvalue"



class JSONFileItem
{
public:

std::string typeOfContent_;
std::string contentValue_;


	JSONFileItem(	std::string typeOfContent,
				std::string contentValue);

				
				
				

	~JSONFileItem(void);

};

#endif

