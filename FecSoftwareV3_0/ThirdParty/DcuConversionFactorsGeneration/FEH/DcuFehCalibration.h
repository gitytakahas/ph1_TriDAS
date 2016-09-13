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
                       Guido Magazzu - CERN - Geneva, Switzerland
*/

#ifndef DCUCALIBRATION_H
#define DCUCALIBRATION_H

#include <math.h>

#include <iostream>
#include <string>

#include "stringConv.h"

  /** Class which contains all the calibration values for the DCUs
   */
class DcuFehCalibration {

 private:

  /** Hybrid ID
   */
  std::string hybridId_ ;

  /** DCU hard id
   */
  unsigned long dcuHardId_ ;

  /** DCU hard id in the revert way
   */
  unsigned long dcuHardRevertId_ ;

  /** DCU hard id in the three original number
   */
  std::string etDcuId_ ;

  /** Module ID
   */
  std::string moduleId_ ;

  /** StructureId (ie. Petal, ROD, Module)
   */
  std::string structureId_ ;

  /** Sub detector
   */
  std::string subDetector_ ;

  /** Number of thermistored connect to the 20 microA source
   */
  unsigned int thStatus_ ;

  /** Inputs
   */
  double DCU_CAL_HIGHINPUT_MV_[13] ;
  double DCU_CAL_HIGH_VNOM_[13] ;
  double DCU_FHIT_MICROVPADC_[3] ;
  double DCU_0_RAW_[3] ;
  double DCU_1_RAW_[3] ;
  double DCU_2_RAW_[3] ;
  double DCU_3_RAW_[3] ;
  double DCU_4_RAW_[3] ;
  double DCU_7_RAW_[3] ;
  double DCU_6_RAW0_ ;

  double HYBTEMP_ ;
  double DCUCH4_  ;
  double DCUCH7_  ;

  double etV250InMv_ ;
  double fhitVrefInMV_ ;

  /** Thermistor resistance at T0 = 25 C (in ohm)
   */
  static const double RTH0    = 10000  ;
  /** Thermistor temperature coefficient (in K^-1)
   */
  static const double BETA    = 3280   ;
  /** Sensing resistor for the detector leakage current (in ohm)
   */
  static const double RLEAK   = 681    ;
  /** Reference temperature T0 = 25 C (in K) 
   */
  static const double T0      = 298.14 ;
  /** Integrated temperature sensor gain (in ADC count / K)
   */
  static const double GAINTS  = 8.901   ;
  /** Calibration resistance for i20 evaluation (in ohm)
   */
  static const double R0CAL   = 10000  ;
  /** Applied V250 power supply voltage (in mV)
   */
  static const double V250CAL = 2500   ;
  /** Mean value of the I10 current (from calibration of TIB/TEC CCUMs)
   */
  static const double I10K    = 0.009406 ; // 06/04/2006 => FD, GM

  /** Ratio between the mean value of the I20 current and the mean value of the I10 current 
   * (from calibration of TIB/TEC CCUMs)
   */
  static const double KI      = 2.055    ; // 06/04/2006 => FD, GM
  /** Temperature difference between the reference thermistor and the thermistor mounted on the hybrid (in K)
   */
  static const double DELTAT  = 0      ;
  /** Difference between Temperature in K and in C
   */
  static const double DIFFKC  = 273.14 ;

  /** Is the HYBTEMP, DCUH4, DCUH7 are retreived from TESTWITHPA_1_HYB_ 
   */
  bool missingTpa_ ;

  /** Conversion values
   */
  double adc0Gain_     ;
  double adc0Offset_   ;
  bool   adc0Cal_  ;
  double i20_      ; // 20 mA current
  double kdiv_     ; // ratio divider for 2.5 V
  double i10_      ; // 10 mA current
  double offsetTScal_ ;
  double offsetTStpa_ ;
  double inl_      ; // for gain and offset
  int    indexOW_  ; // index of the overflow
  double thybcal_     ; // calibration temperature

