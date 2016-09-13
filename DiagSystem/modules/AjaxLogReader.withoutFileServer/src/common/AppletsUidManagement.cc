#include "AppletsUidManagement.h"

#define NOT_EOF !(feof(fileOut_))


AppletsUidManagement::AppletsUidManagement()
{
	sourceFile_ = NULL;
	selectedLogsReadFromFile_ = 0;
	sourceFileIsOpen_ = false;
	filePos_ = 0;
	logcounter_ = 0;
	limitReadToLastNLogs_ = -1;
}



AppletsUidManagement::~AppletsUidManagement()
{
}



void AppletsUidManagement::defineAppletUid()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);

	unsigned long resu1, resu2;
	resu1 = tv.tv_sec;
	resu2 = tv.tv_usec/1000;
	char s1[100];
	sprintf(s1,"%lu",resu1);
	char s2[100];
	sprintf(s2,"%.3lu",resu2);
	char s3[100];
	strcpy(s3,s1);
	strcat(s3,s2);

	appletUID_ = (std::string)s3;
	//std::cout << "Applet UID defined to : " << appletUID_ << std::endl;
}



std::string AppletsUidManagement::getAppletUid()
{
	return appletUID_;
}


