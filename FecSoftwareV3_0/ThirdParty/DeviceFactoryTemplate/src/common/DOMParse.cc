/*
 * Copyright 1999-2002,2004 The Apache Software Foundation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * $Id: DOMParse.cc,v 1.10 2008/05/23 09:30:17 fdrouhin Exp $
 */

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMBuilder.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMError.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>

#include "DOMCount.hpp"

#include <string.h>
#include <stdlib.h>

#if defined(XERCES_NEW_IOSTREAMS)
#include <fstream>
#else
#include <fstream.h>
#endif

#include "FecExceptionHandler.h"

#include "keyType.h"

#include "deviceDescription.h"
#include "piaResetDescription.h"
#include "apvDescription.h"
#include "muxDescription.h"
#include "dcuDescription.h"
#include "laserdriverDescription.h"
#include "pllDescription.h"

#include "DbFecAccess.h"

#define XMLFECDEVICE
#ifdef XMLFECDEVICE
#  include "XMLFecDevice.h"
#endif

static int countPLL = 0, countAPV25 = 0, countDCUCCU = 0, countDCUFEH = 0, countAPVMUX = 0, countPIA = 0, countAOH = 0, countDOH = 0 ;

#include "deviceType.h"
deviceVector vDevices ; 
piaResetVector vPiaReset ;

// ---------------------------------------------------------------------------
//  This is a simple program which invokes the DOMParser to build a DOM
//  tree for the specified input file. It then walks the tree and counts
//  the number of elements. The element count is then printed.
// ---------------------------------------------------------------------------
static void usage() {
  std::cout << "\nUsage:\n"
    "    DOMCount [options] <XML file | List file>\n\n"
    "This program invokes the DOMBuilder, builds the DOM tree,\n"
    "and then prints the number of elements found in each XML file.\n\n"
    "Options:\n"
    "    -l          Indicate the input file is a List File that has a list of xml files.\n"
    "                Default to off (Input file is an XML file).\n"
    "    -v=xxx      Validation scheme [always | never | auto*].\n"
    "    -n          Enable namespace processing. Defaults to off.\n"
    "    -s          Enable schema processing. Defaults to off.\n"
    "    -f          Enable full schema constraint checking. Defaults to off.\n"
    "    -locale=ll_CC specify the locale, default: en_US.\n"
    "    -p          Print out names of elements and attributes encountered.\n"
    "    -?          Show this help.\n\n"
    "  * = Default if not provided explicitly.\n"
	    << std::endl;
}

/** \brief Display an XML clob buffer
 */
void display ( oracle::occi::Clob *xmlClob ) {

  const XMLByte *xmlBuffer;
  
  unsigned int resultLength = (*xmlClob).length();
  if ((xmlBuffer = (XMLByte *) calloc(resultLength+1, sizeof(char)))) {
    if (resultLength != (*xmlClob).read(resultLength, (unsigned char *)xmlBuffer, resultLength+1)) {
      std::cerr << "Error during the display of a XML clob, wrong size of the buffer" << std::endl ;
    }
    else
      std::cout << "XMLFec::createInputSource ( ): " << xmlBuffer << std::endl ;
  }
  
  delete xmlBuffer ;
}

// ---------------------------------------------------------------------------
//
//  Recursively Count up the total number of child Elements under the specified Node.
//  Process attributes of the node, if any.
//
// ---------------------------------------------------------------------------
static int countChildElements(DOMNode *n, bool printOutEncounteredEles) {
  DOMNode *child;
  int count = 0;
  if (n) {
    if (n->getNodeType() == DOMNode::ELEMENT_NODE) {

      if(printOutEncounteredEles) {
	char *name = XMLString::transcode(n->getNodeName());
	std::cout <<"----------------------------------------------------------"<<std::endl;
	std::cout <<"Encountered Element : "<< name << std::endl;
                
	XMLString::release(&name);
			
	if(n->hasAttributes()) {
	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  int nSize = pAttributes->getLength();
	  std::cout <<"\tAttributes" << std::endl;
	  std::cout <<"\t----------" << std::endl;
	  for(int i=0;i<nSize;++i) {
	    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
	    // get attribute name
	    char *name = XMLString::transcode(pAttributeNode->getName());
                        
	    std::cout << "\t" << name << "=";
	    XMLString::release(&name);
                        
	    // get attribute type
	    name = XMLString::transcode(pAttributeNode->getValue());
	    std::cout << name << std::endl;
	    XMLString::release(&name);
	  }
	}
      }
      ++count;
    }
    for (child = n->getFirstChild(); child != 0; child=child->getNextSibling())
      count += countChildElements(child, printOutEncounteredEles);
  }
  return count;
}

