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

#ifndef TKRUN_H
#define TKRUN_H

#include <string>

#include "keyType.h"
#include "tscTypes.h"

#include "TkRunDefinition.h"


/**
 * \class TkRun
 * Interface for runs
 * 
 */
class TkRun {

 private:

  unsigned int runNumber_;
  std::string stateName_; 
  std::string partitionName_; 
  std::string  mode_;
  std::string startTime_;
  std::string endTime_;
  unsigned int fecVersionMajorId_; 
  unsigned int fecVersionMinorId_; 
  unsigned int fedVersionMajorId_;
  unsigned int fedVersionMinorId_;
  unsigned int connectionVersionMajorId_;
  unsigned int connectionVersionMinorId_;
  unsigned int dcuInfoVersionMajorId_;
  unsigned int dcuInfoVersionMinorId_;
  unsigned int dcuPsuMapVersionMajorId_;
  unsigned int dcuPsuMapVersionMinorId_;
  unsigned int o2o_;
  unsigned int local_;
  std::string  comments_;
  unsigned int analysisVersionMapPointerId_;
  unsigned int maskVersionMajorId_;
  unsigned int maskVersionMinorId_;

  public:
  
  /** Constructor
   */
  TkRun ( unsigned int runNumber, std::string stateName, std::string partitionName, std::string mode, std::string startTime, std::string endTime, unsigned int fecVersionMajorId, unsigned int fecVersionMinorId, unsigned int fedVersionMajorId, unsigned int fedVersionMinorId, unsigned int connectionVersionMajorId, unsigned int connectionVersionMinorId, unsigned int dcuInfoVersionMajorId, unsigned int dcuInfoVersionMinorId, unsigned int dcuPsuMapVersionMajorId, unsigned int dcuPsuMapVersionMinorId, unsigned int o2o, unsigned int local, std::string  comments, unsigned int analysisVersionMapPointerId, unsigned maskVersionMajorId, unsigned int maskVersionMinorId ):
    runNumber_(runNumber),
    stateName_(stateName),
    partitionName_(partitionName),
    mode_(mode),
    startTime_(startTime),
    endTime_(endTime),
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
    o2o_(o2o),
    local_(local),
    comments_(comments),
    analysisVersionMapPointerId_(analysisVersionMapPointerId),
    maskVersionMajorId_(maskVersionMajorId),
    maskVersionMinorId_(maskVersionMinorId)
    {
    }

