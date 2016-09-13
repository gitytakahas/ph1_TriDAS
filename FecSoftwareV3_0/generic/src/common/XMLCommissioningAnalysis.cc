/*
  This file is part of Fec Software project.

  Fec Software is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

  Fec Software is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with Fec Software; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  Copyright 2002 - 2007, Sebastien BEL - CERN GENEVA
*/

#include "XMLCommissioningAnalysis.h"

//#define DISPLAYBUFFER

const char *XMLCommissioningAnalysis::RUNVERSION[] = {"runNumber","versionMajorId","versionMinorId"};



//----------------------------------
XMLCommissioningAnalysis::XMLCommissioningAnalysis() throw (FecExceptionHandler) : XMLCommonFec () {
  _init();
}

//----------------------------------
XMLCommissioningAnalysis::XMLCommissioningAnalysis( const XMLByte* xmlBuffer ) throw (FecExceptionHandler) : XMLCommonFec( xmlBuffer ) {
  _init();
}

#ifdef DATABASE
//----------------------------------
XMLCommissioningAnalysis::XMLCommissioningAnalysis( DbCommissioningAnalysisAccess *dbAccess )throw (FecExceptionHandler) : XMLCommonFec( (DbCommonAccess *)dbAccess ) {
  _init();
}
#endif

//----------------------------------
XMLCommissioningAnalysis::XMLCommissioningAnalysis( std::string xmlFileName ) throw (FecExceptionHandler) : XMLCommonFec( xmlFileName ) {
  _init();
  parseXMLBuffer();
}

//----------------------------------
void XMLCommissioningAnalysis::_init() {
  _counter = 0;
  _parameterNames = NULL;
  _currentCommissioningType = CommissioningAnalysisDescription::T_UNKNOWN;
  _pAttributes = NULL;
}

//----------------------------------
XMLCommissioningAnalysis::~XMLCommissioningAnalysis() {
  if ( this->_parameterNames != NULL ) {
    CommissioningAnalysisDescription::deleteParameterNames(_parameterNames);
    delete _parameterNames;
  }
}


//------------------------------------
parameterDescriptionNameType *XMLCommissioningAnalysis::getRunVersionParameterNames() {
  parameterDescriptionNameType *parameterNames = new parameterDescriptionNameType;
  (*parameterNames)[RUNVERSION[RUNVERSION_RUN]]          = new ParameterDescription(RUNVERSION[RUNVERSION_RUN],          ParameterDescription::INTEGER16 );
  (*parameterNames)[RUNVERSION[RUNVERSION_VERSIONMAJOR]] = new ParameterDescription(RUNVERSION[RUNVERSION_VERSIONMAJOR], ParameterDescription::STRING    );
  (*parameterNames)[RUNVERSION[RUNVERSION_VERSIONMINOR]] = new ParameterDescription(RUNVERSION[RUNVERSION_VERSIONMINOR], ParameterDescription::STRING    );
  return parameterNames;
}