// ---------------------------------------------------------------------------
//
//  Recursively parse and create the descriptions from a file for the total number of child Elements under the specified Node.
//  Process attributes of the node, if any.
//
// ---------------------------------------------------------------------------
static int parseDeviceElements(DOMNode *n, bool printOutEncounteredEles) {
  //deviceVector vDevices ;
  //piaVector vPiaReset ;

  DOMNode *child;
  int count = 0;

  if (n) {
    if (n->getNodeType() == DOMNode::ELEMENT_NODE) {

      char *name = XMLString::transcode(n->getNodeName());
      //std::cout <<"----------------------------------------------------------"<<std::endl;
      //std::cout <<"Encountered Element : "<< name << std::endl;

      if(n->hasAttributes()) {

	int crate = 0, fec = 0,ring = 0,ccu = 0,channel = 0,address = 0 ;
	char fecHardwareId[100] = "0" ;
	bool enabled = true ;
	unsigned long delayActiveReset = 0, intervalDelayReset = 0, mask = 0 ;        // PIA
	int apvMode = 0, latency = 0, muxGain = 0, ipre = 0, ipcasc = 0, ipsf = 0 ;   // APV25
	int isha = 0, issf = 0, ipsp = 0, imuxin = 0, ical = 0, ispare = 0, vfp = 0 ; // APV25
	int vfs = 0, vpsp = 0, cdrv = 0, csel = 0, apvError = 0 ;                     // APV25
	int resistor = 0 ;                                                            // APV MUX
	unsigned int timeStamp = 0, dcuHardId = 0 ;                                   // DCU
	int channel0 = 0, channel1 = 0, channel2 = 0, channel3 = 0, channel4 = 0 ;    // DCU
	int channel5 = 0, channel6 = 0, channel7 = 0 ;                                // DCU
	char dcuType[100] ;                                                           // DCU
	bool dcuEnabled = true ;                                                      // DCU
	int bias0 = 0, bias1 = 0, bias2 = 0, gain0 = 0, gain1 = 0, gain2 = 0 ;        // Laserdriver (AOH or DOH)
	int delayFine = 0,delayCoarse = 0 ;                                           // PLL


	// -------------------------------------------------------------------------------------
	// ------------------------------------------------- PIA reset
	if (!strcmp(name,"PIARESET")) {

	  unsigned int val = 0 ;

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  int nSize = pAttributes->getLength();
	  //std::cout <<"\tAttributes" << std::endl;
	  //std::cout <<"\t----------" << std::endl;
	  for(int i=0;i<nSize;++i) {
	    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
	    // get attribute name
	    char *name = XMLString::transcode(pAttributeNode->getName());
		      
	    //std::cout << "\t" << name << "=";
                        
	    // get attribute type
	    char *value = XMLString::transcode(pAttributeNode->getValue());
	    //std::cout << value << std::endl;

	    if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID])) { val ++ ;  strncpy(fecHardwareId,value,100) ; }
	    else if (!strcmp(name,piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::PIACRATEID])) { val ++ ;  sscanf (value,"%d",&crate) ; } 
	    else if (!strcmp(name,piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::PIAFECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ; } 
	    else if (!strcmp(name,piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::PIARINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ; }
	    else if (!strcmp(name,piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::PIACCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ; }
	    else if (!strcmp(name,piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::PIACHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; }
	    else if (!strcmp(name,piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::DELAYACTIVERESET])) { val ++ ;  sscanf (value,"%ld",&delayActiveReset) ; }
	    else if (!strcmp(name,piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::INTERVALDELAYRESET])) { val ++ ;  sscanf (value,"%ld",&intervalDelayReset) ; }
	    else if (!strcmp(name,piaResetDescription::PIARESETPARAMETERNAMES[piaResetDescription::MASK])) { val ++ ;  sscanf (value,"%ld",&mask) ; }


	    XMLString::release(&name);
	    XMLString::release(&value);
	  }
	    
	  if (val != 9) std::cerr << "PIA reset: invalid number of parameters: " << val << "/9" << std::endl ;

	  piaResetDescription *piaDevice = new piaResetDescription(buildCompleteKey(fec,ring,ccu,channel,address),
								   delayActiveReset, intervalDelayReset, mask) ;
	  piaDevice->setEnabled(enabled) ;
	  //vPiaReset->push_back(piaDevice) ;

	  countPIA ++ ;
	}
	// -------------------------------------------------------------------------------------
	// ------------------------------------------------- APV25
	else if (!strcmp(name,"APV25")) {

	  unsigned int val = 0 ;

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  int nSize = pAttributes->getLength();
	  //std::cout <<"\tAttributes" << std::endl;
	  //std::cout <<"\t----------" << std::endl;
	  for(int i=0;i<nSize;++i) {
	    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
	    // get attribute name
	    char *name = XMLString::transcode(pAttributeNode->getName());
		      
	    //std::cout << "\t" << name << "=";
                        
	    // get attribute type
	    char *value = XMLString::transcode(pAttributeNode->getValue());
	    //std::cout << value << std::endl;

	    if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID])) { val ++ ;  strncpy(fecHardwareId,value,100) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID])) { val ++ ;  sscanf (value,"%d",&crate) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CCHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CADDRESS])) { val ++ ;  sscanf (value,"%d",&address) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::APVMODE])) { val ++ ;  sscanf (value,"%d",&apvMode) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::APVLATENCY])) { val ++ ;  sscanf (value,"%d",&latency) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::MUXGAIN])) { val ++ ;  sscanf (value,"%d",&muxGain) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::IPRE])) { val ++ ;  sscanf (value,"%d",&ipre) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::IPCASC])) { val ++ ;  sscanf (value,"%d",&ipcasc) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::IPSF])) { val ++ ;  sscanf (value,"%d",&ipsf) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::ISHA])) { val ++ ;  sscanf (value,"%d",&isha) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::ISSF])) { val ++ ;  sscanf (value,"%d",&issf) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::IPSP])) { val ++ ;  sscanf (value,"%d",&ipsp) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::IMUXIN])) { val ++ ;  sscanf (value,"%d",&imuxin) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::ICAL])) { val ++ ;  sscanf (value,"%d",&ical) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::ISPARE])) { val ++ ;  sscanf (value,"%d",&ispare) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::VFP])) { val ++ ;  sscanf (value,"%d",&vfp) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::VFS])) { val ++ ;  sscanf (value,"%d",&vfs) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::VPSP])) { val ++ ;  sscanf (value,"%d",&vpsp) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::CDRV])) { val ++ ;  sscanf (value,"%d",&cdrv) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::CSEL])) { val ++ ;  sscanf (value,"%d",&csel) ; }
	    else if (!strcmp(name,apvDescription::APVPARAMETERNAMES[apvDescription::APVERROR])) { val ++ ;  sscanf (value,"%d",&apvError) ; }
	    else std::cout << name << " is not known by the parsing" << std::endl ;

	    XMLString::release(&name);
	    XMLString::release(&value);
	  }

	  if (val != 26) std::cerr << "APV25: invalid number of parameters: " << val << "/26" << std::endl ;

	  apvDescription *device = new apvDescription(buildCompleteKey(fec,ring,ccu,channel,address),
						      apvMode, latency, muxGain, ipre, ipcasc, ipsf,
						      isha, issf, ipsp, imuxin, ical, ispare, vfp,
						      vfs, vpsp, cdrv, csel,apvError) ;
	  device->setEnabled(enabled) ;
	  //vDevices->push_back(device) ;

	  countAPV25 ++ ;
	}
	// -------------------------------------------------------------------------------------
	// ------------------------------------------------- APVMUX
	else if (!strcmp(name,"APVMUX")) {

	  unsigned int val = 0 ;

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  int nSize = pAttributes->getLength();
	  //std::cout <<"\tAttributes" << std::endl;
	  //std::cout <<"\t----------" << std::endl;
	  for(int i=0;i<nSize;++i) {
	    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
	    // get attribute name
	    char *name = XMLString::transcode(pAttributeNode->getName());
		      
	    //std::cout << "\t" << name << "=";
                        
	    // get attribute type
	    char *value = XMLString::transcode(pAttributeNode->getValue());
	    //std::cout << value << std::endl;

	    if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID])) { val ++ ;  strncpy(fecHardwareId,value,100) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID])) { val ++ ;  sscanf (value,"%d",&crate) ;  } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ;  } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ;  }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ;  }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CCHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ;  }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CADDRESS])) { val ++ ;  sscanf (value,"%d",&address) ;  }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; }
	    else if (!strcmp(name,muxDescription::MUXPARAMETERNAMES[muxDescription::RESISTOR])) { val ++ ;  sscanf (value,"%d",&resistor) ; }
	    else std::cout << name << " is not known by the parsing" << std::endl ;
	    XMLString::release(&name);
	    XMLString::release(&value);
	  }

	  if (val != 9) std::cerr << "APVMUX: invalid number of parameters: " << val << "/9" << std::endl ;
	    
	  muxDescription *device = new muxDescription(buildCompleteKey(fec,ring,ccu,channel,address),resistor) ;
	  device->setEnabled(enabled) ;
	  //vDevices->push_back(device) ;
	    
	  countAPVMUX ++ ;
	}
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- DCU
	else if (!strcmp(name,"DCU")) {

	  unsigned int val = 0 ;
	    
	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  int nSize = pAttributes->getLength();
	  //std::cout <<"\tAttributes" << std::endl;
	  //std::cout <<"\t----------" << std::endl;
	  for(int i=0;i<nSize;++i) {
	    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
	    // get attribute name
	    char *name = XMLString::transcode(pAttributeNode->getName());
	      
	    //std::cout << "\t" << name << "=";
	      
	    // get attribute type
	    char *value = XMLString::transcode(pAttributeNode->getValue());
	    //std::cout << value << std::endl;
	      
	    if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID])) { val ++ ;  strncpy(fecHardwareId,value,100) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID])) { val ++ ;  sscanf (value,"%d",&crate) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CCHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CADDRESS])) { val ++ ;  sscanf (value,"%d",&address) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::DCUTIMESTAMP])) { val ++ ;  sscanf (value,"%u",&timeStamp) ; }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::DCUHARDID])) { val ++ ;  sscanf (value,"%u",&dcuHardId) ; }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL0])) { val ++ ;  sscanf (value,"%d",&channel0) ; }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL1])) { val ++ ;  sscanf (value,"%d",&channel1) ; }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL2])) { val ++ ;  sscanf (value,"%d",&channel2) ; }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL3])) { val ++ ;  sscanf (value,"%d",&channel3) ; }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL4])) { val ++ ;  sscanf (value,"%d",&channel4) ; }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL5])) { val ++ ;  sscanf (value,"%d",&channel5) ; }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL6])) { val ++ ;  sscanf (value,"%d",&channel6) ; }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL7])) { val ++ ;  sscanf (value,"%d",&channel7) ; }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::EDCUTYPE])) { val ++ ;  strncpy(dcuType,value,100) ; }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::DCUREADOUTENABLED])) { val ++ ; if (value[0] == 'F') dcuEnabled = false ;}
	    else std::cout << name << " is not known by the parsing" << std::endl ;

	    XMLString::release(&name);
	    XMLString::release(&value);
	  }
	      
	  if (val != 20) std::cerr << "DCU: invalid number of parameters: " << val << "/20" << std::endl ;
	    
	  dcuDescription *device = new dcuDescription(buildCompleteKey(fec,ring,ccu,channel,address),timeStamp, dcuHardId,
						      channel0, channel1, channel2, channel3, channel4, channel5, 
						      channel6, channel7, dcuType) ;
	  device->setEnabled(enabled) ;
	  device->setDcuReadoutEnabled(dcuEnabled) ;
	  //vDevices->push_back(device) ;
	    
	  if (device->getDcuType() == DCUCCU) countDCUCCU ++ ;
	  else if (device->getDcuType() == DCUFEH) countDCUFEH ++ ;
	}
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- Laserdriver
	else if (!strcmp(name,"LASERDRIVER")) {

	  unsigned int val = 0 ;

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  int nSize = pAttributes->getLength();
	  //std::cout <<"\tAttributes" << std::endl;
	  //std::cout <<"\t----------" << std::endl;
	  for(int i=0;i<nSize;++i) {
	    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
	    // get attribute name
	    char *name = XMLString::transcode(pAttributeNode->getName());
		      
	    //std::cout << "\t" << name << "=";
                        
	    // get attribute type
	    char *value = XMLString::transcode(pAttributeNode->getValue());
	    //std::cout << value << std::endl;

	    if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID])) { val ++ ;  strncpy(fecHardwareId,value,100) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID])) { val ++ ;  sscanf (value,"%d",&crate) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CCHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CADDRESS])) { val ++ ;  sscanf (value,"%d",&address) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; }
	    else if (!strcmp(name,laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS0])) { val ++ ;  sscanf (value,"%d",&bias0) ; }
	    else if (!strcmp(name,laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS1])) { val ++ ;  sscanf (value,"%d",&bias1) ; }
	    else if (!strcmp(name,laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::BIAS2])) { val ++ ;  sscanf (value,"%d",&bias2) ; }
	    else if (!strcmp(name,laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN0])) { val ++ ;  sscanf (value,"%d",&gain0) ; }
	    else if (!strcmp(name,laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN1])) { val ++ ;  sscanf (value,"%d",&gain1) ; }
	    else if (!strcmp(name,laserdriverDescription::LASERDRIVERPARAMETERNAMES[laserdriverDescription::GAIN2])) { val ++ ;  sscanf (value,"%d",&gain2) ; }
	    else std::cout << name << " is not known by the parsing" << std::endl ;

	    XMLString::release(&name);
	    XMLString::release(&value);
	  }

	  if (val != 14) std::cerr << "Laserdriver: invalid number of parameters: " << val << "/14" << std::endl ;

	  laserdriverDescription *device = new laserdriverDescription(buildCompleteKey(fec,ring,ccu,channel,address),
								      bias0, bias1, bias2, gain0, gain1, gain2) ;
	  device->setEnabled(enabled) ;
	  //vDevices->push_back(device) ;

	  if (address == 0x60) countAOH ++ ;
	  else if (address == 0x70) countDOH ++ ;
	}
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- PLL
	else if (!strcmp(name,"PLL")) {

	  unsigned int val = 0 ;

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  int nSize = pAttributes->getLength();
	  //std::cout <<"\tAttributes" << std::endl;
	  //std::cout <<"\t----------" << std::endl;
	  for(int i=0;i<nSize;++i) {
	    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
	    // get attribute name
	    char *name = XMLString::transcode(pAttributeNode->getName());
		      
	    //std::cout << "\t" << name << "=";
                        
	    // get attribute type
	    char *value = XMLString::transcode(pAttributeNode->getValue());
	    //std::cout << value << std::endl;

	    if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID])) { val ++ ;  strncpy(fecHardwareId,value,100) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID])) { val ++ ;  sscanf (value,"%d",&crate) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CCHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CADDRESS])) { val ++ ;  sscanf (value,"%d",&address) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; }
	    else if (!strcmp(name,pllDescription::PLLPARAMETERNAMES[pllDescription::DELAYFINE])) { val ++ ;  sscanf (value,"%d",&delayFine) ; }
	    else if (!strcmp(name,pllDescription::PLLPARAMETERNAMES[pllDescription::DELAYCOARSE])) { val ++ ;  sscanf (value,"%d",&delayCoarse) ; }
	    else std::cout << name << " is not known by the parsing" << std::endl ;

	    XMLString::release(&name);
	    XMLString::release(&value);
	  }

	  if (val != 10) std::cerr << "PLL: invalid number of parameters: " << val << "/10" << std::endl ;

	  pllDescription *device = new pllDescription(buildCompleteKey(fec,ring,ccu,channel,address),delayFine,delayCoarse) ;
	  device->setEnabled(enabled) ;
	  //vDevices->push_back(device) ;

	  countPLL ++ ;
	}
      }

      // Release name
      XMLString::release(&name);

      ++count;
    }

    for (child = n->getFirstChild(); child != 0; child=child->getNextSibling())
      count += parseDeviceElements(child, printOutEncounteredEles);
  }

  return count;
}

