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
  
  Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
  //#define DEBUGMSGERROR

#ifndef TKSTATE_H
#define TKSTATE_H

#include <string>

#include "keyType.h"
#include "tscTypes.h"

#include "TkStateDefinition.h"


/**
 * \class TkState
 * Interface for states
 * 
 *
 *
 */
class TkState {

 private:

  /** State name
   */
  std::string stateName_; 

  /** Partition name
   */
  std::string partitionName_; 

  /** FEC version major ID
   */
  unsigned int fecVersionMajorId_; 

  /** FEC version minor ID
   */
  unsigned int fecVersionMinorId_; 

  /** FED version major ID
   */
  unsigned int fedVersionMajorId_;

  /** FED version minor ID
   */
  unsigned int fedVersionMinorId_;

  /** Connection version major ID
   */
  unsigned int connectionVersionMajorId_;

  /** Connection version minor ID
   */
  unsigned int connectionVersionMinorId_;

  /** DCU info version major ID
   */
  unsigned int dcuInfoVersionMajorId_;

  /** DCU info version minor ID
   */
  unsigned int dcuInfoVersionMinorId_;

  /** DCU/PSU map version major ID
   */
  unsigned int dcuPsuMapVersionMajorId_;

  /** DCU/PSU map version minor ID
   */
  unsigned int dcuPsuMapVersionMinorId_;

  /** Analysis version map entry pointer
   */
  unsigned int analysisVersionMapPointerId_;

  /** Mask version major ID
   */
  unsigned int maskVersionMajorId_;

  /** Mask version minor ID
   */
  unsigned int maskVersionMinorId_;

  public:
  
  /** Constructor
   */
  TkState ( std::string stateName, std::string partitionName, unsigned int fecVersionMajorId, unsigned int fecVersionMinorId, 
	    unsigned int fedVersionMajorId, unsigned int fedVersionMinorId, 
	    unsigned int connectionVersionMajorId, unsigned int connectionVersionMinorId, 
	    unsigned int dcuInfoVersionMajorId, unsigned int dcuInfoVersionMinorId, 
	    unsigned int dcuPsuMapVersionMajorId, unsigned int dcuPsuMapVersionMinorId,
	    unsigned int analysisVersionMapPointerId,
	    unsigned int maskVersionMajorId, unsigned int maskVersionMinorId):
    stateName_(stateName),
    partitionName_(partitionName),
    fecVersionMajorId_(fecVersionMajorId),
    fecVersionMinorId_(fecVersionMinorId),
    fedVersionMajorId_(fedVersionMajorId),
    fedVersionMinorId_(fedVersionMinorId),
    connectionVersionMajorId_(connectionVersionMajorId),
    connectionVersionMinorId_(connectionVersionMinorId),
    dcuInfoVersionMajorId_(dcuInfoVersionMajorId),
    dcuInfoVersionMinorId_(dcuInfoVersionMinorId),
    dcuPsuMapVersionMajorId_(dcuPsuMapVersionMajorId),
    dcuPsuMapVersionMinorId_(dcuPsuMapVersionMinorId),
    analysisVersionMapPointerId_(analysisVersionMapPointerId),
    maskVersionMajorId_(maskVersionMajorId),
    maskVersionMinorId_(maskVersionMinorId)
    {
    }