//----------------------------------
unsigned int XMLCommissioningAnalysis::parseAttributes( XERCES_CPP_NAMESPACE::DOMNode *n ) {

  XERCES_CPP_NAMESPACE::DOMNode *child;
  unsigned int count = 0;

  if (n) {

    if (n->getNodeType() == XERCES_CPP_NAMESPACE::DOMNode::ELEMENT_NODE) {

      if( n->hasAttributes() ) {

	CommissioningAnalysisDescription *myDesc = NULL;
	// get all the attributes of the node
	this->_pAttributes = n->getAttributes();


	if ( this->_parameterNames == NULL ) {
					
	  //std::cout << __func__ << " . _parameterNames is NULL" << std::endl;

	  char *name = XERCES_CPP_NAMESPACE::XMLString::transcode(n->getNodeName());

	  //std::cout << __func__ << " . element name: " << name << std::endl;

	  if ( ! strcmp(name,"ANALYSISRUNVERSION") ) {

	    XERCES_CPP_NAMESPACE::DOMAttr *pAttributeNode = (XERCES_CPP_NAMESPACE::DOMAttr*)_pAttributes->item(0);
	    char *run = XERCES_CPP_NAMESPACE::XMLString::transcode(pAttributeNode->getValue());
	    pAttributeNode = (XERCES_CPP_NAMESPACE::DOMAttr*)_pAttributes->item(1);
	    char *maj = XERCES_CPP_NAMESPACE::XMLString::transcode(pAttributeNode->getValue());
	    pAttributeNode = (XERCES_CPP_NAMESPACE::DOMAttr*)_pAttributes->item(2);
	    char *min = XERCES_CPP_NAMESPACE::XMLString::transcode(pAttributeNode->getValue());

	    std::pair<uint32_t, uint32_t> version(atoi(maj), atoi(min));
	    uint32_t runId = atoi(run);

	    _pairOfRunVersion[ runId ].push_back( version );

	    //std::cout << " . . . " << runId << ", " << atoi(maj) << ", " << atoi(min) << " -> " << _pairOfRunVersion.size() << std::endl;

	    XERCES_CPP_NAMESPACE::XMLString::release(&run);
	    XERCES_CPP_NAMESPACE::XMLString::release(&maj);
	    XERCES_CPP_NAMESPACE::XMLString::release(&min);

	  } else if ( ! strcmp(name,"ANALYSISLOCALVERSIONS") ) {

	    XERCES_CPP_NAMESPACE::DOMAttr *pAttributeNode = (XERCES_CPP_NAMESPACE::DOMAttr*)_pAttributes->item(0);
	    char *analysisType = XERCES_CPP_NAMESPACE::XMLString::transcode(pAttributeNode->getValue());
	    pAttributeNode = (XERCES_CPP_NAMESPACE::DOMAttr*)_pAttributes->item(1);
	    char *maj = XERCES_CPP_NAMESPACE::XMLString::transcode(pAttributeNode->getValue());
	    pAttributeNode = (XERCES_CPP_NAMESPACE::DOMAttr*)_pAttributes->item(2);
	    char *min = XERCES_CPP_NAMESPACE::XMLString::transcode(pAttributeNode->getValue());

	    std::pair<uint32_t, uint32_t> version(atoi(maj), atoi(min));
	    CommissioningAnalysisDescription::commissioningType analType = CommissioningAnalysisDescription::getAnalysisType(analysisType);
	    if ( analType == CommissioningAnalysisDescription::T_UNKNOWN ) {
	      XERCES_CPP_NAMESPACE::XMLString::release(&name);
	      std::cerr << "XMLCommissioningAnalysis::parseAttributes -> Unknown analysis type: " << analysisType << std::endl;
	      std::cout << "XML File parsing will be canceled!" << std::endl;
	      _analysisLocalVersions.clear();
	      _counter = 0;
	      return 0;
	    }
	    _analysisLocalVersions[ analType ] = version;

	    //std::cout << " . . . " << analType << ", " << atoi(maj) << ", " << atoi(min) << " -> " << _analysisLocalVersions.size() << std::endl;

	    XERCES_CPP_NAMESPACE::XMLString::release(&analysisType);
	    XERCES_CPP_NAMESPACE::XMLString::release(&maj);
	    XERCES_CPP_NAMESPACE::XMLString::release(&min);

	  } else if ( ! strcmp(name,"VPSPSCANANALYSIS") ) {
	    _currentCommissioningType = CommissioningAnalysisDescription::T_ANALYSIS_VPSPSCAN;
	    _parameterNames = VpspScanAnalysisDescription::getParameterNames();

	  } else if ( ! strcmp(name,"TIMINGANALYSIS") ) {
	    _currentCommissioningType = CommissioningAnalysisDescription::T_ANALYSIS_TIMING;
	    _parameterNames = TimingAnalysisDescription::getParameterNames();

	  } else if ( ! strcmp(name,"PEDESTALSANALYSIS") ) {
	    _currentCommissioningType = CommissioningAnalysisDescription::T_ANALYSIS_PEDESTALS;
	    _parameterNames = PedestalsAnalysisDescription::getParameterNames();

	  } else if ( ! strcmp(name,"OPTOSCANANALYSIS") ) {
	    _currentCommissioningType = CommissioningAnalysisDescription::T_ANALYSIS_OPTOSCAN;
	    _parameterNames = OptoScanAnalysisDescription::getParameterNames();

	  } else if ( ! strcmp(name,"FINEDELAYANALYSIS") ) {
	    _currentCommissioningType = CommissioningAnalysisDescription::T_ANALYSIS_FINEDELAY;
	    _parameterNames = FineDelayAnalysisDescription::getParameterNames();

	  } else if ( ! strcmp(name,"FASTFEDCABLINGANALYSIS") ) {
	    _currentCommissioningType = CommissioningAnalysisDescription::T_ANALYSIS_FASTFEDCABLING;
	    _parameterNames = FastFedCablingAnalysisDescription::getParameterNames();

	  } else if ( ! strcmp(name,"CALIBRATIONANALYSIS") ) {
	    _currentCommissioningType = CommissioningAnalysisDescription::T_ANALYSIS_CALIBRATION;
	    _parameterNames = CalibrationAnalysisDescription::getParameterNames();

	  } else if ( ! strcmp(name,"APVLATENCYANALYSIS") ) {
	    _currentCommissioningType = CommissioningAnalysisDescription::T_ANALYSIS_APVLATENCY;
	    _parameterNames = ApvLatencyAnalysisDescription::getParameterNames();

	  } else if ( strcmp(name,"ROWSET") ) {
	    XERCES_CPP_NAMESPACE::XMLString::release(&name);
	    std::cerr << "XMLCommissioningAnalysis::parseAttributes -> Unknown XML element name: " << name << std::endl;
	    std::cout << "XML File parsing will be canceled!" << std::endl;
	    _dVector.clear();
	    deleteRunVersion();
	    _counter = 0;
	    return 0;
	  }

	  // Release name
	  XERCES_CPP_NAMESPACE::XMLString::release(&name);
	}
	//		std::cout << "this->_parameterNames: " << this->_parameterNames << std::endl;
	// make two times the same test:
	if ( this->_parameterNames != NULL ) {
	  count = XMLCommonFec::parseAttributes( _parameterNames, _pAttributes );
	  if ( this->_currentCommissioningType == CommissioningAnalysisDescription::T_ANALYSIS_VPSPSCAN )
	    myDesc = new VpspScanAnalysisDescription(*_parameterNames);
	  else if ( this->_currentCommissioningType == CommissioningAnalysisDescription::T_ANALYSIS_TIMING )
	    myDesc = new TimingAnalysisDescription(*_parameterNames);
	  else if ( this->_currentCommissioningType == CommissioningAnalysisDescription::T_ANALYSIS_PEDESTALS ) 
	    myDesc = new PedestalsAnalysisDescription(*_parameterNames);
	  else if ( this->_currentCommissioningType == CommissioningAnalysisDescription::T_ANALYSIS_OPTOSCAN ) 
	    myDesc = new OptoScanAnalysisDescription(*_parameterNames);
	  else if ( this->_currentCommissioningType == CommissioningAnalysisDescription::T_ANALYSIS_FINEDELAY )
	    myDesc = new FineDelayAnalysisDescription(*_parameterNames);
	  else if ( this->_currentCommissioningType == CommissioningAnalysisDescription::T_ANALYSIS_FASTFEDCABLING )
	    myDesc = new FastFedCablingAnalysisDescription(*_parameterNames);
	  else if ( this->_currentCommissioningType == CommissioningAnalysisDescription::T_ANALYSIS_CALIBRATION )
	    myDesc = new CalibrationAnalysisDescription(*_parameterNames);
	  else if ( this->_currentCommissioningType == CommissioningAnalysisDescription::T_ANALYSIS_APVLATENCY )
	    myDesc = new ApvLatencyAnalysisDescription(*_parameterNames);
	  _dVector.push_back(myDesc);
	}
	_counter ++ ;

      }

      ++count;
    }

    for ( child=n->getFirstChild(); child != 0; child=child->getNextSibling() )
      count+=parseAttributes(child) ;
  }

  return count;
}

