/*
This file is part of Fec Software project.

Fec Software is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Fec Software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Fec Software; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/

#ifndef DCUCCUCALIBRATION_H
#define DCUCCUCALIBRATION_H

#include <math.h>

#include <iostream>
#include <string>

#include "stringConv.h"

  /** Class which contains all the calibration values for the DCUs
   */
class DcuCcuCalibration {

 private:

  /** SUB detector
   */
  std::string subDetector_ ; // TIB, TOB, TEC, TID

  /** CCU ID
   */
  std::string ccuId_ ;

  /** DCU hard id
   */
  unsigned long dcuHardId_ ;

  /** DCU hard id in the revert way
   */
  unsigned long dcuHardRevertId_ ;

  /** DCU hard id in the three original number
   */
  std::string etDcuIdH_ ;
  std::string etDcuIdM_ ;
  std::string etDcuIdL_ ;

  /** StructureId (ie. Petal, ROD, Module)
   */
  std::string structureId_ ;

  // -------------------------------------------------------------------------------------
  /** Inputs: parameter for the TOB CCUMs
   */
  double G0_, D0_, G2_, D2_, G3_, D3_, I0_, R68_, TSOUT_, TSROUT_ ;

  // -------------------------------------------------------------------------------------
  /** Inputs: parameter for the TIB/TEC CCUMs
   */
  double A_MICROVPADC_, B_MICROV_, I20_MICROA_, I10_MICROA_, TS_, TS_REF_, TEMP_DEGC_, TEMP_REF_DEGC_ ;

  // -------------------------------------------------------------------------------------
  /** Constants magic numbers from analysis
   */
  static const double GTS = 8.901 ;

  static const double DELTAT_TOB     = 1.293 ;
  static const double TSROFFSET_TOB  = 2465 ;

  static const double DELTAT_TECTIB  = 2.903 ;
  static const double TSROFFSET_TECTIB  = 2398 ;

  // -------------------------------------------------------------------------------------
  /** Outputs for TOB, TEC, TIB
   */
  /** 
   */
  double tsOffset_ ;
  double tsGain_ ;
  bool tsCal_ ;

  double adcGain0_ ;
  double adcOffset0_ ;
  bool adc0Cal_ ;

  double i20_ ;
  double i10_ ;
  bool iCal_ ;

  double r68_ ;
  bool r68Cal_ ;

  double adcGain2_ ;
  double adcOffset2_ ;
  bool adc2Cal_ ;

  double adcGain3_ ;
  double adcOffset3_ ;
  bool adc3Cal_ ;

 public:

  /** For TOB DCU on CCU
   * 1,2,3 => TOB
   */
  DcuCcuCalibration ( unsigned int subDetector, std::string ccuId, std::string etDcuIdH, std::string etDcuIdM, std::string etDcuIdL, std::string structureId, 
		      double G0, double D0, double G2, double D2, double G3, double D3, double I0, double R68, double TSOUT, double TSROUT )
    throw (std::string) {

    if ( (subDetector == 1) || (subDetector == 2) || (subDetector == 3) ) {

      // ---------------------------
      subDetector_ = "TOB" ;

      // ---------------------------
      structureId_ = structureId ;
      ccuId_ = ccuId ;

      // ----------------------------
      etDcuIdH_ = etDcuIdH ;
      etDcuIdL_ = etDcuIdL ;
      etDcuIdM_ = etDcuIdM ;

      unsigned int chipAddH = fromString<unsigned int>(etDcuIdH) ;
      unsigned int chipAddM = fromString<unsigned int>(etDcuIdM) ;
      unsigned int chipAddL = fromString<unsigned int>(etDcuIdL) ;

      dcuHardRevertId_ = chipAddL | (chipAddM << 8) | (chipAddH << 16) ;
      dcuHardId_ = chipAddH | (chipAddM << 8) | (chipAddL << 16) ;

      // ---------------------------
      G0_ = G0 ;
      D0_ = D0 ;
      G2_ = G2 ;
      D2_ = D2 ;
      G3_ = G3 ;
      D3_ = D3 ;
      I0_ = I0 ;
      R68_ = R68 ;
      TSOUT_ = TSOUT ;
      TSROUT_ = TSROUT ;
    }
    else throw std::string("NOT A TOB DCU ON CCU") ;
  }