unsigned int parseAttributesOrig(parameterDescriptionNameType *parameterNames, DOMNamedNodeMap *pAttributes) 
{
  unsigned int val = 0 ;
  for (unsigned int i = 0; i < pAttributes->getLength(); i++){

    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
    StrX name(pAttributeNode->getName());
    StrX value(pAttributeNode->getValue());
    
    if ((*parameterNames).find(name.localForm()) != (*parameterNames).end()) {
       (*parameterNames)[name.localForm()]->setValue(value.getString());
       val ++ ;
    }
  }
  return val ;
}

// ---------------------------------------------------------------------------
//
//  Recursively parse and create the descriptions from a file for the total number of child Elements under the specified Node.
//  Process attributes of the node, if any.
//
// ---------------------------------------------------------------------------
static unsigned int parseAttributes(parameterDescriptionNameType *parameterNames, DOMNamedNodeMap *pAttributes)
{
  unsigned int val = 0 ;
  for (unsigned int i=0; i<pAttributes->getLength(); i++){

    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
    char *name = XMLString::transcode(pAttributeNode->getName());
    char *value = XMLString::transcode(pAttributeNode->getValue());

    if ((*parameterNames).find(name) != (*parameterNames).end()) {
       (*parameterNames)[name]->setValue(value);
       val ++ ;
    }

    XMLString::release(&name);
    XMLString::release(&value);
  }
  return val ;
}

