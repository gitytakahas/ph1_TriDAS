#ifndef __COMMANDLINEOPTIONS_H
#define __COMMANDLINEOPTIONS_H

struct CommandLineOptionCode 
  {
  enum 
    {
      HELP = 1,
      DOWNLOAD_DESCRIPTION = 2,
      DOWNLOAD_HISTORY = 3,
      ANALYSIS_TYPE = 4,
      PARTITION = 5,
      RUNNUMBER = 6, 
      VERSION = 7,
      GET_ANALYSIS_TYPE = 8,
      INPUT = 9,
      OUTPUT = 10,
      PRINTED_ROWS_NUMBER = 11,
      DOWNLOAD_ANALYSISVERSIONS = 12
    };
  };

#endif