  /** For TIB, TEC DCU on CCU
   * 4 => CCU TEC F0
   * 5 => CCU TEC F1
   * 6 => CCU TEC B0
   * 7 => CCU TEC B1
   * 8 => CCU TEC DOM
   */
  DcuCcuCalibration ( unsigned int subDetector, std::string ccuId, std::string etDcuIdH, std::string etDcuIdM, std::string etDcuIdL, std::string structureId, 
		      double A_MICROVPADC, double B_MICROV, double I20_MICROA, double I10_MICROA, double TS,double  TS_REF, double TEMP_DEGC, double TEMP_REF_DEGC ) 
    throw (std::string) {

    if ( (subDetector == 4) || (subDetector == 5) || (subDetector == 6) || (subDetector == 7) || (subDetector == 8)) {

      subDetector_ = "TEC" ;

      // ---------------------------
      structureId_ = structureId ;
      ccuId_ = ccuId ;

      // ----------------------------
      etDcuIdH_ = etDcuIdH ;
      etDcuIdL_ = etDcuIdL ;
      etDcuIdM_ = etDcuIdM ;

      unsigned int chipAddH = fromString<unsigned int>(etDcuIdH) ;
      unsigned int chipAddM = fromString<unsigned int>(etDcuIdM) ;
      unsigned int chipAddL = fromString<unsigned int>(etDcuIdL) ;

      dcuHardId_ = chipAddL | (chipAddM << 8) | (chipAddH << 16) ;
      dcuHardRevertId_ = chipAddH | (chipAddM << 8) | (chipAddL << 16) ;

      // ---------------------------
      A_MICROVPADC_ = A_MICROVPADC ;
      B_MICROV_ = B_MICROV ;
      I20_MICROA_ = I20_MICROA ;
      I10_MICROA_ = I10_MICROA ; 
      TS_ = TS ;
      TS_REF_ = TS_REF ; 
      TEMP_DEGC_ = TEMP_DEGC ; 
      TEMP_REF_DEGC_ = TEMP_REF_DEGC ;
    }
    else throw std::string("NOT A TEC OR TIB DCU ON CCU") ;
  }