  /** Hybrid temperature during the test with pitch adapter
   */
  double thybtpa_     ; 

  /** DCU temperature during the test with FHIT reception
   */
  double tdcucal_     ; 

  /** DCU temperature during the test with pitch adapter
   */
  double tdcutpa_     ; 
  /** Reference temperature during the test with the pitch adapter
   */ 
  double tref_     ;

  /** Tool ID for the pitch adapter
   */
  std::string paToolId_ ;

  /** Tool ID for the FHIT reception 
   */
  std::string frToolId_ ;

  /** Input ID for the pitch adapter test
   */
  std::string paInputId_ ;

  /** Input ID for the FHIT reception 
   */
  std::string frInputId_ ;

  /** Test_ID for the pitch adapter test
   */
  std::string paTestId_ ;

  /** Test_ID for the FHIT reception 
   */
  std::string frTestId_ ;

  /** FHIT_NUMBER
   */
  std::string fhitNumber_ ;

  /** FHIT_FIRMWARE_VERSION
   */
  std::string fhitFirmwareVersion_ ;

  /** FHIT_BOARD_VERSION
   */
  std::string fhitBoardVersion_ ;

  /** Calcalute the linear fit
   * \return the inl
   */
  void linearRegression ( double *x, double *y, double *gain, double *offset, double *den, unsigned int startPoint, unsigned int endPoint, double *inl, double *index ) {

    // http://physinfo.ulb.ac.be/divers_html/examens/corrige_info_juin_99.html
    double sx = 0.0, sy = 0.0, sxy = 0.0, sx2 = 0.0 ;
    for (unsigned int j = startPoint ; j <= endPoint ; j ++) {

      //sx := sx + x[j];
      sx = sx + x[j] ;
      //sy := sy + y[j];
      sy = sy + y[j]   ;
      //sx2 := sx2 + x[j] * x[j];
      sx2 = sx2 + x[j] * y[j] ;
      //sxy := sxy + x[j] * y[j];
      sxy = sxy + x[j] * y[j] ;
    }

    // den := n * sx2 - sx * sx;           {denominateur}
    *den = (endPoint-startPoint+1) * sx2 - sx * sx ;            
    // a := (sy * sx2 - sxy * sx) / den;   {coefficient a}
    *gain = (sy * sx2 - sxy * sx) / *den;
    // b := (n * sxy - sy * sx) / den      {coefficient b}
    *offset = ((endPoint-startPoint+1) * sxy - sy * sx) / *den ;

    // Evaluate the INL
    *inl = 0 ;
    *index = 0 ;

    for (unsigned int j = startPoint ; j <= endPoint ; j ++) {

      double val = *gain * x[j] + *offset ;
      double inli = val - y[j] ;

      if (inli < 0) inli = -inli ;
      if (inli > *inl) {
	*inl = inli ;
	*index = j ;
      }
    }
  }

 public:

  /**
   * param DCU hardware ID
   */
  DcuFehCalibration ( std::string hybridId, unsigned long dcuHardId, unsigned long dcuHardRevertId, std::string etDcuId, std::string moduleId = "0", std::string structureId = "0", std::string subDetector = "Unknown", int thStatus = 2 ): hybridId_(hybridId), dcuHardId_(dcuHardId), dcuHardRevertId_(dcuHardRevertId), etDcuId_(etDcuId), moduleId_(moduleId), structureId_(structureId), subDetector_(subDetector), thStatus_(thStatus)  {

    if (hybridId_ == "") hybridId_ = "0" ;
    if (moduleId_ == "") moduleId_ = "0" ;
    if (structureId_ == "") structureId_ = "0" ;
    if (subDetector_ == "") subDetector_ = "Unknown" ;
  }


