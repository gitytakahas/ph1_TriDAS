/***************************************************************************
 *   Copyright (C) 2008 by Sebastien BEL - CERN GENEVA                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "CommandLineParameterSet.h"
#include "CommandLineOptions.h"
#include "perfecthash.h"
#include <iostream>


/************************************************************/
CommandLineParameterSet::CommandLineParameterSet() {
	_init();
}

/************************************************************/
CommandLineParameterSet::CommandLineParameterSet(int argc, char* argv[]) {

std::cout << "Command line arguments start" << std::endl;

	_init();

	for (int i=1; i<argc; ++i) {

		std::string cmdLineOption = argv[i];
		const CommandLineOption* option = Perfect_Hash::IsValidCommandLineOption(cmdLineOption.c_str(), cmdLineOption.length());
		if(!option) {
			std::cerr << "ERROR: Unknown argument: " << argv[i] << std::endl;
			_printHelp(argv);
		}

		_setOption(option->OptionCode);

		switch (option->OptionCode) {
			case CommandLineOptionCode::HELP :
				_help = true;
				_printHelp(argv);
				break;
			case CommandLineOptionCode::DOWNLOAD_DESCRIPTION :
				_download_description = true;
std::cout << " . Download descriptions" << std::endl;
				break;
			case CommandLineOptionCode::DOWNLOAD_HISTORY :
std::cout << " . Download history" << std::endl;
				_download_history = true;
				break;
			case CommandLineOptionCode::ANALYSIS_TYPE :
			{	bool found=false;
				this->_hasMoreArguments(argc, argv, i, "ERROR: Analysis type is not specified!");
				i++;
				for ( int o=0; o<=CommissioningAnalysisDescription::T_ANALYSIS_VPSPSCAN && !found; o++ ) {
					if ( strcmp( CommissioningAnalysisDescription::COMMISSIONINGANALYSISTYPE[o], argv[i] ) == 0 ) {
						_analysis_type = (CommissioningAnalysisDescription::commissioningType)o;
						found=true;
					}
				}
				if ( !found ) {
					std::cerr << "ERROR: Unknown commissioning type: " << argv[i] << std::endl;
					_printHelp(argv);
				}
std::cout << " . Analysis type is specified: " << _analysis_type << " " << CommissioningAnalysisDescription::COMMISSIONINGANALYSISTYPE[_analysis_type] << std::endl;
				break;
			}
			case CommandLineOptionCode::PARTITION :
				this->_hasMoreArguments(argc, argv, i, "ERROR: Partition name is not specified!");
				_partition = argv[++i];
std::cout << " . Partition is specified    : " << _partition << std::endl;
				break;
			case CommandLineOptionCode::RUNNUMBER :
				this->_hasMoreArguments(argc, argv, i, "ERROR: Run number is not specified!");
				_run = atoi(argv[++i]);
std::cout << " . Run number is specified   : " << _run << std::endl;
				break;
			case CommandLineOptionCode::VERSION :
			{	this->_hasMoreArguments(argc, argv, i, "ERROR: Version is not specified!");
				std::string s = argv[++i];
				std::stringstream t;
				bool major=true;
				for ( size_t i=0; i<s.size(); ++i) {
					if ( s[i] == '.' ) {
						major=false;
						_major_version = atoi(t.rdbuf()->str().c_str());
						t.rdbuf()->str("");
					} else {
						t << s[i];
					}
				}
				_minor_version = atoi(t.rdbuf()->str().c_str());
std::cout << " . Version is specified      : " << _major_version << "." << _minor_version << std::endl;
				break;
			}
			case CommandLineOptionCode::GET_ANALYSIS_TYPE :
				std::cout << "List of supported analysis types:" << std::endl;
				std::cout << _analysisType(" . ", "\n");
				exit(0);
				break;
			case CommandLineOptionCode::INPUT :
			{	this->_hasMoreArguments(argc, argv, i, "ERROR: Input type is not specified!");
				std::string src = argv[++i];
				if ( "DATABASE" == src ) {
					this->_input = SRC_DATABASE;
				} else if ( "XMLFILE" == src ) {
					this->_input = SRC_XMLFILE;
					this->_hasMoreArguments(argc, argv, i, "ERROR: Input file name is not specified!");
					this->_input_file = argv[++i];
				} else {
					std::cerr << "ERROR: Unknown source type: " << argv[i] << std::endl;
					_printHelp(argv);
				}
std::cout << " . Input source is specified : " << src << " " << this->_input_file << std::endl;
				break;
			}
			case CommandLineOptionCode::OUTPUT :
			{	this->_hasMoreArguments(argc, argv, i, "ERROR: Output type is not specified!");
				std::string src = argv[++i];
				if ( "DATABASE" == src ) {
					this->_output = SRC_DATABASE;
				} else if ( "XMLFILE" == src ) {
					this->_output = SRC_XMLFILE;
					this->_hasMoreArguments(argc, argv, i, "ERROR: Output file name is not specified!");
					this->_output_file = argv[++i];
				} else {
					std::cerr << "ERROR: Unknown source type: " << argv[i] << std::endl;
					_printHelp(argv);
				}
std::cout << " . Output source is specified: " << src << " " << this->_output_file << std::endl;
				break;
			}
			case CommandLineOptionCode::PRINTED_ROWS_NUMBER :
			{	this->_hasMoreArguments(argc, argv, i, "ERROR: Number of descriptions to be printed is not specified!");
				std::string nb = argv[++i];
				if ( nb[0]=='-' ) {
					std::cerr << "ERROR: Missing the number of descriptions to be printed" << std::endl;
					_printHelp(argv);
				}
std::cout << " . Number of descriptions to be printed is specified: " << nb;
				this->_printed_rows_number = atoi(nb.c_str());
std::cout << "(" << this->_printed_rows_number << ")" << std::endl;
				break;
			}
			case CommandLineOptionCode::DOWNLOAD_ANALYSISVERSIONS :
			{	this->_hasMoreArguments(argc, argv, i, "ERROR: Global analysis version is not specified!");
				std::string globalVersion = argv[++i];
				if ( globalVersion[0]=='-' ) {
					std::cerr << "ERROR: Missing the global analysis version" << std::endl;
					_printHelp(argv);
				}
				this->_download_versions=true;
std::cout << " . Global analysis version is specified: " << globalVersion;
				this->_global_version = atoi(globalVersion.c_str());
std::cout << "(" << this->_global_version << ")" << std::endl;
				break;
			}
			default:
				std::cout << "WARNING: unmanaged, but nevertheless allowed, argument: " << argv[i] << std::endl;
				break;
		}
	}

	if ( _download_history && _download_description ) {
		std::cerr << "ERROR: Both calibration and history have been requested..." << std::endl;
		_printHelp(argv);
	}
	if ( ! (   isSet(CommandLineOptionCode::DOWNLOAD_DESCRIPTION)
		   || isSet(CommandLineOptionCode::DOWNLOAD_HISTORY)
		   || isSet(CommandLineOptionCode::HELP)
		   || isSet(CommandLineOptionCode::GET_ANALYSIS_TYPE)
		   || isSet(CommandLineOptionCode::DOWNLOAD_ANALYSISVERSIONS) ) ) {
		std::cerr << "ERROR: No command specified!" << std::endl;
		_printHelp(argv);
	}
	if ( isSet(CommandLineOptionCode::DOWNLOAD_DESCRIPTION) ) {
		if ( ! (   isSet(CommandLineOptionCode::PARTITION) 
				&& isSet(CommandLineOptionCode::RUNNUMBER)
				&& isSet(CommandLineOptionCode::ANALYSIS_TYPE) )
			&&
			 ! (   isSet(CommandLineOptionCode::PARTITION) 
				&& isSet(CommandLineOptionCode::ANALYSIS_TYPE)
				&& isSet(CommandLineOptionCode::VERSION) ) 
			&& (    this->_input_file == "" ) ) {

			std::cerr << "ERROR: Missing filter parameter or input file name for this kind of description retrieval" << std::endl;
			_printHelp(argv);
		}
	} else if ( isSet(CommandLineOptionCode::DOWNLOAD_HISTORY) ) {
		if ( ! (   isSet(CommandLineOptionCode::PARTITION) 
				&& isSet(CommandLineOptionCode::ANALYSIS_TYPE) ) ) {
			std::cerr << "ERROR: Missing filter parameter for history retrieval" << std::endl;
			_printHelp(argv);
		}
		if ( isSet(CommandLineOptionCode::OUTPUT) ) {
			std::cerr << "WARNING: Output option cannot be used for this command: (-o|--output) will be ignored" << std::endl;
			this->_output = SRC_UNKNOWN;
		}
	}
	if ( !isSet(CommandLineOptionCode::INPUT) ) {
		std::cerr << "ERROR: no input source specified!" << std::endl;
		_printHelp(argv);
	}
std::cout << "Command line arguments parsed" << std::endl;
}