  /** Constructor with the parameter definition extracted from the database or XML file
   */
  TkState ( parameterDescriptionNameType parameterNames ):
    stateName_(""),
    partitionName_(""),
    fecVersionMajorId_(0),
    fecVersionMinorId_(0),
    fedVersionMajorId_(0),
    fedVersionMinorId_(0),
    connectionVersionMajorId_(0),
    connectionVersionMinorId_(0),
    dcuInfoVersionMajorId_(0),
    dcuInfoVersionMinorId_(0),
    dcuPsuMapVersionMajorId_(0),
    dcuPsuMapVersionMinorId_(0),
    analysisVersionMapPointerId_(0),
    maskVersionMajorId_(0),
    maskVersionMinorId_(0)
    {
      setStateName((std::string)parameterNames[TKSTATEPARAMETERNAMES[STATENAME]]->getValue());
      setPartitionName((std::string)parameterNames[TKSTATEPARAMETERNAMES[PARTITIONNAME]]->getValue());
      setFecVersionMajorId(*((unsigned int*) parameterNames[TKSTATEPARAMETERNAMES[FECVERSIONMAJORID]]->getValueConverted()));
      setFecVersionMinorId(*((unsigned int*)parameterNames[TKSTATEPARAMETERNAMES[FECVERSIONMINORID]]->getValueConverted()));
      setFedVersionMajorId(*((unsigned int*)parameterNames[TKSTATEPARAMETERNAMES[FEDVERSIONMAJORID]]->getValueConverted()));
      setFedVersionMinorId(*((unsigned int*)parameterNames[TKSTATEPARAMETERNAMES[FEDVERSIONMINORID]]->getValueConverted()));
      setConnectionVersionMajorId(*((unsigned int*)parameterNames[TKSTATEPARAMETERNAMES[CONNECTIONVERSIONMAJORID]]->getValueConverted()));
      //std::cout<<"\nvalue2 : "<<*((int*)parameterNames[TKSTATEPARAMETERNAMES[CONNECTIONVERSIONMINORID]]->getValueConverted())<<std::endl;
      setConnectionVersionMinorId(*((unsigned int*)parameterNames[TKSTATEPARAMETERNAMES[CONNECTIONVERSIONMINORID]]->getValueConverted()));
      setDcuInfoVersionMajorId(*((unsigned int*)parameterNames[TKSTATEPARAMETERNAMES[DCUINFOVERSIONMAJORID]]->getValueConverted()));
      setDcuInfoVersionMinorId(*((unsigned int*)parameterNames[TKSTATEPARAMETERNAMES[DCUINFOVERSIONMINORID]]->getValueConverted()));
      setDcuPsuMapVersionMajorId(*((unsigned int*)parameterNames[TKSTATEPARAMETERNAMES[DCUPSUMAPVERSIONMAJORID]]->getValueConverted()));
      setDcuPsuMapVersionMinorId(*((unsigned int*)parameterNames[TKSTATEPARAMETERNAMES[DCUPSUMAPVERSIONMINORID]]->getValueConverted()));
      setAnalysisVersionMapPointerId(*((unsigned int*)parameterNames[TKSTATEPARAMETERNAMES[ANALYSISVERSIONMAPID]]->getValueConverted()));
      //parameterNames[TKSTATEPARAMETERNAMES[ANALYSISVERSIONMAPID]]->setValue("0");
      setMaskVersionMajorId(*((unsigned int*)parameterNames[TKSTATEPARAMETERNAMES[MASKVERSIONMAJORID]]->getValueConverted()));
      setMaskVersionMinorId(*((unsigned int*)parameterNames[TKSTATEPARAMETERNAMES[MASKVERSIONMINORID]]->getValueConverted()));
  }
    
 

  /** Get the state name
   */
  std::string getStateName ( ) {
    return (stateName_) ;
  }

  /**Set the stateName
   */
  void setStateName ( std::string n) {
    stateName_ = n ;
  }

  /** Get the partition name
   */
  std::string getPartitionName ( ) {
    return (partitionName_) ;
  }

  /** Set the partition name
   */
  void setPartitionName ( std::string n) {
    partitionName_ = n ;
  }

  /** Get the FecVersionMajorId
   */
  unsigned int getFecVersionMajorId ( ) {
    return (fecVersionMajorId_) ;
  }

  /** Set the FecVersionMajorId
   */
  void setFecVersionMajorId ( unsigned int v ) {
    fecVersionMajorId_ = v ;
  }

  /** Get the FecVersionMinorId
   */
  unsigned int getFecVersionMinorId ( ) {
    return (fecVersionMinorId_) ;
  }