  /** Calculate the conversion values
   */
  void evaluateConversionValues ( double ts_error = 10, 
				  double adcGain0Low = 1.9, double adcGain0High = 2.3,
				  double adcOffset0Low = -10, double adcOffset0High = 10,
				  double I20Low = 16, double I20High = 24,
				  double I10Low = 8, double I10High = 12,
				  double r68Low = 65, double r68High = 71,
				  double adcGain2Low = 0.76, double adcGain2High = 0.96,
				  double adcOffset2Low = -10, double adcOffset2High = 10,
				  double adcGain3Low = 1.5, double adcGain3High = 1.7,
				  double adcOffset3Low = -10, double adcOffset3High = 10
				  ) {

    if (subDetector_ == "TOB") {

      // ---
      tsOffset_ = TSOUT_ - TSROUT_ + TSROFFSET_TOB - (GTS * DELTAT_TOB) ;
      tsGain_ = GTS ;
      tsCal_ = true ;

      // ---
      adcGain0_ = G0_ ;
      adcOffset0_ = D0_ ;
      if ( (adcGain0_ <= adcGain0Low) || (adcGain0_ >= adcGain0High) ||
	   (adcOffset0_ <= adcOffset0Low) || (adcOffset0_ >= adcOffset0High) ) adc0Cal_ = false ;
      else adc0Cal_ = true ;

      // ---
      i20_ = I0_ ;
      i10_ = 0 ;   // not defined
      if ( (i20_ <= I20Low) || (i20_ >= I20High) ) iCal_ = false ;
      else iCal_ = true ;

      // ---
      r68_ = R68_ ;
      if ( (r68_ <= r68Low) || (r68_ >= r68High) ) r68Cal_ = false ;
      else r68Cal_ = true ;

      // ---
      adcGain2_ = G2_ ;
      adcOffset2_ = D2_ ;
      if ( (adcGain2_ <= adcGain2Low) || (adcGain2_ >= adcGain2High) ||
	   (adcOffset2_ <= adcOffset2Low) || (adcOffset2_ >= adcOffset2High) ) adc2Cal_ = false ;
      else adc2Cal_ = true ;

      // ---
      adcGain3_ = G3_ ;
      adcOffset3_ = D3_ ;
      if ( (adcGain3_ <= adcGain3Low) || (adcGain3_ >= adcGain3High) ||
	   (adcOffset3_ <= adcOffset3Low) || (adcOffset3_ >= adcOffset3High) ) adc3Cal_ = false ;
      else adc3Cal_ = true ;
    } 
    else { // TIB and TEC

      // ---
      tsOffset_ = TS_ - TS_REF_ + TSROFFSET_TECTIB - (GTS * DELTAT_TECTIB) ;
      tsGain_ = GTS ;
      double ts_ref_expected = GTS * TEMP_REF_DEGC_ + TSROFFSET_TECTIB ;
      double error = TS_REF_ - ts_ref_expected ;
      if (error < 0) error = -error ;
      if (error >= ts_error) tsCal_ = false ;
      else tsCal_ = true ;

      // ---
//      adcGain0_ = 1/A_MICROVPADC_ ; - modified by Guido - May 29th 2006
      adcGain0_ = 1000/A_MICROVPADC_ ;
      adcOffset0_ = 0 ;
      if (adcGain0_ <= adcGain0Low || adcGain0_ >= adcGain0High) adc0Cal_ = false ;
      else adc0Cal_ = true ;

      // ---
      i20_ = I20_MICROA_ ;
      i10_ = I10_MICROA_ ;
      if ( (i20_ <= I20Low) || (i20_ >= I20High) ||
//	   (i10_ <= I20Low) || (i10_ >= I10High) ) iCal_ = false ; - modified by Guido - June 7th 2006
	   (i10_ <= I10Low) || (i10_ >= I10High) ) iCal_ = false ; 
      else iCal_ = true;

      // ---
      r68_ = 0 ;       // not defined
      r68Cal_ = true ;

      // ---
      adcGain2_ = 0 ;   // not defined
      adcOffset2_ = 0 ; // not defined
      adc2Cal_ = true ;

      // ---
      adcGain3_ = 0 ;   // not defined
      adcOffset3_ = 0 ; // not defined
      adc3Cal_ = true ;
    }
  }

  /** set method for each attribut
   */
  void setSubDetector ( std::string subDetector ) {

    subDetector_ = subDetector ;
  }

  /** set method for each attribut
   */
  void setSubDetector ( unsigned int subDetector ) throw (std::string) {

    if ( (subDetector == 1) || (subDetector == 2) || (subDetector == 3) ) subDetector_ = "TOB" ;
    else if ( (subDetector == 4) || (subDetector == 5) || (subDetector == 6) || (subDetector == 7) ) subDetector_ = "TEC" ;
//    else if (subDetector == 8) subDetector_ = "TIB" ; - modified by Guido - May 29th 2006
    else if (subDetector == 8) subDetector_ = "TEC_DOHM" ;
    else throw std::string ("NOT A CORRECT VALUE FOR A SUBDETECTOR") ;
  }

  /** get method for each attribut
   */
  std::string getSubDetector ( ) {

    return (subDetector_) ;
  }

  /** set method for each attribut
   */
  void setCcuId ( std::string ccuId ) {

    ccuId_ = ccuId ;
  }

  /** get method for each attribut
   */
  std::string getCcuId ( ) {

    return (ccuId_) ;
  }

  /** set method for each attribut
   */
  void setStructureId ( std::string structureId ) {

    structureId_ = structureId ;
  }

  /** get method for each attribut
   */
  std::string getStructureId ( ) {

    return (structureId_) ;
  }

