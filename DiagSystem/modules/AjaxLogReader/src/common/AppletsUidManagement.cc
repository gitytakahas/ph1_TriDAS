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

	//currentLogFileName_ = "/opt/trackerDAQ/config/AjaxLogReaderWithFileParserInfos.xml";
	currentLogFileName_ = "";
	
//	std::cout << "In AppletsUidManagement ; instanciating currentLogFileName_ as /opt/trackerDAQ/config/AjaxLogReaderWithFileParserInfos.xml" << std::endl;
	previousLogFileName_ = "VOID";
	logFileNameHasChanged_ = true;
	isWorkingWithOnlineFile_ = true;


	setAppletRepository_ = false;
	setAppletLogFile_ = false;
	toggleFileChange_ = false;

	gedSpecs_ = "not using GED";
	hasJustBeenInstanciated_ = true;
	ajaxspecs_ = "";

currentRepositoryName_ = "/home/lgross";
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





void AppletsUidManagement::checkAndSetIfLogFileHasChanged()
{
	FILE * fileOut;
	if (currentLogFileName_ != previousLogFileName_)
	{
		logFileNameHasChanged_ = true;
		fileOut = fopen(currentLogFileName_.c_str(), "r");
		
		if (fileOut == NULL)
		{
			std::stringstream errMsg;				
			errMsg << "Error : Unable to open log file \"" << currentLogFileName_.c_str() << "\" in read mode. Maybe a permissions problem?" << std::endl;
			std::cout << errMsg.str() << std::endl;
		}
		else
		{
			fclose(fileOut);
		}
		
		previousLogFileName_ = currentLogFileName_;
	}
	else logFileNameHasChanged_ = false;
}

















