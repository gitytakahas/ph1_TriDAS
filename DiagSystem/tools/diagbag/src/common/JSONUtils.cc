#include <diagbag/JSONUtils.h>


JSONUtils::JSONUtils(void) {}


JSONUtils::~JSONUtils(void) {}



std::string JSONUtils::craftJsonArray(std::vector<JSONItem>*  jsonItemsList)
{
std::string array="";



			array += "[";

			while (jsonItemsList->empty() == false)
			{
				JSONItem* jsonItem = (JSONItem*)&(*(jsonItemsList->end() - 1));


				array += "{\"";
				array += JSON_LEVEL;
				array += "\": \"";
				array += jsonItem->level_;
				array += "\",";



				array += "\"";
				array += JSON_ERRCODE;
				array += "\": \"";
				array += jsonItem->errcode_;
				array += "\",";



				array += "\"";
				array += JSON_MSG;
				array += "\": \"";
				array += jsonItem->message_;
				array += "\",";



				array += "\"";
				array += JSON_TIMESTAMP;
				array += "\": \"";
				array += jsonItem->timestamp_;
				array += "\",";



				array += "\"";
				array += JSON_SOURCE;
				array += "\": \"";
				array += jsonItem->source_;
				array += "\",";



				array += "\"";
				array += JSON_SYSID;
				array += "\": \"";
				array += jsonItem->systemid_;
				array += "\",";



				array += "\"";
				array += JSON_SUBSYSID;
				array += "\": \"";
				array += jsonItem->subsystemid_;
				array += "\",";



				array += "\"";
				array += JSON_EBUFF;
				array += "\": \"";
				array += jsonItem->extraBuffer_;
				array += "\",";



				array += "\"";
				array += JSON_MACHINE;
				array += "\": \"";
				array += jsonItem->machine_;
				array += "\",";



				array += "\"";
				array += JSON_PORT;
				array += "\": \"";
				array += jsonItem->port_;
				array += "\",";



				array += "\"";
				array += JSON_PROCNAME;
				array += "\": \"";
				array += jsonItem->procname_;
				array += "\",";


				array += "\"";
				array += JSON_LOGNUMBER;
				array += "\": \"";
				array += jsonItem->lognumber_;
				//std::cout << "Sending LogNumber value " << jsonItem->lognumber_ << " in JSON array" << std::endl;
				array += "\",";


				array += "\"";
				array += JSON_FECHARDID;
				array += "\": \"";
				array += jsonItem->fecHardId_;
				array += "\",";


				array += "\"";
				array += JSON_RING;
				array += "\": \"";
				array += jsonItem->ring_;
				array += "\",";



				array += "\"";
				array += JSON_CCU;
				array += "\": \"";
				array += jsonItem->ccu_;
				array += "\",";


				array += "\"";
				array += JSON_I2CCHANNEL;
				array += "\": \"";
				array += jsonItem->i2cChannel_;
				array += "\",";


				array += "\"";
				array += JSON_I2CADDRESS;
				array += "\": \"";
				array += jsonItem->i2cAddress_;
				array += "\",";


				array += "\"";
				array += JSON_FEDID;
				array += "\": \"";
				array += jsonItem->fedId_;
				array += "\",";


				array += "\"";
				array += JSON_FEDCHANNEL;
				array += "\": \"";
				array += jsonItem->fedChannel_;
				array += "\",";


				array += "\"";
				array += JSON_CRATE;
				array += "\": \"";
				array += jsonItem->crate_;
				array += "\",";


				array += "\"";
				array += JSON_SLOT;
				array += "\": \"";
				array += jsonItem->slot_;
				array += "\",";







				array += "\"";
				array += JSON_NESTEDCLASSPATH;
				array += "\": \"";
				array += jsonItem->nestedclasspath_;
				array += "\",";


				array += "\"";
				array += JSON_NESTEDFILENAME;
				array += "\": \"";
				array += jsonItem->nestedfilename_;
				array += "\",";


				array += "\"";
				array += JSON_NESTEDLINENUMBER;
				array += "\": \"";
				array += jsonItem->nestedlinenumber_;
				array += "\",";

				array += "\"";
				array += JSON_PARTITIONNAME;
				array += "\": \"";
				array += jsonItem->partitionname_;
				array += "\",";



				array += "\"";
				array += "APP_REG_ERROR";
				array += "\": \"";
				array += "FALSE";
				array += "\",";


				array += "\"";
				array += "LOGFILENAME";
				array += "\": \"";
				array += jsonItem->logfilename_;
				array += "\",";



				array += "\"";
				array += "GEDSPECS";
				array += "\": \"";
				array += jsonItem->gedspecs_;
				array += "\",";

				array += "\"";
				array += "AJAXSPECS";
				array += "\": \"";
				array += jsonItem->ajaxspecs_;
				array += "\",";


				array += "\"";
				array += "ISONLINE";
				array += "\": \"";
				array += jsonItem->isonline_;
				array += "\",";



				array += "\"";
				array += JSON_PROCINSTANCE;
				array += "\": \"";
				array += jsonItem->procinstance_;
				array += "\"}";


				jsonItemsList->pop_back();
				
				if (jsonItemsList->empty() == false)
				{
					array += ",";
				}

			}
			array += "]";
//std::cout << "ARRAY = " << array << std::endl;
return array;
}