  /** Calculate the conversion values
   */
  void evaluateConversionValues ( unsigned int startPoint = 2, unsigned int endPoint = 11, double inlOverflow = 5, 
				  double adcGain0Low = 1.9, double adcGain0High = 2.3, 
				  double adcOffset0Low = -100, double adcOffset0High = 100 ) {

    // ------------------------------------------------
    // Linear fit of => gain/offset
    //    - DCU_CAL_HIGHINPUT_MV
    //    - DCU_CAL_HIGH_VNOM
    // use the samples between 2 and 11 (the other values are out of the linear range
    unsigned int linearBegin = startPoint ;
    unsigned int linearEnd   = endPoint   ;

    // http://physinfo.ulb.ac.be/divers_html/examens/corrige_info_juin_99.html
    double sx = 0.0, sy = 0.0, sxy = 0.0, sx2 = 0.0 ;
    for (unsigned int j = linearBegin ; j <= linearEnd ; j ++) {

      //sx := sx + x[j];
      sx = sx + DCU_CAL_HIGHINPUT_MV_[j] ;
      //sy := sy + y[j];
      sy = sy + DCU_CAL_HIGH_VNOM_[j]   ;
      //sx2 := sx2 + x[j] * x[j];
      sx2 = sx2 + DCU_CAL_HIGHINPUT_MV_[j] * DCU_CAL_HIGHINPUT_MV_[j] ;
      //sxy := sxy + x[j] * y[j];
      sxy = sxy + DCU_CAL_HIGHINPUT_MV_[j] * DCU_CAL_HIGH_VNOM_[j] ;
    }

    // den := n * sx2 - sx * sx;           {denominateur}
    double den = (linearEnd-linearBegin+1) * sx2 - sx * sx ;            
    // a := (sy * sx2 - sxy * sx) / den;   {coefficient a}
    double a = (sy * sx2 - sxy * sx) / den;
    // b := (n * sxy - sy * sx) / den      {coefficient b}
    double b = ((linearEnd-linearBegin+1) * sxy - sy * sx) / den ;

    // Conversion values
    adc0Offset_ = a ;
    adc0Gain_   = b ;

    // Evaluate the INL
    inl_ = 0 ;
    indexOW_ = -1 ;
    for (unsigned int j = linearBegin ; j <= linearEnd ; j ++) {
      
      double val = adc0Gain_ * DCU_CAL_HIGHINPUT_MV_[j] + adc0Offset_ ;
      double inl = val - DCU_CAL_HIGH_VNOM_[j] ;
      if (inl < 0) inl = -inl ;
      if (inl > inl_) {
	inl_ = inl ;
	indexOW_ = j ;
      }
    }

    // Test the inl
    if (inl_ > inlOverflow) {

      std::cout << "The INL (" << inl_ << ") is too important removing the point " << indexOW_ << std::endl ;

      // Remove the point that gave the error and re-evaluate the linear fit
      double dcuCalHighInputMV[13] ;
      double dcuCalHighVnom[13] ;

      // Copy the values except the indexOW_
      int j = startPoint ;
      for (int i = linearBegin ; i <= linearEnd ; i ++) {

	if (i != indexOW_) {
	  dcuCalHighInputMV[j] = DCU_CAL_HIGHINPUT_MV_[i] ;
	  dcuCalHighVnom[j] = DCU_CAL_HIGH_VNOM_[i] ;
	  j ++ ;
	}
      }

      linearEnd -- ;

      // http://physinfo.ulb.ac.be/divers_html/examens/corrige_info_juin_99.html
      double sx = 0.0, sy = 0.0, sxy = 0.0, sx2 = 0.0 ;
      for (unsigned int j = linearBegin ; j <= linearEnd ; j ++) {

	//sx := sx + x[j];
	sx = sx + dcuCalHighInputMV[j] ;
	//sy := sy + y[j];
	sy = sy + dcuCalHighVnom[j]   ;
	//sx2 := sx2 + x[j] * x[j];
	sx2 = sx2 + dcuCalHighInputMV[j] * dcuCalHighInputMV[j] ;
	//sxy := sxy + x[j] * y[j];
	sxy = sxy + dcuCalHighInputMV[j] * dcuCalHighVnom[j] ;
      }

      // den := n * sx2 - sx * sx;           {denominateur}
      double den = (linearEnd-linearBegin+1) * sx2 - sx * sx ;            
      // a := (sy * sx2 - sxy * sx) / den;   {coefficient a}
      double a = (sy * sx2 - sxy * sx) / den;
      // b := (n * sxy - sy * sx) / den      {coefficient b}
      double b = ((linearEnd-linearBegin+1) * sxy - sy * sx) / den ;

      //std::cout << adc0Offset_ << " / " << a << std::endl ;
      //std::cout << adc0Gain_ << " / " << b << std::endl ;

      // Conversion values
      adc0Offset_ = a ;
      adc0Gain_   = b ;

      // Evaluate the INL
      inl_ = 0 ;
      indexOW_ = -1 ;
      for (unsigned int j = linearBegin ; j <= linearEnd ; j ++) {
      
	double val = adc0Gain_ * dcuCalHighInputMV[j] + adc0Offset_ ;
	double inl = val - dcuCalHighVnom[j] ;
	if (inl < 0) inl = -inl ;
	if (inl > inl_) {
	  inl_ = inl ;
	  indexOW_ = j ;
	}
      }

      //std::cout << "New inl = " << inl_ << std::endl ;
    }

#ifdef DEBUGMSGERROR
    std::cout << "Gain " << adc0Gain_ << std::endl ;
    std::cout << "Offset " << adc0Offset_ << std::endl ;
#endif

    if ( (adc0Gain_ <= adcGain0Low) || (adc0Gain_ >= adcGain0High) ||
	 (adc0Offset_ <= adcOffset0Low) || (adc0Offset_ >= adcOffset0High) ) adc0Cal_ = false ;
    else adc0Cal_ = true ;

    adc0Cal_ |= (inl_ > inlOverflow) ;


    // -----------------------------------------------------
    // 20 micro current i20

    i20_ = (DCU_0_RAW_[0] - adc0Offset_) / (adc0Gain_ * R0CAL) ; // in ampere

#ifdef DEBUGMSGERROR
    std::cout << "DCU_0_RAW_[0] = " << DCU_0_RAW_[0] << std::endl ;
    std::cout << "R0CAL = " << R0CAL << std::endl ;
    std::cout << "i20 = " << i20_ << std::endl ;
#endif

    // -----------------------------------------------------
    // kdiv
    kdiv_ = (DCU_1_RAW_[0] - adc0Offset_) / (adc0Gain_ * etV250InMv_) ; //;V250CAL) ;

#ifdef DEBUGMSGERROR
    std::cout << "DCU_1_RAW_[0] = " << DCU_1_RAW_[0] << std::endl ;
    std::cout << "V250CAL = " << V250CAL << std::endl ;
    std::cout << "kdiv = " << kdiv_ << std::endl ;
#endif

    // -----------------------------------------------------
    // i10: first option
    //double ex = exp ( BETA * ((1 / (HYBTEMP_+DIFFKC)) - (1 / T0)) ) ;
    //i10_ = DCUCH4_ / (adc0Gain_ * RTH0 * ex ) ;

    // -----------------------------------------------------
    // i10: second option
    i10_ = i20_ / KI ;

    // -----------------------------------------------------
    // thybcal_
//    double value = log(DCU_4_RAW_[0] / (adc0Gain_ * i10_ * RTH0)) ;
    double value = log(DCU_4_RAW_[0] / (adc0Gain_ * I10K * RTH0)) ;
    thybcal_ = 1/(1/T0 + value/BETA) ;

    // -----------------------------------------------------
    // tref_
    tref_ = HYBTEMP_ ;

    // -----------------------------------------------------
    // thybtpa_
//    value = log(DCUCH4_ / (adc0Gain_ * i10_ * RTH0)) ;
    value = log(DCUCH4_ / (adc0Gain_ * I10K * RTH0)) ;
    thybtpa_ = 1/(1/T0 + value/BETA) ;

    // -----------------------------------------------------
    // offsetTScal
    offsetTScal_ = GAINTS * (T0-thybcal_) + DCU_7_RAW_[0] ; 

    // -----------------------------------------------------
    // offsetTSTpa
    offsetTStpa_ = GAINTS * (T0-thybtpa_) + DCUCH7_ ; 

    // -----------------------------------------------------
    // tdcucal_
    tdcucal_ = (DCU_7_RAW_[0] - offsetTStpa_)/GAINTS ;

    // -----------------------------------------------------
    // tdcutpa_
    tdcutpa_ = (DCUCH7_ - offsetTScal_)/GAINTS ;
  }

