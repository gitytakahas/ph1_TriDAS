/*
  This file is part of Fec Software project.

  author: Juha Petäjäjärvi
  based on XMLFecDevice by Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/

#include "TotemXMLFecDevice.h"

#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/dom/DOMLocator.hpp>
#include <xercesc/dom/DOMNamedNodeMap.hpp>
#include <xercesc/dom/DOMAttr.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>
#include <xercesc/sax/SAXException.hpp>

using namespace XERCES_CPP_NAMESPACE ;

#ifndef DISPLAY
# define DISPLAY(x){}
#endif

#define TK_EXPLICIT_COMPARISON

/**Create an TotemXMLFec dedicated for devices such as Pll, Laserdriver, ApvFec, ApvMux, Philips,...<BR>
 * Call the <I>TotemXMLFec:TotemXMLFec()</I> constructor<BR>
 * Call the method <I>TotemXMLFecDevice::init()</I><BR>
 * @exception FecExceptionHandler
 * @see <I>TotemXMLFec::TotemXMLFec()</I>
 * @see <I>TotemXMLFecDevice::init()</I>
 */
TotemXMLFecDevice::TotemXMLFecDevice () throw (FecExceptionHandler) : XMLFecDevice (){

  initParameterNames();
}

#ifdef DATABASE
/**Creates a TotemXMLFecDevice with access to the database<BR>
 * Call the <I>TotemXMLFec::TotemXMLFec(DbAccess *dbAccess, std::string partitionName)</I> constructor<BR>
 * Call the method <I>TotemXMLFecDevice::init()</I><BR>
 * @param dbAccess - class for the database access
 * @param partitionName - partition name
 * @exception FecExceptionHandler
 * @see <I>TotemXMLFec::TotemXMLFec(DbAccess *dbAccess, std::string partitionName)</I>
 * @see <I>TotemXMLFecDevice::init()</I>
 */
TotemXMLFecDevice::TotemXMLFecDevice ( DbFecAccess *dbAccess )  throw (FecExceptionHandler) : XMLFecDevice( dbAccess ) {
 
 initParameterNames();
}
#endif

/**Creates a TotemXMLFecDevice from a buffer<BR>
 * Call the <I>TotemXMLFec::TotemXMLFec(const XMLByte* xmlBuffer)</I> constructor<BR>
 * Call the method <I>TotemXMLFecDevice::init()</I><BR>
 * @param xmlBuffer - buffer
 * @exception FecExceptionHandler
 * @see <I>TotemXMLFec::TotemXMLFec(const XMLByte* xmlBuffer)</I>
 * @see <I>TotemXMLFecDevice::init()</I>
 */
TotemXMLFecDevice::TotemXMLFecDevice (const XMLByte* xmlBuffer ) throw (FecExceptionHandler) : XMLFecDevice( xmlBuffer ){
  
  initParameterNames();
}

/**Creates a TotemXMLFecDevice from a file<BR>
 * Call the <I>TotemXMLFec::TotemXMLFec(std::string xmlFileName)</I> constructor<BR>
 * Call the method <I>TotemXMLFecDevice::init()</I><BR>
 * @param xmlFileName - file name that contains xml device descriptions
 * @exception FecExceptionHandler
 * @see <I>TotemXMLFec::TotemXMLFec(std::string xmlFileName)</I>
 * @see <I>TotemXMLFecDevice::init()</I>
 */
TotemXMLFecDevice::TotemXMLFecDevice ( std::string xmlFileName ) throw (FecExceptionHandler) : XMLFecDevice( xmlFileName ) {

  initParameterNames();
}

/** Destructor
 */
TotemXMLFecDevice::~TotemXMLFecDevice (){

  philipsDescription::deleteParameterNames(philipsParameterNames_); delete philipsParameterNames_ ;

  vfatDescription::deleteParameterNames(vfatParameterNames_); delete vfatParameterNames_ ;
  totemCChipDescription::deleteParameterNames(totemCChipParameterNames_); delete totemCChipParameterNames_ ;
  totemBBDescription::deleteParameterNames(totemBBParameterNames_); delete totemBBParameterNames_ ;
}


/** To avoid multiple copy of the parameter names for the description
 */