  /** set method for each attribut
   */
  void setDcuHardId ( unsigned int dcuHardId ) {

    dcuHardId_ = dcuHardId ;
  }

  /** get method for each attribut
   */
  unsigned getDcuHardId ( ) {

    return (dcuHardId_) ;
  }

  /** set method for each attribut
   */
  void setDcuHardRevertId ( unsigned int dcuHardRevertId ) {

    dcuHardRevertId_ = dcuHardRevertId ;
  }

  /** get method for each attribut
   */
  unsigned int getDcuHardRevertId ( ) {

    return (dcuHardRevertId_) ;
  }


  /** set method for each attribut
   */
  void setTsOffset ( double tsOffset ) {

    tsOffset_ = tsOffset ;
  }

  /** get method for each attribut
   */
  double getTsOffset ( ) {

    return (tsOffset_) ;
  }

  /** set method for each attribut
   */
  void setTsGain ( double tsGain ) {

    tsGain_ = tsGain ;
  }

  /** get method for each attribut
   */
  double getTsGain ( ) {

    return (tsGain_) ;
  }

  /** flag
   */
  bool getTsCal ( ) {

    return (tsCal_) ;
  }

  /** set method for each attribut
   */
  void setAdcOffset0 ( double adcOffset0 ) {

    adcOffset0_ = adcOffset0 ;
  }

  /** set method for each attribut
   */
  double getAdcOffset0 ( ) {

    return (adcOffset0_) ;
  }

  /** set method for each attribut
   */
  void setAdcGain0 ( double adcGain0 ) {

    adcGain0_ = adcGain0 ;
  }

  /** get method for each attribut
   */
  double getAdcGain0 ( ) {

    return (adcGain0_) ;
  }

  /** flag
   */
  bool getAdc0Cal ( ) {

    return (adc0Cal_) ;
  }

  /** set method for each attribut
   */
  void setAdcOffset2 ( double adcOffset2 ) {

    adcOffset2_ = adcOffset2 ;
  }

  /** set method for each attribut
   */
  double getAdcOffset2 ( ) {

    return (adcOffset2_) ;
  }

  /** set method for each attribut
   */
  void setAdcGain2 ( double adcGain2 ) {

    adcGain2_ = adcGain2 ;
  }

  /** get method for each attribut
   */
  double getAdcGain2 ( ) {

    return (adcGain2_) ;
  }

  /** flag
   */
  bool getAdc2Cal ( ) {

    return (adc2Cal_) ;
  }

  /** set method for each attribut
   */
  void setAdcOffset3 ( double adcOffset3 ) {

    adcOffset3_ = adcOffset3 ;
  }

  /** set method for each attribut
   */
  double getAdcOffset3 ( ) {

    return (adcOffset3_) ;
  }

  /** set method for each attribut
   */
  void setAdcGain3 ( double adcGain3 ) {

    adcGain3_ = adcGain3 ;
  }

  /** get method for each attribut
   */
  double getAdcGain3 ( ) {

    return (adcGain3_) ;
  }

  /** flag
   */
  bool getAdc3Cal ( ) {

    return (adc3Cal_) ;
  }

  /** set method for each attribut
   */
  void setI20 ( double i20 ) {

    i20_ = i20 ;
  }

  /** get method for each attribut
   */
  double getI20 ( ) {

    return (i20_) ;
  }
  
  /** set method for each attribut
   */
  void setI10 ( double i10 ) {

    i10_ = i10 ;
  }

  /** get method for each attribut
   */
  double getI10 ( ) {

    return (i10_) ;
  }

  /** flag
   */
  bool getICal ( ) {

    return (iCal_) ;
  }

  /** set method for each attribut
   */
  void setR68 ( double r68 ) {

    r68_ = r68 ;
  }

  /** get method for each attribut
   */
  double getR68 ( ) {

    return (r68_) ;
  }

  /** flag
   */
  bool getR68Cal ( ) {

    return (r68Cal_) ;
  }
} ;


#endif