  /** set
   */
  void setHybrid ( std::string hybridId ) {
    hybridId_ = hybridId ;
  }

  /** get
   */
  std::string getHybridId ( ) {

    return (hybridId_) ;
  }
  

  /** set
   */
  void setDcuHardId ( unsigned long dcuHardId ) {
    dcuHardId_ = dcuHardId ;
  }

  /** get
   */
  unsigned long getDcuHardId ( ) {

    return (dcuHardId_) ;
  }

  /** set
   */
  void setDcuHardRevertId ( unsigned long dcuHardRevertId ) {
    dcuHardRevertId_ = dcuHardRevertId ;
  }

  /** get
   */
  unsigned long getDcuHardRevertId ( ) {

    return (dcuHardRevertId_) ;
  }
  
  /** set
   */
  void setEtDcuId ( std::string etDcuId ) {
    etDcuId_ = etDcuId ;
  }

  /** get
   */
  std::string getEtDcuId ( ) {

    return (etDcuId_) ;
  }

  /** set
   */
  void setModuleId ( std::string moduleId ) {
    moduleId_ = moduleId ;
  }

  /** get
   */
  std::string getModuleId ( ) {

    return (moduleId_) ;
  }

  /** set
   */
  void setStructureId ( std::string structureId ) {

    structureId_ = structureId ;
  }

