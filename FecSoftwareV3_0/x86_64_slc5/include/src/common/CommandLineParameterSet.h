#ifndef COMMANDLINEPARAMETERSET_H_
#define COMMANDLINEPARAMETERSET_H_

#include <vector>
#include <string>
#include <algorithm>
#include "CommissioningAnalysisDescription.h"
#include "CommandLineOptions.h"


class CommandLineParameterSet {

  public:
	typedef enum { SRC_UNKNOWN, SRC_DATABASE, SRC_XMLFILE } dataSourceType;

  private:
	// commands:
	bool             _help;
	bool             _download_description;
	bool             _download_history;
	bool             _download_versions;
	// options & parameters:
	bool             _upload;
	dataSourceType   _input;
	dataSourceType   _output;
	std::string      _input_file; // file name
	std::string      _output_file;// file name

	std::string      _partition;
	unsigned int     _run;
	unsigned int     _minor_version;
	unsigned int     _major_version;
	
	unsigned int     _global_version;
	
	unsigned int     _printed_rows_number;

	std::vector<int> _options;

	CommissioningAnalysisDescription::commissioningType _analysis_type;

	// internal methods:
	void _hasMoreArguments(int argc, char* argv[], int current, std::string msg);
	std::string _analysisType( std::string left, std::string right );
	void _printHelp(char* argv[]);
	void _setOption(const int opt ) { _options.push_back(opt); }

	void _init();


  public:
	// con(de)structors:
	CommandLineParameterSet();
	CommandLineParameterSet(int argc, char* argv[]);
	virtual ~CommandLineParameterSet() {}

	// public getters:
	// commands:
	bool getHelp()                 const { return _help; }
	bool getDownloadDescription()  const { return _download_description; }
	bool getDownloadHistory()      const { return _download_history; }
	bool getDownloadVersions()     const { return _download_versions; }

	// options & parameters:
	dataSourceType getInput()      const { return _input; }
	dataSourceType getOutput()     const { return _output; }
	std::string    getInputFile()  const { return _input_file; }
	std::string    getOutputFile()  const { return _output_file; }
	unsigned int   getNumberToBePrinted() const { return _printed_rows_number; }

	std::string  getPartition()    const { return _partition; }
	unsigned int getRun()          const { return _run; }
	unsigned int getMinorVersion() const { return _minor_version; }
	unsigned int getMajorVersion() const { return _major_version; }
	unsigned int getGlobalVersion() const { return _global_version; }


	CommissioningAnalysisDescription::commissioningType getAnalysisType() const { return _analysis_type; }
	void setAnalysisType(CommissioningAnalysisDescription::commissioningType type);

	// indicates wheter an options has been specified:
	bool isSet(int opt)            const { return std::find(_options.begin(),_options.end(),opt)!=_options.end(); }

};

#endif