  /** Set the FecVersionMinorId
   */
  void setFecVersionMinorId ( unsigned int v ) {
    fecVersionMinorId_ = v ;
  }

  /** Get the FedVersionMajorId
   */
  unsigned int getFedVersionMajorId ( ) {
    return (fedVersionMajorId_) ;
  }

  /** Set the FedVersionMajorId
   */
  void setFedVersionMajorId ( unsigned int v ) {
    fedVersionMajorId_ = v ;
  }

  /** Get the FedVersionMinorId
   */
  unsigned int getFedVersionMinorId ( ) {
    return (fedVersionMinorId_) ;
  }

  /** Set the FedVersionMinorId
   */
  void setFedVersionMinorId (unsigned  int v ) {
    fedVersionMinorId_ = v ;
  }

  /** Get the ConnectionVersionMajorId
   */
 unsigned  int getConnectionVersionMajorId ( ) {
    return (connectionVersionMajorId_) ;
  }

  /** Set the ConnectionVersionMajorId
   */
  void setConnectionVersionMajorId ( unsigned int v ) {
    connectionVersionMajorId_ = v ;
  }

  /** Get the ConnectionVersionMinorId
   */
  unsigned int getConnectionVersionMinorId ( ) {
    return (connectionVersionMinorId_) ;
  }

  /** Set the ConnectionVersionMinorId
   */
  void setConnectionVersionMinorId ( unsigned int v ) {
    connectionVersionMinorId_ = v ;
  }

  /** Get the DcuInfoVersionMajorId
   */
  unsigned int getDcuInfoVersionMajorId ( ) {
    return (dcuInfoVersionMajorId_) ;
  }

  /** Set the DcuInfoVersionMajorId
   */
  void setDcuInfoVersionMajorId ( unsigned int v ) {
    dcuInfoVersionMajorId_ = v;
  }

  /** Get the DcuInfoVersionMinorId
   */
  unsigned int getDcuInfoVersionMinorId ( ) {
    return (dcuInfoVersionMinorId_) ;
  }

  /** Set the DcuInfoVersionMinorId
   */
  void setDcuInfoVersionMinorId ( unsigned int v ) {
    dcuInfoVersionMinorId_ = v;
  }

  /** Get the DcuPsuMapVersionMajorId
   */
  unsigned int getDcuPsuMapVersionMajorId ( ) {
    return (dcuPsuMapVersionMajorId_) ;
  }

  /** Set the DcuPsuMapVersionMajorId
   */
  void setDcuPsuMapVersionMajorId ( unsigned int v ) {
    dcuPsuMapVersionMajorId_ = v;
  }

  /** Get the DcuPsuMapVersionMinorId
   */
  unsigned int getDcuPsuMapVersionMinorId ( ) {
    return (dcuPsuMapVersionMinorId_) ;
  }

  /** Set the DcuPsuMapVersionMinorId
   */
  void setDcuPsuMapVersionMinorId ( unsigned int v ) {
    dcuPsuMapVersionMinorId_ = v;
  }

  /** Set the analysis version map entry pointer */
  unsigned int getAnalysisVersionMapPointerId() {
  	return analysisVersionMapPointerId_;
  }

  /** Get the analysis version map entry pointer */
  void setAnalysisVersionMapPointerId( unsigned int value ) {
  	analysisVersionMapPointerId_=value;
  }

  /** Get the maskVersionMajorId
   */
  unsigned int getMaskVersionMajorId ( ) {
    return (maskVersionMajorId_) ;
  }

  /** Set the maskVersionMajorId
   */
  void setMaskVersionMajorId ( unsigned int v ) {
    maskVersionMajorId_ = v;
  }

  /** Get the maskVersionMinorId
   */
  unsigned int getMaskVersionMinorId ( ) {
    return (maskVersionMinorId_) ;
  }

  /** Set the maskVersionMinorId
   */
  void setMaskVersionMinorId ( unsigned int v ) {
    maskVersionMinorId_ = v;
  }


