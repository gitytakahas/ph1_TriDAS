#define DIAG_WILL_COMPILE_FOR_PIXELS_yes

#ifdef DIAG_WILL_COMPILE_FOR_PIXELS_yes
	//Declare returned string headers here
	#define REPORT_ERROR_RETURN_TYPE	std::string
	#define REPORT_ERROR_RETURN_COMMAND	return(msg);

#else
	//Declare returned void headers here
	#define REPORT_ERROR_RETURN_TYPE	void
	#define REPORT_ERROR_RETURN_COMMAND	//return(msg);

#endif