static int parseDeviceElements2(DOMNode *n, bool printOutEncounteredEles) {

  //std::cout << "parseAttributes => used" << std::endl ;

  //deviceVector vDevices ;
  //piaVector vPiaReset ;

  DOMNode *child;
  int count = 0;

  if (n) {
    if (n->getNodeType() == DOMNode::ELEMENT_NODE) {

      char *name = XMLString::transcode(n->getNodeName());
      //std::cout <<"----------------------------------------------------------"<<std::endl;
      //std::cout <<"Encountered Element : "<< name << std::endl;

      if(n->hasAttributes()) {

// 	int fec = 0,ring = 0,ccu = 0,channel = 0,address = 0 ;
// 	char fecHardwareId[100] = "0" ;
// 	bool enabled = true ;
// 	unsigned long delayActiveReset = 0, intervalDelayReset = 0, mask = 0 ;        // PIA
// 	int apvMode = 0, latency = 0, muxGain = 0, ipre = 0, ipcasc = 0, ipsf = 0 ;   // APV25
// 	int isha = 0, issf = 0, ipsp = 0, imuxin = 0, ical = 0, ispare = 0, vfp = 0 ; // APV25
// 	int vfs = 0, vpsp = 0, cdrv = 0, csel = 0, apvError = 0 ;                     // APV25
// 	int resistor = 0 ;                                                            // APV MUX
// 	unsigned int timeStamp = 0, dcuHardId = 0 ;                                   // DCU
// 	int channel0 = 0, channel1 = 0, channel2 = 0, channel3 = 0, channel4 = 0 ;    // DCU
// 	int channel5 = 0, channel6 = 0, channel7 = 0 ;                                // DCU
// 	char dcuType[100] ;                                                           // DCU
// 	bool dcuEnabled = true ;                                                      // DCU
// 	int bias0 = 0, bias1 = 0, bias2 = 0, gain0 = 0, gain1 = 0, gain2 = 0 ;        // Laserdriver (AOH or DOH)
// 	int delayFine = 0,delayCoarse = 0 ;                                           // PLL


	// -------------------------------------------------------------------------------------
	// ------------------------------------------------- PIA reset
	if (!strcmp(name,"PIARESET")) {

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  parameterDescriptionNameType *parameterNames = piaResetDescription::getParameterNames() ;
	  unsigned int val = parseAttributes(parameterNames,pAttributes) ;
	  if (val != 9) std::cerr << "PIA reset: invalid number of parameters: " << val << "/9" << std::endl ;
	  piaResetDescription *piaDevice = new piaResetDescription(*parameterNames) ;
	  piaResetDescription::deleteParameterNames(parameterNames);
	  vPiaReset.push_back(piaDevice) ;

	  countPIA ++ ;
	}
	// -------------------------------------------------------------------------------------
	// ------------------------------------------------- APV25
	else if (!strcmp(name,"APV25")) {

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  parameterDescriptionNameType *parameterNames = apvDescription::getParameterNames() ;
	  unsigned int val = parseAttributes(parameterNames,pAttributes) ;
	  if (val != 25) std::cerr << "APV25: invalid number of parameters: " << val << "/25" << std::endl ;
	  apvDescription *device = new apvDescription(*parameterNames) ;
	  apvDescription::deleteParameterNames(parameterNames);
	  vDevices.push_back(device) ;

	  countAPV25 ++ ;
	}
	// -------------------------------------------------------------------------------------
	// ------------------------------------------------- APVMUX
	else if (!strcmp(name,"APVMUX")) {

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  parameterDescriptionNameType *parameterNames = muxDescription::getParameterNames() ;
	  unsigned int val = parseAttributes(parameterNames,pAttributes) ;
	  if (val != 8) std::cerr << "APVMUX: invalid number of parameters: " << val << "/8" << std::endl ;
	  muxDescription *device = new muxDescription(*parameterNames) ;
	  muxDescription::deleteParameterNames(parameterNames);
	  vDevices.push_back(device) ;

	  countAPVMUX ++ ;
	}
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- DCU
	else if (!strcmp(name,"DCU")) {

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  parameterDescriptionNameType *parameterNames = dcuDescription::getParameterNames() ;
	  unsigned int val = parseAttributes(parameterNames,pAttributes) ;
	  if (val != 19) std::cerr << "DCU: invalid number of parameters: " << val << "/19" << std::endl ;
	  dcuDescription *device = new dcuDescription(*parameterNames) ;
	  dcuDescription::deleteParameterNames(parameterNames);
	  vDevices.push_back(device) ;

	  if (device->getDcuType() == DCUCCU) countDCUCCU ++ ;
	  else if (device->getDcuType() == DCUFEH) countDCUFEH ++ ;
	}
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- Laserdriver
	else if (!strcmp(name,"LASERDRIVER")) {

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  parameterDescriptionNameType *parameterNames = laserdriverDescription::getParameterNames() ;
	  unsigned int val = parseAttributes(parameterNames,pAttributes) ;
	  if (val != 13) std::cerr << "Laserdriver: invalid number of parameters: " << val << "/13" << std::endl ;
	  laserdriverDescription *device = new laserdriverDescription(*parameterNames) ;
	  laserdriverDescription::deleteParameterNames(parameterNames);
	  vDevices.push_back(device) ;

	  if (getAddressKey(device->getKey()) == 0x60) countAOH ++ ;
	  else if (getAddressKey(device->getKey()) == 0x70) countDOH ++ ;
	}
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- PLL
	else if (!strcmp(name,"PLL")) {

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  parameterDescriptionNameType *parameterNames = pllDescription::getParameterNames() ;
	  unsigned int val = parseAttributes(parameterNames,pAttributes) ;
	  if (val != 9) std::cerr << "PLL: invalid number of parameters: " << val << "/9" << std::endl ;
	  pllDescription *device = new pllDescription(*parameterNames) ;
	  pllDescription::deleteParameterNames(parameterNames);
	  vDevices.push_back(device) ;

	  countPLL ++ ;
	}
      }

      // Release name
      XMLString::release(&name);

      ++count;
    }

    for (child = n->getFirstChild(); child != 0; child=child->getNextSibling())
      count += parseDeviceElements2(child, printOutEncounteredEles);
  }

  return count;
}