  /** set
   */
  void setSubDectector ( std::string subDetector ) {
    
    subDetector_ = subDetector ;
  }

  /** get
   */
  std::string getSubDetector ( ) {

    return subDetector_ ;
  }

  /** get
   */
  std::string getStructureId ( ) {

    return (structureId_) ;
  }

  /** set
   */
  void setDcuCalHighInputMV ( double DCU_CAL_HIGHINPUT_MV[13] ) {

    for (int i = 0 ; i < 13 ; i ++) 
      DCU_CAL_HIGHINPUT_MV_[i] = DCU_CAL_HIGHINPUT_MV[i] ;
  }

  /** get
   */
  void getDcuCalHighInputMV ( double DCU_CAL_HIGHINPUT_MV[13] ) {

    for (int i = 0 ; i < 13 ; i ++) 
      DCU_CAL_HIGHINPUT_MV[i] = DCU_CAL_HIGHINPUT_MV_[i] ;
  }

  /** set
   */
  void setDcuCalHighVNOM ( double DCU_CAL_HIGH_VNOM[13] ) {

    for (int i = 0 ; i < 13 ; i ++) 
      DCU_CAL_HIGH_VNOM_[i] = DCU_CAL_HIGH_VNOM[i] ;
  }

  /** get
   */
  void getDcuCalHighVNOM ( double DCU_CAL_HIGH_VNOM[13] ) {

    for (int i = 0 ; i < 13 ; i ++) 
      DCU_CAL_HIGH_VNOM[i] = DCU_CAL_HIGH_VNOM_[i] ;
  }

  /** set
   */
  void setDcuFhitMicroVPADC ( double DCU_FHIT_MICROVPADC[13] ) {

    for (int i = 0 ; i < 13 ; i ++) 
      DCU_FHIT_MICROVPADC_[i] = DCU_FHIT_MICROVPADC[i] ;
  }

  /** get
   */
  void getDcuFhitMicroVPADC ( double DCU_FHIT_MICROVPADC[13] ) {

    for (int i = 0 ; i < 13 ; i ++) 
      DCU_FHIT_MICROVPADC[i] = DCU_FHIT_MICROVPADC_[i] ;
  }