//------------------------------------
void XMLCommissioningAnalysis::deleteRunVersion() {
  _pairOfRunVersion.clear() ;
}

//------------------------------------
void XMLCommissioningAnalysis::deleteAnalysisLocalVersions() {
  _analysisLocalVersions.clear() ;
}

//----------------------------------
void XMLCommissioningAnalysis::clearVector() {
  deleteRunVersion();
  deleteAnalysisLocalVersions();
  _currentCommissioningType = CommissioningAnalysisDescription::T_UNKNOWN;
  while (_dVector.size() > 0) {
    delete _dVector.back();
    _dVector.pop_back();
  }
  _dVector.clear() ;
}

//----------------------------------
std::vector<CommissioningAnalysisDescription*> XMLCommissioningAnalysis::getDescriptions() {
  return _dVector;
}

//----------------------------------
void XMLCommissioningAnalysis::setDescriptions( std::vector<CommissioningAnalysisDescription*> &inVector, CommissioningAnalysisDescription::commissioningType type ) throw (FecExceptionHandler) {
  clearVector();
  _currentCommissioningType = type;
  std::cout << "SET DESCRIPTIONS:" << inVector.size() << " TYPE:" << CommissioningAnalysisDescription::getAnalysisType(type) << std::endl;
  for (std::vector<CommissioningAnalysisDescription*>::iterator it=inVector.begin(); it!=inVector.end(); it ++) {
    _dVector.push_back(*it);
  }
  std::cout << "DESCRIPTIONS SET:" << _dVector.size() << std::endl;
}