std::string JSONUtils::craftJsJsonArray(std::vector<JSONItem>*  jsonItemsList)
{
std::string array="";



			array += "{Logs:[";

			while (jsonItemsList->empty() == false)
			{
				JSONItem* jsonItem = (JSONItem*)&(*(jsonItemsList->end() - 1));


				array += "{\"";
				array += JSON_LEVEL;
				array += "\": \"";
				array += jsonItem->level_;
				array += "\",";



				array += "\"";
				array += JSON_ERRCODE;
				array += "\": \"";
				array += jsonItem->errcode_;
				array += "\",";



				array += "\"";
				array += JSON_MSG;
				array += "\": \"";
				array += jsonItem->message_;
				array += "\",";



				array += "\"";
				array += JSON_TIMESTAMP;
				array += "\": \"";
				array += jsonItem->timestamp_;
				array += "\",";



				array += "\"";
				array += JSON_SOURCE;
				array += "\": \"";
				array += jsonItem->source_;
				array += "\",";



				array += "\"";
				array += JSON_SYSID;
				array += "\": \"";
				array += jsonItem->systemid_;
				array += "\",";



				array += "\"";
				array += JSON_SUBSYSID;
				array += "\": \"";
				array += jsonItem->subsystemid_;
				array += "\",";



				array += "\"";
				array += JSON_EBUFF;
				array += "\": \"";
				array += jsonItem->extraBuffer_;
				array += "\",";



				array += "\"";
				array += JSON_MACHINE;
				array += "\": \"";
				array += jsonItem->machine_;
				array += "\",";				array += "\"";
				array += "AJAXSPECS";
				array += "\": \"";
				array += jsonItem->gedspecs_;
				array += "\",";




				array += "\"";
				array += JSON_PORT;
				array += "\": \"";
				array += jsonItem->port_;
				array += "\",";



				array += "\"";
				array += JSON_PROCNAME;
				array += "\": \"";
				array += jsonItem->procname_;
				array += "\",";


				array += "\"";
				array += JSON_LOGNUMBER;
				array += "\": \"";
				array += jsonItem->lognumber_;
				//std::cout << "Sending LogNumber value " << jsonItem->lognumber_ << " in JSON array" << std::endl;
				array += "\",";


				array += "\"";
				array += JSON_FECHARDID;
				array += "\": \"";
				array += jsonItem->fecHardId_;
				array += "\",";


				array += "\"";
				array += JSON_RING;
				array += "\": \"";
				array += jsonItem->ring_;
				array += "\",";



				array += "\"";
				array += JSON_CCU;
				array += "\": \"";
				array += jsonItem->ccu_;
				array += "\",";


				array += "\"";
				array += JSON_I2CCHANNEL;
				array += "\": \"";
				array += jsonItem->i2cChannel_;
				array += "\",";


				array += "\"";
				array += JSON_I2CADDRESS;
				array += "\": \"";
				array += jsonItem->i2cAddress_;
				array += "\",";


				array += "\"";
				array += JSON_FEDID;
				array += "\": \"";
				array += jsonItem->fedId_;
				array += "\",";


				array += "\"";
				array += JSON_FEDCHANNEL;
				array += "\": \"";
				array += jsonItem->fedChannel_;
				array += "\",";


				array += "\"";
				array += JSON_CRATE;
				array += "\": \"";
				array += jsonItem->crate_;
				array += "\",";


				array += "\"";
				array += JSON_SLOT;
				array += "\": \"";
				array += jsonItem->slot_;
				array += "\",";







				array += "\"";
				array += JSON_NESTEDCLASSPATH;
				array += "\": \"";
				array += jsonItem->nestedclasspath_;
				array += "\",";


				array += "\"";
				array += JSON_NESTEDFILENAME;
				array += "\": \"";
				array += jsonItem->nestedfilename_;
				array += "\",";


				array += "\"";
				array += JSON_NESTEDLINENUMBER;
				array += "\": \"";
				array += jsonItem->nestedlinenumber_;
				array += "\",";


				array += "\"";
				array += JSON_PARTITIONNAME;
				array += "\": \"";
				array += jsonItem->partitionname_;
				array += "\",";



				array += "\"";
				array += "APP_REG_ERROR";
				array += "\": \"";
				array += "FALSE";
				array += "\",";


				array += "\"";
				array += "LOGFILENAME";
				array += "\": \"";
				array += jsonItem->logfilename_;
				array += "\",";

				array += "\"";
				array += "GEDSPECS";
				array += "\": \"";
				array += jsonItem->gedspecs_;
				array += "\",";

				array += "\"";
				array += "AJAXSPECS";
				array += "\": \"";
				array += jsonItem->ajaxspecs_;
				array += "\",";

				array += "\"";
				array += "ISONLINE";
				array += "\": \"";
				array += jsonItem->isonline_;
				array += "\",";



				array += "\"";
				array += JSON_PROCINSTANCE;
				array += "\": \"";
				array += jsonItem->procinstance_;
				array += "\"}";


				jsonItemsList->pop_back();
				
				if (jsonItemsList->empty() == false)
				{
					array += ",";
				}

			}
			array += "]";
			array += "}";

//std::cout << "JSARRAY = " << array << std::endl;

return array;
}





