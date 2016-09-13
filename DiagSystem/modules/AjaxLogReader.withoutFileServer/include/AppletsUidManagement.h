#ifndef _AppletsUidManagement_h_
#define _AppletsUidManagement_h_

#include <sys/time.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>


#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdio.h>

#include <string.h>

class AppletsUidManagement
{
public:

	FILE * sourceFile_;
	bool sourceFileIsOpen_;
	std::string appletUID_;
	int appletlogsBurstLength_;
	int selectedLogsReadFromFile_;

	long filePos_;
	unsigned long logcounter_;


	int limitReadToLastNLogs_;


	AppletsUidManagement();
	~AppletsUidManagement();

	void defineAppletUid();
	std::string getAppletUid();
};


#endif