void TotemXMLFecDevice::initParameterNames ( ) {
  
  apvParameterNames_         = apvDescription::getParameterNames() ;
  muxParameterNames_         = muxDescription::getParameterNames() ;
  dcuParameterNames_         = dcuDescription::getParameterNames() ;
  laserdriverParameterNames_ = laserdriverDescription::getParameterNames() ;
  pllParameterNames_         = pllDescription::getParameterNames() ;
  philipsParameterNames_     = philipsDescription::getParameterNames() ;

#ifdef PRESHOWER
  deltaParameterNames_       = deltaDescription::getParameterNames() ;
  paceParameterNames_        = paceDescription::getParameterNames() ;
  kchipParameterNames_       = kchipDescription::getParameterNames() ;
  gohParameterNames_         = gohDescription::getParameterNames() ;
#endif

  vfatParameterNames_        = vfatDescription::getParameterNames() ;
  totemCChipParameterNames_  = totemCChipDescription::getParameterNames() ;
  totemBBParameterNames_     = totemBBDescription::getParameterNames() ;
}

/**Generates an XML buffer from the parameter <I>dVector</I> and writes this buffer into the file named <I>outputFileName</I>.<BR>
 * @param dVector - device list
 * @param outputFileName - name of the output xml file
 * @exception FecExceptionHandler : a FecExceptionHandler is raised if
 *     - <I>dVector</I> is not initialized
 *     - the TotemMemBufOutputSource created from the parameter <I>dVector</I> is not initialized 
 * @see XMLCommonFec::writeXMLFile(std::ostringstream *xmlBuffer,std::string xmlFileName);
 */
void TotemXMLFecDevice::setDevices ( deviceVector dVector, std::string outputFileName) throw (FecExceptionHandler) {

  if (dVector.size()) {
    TotemMemBufOutputSource memBufOS(dVector);
    XMLFec::writeXMLFile(memBufOS.getOutputBuffer()->str(), outputFileName);
  }
  else {
    RAISEFECEXCEPTIONHANDLER( NODATAAVAILABLE, NODATAAVAILABLE_MSG + " to be uploaded in DB", ERRORCODE) ;
  }
}

/**Generates an XML buffer from the attribute <I>dVector_</I> and writes this buffer into the file named <I>outputFileName</I>.<BR>
 * @param outputFileName - name of the output xml file
 * @exception FecExceptionHandler
 * @see XMLFecDevice::setDevices ( deviceVector *dVector, std::string outputFileName)
 */
void TotemXMLFecDevice::setDevices ( std::string outputFileName ) throw (FecExceptionHandler) {

  setDevices( dVector_, outputFileName);
}



// ---------------------------------------------------------------------------------------------
// Parser
// ---------------------------------------------------------------------------------------------

/** Parse the elements for a FEC buffer
 * \param DOMNode - DOM buffer (DOMDocument or DOMNode)
 */