// ---------------------------------------------------------------------------
//
//   main
//
// ---------------------------------------------------------------------------
int main(int argC, char* argV[])
{

  // Check command line and extract arguments.
  if (argC < 2) {
    usage();
    return 1;
  }

  const char*                xmlFile = 0;
  AbstractDOMParser::ValSchemes valScheme = AbstractDOMParser::Val_Auto;
  bool                       doNamespaces       = false;
  bool                       doSchema           = false;
  bool                       schemaFullChecking = false;
  bool                       doList = false;
  bool                       errorOccurred = false;
  bool                       recognizeNEL = false;
  bool                       printOutEncounteredEles = false;
  char                       localeStr[64];
  std::string                partitionName = "nil" ;
  memset(localeStr, 0, sizeof localeStr);

  int argInd;
  for (argInd = 1; argInd < argC; argInd++) {

    // Break out on first parm not starting with a dash
    if (argV[argInd][0] != '-')
      break;

    // Watch for special case help request
    if (!strcmp(argV[argInd], "-?")) {
      usage();
      return 2;
    }
    else if (!strncmp(argV[argInd], "-v=", 3)
	     ||  !strncmp(argV[argInd], "-V=", 3)) {

      const char* const parm = &argV[argInd][3];
	  
      if (!strcmp(parm, "never"))
	valScheme = AbstractDOMParser::Val_Never;
      else if (!strcmp(parm, "auto"))
	valScheme = AbstractDOMParser::Val_Auto;
      else if (!strcmp(parm, "always"))
	valScheme = AbstractDOMParser::Val_Always;
      else {
	std::cerr << "Unknown -v= value: " << parm << std::endl;
	return 2;
      }
    }
    else if (!strcmp(argV[argInd], "-n")
	     ||  !strcmp(argV[argInd], "-N")) {
      doNamespaces = true;
    }
    else if (!strcmp(argV[argInd], "-s")
	     ||  !strcmp(argV[argInd], "-S")) {
      doSchema = true;
    }
    else if (!strcmp(argV[argInd], "-f")
	     ||  !strcmp(argV[argInd], "-F")) {

      schemaFullChecking = true;
    }
    else if (!strcmp(argV[argInd], "-l") 
	     ||  !strcmp(argV[argInd], "-L")) {
      doList = true;
    }
    else if (!strcmp(argV[argInd], "-special:nel")) {
      
      // turning this on will lead to non-standard compliance behaviour
      // it will recognize the unicode character 0x85 as new line character
      // instead of regular character as specified in XML 1.0
      // do not turn this on unless really necessary
      
      recognizeNEL = true;
    }
    else if (!strcmp(argV[argInd], "-p")
	     ||  !strcmp(argV[argInd], "-P")) {

      printOutEncounteredEles = true;
    }
    else if (!strncmp(argV[argInd], "-locale=", 8)) {

      // Get out the end of line
      strcpy(localeStr, &(argV[argInd][8]));
    }			
    else {

      std::cerr << "Unknown option '" << argV[argInd]
		<< "', ignoring it\n" << std::endl;
    }
  }

  // Initialize the XML4C system
  try {
    if (strlen(localeStr)) {
      XMLPlatformUtils::Initialize(localeStr);
    }
    else {
      XMLPlatformUtils::Initialize();
    }

    if (recognizeNEL) {
      XMLPlatformUtils::recognizeNEL(recognizeNEL);
    }
  }
  catch (const XMLException& toCatch) {

    std::cerr << "Error during initialization! :\n"
	      << StrX(toCatch.getMessage()) << std::endl;
    return 1;
  }

  // Instantiate the DOM parser.
  static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
  DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(gLS);
  DOMBuilder        *parser = ((DOMImplementationLS*)impl)->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);

  parser->setFeature(XMLUni::fgDOMNamespaces, doNamespaces);
  parser->setFeature(XMLUni::fgXercesSchema, doSchema);
  parser->setFeature(XMLUni::fgXercesSchemaFullChecking, schemaFullChecking);

  if (valScheme == AbstractDOMParser::Val_Auto) {

    parser->setFeature(XMLUni::fgDOMValidateIfSchema, true);
  }
  else if (valScheme == AbstractDOMParser::Val_Never) {

    parser->setFeature(XMLUni::fgDOMValidation, false);
  }
  else if (valScheme == AbstractDOMParser::Val_Always) {

    parser->setFeature(XMLUni::fgDOMValidation, true);
  }

  // enable datatype normalization - default is off
  parser->setFeature(XMLUni::fgDOMDatatypeNormalization, true);

  // And create our error handler and install it
  DOMParseCountErrorHandler errorHandler;
  parser->setErrorHandler(&errorHandler);

  //
  //  Get the starting time and kick off the parse of the indicated
  //  file. Catch any exceptions that might propogate out of it.
  //
  unsigned long duration = 0 ;

  bool more = true;
  std::ifstream fin;

  // the input is a list file
  if (doList)
    fin.open(argV[argInd]);

  if (fin.fail()) {
    std::cerr <<"Cannot open the list file: " << argV[argInd] << std::endl;
    return 2;
  }

  while (more) {
    char fURI[1000];
    //initialize the array to zeros
    memset(fURI,0,sizeof(fURI));

    if (doList) {

      if (! fin.eof() ) {
	fin.getline (fURI, sizeof(fURI));
	if (!*fURI)
	  continue;
	else {
	  xmlFile = fURI;
	  std::cerr << "==Parsing== " << xmlFile << std::endl;
	}
      }
      else
	break;
    }
    else {
      xmlFile = argV[argInd];
      if (!strcmp(xmlFile,"database") && (argInd+1 < argC)) {

	argInd ++ ;
	partitionName = std::string(argV[argInd]) ;
      }
      more = false;
    }

    //reset error count first
    errorHandler.resetErrors();

    XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument *doc = 0;

    try
      {
	// reset document pool
	parser->resetDocumentPool();

	// -----------------------------------------------------------------------------------------------
	// Read from the database
	if (!strcmp(xmlFile,"database")) {
	    
	  // Read XML buffer from the database
	  try {
	    std::string login="nil", passwd="nil", path="nil" ;
	    DbFecAccess::getDbConfiguration (login, passwd, path) ;
	    if ((login != "nil") && (partitionName != "") && (partitionName != "nil")) {

	      std::cout << "Retreiving the database for " << login << "@" << path << " for partition " << partitionName << std::endl ;

	      DbFecAccess dbFecAccess(login,passwd,path) ;
	      const unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
	      oracle::occi::Clob *xmlClob = dbFecAccess.getXMLClob(partitionName) ;
	      const unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();
	      std::cout << "Retreive the CLOB from database took " << (endMillis-startMillis) << " ms" << std::endl ;
	      duration += endMillis-startMillis ;
		
	      unsigned int resultLength = 0 ;
	      if (xmlClob != NULL) resultLength = (*xmlClob).length();
	      if (resultLength) {

		display(xmlClob) ;

		XERCES_CPP_NAMESPACE::MemBufInputSource* xmlInputSource = NULL;
		const unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
		XMLByte *xmlBuffer = (XMLByte *) calloc(resultLength+1, sizeof(char)) ;
		(*xmlClob).read(resultLength, xmlBuffer, resultLength+1) ;
		const std::string xmlBufferId = "myXmlBuffer";
		// create an InputSource from the buffer
		xmlInputSource = new XERCES_CPP_NAMESPACE::MemBufInputSource ( (const XMLByte*)xmlBuffer, strlen((const char*)xmlBuffer), (const XMLCh *)xmlBufferId.c_str(), false, XERCES_CPP_NAMESPACE::XMLPlatformUtils::fgMemoryManager);
		xercesc::DOMInputSource *domInputSource = new Wrapper4InputSource((InputSource*)xmlInputSource) ;
		const unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();
		doc = parser->parse(*domInputSource) ;
		std::cout << "Parse the buffer took " << (endMillis-startMillis) << " ms" << std::endl ;
		duration += endMillis-startMillis ;
		//std::cout << xmlBuffer << std::endl ;
	      }
	      else {
		std::cerr << "No Buffer found for the partition " << partitionName << std::endl;
	      }
	    }
	    else {
	      if (login == "nil") 
		std::cerr << "Please set the CONFDB environemental variable as CONFDB=login/password@path" << std::endl ;
	      else if ( (partitionName == "") || (partitionName == "nil") ) 
		std::cerr << "Please specify a partition name" << std::endl ;

	      exit (-1) ;
	    }
	  }
	  catch (FecExceptionHandler &e) {
	    std::cerr << "Error during the reading of the database " << e.what() << std::endl ;
	  }
	  catch (oracle::occi::SQLException &e) {
	    std::cerr << "Error during the reading of the database " << e.what() << std::endl ;
	  }
	}
	else {
	    
	  const unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
	  doc = parser->parseURI(xmlFile);
	  const unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Parse the file took " << (endMillis-startMillis) << " ms" << std::endl ;
	  duration += (endMillis-startMillis) ;
	}
      }
    catch (const XMLException& toCatch)
      {
	XERCES_STD_QUALIFIER cerr << "\nError during parsing: '" << xmlFile << "'\n"
				  << "Exception message is:  \n"
				  << StrX(toCatch.getMessage()) << "\n" << XERCES_STD_QUALIFIER endl;
	errorOccurred = true;
	continue;
      }
    catch (const DOMException& toCatch)
      {
	const unsigned int maxChars = 2047;
	XMLCh errText[maxChars + 1];

	XERCES_STD_QUALIFIER cerr << "\nDOM Error during parsing: '" << xmlFile << "'\n"
				  << "DOMException code is:  " << toCatch.code << XERCES_STD_QUALIFIER endl;

	if (DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
	  XERCES_STD_QUALIFIER cerr << "Message is: " << StrX(errText) << XERCES_STD_QUALIFIER endl;

	errorOccurred = true;
	continue;
      }
    catch (...)
      {
	XERCES_STD_QUALIFIER cerr << "\nUnexpected exception during parsing: '" << xmlFile << "'\n";
	errorOccurred = true;
	continue;
      }

    //
    //  Extract the DOM tree, get the list of all the elements and report the
    //  length as the count of elements.
    //
    if (errorHandler.getSawErrors()) {

      XERCES_STD_QUALIFIER cout << "\nErrors occurred, no output available\n" << XERCES_STD_QUALIFIER endl;
      errorOccurred = true;
    }
    else
      {
#ifdef XMLFECDEVICE
	unsigned int elementCount = 0;
	if (doc) {
	  const unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
	  //elementCount = parseFileElements2((DOMNode*)doc->getDocumentElement(), printOutEncounteredEles);
	  XMLFecDevice xmlFecDevice ;
	  elementCount = xmlFecDevice.parseAttributes((DOMNode*)doc->getDocumentElement()) ;
	  //elementCount = countChildElements((DOMNode*)doc->getDocumentElement(), printOutEncounteredEles);
	  const unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Generation of the descriptions took " << (endMillis - startMillis) << " ms" << std::endl ;
	  duration += endMillis - startMillis;
	  // test getElementsByTagName and getLength
	  XMLCh xa[] = {chAsterisk, chNull};
	  if (elementCount != doc->getElementsByTagName(xa)->getLength()) {
	    XERCES_STD_QUALIFIER cout << "\nErrors occurred, element count is wrong\n" << XERCES_STD_QUALIFIER endl;
	    errorOccurred = true;
	  }
	}

	// Print out the stats that we collected and time taken.
	XERCES_STD_QUALIFIER cout << xmlFile << ": " << duration << " ms ("
				  << elementCount << " elems)." << XERCES_STD_QUALIFIER endl;
#else    
	unsigned int elementCount = 0;
	if (doc) {
	  const unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
	  //elementCount = parseFileElements2((DOMNode*)doc->getDocumentElement(), printOutEncounteredEles);
	  elementCount = parseDeviceElements((DOMNode*)doc->getDocumentElement(), printOutEncounteredEles);
	  //elementCount = countChildElements((DOMNode*)doc->getDocumentElement(), printOutEncounteredEles);
	  const unsigned long endMillis = XMLPlatformUtils::getCurrentMillis();
	  std::cout << "Generation of the descriptions took " << (endMillis - startMillis) << " ms" << std::endl ;
	  duration += endMillis - startMillis;
	  // test getElementsByTagName and getLength
	  XMLCh xa[] = {chAsterisk, chNull};
	  if (elementCount != doc->getElementsByTagName(xa)->getLength()) {
	    XERCES_STD_QUALIFIER cout << "\nErrors occurred, element count is wrong\n" << XERCES_STD_QUALIFIER endl;
	    errorOccurred = true;
	  }
	}
	
	// Print out the stats that we collected and time taken.
	XERCES_STD_QUALIFIER cout << xmlFile << ": " << duration << " ms ("
				  << elementCount << " elems)." << XERCES_STD_QUALIFIER endl;
	
	std::cout << "Found " << countPIA << " PIA" << std::endl ;
	std::cout << "Found " << countAPV25 << " APV25" << std::endl; 
	std::cout << "Found " << countAPVMUX << " APVMUX" << std::endl ;
	std::cout << "Found " << countDCUCCU << " DCU on CCU" << std::endl ;
	std::cout << "Found " << countDCUFEH << " DCU on FEH" << std::endl ;
	std::cout << "Found " << countAOH << " AOH" << std::endl ;
	std::cout << "Found " << countDOH << " DOH" << std::endl ;
	std::cout << "Found " << countPLL << " PLL" << std::endl ;
#endif
      }
  }

  //
  //  Delete the parser itself.  Must be done prior to calling Terminate, below.
  //
  parser->release();

  // And call the termination method
  XMLPlatformUtils::Terminate();

  if (doList)
    fin.close();

  if (errorOccurred)
    return 4;
  else
    return 0;
}



DOMParseCountErrorHandler::DOMParseCountErrorHandler() :

  fSawErrors(false) {
}

DOMParseCountErrorHandler::~DOMParseCountErrorHandler() {
}


// ---------------------------------------------------------------------------
//  DOMCountHandlers: Overrides of the DOM ErrorHandler interface
// ---------------------------------------------------------------------------
bool DOMParseCountErrorHandler::handleError(const DOMError& domError) {

  fSawErrors = true;
  if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
    XERCES_STD_QUALIFIER cerr << "\nWarning at file ";
  else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
    XERCES_STD_QUALIFIER cerr << "\nError at file ";
  else
    XERCES_STD_QUALIFIER cerr << "\nFatal Error at file ";

  XERCES_STD_QUALIFIER cerr << StrX(domError.getLocation()->getURI())
			    << ", line " << domError.getLocation()->getLineNumber()
			    << ", char " << domError.getLocation()->getColumnNumber()
			    << "\n  Message: " << StrX(domError.getMessage()) << XERCES_STD_QUALIFIER endl;

  return true;
}

void DOMParseCountErrorHandler::resetErrors() {

  fSawErrors = false;
}