  /** set
   */
  void setDcu0Raw ( double DCU_0_RAW[3] ) {

    for (int i = 0 ; i < 3 ; i ++) 
      DCU_0_RAW_[i] = DCU_0_RAW[i] ;
  }

  /** get
   */
  void getDcu0Raw ( double DCU_0_RAW[3] ) {

    for (int i = 0 ; i < 3 ; i ++) 
      DCU_0_RAW [i] = DCU_0_RAW_[i] ;
  }

  /** set
   */
  void setDcu1Raw ( double DCU_1_RAW[3] ) {

    for (int i = 0 ; i < 3 ; i ++) 
      DCU_1_RAW_[i] = DCU_1_RAW[i] ;
  }

  /** get
   */
  void getDcu1Raw ( double DCU_1_RAW[3] ) {

    for (int i = 0 ; i < 3 ; i ++) 
      DCU_1_RAW [i] = DCU_1_RAW_[i] ;
  }

  /** set
   */
  void setDcu2Raw ( double DCU_2_RAW[3] ) {

    for (int i = 0 ; i < 3 ; i ++) 
      DCU_2_RAW_[i] = DCU_2_RAW[i] ;
  }

  /** get
   */
  void getDcu2Raw ( double DCU_2_RAW[3] ) {

    for (int i = 0 ; i < 3 ; i ++) 
      DCU_2_RAW [i] = DCU_2_RAW_[i] ;
  }

  /** set
   */
  void setDcu3Raw ( double DCU_3_RAW[3] ) {

    for (int i = 0 ; i < 3 ; i ++) 
      DCU_3_RAW_[i] = DCU_3_RAW[i] ;
  }

  /** get
   */
  void getDcu3Raw ( double DCU_3_RAW[3] ) {

    for (int i = 0 ; i < 3 ; i ++) 
      DCU_3_RAW [i] = DCU_3_RAW_[i] ;
  }

  /** set
   */
  void setDcu4Raw ( double DCU_4_RAW[3] ) {

    for (int i = 0 ; i < 3 ; i ++) 
      DCU_4_RAW_[i] = DCU_4_RAW[i] ;
  }

  /** get
   */
  void getDcu4Raw ( double DCU_4_RAW[3] ) {

    for (int i = 0 ; i < 3 ; i ++) 
      DCU_4_RAW [i] = DCU_4_RAW_[i] ;
  }

  /** set
   */
  void setDcu7Raw ( double DCU_7_RAW[3] ) {

    for (int i = 0 ; i < 3 ; i ++) 
      DCU_7_RAW_[i] = DCU_7_RAW[i] ;
  }

  /** get
   */
  void getDcu7Raw ( double DCU_7_RAW[3] ) {

    for (int i = 0 ; i < 3 ; i ++) 
      DCU_7_RAW [i] = DCU_7_RAW_[i] ;
  }

  /** set
   */
  void setDcu6Raw0 ( double DCU_6_RAW0 ) {

    DCU_6_RAW0_ = DCU_6_RAW0 ;
  }

  /** get
   */
  double getDcu6Raw0 () {

    return DCU_6_RAW0_ ;
  }

  /** set
   */
  void setHybTemp ( double HYBTEMP ) {

    HYBTEMP_ = HYBTEMP ;
  }

  /** get
   */
  double getHybTemp ( ) {

    return HYBTEMP_ ;
  }

  /** set 
   */
  void setDcuCH4 ( double DCUCH4 ) {

    DCUCH4_ = DCUCH4 ;
  }

  /** get
   */
  double getDcuCH4 ( ) {

    return DCUCH4_ ;
  }

  /** set 
   */
  void setDcuCH7 ( double DCUCH7 ) {

    DCUCH7_ = DCUCH7 ;
  }

  /** get
   */
  double getDcuCH7 ( ) {

    return DCUCH7_ ;
  }

