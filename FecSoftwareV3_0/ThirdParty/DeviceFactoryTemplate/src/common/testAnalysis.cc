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


#include <iostream>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <string>
#include <stdio.h>
#include <vector>
#include <map>


#include "FecExceptionHandler.h" // exceptions
#include "DeviceFactory.h"       // devicefactory
#include "MemBufOutputSource.h"  // XML buffer


#include "CommissioningAnalysisDescription.h"
#include "CommissioningAnalysisFactory.h"

#include "ApvLatencyAnalysisDescription.h"
#include "CalibrationAnalysisDescription.h"
#include "FastFedCablingAnalysisDescription.h"
#include "FineDelayAnalysisDescription.h"
#include "OptoScanAnalysisDescription.h"
#include "PedestalsAnalysisDescription.h"
#include "TimingAnalysisDescription.h"
#include "VpspScanAnalysisDescription.h"

#include "CommandLineParameterSet.h"



int main(int argc, char **argv) {

	CommandLineParameterSet options(argc,argv);

	int returnVal=EXIT_SUCCESS;

	//size_t max_displayed_descriptions = 500;
	DeviceFactory *deviceFactory = NULL;
	DeviceFactory *deviceFactoryUpload = NULL;
	
	std::string login="nil", passwd="nil", path="nil";


	// if database access needed:
	if (	   ( options.getInput() == CommandLineParameterSet::SRC_DATABASE )
		|| ( options.getOutput() == CommandLineParameterSet::SRC_DATABASE ) ) {

		// retrieve the connection of the database through an environmental variable called CONFDB
		DbAccess::getDbConfiguration (login, passwd, path);
	
		if (login == "nil" || passwd=="nil" || path=="nil") {
			std::cerr << "ERROR: Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl;
			return 2;
		}

		// create connection:
		try {
			unsigned long startMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
			// create the database access
			deviceFactory = new DeviceFactory ( login, passwd, path );
			unsigned long endMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
			std::cout << "> Creation of the DeviceFactory " << (endMillis-startMillis) << " ms" << std::endl ;
		} catch (oracle::occi::SQLException &e) {
			std::cerr << "ERROR: Cannot create the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
			std::cerr << e.what() << std::endl ;
			return -1;
		}

		// be sure that xml file source is default (database connection is kept in memory)
		if ( options.getInput() == CommandLineParameterSet::SRC_XMLFILE ) {
			deviceFactory->setUsingFile();
		}


	} else {
		// no database needed:
		deviceFactory = new DeviceFactory();
	}





	/*******************************************/
	/* Perform test                            */
	/*******************************************/
	try {

		unsigned long startUploadMillis = 0;
		unsigned long endUploadMillis = 0;
		unsigned long startDownloadMillis = 0;
		unsigned long endDownloadMillis = 0;

		if ( options.getDownloadDescription() ) {
	
			std::vector<CommissioningAnalysisDescription*> desc;

			startDownloadMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

			if ( options.getInputFile() != "" ) {
			std::cout << "> Case I.0: download from file" << std::endl;
				//deviceFactory->setCommissioningAnalysisInputFileName( options.getInputFile() );
				desc = deviceFactory->getDescriptions(options.getInputFile());
				//deviceFactory->addCommissioningAnalysisInputFileName( options.getInputFile() );
std::cout << "Back to testAnalysis.cc" << std::endl;
			} else {

				if ( options.isSet(CommandLineOptionCode::PARTITION)
					&& options.isSet(CommandLineOptionCode::RUNNUMBER)
					&& options.isSet(CommandLineOptionCode::ANALYSIS_TYPE) ) {
	
			std::cout << "> Case I.1: run, partition and type specified (analysis result of a given run)"  << std::endl;
					desc = deviceFactory->getCalibrationData(options.getRun(), options.getPartition(), options.getAnalysisType() );
	
				} else {
	
			std::cout << "> Case I.2: partition, version and type specified (analysis result associated to a given partition)" << std::endl;
					desc = deviceFactory->getAnalysisHistory(options.getPartition(), options.getMajorVersion(), options.getMinorVersion(), options.getAnalysisType() );
	
				}

			}

			endDownloadMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();



			if ( options.isSet(CommandLineOptionCode::OUTPUT) && desc.size() > 0 ) {

		std::cout << " . . Preparing upload: " << desc.size() << std::endl;

				if ( options.getOutput() == CommandLineParameterSet::SRC_DATABASE ) {
					deviceFactoryUpload = new DeviceFactory( login, passwd, path );
					deviceFactoryUpload->setUsingDb();
				} else {
					deviceFactoryUpload = new DeviceFactory();
					deviceFactoryUpload->setOutputFileName( options.getOutputFile() );
				}

		std::cout << " . . . a) Upload without update current state:" << std::endl;
				CommissioningAnalysisDescription  *c = desc[0];
				startUploadMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

		std::cout << " . . . . Uploading descriptions..." << std::endl;
				deviceFactoryUpload->uploadAnalysis(
							c->getRunNumber(),
							c->getPartition(),
							options.getAnalysisType(),
							desc,
							false );
		std::cout << " . . . . Descriptions uploaded" << std::endl;
				endUploadMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

		std::cout << " . . . b) Upload with update current state option:" << std::endl;
				startUploadMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

		std::cout << " . . . . Uploading descriptions..." << std::endl;
				uint32_t newVer = deviceFactoryUpload->uploadAnalysis(
							c->getRunNumber(),
							c->getPartition(),
							options.getAnalysisType(),
							desc,
							true );
		std::cout << " . . . . Descriptions uploaded" << std::endl;
		std::cout << " . . . . Update current state: " << newVer << std::endl;
				deviceFactoryUpload->uploadAnalysisState(newVer);
		std::cout << " . . . . Current state updated" << std::endl;
				endUploadMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
				
			}



			for (size_t i = 0, size = desc.size(); i < size; ++i) {
				if ( i >= options.getNumberToBePrinted() ) {
					std::cout << "........................" << std::endl;
					std::cout << "WARNING: vector of descriptions is too large to be entirely displayed" << std::endl;
					break;
				}
				CommissioningAnalysisDescription  *c = desc[i];
				c->display();
			}

			std::cout << desc.size() << " descriptions found" << std::endl;

			std::cout << " Test the copyVector() method:" << std::endl;
			std::vector<CommissioningAnalysisDescription*> copy;
			CommissioningAnalysisFactory::vectorCopy(copy, desc);
			std::cout << " Done" << std::endl;
			std::cout << " Release pointers...." << std::endl;
			for (size_t i = 0, size = copy.size(); i < size; ++i) {
				CommissioningAnalysisDescription  *c = copy[i];
				delete c;
			}
			std::cout << " Done" << std::endl;

		} else if ( options.getDownloadHistory() ) {

		std::cout << "> Case II.1: retrieve all analysis result versions (partition and analysis type)"  << std::endl;

			HashMapRunVersion history;

			startDownloadMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
			history = deviceFactory->getAnalysisHistory(options.getPartition(), options.getAnalysisType());
			endDownloadMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();

			for ( HashMapRunVersion::iterator iter = history.begin(); iter != history.end(); iter++ ) {
				unsigned int _run_ = (*iter).first;
				std::vector< std::pair<unsigned int, unsigned int> > _versions_ = (*iter).second;
				for ( size_t i = 0; i<_versions_.size(); ++i ) {
					std::pair<unsigned int, unsigned int> _version_ = _versions_[i];
					std::cout << " . run: " << _run_ << " -> " << _version_.first << "." << _version_.second <<  std::endl;
				}
			}
		} else if ( options.getDownloadVersions() ) {

		std::cout << "> Case III.1: retrieve local analysis versions (analysis type, version) by global version"  << std::endl;

			HashMapAnalysisVersions versions;
			
			startDownloadMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
			versions = deviceFactory->getLocalAnalysisVersions(options.getGlobalVersion());
			endDownloadMillis = XERCES_CPP_NAMESPACE::XMLPlatformUtils::getCurrentMillis();
			
			for ( HashMapAnalysisVersions::iterator iter = versions.begin(); iter != versions.end(); iter++ ) {
				unsigned int _analType_ = (*iter).first;
				std::pair<unsigned int, unsigned int> _version_ = (*iter).second;
				std::cout << " . Type: " << CommissioningAnalysisDescription::getAnalysisType((CommissioningAnalysisDescription::commissioningType)_analType_) << "[" << _analType_ << "] -> " << _version_.first << "." << _version_.second <<  std::endl;
			}
	
		}

		std::cout << "> Data retrieval took: " << (endDownloadMillis-startDownloadMillis) << " ms" << std::endl;
		if ( options.isSet(CommandLineOptionCode::OUTPUT) )
			std::cout << "> Data upload took: " << (endUploadMillis-startUploadMillis) << " ms" << std::endl;

	}
	catch (FecExceptionHandler &e) {
		std::cerr << "ERROR: Error during the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
		std::cerr << e.what() << std::endl;
		returnVal=127;
	}
	catch (oracle::occi::SQLException &e) {
		std::cerr << "ERROR: Oracle error during the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
		std::cerr << e.what() << std::endl ;
		returnVal=126;
	}


	// ***************************************************************************************************************************
	// Delete the database access
	// ***************************************************************************************************************************
	try {     
		// this method handle the remove of the vectors
		delete deviceFactory;
	}
	catch (oracle::occi::SQLException &e) {
		std::cerr << "ERROR: Cannot close the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
		std::cerr << e.what() << std::endl;
		returnVal=125;
	}
	try {     
		// this method handle the remove of the vectors
		if ( deviceFactoryUpload != NULL ) delete deviceFactoryUpload;
	}
	catch (oracle::occi::SQLException &e) {
		std::cerr << "ERROR: Cannot close the access to the database: " << login << "/" << passwd << "@" << path << std::endl ;
		std::cerr << e.what() << std::endl;
		returnVal=124;
	}

	return returnVal;
}