std::string JSONUtils::craftJsonBadAppletArray()
{
	std::string array="";
	array += "[";




				array += "{\"";
				array += JSON_LEVEL;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_ERRCODE;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_MSG;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_TIMESTAMP;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_SOURCE;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_SYSID;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_SUBSYSID;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_EBUFF;
				array += "\": \"";
				array += "";
				array += "\",";




				array += "\"";
				array += JSON_MACHINE;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_PORT;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_PROCNAME;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_LOGNUMBER;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_FECHARDID;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_RING;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_CCU;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_I2CCHANNEL;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_I2CADDRESS;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_FEDID;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_FEDCHANNEL;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_CRATE;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_SLOT;
				array += "\": \"";
				array += "";
				array += "\",";




				array += "\"";
				array += JSON_NESTEDCLASSPATH;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_NESTEDFILENAME;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_NESTEDLINENUMBER;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_PARTITIONNAME;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += "LOGFILENAME";
				array += "\": \"";
				array += "unassigned";
				array += "\",";


				array += "\"";
				array += "APP_REG_ERROR";
				array += "\": \"";
				array += "TRUE";
				array += "\",";



				array += "\"";
				array += JSON_PROCINSTANCE;
				array += "\": \"";
				array += "";
				array += "\"}";









	array += "]";
return array;
}


std::string JSONUtils::craftJsJsonBadAppletArray()
{
	std::string array="";
	array += "{Logs:[";
	


				array += "{\"";
				array += JSON_LEVEL;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_ERRCODE;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_MSG;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_TIMESTAMP;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_SOURCE;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_SYSID;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_SUBSYSID;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_EBUFF;
				array += "\": \"";
				array += "";
				array += "\",";




				array += "\"";
				array += JSON_MACHINE;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_PORT;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_PROCNAME;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_LOGNUMBER;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_FECHARDID;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_RING;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_CCU;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_I2CCHANNEL;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_I2CADDRESS;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_FEDID;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_FEDCHANNEL;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_CRATE;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_SLOT;
				array += "\": \"";
				array += "";
				array += "\",";




				array += "\"";
				array += JSON_NESTEDCLASSPATH;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_NESTEDFILENAME;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += JSON_NESTEDLINENUMBER;
				array += "\": \"";
				array += "";
				array += "\",";



				array += "\"";
				array += JSON_PARTITIONNAME;
				array += "\": \"";
				array += "";
				array += "\",";


				array += "\"";
				array += "LOGFILENAME";
				array += "\": \"";
				array += "unassigned";
				array += "\",";


				array += "\"";
				array += "APP_REG_ERROR";
				array += "\": \"";
				array += "TRUE";
				array += "\",";



				array += "\"";
				array += JSON_PROCINSTANCE;
				array += "\": \"";
				array += "";
				array += "\"}";





	array += "]";
	array += "}";
return array;
}



