unsigned int TotemXMLFecDevice::parseAttributes(XERCES_CPP_NAMESPACE::DOMNode *n) {

  XERCES_CPP_NAMESPACE::DOMNode *child;
  unsigned int count = 0;

  if (n) {
    if (n->getNodeType() == DOMNode::ELEMENT_NODE) {

      char *name = XMLString::transcode(n->getNodeName());
      //std::cout <<"----------------------------------------------------------"<<std::endl;
      //std::cout <<"Encountered Element : "<< name << std::endl;

      if(n->hasAttributes()) {

#ifdef TK_EXPLICIT_COMPARISON
	int crate = 0, fec = 0,ring = 0,ccu = 0,channel = 0,address = 0, vmeControllerDaisyChainId = 0 ;
	char fecHardwareId[100] = "0" ;
	bool enabled = true ;
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
	int delayFine = 0, delayCoarse = 0, pllDac = 255 ;                            // PLL
#endif

	// -------------------------------------------------------------------------------------
	// ------------------------------------------------- APV25
	if (!strcmp(name,"APV25")) {

#ifdef TK_EXPLICIT_COMPARISON
	  unsigned int val = 0 ;

	  // get all the attributes of the node
	  DOMNamedNodeMap *pAttributes = n->getAttributes();
	  int nSize = pAttributes->getLength();

	  for(int i=0;i<nSize;++i) {
	    DOMAttr *pAttributeNode = (DOMAttr*) pAttributes->item(i);
	    // get attribute name
	    char *name = XMLString::transcode(pAttributeNode->getName());
	    //std::cout << "\t" << name << "/" << "=";
                        
	    // get attribute type
	    char *value = XMLString::transcode(pAttributeNode->getValue());
	    //std::cout << value << std::endl;

	    if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID])) { val ++ ;  strncpy(fecHardwareId,value,100) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID])) { val ++ ;  sscanf (value,"%d",&crate) ;} 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::VMECONTROLLERDAISYCHAINID])) { /*val ++ ;*/  sscanf (value,"%d",&vmeControllerDaisyChainId) ; } 
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
	    else std::cerr << "Unknown tag: " << name << std::endl ;
	    XMLString::release(&name);
	    XMLString::release(&value);
	  }

	  if (val != 26) std::cerr << "APV25: invalid number of parameters: " << val << "/26" << std::endl ;

	  apvDescription *device = new apvDescription(buildCompleteKey(fec,ring,ccu,channel,address),
						      apvMode, latency, muxGain, ipre, ipcasc, ipsf,
						      isha, issf, ipsp, imuxin, ical, ispare, vfp,
						      vfs, vpsp, cdrv, csel,apvError) ;
	  device->setEnabled(enabled) ;
	  device->setFecHardwareId(fecHardwareId,crate) ;
	  //device->setCrateId(crate) ;
	  device->setVMEControllerDaisyChainId(vmeControllerDaisyChainId) ;
#else
	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(apvParameterNames_,pAttributes) ;
	  if ( (val != 26) && (val != 27) ) std::cerr << "APV: invalid number of parameters: " << val << "/" << 26 << std::endl ;
	  apvDescription *device = new apvDescription (*apvParameterNames_) ;
#endif

	  dVector_.push_back(device) ;
	  countAPV25 ++ ;
	}
	// -------------------------------------------------------------------------------------
	// ------------------------------------------------- APVMUX
	else if (!strcmp(name,"APVMUX")) {

#ifdef TK_EXPLICIT_COMPARISON
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
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::VMECONTROLLERDAISYCHAINID])) { /*val ++ ;*/  sscanf (value,"%d",&vmeControllerDaisyChainId) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CCHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CADDRESS])) { val ++ ;  sscanf (value,"%d",&address) ; }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; }
	    else if (!strcmp(name,muxDescription::MUXPARAMETERNAMES[muxDescription::RESISTOR])) { val ++ ;  sscanf (value,"%d",&resistor) ; }
	    else std::cerr << "Unknown tag: " << name << std::endl ;
	    XMLString::release(&name);
	    XMLString::release(&value);
	  }

	  if (val != 9) std::cerr << "APVMUX: invalid number of parameters: " << val << "/9" << std::endl ;
	    
	  muxDescription *device = new muxDescription(buildCompleteKey(fec,ring,ccu,channel,address),resistor) ;
	  device->setEnabled(enabled) ;
	  device->setFecHardwareId(fecHardwareId,crate) ;
	  //device->setCrateId(crate) ;
	  device->setVMEControllerDaisyChainId(vmeControllerDaisyChainId) ;
#else
	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(muxParameterNames_,pAttributes) ;
	  if ( (val != 9) && (val != 10) ) std::cerr << "MUX: invalid number of parameters: " << val << "/" << 9 << std::endl ;
	  muxDescription *device = new muxDescription (*muxParameterNames_) ;
