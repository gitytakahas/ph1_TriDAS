#define DEBUG_FEC_LOGGER



#define LOG_FILES_ROOT	"/tmp/"
#define XML_LOG_FILES_EXTENSION	".xml"

#define XML_FEC_LOGFILE_NAME "logFecErrors.LclEd"
#define XML_FED_LOGFILE_NAME "logFedErrors.LclEd"
#define XML_OTHER_LOGFILE_NAME "logOtherErrors.LclEd"

#define REMOVE_COMMAND "/bin/rm -f "
#define MOVE_COMMAND "mv -f "



/*
#define DELETE_XML_FEC_LOGFILE_NAME "rm -f /tmp/logFecErrors.xml"
#define DELETE_XML_FED_LOGFILE_NAME "rm -f /tmp/logFedErrors.xml"
#define DELETE_XML_OTHER_LOGFILE_NAME "rm -f /tmp/logOtherErrors.xml"

#define MOVE_COMPLETED_XML_FEC_LOGFILE_NAME "mv /tmp/logFecErrors.xml /tmp/logFecErrors.completed.xml"
#define MOVE_COMPLETED_XML_FED_LOGFILE_NAME "mv /tmp/logFedErrors.xml /tmp/logFedErrors.completed.xml"
#define MOVE_COMPLETED_XML_OTHER_LOGFILE_NAME "mv /tmp/logOtherErrors.xml /tmp/logOtherErrors.completed.xml"

#define DELETE_COMPLETED_XML_FEC_LOGFILE_NAME "rm -f /tmp/logFecErrors.completed.xml"
#define DELETE_COMPLETED_XML_FED_LOGFILE_NAME "rm -f /tmp/logFedErrors.completed.xml"
#define DELETE_COMPLETED_XML_OTHER_LOGFILE_NAME "rm -f /tmp/logOtherErrors.completed.xml"
*/
/*
#define XML_FEC_LOGFILE_NAME "/tmp/logFecErrors.xml"
#define XML_FED_LOGFILE_NAME "/tmp/logFedErrors.xml"
#define XML_OTHER_LOGFILE_NAME "/tmp/logOtherErrors.xml"
*/
#define XML_VERSION "'1.0'"
#define XML_ENCODING "'UTF-8'"
#define XML_DOCTYPE_ROOT "[]"


#define HEADER_TYPE_EXTENSION_TABLE_NAME_FOR_FEC "SI_TRK_FEC_ERRORS"
#define HEADER_TYPE_EXTENSION_TABLE_NAME_FOR_FED "SI_TRK_FED_ERRORS"
#define HEADER_TYPE_EXTENSION_TABLE_NAME_FOR_OTHER "SI_TRK_OTHER_ERRORS"

#define HEADER_TYPE_NAME_FOR_FEC "Si Tracker Fec Errors"
#define HEADER_TYPE_NAME_FOR_FED "Si Tracker Fed Errors"
#define HEADER_TYPE_NAME_FOR_OTHER "Si Tracker Other Errors"

#define HEADER_RUN_RUN_TYPE "Si_Trk_Db_Errors_Testing"
#define HEADER_RUN_RUN_NUMBER "1"
#define HEADER_RUN_INITIATED_BY_USER "Laurent Gross"
#define HEADER_RUN_LOCATION "CERN"
#define HEADER_RUN_COMMENT_DESCRIPTION "Silicium Tracker - Testing XML file generation for Db Errors Logging"



#define COMMENT_DESCRIPTION "Testing compatibility of FEC/FED/OTHER errors XML pattern with CMS Errors DB for Silicium Tracker"

#define DATA_FEC_FILE_NAME "SI_TRK_FEC_ERRORS.xml"
#define DATA_FED_FILE_NAME "SI_TRK_FED_ERRORS.xml"
#define DATA_OTHER_FILE_NAME "SI_TRK_OTHER_ERRORS.xml"

#define ERRXML_VERSION "v_1_0_1"
#define PART_FEC_SERIAL_NUMBER "TRK_FEC_128_SERIAL"
#define PART_FED_SERIAL_NUMBER "TRK_FED_128_SERIAL"
#define PART_OTHER_SERIAL_NUMBER "TRK_OTHER_128_SERIAL"


#define TRK_FEC_NAME "SI_TRK_VME_FEC_BOARD"
#define TRK_FED_NAME "SI_TRK_VME_FED_BOARD"
#define TRK_OTHER_NAME "SI_TRK_VME_OTHER_BOARD"

#define VME_ADDR "3020140B9CCEC6"
#define CRATE_SLOT "34"
#define FEC_SLOT "4"
#define RING "6"
#define ERR_CODE "210"
#define ERR_MSG "Ceci est un message du FEC Supervisor"
#define FEC_STAT_REG0 "17480"
#define PROCESS_INSTANCE "6"


#define XML_WRITE_ACESS "w"
#define XML_READ_ACESS "r"
#define XML_APPEND_ACESS "a"

#define MAX_LOG_NUMBER_IN_FEC_XML_FILE		100000
#define MAX_LOG_NUMBER_IN_FED_XML_FILE		100000
#define MAX_LOG_NUMBER_IN_OTHER_XML_FILE	100000


#define MAX_NBR_OF_GENERAL_FILE_LOGS 	100000