std::string JSONUtils::craftJsonFileParsingArray(std::vector<JSONFileItem>*  jsonFileItemsList)
{
std::string array="";



			array += "[";

			while (jsonFileItemsList->empty() == false)
			{
				JSONFileItem* jsonFileItem = (JSONFileItem*)&(*(jsonFileItemsList->end() - 1));


				array += "{\"";
				array += JSON_TYPEOFCONTENT;
				array += "\": \"";
				array += jsonFileItem->typeOfContent_;
				array += "\",";



				array += "\"";
				array += "FILE_LISTING_ERROR";
				array += "\": \"";
				array += "FALSE";
				array += "\",";


				array += "\"";
				array += JSON_CONTENTVALUE;
				array += "\": \"";
				array += jsonFileItem->contentValue_;
				array += "\"}";




				jsonFileItemsList->pop_back();
				
				if (jsonFileItemsList->empty() == false)
				{
					array += ",";
				}

			}
			array += "]";
//std::cout << "ARRAY = " << array << std::endl;
return array;
}








std::string JSONUtils::craftJsJsonFileParsingArray(std::vector<JSONFileItem>*  jsonFileItemsList)
{
std::string array="";



			array += "{Files:[";

			while (jsonFileItemsList->empty() == false)
			{
				JSONFileItem* jsonFileItem = (JSONFileItem*)&(*(jsonFileItemsList->end() - 1));


				array += "{\"";
				array += JSON_TYPEOFCONTENT;
				array += "\": \"";
				array += jsonFileItem->typeOfContent_;
				array += "\",";



				array += "\"";
				array += "FILE_LISTING_ERROR";
				array += "\": \"";
				array += "FALSE";
				array += "\",";

				array += "\"";
				array += JSON_CONTENTVALUE;
				array += "\": \"";
				array += jsonFileItem->contentValue_;
				array += "\"}";


				jsonFileItemsList->pop_back();
				
				if (jsonFileItemsList->empty() == false)
				{
					array += ",";
				}

			}
			array += "]";
			array += "}";

//std::cout << "JSARRAY = " << array << std::endl;

return array;
}










std::string JSONUtils::craftJsonBadFileParsingArray(std::vector<JSONFileItem>*  jsonFileItemsList)
{
std::string array="";



			array += "[";

			while (jsonFileItemsList->empty() == false)
			{
				JSONFileItem* jsonFileItem = (JSONFileItem*)&(*(jsonFileItemsList->end() - 1));


				array += "{\"";
				array += JSON_TYPEOFCONTENT;
				array += "\": \"";
				array += jsonFileItem->typeOfContent_;
				array += "\",";



				array += "\"";
				array += "FILE_LISTING_ERROR";
				array += "\": \"";
				array += "TRUE";
				array += "\",";


				array += "\"";
				array += JSON_CONTENTVALUE;
				array += "\": \"";
				array += jsonFileItem->contentValue_;
				array += "\"}";




				jsonFileItemsList->pop_back();
				
				if (jsonFileItemsList->empty() == false)
				{
					array += ",";
				}

			}
			array += "]";
//std::cout << "ARRAY = " << array << std::endl;
return array;
}








std::string JSONUtils::craftJsJsonBadFileParsingArray(std::vector<JSONFileItem>*  jsonFileItemsList)
{
std::string array="";



			array += "{Files:[";

			while (jsonFileItemsList->empty() == false)
			{
				JSONFileItem* jsonFileItem = (JSONFileItem*)&(*(jsonFileItemsList->end() - 1));


				array += "{\"";
				array += JSON_TYPEOFCONTENT;
				array += "\": \"";
				array += jsonFileItem->typeOfContent_;
				array += "\",";



				array += "\"";
				array += "FILE_LISTING_ERROR";
				array += "\": \"";
				array += "TRUE";
				array += "\",";

				array += "\"";
				array += JSON_CONTENTVALUE;
				array += "\": \"";
				array += jsonFileItem->contentValue_;
				array += "\"}";


				jsonFileItemsList->pop_back();
				
				if (jsonFileItemsList->empty() == false)
				{
					array += ",";
				}

			}
			array += "]";
			array += "}";

//std::cout << "JSARRAY = " << array << std::endl;

return array;
}