/************************************************************/
void CommandLineParameterSet::_init() {
	_help=false;
	_download_description=false;
	_download_history=false;
	_upload=false;
	_download_versions=false;

	_input = SRC_UNKNOWN;
	_output = SRC_UNKNOWN;
	_input_file = "";
	_output_file = "";

	_analysis_type = CommissioningAnalysisDescription::T_UNKNOWN;
	_partition="";
	_run=0;
	_minor_version=0;
	_major_version=0;
	_global_version=0;

	_printed_rows_number=500;
}

/************************************************************/
void CommandLineParameterSet::_hasMoreArguments(int argc, char* argv[], int current, std::string msg) {
	if ( argc <= current+1 ) {
		std::cerr << msg << std::endl;
		_printHelp(argv);
	}
}

/************************************************************/
std::string CommandLineParameterSet::_analysisType( std::string left, std::string right ) {
	std::stringstream buf;
	for ( int o=0; o<=CommissioningAnalysisDescription::T_ANALYSIS_VPSPSCAN; o++ ) {
		buf << left << CommissioningAnalysisDescription::COMMISSIONINGANALYSISTYPE[o] << right;
	}
	return buf.str();
}

/************************************************************/
void CommandLineParameterSet::setAnalysisType( CommissioningAnalysisDescription::commissioningType type ) {
	this->_analysis_type = type;
}