  /** set
   */
  void setAdc0Gain ( double adc0Gain ) {

    adc0Gain_ = adc0Gain ;
  }

  /** get 
   */
  double getAdc0Gain ( ) {

    return adc0Gain_ ;
  }

  /** set
   */
  void setAdc0Offset ( double adc0Offset ) {

    adc0Offset_ = adc0Offset ;
  }

  /** get 
   */
  double getAdc0Offset ( ) {

    return adc0Offset_ ;
  }

  /** set
   */
  void setAdc0Cal ( bool adc0Cal ) {

    adc0Cal_ = adc0Cal ;
  }

  /** get
   */
  bool getAdc0Cal ( ) {

    return adc0Cal_ ;
  }

  /** set
   */
  void setI20 ( double i20 ) {

    i20_ = i20 ;
  }

  /** get 
   */
  double getI20 ( ) {

    return i20_ ;
  }

  /** set
   */
  void setKdiv ( double kdiv ) {

    kdiv_ = kdiv ;
  }

  /** get 
   */
  double getKDiv ( ) {

    return kdiv_ ;
  }

  /** set
   */
  void setI10 ( double i10 ) {

    i10_ = i10 ;
  }

  /** get 
   */
  double getI10 ( ) {

    return i10_ ;
  }

  /** set
   */
  void setInl ( double inl ) {

    inl_ = inl ;
  }

  /** get 
   */
  double getInl ( ) {

    return inl_ ;
  }

  /** set
   */
  void setIndexOW ( int indexOW ) {

    indexOW_ = indexOW ;
  }

  /** get 
   */
  int getIndexOW ( ) {

    return indexOW_ ;
  }

  /** set
   */
  void setOffsetTScal ( double offsetTScal ) {

    offsetTScal_ = offsetTScal ;
  }

  /** get 
   */
  double getOffsetTScal ( ) {

    return offsetTScal_ ;
  }

  /** set
   */
  void setOffsetTStpa ( double offsetTStpa ) {

    offsetTStpa_ = offsetTStpa ;
  }

  /** get 
   */
  double getOffsetTStpa ( ) {

    return offsetTStpa_ ;
  }

  /** set
   */
  void setThStatus ( int thStatus ) {

    thStatus_ = thStatus ;
  }

  /** get
   */
  int getThStatus ( ) {

    return thStatus_ ;
  }

  /** set
   */
  void setThybcal ( double thybcal ) {
    thybcal_ = thybcal ;
  }

  /** get
   */
  double getThybcal ( ) {

    return thybcal_ ;
  }

  /** set
   */
  void setTref ( double tref ) {
    tref_ = tref ;
  }

  /** get
   */
  double getTref ( ) {

    return tref_ ;
  }

  /** set
   */
  void setThybTpa ( double thybtpa ) {
    thybtpa_ = thybtpa ;
  }

  /** get
   */
  double getThybTpa ( ) {

    return thybtpa_ ;
  }

  /** set
   */
  void setTdcucal ( double tdcucal ) {
    tdcucal_ = tdcucal ;
  }

  /** get
   */
  double getTdcucal ( ) {

    return tdcucal_ ;
  }

  /** set
   */
  void setTdcutpa ( double tdcutpa ) {
    tdcutpa_ = tdcutpa ;
  }

  /** get
   */
  double getTdcutpa ( ) {

    return tdcutpa_ ;
  }

  /** set
   */
  void setPAToolId ( std::string paToolId ) {

    paToolId_ = paToolId ;
  }

  /** get
   */
  std::string getPATooldId ( ) {

    return paToolId_ ;
  }

  /** set
   */
  void setFRToolId ( std::string frToolId ) {

    frToolId_ = frToolId ;
  }

  /** get
   */
  std::string getFRTooldId ( ) {

    return frToolId_ ;
  }

  /** set
   */
  void setDcu7Raw ( int pos, double value ) {

    DCU_7_RAW_[pos] = value ;
  }

  /** get
   */
  double getDcu7Raw ( int pos ) {

    return DCU_7_RAW_[pos] ;
  }