#endif

	  dVector_.push_back(device) ;	    
	  countAPVMUX ++ ;
	}
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- DCU
	else if (!strcmp(name,"DCU")) {

#ifdef TK_EXPLICIT_COMPARISON
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

	    if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID])) { val ++ ;  strncpy(fecHardwareId,value,100) ; DISPLAY("FEC hard ID"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID])) { val ++ ;  sscanf (value,"%d",&crate) ; DISPLAY("crate"); } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::VMECONTROLLERDAISYCHAINID])) { /*val ++ ;*/  sscanf (value,"%d",&vmeControllerDaisyChainId) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ; DISPLAY("fec"); } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ; DISPLAY("ring"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ; DISPLAY("ccu"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CCHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ; DISPLAY("channel"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CADDRESS])) { val ++ ;  sscanf (value,"%d",&address) ; DISPLAY("address"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; DISPLAY("enabled"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::DCUTIMESTAMP])) { val ++ ;  sscanf (value,"%u",&timeStamp) ; DISPLAY("timeStamp"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::DCUHARDID])) { val ++ ;  sscanf (value,"%u",&dcuHardId) ; DISPLAY("dcuHardId"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL0])) { val ++ ;  sscanf (value,"%d",&channel0) ; DISPLAY("channel0"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL1])) { val ++ ;  sscanf (value,"%d",&channel1) ; DISPLAY("channel1"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL2])) { val ++ ;  sscanf (value,"%d",&channel2) ; DISPLAY("channel2"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL3])) { val ++ ;  sscanf (value,"%d",&channel3) ; DISPLAY("channel3"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL4])) { val ++ ;  sscanf (value,"%d",&channel4) ; DISPLAY("channel4"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL5])) { val ++ ;  sscanf (value,"%d",&channel5) ; DISPLAY("channel5"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL6])) { val ++ ;  sscanf (value,"%d",&channel6) ; DISPLAY("channel6"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::CHANNEL7])) { val ++ ;  sscanf (value,"%d",&channel7) ; DISPLAY("channel7"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::EDCUTYPE])) { val ++ ;  strncpy(dcuType,value,100) ; DISPLAY("dcuType"); }
	    else if (!strcmp(name,dcuDescription::DCUPARAMETERNAMES[dcuDescription::DCUREADOUTENABLED])) { val ++ ; if (value[0] == 'F') dcuEnabled = false ; DISPLAY("dcuEnabled");}
	    else std::cerr << "Unknown tag: " << name << std::endl ;
	      
	    XMLString::release(&name);
	    XMLString::release(&value);
	  }
	      
	  if (val != 20) std::cerr << "DCU: invalid number of parameters: " << val << "/20" << std::endl ;
	    
	  dcuDescription *device = new dcuDescription(buildCompleteKey(fec,ring,ccu,channel,address),timeStamp, dcuHardId,
						      channel0, channel1, channel2, channel3, channel4, channel5, 
						      channel6, channel7, dcuType) ;
	  device->setEnabled(enabled) ;
	  device->setDcuReadoutEnabled(dcuEnabled) ;
	  device->setFecHardwareId(fecHardwareId,crate) ;
	  //device->setCrateId(crate) ;
	  device->setVMEControllerDaisyChainId(vmeControllerDaisyChainId) ;
#else
	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(dcuParameterNames_,pAttributes) ;
	  if ( (val != 20) && (val != 21) ) std::cerr << "DCU: invalid number of parameters: " << val << "/" << 20 << std::endl ;
	  dcuDescription *device = new dcuDescription (*dcuParameterNames_) ;
#endif

	  dVector_.push_back(device) ;	    
	  if (device->getDcuType() == DCUCCU) countDCUCCU ++ ;
	  else if (device->getDcuType() == DCUFEH) countDCUFEH ++ ;
	}
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- Laserdriver
	else if (!strcmp(name,"LASERDRIVER")) {

#ifdef TK_EXPLICIT_COMPARISON
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
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::VMECONTROLLERDAISYCHAINID])) { /*val ++ ;*/  sscanf (value,"%d",&vmeControllerDaisyChainId) ; } 
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
	    else std::cerr << "Unknown tag: " << name << std::endl ;
	    XMLString::release(&name);
	    XMLString::release(&value);
	  }

	  if (val != 14) std::cerr << "Laserdriver: invalid number of parameters: " << val << "/14" << std::endl ;

	  laserdriverDescription *device = new laserdriverDescription(buildCompleteKey(fec,ring,ccu,channel,address),
								      bias0, bias1, bias2, gain0, gain1, gain2) ;
	  device->setEnabled(enabled) ;
	  device->setFecHardwareId(fecHardwareId,crate) ;
	  //device->setCrateId(crate) ;
	  device->setVMEControllerDaisyChainId(vmeControllerDaisyChainId) ;