//----------------------------------
void XMLCommissioningAnalysis::uploadAnalysis( std::string outputFileName ) throw (FecExceptionHandler) {
  if ( _dVector.size() ) {
    MemBufOutputSource memBufOS( _dVector, _currentCommissioningType, false );
    XMLCommonFec::writeXMLFile( memBufOS.getOutputBuffer()->str(), outputFileName );
  } else {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }
}



//----------------------------------
uint32_t XMLCommissioningAnalysis::uploadAnalysis( bool updateCurrentState ) throw ( FecExceptionHandler ) {
#ifndef DATABASE
  return 0;
#else
  MemBufOutputSource memBufOS( _dVector, _currentCommissioningType, true );
  uint32_t partitionStateVersionID = 0;
  try {
    if ( dataBaseAccess_ ) {
      std::string xmlBuffer = (memBufOS.getOutputBuffer())->str();
#ifdef DISPLAYBUFFER
      std::cout << xmlBuffer << std::endl;
#endif
      ( (DbCommissioningAnalysisAccess *)dataBaseAccess_ )->setXMLClob( xmlBuffer, updateCurrentState, partitionStateVersionID );
    } else {
      RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
    }
  } catch (oracle::occi::SQLException &e) {
    RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG + ": unable to upload analysis (OCCI exception)", e), ERRORCODE) ;
  }
  return partitionStateVersionID;
#endif
}


//----------------------------------
std::vector<CommissioningAnalysisDescription*> XMLCommissioningAnalysis::getCalibrationData( uint32_t runNumber,
											     std::string partitionName, CommissioningAnalysisDescription::commissioningType type ) throw ( FecExceptionHandler ) {
#ifndef DATABASE
  {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, "Database support not compiled", FATALERRORCODE) ;
  }