  /** brief Clone TK DCU info description
   * \return the device description cloned
   */
  TkState *clone (){
    
    TkState *state = new TkState(stateName_, partitionName_, fecVersionMajorId_, fecVersionMinorId_, fedVersionMajorId_, fedVersionMinorId_, connectionVersionMajorId_, connectionVersionMinorId_, dcuInfoVersionMajorId_, dcuInfoVersionMinorId_, dcuPsuMapVersionMajorId_, dcuPsuMapVersionMinorId_, analysisVersionMapPointerId_, maskVersionMajorId_, maskVersionMinorId_) ;

    return (state) ;
  }

  /** \brief Display the Dcu Info
   */
  void display ( ) {

    std::cout<< "State Name : "<<stateName_<< std::endl ;
    std::cout<< "Partition Name : "<<partitionName_<< std::endl ;
    std::cout<< "Fec major version : "<<fecVersionMajorId_<< std::endl ;
    std::cout<< "Fec minor version : "<<fecVersionMinorId_<< std::endl ;
    std::cout<< "Fed major version : "<<fedVersionMajorId_<< std::endl ;
    std::cout<< "Fed minor version : "<<fedVersionMinorId_<< std::endl ;
    std::cout<< "Connection major version : "<<connectionVersionMajorId_<< std::endl ;
    std::cout<< "Connection minor version : "<<connectionVersionMinorId_<< std::endl ;
    std::cout<< "DCU info major version : "<<dcuInfoVersionMajorId_<< std::endl ;
    std::cout<< "DCU info minor version : "<<dcuInfoVersionMinorId_<< std::endl ;
    std::cout<< "DCU PSU map major version : "<<dcuPsuMapVersionMajorId_<< std::endl ;
    std::cout<< "DCU PSU map minor version : "<<dcuPsuMapVersionMinorId_<< std::endl ;
    std::cout<< "Analysis version map entry Pointer: "<<analysisVersionMapPointerId_<< std::endl;
    std::cout<< "Mask major version : "<<maskVersionMajorId_<< std::endl ;
    std::cout<< "Mask minor version : "<<maskVersionMinorId_<< std::endl ;

  }

  /** \brief sort by partition name
   * \param r1 - state description
   * \param r2 - state description
   */
  static bool sortByPartitionName ( TkState *r1, TkState *r2 ) {

    if (r1->getPartitionName() < r2->getPartitionName()) return true ;
    else return false ;
  }