  /** Constructor with the parameter definition extracted from the database or XML file
   */
  TkRun ( parameterDescriptionNameType parameterNames ):
    runNumber_(0),
    stateName_(""),
    partitionName_(""),
    mode_(""),
    startTime_(""),
    endTime_(""),
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
    o2o_(0),
    local_(0),
    comments_(""),
    analysisVersionMapPointerId_(0),
    maskVersionMajorId_(0),
    maskVersionMinorId_(0)
    {
      setRunNumber(*((unsigned int*) parameterNames[TKRUNPARAMETERNAMES[RUNNUMBER]]->getValueConverted()));
      setStateName((std::string)parameterNames[TKRUNPARAMETERNAMES[RUNSTATENAME]]->getValue());
      setPartitionName((std::string)parameterNames[TKRUNPARAMETERNAMES[RUNPARTITIONNAME]]->getValue());
      setMode((std::string)parameterNames[TKRUNPARAMETERNAMES[RUNMODE]]->getValue());
      setStartingTime((std::string)parameterNames[TKRUNPARAMETERNAMES[RUNSTARTINGTIME]]->getValue());
      setEndingTime((std::string)parameterNames[TKRUNPARAMETERNAMES[RUNENDINGTIME]]->getValue());
      setFecVersionMajorId(*((unsigned int*)parameterNames[TKRUNPARAMETERNAMES[RUNFECVERSIONMAJORID]]->getValueConverted()));
      setFecVersionMinorId(*((unsigned int*)parameterNames[TKRUNPARAMETERNAMES[RUNFECVERSIONMINORID]]->getValueConverted()));
      setFedVersionMajorId(*((unsigned int*)parameterNames[TKRUNPARAMETERNAMES[RUNFEDVERSIONMAJORID]]->getValueConverted()));
      setFedVersionMinorId(*((unsigned int*)parameterNames[TKRUNPARAMETERNAMES[RUNFEDVERSIONMINORID]]->getValueConverted()));
      setConnectionVersionMajorId(*((unsigned int*)parameterNames[TKRUNPARAMETERNAMES[RUNCONNECTIONVERSIONMAJORID]]->getValueConverted()));
      setConnectionVersionMinorId(*((unsigned int*)parameterNames[TKRUNPARAMETERNAMES[RUNCONNECTIONVERSIONMINORID]]->getValueConverted()));
      setDcuInfoVersionMajorId(*((unsigned int*)parameterNames[TKRUNPARAMETERNAMES[RUNDCUINFOVERSIONMAJORID]]->getValueConverted()));
      setDcuInfoVersionMinorId(*((unsigned int*)parameterNames[TKRUNPARAMETERNAMES[RUNDCUINFOVERSIONMINORID]]->getValueConverted()));
      setDcuPsuMapVersionMajorId(*((unsigned int*)parameterNames[TKRUNPARAMETERNAMES[RUNDCUPSUMAPVERSIONMAJORID]]->getValueConverted()));
      setDcuPsuMapVersionMinorId(*((unsigned int*)parameterNames[TKRUNPARAMETERNAMES[RUNDCUPSUMAPVERSIONMINORID]]->getValueConverted()));
      setO2O(*((unsigned int*)parameterNames[TKRUNPARAMETERNAMES[RUNO2O]]->getValueConverted()));
      setLocal(*((unsigned int*)parameterNames[TKRUNPARAMETERNAMES[RUNLOCAL]]->getValueConverted()));
      setComments((std::string)parameterNames[TKRUNPARAMETERNAMES[RUNCOMMENTS]]->getValue());
      setAnalysisVersionMapPointerId(*((unsigned int*)parameterNames[TKRUNPARAMETERNAMES[RUNANALYSISVERSIONMAPID]]->getValueConverted()));
      setMaskVersionMajorId(*((unsigned int*)parameterNames[TKRUNPARAMETERNAMES[RUNMASKVERSIONMAJORID]]->getValueConverted()));
      setMaskVersionMinorId(*((unsigned int*)parameterNames[TKRUNPARAMETERNAMES[RUNMASKVERSIONMINORID]]->getValueConverted()));
  }

  /** Get the run number
   */
  unsigned int getRunNumber ( ) {
    return (runNumber_) ;
  }

  /** Set the run number
   */
  void setRunNumber ( unsigned int v ) {
    runNumber_ = v ;
  }


  /** Get the run name
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

  /** Get the mode
   */
  std::string getMode ( ) {
    return (mode_) ;
  }