#else
  if ( dataBaseAccess_ ) {
    oracle::occi::Clob *xmlClob;
    std::string xmlBufferId = "theXMLAnalysisDescriptionBuffer";
    try {

      if ( ( xmlClob = ((DbCommissioningAnalysisAccess *)dataBaseAccess_)->getCalibrationData(runNumber, partitionName, type) ) ) {

#ifdef DISPLAYBUFFER
	display(xmlClob) ;
#endif

	XERCES_CPP_NAMESPACE::InputSource *xmlInputSource = createInputSource(xmlClob, xmlBufferId);
	XERCES_CPP_NAMESPACE::DOMInputSource *domInputSource = new XERCES_CPP_NAMESPACE::Wrapper4InputSource((XERCES_CPP_NAMESPACE::InputSource*)xmlInputSource) ;
	domDocument_ = parser_->parse(*domInputSource) ;
	delete domInputSource ;

      } else {
	domDocument_ = NULL ;
      }

    } catch (oracle::occi::SQLException &e) {
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG + ": unable to get the calibration data (OCCI exception)", e), ERRORCODE) ;
    }
    catch (const XERCES_CPP_NAMESPACE::SAXException &ex) {
      RAISEFECEXCEPTIONHANDLER ( XML_SAXEXCEPTION, StrX(ex.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (const XERCES_CPP_NAMESPACE::XMLException& toCatch) {
      RAISEFECEXCEPTIONHANDLER ( XML_XMLEXCEPTION, StrX(toCatch.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (const XERCES_CPP_NAMESPACE::DOMException& toCatch) {
      const unsigned int maxChars = 2047;
      XMLCh errText[maxChars + 1];
      if (XERCES_CPP_NAMESPACE::DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
	RAISEFECEXCEPTIONHANDLER ( XML_DOMEXCEPTION, StrX(errText).getString(), ERRORCODE ) ;
      else
	RAISEFECEXCEPTIONHANDLER ( XML_DOMEXCEPTION, StrX(toCatch.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (...) {
      RAISEFECEXCEPTIONHANDLER(CODECONSISTENCYERROR, XML_PARSINGERROR_MSG + ": unknown exception", ERRORCODE) ;
    }

  } else {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }

  parseXMLBuffer();
  return _dVector;
#endif
}


//----------------------------------
HashMapRunVersion XMLCommissioningAnalysis::getAnalysisHistory( std::string partitionName, CommissioningAnalysisDescription::commissioningType type ) throw ( FecExceptionHandler ) {
#ifndef DATABASE
  {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, "Database support not compiled", FATALERRORCODE) ;
  }
#else
  if ( dataBaseAccess_ ) {
    oracle::occi::Clob *xmlClob;
    std::string xmlBufferId = "theXMLAnalysisRunVersionBuffer";
    try {

      if ( ( xmlClob = ((DbCommissioningAnalysisAccess *)dataBaseAccess_)->getAnalysisHistory(partitionName, type) ) ) {

#ifdef DISPLAYBUFFER
	display(xmlClob) ;
#endif

	XERCES_CPP_NAMESPACE::InputSource *xmlInputSource = createInputSource(xmlClob, xmlBufferId);
	XERCES_CPP_NAMESPACE::DOMInputSource *domInputSource = new XERCES_CPP_NAMESPACE::Wrapper4InputSource((XERCES_CPP_NAMESPACE::InputSource*)xmlInputSource) ;
	domDocument_ = parser_->parse(*domInputSource) ;
	delete domInputSource ;

      } else {
	domDocument_ = NULL ;
      }

    } catch (oracle::occi::SQLException &e) {
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG + ": unable to get analysis (OCCI exception) for partition " + partitionName, e), ERRORCODE) ;
    }
    catch (const XERCES_CPP_NAMESPACE::SAXException &ex) {
      RAISEFECEXCEPTIONHANDLER ( XML_SAXEXCEPTION, StrX(ex.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (const XERCES_CPP_NAMESPACE::XMLException& toCatch) {
      RAISEFECEXCEPTIONHANDLER ( XML_XMLEXCEPTION, StrX(toCatch.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (const XERCES_CPP_NAMESPACE::DOMException& toCatch) {
      const unsigned int maxChars = 2047;
      XMLCh errText[maxChars + 1];
      if (XERCES_CPP_NAMESPACE::DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
	RAISEFECEXCEPTIONHANDLER ( XML_DOMEXCEPTION, StrX(errText).getString(), ERRORCODE ) ;
      else
	RAISEFECEXCEPTIONHANDLER ( XML_DOMEXCEPTION, StrX(toCatch.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (...) {
      RAISEFECEXCEPTIONHANDLER(CODECONSISTENCYERROR, XML_PARSINGERROR_MSG + ": unknown exception", ERRORCODE) ;
    }

  } else {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }

  parseXMLBuffer();

  return _pairOfRunVersion;
#endif
}

//----------------------------------
std::vector<CommissioningAnalysisDescription*> XMLCommissioningAnalysis::getAnalysisHistory( std::string partitionName,
											     uint32_t versionMajorID, uint32_t versionMinorID, CommissioningAnalysisDescription::commissioningType type ) throw ( FecExceptionHandler ) {
#ifndef DATABASE
  {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, "Database support not compiled", FATALERRORCODE) ;	
  }
#else
  if ( dataBaseAccess_ ) {
    oracle::occi::Clob *xmlClob;
    std::string xmlBufferId = "theXMLAnalysisHistoryBuffer";
    try {

      if ( ( xmlClob = ((DbCommissioningAnalysisAccess *)dataBaseAccess_)->getAnalysisHistory(partitionName, versionMajorID, versionMinorID, type) ) ) {

#ifdef DISPLAYBUFFER
	display(xmlClob) ;
#endif

	XERCES_CPP_NAMESPACE::InputSource *xmlInputSource = createInputSource(xmlClob, xmlBufferId);
	XERCES_CPP_NAMESPACE::DOMInputSource *domInputSource = new XERCES_CPP_NAMESPACE::Wrapper4InputSource((XERCES_CPP_NAMESPACE::InputSource*)xmlInputSource) ;
	domDocument_ = parser_->parse(*domInputSource) ;
	delete domInputSource ;

      } else {
	domDocument_ = NULL ;
      }

    } catch (oracle::occi::SQLException &e) {
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG + ": unable to get analysis (OCCI exception) for partition " + partitionName, e), ERRORCODE) ;
    }
    catch (const XERCES_CPP_NAMESPACE::SAXException &ex) {
      RAISEFECEXCEPTIONHANDLER ( XML_SAXEXCEPTION, StrX(ex.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (const XERCES_CPP_NAMESPACE::XMLException& toCatch) {
      RAISEFECEXCEPTIONHANDLER ( XML_XMLEXCEPTION, StrX(toCatch.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (const XERCES_CPP_NAMESPACE::DOMException& toCatch) {
      const unsigned int maxChars = 2047;
      XMLCh errText[maxChars + 1];
      if (XERCES_CPP_NAMESPACE::DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
	RAISEFECEXCEPTIONHANDLER ( XML_DOMEXCEPTION, StrX(errText).getString(), ERRORCODE ) ;
      else
	RAISEFECEXCEPTIONHANDLER ( XML_DOMEXCEPTION, StrX(toCatch.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (...) {
      RAISEFECEXCEPTIONHANDLER(CODECONSISTENCYERROR, XML_PARSINGERROR_MSG + ": unknown exception", ERRORCODE) ;
    }

  } else {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }

  parseXMLBuffer();
  return _dVector;
#endif
}


//----------------------------------
HashMapAnalysisVersions XMLCommissioningAnalysis::getLocalAnalysisVersions( uint32_t globalAnalysisVersion ) throw ( FecExceptionHandler ) {
#ifndef DATABASE
  {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, "Database support not compiled", FATALERRORCODE) ;
  }
#else
  if ( dataBaseAccess_ ) {
    oracle::occi::Clob *xmlClob;
    std::string xmlBufferId = "theXMLLocalAnalysisVersions";
    try {

      if ( ( xmlClob = ((DbCommissioningAnalysisAccess *)dataBaseAccess_)->getLocalAnalysisVersions(globalAnalysisVersion) ) ) {

#ifdef DISPLAYBUFFER
	display(xmlClob) ;
#endif

	XERCES_CPP_NAMESPACE::InputSource *xmlInputSource = createInputSource(xmlClob, xmlBufferId);
	XERCES_CPP_NAMESPACE::DOMInputSource *domInputSource = new XERCES_CPP_NAMESPACE::Wrapper4InputSource((XERCES_CPP_NAMESPACE::InputSource*)xmlInputSource) ;
	domDocument_ = parser_->parse(*domInputSource) ;
	delete domInputSource ;

      } else {
	domDocument_ = NULL ;
      }

    } catch (oracle::occi::SQLException &e) {
      RAISEFECEXCEPTIONHANDLER (DB_PLSQLEXCEPTIONRAISED, what(DB_PLSQLEXCEPTIONRAISED_MSG + ": unable to get local analysis (OCCI exception)", e), ERRORCODE) ;
    }
    catch (const XERCES_CPP_NAMESPACE::SAXException &ex) {
      RAISEFECEXCEPTIONHANDLER ( XML_SAXEXCEPTION, StrX(ex.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (const XERCES_CPP_NAMESPACE::XMLException& toCatch) {
      RAISEFECEXCEPTIONHANDLER ( XML_XMLEXCEPTION, StrX(toCatch.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (const XERCES_CPP_NAMESPACE::DOMException& toCatch) {
      const unsigned int maxChars = 2047;
      XMLCh errText[maxChars + 1];
      if (XERCES_CPP_NAMESPACE::DOMImplementation::loadDOMExceptionMsg(toCatch.code, errText, maxChars))
	RAISEFECEXCEPTIONHANDLER ( XML_DOMEXCEPTION, StrX(errText).getString(), ERRORCODE ) ;
      else
	RAISEFECEXCEPTIONHANDLER ( XML_DOMEXCEPTION, StrX(toCatch.getMessage()).getString(), ERRORCODE ) ;
    }
    catch (...) {
      RAISEFECEXCEPTIONHANDLER(CODECONSISTENCYERROR, XML_PARSINGERROR_MSG + ": unknown exception", ERRORCODE) ;
    }

  } else {
    RAISEFECEXCEPTIONHANDLER (DB_NOTCONNECTED, DB_NOTCONNECTED_MSG, FATALERRORCODE) ;
  }

  parseXMLBuffer();

  return _analysisLocalVersions;
#endif
}