  /** \brief Return a list of parameter name
   * must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) {

    parameterDescriptionNameType *parameterNames = new parameterDescriptionNameType ;
    
    (*parameterNames)[TKSTATEPARAMETERNAMES[STATENAME]] = new ParameterDescription(TKSTATEPARAMETERNAMES[STATENAME], ParameterDescription::STRING) ;
    (*parameterNames)[TKSTATEPARAMETERNAMES[PARTITIONNAME]] = new ParameterDescription(TKSTATEPARAMETERNAMES[PARTITIONNAME], ParameterDescription::STRING) ;
    (*parameterNames)[TKSTATEPARAMETERNAMES[FECVERSIONMAJORID]] = new ParameterDescription(TKSTATEPARAMETERNAMES[FECVERSIONMAJORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKSTATEPARAMETERNAMES[FECVERSIONMINORID]] = new ParameterDescription(TKSTATEPARAMETERNAMES[FECVERSIONMINORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKSTATEPARAMETERNAMES[FEDVERSIONMAJORID]] = new ParameterDescription(TKSTATEPARAMETERNAMES[FEDVERSIONMAJORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKSTATEPARAMETERNAMES[FEDVERSIONMINORID]] = new ParameterDescription(TKSTATEPARAMETERNAMES[FEDVERSIONMINORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKSTATEPARAMETERNAMES[CONNECTIONVERSIONMAJORID]] = new ParameterDescription(TKSTATEPARAMETERNAMES[CONNECTIONVERSIONMAJORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKSTATEPARAMETERNAMES[CONNECTIONVERSIONMINORID]] = new ParameterDescription(TKSTATEPARAMETERNAMES[CONNECTIONVERSIONMINORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKSTATEPARAMETERNAMES[DCUINFOVERSIONMAJORID]] = new ParameterDescription(TKSTATEPARAMETERNAMES[DCUINFOVERSIONMAJORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKSTATEPARAMETERNAMES[DCUINFOVERSIONMINORID]] = new ParameterDescription(TKSTATEPARAMETERNAMES[DCUINFOVERSIONMINORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKSTATEPARAMETERNAMES[DCUPSUMAPVERSIONMAJORID]] = new ParameterDescription(TKSTATEPARAMETERNAMES[DCUPSUMAPVERSIONMAJORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKSTATEPARAMETERNAMES[DCUPSUMAPVERSIONMINORID]] = new ParameterDescription(TKSTATEPARAMETERNAMES[DCUPSUMAPVERSIONMINORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKSTATEPARAMETERNAMES[ANALYSISVERSIONMAPID]] = new ParameterDescription(TKSTATEPARAMETERNAMES[ANALYSISVERSIONMAPID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKSTATEPARAMETERNAMES[MASKVERSIONMAJORID]] = new ParameterDescription(TKSTATEPARAMETERNAMES[MASKVERSIONMAJORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKSTATEPARAMETERNAMES[MASKVERSIONMINORID]] = new ParameterDescription(TKSTATEPARAMETERNAMES[MASKVERSIONMINORID], ParameterDescription::INTEGER32) ;

    // Set default values in case some informations are missing in the XML
    (*parameterNames)[TKSTATEPARAMETERNAMES[STATENAME]]->setValue("NONE");
    (*parameterNames)[TKSTATEPARAMETERNAMES[PARTITIONNAME]]->setValue("NONE");
    (*parameterNames)[TKSTATEPARAMETERNAMES[FECVERSIONMAJORID]]->setValue("0");
    (*parameterNames)[TKSTATEPARAMETERNAMES[FECVERSIONMINORID]]->setValue("0");
    (*parameterNames)[TKSTATEPARAMETERNAMES[FEDVERSIONMAJORID]]->setValue("0");
    (*parameterNames)[TKSTATEPARAMETERNAMES[FEDVERSIONMINORID]]->setValue("0");
    (*parameterNames)[TKSTATEPARAMETERNAMES[CONNECTIONVERSIONMAJORID]]->setValue("0");
    (*parameterNames)[TKSTATEPARAMETERNAMES[CONNECTIONVERSIONMINORID]]->setValue("0");
    (*parameterNames)[TKSTATEPARAMETERNAMES[DCUINFOVERSIONMAJORID]]->setValue("0");
    (*parameterNames)[TKSTATEPARAMETERNAMES[DCUINFOVERSIONMINORID]]->setValue("0");
    (*parameterNames)[TKSTATEPARAMETERNAMES[DCUPSUMAPVERSIONMAJORID]]->setValue("0");
    (*parameterNames)[TKSTATEPARAMETERNAMES[DCUPSUMAPVERSIONMINORID]]->setValue("0");
    (*parameterNames)[TKSTATEPARAMETERNAMES[ANALYSISVERSIONMAPID]]->setValue("0");
    (*parameterNames)[TKSTATEPARAMETERNAMES[MASKVERSIONMAJORID]]->setValue("0");
    (*parameterNames)[TKSTATEPARAMETERNAMES[MASKVERSIONMINORID]]->setValue("0");
    
    return parameterNames ;
  }

  /** \brief Delete a list of parameter name and its content
   * \warning the parameter parameterNames is deleted by this method
   */
  static void deleteParameterNames(parameterDescriptionNameType *parameterNames) {
    deviceDescription::deleteParameterNames ( parameterNames ) ;
  }

} ;

#endif