/************************************************************/
void CommandLineParameterSet::_printHelp(char* argv[]) {
	std::cout << std::endl;
	std::cout << "Name:" << std::endl; 
	std::cout << "\t" << argv[0] << " - Tracker analysis result upload/download API" << std::endl;
	std::cout << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << "\t" << argv[0] << " [command] [parameters] [options]" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	// commands:
	std::cout << "Commands:" << std::endl;
	std::cout << "\t-dd, --download-description" << std::endl;
	std::cout << "\t\tget a vector of analysis descriptions, filter must be defined" << std::endl;
	std::cout << "\t\twith {-p, -v, -t} or {-p, -r, -t} options"  << std::endl;
	std::cout << std::endl;
	std::cout << "\t-dh, --download-history" << std::endl;
	std::cout << "\t\tretrieves all analysis result pair of <runnumber, version> of the specified" << std::endl;
	std::cout << "\t\tpartition (-p) and analysis type (-t)" << std::endl;
	std::cout << std::endl;
	std::cout << "\t-dv <value>, --download-versions <value>" << std::endl;
	std::cout << "\t\tretrieve local analysis type and associated version by global analysis version (value)" << std::endl;
	std::cout << std::endl;
	std::cout << "\t-gct, --get-analysis-type" << std::endl;
	std::cout << "\t\tprint all supported analysis types (no option needed)" << std::endl;
	std::cout << std::endl;
	std::cout << "\t-h, --help" << std::endl;
	std::cout << "\t\tprint this message" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	// parameters:
	std::cout << "Parameters:" << std::endl;
	std::cout << "\t-t <value>, --analysis-type <value>" << std::endl;
	std::cout << "\t\tgives the analysis type (value), possible values are: " << std::endl;
	std::cout << "\t\t" << _analysisType("", ",") << std::endl;
	std::cout << std::endl;
	std::cout << "\t-p <value>, --partition <value>" << std::endl;
	std::cout << "\t\tgives the partition name (value)" << std::endl;
	std::cout << std::endl;
	std::cout << "\t-r <value>, --run <value>" << std::endl;
	std::cout << "\t\tgives the run number (value)" << std::endl;
	std::cout << std::endl;
	std::cout << "\t-v <value>, --version <value>" << std::endl;
	std::cout << "\t\tgives the analysis version (value)" << std::endl;
	std::cout << std::endl;
	std::cout << "\t-i <type> <filename>, --input <type> <filename>" << std::endl;
	std::cout << "\t\tdata source to be used where type={DATABASE|XMLFILE} and filename is the input file " << std::endl;
	std::cout << "\t\tto be dowloaded. This parameter is required with download commands" << std::endl;
	std::cout << std::endl;
	std::cout << std::endl;
	// options:
	std::cout << "Options:" << std::endl;
	std::cout << "\t-o <type> <filename>, --output <type> <filename>" << std::endl;
	std::cout << "\t\tdowloaded data (with command -dd) will be uploaded in a second step (copied in a new" << std::endl;
	std::cout << "\t\tversion or in a file). The data target to be used is defined by its type, where" << std::endl;
	std::cout << "\t\ttype={DATABASE|XMLFILE}. When type=XMLFILE, the output filename where descriptions" << std::endl;
	std::cout << "\t\twill be uploaded has to be set." << std::endl;
	
	std::cout << "\t-rn <number>, --printed_rows_number <number>" << std::endl;
	std::cout << "\t\tspecifies the number of descriptions to be printed" << std::endl;
	
	std::cout << std::endl;
	std::cout << std::endl;
  if ( _help )
  	exit(0);
  else
  	exit(1);
}

