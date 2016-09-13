#ifndef _JSONUtils_h_
#define _JSONUtils_h_



#include "DiagCompileOptions.h"
#include <diagbag/DiagBagWizard.h>
#include <diagbag/JSONItem.h>
#include <diagbag/JSONFileItem.h>



class JSONUtils
{
public:



	JSONUtils(void);

	~JSONUtils(void);

	std::string craftJsonArray(std::vector<JSONItem> *);
	std::string craftJsJsonArray(std::vector<JSONItem> *);

	std::string craftJsonBadAppletArray();
	std::string craftJsJsonBadAppletArray();



	std::string craftJsonFileParsingArray(std::vector<JSONFileItem> *);
	std::string craftJsJsonFileParsingArray(std::vector<JSONFileItem> *);

	std::string craftJsonBadFileParsingArray(std::vector<JSONFileItem> *);
	std::string craftJsJsonBadFileParsingArray(std::vector<JSONFileItem> *);

};

#endif