  /** Set the mode
   */
  void setMode ( std::string n) {
    mode_ = n ;
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
  unsigned int getConnectionVersionMajorId ( ) {
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
  void setConnectionVersionMinorId (unsigned  int v ) {
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
    dcuInfoVersionMajorId_ = v ;
  }

  /** Get the DcuInfoVersionMinorId
   */
  unsigned int getDcuInfoVersionMinorId ( ) {
    return (dcuInfoVersionMinorId_) ;
  }

  /** Set the DcuInfoVersionMinorId
   */
  void setDcuInfoVersionMinorId (unsigned  int v ) {
    dcuInfoVersionMinorId_ = v ;
  }

  /** Get the DcuPsuMapVersionMajorId
   */
  unsigned int getDcuPsuMapVersionMajorId ( ) {
    return (dcuPsuMapVersionMajorId_) ;
  }

  /** Set the DcuPsuMapVersionMajorId
   */
  void setDcuPsuMapVersionMajorId ( unsigned int v ) {
    dcuPsuMapVersionMajorId_ = v ;
  }

  /** Get the DcuPsuMapVersionMinorId
   */
  unsigned int getDcuPsuMapVersionMinorId ( ) {
    return (dcuPsuMapVersionMinorId_) ;
  }

  /** Set the DcuPsuMapVersionMinorId
   */
  void setDcuPsuMapVersionMinorId (unsigned  int v ) {
    dcuPsuMapVersionMinorId_ = v ;
  }

  /** Get the o2o value
   */
  int getO2O ( ) {
    return (o2o_) ;
  }

  /** Set the o2o value
   */
  void setO2O ( unsigned int r ) {
    o2o_ = r ;
  }

  /** Get the local value
   */
  int getLocal ( ) {
    return (local_) ;
  }

  /** Set the local value
   */
  void setLocal ( unsigned int l ) {
    local_ = l ;
  }

  /** Get the startingTime value
   */
  std::string getStartingTime ( ) {
    return (startTime_) ;
  }

  /** Set the startingTime value
   */
  void setStartingTime ( std::string r ) {
    startTime_ = r ;
  }

  /** Get the endingTime value
   */
  std::string getEndingTime ( ) {
    return (endTime_) ;
  }

  /** Set the endingTime value
   */
  void setEndingTime ( std::string r ) {
    endTime_ = r ;
  }

   /** Get the comment
   */
  std::string getComments ( ) {
    return (comments_) ;
  }

  /** Set the comment
   */
  void setComments ( std::string c) {
    comments_ = c ;
  }

  /** Set the analysis version map entry pointer */
  unsigned int getAnalysisVersionMapPointerId() {
  	return analysisVersionMapPointerId_;
  }

  /** Get the analysis version map entry pointer */
  void setAnalysisVersionMapPointerId( unsigned int value ) {
  	analysisVersionMapPointerId_=value;
  }

  /** Get the MaskVersionMajorId
   */
  unsigned int getMaskVersionMajorId ( ) {
    return (maskVersionMajorId_) ;
  }

  /** Set the MaskVersionMajorId
   */
  void setMaskVersionMajorId ( unsigned int v ) {
    maskVersionMajorId_ = v ;
  }

  /** Get the MaskVersionMinorId
   */
  unsigned int getMaskVersionMinorId ( ) {
    return (maskVersionMinorId_) ;
  }

  /** Set the MaskVersionMinorId
   */
  void setMaskVersionMinorId (unsigned  int v ) {
    maskVersionMinorId_ = v ;
  }

  /** brief Clone TK DCU info description
   * \return the device description cloned
   */
  TkRun *clone (){
    
    TkRun *run = new TkRun(runNumber_, stateName_, partitionName_, mode_, startTime_, endTime_, fecVersionMajorId_, fecVersionMinorId_, fedVersionMajorId_, fedVersionMinorId_, connectionVersionMajorId_, connectionVersionMinorId_, dcuInfoVersionMajorId_, dcuInfoVersionMinorId_, dcuPsuMapVersionMajorId_, dcuPsuMapVersionMinorId_, o2o_, local_, comments_, analysisVersionMapPointerId_, maskVersionMajorId_, maskVersionMinorId_);

    return (run) ;
  }

  /** \brief Display the Dcu Info
   */
  void display ( ) {

    std::cout<< "Run Number : "<<runNumber_<< std::endl ;
    std::cout<< "State Name : "<<stateName_<< std::endl ;
    std::cout<< "Partition Name : "<<partitionName_<< std::endl ;
    std::cout<< "Mode : "<<mode_<< std::endl ;
    std::cout<< "Starting Time : "<<startTime_<< std::endl ;
    std::cout<< "Ending Time : "<<endTime_<< std::endl ;
    std::cout<< "Fec major version : "<<fecVersionMajorId_<< std::endl ;
    std::cout<< "Fec minor version : "<<fecVersionMinorId_<< std::endl ;
    std::cout<< "Fed major version : "<<fedVersionMajorId_<< std::endl ;
    std::cout<< "Fed minor version : "<<fedVersionMinorId_<< std::endl ;
    std::cout<< "Connection major version : "<<connectionVersionMajorId_<< std::endl ;
    std::cout<< "Connection minor version : "<<connectionVersionMinorId_<< std::endl ;
    std::cout<< "DcuInfo major version : "<<dcuInfoVersionMajorId_<< std::endl ;
    std::cout<< "DcuInfo minor version : "<<dcuInfoVersionMinorId_<< std::endl ;
    std::cout<< "DcuPsuMap major version : "<<dcuPsuMapVersionMajorId_<< std::endl ;
    std::cout<< "DcuPsuMap minor version : "<<dcuPsuMapVersionMinorId_<< std::endl ;
    std::cout<< "Analysis version map entry Pointer: "<<analysisVersionMapPointerId_<< std::endl;
    std::cout<< "Mask major version : "<<maskVersionMajorId_<< std::endl ;
    std::cout<< "Mask minor version : "<<maskVersionMinorId_<< std::endl ;
    std::cout<< "O2O : "<<o2o_<< std::endl ;
    std::cout<< "Local : "<<local_<< std::endl ;
    std::cout<< "Comment : "<<comments_<< std::endl ;

  }

  /** \brief Return a list of parameter name
   * must be delete by the owner of the description
   */
  static parameterDescriptionNameType *getParameterNames ( ) {

    parameterDescriptionNameType *parameterNames = new parameterDescriptionNameType ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNNUMBER]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNNUMBER], ParameterDescription::INTEGER32);
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNSTATENAME]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNSTATENAME], ParameterDescription::STRING) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNPARTITIONNAME]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNPARTITIONNAME], ParameterDescription::STRING) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNMODE]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNMODE], ParameterDescription::STRING) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNSTARTINGTIME]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNSTARTINGTIME], ParameterDescription::STRING) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNENDINGTIME]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNENDINGTIME], ParameterDescription::STRING) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNFECVERSIONMAJORID]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNFECVERSIONMAJORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNFECVERSIONMINORID]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNFECVERSIONMINORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNFEDVERSIONMAJORID]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNFEDVERSIONMAJORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNFEDVERSIONMINORID]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNFEDVERSIONMINORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNCONNECTIONVERSIONMAJORID]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNCONNECTIONVERSIONMAJORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNCONNECTIONVERSIONMINORID]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNCONNECTIONVERSIONMINORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNDCUINFOVERSIONMAJORID]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNDCUINFOVERSIONMAJORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNDCUINFOVERSIONMINORID]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNDCUINFOVERSIONMINORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNDCUPSUMAPVERSIONMAJORID]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNDCUPSUMAPVERSIONMAJORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNDCUPSUMAPVERSIONMINORID]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNDCUPSUMAPVERSIONMINORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNO2O]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNO2O], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNLOCAL]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNLOCAL], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNCOMMENTS]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNCOMMENTS], ParameterDescription::STRING) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNANALYSISVERSIONMAPID]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNANALYSISVERSIONMAPID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNMASKVERSIONMAJORID]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNMASKVERSIONMAJORID], ParameterDescription::INTEGER32) ;
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNMASKVERSIONMINORID]] = new ParameterDescription(TKRUNPARAMETERNAMES[RUNMASKVERSIONMINORID], ParameterDescription::INTEGER32) ;

    // Set default values in case some informations are missing in the XML
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNNUMBER]]->setValue("0");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNSTATENAME]]->setValue("NONE");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNPARTITIONNAME]]->setValue("NONE");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNMODE]]->setValue("NONE");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNSTARTINGTIME]]->setValue("NONE");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNENDINGTIME]]->setValue("NONE");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNFECVERSIONMAJORID]]->setValue("0");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNFECVERSIONMINORID]]->setValue("0");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNFEDVERSIONMAJORID]]->setValue("0");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNFEDVERSIONMINORID]]->setValue("0");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNCONNECTIONVERSIONMAJORID]]->setValue("0");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNCONNECTIONVERSIONMINORID]]->setValue("0");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNDCUINFOVERSIONMAJORID]]->setValue("0");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNDCUINFOVERSIONMINORID]]->setValue("0");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNDCUPSUMAPVERSIONMAJORID]]->setValue("0");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNDCUPSUMAPVERSIONMINORID]]->setValue("0");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNO2O]]->setValue("-1");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNLOCAL]]->setValue("-1");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNCOMMENTS]]->setValue("");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNANALYSISVERSIONMAPID]]->setValue("0");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNMASKVERSIONMAJORID]]->setValue("0");
    (*parameterNames)[TKRUNPARAMETERNAMES[RUNMASKVERSIONMINORID]]->setValue("0");

    return parameterNames ;
  }

  /** \brief Delete a list of parameter name and its content
   * \warning the parameter parameterNames is deleted by this method
   */
  static void deleteParameterNames(parameterDescriptionNameType *parameterNames) {
    deviceDescription::deleteParameterNames ( parameterNames ) ;
  }

  /** \brief compare two runs to sort it
   */
  static bool sortByRunNumber ( TkRun *r1, TkRun *r2 ) {

    if (r1->getRunNumber() < r2->getRunNumber()) return true ;
    else return false ;
  }

  /** \brief compare two runs to sort it
   */
  static bool sortByReverseRunNumber ( TkRun *r1, TkRun *r2 ) {

    if (r1->getRunNumber() > r2->getRunNumber()) return true ;
    else return false ;
  }

  /** \brief retreive the corresponding modeid for a given mode
   * possible modes are:
   * <ul>
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (1, 'PHYSIC') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (2, 'PEDESTAL') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (3, 'CALIBRATION') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (33, 'CALIBRATION_DECO') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (4, 'GAINSCAN') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (5, 'TIMING') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (6, 'LATENCY') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (7, 'DELAY') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (10, 'PHYSIC10') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (11, 'CONNECTION') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (8, 'DELAY_TTC') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (12, 'TIMING_FED') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (13, 'BARE_CONNECTION') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (14, 'VPSPSCAN') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (15, 'SCOPE') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (16, 'FAST_CONNECTION') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (17, 'DELAY_LAYER') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (18, 'PHYSIC_ZERO_SUPPRESSION') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (19, 'CALIBRATIONSCANPEAK') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (20, 'CALIBRATIONSCAN_DECO') ;
   * <li> INSERT INTO ModeType (runMode, modeDescription) VALUES (21, 'VERY_FAST_CONNECTION') ;
   * </ul>
   */
  static unsigned int getModeId ( std::string mode ) {

    unsigned int runMode = 0 ;
    if (mode == "PHYSIC") runMode = 1 ;
    else if (mode == "PEDESTAL") runMode = 2 ;
    else if (mode == "CALIBRATION") runMode = 3 ;
    else if (mode == "CALIBRATION_DECO") runMode = 33 ;
    else if (mode == "GAINSCAN") runMode = 4 ;
    else if (mode == "TIMING") runMode = 5 ;
    else if (mode == "LATENCY") runMode = 6 ;
    else if (mode == "DELAY") runMode = 7 ;
    else if (mode == "PHYSIC10") runMode = 10 ;
    else if (mode == "CONNECTION") runMode = 11 ;
    else if (mode == "DELAY_TTC") runMode = 8 ;
    else if (mode == "TIMING_FED") runMode = 12 ;
    else if (mode == "BARE_CONNECTION") runMode = 13 ;
    else if (mode == "VPSPSCAN") runMode = 14 ;
    else if (mode == "SCOPE") runMode = 15 ;
    else if (mode == "FAST_CONNECTION") runMode = 16 ;
    else if (mode == "DELAY_LAYER") runMode = 17 ;
    else if (mode == "PHYSIC_ZERO_SUPPRESSION") runMode = 18 ;
    else if (mode == "CALIBRATIONSCANPEAK") runMode = 19 ;
    else if (mode == "CALIBRATIONSCAN_DECO") runMode = 20 ;
    else if (mode == "VERY_FAST_CONNECTION") runMode = 21 ;

    return runMode ;
  }
} ;

#endif