#else
	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(laserdriverParameterNames_,pAttributes) ;
	  if ( (val != 14) && (val != 15) ) std::cerr << "Laserdriver: invalid number of parameters: " << val << "/" << 14 << std::endl ;
	  laserdriverDescription *device = new laserdriverDescription (*laserdriverParameterNames_) ;
#endif

	  dVector_.push_back(device) ;
	  if (device->getDeviceType() == LASERDRIVER) countAOH ++ ;
	  else if (device->getDeviceType() == DOH) countDOH ++ ;
	  else std::cerr << "Unknow device type, expecting laserdriver/DOH: " << device->getDeviceType() << std::endl ;
	}
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- PLL
	else if (!strcmp(name,"PLL")) {

#ifdef TK_EXPLICIT_COMPARISON
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

	    if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECHARDWAREID])) { val ++ ;  strncpy(fecHardwareId,value,100) ; DISPLAY("FEC hard ID"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CRATEID])) { val ++ ;  sscanf (value,"%d",&crate) ; DISPLAY("crate"); } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::VMECONTROLLERDAISYCHAINID])) { /*val ++ ;*/  sscanf (value,"%d",&vmeControllerDaisyChainId) ; } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::FECSLOT])) { val ++ ;  sscanf (value,"%d",&fec) ; DISPLAY("fec"); } 
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::RINGSLOT])) { val ++ ;  sscanf (value,"%d",&ring) ; DISPLAY("ring"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::CCUADDRESS])) { val ++ ;  sscanf (value,"%d",&ccu) ; DISPLAY("ccu"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CCHANNEL])) { val ++ ;  sscanf (value,"%d",&channel) ; DISPLAY("channel"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::I2CADDRESS])) { val ++ ;  sscanf (value,"%d",&address) ; DISPLAY("address"); }
	    else if (!strcmp(name,deviceDescription::FECPARAMETERNAMES[deviceDescription::ENABLED])) { val ++ ; if (value[0] == 'F') enabled = false ; DISPLAY("enabled"); }
	    else if (!strcmp(name,pllDescription::PLLPARAMETERNAMES[pllDescription::DELAYFINE])) { val ++ ;  sscanf (value,"%d",&delayFine) ; DISPLAY("delayFine"); }
	    else if (!strcmp(name,pllDescription::PLLPARAMETERNAMES[pllDescription::DELAYCOARSE])) { val ++ ;  sscanf (value,"%d",&delayCoarse) ; DISPLAY("delayCoarse");}
	    else if (!strcmp(name,pllDescription::PLLPARAMETERNAMES[pllDescription::PLLDAC])) { val ++ ;  sscanf (value,"%d",&pllDac) ; DISPLAY("pllDac"); }
	    else std::cerr << "Unknown tag: " << name << std::endl ;
	    XMLString::release(&name);
	    XMLString::release(&value);
	  }

	  if (val != 11) std::cerr << "PLL: invalid number of parameters: " << val << "/11" << std::endl ;

	  pllDescription *device = new pllDescription(buildCompleteKey(fec,ring,ccu,channel,address),delayFine,delayCoarse,pllDac) ;
	  device->setEnabled(enabled) ;
	  device->setFecHardwareId(fecHardwareId,crate) ;
	  //device->setCrateId(crate) ;
	  device->setVMEControllerDaisyChainId(vmeControllerDaisyChainId) ;
#else
	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(pllParameterNames_,pAttributes) ;
	  if ( (val != 11) && (val != 12) ) std::cerr << "PLL: invalid number of parameters: " << val << "/" << 11 << std::endl ;
	  pllDescription *device = new pllDescription (*pllParameterNames_) ;
#endif

	  dVector_.push_back(device) ;
	  countPLL ++ ;
	}
	// ----------------------------------------------- PHILIPS
	else if (!strcmp(name,"PHILIPS")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(philipsParameterNames_,pAttributes) ;
	  if ( (val != 6) && (val != 7) ) std::cerr << "Philips description: invalid number of parameters: " << val << "/" << 6 << std::endl ;
	  philipsDescription *philips = new philipsDescription (*philipsParameterNames_) ;
	  dVector_.push_back(philips) ;
	}

