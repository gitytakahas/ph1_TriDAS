/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Fec Software; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2002 - 2003, Damien VINTACHE - IReS/IN2P3
  $Date: 2013/01/21 13:45:08 $
  $Revision: 1.3.34.1 $
  $Id: FEDTest.cc,v 1.3.34.1 2013/01/21 13:45:08 agiassi Exp $
  $Name:  $
*/


#include <vector>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include "DbFedAccess.h"
#include "Utils.h"



int main(int argC, char* argV[]) {
  time_t time0, time1;

  DbFedAccess *dbFedAccess = NULL; 

  try {
    dbFedAccess = new DbFedAccess();
    //    sleep(60);
    char *fedBuffer;
    char *ttcrxBuffer;
    char *voltageMonitorBuffer;
    char *feFpgaBuffer;
    char *channelPairBuffer;
    char *channelBuffer;
    char *apvFedBuffer;
    char *stripBuffer;
    int versionUpdate = 0;
    int i=1;
    int fedId = 0;
    int crateId = 0;
    oracle::occi::Clob *xmlClob = NULL;

    std::string *xmlBuffer = new std::string[10];

    std::string dirName, partitionName, mode;

    if (argC > 1){
      while (i<argC){
	if (!std::string(argV[i]).compare("-f")) {
 	  dirName = std::string(argV[i+1]) + "/";
	   	  std::cout << " using fileName = " << dirName << std::endl;
 	  i = i+2;
 	} else if (!std::string(argV[i]).compare("-partition")) {
 	  partitionName = std::string(argV[i+1]);
	   	  std::cout << " using partitionName = " << partitionName << std::endl;
 	  i = i+2;
 	} else if (!std::string(argV[i]).compare("-crateId")) {
 	  crateId = fromString<int>(std::string(argV[i+1]));
	   	  std::cout << " using crateId = " << fedId << std::endl;
 	  i = i+2;
 	} else if (!std::string(argV[i]).compare("-fedId")) {
 	  fedId = fromString<int>(std::string(argV[i+1]));
	   	  std::cout << " using fedId = " << fedId << std::endl;
 	  i = i+2;
 	} else if (!std::string(argV[i]).compare("-mode")) {
 	  mode = std::string(argV[i+1]);
	   	  std::cout << " using mode = " << mode << std::endl;
 	  i = i+2;
 	} else if (!std::string(argV[i]).compare("-versionUpdate")) {
 	  versionUpdate = fromString<int>(std::string(argV[i+1]));
	   	  std::cout << " using versionUpdate = " << versionUpdate << std::endl;
 	  i = i+2;
 	}
      }
    }


//     for (int k=0; k<8 ; k++){
//       std::cout << xmlBuffer[k].size() << std::endl;
//     }

    if (!mode.compare("upload")) {
      std::string xmlTestFed;
      std::string xmlTestTtcrx;
      std::string xmlTestVoltageMonitor;
      std::string xmlTestFeFpga;
      std::string xmlTestChannelPair;
      std::string xmlTestChannel;
      std::string xmlTestApvFed;
      std::string xmlTestStrip;

      if (fedId) {
	xmlTestFed = dirName + "newTestFed.xml" + toString(fedId);
	xmlTestTtcrx = dirName + "newTestTtcrx.xml" + toString(fedId);
	xmlTestVoltageMonitor = dirName + "newTestVoltageMonitor.xml" + toString(fedId);
	xmlTestFeFpga = dirName + "newTestFeFpga.xml" + toString(fedId);
	xmlTestChannelPair = dirName + "newTestChannelPair.xml" + toString(fedId);
	xmlTestChannel = dirName + "newTestChannel.xml" + toString(fedId);
	xmlTestApvFed = dirName + "newTestApvFed.xml" + toString(fedId);
	xmlTestStrip = dirName + "newTestStrip.xml" + toString(fedId);
      } else if (crateId){
	xmlTestFed = dirName + "newTestFed.xml" + partitionName + toString(crateId);
	xmlTestTtcrx = dirName + "newTestTtcrx.xml" + partitionName + toString(crateId);
	xmlTestVoltageMonitor = dirName + "newTestVoltageMonitor.xml" + partitionName + toString(crateId);
	xmlTestFeFpga = dirName + "newTestFeFpga.xml" + partitionName + toString(crateId);
	xmlTestChannelPair = dirName + "newTestChannelPair.xml" + partitionName + toString(crateId);
	xmlTestChannel = dirName + "newTestChannel.xml" + partitionName + toString(crateId);
	xmlTestApvFed = dirName + "newTestApvFed.xml" + partitionName + toString(crateId);
	xmlTestStrip = dirName + "newTestStrip.xml" + partitionName + toString(crateId);
      } else {
	xmlTestFed = dirName + "newTestFed.xml" + partitionName;
	xmlTestTtcrx = dirName + "newTestTtcrx.xml" + partitionName;
	xmlTestVoltageMonitor = dirName + "newTestVoltageMonitor.xml" + partitionName;
	xmlTestFeFpga = dirName + "newTestFeFpga.xml" + partitionName;
	xmlTestChannelPair = dirName + "newTestChannelPair.xml" + partitionName;
	xmlTestChannel = dirName + "newTestChannel.xml" + partitionName;
	xmlTestApvFed = dirName + "newTestApvFed.xml" + partitionName;
	xmlTestStrip = dirName + "newTestStrip.xml" + partitionName;
      }
      
      
      std::cout <<   xmlTestFed << std::endl;  
      Utils::init(xmlTestFed, &fedBuffer);
      Utils::init(xmlTestTtcrx, &ttcrxBuffer);
      Utils::init(xmlTestVoltageMonitor, &voltageMonitorBuffer);
      Utils::init(xmlTestFeFpga, &feFpgaBuffer);
      Utils::init(xmlTestChannelPair, &channelPairBuffer);
      Utils::init(xmlTestChannel, &channelBuffer);
      Utils::init(xmlTestApvFed, &apvFedBuffer);
      Utils::init(xmlTestStrip, &stripBuffer);
      
      xmlBuffer[0] = std::string((const char*)(fedBuffer));
      xmlBuffer[1] = std::string((const char*)(ttcrxBuffer));
      xmlBuffer[2] = std::string((const char*)(voltageMonitorBuffer));
      xmlBuffer[3] = std::string((const char*)(feFpgaBuffer));
      xmlBuffer[4] = std::string((const char*)(channelPairBuffer));
      xmlBuffer[5] = std::string((const char*)(channelBuffer));
      xmlBuffer[6] = std::string((const char*)(apvFedBuffer));
      xmlBuffer[7] = std::string((const char*)(stripBuffer));
   
      /*
       *   - nextMajorVersion.0 if versionUpdate = 1
       *   - currentMajorVersion.nextMinorVersion if versionUpdate = 0
       *   - currentMajorVersion.0 if versionUpdate = 2
       */
//       for (int j=0; j < 10; j++){
// 	std::cout << "\t j : " << j << "\t";
 	time(&time0);
	dbFedAccess->setXMLClob(xmlBuffer, partitionName, versionUpdate);
 	time(&time1);
	std::cout << "\t time : " << difftime(time1,time0) << "\n";
//       }

      return (int)difftime(time1,time0);


    } else if (!mode.compare("download")) {
      time(&time0);
      if (fedId) {
	xmlClob = dbFedAccess->getXMLClob(partitionName, (unsigned int)fedId);
      } else {
	xmlClob = dbFedAccess->getXMLClob(partitionName);
      }
      unsigned int bufferSize = xmlClob->length();
      time(&time1);
      std::cout << "bufferSize : " << bufferSize << " download time : " << difftime(time1,time0) << std::endl;
      
      //       XMLByte *xmlBuffer;
      //       if (xmlClob != NULL){
      // 	if (bufferSize>0) {
      // 	  if ((xmlBuffer = (XMLByte *) calloc(bufferSize+1, sizeof(char))) != NULL) {
      // 	    if (bufferSize == (*xmlClob).read(bufferSize, xmlBuffer, bufferSize+1)){
      // 	      const std::string xmlBufferId = "myXmlBuffer";
      // 	      // 	  std::cout << std::endl;
      // 	      // std::cout << xmlBuffer << std::endl;
      // 	    } else {
      // 	      std::cerr << "failed while reading the XML Clob"  << std::endl;
      // 	    }
      // 	  } else {
      // 	    std::cerr << "failed during memory allocation"  << std::endl;  		  }
      // 	} else {
      // 	  std::cerr << "no data found... " << std::endl;
      // 	}
      //       } else {
      // 	std::cerr << "failed while trying to get the XML Clob"  << std::endl;
      //       }
    }
    
    //    free(xmlBuffer);
    
    //    delete[] xmlBuffer;
    delete dbFedAccess;
    
  } catch (oracle::occi::SQLException &ex) {
    std::cerr << "SQLException... !!! " << std::endl;
    std::string errorMessage = ex.what();
    std::cerr << errorMessage << std::endl;
  } catch (...){
    std::cerr << "oups !!!!" << std::endl;
    if(dbFedAccess) delete dbFedAccess;
  }
}