  /** Retreive the silicon temperature (channel 0 of the DCU)
   */
  double getSiliconTemperature ( unsigned int adcCount ) throw (std::string) {

    double value = 0.0 ;
    if (thStatus_ == 0) {

      std::cerr << "No thermistor connected" << std::endl ;
      throw std::string (  "No thermistor connected" ) ;
    }
    else {

      value = log((thStatus_ * adcCount) / (adc0Gain_ * i20_ * RTH0)) ;
      value = 1/T0 + value / BETA ;
    } 

    return (value) ;
  }

  /** Retreive the voltage V250 (channel 1 of the DCU)
   */
  double getV250 ( unsigned int adcCount ) {

    double value = adcCount / ( adc0Gain_ * kdiv_ ) ;

    return value ;
  }

  /** Retreive the missingTpa so no values from TESTWITHPA_1_HYB_
   */
  bool getMissingTpa ( ) {

    return missingTpa_ ;
  }

  /** Set the missingTpa so no values from TESTWITHPA_1_HYB_
   */
  void setMissingTpa ( bool missingTpa ) {

    missingTpa_ = missingTpa ;
  }

  /** Input ID for the pitch adapter test
   */
  void setPAInputId ( std::string paInputId) {

    paInputId_ = paInputId ;
  }

  /** Input ID for the pitch adapter test
   */
  std::string getPAInputId ( ) {

    return paInputId_ ;
  }

  /** Input ID for the FHIT reception
   */
  void setFRInputId ( std::string frInputId ) {

    frInputId_ = frInputId ;
  }

  /** Input ID for the FHIT reception
   */
  std::string getFRInputId ( ) {

    return frInputId_ ;
  }

  /** Test ID for the pitch adapter test
   */
  void setPATestId ( std::string paTestId ) {

    paTestId_ = paTestId ;
  }

  /** Test ID for the pitch adapter test
   */
  std::string getPATestId ( ) {

    return paTestId_ ;
  }

  /** Test ID for the FHIT reception
   */
  void setFRTestId ( std::string frTestId ) {

    frTestId_ = frTestId ;
  }

  /** Test ID for the FHIT reception
   */
  std::string getFRTestId ( ) {

    return frTestId_ ;
  }

  /** FHIT NUMBER for the FHIT reception
   */
  void setFhitNumber ( std::string fhitNumber ) {

    fhitNumber_ = fhitNumber ;
  }

  /** FHIT NUMBER for the FHIT reception
   */
  std::string getFhitNumber ( ) {

    return fhitNumber_ ;
  }

  /** FHIT_FIRMWARE_VERSION for the FHIT reception
   */
  void setFhitFirmwareVersion ( std::string fhitFirmwareVersion ) {

    fhitFirmwareVersion_ = fhitFirmwareVersion ;
  }

  /** FHIT_FIRMWARE_VERSION ID for the FHIT reception
   */
  std::string getFhitFirmwareVersion ( ) {

    return fhitFirmwareVersion_ ;
  }

  /** FHIT_BOARD_VERSION for the FHIT reception
   */
  void setFhitBoardVersion ( std::string fhitBoardVersion ) {

    fhitBoardVersion_ = fhitBoardVersion ;
  }

  /** FHIT_BOARD_VERSION for the FHIT reception
   */
  std::string getFhitBoardVersion ( ) {

    return fhitBoardVersion_ ;
  }

  /** double etV250InMv_ ;
   */
  void setEtV250InMv (double etV250InMv) {

    etV250InMv_ = etV250InMv ;
  }

  /** get
   */
  double getEtV250InMv ( ) {

    return etV250InMv_ ;
  }

  /** double fhitVrefInMV_ ;
   */
  void setFhitVrefInMV ( double fhitVrefInMV ) {

    fhitVrefInMV_ = fhitVrefInMV ;
  }

  /** get
   */
  double getFhitVrefInMV ( ) {

    return fhitVrefInMV_ ;
  }
} ;


#endif