#ifdef PRESHOWER
	// *************************************************************************************
	// Please note that the explicit comparison done as the Tracker devices has a performance
	// improvement between 20 to 40% of the time
	// no counter has been added
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- DELTA
	else if (!strcmp(name,"DELTA")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(deltaParameterNames_,pAttributes) ;
	  if ( (val != DELTA_DESC_PAR_NUM) && (val != (DELTA_DESC_PAR_NUM+1)) ) std::cerr << "Delta description: invalid number of parameters: " << std::dec << val << "/" << DELTA_DESC_PAR_NUM << std::endl ;
	  deltaDescription *delta = new deltaDescription (*deltaParameterNames_) ;

	  dVector_.push_back(delta) ;
	}
	else if (!strcmp(name,"PACEAM")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(paceParameterNames_,pAttributes) ;
	  if ( (val != PACE_DESC_PAR_NUM) && (val != (PACE_DESC_PAR_NUM+1)) ) std::cerr << "Pace description: invalid number of parameters: " << std::dec << val << "/" << PACE_DESC_PAR_NUM << std::endl ;
	  paceDescription *pace = new paceDescription (*paceParameterNames_) ;
	  dVector_.push_back(pace) ;
	}
	else if (!strcmp(name,"KCHIP")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(kchipParameterNames_,pAttributes) ;
	  if ( (val != KCHIP_DESC_PAR_NUM) && (val != (KCHIP_DESC_PAR_NUM+1)) ) std::cerr << "Kchip description: invalid number of parameters: " << std::dec << val << "/" << KCHIP_DESC_PAR_NUM << std::endl ;
	  kchipDescription *kchip = new kchipDescription (*kchipParameterNames_) ;
	  dVector_.push_back(kchip) ;
	}
	else if (!strcmp(name,"GOH")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(gohParameterNames_,pAttributes) ;
	  if (val != GOH_DESC_PAR_NUM) std::cerr << "Goh description: invalid number of parameters: " << std::dec << val << "/" << GOH_DESC_PAR_NUM << std::endl ;
	  gohDescription *goh = new gohDescription (*gohParameterNames_) ;
	  dVector_.push_back(goh) ;
	}
#endif

	// *************************************************************************************
	// Please note that the explicit comparison done as the Tracker devices has a performance
	// improvement between 20 to 40% of the time
	// no counter has been added
	// -------------------------------------------------------------------------------------
	// ----------------------------------------------- VFAT
	else if (!strcmp(name,"VFAT")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(vfatParameterNames_,pAttributes) ;
	  if ( (val != 149) && (val != 150) ) std::cerr << "Vfat description: invalid number of parameters: " << val << "/" << 149 << std::endl ;
	  vfatDescription *vfat = new vfatDescription (*vfatParameterNames_) ;
	  dVector_.push_back(vfat) ;
	  //countVfat ++ ;
	}
	// ----------------------------------------------- CCHIP
	else if (!strcmp(name,"CCHIP")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(totemCChipParameterNames_,pAttributes) ;
	  if ( (val != 149) && (val != 150) ) std::cerr << "Cchip description: invalid number of parameters: " << val << "/" << 149 << std::endl ;
	  totemCChipDescription *cchip = new totemCChipDescription (*totemCChipParameterNames_) ;
	  dVector_.push_back(cchip) ;
	  //countCchip ++ ;
	}
	// ----------------------------------------------- TBB
	else if (!strcmp(name,"TBB")) {

	  // get all the attributes of the node
	  XERCES_CPP_NAMESPACE::DOMNamedNodeMap *pAttributes = n->getAttributes();
	  unsigned int val = XMLCommonFec::parseAttributes(totemBBParameterNames_,pAttributes) ;
	  if ( (val != 5) && (val != 6) ) std::cerr << "Tbb description: invalid number of parameters: " << val << "/" << 5 << std::endl ;
	  totemBBDescription *tbb = new totemBBDescription (*totemBBParameterNames_) ;
	  dVector_.push_back(tbb) ;
	  //countTBB ++ ;
	}
      }

      // Release name
      XMLString::release(&name);

      ++count;
    }

    for (child = n->getFirstChild(); child != 0; child=child->getNextSibling()){ count += parseAttributes(child) ; }
  }

  return count;
}
